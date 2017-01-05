

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, INPUT);
//  pinMode(4, OUTPUT);

}

void bl(int pin) {
  digitalWrite(pin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(5000);              // wait for a second
  digitalWrite(pin, LOW);    // turn the LED off by making the voltage LOW
  delay(5000);              // wait for a second

}

// the loop function runs over and over again forever
void loop() {
  bl(0);
//  bl(2);
 // bl(4);
}
