#ifndef __CONTROL_H
#define __CONTROL_H

#include "sys.h"

typedef enum 
{
	PREPARE_STATE,         	//上电初始化
	CHECK_STATE,           	//自检状态  在此阶段能够通过内核重启解决的问题发生时将会自动内核重启
	KICK_UP,				//踢球电机上紧弹簧
	KICK_OUT,				//踢球电机释放弹簧
	NORMAL_STATE,          	//正常输入状态，云台使用陀螺仪数据调节
	
	LOST_CAN1_STATE,		//CAN1数据丢失, 即控制速度电机出现错误
	LOST_CAN2_STATE,       	//CAN2数据丢失, 即控制方向电机出现错误
	
	PROTECT_STATE,         	//保护状态，在程序循环时执行，当检测到程序发生异常或者传感器异常时进入该模式
	ERROR_STATE,           	//为底盘电机出现错误
	STOP_STATE,            	//出现错误，强制关闭所有输出
}WorkState_e;

void control_Task(void);
void Work_State_Change(void);
void Work_Execute(void);
void Led_State(void);
void OUTPUT_OFF(void);


void control_task(void);
WorkState_e GetWorkState(void);
void SetWorkState(WorkState_e state);

#endif

