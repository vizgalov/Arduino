#include <Time.h>

#include <DS1302RTC.h>


// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.
#define encoder0PinA  2
#define encoder0PinB  4


#include <Wire.h>


volatile unsigned int pos = 0;
volatile unsigned int cnt = 0;
int last_pos = 0;
int last_cnt = 0;
static volatile byte reg = 0;
static volatile byte change = 0;
static volatile byte vals[4];
static volatile byte mode = 0;

#define MODE_DISP   0x01
#define MODE_SEL    0x02
#define MODE_EDIT   0x03

// Set pins:  CE, IO,CLK
DS1302RTC RTC(7, 6, 5);


void setup()
{
  mode = MODE_DISP;
  for (int i=0; i <=4 ; i++){
      vals[i] = 0;
   } 
  
  for (int i=2; i <=5 ; i++){
      pinMode(i, INPUT);
   } 
   pinMode(0, OUTPUT);

  Wire.begin(); // join i2c bus (address optional for master)
    //Serial.begin(9600);           // start serial for output
    //Serial.println("ready");
  attachInterrupt(0, doEncoder, RISING);  // encoder pin on interrupt 0 - pin 2
  attachInterrupt(1, doClk, RISING);  // encoder pin on interrupt 0 - pin 2

}

void doClk() {
  pos = 0;
  cnt = 0;
//  reg++;
//  if (reg > 3) reg = 0;
  switch (mode) {
    case MODE_DISP:
      mode = MODE_SEL;
      break;
    case MODE_SEL:
      mode = MODE_EDIT;
      break;
    case MODE_EDIT:
      mode = MODE_DISP;
      break;
    default:
      mode = MODE_DISP;
      break;
    
  }
  change = 1;
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
    switch (mode) { 
      case MODE_DISP:
        mode = MODE_SEL;
        break;
      case MODE_SEL:
        if (reg > 0) reg--;
        break;
      case MODE_EDIT:
        vals[reg]--;
        break;
      default:
        break;
    }
  } else {
    pos++;
    switch (mode) {
      case MODE_DISP:
        mode = MODE_SEL;
        break;
      case MODE_SEL:
        reg++;
        if (reg >3) reg = 3;
        break;
      case MODE_EDIT:
        vals[reg]++;
        break;
      default:
        break;
    }
  }
  change = 1;
}

byte b1  = 0;
byte b2  = 0;

void sendb(byte m, byte r) {
      Wire.beginTransmission(4); // transmit to device #4
      Wire.write( (r&0x0F) | (m <<4) );        // sends five bytes
      Wire.write(vals[r]);        // sends five bytes
      Wire.endTransmission();    // stop transmitting

      Wire.beginTransmission(6); // transmit to device #4
      Wire.write( (r&0x0F) | (m <<4) );        // sends five bytes
//      Wire.write(vals[r]);        // sends five bytes
      Wire.endTransmission();    // stop transmitting

}

void loop()
{
  if (change == 1) {
      sendb(mode, reg);
      change = 0;
  }
      b1++;
      if (b1 == 255) b2++;
      
  digitalWrite(0, bitRead(b1,4));
  delay(50);
  
  if (b1 == 0) {
    tmElements_t tm;
    
    if (! RTC.read(tm)) {
      Wire.beginTransmission(4); // transmit to device #4
      Wire.write(0xFF);        // sends five bytes
      Wire.write(tm.Hour);        // sends five bytes
      Wire.write(tm.Minute);        // sends five bytes
      Wire.endTransmission();    // stop transmitting
    } else {
      byte halt = 0;
      byte wr = 0;
      if (RTC.haltRTC()) halt = 1;   
      if (RTC.writeEN()) wr = 1;   


      Wire.beginTransmission(4); // transmit to device #4
      Wire.write(0xFF);        // sends five bytes
      Wire.write(halt);        // sends five bytes
      Wire.write(wr);        // sends five bytes
      Wire.endTransmission();    // stop transmitting
  }

  }


} 
