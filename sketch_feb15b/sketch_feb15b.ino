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

int vals[4];

int val = 1;
byte b = 1;
byte b1 = 0;
byte b2 = 0;
byte b3 = 0;
byte b4 = 0;
int i;
byte dig[] = {123, 65, 55, 87, 77, 94, 126, 67, 127, 95};
int byteToDigs[255];

// the setup function runs once when you press reset or power the board
void setup() {
  initByteToDigs();
  // initialize digital pin 13 as an output.
  for (int i=22; i <= 38; i++){
      pinMode(i, OUTPUT);
   } 
  for (int i=42; i <= 46; i++){
      pinMode(i, OUTPUT);
   } 
   
}

void initByteToDigs(){
  int num = 0;
  int twoDigs;
  for (int i=0;i<10;i++) {
    for (int j=0;j<10;j++) {
      twoDigs = dig[j];
      twoDigs = (twoDigs << 8) + dig[i];
      byteToDigs[num] = twoDigs;
      num++;
    }
  }
}

void writeDigs(byte b) {
  int d = byteToDigs[b];
  PORTC = highByte(d);
  PORTA = lowByte(d);
}

void displayInt(int num, int pos) {
  displayLowByte(lowByte(num));
  displayHighByte(highByte(num));
  setDrive(pos);
} 

void displayLowByte(byte b) {
  for (int i=0; i < 7; i++) {
    digitalWrite(i + 22, bitRead(b,i));
  }
}

void displayHighByte(byte b) {
  for (int i=0; i < 7; i++) {
    digitalWrite(i + 29, bitRead(b,i));
  }
}

void setDrive(int d) {
    digitalWrite(42, bitRead(d,0));
    digitalWrite(43, bitRead(d,1));
    digitalWrite(44, bitRead(d,2));
    digitalWrite(45, bitRead(d,3));
  
}

// the loop function runs over and over again forever
void loop() {
//  val = 1;
  for (int i=0; i < 4;i++) {
    setDrive(7);
    switch (i) {
      case 0:
        writeDigs(b1);
        break;
      case 1:
        writeDigs(b2);
        break;
      case 2:
        writeDigs(b3);
        break;
      case 3:
        writeDigs(b4);
        break;
      default: 
        writeDigs(b4);
        break;
    }
    setDrive(i );
  //  PORTA = digs[val % 10];
  //  PORTC = digs[val % 10]; 
    //displayInt(val,i);
 //   delay(300);
    delayMicroseconds(500);
  }
 // i++;
//  if (i>50) {
//    i = 0;
//    val++;
//  }
//  b++;
//  val = val + 1;
//  b++;
  b1++;
  if (b1 > 99) {
    b1 = 0;
    b2++;
  }
  if (b2 > 99) {
    b2 = 0;
    b3++;
  }
  if (b3 > 99) {
    b3 = 0;
    b4++;
  }
  if (b4 > 99) {
    b4 = 0;
  }
//  delay(500);
}


