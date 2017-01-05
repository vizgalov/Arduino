/**
 * ScopeAnalog
 *
 *
 * Copyright 2009 Jonathan Oxer <jon@oxer.com.au>
 * Copyright 2009 Hugh Blemings <hugh@blemings.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. http://www.gnu.org/licenses/
 *
 * www.practicalarduino.com/projects/scope-logic-analyzer
 */

/**
 * Define macros for setting and clearing bits in the ADC prescale register
 */
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

int val;

/**
 * Sets the ADC prescaler to 16, prepares the relevant digital inputs,
 * and opens serial comms with the host
 */
void setup()
{
  sbi(ADCSRA,ADPS2);
  cbi(ADCSRA,ADPS1);
  cbi(ADCSRA,ADPS0);
  pinMode(A0, INPUT);
  Serial.begin(115200);
}

/**
 * Read the inputs and send values to the host. The analog read values
 * from the digital inputs need to be fudged so they read 0 or 1023
 */
 
#define SIZE 1024
static int vals[SIZE];
void loop()
{
  Serial.println("Ready  ");

  delay (3000);
  
  Serial.println("Start  ");
  long t = millis();
  for( int i=0; i<SIZE; i++ ){
    vals[i] =  analogRead(A0) ;
  }
  t = millis() - t;
  Serial.print("Listing  ");
  Serial.println(t);

  for( int i=0; i<SIZE; i++ ){
    long l = vals[i];
    Serial.println(l*5000/1023);
  }

  Serial.println("Complete");
  delay (13000);
}
