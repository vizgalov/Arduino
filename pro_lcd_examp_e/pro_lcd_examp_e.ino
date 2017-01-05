#include <EEPROM.h>

#include <DallasTemperature.h>
#include <OneWire.h>
#include <dht.h>

#include <LiquidCrystal.h>

#define DEBUG_ENABLED 1

#define DHT11_PIN 17

#define encoderPinA  2
#define encoderPinB  11
#define encoderPinClk  3

#define INPUT_STATE_IDLE  0
#define INPUT_STATE_SET_HUM  1
#define INPUT_STATE_SET_TEMP  2

volatile byte inputState = INPUT_STATE_IDLE;

volatile int8_t maxHum = 30;
volatile int8_t minTemp = 5;

volatile boolean doStore = false;
long flashWrites = 1;
volatile long lastStoreTs = 0;

dht DHT;

LiquidCrystal lcd(13, 10, 9, 8, 7, 6);

void setup() {

  pinMode(encoderPinA, INPUT); 
  pinMode(encoderPinB, INPUT_PULLUP); 
  pinMode(encoderPinClk, INPUT); 

  maxHum = EEPROM.read(0);
  minTemp = EEPROM.read(1);
  EEPROM.get(2, flashWrites);

  // set up the LCD's number of columns and rows:
  lcd.begin(8 , 2);
  lcd.clear();
  lcd.print(flashWrites);
  lcd.setCursor(0, 1);
  lcd.print("writes fl");
  delay(2000);
  lcd.clear();
  
  Serial.begin(115200);
  Serial.println("DHT TEST PROGRAM ");
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHT_LIB_VERSION);
  Serial.println();
  Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)");

  attachInterrupt(0, doEncoder, RISING);  // encoder pin on interrupt 0 - pin 2
  attachInterrupt(1, doClk, RISING);  // encoder pin on interrupt 0 - pin 2

}

void storeToFlash() {
  flashWrites++;
  if (flashWrites < 10000) {
    Serial.print("FLASH: write ");
    Serial.println(flashWrites);
    EEPROM.write(0, maxHum);
    EEPROM.write(1, minTemp);
    EEPROM.put(2, flashWrites);
  }
}

void doClk() {
  switch (inputState)
  {
    case INPUT_STATE_IDLE:  
//      log("changing state  idle -> set hum");
      inputState = INPUT_STATE_SET_HUM;
      break;

    case INPUT_STATE_SET_HUM: 
//      log("changing state set hum -> set temp");
      inputState = INPUT_STATE_SET_TEMP;
      break;

    case INPUT_STATE_SET_TEMP: 
//      log("changing state set temp -> idle");
      inputState = INPUT_STATE_IDLE;
      doStore = true;
      lastStoreTs = millis();
      Serial.println("Set doStore = true ");
      break;

    default: 
    break;
  }

}

void doEncoder() {

  switch (inputState)
  {
    case INPUT_STATE_IDLE:  
//      log("state:idle");
      break;

    case INPUT_STATE_SET_HUM: 
      updateHum(digitalRead(encoderPinB)== digitalRead(encoderPinA));
//      Serial.print("state:set hum ");
//      Serial.println(maxHum);
    break;

    case INPUT_STATE_SET_TEMP: 
      updateTemp(digitalRead(encoderPinB)== digitalRead(encoderPinA));
//      Serial.print("state:set temp ");
//      Serial.println(minTemp);
      break;

    default: 
      break;
  }
}

void updateHum(bool direction) {
   if (direction) {
    if (maxHum > 20) maxHum--;
  } else {
    if (maxHum < 90) maxHum++;
  }
}

void updateTemp(bool direction) {
   if (direction) {
      if (minTemp > -5) minTemp--;
  } else {
      if (minTemp < 30) minTemp++;
  }
}

long dhtLastRead = 0;
long dhtLastSuccess = 0;
long mills;

double last_hum = -1;
double last_temp = -1;

