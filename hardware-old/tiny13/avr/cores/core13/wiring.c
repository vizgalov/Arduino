/*
*** Core13 ***
Arduino core designed for Attiny13 and similar devices.
NO WARRANTEE OR GUARANTEES!
Written by John "smeezekitty" 
You are free to use, redistribute and modify at will EXCEPT IF MARKED OTHERWISE IN A PARTICULAR SOURCE FILE!
Version 0.19
*/

#include "wiring_private.h"
#include <avr/interrupt.h>
volatile unsigned long ovrf=0;
ISR(TIM0_OVF_vect){
	ovrf++; //Increment counter every 256 clock cycles
}
unsigned long millis(){
	unsigned long x;
	asm("cli"); 
	/*Scale number of timer overflows to milliseconds*/
// 16000 Hz/s / 256 clock cycles(ovrf) = 62.5 * 16 = 1000 u sec.
	#if F_CPU == 16000
	x = ovrf * 16;
	#elif F_CPU == 128000
	x = ovrf * 2;
    #elif F_CPU == 600000
	x = ovrf / 2;
	#elif F_CPU == 1000000
	x = ovrf / 4;
	#elif F_CPU == 1200000
	x = ovrf / 5;
	#elif F_CPU == 4000000
	x = ovrf / 16;
	#elif F_CPU == 4800000
	x = ovrf / 19;
	#elif F_CPU == 8000000
	x = ovrf / 31;
	#elif F_CPU == 9600000
	x = ovrf / 37;
    #elif F_CPU == 10000000
	x = ovrf / 39;
	#elif F_CPU == 12000000
	x = ovrf / 47;
	#elif F_CPU == 16000000
	x = ovrf / 63;
	#else
	#error This CPU frequency is not defined
	#endif
	asm("sei");
	return x;
}
/*The following improved micros() code was contributed by a sourceforge user "BBC25185" Thanks!*/
unsigned long micros(){
	unsigned long x;
	asm("cli");
// 16000 Hz/s / 256 clock cycles(ovrf) = 62.5 * 16000 = 1000000 u sec.
	#if F_CPU == 16000
	x = ovrf * 16000;
	#elif F_CPU == 128000
	x = ovrf * 2000;
	#elif F_CPU == 600000
	x = ovrf * 427;
	#elif F_CPU == 1000000
	x = ovrf * 256;
	#elif F_CPU == 1200000
	x = ovrf * 213;
	#elif F_CPU == 4000000
	x = ovrf * 64;
	#elif F_CPU == 4800000
	x = ovrf * 53;
	#elif F_CPU == 8000000
	x = ovrf * 32;
	#elif F_CPU == 9600000
	x = ovrf * 27;
	#elif F_CPU == 10000000
	x = ovrf * 26;
	#elif F_CPU == 12000000
	x = ovrf * 21;
	#elif F_CPU == 16000000
	x = ovrf * 16;
	#else 
	#error This CPU frequency is not defined
	#endif
	asm("sei");
	return x; 
}
void delay(unsigned ms){
	while(ms--){
		_delay_ms(1); //Using the libc routine over and over is non-optimal but it works and is close enough
	}//Note, I may have to reimplement this because the avr-libc delay is too slow *todo*
}
void delayMicroseconds(unsigned us){
/*
For 4MHz, 4 cycles take a uS - znaci 1 milioniti deo sek. i onda us prosledjene kao parametar ovoj funk. ne treba podesavati , ako je F_CPU veci onda parametar treba mnoziti za onoliko puta
koliko je F_CPU veci od 4 MHz - us <<= 1 ( 2 na 1 za 8mhz ) us <<= 2 ( 2 na 2 za 16 mhz ) tj. deliti ako je F_CPU manji - us >>= 8 ( 2 na 8 za 16 khz tj. 256 jer je 16 khz * 256 = 4 mhz )
*/
	//*todo* measure actual speed at different clock speeds and try to adjust for closer delays
	if(us == 0){return;}
	#if F_CPU == 16000000 || F_CPU == 12000000
	if(--us == 0){return;}
	us <<= 2;
	us -= 2; //Underflow possible?
	#elif F_CPU == 8000000 || F_CPU == 9600000 || F_CPU == 10000000
	if(--us == 0){return;}
	if(--us == 0){return;}
	us <<= 1;
	us--; //underflow possible?
	#elif F_CPU == 4000000 || F_CPU == 4800000
	if(--us == 0){return;}
	if(--us == 0){return;}
	//For 4MHz, 4 cycles take a uS. This is good for minimal overhead
	#elif F_CPU == 1000000 || F_CPU == 1200000//For slow clocks, us delay is marginal.
	if(--us == 0){return;}
	if(--us == 0){return;}
	us >>= 2; 
	us--; //Underflow?
	#elif F_CPU == 600000
	if(--us == 0){return;}
	if(--us == 0){return;}
	us >>= 3;
	#elif F_CPU == 128000
	if(--us == 0){return;}
	if(--us == 0){return;}
	us >>= 5;
	#elif F_CPU == 16000
	if(--us == 0){return;}
	if(--us == 0){return;}
	us >>= 9; // us parametar delimo sa 2 na 8 ( 256 ) jer je 4 MHz oko 256 puta brzi od 16 khz-a pa nam je potrebno 256 puta manja vrednost za delay
//	us++;     // pa podizemo za 1 jer shift deljenje zaokruzuje na 1. manji broj
	#else 
	#error Invalid F_CPU value
	#endif
	asm __volatile__("1: sbiw %0,1\n\t"
			 "brne 1b" : "=w" (us) : "0" (us));
}
void init(){
	//Setup timer interrupt and PWM pins
	TCCR0B |= _BV(CS00);
	TCCR0A |= _BV(WGM00)|_BV(WGM01);
	TIMSK0 |= 2;
	TCNT0=0; //Causes malfunction?
	sei();
	ADMUX=0;
//#######################################################
	//Set up ADC clock depending on F_CPU
	// prescaler 2 ( minimum ) za 16 khz i 128 khz
//#######################################################
	#if F_CPU == 16000     
	ADCSRA |= _BV(ADEN);
	#elif F_CPU == 128000
	ADCSRA |= _BV(ADEN);
	#elif F_CPU == 1000000 || F_CPU == 1200000 || F_CPU == 600000
	ADCSRA |= _BV(ADEN) | _BV(ADPS1);
	#else
	ADCSRA |= _BV(ADEN) | _BV(ADPS1) | _BV(ADPS0) | _BV(ADPS2);
	#endif
}
	
