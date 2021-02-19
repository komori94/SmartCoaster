#ifndef TIMER_H_
#define TIMER_H_


extern unsigned int volatile milli;

extern unsigned int volatile millisekunden;
extern unsigned int volatile sekunde;
extern unsigned int volatile minute;
extern unsigned int volatile stunde;

void initTimer();
void TimerIR();
void sec_timer();

#endif /* IRREMOTE_H_ */