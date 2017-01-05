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

long mics[] = {833, 1667, 2500, 3333, 4167, 5000, 5833, 6667, 7500};

byte readData1() {
  long startMic = micros();
  byte out = 0;
  for (int i=0; i<9;i++) {
    byte data = 0;
    byte cnt = 0;
    change();
    while (micros() < startMic + mics[i]) {
     data = data + digitalRead(3);
     cnt++;
    }
    out = out << 1;
    if (data > cnt/2) out++;
  }
  return out;
}

byte readData2() {
  long nextMic = micros() + 833;
  byte out = 0;
  byte bitOut = 0;
  byte cnt = 0;
  byte data = 0;
  while (1) {
    data = data + digitalRead(3);
    cnt++;
    if (micros() > nextMic) {
      byte b = decide(data, cnt);
      out = out << 1;
      if (data > cnt/2) out++;
      if ((bitOut == 0) && (out != 0)) return 0;
      bitOut++;
      cnt = 0;
      data = 0;
      nextMic = nextMic + 833;
      if (bitOut > 8) break;
    }    
  }
  return out;
}


byte readData() {
  long nextMic = micros() + 833;
  byte out = 0;
  byte bitOut = 0;
  byte cnt = 0;
  byte data = 0;
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
      nextMic = nextMic + 833;
      
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

// the loop function runs over and over again forever
byte data, lastData;
void loop() {
//  i = digitalRead(3);
//  digitalWrite(4,i);
//    printByte(7);
  long p = pulseIn(3, HIGH, 10000000L);
  if (p > 0) {
    lastData = data;
    data = readData2();
    if (data  == 0x6C)      digitalWrite( 4, 1);
    else if ( data != 0)    digitalWrite( 4, 0);
  } else {
    digitalWrite( 4, 0);
  }
//  digitalWrite( 4, ! digitalRead( 4 ) );
 // delayMicroseconds(10);              // wait for a second
}
void change() {
   digitalWrite( 4, ! digitalRead( 4 ) );
}

void printByte(byte b) {
    digitalWrite(0, 0);
    for (int i=0;i<8;i++) {
      digitalWrite( 4, bitRead(b,i));
      digitalWrite(0, 1);
      delayMicroseconds(100);
//      b = b >> 1;
      digitalWrite(0, 0);
    }
}

