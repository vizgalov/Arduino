// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
    Serial.begin(9600);           // start serial for output
    Serial.println("ready");
}


void loop()
{

  if (Serial.available() > 0) {

    // look for the next valid integer in the incoming serial stream:
    byte b1 = Serial.parseInt(); 
    // do it again:
    byte b2 = Serial.parseInt(); 

    // look for the newline. That's the end of your
    // sentence:
    while (Serial.available() > 0) {
      Serial.read();
    }
      Serial.print(" Sending ");
      Serial.print(b1, HEX);
      Serial.print(" ");
      Serial.println(b2, HEX);

      Wire.beginTransmission(4); // transmit to device #4
      Wire.write(b1);        // sends five bytes
      Wire.write(b2);              // sends one byte  
      Wire.endTransmission();    // stop transmitting

      // print the three numbers in one string as hexadecimal:
      Serial.print(" Sent ");
      Serial.print(b1, HEX);
      Serial.print(" ");
      Serial.println(b2, HEX);
  }

  delay(500);


} 
