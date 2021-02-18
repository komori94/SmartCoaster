#ifndef IRREMOTE_H_
#define IRREMOTE_H_



#include <inttypes.h>

//times for pulse length check (100 uS * 130 = 13 mS)
#define AGC_BURST 130
#define REPEAT_TIME 110
#define ONES_TIME 20
#define ZEROS_TIME 9
#define PACKET_TIME 1150


 extern uint8_t irFlag;
 extern uint8_t ready;

void initIR();
uint32_t getIRCommandRec();

#endif /* IRREMOTE_H_ */