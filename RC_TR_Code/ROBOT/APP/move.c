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
	
	//TR_Control.TR_state这个才是对的
	switch(TTEST)
	{
		case 0:														//"0"模式   等待上位机发送信号 
		{
			StopMove();
			SEND_PID();												//正式情况中暂定不调用, 也可能调用(到时候看效果), 调试过程中, 先放着,
			Work_mode=0;
			break;
		}
		case 1:														//"1" 模式  底盘运动
		{
			Speed_analysis();										//分析上位机传来的速度数据
			SEND_PID();												//发送
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
			//KICK_OUT();
			Work_mode=5;
			break;
		}
		default:
			break;
			
	}
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
u8 Tar_quan=5;					//测试圈数
void Kick_UP(void)
{
	extern motoinfo moto_dir_ctl[5];
	extern s32 Target_kick;
	static u8 num=0;
	
	if(num==0)
	{
		moto_dir_ctl[4].abs_angle=0;
		num=Tar_quan+1;
	}
	if(num==1)
	{
		TTEST=1;
		num=0;
		return ;
	}
	if(moto_dir_ctl[4].abs_angle>=Target_kick*0.9)
	{
		num--;
		moto_dir_ctl[4].abs_angle=0;
	}
	PID_Kick_Send();
}

void Open_underdoor(void)
{
	Door_PWM=500;
	TIM_SetCompare1(TIM3,Door_PWM);
}

void Close_underdoor(void)
{
	Door_PWM=1800;
	TIM_SetCompare1(TIM3,Door_PWM);
}
void Limit_Speed_Angle()				//测试保护代码
{
	s32 SpeedLimit=7000;
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


