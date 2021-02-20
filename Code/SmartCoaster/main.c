/*
 * Neopixel03.c
 *
 * Created: 4/23/2019 5:31:17 PM
 * Author : dendo
 */ 
#ifndef F_CPU					// if F_CPU was not defined in Project -> Properties
#define F_CPU 8000000UL			// define it now as 1 MHz unsigned long
#endif


#include "Timer.h"
#include "ADC.h"
#include "light_ws2812.h"
#include "irremote.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#define OFF 0xbc4
#define VOLPLUS 0xb94
#define	FUNCSTOP 0xb84

#define LEFT 0xbb4
#define	PLAY 0xbf4
#define RIGHT 0xbc4

#define DOWN 0xf80
#define	VOLMINUS 0xea1
#define UP 0xf60

#define ZERO 0xe91
#define	EQ 0xe61
#define STREPT 0xf20

#define ONE 0xf30
#define TWO 0xe71
#define	THREE 0xa15
#define FOUR 0xf70
#define FIVE 0xe31
#define SIX 0xa55
#define	SEVEN 0xbd4
#define EIGHT 0xad5
#define NINE 0xb64
#define SAMPLETIME 10000  
#define DELAY 2 

uint16_t validcodes [] = {
	
	 0xbc4
	,0xb94
	,0xb84
	,0xbb4
	,0xbf4
	,0xbc4
	,0xf80
	,0xea1
	,0xf60
	,0xe91
	,0xe61
	,0xf20
	,0xf30
	,0xe71
	,0xa15
	,0xf70
	,0xe31
	,0xa55
	,0xbd4
	,0xad5
	,0xb64
};

struct cRGB led[1];
#define MAXPIX 24
#define SPINLENGTH 3
#define CPIX 22
#define CLENGTH 3

uint8_t colors [][3]=
		{
			{200,20,0},
			{200,0,20},
			{0,200,200},
			{0,200,0},
			{0,0,200},
			{200,50,0},
			{200,200,200}
		};

uint8_t vergleich=0;
//uint8_t state=20;
int8_t adc_val;
uint8_t fillFlag=0;
uint32_t ir=0;
uint64_t volatile IRmodeStart;
uint8_t c_spin_color[3]={0,20,20};
uint8_t colormode=0;
uint8_t defaultcolors=1;
uint8_t validinput=0;
uint8_t volatile adc_offsets[6]={22,18,22,30,38,23};
uint8_t ml[6]={28,28,28,28,28,28};
uint8_t taramode=0;
uint8_t alcoholmode=4;
uint8_t adc_tara=0;
uint8_t adc_values[5]={0,0,0,0,0};
uint8_t druck_values[10]={0,0,0,0,0,0,0,0,0,0};
uint8_t zeiger_values[5]={1,5,10,15,20};  //menü auswahl die zahlen stehen für den wert des Zeigers
uint8_t samplecounter=0;
void auswahl();
void rotating_spin();
void c_spin();
void adc_anzeige();
void clear();
void ReadCommand(uint32_t cmd);
void fillRGB(uint8_t * color);
void turnofftimer();
void colorselection (int ir);
void assignColor(uint8_t * dest,uint8_t * source);
void displayPalette();
void setState (int ir);
void displayMenu (uint8_t red,uint8_t green,uint8_t blue);
void taraselection(int ir,uint8_t adc_val);
void akku_anzeige();
void init_ml();
///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {
	//initTimer();
	initADC();
	init_ml();

	//initIR();
	//DDRB |= (1 << 5) | (1 << 4);		   // set PB5 (pin 28) and PC4 (pin 27) for output
    DDRB |= (1 << 1);				        //set PB1 (D9) output for LED ring
	DDRC &= ~(1 << 5); 				        // set PC5 input for ADC 
	DDRC &= ~(1 << 4); 				        // set PC4 input for ADC vergleichswert

	DDRC &= ~(1 << 1);                       //set PC1 input for IR

	while (1) 
	{
     
	if (irFlag==0) 
	{
	fillFlag=0;
	ready=0;
	switch(state)
	{
	case 1 :
		
		adc_anzeige();
	case 20  :
	
	auswahl();
	break;
	
	case 5  :
	//adc_anzeige();
	c_spin(); 
	break;
	
	case 10  :
	ReadCommand(0b0110111010101111);
	break;
	
	case 15 :
	//ReadCommand(akkuoffset);
	akku_anzeige();
		
	default :
	break;
	//clear();
	}
	}
	else
	{
		
		if (fillFlag==0)
		{
			
		    displayMenu(0,10,15); //tuerkis fuer tara
			fillFlag=1;
			initTimer();
			IRmodeStart=milli;
		}
		else
		{
			if (milli-IRmodeStart>500)
			{
				ready=1;
				_delay_ms(50);
				ir = getIRCommandRec();
				int d;
				validinput=0;
				for (d=0;d<21;d++)
				{
					if (ir==validcodes[d])
					{
						validinput=1;
					}
				}
				
								
				if (validinput)
				{
							
				if (ir==VOLPLUS && !colormode && !taramode)  //if taste6 color mode  
				{
				colormode=1;
				displayPalette();	
								
				}
				else if (ir==EQ && !taramode && !colormode)  //TODO: put taramode on different pin
				{
				taramode=1;
				displayMenu(20,20,20); //weiss fuer tara
					
				}
				else if (ir!=0 && !colormode && !taramode)
				{
					turnofftimer();
					irFlag=0;
					clear();
					setState (ir);

				}
				else if (colormode)
				{
					colorselection(ir);
				}
				else if (taramode)
				{
					adc_tara=ADCH;
					taraselection(ir,adc_tara);
				}
				}
			
			
			}
		}
		
	
	}
	
	
}
	
	return(0);				
}




