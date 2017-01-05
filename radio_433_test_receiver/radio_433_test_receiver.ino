#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile

RH_ASK driver;

 
void setup()
{
  Serial.begin(115200);
  Serial.println("RH TEST PROGRAM ");
      
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


uint8_t buf[2];
uint8_t buflen = 1;

uint8_t speed = 0;


void loop()
{
    
    if (driver.recv(buf, &buflen)) // Non-blocking
    {
          Serial.print("Received "); 
          Serial.print(buf[0]); 
          Serial.println(buf[1]); 

    }
      delay(100);

}


