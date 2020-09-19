#include "main.h"
/*************************
��������	Protect
�������ܣ�	���п��Ź���� 
������ 		Check_Task()�Լ캯��  
Ч��:		��������ʱ, ת��״̬��, �������ȼ�Ϊ: �ٶȵ�� ����-> �Ƕȵ�� ����-> ������ ����-> ���»�ͨ��
ʱ��:		2020/6/24
δ����
*************************/

#define LOST_THRESHOLD 100

Error_check_t Error_Check={LOST_CYCLE,{0},{0}};
u8 error_flag=0;
void LostCountAdd(u16*lostcount)       //ι��
{
	if(*lostcount<0xFFFE)
	(*lostcount)++;
}

void LostCountFeed(u16* lostcount)     //�幷
{
	*lostcount =0;
}

u8 LostCountCheck(u16 lostcount,u8* statu,const u16 cycle)	//����ı�
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
	{//�ٶȵ�����ִ���
			SetWorkState(LOST_CAN1_STATE);	
	}
	else if(Error_Check.statu[LOST_LEFT_FRONT_Angle]==1||Error_Check.statu[LOST_RIGHT_FRONT_Angle]==1||Error_Check.statu[LOST_RIGHT_BACK_Angle]==1||Error_Check.statu[LOST_LEFT_BACK_Angle]==1)
	{//���������ִ���
			SetWorkState(LOST_CAN2_STATE);	
	}
	else if(Error_Check.statu[LOST_KICK]==1)
	{//���������ִ���
//			SetWorkState(LOST_CAN2_STATE);				//����������CANδ��, ���Դ��޸�
	}
	else if(Error_Check.statu[LOST_TRANSMISSION]==1)
	{//������ ͨ��ʧ�� 
		    SetWorkState(PROTECT_STATE);
	}
}



