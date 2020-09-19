#ifndef __LED_H
#define __LED_H

#include "sys.h"

#define LED_RED PCout(0)	// DS0
#define LED_GREEN PCout(1)	// DS1	

#define RUNNING 1
#define ROMOTE_CTLING 2

#define LED_RED_ON()									GPIO_SetBits(GPIOC, GPIO_Pin_0)
#define LED_RED_OFF()									GPIO_ResetBits(GPIOC, GPIO_Pin_0)
#define LED_GREEN_ON()								    GPIO_SetBits(GPIOC, GPIO_Pin_1)
#define LED_GREEN_OFF()									GPIO_ResetBits(GPIOC, GPIO_Pin_1)

void LED_Init(void);
void LED_State_Check(void);

#endif 


