#define DEBUG_SERIAL 1
//#define DEBUG_TRACING 1

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

#define ALERT_PIN 14

#define INPUT_STATE_IDLE  0
#define INPUT_STATE_SET_HUM  1
#define INPUT_STATE_SET_TEMP  2

#define INPUT_STATE_PARAM_LIGHT_TTL  5
#define INPUT_STATE_PARAM_  6

#define ERR_LOW_TEMP_READ  0x01;
#define ERR_DHT_READ 0x02;

#define PROG_STATE_OFF  0
#define PROG_STATE_IDLE  1
#define PROG_STATE_SET_LIGHT_TTL  2
#define PROG_STATE_SET_RH_ENABLED  3
#define PROG_STATE_SET_RH_PERIOD  4
#define PROG_STATE_SET_ALWAYS_FAN  5
#define PROG_STATE_SET_LIGHT_OR_FAN  6

volatile byte inputState = INPUT_STATE_IDLE;
volatile byte progState = PROG_STATE_OFF;

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
long rh_period = 5;
int lightTimeout = 2;
boolean alwaysFan = false;
boolean lightOrFan = false;

void setup() {
  
  pinMode(encoderPinA, INPUT); 
  pinMode(encoderPinB, INPUT_PULLUP); 
  pinMode(encoderPinClk, INPUT); 

  pinMode(LIGHT_PIN, INPUT_PULLUP); 

  digitalWrite(LIGHT_OUT_PIN, LOW);
  digitalWrite(FAN_OUT_PIN, LOW);  
  pinMode(LIGHT_OUT_PIN, OUTPUT); 
  pinMode(FAN_OUT_PIN, OUTPUT);   
  pinMode(ALERT_PIN, OUTPUT); 

  restoreFromFlash();

  // set up the LCD's number of columns and rows:
  lcd.begin(8 , 2);
  lcd.clear();
  lcd.print(flashWrites);
  lcd.setCursor(0, 1);
  lcd.print("writes fl");
  alert();
  delay(1000);
  noAlert();
  lcd.clear();
  
  
  Serial.begin(115200);
  Serial.println("DHT TEST PROGRAM ");
 
//  if (digitalRead(encoderPinClk) == LOW && digitalRead(LIGHT_PIN) == LOW) {
  if (digitalRead(LIGHT_PIN) == LOW) {
    progState = PROG_STATE_IDLE;
    attachInterrupt(0, doEncoderProg, RISING);  // encoder pin on interrupt 0 - pin 2
    attachInterrupt(1, doClkProg, RISING);  // encoder pin on interrupt 0 - pin 2
  } else {
    attachInterrupt(0, doEncoder, RISING);  // encoder pin on interrupt 0 - pin 2
    attachInterrupt(1, doClk, RISING);  // encoder pin on interrupt 0 - pin 2
    if (!driver.init()) {
      lcd.setCursor(0, 0);
      lcd.print("RH failed");
      delay(1000);
      Serial.println(" RH init failed");
      rh_enabled = false;
    }
  }
  
  wdt_enable(WDTO_8S);
}

void alert(){
  digitalWrite(ALERT_PIN, HIGH);
}

void noAlert(){
  digitalWrite(ALERT_PIN, LOW);
}

void storeToFlash() {
  flashWrites++;
  alert();
  delay(300);
  noAlert();
  if (flashWrites < 10000) {
    Serial.print("FLASH: write ");
    Serial.println(flashWrites);
    EEPROM.write(0, maxHum);
    EEPROM.write(1, minTemp);
    EEPROM.put(2, flashWrites);

    EEPROM.put(10, lightTimeout);
    EEPROM.put(12, rh_enabled);
    EEPROM.put(13, rh_period);
    EEPROM.put(17, alwaysFan);
    EEPROM.put(18, lightOrFan);  
  }
  alert();
  delay(300);
  noAlert();
}

