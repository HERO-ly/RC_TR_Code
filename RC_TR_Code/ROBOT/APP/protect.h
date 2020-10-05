#ifndef __PROTECT_H
#define __PROTECT_H

#include "sys.h"

#define LOST_CYCLE \
{\
	20,\
	20,\
	20,\
	20,\
	20,\
	20,\
	20,\
	20,\
	20,\
}

enum LOST_TYPES				
{
	LOST_LEFT_FRONT_Angle,	            //底盘左前轮	1号方向电机					0
	LOST_RIGHT_FRONT_Angle,				//底盘右前轮	2号方向电机					1
	LOST_RIGHT_BACK_Angle,				//底盘右后轮	3号方向电机					2
	LOST_LEFT_BACK_Angle,				//底盘左后轮	4号方向电机					3
	LOST_KICK,							//踢球结构电机								4
	LOST_LEFT_FRONT_Speed,	            //底盘左前轮	1号速度电机					5
	LOST_RIGHT_FRONT_Speed,				//底盘右前轮	2号速度电机					6
	LOST_RIGHT_BACK_Speed,				//底盘右后轮	3号速度电机					7
	LOST_LEFT_BACK_Speed,				//底盘左后轮	4号速度电机					8
	LOST_TRANSMISSION,			   		//主副板连接  								9
	LOST_TYPE_NUM
};


typedef struct
{
	const u16 cycle[LOST_TYPE_NUM];
	u16 count[LOST_TYPE_NUM];
	u8 statu[LOST_TYPE_NUM];
}Error_check_t;

extern Error_check_t Error_Check;



void LostCountAdd(u16*lostcount);
void LostCountFeed(u16* lostcount);
void Check_Task(void);
u8 LostCountCheck(u16 lostcount,u8* statu,const u16 cycle);

#endif