void rotating_spin() //PB1 muss ring attached sein
{
	

	uint8_t red=10;
	uint8_t green=10;
	uint8_t b=10;
	uint8_t i;
	
	//max 24, spin length 4

	for(i=0; i<MAXPIX; i++) { // For each pixel...
		
		led[i].r=red;led[i].g=green;led[i].b=b;

		if (i>SPINLENGTH-1)
		{
			led[i-SPINLENGTH].r=0;led[i-SPINLENGTH].g=0;led[i-SPINLENGTH].b=0;

		}
		else
		{
			led[MAXPIX-(SPINLENGTH-i)].r=0;led[MAXPIX-(SPINLENGTH-i)].g=0;led[MAXPIX-(SPINLENGTH-i)].b=0;

		}

		ws2812_setleds(led,24);
		_delay_ms(35); // Pause before next pass through loop
		
	}
	return;
}

void c_spin() //PB1 muss ring attached sein
{
	
	
	if (defaultcolors)
	{
		c_spin_color [0] =0;
		c_spin_color [1] =10;
		c_spin_color [2] =30;
	}

	uint8_t i;
	uint8_t delay=30;

		
	for(i=0; i<CPIX; i++) { // For each pixel...
		if(state==20){return;}
		led[i].r=c_spin_color[0];
		led[i].g=c_spin_color[1];
		led[i].b=c_spin_color[2];

		if (i>CLENGTH-1)
		{
			led[i-CLENGTH].r=0;
			led[i-CLENGTH].g=0;
			led[i-CLENGTH].b=0;

		}
		ws2812_setleds(led,24);
		_delay_ms(delay); // Pause before next
	}
	for(i=CPIX-1; i>0; i--) { // For each pixel...
		if(state==20){return;}	
		led[i].r=c_spin_color[0];
		led[i].g=c_spin_color[1];
		led[i].b=c_spin_color[2];

		if (i<CPIX-CLENGTH)
		{
			led[i+CLENGTH].r=0;
			led[i+CLENGTH].g=0;
			led[i+CLENGTH].b=0;
		}
		ws2812_setleds(led,24);
		_delay_ms(delay); // Pause before next
	}
	return;
}

