
#include <Wire.h>


#define MODE_DISP   0x01
#define MODE_SEL    0x02
#define MODE_EDIT   0x03

#define TEMP_TABLE_SIZE 26

#define COMMAND_SET_SWITCH 1
#define COMMAND_GET_SWITCH 2
#define COMMAND_GET_TEMP   3
#define COMMAND_GET_VOLTS  4



// Define various ADC prescaler
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);


static volatile int cnt = 0x1000;

static byte channelToPins[] = {0x00, 0x08, 0x04, 0x0C, 0x02, 0x0A, 0x06, 0x0E, 0x01, 0x09, 0x05, 0x0D, 0x03, 0x0B, 0x07, 0x0F};  
byte tempData[6] ;  
int voltData[] = {15, 16, 17, 14, 15}; 
static long voltScale[] = {5000,5000,5000,5000,5000};
volatile byte sw = 0;
volatile  byte respOK = 0;
volatile  byte respERR = 255;
volatile byte* resp = &respERR;
volatile byte respSize;

static int tempTable[TEMP_TABLE_SIZE] = {3898,3684,3445,3189,2919,2645,2370,2105,1856,1626,1418,1226,1058,912,786,676,581,500,431,372,322,280,243,211,185,162};  
 
static long lastTermoMillis = 0;
static byte muxChannel;

//static volatile byte command = 0;
//static volatile byte data = 0;

byte tempFromVolts(int v) {
  if (v > tempTable[0]) return 0;
  if (v < tempTable[TEMP_TABLE_SIZE]) return TEMP_TABLE_SIZE*5;
  int a = 0;
  int b = 26;
  int x;
  while (a + 1 != b) {
    x = (a + b) / 2;
    if (v > tempTable[x]) {
      b = x;
    } else {
      a = x;
    }
  }
 int temp = 5 * a + (5*tempTable[a] - 5*v)/(tempTable[a] - tempTable[b]);
 if (temp > 255) return 255;
 else return temp; 
}

// the setup function runs once when you press reset or power the board
void setup() {
    pinMode(3, OUTPUT);
  
  ACSR  |=  (1<<ACI);    // clear Analog Comparator interrupt
  ACSR  |=
        (0<<ACD)   |         // Comparator ON
        (0<<ACBG)  |         // Disconnect 1.23V reference from AIN0 (use AIN0 and AIN1 pins)
        (0<<ACIE)  |         // Comparator Interrupt enabled
        (0<<ACIC)  |         // input capture disabled
        (0<<ACIS1) |         // set interrupt bit on rising edge
        (0<<ACIS0);          //    (ACIS1 and ACIS0 == 11)
   // Enable the interrupt
  ACSR = ACSR | (1 << ACIE);

//    Serial.begin(9600);
  // set up the ADC
  ADCSRA &= ~PS_128;  // remove bits set by Arduino library

  // you can choose a prescaler from above.
  // PS_16, PS_32, PS_64 or PS_128
  ADCSRA |= PS_128;    // set our own prescaler to 64 

  // initialize digital pin 13 as an output.
  pinMode(0, INPUT);
  pinMode(1, INPUT);
  pinMode(14, INPUT);
  pinMode(15, INPUT);
  pinMode(17, INPUT);
  pinMode(5, OUTPUT);
  pinMode(8, OUTPUT);

  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  
  setChannel(8);
  
//  Wire.begin(); // join i2c bus (address optional for master)
  Wire.begin(2);                // join i2c bus with address #2
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event

}

ISR(ANALOG_COMP_vect ) {
  if ( (ACSR & (1 << ACO))== 0 ) {
    digitalWrite(3, HIGH);   // turn the LED on (HIGH is the voltage level)
  } else {
    digitalWrite(3, LOW);    // turn the LED off by making the voltage LOW
  }
}


void rotateChannel() {
  if (muxChannel == 15) muxChannel = 8;
  else muxChannel++;
  digitalWrite(9, HIGH);
  PORTB = (PORTB & 0xC3) | (channelToPins[muxChannel] << 2);
  digitalWrite(9, LOW);
}

