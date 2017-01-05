#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile

RH_ASK driver;

int trigPin = 0;    //Trig - green Jumper
int echoPin = 1;    //Echo - yellow Jumper
int vccPin = 2;    //Echo - yellow Jumper
long distance, lastDistance;
long limit = 5; 
 
void setup()
{
   pinMode(trigPin, OUTPUT);
   pinMode(echoPin, INPUT);
   pinMode(vccPin, OUTPUT);
   pinMode(9, OUTPUT);
   pinMode(10, OUTPUT);
   pinMode(13, OUTPUT);
   pinMode(5, OUTPUT);

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


unsigned long tm;

long duration, cm, inches;

void checkEcho() {
  if (digitalRead(echoPin)) {
    digitalWrite(vccPin, LOW);
    delay(20);
    digitalWrite(vccPin, HIGH);
  }  
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

void setSpeed(uint8_t s) {
    if (distance < 5) {
      analogWrite(5, s); 
      analogWrite(6, 0); 
      digitalWrite(9, HIGH);
    } else {
      analogWrite(5, s); 
      analogWrite(6, s); 
      digitalWrite(9, LOW);
    }
    
}

uint8_t buf[2];
uint8_t buflen = 1;

uint8_t speed = 0;


void loop()
{
//    checkEcho();
    lastDistance = distance;
    distance = getDistance();

    if (driver.recv(buf, &buflen)) // Non-blocking
    {
       speed = buf[0];
       setSpeed(speed);
    }
//      setSpeed(speed);
     
    if ((distance < limit) !=  (lastDistance < limit)) {
      setSpeed(speed);
    }
    
    unsigned long mil = millis();
    if (mil - tm > 1000) {
      tm = mil;
      digitalWrite(10,!digitalRead(10));
    }
  delay(127);
}


