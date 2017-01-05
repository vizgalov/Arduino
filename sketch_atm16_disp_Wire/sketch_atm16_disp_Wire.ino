#include <Wire2.h>
//#include <compat/twi.h>

#define encoder0PinA  10
#define encoder0PinB  12

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


volatile unsigned int pos = 0;
volatile unsigned int cnt = 0;
int last_pos = 0;
int last_cnt = 0;
//int vals[4];
unsigned long last_mills = 0;

int val = 1;
byte b = 1;
volatile byte b1 = 0;
volatile byte b2 = 0;
volatile byte b3 = 0;
volatile byte b4 = 0;

static byte logs[256];
static volatile byte logi = 0;
static volatile byte logd = 255;
static byte logth = 0;

int bi = 0;


int i;

static byte dig[] = {123, 65, 55, 87, 77, 94, 126, 67, 127, 95};
static byte dig16[] = {123, 65, 55, 87, 77, 94, 126, 67, 127, 95, 239, 252, 58, 245, 190, 46};
static int byteToDigs[256];
 

void setup() { 


  pinMode(encoder0PinA, INPUT); 
  pinMode(encoder0PinB, INPUT); 
  pinMode(11, INPUT);
  initByteToDigs16();
  // initialize digital pin 13 as an output.
  for (int i=19; i <= 32; i++){
      pinMode(i, OUTPUT);
   } 
  for (int i=0; i <= 7; i++){
      pinMode(i, OUTPUT);
   } 

 logb(3);
 logb(3);
logb(3);
logb(3);
logb(3);
logb(3);
logb(3);
logb(3);

  //Wire2.begin(4);                // join i2c bus with address #4
  //Wire2.onReceive(receiveEvent); // register event

//TWCR = 0x45; // ACK bit, TWI & interrupt enabled
//TWSR = 0x48; // zero out clock prescaler bits
//TWAR = 0x08; // slave address = 0x80, no general call support
// Step two (2) : Create an interrupt handler
// logb(TWCR);
// logb(TWAR);
// logb(TWSR);
// logb(TWDR);
 
} 

void receiveEvent(int howMany)
{
//    logb(howMany);
 // while(0 < Wire2.available()) // loop through all but the last
  {
  //  byte b = Wire2.read(); // receive byte as a character
//    logb(b);
  }
}



void logb(byte s) {
  if (logth == 1) {
    if (logd == logi - 1) {
      logth = 0;
      logs[logi] = 0;
      logi++;
    } else {
      return;
    }
  } 
  logs[logi] = s;
  logi++;
  if (logi == logd) logth = 1;
//  bi++;
//  if (bi > 3) bi = 0;
}

void disp(byte b) {
  b4 = b3;
  b3 = b2;
  b2 = b1;
  b1 = b;
  
//  b4 = logi;
//  b3 = logd;
}


/* 
 * Function twi_stop
 * Desc     relinquishes bus master status
 * Input    none
 * Output   none
 */
void twi_stop(void)
{
  // send stop condition
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTO);
  // wait for stop condition to be exectued on bus
  // TWINT is not set after a stop condition!
  while(TWCR & _BV(TWSTO)){
    continue;
  }
}

/* 
 * Function twi_releaseBus
 * Desc     releases bus control
 * Input    none
 * Output   none
 */
void twi_releaseBus(void)
{
  // release bus
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT);

}

void twi_reply(uint8_t ack)
{
  // transmit master read ready signal, with or without ack
  if(ack){
    TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
  }else{
	  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
  }
}




void initByteToDigs(){
  int num = 0;
  int twoDigs;
  for (int i=0;i<10;i++) {
    for (int j=0;j<10;j++) {
      twoDigs = dig[j];
      twoDigs = (twoDigs << 8) + dig[i];
      byteToDigs[num] = twoDigs;
      num++;
    }
  }
}

void initByteToDigs16(){
  int num = 0;
  int twoDigs;
  for (int i=0;i<16;i++) {
    for (int j=0;j<16;j++) {
      twoDigs = dig16[j];
      twoDigs = (twoDigs << 8) + dig16[i];
      byteToDigs[num] = twoDigs;
      num++;
    }
  }
}

void writeDigs(byte b) {
  int d = byteToDigs[b];
  PORTB = highByte(d);
  PORTA = lowByte(d);
}

void enableDrive(boolean enable) {
  if (enable) 
    digitalWrite(19, LOW ); 
  else 
    digitalWrite(19, HIGH);
}


void setDrive(int d) {
    digitalWrite(20, bitRead(d,0));
    digitalWrite(21, bitRead(d,1));
    digitalWrite(22, bitRead(d,2));
    digitalWrite(23, bitRead(d,3));
  
}

void loop(){
  for (int i=0; i < 4;i++) {
    enableDrive(false);
    switch (i) {
      case 0:
        writeDigs(b1);
        break;
      case 1:
        writeDigs(b2);
        break;
      case 2:
        writeDigs(b3);
        break;
      case 3:
        writeDigs(b4);
        break;
      default: 
  //      writeDigs(b4);
        break;
    }
    setDrive(i);
    enableDrive(true);
    delayMicroseconds(500);
//    delay(1);
  }
  
  dispIfREq();

}

void dispIfREq() {
  unsigned long m = millis();
  if (m - last_mills > 1000) {
    last_mills = m;
    if (logd != (logi - 1)) {
      logd++;
      disp(logs[logd]);
    }
  }
}

void doClk() {
  pos = 0;
  cnt = 0;
}

void doEncoder() {
  /* If pinA and pinB are both high or both low, it is spinning
   * forward. If they're different, it's going backward.
   *
   * For more information on speeding up this process, see
   * [Reference/PortManipulation], specifically the PIND register.
   */
//  int m = millis();
//  if (last_mills == m) {
//    return;
//  } 
//  last_mills = m;
  cnt++; 
  if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB)) {
    pos--;
  } else {
    pos++;
  }

}

 
/*  to read the other two transitions - just use another attachInterrupt()
in the setup and duplicate the doEncoder function into say, 
doEncoderA and doEncoderB. 
You also need to move the other encoder wire over to pin 3 (interrupt 1). 
*/ 
