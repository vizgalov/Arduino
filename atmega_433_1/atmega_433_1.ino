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


