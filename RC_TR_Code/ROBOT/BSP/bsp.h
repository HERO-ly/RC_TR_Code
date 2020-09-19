#ifndef __BSP_H
#define __BSP_H

#include "sys.h"

#define Angle_Mode 0x1
#define Speed_Mode 0x2

void BSP_Init(void);
void SpeedCircle_Init(u8 Mode_Angle_Speed);
void PositionCircle_Init(void);

#endif


