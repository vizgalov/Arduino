#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

RH_ASK driver;

void setup()
{
   pinMode(9, OUTPUT);
   pinMode(10, OUTPUT);

   if (!driver.init()) {
      digitalWrite(9,1);
      digitalWrite(10,1);
      delay(1000);
      digitalWrite(9,0);
      digitalWrite(10,0);
      delay(1000);
      digitalWrite(9,1);
      digitalWrite(10,1);
      delay(1000);
      digitalWrite(9,0);
      digitalWrite(10,0);
   }
   
   digitalWrite(9,1);
   digitalWrite(10,1);
   delay(500);
   digitalWrite(9,0);
   digitalWrite(10,0);
}


uint8_t left = 0;
uint8_t right = 0;
uint8_t val = 0;


void loop()
{
    val = (left << 4) | (right & 0x0F);
    driver.send(&val, 1);
    driver.waitPacketSent();
    delay(1000);
    left = left + 1;
    right = right + 1;
}
