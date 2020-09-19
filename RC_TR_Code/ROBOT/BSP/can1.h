#ifndef __CAN1_H
#define __CAN1_H

#include "sys.h"

void CAN1_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);
void CAN1_Send_Angle(s8* msg,u8 len);
void CAN1_Send_Speed(s8* msg,u8 len);
#endif


