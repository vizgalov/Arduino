#include <EEPROM.h>
#include <avr/wdt.h>

#include <DallasTemperature.h>
#include <OneWire.h>
#include <dht.h>

#include <LiquidCrystal.h>

#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

#define DEBUG_ENABLED 1

#define DHT11_PIN 17

#define TEMP_SENSOR_PIN 15

#define encoderPinA  2
#define encoderPinB  11
#define encoderPinClk  3

#define LIGHT_PIN 16
#define LIGHT_OUT_PIN 5

#define FAN_OUT_PIN 4

#define INPUT_STATE_IDLE  0
#define INPUT_STATE_SET_HUM  1
#define INPUT_STATE_SET_TEMP  2

#define INPUT_STATE_PARAM_LIGHT_TTL  5
#define INPUT_STATE_PARAM_  6

#define ERR_LOW_TEMP_READ  0x01;

volatile byte inputState = INPUT_STATE_IDLE;

volatile int8_t maxHum = 30;
volatile int8_t minTemp = 5;

volatile boolean doStore = false;
long flashWrites = 1;
volatile long lastStoreTs = 0;

volatile long lastUserActionTs = 0;

dht DHT;

LiquidCrystal lcd(13, 10, 9, 8, 7, 6);

OneWire  ds(TEMP_SENSOR_PIN);  // Номер ноги к которой подключили датчик

static byte t_addr[8] = {0x28, 0xFF, 0x9D, 0x20, 0x4B, 0x04, 0x00, 0xA8};
byte t_data[12];
//    RH_ASK(uint16_t speed = 2000, uint8_t rxPin = 11, uint8_t txPin = 12, uint8_t pttPin = 10, bool pttInverted = false);
RH_ASK driver(2000, 18, 12, 19, false);
boolean rh_enabled = true;

