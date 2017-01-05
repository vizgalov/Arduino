#include <LiquidCrystal.h>

 LiquidCrystal lcd(8,   9,  4,  5,  6,  7);


byte b = 0x00;
 
 
 // define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
 
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

#define DE_PIN   53

byte r = 0x00;

void setup()
{
    pinMode(DE_PIN, OUTPUT);
    digitalWrite(DE_PIN, HIGH);
    lcd.clear();               // start the library
    lcd.begin(16, 2);               // start the library
    lcd.setCursor(0,0);             // set the LCD cursor   position 
    Serial.begin(9600);           // start serial for output
    Serial.println("Start");         // print the character
    Serial1.begin(9600);
}

void loop()
{
  
    Serial.write(b);
    sendByte(b); 
    b = b + 0x40; 

    lcd.setCursor(0, 1);
    lcd.print(r);
    lcd.print("  ");


   delay(500);
}

void sendByte(byte x) {
//    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(x);
    lcd.print("  ");
    digitalWrite(DE_PIN, HIGH);
    Serial1.write(x);
    delay(5);
    digitalWrite(DE_PIN, LOW);
}
  
int read_LCD_buttons(){               // read the buttons
    adc_key_in = analogRead(0);       // read the value from the sensor 
 
    // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
    // we add approx 50 to those values and check to see if we are close
    // We make this the 1st option for speed reasons since it will be the most likely result
 
    if (adc_key_in > 1000) return btnNONE; 
 
    // For V1.1 us this threshold
/*
    if (adc_key_in < 50)   return btnRIGHT;  
    if (adc_key_in < 250)  return btnUP; 
    if (adc_key_in < 450)  return btnDOWN; 
    if (adc_key_in < 650)  return btnLEFT; 
    if (adc_key_in < 850)  return btnSELECT;  
*/ 
   // For V1.0 comment the other threshold and use the one below:
   
     if (adc_key_in < 50)   return btnRIGHT;  
     if (adc_key_in < 195)  return btnUP; 
     if (adc_key_in < 380)  return btnDOWN; 
     if (adc_key_in < 555)  return btnLEFT; 
     if (adc_key_in < 790)  return btnSELECT;   
   
 
    return btnNONE;                // when all others fail, return this.
}


void serialEvent1() {
  while (Serial1.available()) {
    r = Serial.read();
  }
}