void setChannel(byte ch) {
  muxChannel = ch;
  digitalWrite(9, HIGH);
  PORTB = (PORTB & 0xC3) | (channelToPins[muxChannel] << 2);
  digitalWrite(9, LOW);
}


void sendb(byte m, byte r, byte v) {
      Wire.beginTransmission(4); // transmit to device #4
      Wire.write( (r&0x0F) | (m <<4) );        // sends five bytes
      Wire.write(v);        // sends five bytes
      Wire.endTransmission();    // stop transmitting
}

void sendint(int p,int i) {
  sendb(MODE_DISP,p,highByte(i));
  sendb(MODE_DISP,p-1,lowByte(i));
}


int readVolts(byte pin) {
  int val = analogRead(pin);
  long volt = val;
//  volt = volt * 10175 /1023;
  volt = volt * 13100 /1023;
  return volt;  
}

int readTemp(byte pin) {
  int val = analogRead(pin);
  long volt = val;
//  volt = volt * 10175 /1023;
  volt = volt * 5000 /1023;
  return volt;  
}


// the loop function runs over and over again forever
void loop() {
//  Serial.println(volt);
  long m = millis();
  if (m-lastTermoMillis > 1000) {
    lastTermoMillis = m;
//    rotateChannel();
    digitalWrite(5, (!digitalRead(0)) | bitRead(sw,0) );
    digitalWrite(8, (!digitalRead(1)) | bitRead(sw,1) );
  }
  /*
  int disp = muxChannel;
  disp = (disp << 12) | (readTemp(14) & 0xFFF);
  sendint(3,disp);
  long val2 = analogRead(15);
  long volt2 = val2 * 13100 /1023;
  sendint(1,volt2);
  */
  for (int i = 8; i < 14;i++) {
    setChannel(i);
    delay(1);
    int temp = (readTemp(14) + readTemp(14) + readTemp(14) + readTemp(14))/4;
    tempData[muxChannel - 8] = tempFromVolts(temp);
  } 
  for (int i = 15; i < 18;i++) {
   long val = (analogRead(i) + analogRead(i) + analogRead(i) + analogRead(i))/4;
   int v = i - 15;
   long volt = val * voltScale[v] /1023;
   voltData[v] = volt;
  } 
  for (int i = 14; i < 16;i++) {
    setChannel(i);
    delay(1);
    long val = (analogRead(14) + analogRead(14) + analogRead(14) + analogRead(14))/4;
    int v = i - 11;
    long volt = val * voltScale[v] /1023;
    voltData[v] = volt;
  } 

  delay(100);    

  
  

}

void requestEvent()
{  
//  Wire.write(tempData, 6); // respond with message of 6 bytes
  Wire.write((byte*)resp, respSize); // respond with message of 6 bytes
}

void receiveEvent(int howMany)
{
  byte command, data, chsum;
  if (Wire.available() < 3) {
    resp = &respERR;
    respSize = 1;
    while(0 < Wire.available()) // loop through all but the last
    {
      Wire.read(); // receive byte as a character
    }
    return;
  }
  while(2 < Wire.available()) // loop through all but the last
  {
    command = Wire.read(); // receive byte as a character
  }
  data = Wire.read();    // receive byte as an integer
  chsum = Wire.read();
  if (command ^ data == chsum) {
    handleCommand(command, data);
  } else {
    resp = &respERR;
    respSize = 1;
  }
}

void handleCommand(byte command, byte data) {
  switch (command) {
    case COMMAND_SET_SWITCH: 
      sw = data;
      resp = &sw;
      respSize = 1;
      break;
    case COMMAND_GET_SWITCH: 
      resp = &sw;
      respSize = 1;
      break;
    case COMMAND_GET_TEMP: 
      resp = tempData;
      respSize = 6;
      break;
    case COMMAND_GET_VOLTS: 
      resp = voltData;
      respSize = 10;
     break;
    default: 
      resp = &respERR;
      respSize = 1;
     break;
  }
}

