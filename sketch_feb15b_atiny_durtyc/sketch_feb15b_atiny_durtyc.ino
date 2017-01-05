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

int cnt1;
int cnt0;
int cnt;
int val;
int dc;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, INPUT);
  pinMode(1, INPUT);
  pinMode(0, INPUT);
  pinMode(2, INPUT);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
}

void show(int n) {
  digitalWrite(4, HIGH);
  digitalWrite(4, LOW);
  while (n--) {
    digitalWrite(3, HIGH);
    digitalWrite(3, LOW);
    
  }
}

// the loop function runs over and over again forever
void loop() {
       // wait for a second
  val = digitalRead(0);
  if (val == HIGH) {
    cnt1++;
  } else {
    cnt0++;
  }  
  cnt++;
  if (cnt>20) {
    dc = cnt1 / 2;
    show(dc % 10);
    cnt = 0;
    cnt0 = 0;
    cnt1 = 0;
  }
 
  // val содержит значение из диапазона 0..1023, а диапазон значений для analogWrite
  // 0..255. Для этого делим val на 4
//  digitalWrite(3, val);
//  delay(1000);
//  digitalWrite(3, val2);
//  delay(1000);
  delay(300); 


}