int8_t last_maxHum = -1;
int8_t last_minTemp = -1;

bool tick = true;



void loop() {
  mills = millis();
  if (mills > dhtLastRead + 2000) {
    if (DHT.read11(DHT11_PIN) == DHTLIB_OK) {
      dhtLastSuccess = mills;
    }
    dhtLastRead = mills;
    if (last_hum != DHT.humidity || last_temp != DHT.temperature) {
      printCurrentHumTemp(DHT.humidity, DHT.temperature);
      last_hum = DHT.humidity;
      last_temp = DHT.temperature;
    }
  }

  showTargetHumTemp();

  tick = !tick;

  if (doStore) {
    Serial.print("doStore!! ");
    Serial.print(mills);
    Serial.print(" -  ");
    Serial.println(lastStoreTs);
    if (mills > lastStoreTs + 20000) {
      doStore = false;
      lastStoreTs = mills;     
      storeToFlash();
    }
  }
  
  delay(400);
}

void showTargetHumTemp(){
    switch (inputState)
  {
    case INPUT_STATE_IDLE:
      if (last_maxHum != maxHum) {
        printMaxHum(maxHum);
        last_maxHum = maxHum;
      }
      if (last_minTemp != minTemp) {
        printMinTemp(minTemp);
        last_minTemp = minTemp;
      }
      break;

    case INPUT_STATE_SET_HUM: 
      printMinTemp(minTemp);
      if (tick) {
        printMaxHum(maxHum);
      } else {
        printBlankMaxHum();
      }
      break;

    case INPUT_STATE_SET_TEMP:
      printMaxHum(maxHum); 
      if (tick) {
        printMinTemp(minTemp);
      } else {
        printBlankMinTemp();
      }
      break;

    default: 
      break;
  }
}

void printCurrentHumTemp(double hum, double temp) {
  lcd.noCursor();
  lcd.setCursor(0, 0);
  lcd.print(hum,0);
  lcd.print("%");
  lcd.setCursor(4, 0);
  lcd.print(temp,0);
  lcd.print("*C");
}

void printMaxHum(int8_t hum) {
  lcd.noCursor();
  lcd.setCursor(0, 1);
  lcd.print(hum);
  lcd.print("%");
}

void printBlankMaxHum() {
  lcd.setCursor(0, 1);
  lcd.print("    ");
  lcd.setCursor(0, 1);
  lcd.cursor();
}

void printMinTemp(int8_t temp) {
  lcd.noCursor();
  lcd.setCursor(4, 1);
  lcd.print(temp);
  lcd.print("*C");
}

void printBlankMinTemp() {
  lcd.setCursor(4, 1);
  lcd.print("    ");
  lcd.setCursor(4, 1);
  lcd.cursor();
}

void dhtRead(){
//  Serial.print("DHT11, \t");
  int chk = DHT.read11(DHT11_PIN);
  switch (chk)
  {
    case DHTLIB_OK:  
    Serial.print("OK,\t"); 
    break;
    case DHTLIB_ERROR_CHECKSUM: 
    Serial.print("Checksum error,\t"); 
    break;
    case DHTLIB_ERROR_TIMEOUT: 
    Serial.print("Time out error,\t"); 
    break;
    case DHTLIB_ERROR_CONNECT:
        Serial.print("Connect error,\t");
        break;
    case DHTLIB_ERROR_ACK_L:
        Serial.print("Ack Low error,\t");
        break;
    case DHTLIB_ERROR_ACK_H:
        Serial.print("Ack High error,\t");
        break;
    default: 
    Serial.print("Unknown error,\t"); 
    break;
  }
  // DISPLAY DATA
  Serial.print(DHT.humidity, 1);
  Serial.print(",\t");
  Serial.println(DHT.temperature, 1);  
}

void log(int s) {
  if (DEBUG_ENABLED) {
      Serial.print("LOG: ");
      Serial.print(millis());
      Serial.print(" ");
      Serial.println(s);
  }
}

