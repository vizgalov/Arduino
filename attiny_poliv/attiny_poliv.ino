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
  pinMode(0, OUTPUT);
  pinMode(3, INPUT);
  pinMode(4, OUTPUT);
}

long mic = 0;
long ttl = 833;
byte data = 0;
byte cnt =0;

unsigned long 

byte readData() {
  long nextMic = micros() + ttl;
  byte out = 0;
  byte bitOut = 0;
  cnt = 0;
  data = 0;
  while (1) {
    data = data + digitalRead(3);
//    data = data + ((PORTB >> 3) & 0x01);
    cnt++;
    mic = micros();
    if (mic > nextMic) {
      byte b = decide(data, cnt);
      out = out | (b << bitOut);
      bitOut++;
      cnt = 0;
      data = 0;
      nextMic = nextMic + ttl;
      
 //     PORTB = (PORTB & 0x08) | (b << 4);
 //       digitalWrite( 4, b);

      
      if (bitOut > 7) break;
    }    
  }
  return out;
}

byte decide(int data, int cnt) {
  if (data > cnt /2 ) return 1;
  else return 0;
}

int i;
// the loop function runs over and over again forever
void loop() {
//  i = digitalRead(3);
//  digitalWrite(4,i);
  i = pulseIn(3, HIGH, 1000000L);
  if (i > 0) {
    byte d = readData();
 // digitalWrite( 4, ! digitalRead( 4 ) );
    if (d  == 0x02)     digitalWrite( 4, 1);
    else       digitalWrite( 4, 0);
  }
 /*
    for (int i=0;i<8;i++) {
      digitalWrite( 4, 0);
      digitalWrite( 4, d >> (7-i));
      
    }
    */
//  digitalWrite( 4, ! digitalRead( 4 ) );
 // delayMicroseconds(10);              // wait for a second
}


