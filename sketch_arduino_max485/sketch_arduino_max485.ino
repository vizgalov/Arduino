#include <LiquidCrystal.h>
#include <Serial.h>


 LiquidCrystal lcd(8,   9,  4,  5,  6,  7);
 byte i = 0;
void setup()
{
  lcd.begin(16, 2);               // start the library
  lcd.setCursor(0,0);             // set the LCD cursor   position 
  Serial1.begin(300);           // start serial for output
  pinMode(52, OUTPUT);
}

static byte buff[2];

void sendb(byte b) {
  lcd.setCursor(0,0);             // set the LCD cursor   position 
  lcd.print(b);
  buff[0] = 0x80;
  buff[1] = b;
  Serial1.write(buff,2);
  delay(300);
}

void loop()
{
  sendb(0x35);
  sendb(0x36);
  sendb(0x37);
  i++;
  delay(80);
}


