#ifndef __RXANALYSIS_H
#define __RXANALYSIS_H

#include "sys.h"

typedef struct Motoinfo{
	u16 present_angle;
	s32 abs_angle;
	s16 speed;
	s16 electric;
	u8 temtpratrue;
}motoinfo;

void RX_Analysis_Angle(u8 *msg,u32 StdId_Num);
void RX_Analysis_Speed(u8 *msg,u32 StdId_Num);
#endif 


