#include <avr/wdt.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define PIN0_ADDDR 0x36
#define PIN2_ADDDR 0x4F

void setup() {
  wdt_enable(WDTO_4S);
  DDRB = 0x7; 
  GIMSK |=  (1<<PCIE);		// Enable pin change interrupt for PORTB
  PCMSK =  0x08;	    		// Enable pin change interrupt for PB0 (Pin 5) 
  SREG |= ( 1 << SREG_I ); 	// Enable global interrupts 
}



unsigned long mic = 0;
unsigned long lastMic = 0;
unsigned long tn = 0;

byte state = 0;
byte byteOut = 0;
byte cnt;
volatile byte data = 0;
byte last_data;


ISR (PCINT0_vect)
{
//  GIMSK |=  (0<<PCIE);		// Enable pin change interrupt for PORTB
  lastMic = mic;
  mic = micros();
//  lastMil = mil;
//  mil = millis();
  

      byte d = PINB & 0x8;
      if ((mic - lastMic > 100000) && !(d)) {
        state = 1;
        lastMic = mic;
//        sbi(PORTB,0);
      } else {

        switch (state) {
          case 1:
           if (d) {
//              t = (mic - lastMic) >> 3;
//              if (t < 400 | t > 1200) t = 3333;
//              td = 3333;
//              tbd = (byte) (mic - lastMic) >> 10;
              state = 2;
//              cbi(PORTB,0);
            } else {
              state = 0;
            }
            break;
          case 2: 
            if (d) {
              state = 0;
            } else {
              state = 3;
              cnt = 0;
  //            sbi(PORTB,0);
            }
            break;
          case 3: // start
            tn = lastMic;
//            tb = (byte)(mic - lastMic) >> 7;
//            tbn = 0;
            while (tn + 200 < mic) {
//              cbi(PORTB,0);
              cnt++;
//              sbi(PORTB,0);
              tn = tn + 3333;
   //           tbn = tbn + 26;
              byteOut = byteOut >> 1;
              if (!d) byteOut= byteOut + 0x80;
              if (cnt > 8) break;
            } 
            if (cnt > 8) {
              state = 0;
//              cbi(PORTB,0);
              data = byteOut;
            }
            break;
          default:
 //           cbi(PORTB,0);
            break; 
        }
 
      }

    lastMic = mic;
  
 // GIMSK |=  (1<<PCIE);		// Enable pin change interrupt for PORTB
  GIFR |= ( 1 << PCIF ); 	// clear interrupt flag
}


  void loop() {
    wdt_reset();
    if (data != 0) {
      if (last_data == PIN0_ADDDR && data == PIN0_ADDDR) {
            cbi(PORTB,0);
            sbi(PORTB,2);
      } 
      if (last_data == PIN2_ADDDR && data == PIN2_ADDDR) {
            cbi(PORTB,2);
            sbi(PORTB,0);
      }       
      if (last_data != PIN0_ADDDR && data != PIN0_ADDDR && last_data != PIN2_ADDDR && data != PIN2_ADDDR) {
            cbi(PORTB,0);
            cbi(PORTB,2);
      } 
      last_data = data;
 //     printByte(data);
    }
    data = 0;
    delay(200);
  }


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
      

    }
}

