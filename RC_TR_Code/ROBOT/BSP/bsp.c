#include "main.h"

/*--------------------
��������:
	�������г�ʼ������
	
�޸�ʱ��:
	2019/12/10
--------------------*/

void BSP_Init()
{
	LED_Init();															//����LED�Ƴ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);						//�жϷ���
	KEY_Init();															//������ʼ��
	Ball_Check_PC2_Init();
	Circle_Init(Angle_Mode);											//��4����������Ϊ�ٶȻ�, ׼�����нǶȱ궨
	CAN2_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,3,CAN_Mode_Normal);	//CAN2��ʼ��
	TIM3_CAN_SEND_Init(10000-1,84-1);									//1���붨ʱ��
	Kick_Init();
}



