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
	LOST_LEFT_FRONT_Angle,	            //������ǰ��	1�ŷ�����					0
	LOST_RIGHT_FRONT_Angle,				//������ǰ��	2�ŷ�����					1
	LOST_RIGHT_BACK_Angle,				//�����Һ���	3�ŷ�����					2
	LOST_LEFT_BACK_Angle,				//���������	4�ŷ�����					3
	LOST_KICK,							//����ṹ���								4
	LOST_LEFT_FRONT_Speed,	            //������ǰ��	1���ٶȵ��					5
	LOST_RIGHT_FRONT_Speed,				//������ǰ��	2���ٶȵ��					6
	LOST_RIGHT_BACK_Speed,				//�����Һ���	3���ٶȵ��					7
	LOST_LEFT_BACK_Speed,				//���������	4���ٶȵ��					8
	LOST_TRANSMISSION,			   		//����������  								9
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

