#include <LiquidCrystal.h>

 LiquidCrystal lcd(8,   9,  4,  5,  6,  7);

unsigned int sensorValue1, temp1;
unsigned int sensorValue2, temp2;

#define TEMP_HISTORY_SIZE 256;
//unsigned int tempHistory[256];
//byte tempHistoryIndex = 0;

unsigned int tempTarget = 250;
byte signal = 0;
byte heaterOn = 0;
byte boilerOn = 0;

#define SENSOR_PIN_1 A8 
#define SENSOR_PIN_2 A9
#define PWM_PIN 44
#define HEATER_PIN 22
#define BOILER_PIN 32
#define TEMP_TABLE_SIZE 11

static unsigned long tempTable[TEMP_TABLE_SIZE] = {810,765,720,669,608,549,487,431,379,328,284};  
 
 
 // define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
 
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5


unsigned long tempFromVolts(unsigned long v) {
  if (v > tempTable[0]) return 0;
  if (v < tempTable[TEMP_TABLE_SIZE]) return TEMP_TABLE_SIZE*5;
  unsigned long a = 0;
  unsigned long b = TEMP_TABLE_SIZE - 1;
  int x;
  while (a + 1 != b) {
    x = (a + b) / 2;
    if (v > tempTable[x]) {
      b = x;
    } else {
      a = x;
    }
  }
 unsigned long temp = 50 * a + (50*tempTable[a] - 50*v)/(tempTable[a] - tempTable[b]);
 return temp; 
}
  
void setup()
{
    pinMode(SENSOR_PIN_1, INPUT);
    pinMode(SENSOR_PIN_2, INPUT);
    pinMode(BOILER_PIN, INPUT_PULLUP);
    pinMode(PWM_PIN, OUTPUT);
    digitalWrite(HEATER_PIN, HIGH);
    pinMode(HEATER_PIN, OUTPUT);
    lcd.clear();               // start the library
    lcd.begin(16, 2);               // start the library
    lcd.setCursor(0,0);             // set the LCD cursor   position 
    Serial.begin(9600);           // start serial for output
    Serial.println("Start");         // print the character
    
//    for (int i =0;i<256;i++) {
//      tempHistory[i] = readTemp();
//    }
}

void decreaseTempTarget() {
  if (tempTarget == 0) return;
  else tempTarget--;
}

void increaseTempTarget() {
  if (tempTarget == 500) return;
  else tempTarget++;
}

unsigned int readTemp(){
    return (tempFromVolts(analogRead(SENSOR_PIN_2)) + tempFromVolts(analogRead(SENSOR_PIN_2)))/2;
}

int boilerTmp;
int readBoiler() {
  boilerTmp = digitalRead(BOILER_PIN);
  if ((boilerTmp == LOW) && (digitalRead(BOILER_PIN) == boilerTmp)) return 1;
  else return 0;
}

void loop()
{
    sensorValue1 = analogRead(SENSOR_PIN_1);   
    temp1 = sensorValue1 * 5000/1023;
    temp2 = readTemp();
    Serial.print(sensorValue1);
    Serial.print(" ");
    Serial.print(temp1);
    Serial.print("  ");
    Serial.print(sensorValue2);
    Serial.print(" ");
    Serial.print(temp2);
    Serial.println();


   lcd_key = read_LCD_buttons();   // read the buttons
     lcd.setCursor(12, 0);
 
   switch (lcd_key){               // depending on which button was pushed, we perform an action
 
       case btnRIGHT:{             //  push button "RIGHT" and show the word on the screen
            lcd.print("RIGHT ");
             if (signal != 0xFF) {
               signal++;
             }
            break;
       }
       case btnLEFT:{
             lcd.print("LEFT   "); //  push button "LEFT" and show the word on the screen
             if (signal != 0x00) {
               signal--;
             }
             break;
       }    
       case btnUP:{
             lcd.print("UP    ");  //  push button "UP" and show the word on the screen
             increaseTempTarget();
             break;
       }
       case btnDOWN:{
             lcd.print("DOWN  ");  //  push button "DOWN" and show the word on the screen
             decreaseTempTarget();
             break;
       }
       case btnSELECT:{
             lcd.print("SELECT");  //  push button "SELECT" and show the word on the screen
             break;
       }
       case btnNONE:{
             lcd.print("NONE  ");  //  No action  will show "None" on the screen
             break;
       }
   }
  
  boilerOn = readBoiler();
  
  if (heaterOn && (temp2 > tempTarget + 6)) {
    heaterOn = 0;
  }

  if (!heaterOn && (temp2 < tempTarget - 6)) {
    heaterOn = 1;
 //   signal = 0;
  }
  
  if (boilerOn && (temp2 > tempTarget + 6)) {
 //   signal = 255;
  }

  if (!boilerOn && (temp2 < tempTarget - 6)) {
  //  signal = 0;
  }
  
  if ((signal == 255) && (temp2 < tempTarget - 6)) {
   // signal = 0;
  }
  
  if (heaterOn || boilerOn) {
    digitalWrite(HEATER_PIN, HIGH);
  } else {
    digitalWrite(HEATER_PIN, LOW);
  }
      lcd.setCursor(0, 0);
      lcd.print(signal);
      lcd.print("  ");
      lcd.print(boilerOn);
      lcd.println("  ");

      lcd.setCursor(0, 1);
      lcd.print(tempTarget);
      lcd.print(" ");
      lcd.print(temp2);
      lcd.print(" ");
      lcd.print(heaterOn);
      lcd.print("  ");


  analogWrite(PWM_PIN, signal);
  delay(80);
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
 

