#ifndef __CAN2_H
#define __CAN2_H

#include "sys.h"

void CAN2_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);
void CAN2_Send_Angle(s8* msg,u8 len);
void CAN2_Send_Angle_Kick(s8* msg,u8 len);
#endif


