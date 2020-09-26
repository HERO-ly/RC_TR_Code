#include "main.h"

s32 Origin_Angle[4]={0};			//��ʼ�Ƕ�

extern TR_Control_t TR_Control;
extern s32 TragetSpeed[4];			//�ⲿ����Ŀ���ٶ�
extern s32 TragetAngle[4];			//�ⲿ����Ŀ��Ƕ�
u8 Work_mode=0;
u8 Mode2_state=0;
u32 Door_PWM=1500;
u8 TTEST=0;	//����

void Move_Mode_Check()
{
	extern u8 Abs_Angle_State;
	extern u16 Moto_TouchDown[4];
	extern u16 Abs_Angle_Init_Count;
	
	//TR_Control.TR_state������ǶԵ�
	switch(TTEST)
	{
		case 0:														//"0"ģʽ   �ȴ���λ�������ź� 
		{
			StopMove();
			SEND_PID();												//��ʽ������ݶ�������, Ҳ���ܵ���(��ʱ��Ч��), ���Թ�����, �ȷ���,
			Work_mode=0;
			break;
		}
		case 1:														//"1" ģʽ  �����˶�
		{
			Speed_analysis();										//������λ���������ٶ�����
			SEND_PID();												//����
			Work_mode=1;	
			Mode2_state = 0;
			break;
		}
		case 2:														//"2" ģʽ  �������û�нӵ�, ��������λ��������Ϣ
		{	
			Check_ball();											//�ȴ���Ӧ�������ж�, ֱ������λ��һֱ������Ϣ
			Work_mode=2;
			break;
		}
		case 3:														//"3"ģʽ  ���ƶ���򿪵������
		{			
			Open_underdoor();
			Work_mode=3;
			break;
		}
		case 4:														//"4"ģʽ  ���ƶ���رյ���	
		{	
			Close_underdoor();
			Work_mode=4;
			break;
		}
		case 5:														//"5"ģʽ  ����Ͻ�����
		{	
			Kick_UP();
			Work_mode=5;
			break;
		}
		case 6:														//"5"ģʽ  ����ͷŵ���
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
��������:
	�յ�, ���ֻᵽ��ʼ�Ƕ�, ֹͣ��ԭ��

�޸�ʱ��:
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
u8 TAR_START=0;					//����Ȧ��
s32 TAR_KICI_ANGLE=200000;
u8 KICK_num=0;
void Kick_UP(void)
{
	extern motoinfo moto_dir_ctl[5];
	
	if(KICK_num==0)
	{
		moto_dir_ctl[4].abs_angle=0;
		KICK_num=1;
		Kick_Init();
	}if(TAR_START)
	{
		TAR_KICI_ANGLE=560000;
	}
	if(moto_dir_ctl[4].abs_angle>=560000*0.95)
	{
		DIS_Kick_Init();
		KICK_num=0;
		TTEST=1;
	}
//	if(num==1)
//	{
//		TTEST=1;
//		num=0;
//		return ;
//	}
//	if(moto_dir_ctl[4].abs_angle>=Temp_kick*0.9)
//	{
//		num--;
//		Temp_kick-=moto_dir_ctl[4].abs_angle;
//		moto_dir_ctl[4].abs_angle=0;
//	}
	PID_Kick_Send(TAR_KICI_ANGLE);
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

void Limit_Speed_Angle()				//���Ա�������
{
	s32 SpeedLimit=7000;
//	s32 AngleLimit=115000;
	u8 TragrtMoto_Num=0;
	
/*	for (TragrtMoto_Num=0;TragrtMoto_Num<4;TragrtMoto_Num++)				���޸�, ���α궨��, ���ڻ�û�۲���Ƕ������С���ص�
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
	for (TragrtMoto_Num=0;TragrtMoto_Num<4;TragrtMoto_Num++)				//�����޸�, 7000ת��Ŀ���ٶ����ֵ, �ݶ�
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


