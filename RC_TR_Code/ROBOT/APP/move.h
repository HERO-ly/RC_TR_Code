#ifndef __MOVE_H
#define __MOVE_H

#include "sys.h"

#define PI 3.14159265353846
#define In_Angle 180/PI		//½Ç¶È»¯

#define CHECK_FALG 	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)
void Check_ball(void);

void Move_Mode_Check(void);
void Move_ModeTurn(void);
void StopMove(void);
void Limit_Speed_Angle(void);
void Move_ModeTurn_MiddleLine(void);
void Move_ModeTurn_CtlPole(void);
void Move_ModePassing(void);
void Kick_UP(void);
void Kick_OUT(void);
void Open_underdoor(void);
void Close_underdoor(void);
#endif


