#include "main.h"

s32 Origin_Angle[4]={0};			//初始角度

extern TR_Control_t TR_Control;
extern s32 TragetSpeed[4];			//外部引用目标速度
extern s32 TragetAngle[4];			//外部引用目标角度
u8 Work_mode=0;
u8 Mode2_state=0;
u32 Door_PWM=1500;
u8 TTEST=0;	//调试

void Move_Mode_Check()
{
	extern u8 Abs_Angle_State;
	extern u16 Moto_TouchDown[4];
	extern u16 Abs_Angle_Init_Count;
//	if(TR_Control.TR_state!=5&&TR_Control.TR_state!=6)DIS_Kick_Init();
	//TR_Control.TR_state这个才是对的
//	switch(TTEST)
	if(TR_Control.TR_state!=1) StopMove();
	switch(TR_Control.TR_state)
	{
		case 0:														//"0"模式   等待上位机发送信号 
		{
			Work_mode=0;
			break;
		}
		case 1:														//"1" 模式  底盘运动
		{
			Speed_analysis();										//分析上位机传来的速度数据
			Work_mode=1;	
			Mode2_state = 0;
			break;
		}
		case 2:														//"2" 模式  检查球有没有接到, 并且向上位机发送信息
		{	
			Check_ball();											//等待感应器触发中断, 直接向上位机一直发出信息
			Work_mode=2;
			break;
		}
		case 3:														//"3"模式  控制舵机打开挡板放球
		{			
			Open_underdoor();
			Work_mode=3;
			break;
		}
		case 4:														//"4"模式  控制舵机关闭挡板	
		{	
			Close_underdoor();
			Work_mode=4;
			break;
		}
		case 5:														//"5"模式  电机上紧弹簧
		{	
			Kick_UP();
			Work_mode=5;
			break;
		}
		case 6:														//"5"模式  电机释放弹簧
		{	
			Kick_OUT();
			Work_mode=6;
			break;
		}
		default:
			break;
	}SEND_PID();
}

/*--------------------
函数功能:
	空挡, 车轮会到初始角度, 停止在原地

修改时间:
	2020/8/6
--------------------*/
void StopMove()
{
	TragetSpeed[0]=0;
	TragetSpeed[1]=0;
	TragetSpeed[2]=0;
	TragetSpeed[3]=0;
	
	TragetAngle[0]=Origin_Angle[0];
	TragetAngle[1]=Origin_Angle[1];
	TragetAngle[2]=Origin_Angle[2];
	TragetAngle[3]=Origin_Angle[3];
}

void Check_ball()
{
	if(!CHECK_FALG)
	{
		Mode2_state=1;
	}
}
u8 TAR_START=0;					//测试圈数
s32 TAR_KICI_ANGLE=330000;
u8 KICK_num=0;
void Kick_UP(void)
{
	extern motoinfo moto_dir_ctl[5];
	
	if(KICK_num==0)
	{
		moto_dir_ctl[4].abs_angle=0;
		KICK_num=1;
		TAR_KICI_ANGLE=330000;
		DIS_Kick_Init();
		Kick_Init();
		TAR_START=0;
	}
	//PID_Kick_Send(TAR_KICI_ANGLE);
}

void Kick_OUT(void)
{
	extern motoinfo moto_dir_ctl[5];
	TAR_START=1;
	
	if(TAR_START)
	{
		TAR_KICI_ANGLE=560000;
	}
	if(moto_dir_ctl[4].abs_angle>=TAR_KICI_ANGLE*0.99)
	{
	//	DIS_Kick_Init();
		KICK_num=0;
	}//PID_Kick_Send(TAR_KICI_ANGLE);
}

void Open_underdoor(void)
{
	Door_PWM=500;
	TIM_SetCompare1(TIM3,Door_PWM);
}

void Close_underdoor(void)
{
	Door_PWM=2100;
	TIM_SetCompare1(TIM3,Door_PWM);
}

void Limit_Speed_Angle()				//测试保护代码
{
	s32 SpeedLimit=3000;
//	s32 AngleLimit=115000;
	u8 TragrtMoto_Num=0;
	
/*	for (TragrtMoto_Num=0;TragrtMoto_Num<4;TragrtMoto_Num++)				待修改, 二次标定后, 现在还没观察出角度最大最小的特点
	{
		if (TragetAngle[TragrtMoto_Num] > AngleLimit)
		{
			TragetAngle[TragrtMoto_Num]=AngleLimit;
		}
		if (TragetAngle[TragrtMoto_Num] < 0)
		{
			TragetAngle[TragrtMoto_Num]=0;
		}
	}*/
	for (TragrtMoto_Num=0;TragrtMoto_Num<4;TragrtMoto_Num++)				//不用修改, 7000转是目标速度最大值, 暂定
	{
		if (TragetSpeed[TragrtMoto_Num] > SpeedLimit)
		{
			TragetSpeed[TragrtMoto_Num]=SpeedLimit;
		}
		if (TragetSpeed[TragrtMoto_Num] < -SpeedLimit)
		{
			TragetSpeed[TragrtMoto_Num]=-SpeedLimit;
		}
	}
}


