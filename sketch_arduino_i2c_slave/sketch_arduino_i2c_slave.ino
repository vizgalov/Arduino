#include <Wire2.h>

//#include <Wire.h>

void setup()
{
  Wire2.begin(6);                // join i2c bus with address #4
  Serial.begin(9600);           // start serial for output
  Wire2.onReceive(receiveEvent); // register event
}

void loop()
{
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  while(0 < Wire2.available()) // loop through all but the last
  {
    byte c = Wire2.read(); // receive byte as a character
    Serial.print(c, HEX);         // print the character
    Serial.print(" ");         // print the character
  }
  Serial.println();         // print the integer
}
