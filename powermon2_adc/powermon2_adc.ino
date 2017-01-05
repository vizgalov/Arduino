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


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(3, OUTPUT);
  
  ACSR  |=  (1<<ACI);    // clear Analog Comparator interrupt
  ACSR  |=
        (0<<ACD)   |         // Comparator ON
        (0<<ACBG)  |         // Disconnect 1.23V reference from AIN0 (use AIN0 and AIN1 pins)
        (0<<ACIE)  |         // Comparator Interrupt enabled
        (0<<ACIC)  |         // input capture disabled
        (0<<ACIS1) |         // set interrupt bit on rising edge
        (0<<ACIS0);          //    (ACIS1 and ACIS0 == 11)
   // Enable the interrupt
  //ACSR = ACSR | (1 << ACIE);
}

// the loop function runs over and over again forever
void loop() {
  
  if ( (ACSR & (1 << ACO))== 0 ) {
    digitalWrite(3, HIGH);   // turn the LED on (HIGH is the voltage level)
  } else {
    digitalWrite(3, LOW);    // turn the LED off by making the voltage LOW
  }
              // wait for a second
  delay(100);              // wait for a second
}
