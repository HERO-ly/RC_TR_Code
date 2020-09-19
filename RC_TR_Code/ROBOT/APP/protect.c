#include "main.h"
/*************************
函数名：	Protect
函数功能：	进行看门狗监测 
描述： 		Check_Task()自检函数  
效果:		发现问题时, 转换状态机, 错误优先级为: 速度电机 覆盖-> 角度电机 覆盖-> 踢球电机 覆盖-> 上下机通信
时间:		2020/6/24
未测试
*************************/

#define LOST_THRESHOLD 100

Error_check_t Error_Check={LOST_CYCLE,{0},{0}};
u8 error_flag=0;
void LostCountAdd(u16*lostcount)       //喂狗
{
	if(*lostcount<0xFFFE)
	(*lostcount)++;
}

void LostCountFeed(u16* lostcount)     //清狗
{
	*lostcount =0;
}

u8 LostCountCheck(u16 lostcount,u8* statu,const u16 cycle)	//无需改变
{
	if(lostcount>LOST_THRESHOLD*cycle)  //100*20 100*20 100*20
		*statu=1;
	else
		*statu=0;
	return *statu;
}

void Check_Task(void)
{
	int i=0;
	for(;i<LOST_TYPE_NUM;i++)	
	{
		LostCountAdd(&Error_Check.count[i]);
		LostCountCheck(Error_Check.count[i],&Error_Check.statu[i],Error_Check.cycle[i]);
	}
	
	if(Error_Check.statu[LOST_LEFT_FRONT_Speed]==1||Error_Check.statu[LOST_RIGHT_FRONT_Speed]==1||Error_Check.statu[LOST_RIGHT_BACK_Speed]==1||Error_Check.statu[LOST_LEFT_BACK_Speed]==1)
	{//速度电机出现错误
			SetWorkState(LOST_CAN1_STATE);	
	}
	else if(Error_Check.statu[LOST_LEFT_FRONT_Angle]==1||Error_Check.statu[LOST_RIGHT_FRONT_Angle]==1||Error_Check.statu[LOST_RIGHT_BACK_Angle]==1||Error_Check.statu[LOST_LEFT_BACK_Angle]==1)
	{//方向电机出现错误
			SetWorkState(LOST_CAN2_STATE);	
	}
	else if(Error_Check.statu[LOST_KICK]==1)
	{//踢球电机出现错误
//			SetWorkState(LOST_CAN2_STATE);				//由于踢球电机CAN未定, 所以待修改
	}
	else if(Error_Check.statu[LOST_TRANSMISSION]==1)
	{//主副板 通信失败 
		    SetWorkState(PROTECT_STATE);
	}
}



