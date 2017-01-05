#include <avr/wdt.h>


unsigned int sensorValue1, temp1;
unsigned int sensorValue2, temp2;

#define TEMP_HISTORY_SIZE 256;
//unsigned int tempHistory[256];
//byte tempHistoryIndex = 0;

unsigned int tempTarget = 250;
byte signal = 0;
byte heaterOn = 0;
byte boilerOn = 0;
byte motorOn = 0;

#define SENSOR_PIN_1 A3 
#define SENSOR_PIN_2 A2
#define PWM_PIN 3
#define LED_PIN 4
#define HEATER_IN 5
#define HEATER_OUT 6
#define BOILER_IN 7
#define MOTOR_OUT 8

#define TEMP_TABLE_SIZE 11

static unsigned int tempTable[TEMP_TABLE_SIZE] = {810,765,720,669,608,549,487,431,379,328,284};  

unsigned long lock1 = 0;
unsigned long lock2 = 0;

unsigned int tempHigh = 0;
unsigned int tempLow = 0;

void setup()
{
    wdt_enable(WDTO_2S);
    pinMode(SENSOR_PIN_1, INPUT);
    pinMode(SENSOR_PIN_2, INPUT);
    pinMode(PWM_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BOILER_IN, INPUT);
    pinMode(HEATER_IN, INPUT);
    pinMode(HEATER_OUT, OUTPUT);
    pinMode(MOTOR_OUT, OUTPUT);

    digitalWrite(HEATER_OUT, LOW);
    digitalWrite(MOTOR_OUT, motorOn);

    Serial.begin(9600);           // start serial for output
    Serial.println("Start");         // print the character
    
}

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
  
void decreaseTempTarget() {
  if (tempTarget == 0) return;
  else tempTarget--;
}

void increaseTempTarget() {
  if (tempTarget == 500) return;
  else tempTarget++;
}

unsigned int tmpTemp;

unsigned int readTemp(int pin){
  tmpTemp = 0;
  for (int i=0; i<4; i++){
    tmpTemp = tmpTemp + analogRead(pin);
    delay(1);
  }
    return tempFromVolts(tmpTemp/4);
}


int boilerTmp;
  
int readBoiler() {
  return digitalRead(BOILER_IN);
}

int readHeater() {
  if (digitalRead(HEATER_IN)) 
    return 0; 
  else return 1;
}

void setWalvePosition(byte pos) {
  if (lock1 == 0 && pos != signal) {
    signal = pos;
    analogWrite(PWM_PIN, signal);
    lock1 = 500;
  }
}

void setMotorOn(byte val) {
  if (val == motorOn) return;
  if (lock2 == 0 || val == 1) {
    motorOn = val;
    digitalWrite(MOTOR_OUT, motorOn);
    lock2 = 25;
  }
}

void loop()
{
  wdt_reset();
  
  temp1 = readTemp(SENSOR_PIN_1);
  temp2 = readTemp(SENSOR_PIN_2);


  
  boilerOn = readBoiler();
  heaterOn = readHeater();
  
  setMotorOn(boilerOn || heaterOn);
  
  if (boilerOn && !heaterOn) {
    setWalvePosition(255);
    digitalWrite(LED_PIN, HIGH);
  } else {  
    setWalvePosition(0);
    digitalWrite(LED_PIN, LOW);
  }
  

    Serial.print("temp1=");
    Serial.print(temp1);
    Serial.print(", temp2=");
    Serial.print(temp2);
    Serial.print(", boilerOn=");
    Serial.print(boilerOn);
    Serial.print(", heaterOn=");
    Serial.print(heaterOn);
    Serial.print(", motor=");
    Serial.print(motorOn);
    Serial.print(", signal=");
    Serial.print(signal);
    Serial.print(", lock1=");
    Serial.print(lock1);
    Serial.print(", lock2=");
    Serial.print(lock2);
    Serial.println();

  
  if (lock1>0) {lock1--;}
  if (lock2>0) {lock2--;}
  
  delay(200);
}


  


