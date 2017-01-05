//#include <Wire.h>
#include <LiquidCrystal.h>
#include <ampline.h>



  byte com =0;
  int i;


 LiquidCrystal lcd(8,   9,  4,  5,  6,  7);
 
 const int rfpin = A8;
AmperkaLine rf(rfpin);



 
 
void setup()
{
    pinMode(rfpin, INPUT);
   lcd.begin(16, 2);               // start the library
   lcd.setCursor(0,0);             // set the LCD cursor   position 
    Serial.begin(9600);           // start serial for output
    Serial.println("Start");         // print the character

}




void loop()
{
    unsigned long msg;
    int res;
   
    if((res = rf.receive(&msg)) == 0)
    {
        Serial.print(msg,1);
        Serial.println();

     }
    else Serial.println('E');

  

      lcd.setCursor(0, 0);
      lcd.print(buf[0]);


  
  
//  int sensorValue = analogRead(A8);
  
  delay(1);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()

