#define encoder0PinA  19
#define encoder0PinB  4

volatile int pos = 0;
volatile int cnt = 0;
int last_pos = 0;
int last_cnt = 0;
int vals[4];
int last_mills = 0;

int val = 1;
byte b = 1;
byte b1 = 0;
byte b2 = 0;
byte b3 = 0;
byte b4 = 0;
int i;
byte dig[] = {123, 65, 55, 87, 77, 94, 126, 67, 127, 95};
int byteToDigs[255];




void setup() { 


  pinMode(encoder0PinA, INPUT); 
  pinMode(encoder0PinB, INPUT); 
  pinMode(18, INPUT);
  initByteToDigs();
  // initialize digital pin 13 as an output.
  for (int i=22; i <= 38; i++){
      pinMode(i, OUTPUT);
   } 
  for (int i=42; i <= 46; i++){
      pinMode(i, OUTPUT);
   } 

  attachInterrupt(4, doEncoder, RISING);  // encoder pin on interrupt 0 - pin 2
  attachInterrupt(5, doClk, RISING);  // encoder pin on interrupt 0 - pin 2
  Serial.begin (9600);
  Serial.println("start");                // a personal quirk
 
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

void loop(){
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
  //      writeDigs(b4);
        break;
    }
    setDrive(i );
    delayMicroseconds(1500);
  }
/*
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
*/
//  delay(1);
  int p = pos;
  int c = cnt;
  if (p != last_pos || c != last_cnt) {
    last_pos = p;
    last_cnt = c;
 //   Serial.print (p, DEC);
 //   Serial.print ("   ");
 //   Serial.println (c, DEC);
  b1 = c % 100;
  b2 = (c / 100) % 100;
  b3 = p % 100;
  b4 = (p / 100) % 100;  
  }
}

void doClk() {
  pos = 0;
  cnt = 0;
}

void doEncoder() {
  /* If pinA and pinB are both high or both low, it is spinning
   * forward. If they're different, it's going backward.
   *
   * For more information on speeding up this process, see
   * [Reference/PortManipulation], specifically the PIND register.
   */
//  int m = millis();
//  if (last_mills == m) {
//    return;
//  } 
//  last_mills = m;
  cnt++; 
  if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB)) {
    pos--;
  } else {
    pos++;
  }

}

 
/*  to read the other two transitions - just use another attachInterrupt()
in the setup and duplicate the doEncoder function into say, 
doEncoderA and doEncoderB. 
You also need to move the other encoder wire over to pin 3 (interrupt 1). 
*/ 
