/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */

int val;
int val2;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(3, OUTPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(1, INPUT);
  pinMode(0, INPUT);
  pinMode(2, INPUT);
}

// the loop function runs over and over again forever
void loop() {
       // wait for a second
  val = digitalRead(4);   
  val2 = digitalRead(5);   
 
  // val содержит значение из диапазона 0..1023, а диапазон значений для analogWrite
  // 0..255. Для этого делим val на 4
//  digitalWrite(3, val);
//  delay(1000);
//  digitalWrite(3, val2);
//  delay(1000);
  digitalWrite(3, HIGH);
  delay(200);
  digitalWrite(3, LOW);
  delay(200); 
  digitalWrite(3, digitalRead(0));
  delay(1000);
  digitalWrite(3, digitalRead(1));
  delay(1000);
  digitalWrite(3, digitalRead(2));
  delay(1000);
  digitalWrite(3, digitalRead(4));
  delay(1000);
  digitalWrite(3, digitalRead(5));
  delay(1000);
//  digitalWrite(3,HIGH);
//  delay(1000);
//  digitalWrite(3,LOW);
//  delay(1000);


}


