#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile


#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


RH_ASK driver;

int cnt = 0;

void setup()
{

   pinMode(LED_BUILTIN, OUTPUT);
 
   if (!driver.init()) {
      digitalWrite(LED_BUILTIN,1);
      delay(1000);
      digitalWrite(LED_BUILTIN,0);
      delay(1000);
      digitalWrite(LED_BUILTIN,1);
      delay(1000);
      digitalWrite(LED_BUILTIN,0);
   }
   

}



uint8_t val = 0;
uint8_t on = 0;

void loop()
{
    val++;
    
    driver.send(&val, 1);
    driver.waitPacketSent();
      digitalWrite(LED_BUILTIN,1);
      delay(80);
      digitalWrite(LED_BUILTIN,0);
    delay(1000);

}
