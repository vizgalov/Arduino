//#include <Wire.h>
#include <compat/twi.h>

#define encoder0PinA  10
#define encoder0PinB  12

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define MODE_DISP   0x01
#define MODE_SEL    0x02
#define MODE_EDIT   0x03


volatile unsigned int pos = 0;
volatile unsigned int cnt = 0;
int last_pos = 0;
int last_cnt = 0;
int vals[4];
static unsigned long last_mills = 0;
static volatile byte selected = -1;
static volatile byte edited = -1;

int val = 1;
byte b = 1;
static volatile byte b1 = 0;
static volatile byte b2 = 0;
static volatile byte b3 = 0;
static volatile byte b4 = 0;

static byte logs[256];
static volatile byte logi = 0;
static volatile byte logd = 255;
static byte logth = 0;

static int counter = 0;

int bi = 0;


int i;

static byte dig[] = {123, 65, 55, 87, 77, 94, 126, 67, 127, 95};
static byte dig16[] = {123, 65, 55, 87, 77, 94, 126, 67, 127, 95, 239, 252, 58, 245, 190, 46};
static int byteToDigs[256];

static volatile byte twi_state = 0;
static volatile byte command = 0; 
static volatile byte data = 0; 

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

//  attachInterrupt(0, doEncoder, RISING);  // encoder pin on interrupt 0 - pin 2
//  attachInterrupt(1, doClk, RISING);  // encoder pin on interrupt 0 - pin 2
//  Serial.begin (9600);
//  Serial.println("start");                // a personal quirk
//  Wire.begin(4);                // join i2c bus with address #4
//  Wire.onReceive(receiveEvent); // register event

TWCR = 0x45; // ACK bit, TWI & interrupt enabled
TWSR = 0x48; // zero out clock prescaler bits
TWAR = 0x08; // slave address = 0x80, no general call support
// Step two (2) : Create an interrupt handler

} 
/*
ISR(TWI_vect) {
 b4++;
 switch(TW_STATUS) {
    case TW_SR_SLA_ACK: // slave addressed
      break;
    case TW_SR_ARB_LOST_GCALL_ACK: // lost arbitration, returned ack
      twi_reply(1);
      break;
    case TW_SR_DATA_ACK:       // data received, returned ack
    case TW_SR_GCALL_DATA_ACK: // data received generally, returned ack
    case TW_SR_STOP: // stop or repeated start condition received
      twi_stop();
      twi_releaseBus();
      break;
    case TW_SR_DATA_NACK:       // data received, returned nack
    case TW_SR_GCALL_DATA_NACK: // data received generally, returned nack
      // nack back at master
      twi_reply(0);
      break;
    default: // ?? unknown status (error?)
    break;
  }
  twi_reply(1);
//  sbi(TWCR, TWINT); // clear TWINT bit
}
*/
ISR(TWI_vect)
{
  byte s = TW_STATUS;
// logb(s);
  switch(TW_STATUS){
    // All Master
    case TW_START:     // sent start condition
    case TW_REP_START: // sent repeated start condition
      // copy device address and r/w bit to output register and ack
//      TWDR = twi_slarw;
      twi_reply(1);
      break;

    // Master Transmitter
    case TW_MT_SLA_ACK:  // slave receiver acked address
    case TW_MT_DATA_ACK: // slave receiver acked data
          twi_stop();
          break;
    case TW_MT_SLA_NACK:  // address sent, nack received
      twi_stop();
      break;
    case TW_MT_DATA_NACK: // data sent, nack received
      twi_stop();
      break;
    case TW_MT_ARB_LOST: // lost bus arbitration
      twi_releaseBus();
      break;

    // Master Receiver
    case TW_MR_DATA_ACK: // data received, ack sent
      // put byte into buffer
    case TW_MR_SLA_ACK:  // address sent, ack received
      // ack if more bytes are expected, otherwise nack
      twi_reply(1);
      break;
    case TW_MR_DATA_NACK: // data received, nack sent
      // put final byte into buffer
        twi_stop();
	break;
    case TW_MR_SLA_NACK: // address sent, nack received
      twi_stop();
      break;
    // TW_MR_ARB_LOST handled by TW_MT_ARB_LOST case

    // Slave Receiver
    case TW_SR_SLA_ACK:   // addressed, returned ack
      twi_state = 1;
      twi_reply(1);
      break;
    case TW_SR_GCALL_ACK: // addressed generally, returned ack
    case TW_SR_ARB_LOST_SLA_ACK:   // lost arbitration, returned ack
    case TW_SR_ARB_LOST_GCALL_ACK: // lost arbitration, returned ack
      twi_reply(1);
      break;
    case TW_SR_DATA_ACK:       // data received, returned ack
  //    logb(TWDR);
      if (twi_state == 1) {
 //       b1 = TWDR;
        command = TWDR;
        twi_state = 2;
        twi_reply(1);
        break;
      }
      if (twi_state == 2) {
        data = TWDR;
        twi_state = 3;
        twi_reply(1);
        break;
      }
      if (twi_state == 3) {
        twi_state = 4;
        twi_reply(1);
        break;
      }
      twi_reply(1);
      break;
    case TW_SR_GCALL_DATA_ACK: // data received generally, returned ack
    s = TW_SR_GCALL_DATA_ACK;
//    logb(TWDR);
      twi_reply(1);
      break;
    case TW_SR_STOP: // stop or repeated start condition received
      twi_state = 0;
      twi_stop();
      twi_releaseBus();
      break;
    case TW_SR_DATA_NACK:       // data received, returned nack
    case TW_SR_GCALL_DATA_NACK: // data received generally, returned nack
      // nack back at master
      twi_reply(0);
      break;
    
    // Slave Transmitter
    case TW_ST_SLA_ACK:          // addressed, returned ack
    case TW_ST_ARB_LOST_SLA_ACK: // arbitration lost, returned ack
      // transmit first byte from buffer, fall
    case TW_ST_DATA_ACK: // byte sent, ack returned
      // copy data to output register
        twi_reply(1);
      break;
    case TW_ST_DATA_NACK: // received nack, we are done 
    case TW_ST_LAST_DATA: // received ack, but we are done already!
      // ack future responses
      twi_reply(1);
      // leave slave receiver state
      break;

    // All
    case TW_NO_INFO:   // no state information
      break;
    case TW_BUS_ERROR: // bus error, illegal stop/start
      twi_stop();
      break;
  }
//  b1 = bi;
//  logb(twi_state);
  if (twi_state == 3) {
 //   b3 = command;
 //   b4 = data;
    processCommand(command, data);
  }
}

