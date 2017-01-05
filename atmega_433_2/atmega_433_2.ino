#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile

RH_ASK driver;

void setup()
{
    Serial.begin(9600);	// Debugging only
    if (!driver.init())
         Serial.println("init failed");
}

void loop()
{
    uint8_t buf[12];
    uint8_t buflen = sizeof(buf);
    if (driver.recv(buf, &buflen)) // Non-blocking
    {
      int i;
      // Message with a good checksum received, dump it.
      Serial.print("Message: ");
      Serial.println((char*)buf);         
    }
}


#include <Arduino.h>
#include <ampline.h>

  byte com =0;
  int i;

 
const int rfpin = 4;
AmperkaLine rf(rfpin);



 
 
void setup()
{
   pinMode(rfpin, INPUT);
   pinMode(9, OUTPUT);
   pinMode(10, OUTPUT);
   pinMode(13, OUTPUT);
   
   if (!driver.init()) {
      digitalWrite(10,1);
      delay(1000);
      digitalWrite(10,0);
      delay(1000);
      digitalWrite(10,1);
      delay(1000);
      digitalWrite(10,0);
   }

}




void loop()
{
    unsigned long msg;
    int res;
    byte b;
    
    digitalWrite(13,0);
    
//    if((res = rf.receive(&msg)) == 0)
    {
       b=msg;
       analogWrite(9, b);
//    } else {
//      digitalWrite(13,1);
    }
  delay(100);
  analogWrite(10, com++);
}


