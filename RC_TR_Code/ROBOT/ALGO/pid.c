#include "main.h"

PID pid_position[8];							//��������PID����
PID pid_speed[4];								//���������PID����
PID pid_kick[2];

s32 TragetAngle[4]={0,0,0,0};					//Ŀ��Ƕȳ�ʼ��
s32 TragetSpeed[4]={-1000,-1000,-1000,-1000};	//Ŀ���ٶȳ�ʼ��
s32 Target_kick=65000;
extern s8 Tx_Msg_Speed[8];						//CAN1�������ݴ�
extern s8 Tx_Msg_Angle[8];						//CAN2�������ݴ�
u32 PID_OUTPUT_LIMIT=4000;						//PID��������ֵ

extern motoinfo moto_dir_ctl[5];				//�ⲿ���÷������ĵ������
extern motoinfo moto_speed_ctl[4];				//�ⲿ���ö�������ĵ������
extern s32 Origin_Angle[4];						//�ⲿ���÷������궨��ĳ�ʼλ��

/*--------------------
��������:
	��ʼ��������ٶȻ����ƵĲ���
	��ȷ����ʼ�Ƕ�ǰ, ��ʼ������������ٶȻ�
	��ȷ����ʼ�ǶȺ�, ��ʼ������������ٶȻ�
	
�޸�ʱ��:
	2019/12/10
--------------------*/
void Circle_Init(u8 Mode_Angle_Speed)
{
	/*
	Mode_Angle_Speed �����1��ʱ��, ��ô���Ƕ�4����������ʼ���ٶȻ�����
					 �������1�Ļ�, ��ô���Ƕ�4�����������ʼ���ٶȻ�����, ��4����������λ�û�����
	*/
	if (Mode_Angle_Speed==0x1)
	{
//�������ٶȻ���ʼ��	PID *pid_val				Kp		Ki			Kd			error_max		dead_line	intergral_max	output_max
		PID_Init(			&pid_position[1],		15,		0.5,		0.5,		5000,			0,			5000,			PID_OUTPUT_LIMIT/2	);
		PID_Init(			&pid_position[3],		25,		0.5,		0.5,		5000,			0,			5000,			PID_OUTPUT_LIMIT/2	);
		PID_Init(			&pid_position[5],		25,		0.5,		0.5,		5000,			0,			5000,			PID_OUTPUT_LIMIT/2	);
		PID_Init(			&pid_position[7],		25,		0.5,		0.5,		5000,			0,			5000,			PID_OUTPUT_LIMIT	);
//	����õ���1 3 5 7�Ľṹ������, ����Ϊ0 2 4 6��ʱ��ᴢ��λ�û��Ĳ���
		
//	�ٶȵ���ٶȻ���ʼ��	PID *pid_val			Kp		Ki			Kd			error_max		dead_line	intergral_max	output_max
		PID_Init(			&pid_speed[0],			0,		0,			0,			5000,			0,			5000,			0	);
		PID_Init(			&pid_speed[1],			0,		0,			0,			5000,			0,			5000,			0	);
		PID_Init(			&pid_speed[2],			0,		0,			0,			5000,			0,			5000,			0	);
		PID_Init(			&pid_speed[3],			0,		0,			0,			5000,			0,			5000,			0	);
//	��߰Ѷ���������ٶȻ���Ϊ0, ��ֹ������ֵ
	}
	else 
	{
//������λ�û���ʼ��	PID *pid_val				Kp		Ki			Kd			error_max		dead_line	intergral_max	output_max
		PID_Init(			&pid_position[0],		0.21,	0.0028,		0,			5000,			0,			5000,			PID_OUTPUT_LIMIT	);
		PID_Init(			&pid_position[1],		10,		0,			0,			5000,			0,			5000,			PID_OUTPUT_LIMIT	);
		PID_Init(			&pid_position[2],		0.3,	0.0020,		0,			5000,			0,			5000,			PID_OUTPUT_LIMIT	);
		PID_Init(			&pid_position[3],		10,		0,			0,			5000,			0,			5000,			PID_OUTPUT_LIMIT	);
		PID_Init(			&pid_position[4],		0.3,	0.002,		0,			5000,			0,			5000,			PID_OUTPUT_LIMIT	);
		PID_Init(			&pid_position[5],		10,		0,			0,			5000,			0,			5000,			PID_OUTPUT_LIMIT	);
		PID_Init(			&pid_position[6],		0.30,	0.0010,		0,			5000,			0,			5000,			PID_OUTPUT_LIMIT	);
		PID_Init(			&pid_position[7],		15,		0,			0,			5000,			0,			5000,			PID_OUTPUT_LIMIT	);
//	0 2 4 6�����λ�û�����, 1 3 5 7������ٶȻ�����
		
//	�ٶȵ���ٶȻ���ʼ��	PID *pid_val			Kp		Ki			Kd			error_max		dead_line	intergral_max	output_max
		PID_Init(			&pid_speed[0],			15,		0.5,		0.5,		5500,			0,			5000,			PID_OUTPUT_LIMIT	);
		PID_Init(			&pid_speed[1],			15,		0.5,		0.5,		5500,			0,			5000,			PID_OUTPUT_LIMIT	);
		PID_Init(			&pid_speed[2],			15,		0.5,		0.5,		5500,			0,			5000,			PID_OUTPUT_LIMIT	);
		PID_Init(			&pid_speed[3],			15,		0.5,		0.5,		5500,			0,			5000,			PID_OUTPUT_LIMIT	);
		
	}
}

