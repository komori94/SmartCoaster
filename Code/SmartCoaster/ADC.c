/*
 * ADC.c
 *
 * Created: 6/2/2019 6:25:37 PM
 *  Author: dendo
 */ 
#include "ADC.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#ifndef F_CPU					// if F_CPU was not defined in Project -> Properties
#define F_CPU 8000000UL			// define it now as 1 MHz unsigned long
#endif

#define UMAX 4200
#define UMIN 3700
uint8_t state=20;
uint8_t akkuoffset=0;
uint64_t vcc = 0;

void initADC()
{
	ADMUX = 0b01100100; //PC4
	ADCSRA = 0b10100011;
	ADCSRB = 0b00000000;
	PCICR = 0b00000100;
	PCMSK2 = 0b00000100;
	sei();
	ADCSRA |= (1 << ADSC);		// start ADC
	
	
	_delay_ms(500);
	uint8_t temp=ADCH;
	
	if (temp>0)
	{
		vcc= 192000/temp;  //885mv ref mal 255 = 218025 max, aber selbst ausgerechnet mit 4,2V ist 190000 = 255 * 752mV
	}
	else 
	{
		vcc=4000;
	}
	
	if (vcc>UMAX)
	{
		akkuoffset = 22;
	}
	else if (vcc<UMIN)
	{
		akkuoffset=0;
	}
	else
	{
	 akkuoffset = (vcc-UMIN)/((UMAX-UMIN)/22);
	}
	
	
	ADCSRA = 0b00000000;
	ADMUX = 0b01100101;   //PC5
	ADCSRA = 0b10101011;   //bit 3 Interupt enabele
	ADCSRB = 0b00000000;
	PCICR = 0b00000100;
	PCMSK2 = 0b00000100;
	sei();
	ADCSRA |= (1 << ADSC);		// start ADC
	
	
}

ISR(ADC_vect) {
	cli();
	
	if (ADCH>120)
	{
		state=20;
	}
	
	sei();
	}