#include "main.h"

/*--------------------
函数功能:
	调用所有初始化函数
	
修改时间:
	2019/12/10
--------------------*/

void BSP_Init()
{
	LED_Init();															//红绿LED灯初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);						//中断分组
	KEY_Init();															//按键初始化
	Ball_Check_PC2_Init();
	Circle_Init(Angle_Mode);											//把4个方向电机设为速度环, 准备进行角度标定
	CAN2_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,3,CAN_Mode_Normal);	//CAN2初始化
	TIM3_CAN_SEND_Init(10000-1,84-1);									//1毫秒定时器
	Kick_Init();
}



