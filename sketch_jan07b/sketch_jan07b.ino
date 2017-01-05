#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

#define encoder0PinA  2
#define encoder0PinB  4

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


RH_ASK driver;

volatile unsigned int pos = 0;
volatile unsigned int cnt = 0;
int last_pos = 0;
int last_cnt = 0;

void setup()
{
   pinMode(encoder0PinA, INPUT); 
   pinMode(encoder0PinB, INPUT); 

   pinMode(9, OUTPUT);
   pinMode(13, OUTPUT);

    Serial.begin(9600);	  // Debugging only
    if (!driver.init())
         Serial.println("init failed");
   
   digitalWrite(9,1);
   digitalWrite(10,1);
   delay(500);
   digitalWrite(9,0);
   digitalWrite(10,0);

  attachInterrupt(digitalPinToInterrupt(2), doEncoder, RISING);  // encoder pin on interrupt 0 - pin 2
//  attachInterrupt(1, doClk, RISING);  // encoder pin on interrupt 0 - pin 2

}


void doClk() {
  pos = 0;
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
  if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB)) {
    if (pos>0) pos--;
  } else {
    if (pos<15) pos++;
  }
}


uint8_t left = 0;
uint8_t right = 0;
uint8_t val = 0;


void loop()
{
    val = (left << 4) | (right & 0x0F);
    Serial.print("sending ");
    Serial.print(val);
    Serial.print(" pos ");
    Serial.println(pos);
    driver.send(&val, 1);
    driver.waitPacketSent();
    analogWrite(13, val);
    delay(500);
    
    left = 7;
    right = 7;
    if (pos < 7) left = pos;
    if (pos > 7) right = 15 - pos;
//    left = left + 1;
//    right = right + 1;
}