/*--------------------
��������:
	��ʼ��PID�ṹ�����

�޸�ʱ��:
	2019/12/9
--------------------*/

void PID_Init(PID *pid, float Kp, float Ki, float Kd, float error_max, float dead_line, float intergral_max, float output_max)
{
	pid->Kp = Kp;
	pid->Ki = Ki;
	pid->Kd = Kd;
	pid->error_max = error_max;
	pid->output_max = output_max;
	pid->dead_line = dead_line;
	
	pid->intergral_max = intergral_max;
	
	pid->error = 0;
	pid->error_last = 0;
	pid->intergral = 0;
	pid->derivative = 0;
	pid->output = 0;
}

/*--------------------
��������:
	�Է����ĵ�����ݽ���PID�㷨
	���Կ������, ��̬���, ������ݴ�С
	��ֹ���ݹ���ʧ��

�޸�ʱ��:
	2019/12/10
--------------------*/

void PID_General_Cal(PID *pid, float fdbV, float tarV,u8 moto_num,s8 *Tx_msg)
{

	pid->error =  tarV - fdbV;
	if(pid->error > pid->error_max)
		pid->error = pid->error_max;
	if(pid->error < -pid->error_max)
		pid->error = -pid->error_max;
//	if(My_fabs(pid->error) < pid->dead_line)
//		pid->error = 0;
	
	pid->intergral = pid->intergral + pid->error;
	if(pid->intergral > pid->intergral_max)
		pid->intergral = pid->intergral_max;
	if(pid->intergral < -pid->intergral_max)
		pid->intergral = -pid->intergral_max;
	
	pid->derivative = pid->error - pid->error_last;
	pid->error_last = pid->error;
	
	pid->output = pid->Kp*pid->error + pid->Ki*pid->intergral + pid->Kd*pid->derivative;
	
	if(pid->output > pid->output_max)
		pid->output = pid->output_max;
	if(pid->output < -pid->output_max)
		pid->output = -pid->output_max;
	
	Tx_msg[moto_num*2]=((s16)pid->output)>>8;Tx_msg[moto_num*2+1]=(s16)pid->output;
}

void PID_IMU(PID *pid, float fdbV, float tarV)
{

	pid->error =  tarV - fdbV;
	if(pid->error > pid->error_max)
		pid->error = pid->error_max;
	if(pid->error < -pid->error_max)
		pid->error = -pid->error_max;
//	if(My_fabs(pid->error) < pid->dead_line)
//		pid->error = 0;
	
	pid->intergral = pid->intergral + pid->error;
	if(pid->intergral > pid->intergral_max)
		pid->intergral = pid->intergral_max;
	if(pid->intergral < -pid->intergral_max)
		pid->intergral = -pid->intergral_max;
	
	pid->derivative = pid->error - pid->error_last;
	pid->error_last = pid->error;
	
	pid->output = pid->Kp*pid->error + pid->Ki*pid->intergral + pid->Kd*pid->derivative;
	
	if(pid->output > pid->output_max)
		pid->output = pid->output_max;
	if(pid->output < -pid->output_max)
		pid->output = -pid->output_max;
	
	pid_position[0].output+=pid->output;
	pid_position[2].output+=pid->output;
	pid_position[4].output+=pid->output;
	pid_position[6].output+=pid->output;
}

