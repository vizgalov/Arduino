#include <Wire.h>
#include <LiquidCrystal.h>
//#include <Wire.h>


#define COMMAND_SET_SWITCH 1
#define COMMAND_GET_SWITCH 2
#define COMMAND_GET_TEMP   3
#define COMMAND_GET_VOLTS  4

//static byte resp[33];

  byte com =0;
  int i;



  //            lcd(RS,  E, d4, d5, d6, d7)

 LiquidCrystal lcd(8,   9,  4,  5,  6,  7);
 
void setup()
{
     lcd.begin(16, 2);               // start the library
   lcd.setCursor(0,0);             // set the LCD cursor   position 

  
  Wire.begin(6);                // join i2c bus with address #4
  Serial.begin(9600);           // start serial for output
    Serial.println("Start");         // print the character
}


void sendCommand(byte command, byte data, byte[] resp, byte respSize) {
  Wire.beginTransmission(2); // transmit to device #4
  Wire.write(command);        // sends five bytes
  Wire.write(data);              // sends one byte  
  Wire.write(command ^ data);              // sends one byte  
  Wire.endTransmission();    // stop transmitting
  delayMicroseconds(100);
  Wire.requestFrom(2, respSize);    // request 6 bytes from slave device #2
  int i = 0;
  while(Wire.available() > 0) {
     if (i < respSize) resp[i] = Wire.read();
     else Wire.read(); 
  }
  
  
  
}

void loop()
{
  Serial.println("req");         // print the character
  
  com = com +2;
  
  delay(100);
  Wire.requestFrom(2, 32);    // request 6 bytes from slave device #2

      lcd.setCursor(0, 0);
      i = 0;
  while(Wire.available() > 0)    // slave may send less than requested
  { 
    int b1 = Wire.read(); // receive a byte as character
    int b2 = b1 | Wire.read() << 8; // receive a byte as character
    Serial.println(b2);         // print the character
      if (i == 8)       lcd.setCursor(0, 1);
       lcd.print(b2);
    i++;  
//      lcd.print(" ");
  }

    Serial.println("");         // print the character

//      lcd.print(t.sec);
  
  delay(1000);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()

