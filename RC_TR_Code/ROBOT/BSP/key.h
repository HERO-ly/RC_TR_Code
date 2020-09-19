#ifndef __KEY_H
#define __KEY_H

#include "sys.h"

#define KEY1 		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)

void KEY_Init(void);

#endif


