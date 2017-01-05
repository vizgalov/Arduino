#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


void setup() {
  // initialize digital pin 13 as an output.
 // pinMode(0, OUTPUT);
 // pinMode(2, OUTPUT);
 // pinMode(3, INPUT);
  DDRB = 0x7; 
//  pinMode(1, OUTPUT);
  GIMSK |=  (1<<PCIE);		// Enable pin change interrupt for PORTB
  PCMSK =  0x08;	    		// Enable pin change interrupt for PB0 (Pin 5) 
  SREG |= ( 1 << SREG_I ); 	// Enable global interrupts 
}



volatile long mic = 0;
long lastMic = 0;
long t = 833;

byte state = 0;
//static byte positions[] = {0x00, 0x01, 0x03, 0x07, 0xF, 0x1F, 0x3F, 0x7F, 0xFF};
//byte pos = 0;
byte byteOut = 0;
byte cnt;
//static volatile byte vals[9];
//byte vcnt;

//long val = 0;

ISR (PCINT0_vect)
{
  mic = micros();
  GIFR |= ( 1 << PCIF ); 	// clear interrupt flag
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
      out = out << 1;
      if (data > cnt/2) out++;
 //     if ((bitOut == 0) && (out != 0)) return 0;
      bitOut++;
      cnt = 0;
      data = 0;
      nextMic = nextMic + 833;
      if (bitOut > 8) break;
    }    
  }
  return out;
}

  long tn;

  void loop() {
    if (mic != lastMic) {
      byte d = PINB & 0x8;
      if ((mic - lastMic > 20000) && !(d)) {
        state = 1;
        cbi(PORTB,0);
      } else {

        switch (state) {
          case 1:
            t = (mic - lastMic) >> 3;
//          printByte(t>>8);
            state = 2;
            cbi(PORTB,0);
            break;
          case 2: 
            state = 3;
            cnt = 0;
            sbi(PORTB,0);
  //          GIMSK |=  (0<<PCIE);		// Enable pin change interrupt for PORTB
  //          byteOut = readData2();
  //          cbi(PORTB,0);
  //          GIMSK |=  (1<<PCIE);		// Enable pin change interrupt for PORTB
///        printByte(byteOut);
            break;
          case 3: // start
  //        byteOut = readData2();
  //        printByte(byteOut);
            tn = lastMic;
            for (byte i=0; i <9; i++) {
              cbi(PORTB,0);
              cnt++;
              sbi(PORTB,0);
              tn = tn + 833;
              byteOut = byteOut <<1;
              if (tn + 400 > mic) break;
            } 
            if (cnt > 8) {
              state = 0;
              cbi(PORTB,0);
            }
           

//          pos = (mic - lastMic + 400)/t;
 //         val = (val <<4)| pos;
//          vals[vcnt] = pos;
            break;
          default:
 //         digitalWrite(0,0);
            cbi(PORTB,0);
            break; 
        }
    
      }
    }

    lastMic = mic;
    delayMicroseconds(50);
  }
//  digitalWrite( 4, ! digitalRead( 4 ) );
/*
void change(char pin) {
   digitalWrite( pin, ! digitalRead( pin ) );
}
*/
void printByte(byte b) {
    cbi(PORTB,0);
    for (int i=0;i<8;i++) {
      cbi(PORTB,0);
      if (bitRead(b,i)) {
        sbi(PORTB,0);
        sbi(PORTB,0);
        sbi(PORTB,0);
      } else {
        cbi(PORTB,0);
        sbi(PORTB,0);
        cbi(PORTB,0);
      }
      cbi(PORTB,0);
      

//      digitalWrite(0, 1);
//      delayMicroseconds(100);
//      b = b >> 1;
    }
}
/*
ISR (PCINT0_vect9)
{
  GIMSK |=  (0<<PCIE);		// Enable pin change interrupt for PORTB
  mic = micros();
  byte d = PINB & 0x8;
//  printByte(d);

  if ((mic - lastMic > 20000) && !(d)) {
    state = 1;
//    sbi(PORTB,0);
  } else {

  switch (state) {
    case 1:
      t = (mic - lastMic) >> 3;
//      printByte(t>>8);
//      printByte(t>>8);
      state = 2;
      cbi(PORTB,0);
 //     if (d != 0) state = 2;
 //     else state = 0;
      break;
    case 2: 
      state = 3;
      cnt = 0;
      vcnt = 0;
 //     val = 0;
      sbi(PORTB,0);
//      else state = 0;
      break;
    case 3: // start
//    change(0);
//      digitalWrite(0,1);
      pos = (mic - lastMic + 400)/t;
 //     val = (val <<4)| pos;
//     vals[vcnt] = pos;
      vcnt++;
//      printByte(pos);
//      if (pos < 8 && pos > 0) {
//       byteOut = byteOut << pos;
//        if (d) byteOut = byteOut | positions[pos];
        cnt = cnt + pos;
//      }
//      sbi(PORTB,0);
      if (cnt > 8) {
        state = 0;
        cbi(PORTB,0);
 //       change(0);
      } else {
//        state = 0;
      }
      break;
    default:
 //     digitalWrite(0,0);
      cbi(PORTB,0);
      break; 
  }
  
  }
//  digitalWrite( 0, digitalRead(3));
//  PORTB = PORTB&0xFE | PINB >> 3;


  lastMic = mic;

  GIFR |= ( 1 << PCIF ); 	// clear interrupt flag
  GIMSK |=  (1<<PCIE);		// Enable pin change interrupt for PORTB
}
*/
