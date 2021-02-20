/*
 * ADC.h
 *
 * Created: 6/2/2019 6:25:19 PM
 *  Author: dendo
 */ 
#include <avr/io.h>

#ifndef ADC_H_
#define ADC_H_
#ifndef F_CPU					// if F_CPU was not defined in Project -> Properties
#define F_CPU 8000000UL			// define it now as 1 MHz unsigned long
#endif

extern uint8_t akkuoffset;
extern uint64_t vcc;
extern uint8_t state;

void initADC();


#endif /* ADC_H_ */