void restoreFromFlash(){
  maxHum = EEPROM.read(0);
  minTemp = EEPROM.read(1);
  EEPROM.get(2, flashWrites);
  EEPROM.get(10, lightTimeout);  
  EEPROM.get(12, rh_enabled);  
  EEPROM.get(13, rh_period);  
  EEPROM.get(17, alwaysFan);  
  EEPROM.get(18, lightOrFan);  
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

void doClkProg() {
//  lastUserActionTs = millis();
  switch (progState)
  {
    case PROG_STATE_IDLE:  
      progState = PROG_STATE_SET_LIGHT_TTL;
      break;

    case PROG_STATE_SET_LIGHT_TTL: 
      progState = PROG_STATE_SET_RH_ENABLED;
      break;

    case PROG_STATE_SET_RH_ENABLED: 
      progState = PROG_STATE_SET_RH_PERIOD;
      break;

    case PROG_STATE_SET_RH_PERIOD: 
      progState = PROG_STATE_SET_ALWAYS_FAN;       
      break;

    case PROG_STATE_SET_ALWAYS_FAN: 
      progState = PROG_STATE_SET_LIGHT_OR_FAN;
      break;    
      
    case PROG_STATE_SET_LIGHT_OR_FAN: 
      progState = PROG_STATE_IDLE;
      doStore = true;
      break;

    default: 
    break;
  }

}

void doEncoderProg() {
  switch (progState)
  {
    case PROG_STATE_IDLE:  
//      log("state:idle");
      break;

    case PROG_STATE_SET_LIGHT_TTL: 
      updateLightTtl(digitalRead(encoderPinB)== digitalRead(encoderPinA));
    break;

    case PROG_STATE_SET_RH_ENABLED: 
      updateRhEnabled(digitalRead(encoderPinB)== digitalRead(encoderPinA));
      break;

    case PROG_STATE_SET_RH_PERIOD: 
      updateRhPeriod(digitalRead(encoderPinB)== digitalRead(encoderPinA));
      break;
      
    case PROG_STATE_SET_ALWAYS_FAN: 
      updateAlwaysFan(digitalRead(encoderPinB)== digitalRead(encoderPinA));
      break;  
          
    case PROG_STATE_SET_LIGHT_OR_FAN: 
      updateLightOrFan(digitalRead(encoderPinB)== digitalRead(encoderPinA));
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


bool showDht = false;
byte error = 0;
byte state = -1;

int8_t dht_err = 0;
int8_t lowTemp_err = 0;


void loop() {
  mills = millis();

  if (progState == PROG_STATE_OFF) {
      state = inputState;
      if (state != INPUT_STATE_IDLE && (mills > lastUserActionTs + 10000)) {
        inputState = INPUT_STATE_IDLE;
        restoreFromFlash();
      }
      
      if (state == INPUT_STATE_IDLE) {
        dht_err = updateDht(); 
        lowTemp_err = updateLowTemp();
      }
      showCurrentHumTemp();    
      showTargetHumTemp();
            
      updateFan();
    
      if (state == INPUT_STATE_IDLE) {
        sendRh(); 
      }
  } else {
    showProgSettings();  
  }
  
  updateTick();
    
  processLightBtn();

  checkAndStoreToFlash();

  if (error == 0) {
    noAlert();
  } else {
    alert();  
  }
  
  processingFinished();
      
  delay(100);
      
  wdt_reset();
}

byte sendBuf[6];
void sendRh() {
  #if DEBUG_TRACING
  Serial.print("sendRh start: ");
  Serial.println(millis() - mills);
  #endif

  if (rh_enabled && mills > rhLastSendTs + rh_period*1000) {
    rhLastSendTs = mills;
    sendBuf[0] = byte(DHT.humidity);
    sendBuf[1] = byte(DHT.temperature);
    sendBuf[2] = byte(lowTemp);
    sendBuf[3] = byte(error);
    sendBuf[4] = OneWire::crc8(sendBuf, 4);
    Serial.print("Sending : ");
    Serial.print(sendBuf[0]);       
    Serial.print(" ");       
    Serial.print(sendBuf[1]);       
    Serial.print(" ");       
    Serial.print(sendBuf[2]);       
    Serial.print(" ");       
    Serial.print(sendBuf[3]);       
    Serial.print(" ");       
    Serial.print(sendBuf[4]);       
    Serial.println();       
    driver.send(sendBuf, 5);
    driver.waitPacketSent();
  }
}

void updateTick(){
  if (mills > lastTickTs + 400) {
    tick = !tick;
    lastTickTs = mills;
  }
}



void updateFan() {

  if (alwaysFan) {
    digitalWrite(FAN_OUT_PIN, HIGH);
    return;
  }

  if (lightOrFan && lightOn) {
    digitalWrite(FAN_OUT_PIN, LOW);
    return;
  }
  
  checkStaleFan();

  if (dht_err != 0 && lowTemp_err != 0) return;
  
  if (dht_err != 0 && lowTemp_err == 0) {
    if (lowTemp < minTemp) setFan(false);
    return;
  }
  
  if (dht_err == 0 && lowTemp_err == 0) {
    if (DHT.temperature > minTemp && lowTemp > minTemp) {
      if (DHT.humidity > maxHum + 1) setFan(true);
      if (DHT.humidity < maxHum - 1) setFan(false);
    } else {
      setFan(false);
    }
  }
  
  if (dht_err == 0 && lowTemp_err != 0) {
    if (DHT.temperature > minTemp) {
      if (DHT.humidity > maxHum + 1) setFan(true);
      if (DHT.humidity < maxHum - 1) setFan(false);
    } else {
      setFan(false);    
    }
  }

}

long lastFanOn = 0;
long lastFanOff = 0;

void setFan(boolean on) {
  if (on) {
    if (mills > lastFanOff + 30000) {
      lastFanOn = mills;
      digitalWrite(FAN_OUT_PIN, HIGH);
    }
  } else {
    if (mills > lastFanOn + 30000) {
      lastFanOff = mills;
      digitalWrite(FAN_OUT_PIN, LOW);
    }
  }  
}

void checkStaleFan(){
  if (mills > lastFanOn + 36000000) {
    setFan(true);
  }  
}

int8_t updateDht(){
  #if DEBUG_TRACING
  Serial.print("updateDht start: ");
  Serial.println(millis() - mills);
  #endif
  if (mills > dhtLastRead + 2000) {
    dhtLastRead = mills;
    if (DHT.read11(DHT11_PIN) == DHTLIB_OK) {
      dhtLastSuccess = mills;
      error = error&~ERR_DHT_READ;
      return 0;
    } else {
      error = error|ERR_DHT_READ;
      return ERR_DHT_READ;      
    }
  }
}

int8_t updateLowTemp() {
  #if DEBUG_TRACING
  Serial.print("updateLowTemp start: ");
  Serial.println(millis() - mills);
  #endif  
  if (mills > lowTempLastRead + 10000) {
    lowTempLastRead = mills;
    if (readLowTemp(lowTemp) != 0) {
      error = error|ERR_LOW_TEMP_READ;
      return ERR_LOW_TEMP_READ;
    } else {
      error = error&~ERR_LOW_TEMP_READ;
      return 0;
    }
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


boolean lastBtnPressed = false;
boolean btnPressed = false;
long  lastBtnProcessed = 0;

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
    if (mills > lastBtnProcessed + 60000) {
      lightTtl--;
      lastBtnProcessed = mills;
    }
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

  if (ds.reset() == 0) {
    Serial.println("LowTemp DS device not found");
    return -1;
  }
  ds.select(t_addr);    
  ds.write(0xBE);        

  for ( i = 0; i < 9; i++) {          
    t_data[i] = ds.read();
  }

  if (OneWire::crc8(t_data, 8) != t_data[8]) {
    Serial.println("LowTemp DS device CRC error");
    return -2;
  }
 
//  Преобразование данных в фактическую температуру
 
  int16_t raw = (t_data[1] << 8) | t_data[0];

  temp = (double)raw / 16.0;
  return 0;
}

void checkAndStoreToFlash(){
  if (doStore) {
//    Serial.print(mills);
//    Serial.print(" -  ");
//    Serial.println(lastStoreTs);
    if (mills > lastStoreTs + 20000) {
      Serial.print("doStore!! ");
      doStore = false;
      lastStoreTs = mills;     
      storeToFlash();
    }
  }  
}

void updateLightTtl(bool direction) {
   if (direction) {
    if (lightTimeout > 0) lightTimeout--;
  } else {
    if (lightTimeout < 30000) lightTimeout++;
  }
}

void updateRhEnabled(bool direction) {
  rh_enabled = !rh_enabled;
}

void updateAlwaysFan(bool direction) {
  alwaysFan = !alwaysFan;
}

void updateLightOrFan(bool direction) {
  lightOrFan = !lightOrFan;
}

void updateRhPeriod(bool direction) {
   if (direction) {
    if (rh_period > 0) rh_period--;
  } else {
    if (rh_period < 100) rh_period++;
  }
}
void processingFinished(){
  #if DEBUG_TRACING
  Serial.print("Finished: ");
  Serial.println(millis() - mills);
  #endif
}


void showProgSettings(){
  byte s = progState;
  switch (s)
  {
    case PROG_STATE_IDLE:
      printProgSetting("PROG");
      break;

    case PROG_STATE_SET_LIGHT_TTL: 
      printProgSetting("LGHT TTL", lightTimeout);
      break;

    case PROG_STATE_SET_RH_ENABLED:
      printProgSetting("RH ENABL", rh_enabled);
      break;

    case PROG_STATE_SET_RH_PERIOD:
      printProgSetting("RH PERIO", rh_period);
      break;
      
    case PROG_STATE_SET_ALWAYS_FAN:
      printProgSetting("ALW FAN", alwaysFan);
      break;
    
    case PROG_STATE_SET_LIGHT_OR_FAN:
      printProgSetting("LGHT!=FN", lightOrFan);
      break;
            
    default: 
      break;
  }
}

void printProgSetting(char name[]) {
  lcd.clear();
  lcd.noCursor();
  lcd.setCursor(0, 0);
  lcd.print(name);
}
void printProgSetting(char name[], double value) {
  lcd.clear();
  lcd.noCursor();
  lcd.setCursor(0, 0);
  lcd.print(name);
  lcd.setCursor(0, 1);
  if (tick) {
    lcd.print(value,0);
  } else {
    lcd.cursor();
  }  
}
void printProgSetting(char name[], int value) {
  lcd.clear();
  lcd.noCursor();
  lcd.setCursor(0, 0);
  lcd.print(name);
  lcd.setCursor(0, 1);
  if (tick) {
    lcd.print(value);
  } else {
    lcd.cursor();
  }  
}
void printProgSetting(char name[], boolean value) {
  lcd.clear();
  lcd.noCursor();
  lcd.setCursor(0, 0);
  lcd.print(name);
  lcd.setCursor(0, 1);
  if (tick) {
    lcd.print(value);
  } else {
    lcd.cursor();
  }  
}
void printProgSetting(char name[], long value) {
  lcd.clear();
  lcd.noCursor();
  lcd.setCursor(0, 0);
  lcd.print(name);
  lcd.setCursor(0, 1);
  if (tick) {
    lcd.print(value);
  } else {
    lcd.cursor();
  }  
}
void printProgSetting(char name[], byte value) {
  lcd.clear();
  lcd.noCursor();
  lcd.setCursor(0, 0);
  lcd.print(name);
  lcd.setCursor(0, 1);
  if (tick) {
    lcd.print(value);
  } else {
    lcd.cursor();
  }  
}