void adc_anzeige() //PC5 ist der analoge input
{
	uint8_t ad =ADCH;
	uint8_t i;
	uint8_t red_offset=8;
	uint8_t green_offset=8;	
	
	
	ml[4]=32800/vcc;
	
	
	//ReadCommand(ad);
	if (ml[alcoholmode] != 0)
	{
	adc_val=((ad-adc_offsets[alcoholmode])*25)/(ml[alcoholmode]);        //ADCH geht wegen ADLAR von 0 bis 255   delta ad ist 17 fuer 500 gramm und ungefáhr 15 leer
	}
	uint8_t lifted=0;
	uint8_t brightness=10;
	
	
	//adc_val 15 standard, mit flasche 20-21. voll ist er 52-53 berechnet durch lineareitaet : 17 ist linaeritat fuer 500ml zB
	//offset mus als 20-21 gespeichert werden, bzw 20-21 + 15=35,5
	//max 52,5 minus offset ergibt 17, diese 17 muessen in 22 skalen aufgeteilt werden
	//diff 17 durch 22 ist 0.77 schritte pro skala
	// rechnung: wert minus offset durch schritte pro skala
	//143-140/4 ergibt 0, 239-140 durch 4 ergibt 24
	
	
	
	if (ad<(adc_offsets[alcoholmode]-5))
	{
		lifted=1;
	}
	
	
	if (lifted==1)
	{
			rotating_spin();
	}
	else
	{		
		if (adc_val < 3 )
		{
			uint8_t red[]={20,0,0};
			fillRGB(red);
		}
		else
		{
		samplecounter++;
		if (samplecounter==5)
		{
			samplecounter=0;
			//oder hier
		}
		adc_values[samplecounter]=adc_val;
		//evtl mitalt vergleichen
		adc_val=(adc_values[0]+adc_values[1]+adc_values[2]+adc_values[3]+adc_values[4])/5;
		uint8_t red=brightness*((adc_val>25-green_offset)?0:25-adc_val-green_offset);
		uint8_t green=brightness*((adc_val>red_offset)?adc_val-red_offset:0);
		for(i=0; i<25; i++)
			{
				if (i<adc_val)   			  
				{
				led[i].r=red;led[i].g=green;led[i].b=0;			// tuerkis
				ws2812_setleds(led,i);
				}
				else
				{
				led[i].r=0;led[i].g=0;led[i].b=0;			// off4
				ws2812_setleds(led,i);
				}
			}
		}
	}
	_delay_ms(50);
}



void akku_anzeige() 
{
	uint8_t i;
	uint8_t red_offset=8;
	uint8_t green_offset=8;

	uint8_t akku=akkuoffset;    //ADCH geht wegen ADLAR von 0 bis 255
	
	
	if (akku==0)
	{
		uint8_t color[3]={100,0,0};
		fillRGB(color); //fills bright red
	}
	
	uint8_t brightness=1;

	uint8_t red=brightness*((akku>25-green_offset)?0:25-akku-green_offset);
	uint8_t green=brightness*((akku>red_offset)?akku-red_offset:0);

	
		
	for(i=0; i<25; i++)
	{
		//if(state==20){return;}
		if (i<akku)

		{
			led[i].r=red;led[i].g=green;led[i].b=0;			// tuerkis
			ws2812_setleds(led,i);
		}
		else
		{
			led[i].r=0;led[i].g=0;led[i].b=0;			// off4
			ws2812_setleds(led,i);
			
		}
	}
	
}



void clear()
{
	uint8_t d;

	for(d=0; d<24; d++)
	{
		led[d].r=0;led[d].g=0;led[d].b=0;			// off
		
	}
	ws2812_setleds(led,24);
	return;
}

