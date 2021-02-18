/*
 * Timer.c
 *
 * Created: 6/2/2019 5:28:12 PM
 *  Author: dendo
 */ 
#include "Timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

unsigned int volatile milli;
unsigned int volatile millisekunden;
unsigned int volatile sekunde;
unsigned int volatile minute;
unsigned int volatile stunde;

void initTimer()
{
		TCCR0A = 0b00000010;
		TCCR0B = 0b00000101;
		TCCR1A = 0b00000000;
		TCCR1B = 0b00001010;
		TCCR1C = 0b00000000;
		TIMSK1 = 0b00000010;
		OCR0A = 0b10000000;    //8 mhz durch 8 equals 1 mhz equals 1ms 
		OCR1AH = 0b00000000;   //1 ms timer: 		OCR1AH = 0b00000011; OCR1AL = 0b11101000;
	    OCR1AL = 0b01100100;   //is 100 us          //	OCR1AL = 0b00110010;  //50us    //
		
		sei();					// enable interrupts
		millisekunden=0;
		sekunde=0;
		minute=0;
		stunde=0;
}

void TimerIR()
{
	TIMSK1 = 0b00000010;
	sei();
}
///////////////////////////////////////////////////////////////////////////////////////////////////
ISR(TIMER0_COMPA_vect) { //muss irgendwie definiert werden sonst klappt nicht

}

///////////////////////////////////////////////////////////////////////////////////////////////////
ISR(TIMER1_COMPA_vect) {
	cli();
	
    milli++;
	millisekunden++;
	if(millisekunden == 1000)
	{
		
		
		sekunde++;
		millisekunden = 0;
		if(sekunde == 60)
		{
			minute++;
			sekunde = 0;
		}
		if(minute == 60)
		{
			stunde++;
			minute = 0;
		}
		if(stunde == 24)
		{
			stunde = 0;
		}
	}
	sei();
}