/*--------------------
��������:
	����PID���㺯��, �ֱ�������Ʒ�����������ݺͿ��ƶ������������

ͨ��Э��:
	CAN1, CAN2

�޸�ʱ��:
	2020/2/29
--------------------*/
void SEND_PID()
{
	
//	λ�û�����
	PID_General_Cal(&pid_position[0],moto_dir_ctl[0].abs_angle,TragetAngle[0],0,Tx_Msg_Angle);
	PID_General_Cal(&pid_position[2],moto_dir_ctl[1].abs_angle,TragetAngle[1],1,Tx_Msg_Angle);
	PID_General_Cal(&pid_position[4],moto_dir_ctl[2].abs_angle,TragetAngle[2],2,Tx_Msg_Angle);
	PID_General_Cal(&pid_position[6],moto_dir_ctl[3].abs_angle,TragetAngle[3],3,Tx_Msg_Angle);
	
//	����λ�û�����ֵ�����ٶȻ�����
	PID_General_Cal(&pid_position[1],moto_dir_ctl[0].speed,pid_position[0].output,0,Tx_Msg_Angle);
	PID_General_Cal(&pid_position[3],moto_dir_ctl[1].speed,pid_position[2].output,1,Tx_Msg_Angle);
	PID_General_Cal(&pid_position[5],moto_dir_ctl[2].speed,pid_position[4].output,2,Tx_Msg_Angle);
	PID_General_Cal(&pid_position[7],moto_dir_ctl[3].speed,pid_position[6].output,3,Tx_Msg_Angle);
	CAN2_Send_Angle(Tx_Msg_Angle,8);
//	����CAN2�ź�
	
//	�ٶȻ�����
	PID_General_Cal(&pid_speed[0],moto_speed_ctl[0].speed,TragetSpeed[0],0,Tx_Msg_Speed);
	PID_General_Cal(&pid_speed[1],moto_speed_ctl[1].speed,TragetSpeed[1],1,Tx_Msg_Speed);
	PID_General_Cal(&pid_speed[2],moto_speed_ctl[2].speed,TragetSpeed[2],2,Tx_Msg_Speed);
	PID_General_Cal(&pid_speed[3],moto_speed_ctl[3].speed,TragetSpeed[3],3,Tx_Msg_Speed);
	CAN1_Send_Speed(Tx_Msg_Speed,8);
//	����CAN1�ź�
}

/*--------------------
��������:
	����PID�ٶȻ����Ʒ������ﴥ���������ԽǶȳ�ʼλ��

ͨ��Э��:
	CAN2

�޸�ʱ��:
	2019/12/10
--------------------*/

void Abs_Angle_Init()
{
//	ֻ�����ڱ궨�����з��������ٶȻ�
	PID_General_Cal(&pid_position[1],moto_dir_ctl[0].speed,TragetSpeed[0],0,Tx_Msg_Angle);
	PID_General_Cal(&pid_position[3],moto_dir_ctl[1].speed,TragetSpeed[1],1,Tx_Msg_Angle);
	PID_General_Cal(&pid_position[5],moto_dir_ctl[2].speed,TragetSpeed[2],2,Tx_Msg_Angle);
	PID_General_Cal(&pid_position[7],moto_dir_ctl[3].speed,TragetSpeed[3],3,Tx_Msg_Angle);
	CAN2_Send_Angle(Tx_Msg_Angle,8);
//	����CAN2�ź�
	
//	����ʱ����Ƿ�ת���궨λ��
	Moto_TouchCheck(0);
	Moto_TouchCheck(1);
	Moto_TouchCheck(2);
	Moto_TouchCheck(3);

}

void Kick_Init(void)
{
		PID_Init(			&pid_kick[0],		0.21,		0.001,		0,			5000,			0,			5000,			PID_OUTPUT_LIMIT+3000	);
		PID_Init(			&pid_kick[1],		10,		0,			0,			5000,			0,			5000,			PID_OUTPUT_LIMIT+3000	);
}

void PID_Kick_Send(void)
{
	s8 Tx_Msg_Angle_kick[8]={0,0,0,0,0,0,0,0};
	PID_General_Cal(&pid_kick[0],moto_dir_ctl[4].abs_angle,Target_kick,0,Tx_Msg_Angle_kick);
	PID_General_Cal(&pid_kick[1],moto_dir_ctl[4].speed,pid_kick[0].output,0,Tx_Msg_Angle_kick);
	CAN2_Send_Angle_Kick(Tx_Msg_Angle_kick,8);
}