void auswahl()
{
	uint8_t druck =ADCH;
	uint8_t red=10;
	uint8_t green=10;
	uint8_t b=10;
	uint8_t zeiger;
	uint8_t i;
	uint8_t pruefung;
	uint8_t timer_status;

	//max 24, spin length 4
	samplecounter++;
	
	if (samplecounter==10)
	{
		samplecounter=0;
		//oder hier
	}
	druck_values[samplecounter]=druck;
	//evtl mitalt vergleichen
	
	
	druck=(druck_values[0]+druck_values[1]+druck_values[2]+druck_values[3]+druck_values[4]+druck_values[5]+druck_values[6]+druck_values[7]+druck_values[8]+druck_values[9])/10;
	// For each pixel...
	
	zeiger=(druck*0.6-15)+0.5; //von 0-255 auch 0-24 leds	-2 wegen offset sensibler darum 0.8 +0.5 wegen runden
	
	if(zeiger==zeiger_values[0]||zeiger==zeiger_values[1]||zeiger==zeiger_values[2]||zeiger==zeiger_values[3]||zeiger==zeiger_values[4])
	{
	red=0;
	b=0;   //hier timer starten 
	//pruefung=zeiger;
	//timer_status=1;
	vergleich++;
	if(vergleich==30)
	{
		state=zeiger;
	}
	}
	else
	{
	vergleich=0;
	red=10;
	b=10;
	}
	
	_delay_ms(30);
	
	//if (pruefung!=zeiger&&timer_status==1) //wenn pruefung nicht mehr zeiger und der timer an ist wird Timer wieder ausgeschalten und menü wird nicht ausgewäglt
	//{
		//turnofftimer();
		//timer_status=0;
	//}
	
	for (i=0;i<25;i++)
	{
		if(i==zeiger||i==zeiger+1||i==zeiger-1)
		{
			led[i].r=red;led[i].g=green;led[i].b=b;
		}
		else{led[i].r=0;led[i].g=0;led[i].b=0;}
		
	}
	
		//led[zeiger].r=red;led[zeiger].g=green;led[zeiger].b=b;
		//led[zeiger+1].r=red;led[zeiger+1].g=green;led[zeiger+1].b=b;
		//led[zeiger-1].r=red;led[zeiger-1].g=green;led[zeiger-1].b=b;
		//led[zeiger-2].r=0;led[zeiger-2].g=0;led[zeiger-2].b=0;  //löscht leds hiter zeiger
		//led[zeiger+2].r=0;led[zeiger+2].g=0;led[zeiger+2].b=0; //löscht led vor zeiger(beim zurückgehen)


		ws2812_setleds(led,24);
	
		
		
}

void ReadCommand(uint32_t cmd)

{
	
	clear();
	led[0].r=0;led[0].g=30;led[0].b=0;			// tuerkis
	ws2812_setleds(led,24);
	_delay_ms(115); // Pause before next pass through loop
	uint8_t i;
	for( i=0; i<25; i++)
	
	{
		//if(state==20){return;}   packt er nicht
		
		if ((cmd&1)==1)
		{
			
			
			led[i].r=30;led[i].g=0;led[i].b=30;			// tuerkis
			ws2812_setleds(led,24);
		}
		else
		{
			led[i].r=0;led[i].g=0;led[i].b=0;			// tuerkis
			ws2812_setleds(led,24);
			
		}
		cmd=cmd>>1;
		_delay_ms(50); // Pause before next pass through loop

	}
	_delay_ms(1000); // Pause before next pass through loop

	
	
	
	
}

void fillRGB(uint8_t *color)
{
	uint8_t red=color[0];
	uint8_t green=color[1];
	uint8_t blue=color[2];
	uint8_t d;

	for(d=0; d<24; d++)
	{
		led[d].r=red;led[d].g=green;led[d].b=blue;			// off
		
	}
	ws2812_setleds(led,24);
	return;
}

void displayPalette()
{
	uint8_t d;
	uint8_t k;
	for(k=1; k<8; k++)
	{
		
	
	for(d=3*k-3; d<(3*k); d++)
	{
		led[d].r=colors[k-1][0];led[d].g=colors[k-1][1];led[d].b=colors[k-1][2];	
		
	}
	}
	led[21].r=0;
	led[21].g=0;
	led[21].b=0;
	ws2812_setleds(led,24);
	return;
}

void displayMenu (uint8_t red,uint8_t green,uint8_t blue)
{
	
	uint8_t d;

	for(d=0; d<24; d++)
	{
		if (d%2==0)
		{
			led[d].r=red;led[d].g=green;led[d].b=blue;			
		}
		else
		{
			led[d].r=0;led[d].g=0;led[d].b=0;
		}
		
	}
	ws2812_setleds(led,24);
	return;
}

void turnofftimer()

{
		//TIMSK1 = 0b00000010; //an
		TIMSK1 = 0b00000000; //aus
}