void processCommand(byte command, byte data) {
 // logb(command);
 // logb(data);
 byte mode = command >> 4;
 byte reg = command & 0x0F;
 if (mode == MODE_SEL) {
   selected = reg;
 } else {
   selected = -1;
 }
 if (mode == MODE_EDIT) {
   edited = reg;
 } else {
   edited = -1;
 }
  switch (reg) {
    case 0: 
      b1 = data;
      break;
    case 1: 
      b2 = data;
      break;
    case 2: 
      b3 = data;
      break;
    case 3: 
      b4 = data;
      break;
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


void receiveEvent(int howMany)
{
//  if (Wire.available() > 1) b1 = Wire.read();
//  if (Wire.available() > 1) b2 = Wire.read();
//  if (Wire.available() > 1) b3 = Wire.read();
 // b3 = TWBR;
 // b4 = TWAR;
 // b2 = TWSR;  
//  while(1 < Wire.available()) // loop through all but the last
//  {
//    byte b = Wire.read(); // receive byte as a character
//  }
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

void setDrive(int d) {
    digitalWrite(20, bitRead(d,0));
    digitalWrite(21, bitRead(d,1));
    digitalWrite(22, bitRead(d,2));
    digitalWrite(23, bitRead(d,3));
  
}

void enableDrive(boolean enable) {
  if (enable) 
    digitalWrite(19, LOW); 
  else 
    digitalWrite(19, HIGH);
}

void loop(){
  for (int i=0; i < 16;i++) {
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
    
    if ((i == selected)) {
      if (bitRead(counter, 4) == 1) enableDrive(true);
      delayMicroseconds(10000);
    } else {
      enableDrive(true);
      delayMicroseconds(500);
    }  
 //   if (i == selected) delayMicroseconds(10000);
    if (i == edited) delayMicroseconds(10000);
    //b2 = TWSR;
    //b1 = TWDR;
  }
  counter++;
/*
  b1++;
  if (b1 > 99) {
    b1 = 0;
    b2++;
  }
  if (b2 > 99) {
    b2 = 0;
    b3++;
  }
  if (b3 > 99) {
    b3 = 0;
    b4++;
  }
  if (b4 > 99) {
    b4 = 0;
  }
*/
//  delay(1);
/*
  unsigned int p = pos;
  unsigned int c = cnt;
  if (p != last_pos || c != last_cnt) {
    last_pos = p;
    last_cnt = c;
 //   Serial.print (p, DEC);
 //   Serial.print ("   ");
 //   Serial.println (c, DEC);
  b1 = c % 100;
  b2 = (c / 100) % 100;
  b3 = p % 100;
  b4 = (p / 100) % 100;  
  }*/

/*
  unsigned long m = millis();
  if (m - last_mills > 1000) {
    last_mills = m;
    if (logd != (logi - 1)) {
      logd++;
      disp(logs[logd]);
    }
  } 
*/
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
