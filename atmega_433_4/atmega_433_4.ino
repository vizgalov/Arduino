#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile

RH_ASK driver;

int trigPin = 0;    //Trig - green Jumper
int echoPin = 1;    //Echo - yellow Jumper
int vccPin = 2;    //Echo - yellow Jumper
long distance, lastDistance;
long limit = 9; 
 
void setup()
{
   pinMode(trigPin, OUTPUT);
   pinMode(echoPin, INPUT);
   pinMode(vccPin, OUTPUT);

   pinMode(5, OUTPUT);
   pinMode(6, OUTPUT);
   pinMode(7, OUTPUT);
   pinMode(8, OUTPUT);

   pinMode(9, OUTPUT);
   pinMode(10, OUTPUT);
   pinMode(13, OUTPUT);

   pinMode(A3, INPUT_PULLUP);
   pinMode(A4, INPUT);
   pinMode(A5, INPUT); 
   
   
   
   digitalWrite(vccPin, HIGH);
   
   if (!driver.init()) {
      digitalWrite(9,1);
      digitalWrite(10,1);
      delay(1000);
      digitalWrite(9,0);
      digitalWrite(10,0);
      delay(1000);
      digitalWrite(9,1);
      digitalWrite(10,1);
      delay(1000);
      digitalWrite(9,0);
      digitalWrite(10,0);
   }
   
   digitalWrite(9,1);
   digitalWrite(10,1);
   delay(500);
   digitalWrite(9,0);
   digitalWrite(10,0);

}

uint8_t getOvrSpeed() {
  if (!digitalRead(A5)) return 0x77;
  else return 0;
}


unsigned long tm;

long duration, cm, inches;

bool checkEcho() {
  if (digitalRead(A4)) 
    return false;
  if (digitalRead(echoPin)) {
    digitalWrite(vccPin, LOW);
    delay(200);
    digitalWrite(vccPin, HIGH);
    return true;
  }
  return false;  
}

long getDistance() {
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
//  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH, 35000);
 
  // convert the time into a distance
  return (duration/2) / 74; 
}

uint8_t leftSpeed, rightSpeed;

void setSpeed(uint8_t s) {
    leftSpeed = (s << 1) & 0xF0;
    rightSpeed = (s << 5) & 0xF0;
        
    if (distance < limit) {
      digitalWrite(9, HIGH);
      if (distance != 0) {
        analogWrite(5, 0); 
        analogWrite(6, 0); 
        digitalWrite(7, HIGH);
        digitalWrite(8, LOW);
      }
    } else {
      analogWrite(5, leftSpeed); 
      analogWrite(6, rightSpeed); 
      if (bitRead(s,3)) 
        digitalWrite(7, HIGH);
      else 
        digitalWrite(7, LOW);
      if (bitRead(s,7)) 
        digitalWrite(8, HIGH);
      else 
        digitalWrite(8, LOW);
      digitalWrite(9, LOW);
    }
    
}

uint8_t buf[2];
uint8_t buflen = 1;

uint8_t speed = 0;


void loop()
{
    delay(127);

    if (checkEcho()) {
       setSpeed(speed);      
    }
    
    lastDistance = distance;
    distance = getDistance();

    if (getOvrSpeed() != 0) {
      speed = getOvrSpeed();  
    }
    
    if (driver.recv(buf, &buflen)) // Non-blocking
    {
       digitalWrite(10,!digitalRead(10));
       speed = buf[0];
       setSpeed(speed);
    }

  if (!digitalRead(A3)) setSpeed(speed);

    if ((distance < limit) !=  (lastDistance < limit)) {
      setSpeed(speed);
    }
    
    unsigned long mil = millis();
    if (mil - tm > 1000) {
      tm = mil;
      digitalWrite(10,!digitalRead(10));
    }
}