void assignColor(uint8_t * dest,uint8_t * source)
{
	dest[0]=source[0];
	dest[1]=source[1];
	dest[2]=source[2];
}
void colorselection (int ir)
{

	switch (ir)
	{
		case ONE: //Taste 1
        assignColor(c_spin_color,colors[0]);
		fillRGB(c_spin_color);
		break;
		
		case TWO: //Taste 2
        assignColor(c_spin_color,colors[1]);
		fillRGB(c_spin_color);
		break;

		case THREE: //Taste 3
        assignColor(c_spin_color,colors[2]);
		fillRGB(c_spin_color);
		break;
		
		case FOUR: //Taste 4
		assignColor(c_spin_color,colors[3]);
		fillRGB(c_spin_color);
		break;
		
		case FIVE: //Taste 5
		assignColor(c_spin_color,colors[4]);
		fillRGB(c_spin_color);
		break;

		case SIX: //Taste 6
		assignColor(c_spin_color,colors[5]);
		fillRGB(c_spin_color);
		break;
		
		case SEVEN: //Taste 7
		assignColor(c_spin_color,colors[6]);
		fillRGB(c_spin_color);
		break;		

		case PLAY: //Taste play
		colormode=0;
		defaultcolors=0;
		turnofftimer();
		irFlag=0;
		state=0;
		clear();
		
		
		break;
		default:
		break;
	}

}

void setState (int ir)
{
	switch (ir)
	{
		case ONE: //Taste 1 //0,1
	    alcoholmode=0;
		state=1;
		break;
		
		case TWO: //Taste 2  //0,2
	    alcoholmode=1;
	    state=1;
		break;

		case THREE: //Taste 3  //0,3
	    alcoholmode=2;
        state=1;
		break;
		
		case FOUR: //Taste 4  //0,4
		alcoholmode=3;
	    state=1;        
		break;
		
		case FIVE: //Taste 5   //0,5
	    alcoholmode=4;
	    state=1;
		break;

		case SIX: //Taste 6    //0,6
	    alcoholmode=5;
	    state=1;       
		break;
		
		case SEVEN: //Taste 7
        state=3;  //akkuanzeige
		break;
        
		case EIGHT: //Taste 8
        state=2;  //debug
        break;
		
		case NINE: //Taste 9
		state=0;  //cspin
		break;
					
		case EQ: //Taste EQ
		//belegt durch tara
		break;
		
		default:
		state=0;   //cspin
	}
}

void taraselection(int ir,uint8_t adc_val)
{
	
	switch (ir)
	{
		case ONE: //Taste 1 //0,1
		adc_offsets[0]=adc_val;
	    alcoholmode=0;
		state=1;
		taramode=0;
		turnofftimer();
		irFlag=0;
		clear();
		break;
		
		case TWO: //Taste 2  //0,2
		adc_offsets[1]=adc_val;
		alcoholmode=1;
		state=1;
		taramode=0;
		turnofftimer();
		irFlag=0;
		clear();		
		break;

		case THREE: //Taste 3  //0,3
		adc_offsets[2]=adc_val;
		alcoholmode=2;
		state=1;
		taramode=0;
		turnofftimer();
		irFlag=0;
		clear();
		break;
		
		case FOUR: //Taste 4 //0,4
		adc_offsets[3]=adc_val;
		alcoholmode=3;
		state=1;
		taramode=0;
		turnofftimer();
		irFlag=0;
		clear();
		break;
		
		case FIVE: //Taste 5  //0,5
		adc_offsets[4]=adc_val;
		alcoholmode=4;
		state=1;
		taramode=0;
		turnofftimer();
		irFlag=0;
		clear();
		break;

		case SIX: //Taste 6  //0,6
		adc_offsets[5]=adc_val;
		alcoholmode=5;
		state=1;
		taramode=0;
		turnofftimer();
		irFlag=0;
		clear();
		break;
		
		default:
		displayMenu(20,20,20); //weiss fuer tara
		//state=0; //cspin
		//taramode=0;
		//turnofftimer();
		//irFlag=0;
		//clear();
			
		break;
	}
}

void init_ml()
{
	//uint64_t ml1=(112200)/vcc;    //255* 438mV     438mV entsprechen 500ml      //vcc zwischen 3700 und 4200
	//uint64_t ml2=(112200)/vcc;    //255* 438mV
	//uint64_t ml3=(112200)/vcc;    //255* 438mV 
	ml[0]=149600/vcc;    
	ml[1]=29920/vcc;   
	ml[2]=44880/vcc;
	ml[3]=59840/vcc;
	ml[4]=72800/vcc;
	ml[5]=88760/vcc;
	
	
	



}