void setup() {

  pinMode(encoderPinA, INPUT); 
  pinMode(encoderPinB, INPUT_PULLUP); 
  pinMode(encoderPinClk, INPUT); 

  pinMode(LIGHT_PIN, INPUT_PULLUP); 

  digitalWrite(LIGHT_OUT_PIN, LOW);
  digitalWrite(FAN_OUT_PIN, LOW);  
  pinMode(LIGHT_OUT_PIN, OUTPUT); 
  pinMode(FAN_OUT_PIN, OUTPUT); 

  restoreFromFlash();

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

  if (!driver.init()) {
    Serial.println(" RH init failed");
    rh_enabled = false;
  }
  
  wdt_enable(WDTO_8S);
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

void restoreFromFlash(){
  maxHum = EEPROM.read(0);
  minTemp = EEPROM.read(1);
  EEPROM.get(2, flashWrites);
 }

void doClk() {
  lastUserActionTs = millis();
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
  lastUserActionTs = millis();
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

long mills;
long dhtLastRead = 0;
long dhtLastSuccess = 0;
long lowTempLastRead = 0;
long showCurrentSw = 0;
long lastTickTs = 0;
long rhLastSendTs = 0;

double lowTemp = -1;

double last_hum = -1;
double last_temp = -1;
double last_lowTemp = -1;

int8_t last_maxHum = -1;
int8_t last_minTemp = -1;

bool tick = true;

bool lightOn = false;
int lightTtl = 0;
int lightTimeout = 10000;

bool showDht = false;
byte error = 0;
byte state = -1;

byte sendBuf[2];

void loop() {
  error = 0;
  mills = millis();

  state = inputState;
  if (state != INPUT_STATE_IDLE && (mills > lastUserActionTs + 10000)) {
    inputState = INPUT_STATE_IDLE;
    restoreFromFlash();
  }

//  Serial.print("p1: ");
//  Serial.println(millis() - mills);

  if (state == INPUT_STATE_IDLE) {
    updateDht();
//    Serial.print("p2: ");
//    Serial.println(millis() - mills);   
    updateLowTemp();
//    Serial.print("p3: ");
//    Serial.println(millis() - mills);   
  }
  showCurrentHumTemp();    
//    Serial.print("p4: ");
//    Serial.println(millis() - mills);   
  showTargetHumTemp();
//    Serial.print("p5: ");
//    Serial.println(millis() - mills);   

  if (DHT.humidity > maxHum && DHT.temperature > minTemp) {
    digitalWrite(FAN_OUT_PIN, HIGH);
  } else {
    digitalWrite(FAN_OUT_PIN, LOW);
  }

updateTick();

//    Serial.print("p6: ");
//    Serial.println(millis() - mills);   

  processLightBtn();
//    Serial.print("p7: ");
//    Serial.println(millis() - mills);   

  checkAndStoreToFlash();
//     Serial.print("p8: ");
//    Serial.println(millis() - mills);   
  
  sendRh(); 
  
  delay(100);
  
  wdt_reset();
}

void sendRh() {
  if (rh_enabled && mills > rhLastSendTs + 5000) {
    rhLastSendTs = mills;
    sendBuf[0] = byte(lowTemp);
    Serial.print("Sending : ");
    Serial.println(sendBuf[0]);       
    driver.send(sendBuf, 1);
    driver.waitPacketSent();
  }
}

void updateTick(){
  if (mills > lastTickTs + 400) {
    tick = !tick;
    lastTickTs = mills;
  }
}


void updateDht(){
  if (mills > dhtLastRead + 2000) {
    if (DHT.read11(DHT11_PIN) == DHTLIB_OK) {
      dhtLastSuccess = mills;
    }
    dhtLastRead = mills;
  }
}

void updateLowTemp() {
  if (mills > lowTempLastRead + 10000) {
    if (readLowTemp(lowTemp) != 0) {
      error = error|ERR_LOW_TEMP_READ;
    }
    lowTempLastRead = mills;
  }
  
}

void showCurrentHumTemp() {
  if (mills > showCurrentSw + 1500) {
    showDht = !showDht;
    showCurrentSw = mills;
    if (showDht) {
      printCurrentHumTemp(DHT.humidity, DHT.temperature, true);
    } else {
      printCurrentHumTemp(DHT.humidity, lowTemp, false);
    }
  }
}

void processLightBtn1() {
  if (digitalRead(LIGHT_PIN) == LOW) {
    if (lightOn && lightTtl < lightTimeout - 10) {
      lightOn = false;
      lightTtl = 10;
      digitalWrite(LIGHT_OUT_PIN, LOW);
    } else if (lightTtl == 0) {
      lightOn = true;
      lightTtl = lightTimeout;
      digitalWrite(LIGHT_OUT_PIN, HIGH);
    }
  } else {
    if (lightOn) {
       if (lightTtl > 0) {
          lightTtl--;
       } else {
          lightOn = false; 
       }
       digitalWrite(LIGHT_OUT_PIN, HIGH);
    } else {
       if (lightTtl > 0) {
          lightTtl--;
       }
       digitalWrite(LIGHT_OUT_PIN, LOW);
    }    
  }
}

boolean lastBtnPressed = false;
boolean btnPressed = false;

void processLightBtn() {
  btnPressed = digitalRead(LIGHT_PIN) == LOW;
  if (lightOn && lightTtl==0) {
     lightOn = false;    
     digitalWrite(LIGHT_OUT_PIN, LOW);
  }
  if (lightOn && btnPressed && !lastBtnPressed) {
     lightOn = false;
     lightTtl = 0;    
     digitalWrite(LIGHT_OUT_PIN, LOW);
  } else   
  if (!lightOn && btnPressed && !lastBtnPressed) {
     lightOn = true;
     lightTtl = lightTimeout;    
     digitalWrite(LIGHT_OUT_PIN, HIGH);
  }
  
  if (lightTtl > 0) {
    lightTtl--;
  }
  lastBtnPressed = btnPressed;
}

byte lastShownStateForTarget;

void showTargetHumTemp(){
  byte s = inputState;
  switch (s)
  {
    case INPUT_STATE_IDLE:
      if (last_maxHum != maxHum || lastShownStateForTarget != INPUT_STATE_IDLE) {
        printMaxHum(maxHum);
        last_maxHum = maxHum;
      }
      if (last_minTemp != minTemp || lastShownStateForTarget != INPUT_STATE_IDLE) {
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
  lastShownStateForTarget = s;
}

void printCurrentHumTemp(double hum, double temp, boolean up) {
  lcd.noCursor();
  lcd.setCursor(0, 0);
  lcd.print(hum,0);
  lcd.print("%");
  lcd.setCursor(4, 0);
  lcd.print(temp,0);
  lcd.print("*C");
  lcd.setCursor(3, 0);
  if (up) {
    lcd.print("^");
  } else {
    lcd.print("v");
  }
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


int8_t readLowTemp(double &temp) {
  byte i;

  ds.reset();
  ds.select(t_addr);
  ds.write(0x44, 1);      

  delay(1000);     

  if (ds.reset() == 0) return -1;
  ds.select(t_addr);    
  ds.write(0xBE);        

  for ( i = 0; i < 9; i++) {          
    t_data[i] = ds.read();
  }

  if (OneWire::crc8(t_data, 8) != t_data[8]) return -2;
 
//  Преобразование данных в фактическую температуру
 
  int16_t raw = (t_data[1] << 8) | t_data[0];

  temp = (double)raw / 16.0;
  return 0;
}

void checkAndStoreToFlash(){
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
}

