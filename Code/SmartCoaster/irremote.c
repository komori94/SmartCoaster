#include "irremote.h"
#include "Timer.h"
#include <avr/io.h>
#include <avr/common.h>
#include <avr/interrupt.h>

#ifndef F_CPU					// if F_CPU was not defined in Project -> Properties
#define F_CPU 8000000UL			// define it now as 1 MHz unsigned long
#endif

#include <util/delay.h>


	uint8_t ready=1;
	uint8_t irFlag=0;
  
	uint64_t volatile lastSignal;
	uint64_t volatile timePassed;

	uint64_t signalready=0;
	uint8_t counter=0;
	uint64_t dataBuffer;


//will setup counter, and pin interrupt
void initIR()
{
	dataBuffer = 0;
	timePassed = 0;
    lastSignal=0;
	
	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << 1);

	sei();
}

uint32_t getIRCommandRec()  
{
	if (signalready)
	{
		uint64_t temp = dataBuffer;
		dataBuffer=0;
		return temp;
	}
	else
	{
		return 0;
	}
}

//attached to IR 
ISR(PCINT1_vect)
{
	if(irFlag==0)
	{
	dataBuffer=0;
	irFlag = 1;
	signalready=0;
	}
	else
	{
		if (ready)
		{
			    timePassed=milli-lastSignal;
				uint8_t pinState = (PINC & (1 << 1)) >> 1; //interrupt pin state
				if (timePassed>1000)
				{
					dataBuffer=0;
					counter=0;
				}
				signalready=0;
				if (pinState==1)
				{
					
				
					if (counter>21 && counter<34) //eigentlich ab 17 aber geht irgendwie nicht
					{
						if (timePassed>17) //if logic null
						{
							dataBuffer=dataBuffer | (1<<(counter-22));
							
						}
						
					}
				counter++;		
				lastSignal=milli;	
				if (counter==34)
				{
					signalready=1;
					counter=0;
				}
				}
								
		}
	}
	
}

	