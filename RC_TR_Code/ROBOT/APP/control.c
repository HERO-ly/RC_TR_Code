#include "main.h"
/*************************
��������Control
�������ܣ������ƺ��� ״̬���л�
������ control_task()���������б�����⺯��Check_Task() ����״̬�л�����Work_Execute() LEDģʽ�л�����Led_State()
���ж�С���Ƿ�Ϊ��������ģʽ ÿ20ms����λ������һ������ 
����ִ�к���Work_Execute()���������߸�ģʽ��
	Ԥ��ģʽ(PREPARE_STATE)���̵Ƴ��� ûʵ������ �б�Ҫ����ӡ�
	�Լ�ģʽ(CHECK_STATE)���̵Ƴ��� 300ms�����궨ģʽ ��������Լ�ģʽ3000��(ÿ��һ��time_count��һ) ��ô���������λ
	�궨ģʽ(CALI_STATE)���̵Ƴ��� ִ�б궨���� ����1s�궨���� ������������ģʽ
	��������ģʽ(NORMAL_STATE)���̵����� ����Work_start()���� 

	����ģʽ(ERROR_STATE):��Ƴ��� ֻ�����ж������Ƿ����� �Ƚ������������ �ٴμ����� ������������ �������Լ�ģʽ û������ ����������ģʽ
	����������ģʽ3000�� ��ô���������λ

	����ģʽ(PROTECT_STATE):��ƿ��� ֻ�����ж������ܷ����λ���յ����� �Ƚ������������ �ٴμ����λ���������� ������������ �������Լ�ģʽ û������ ����������ģʽ
	������뱣��ģʽ3000�� ��ô���������λ

	CAN2��ʧģʽ(LOST_CAN2_STATE)�����̵ƽ�����˸ ֻ����CAN2�ܷ���յ����� �Ƚ������������ �ٴμ��CAN2�������� ������������ �������Լ�ģʽ û������ ����������ģʽ
	�������CAN2��ʧģʽ3000�� ��ô���������λ
δ���� 2020/5/26
*************************/
WorkState_e workState=PREPARE_STATE;		  //��ʼΪԤ��ģʽ
						  

u32 time_1ms_count=0;						  //1msһ�� �Լ�ģʽר��ʱ���ʱ
u8 Workstate=0;								  //������־λ 1Ϊ��������ģʽ 0Ϊ����ģʽ


void control_task()
{	
	static u8 time=0;//ϵͳ��ʱ��1msһ��
	time_1ms_count++; 
	time++;
	Check_Task();                             //�������
	Work_Execute();	                          //���󱣻�������״̬�Զ�ִ��
	Led_State();                              //ledģʽ�л�
	
	if(GetWorkState()==NORMAL_STATE)		  //�ж��Ƿ�Ϊ��������ģʽ
	{
		Workstate=1;
	}
	else
	{
		Workstate=0;
	}
	
	if(time%10==0)
	{
		mainboard_send();				  //����λ����������
		time=0;
	}
}	


/***************����ִ��**********************/
void Work_Execute(void)
{
	switch(GetWorkState())
	{	
		case PREPARE_STATE:                   //Ԥ��ģʽ �̵Ƴ���
		{
			StopMove();
			SetWorkState(NORMAL_STATE);		  //��ʼ��, ��ʵӦ�������õ�CHECK_STATE, ���м��, �ɹ����ٵ�NORMAL_STATE, ���ڵ�����, �ͷ���
			break;
		}
		case CHECK_STATE:                     //�Լ�ģʽ������ִ��󣬿�ͨ����λ�޸�  ͨ���Ƿ�Ͽ� ���������ֵ�Ƿ�����  �̵Ƴ���
		{	
			static u32 time_count=0;
			time_count++;	
			if(time_1ms_count>300)
			{	
				SetWorkState(NORMAL_STATE);
				if(time_count>3000)			  //�ص��Լ�ģʽ���
				{
					time_count=0;
					NVIC_SystemReset();
				}						
			}
			break;
		}
		case NORMAL_STATE:                    //��������ģʽ��ִ����λ��ָ��  �̵�����
		{
			Move_Mode_Check();
			break;
		}/*
		case ERROR_STATE:					  //�ж������Ƿ�����	��Ƴ���
		{
			static u32 time_count_chassis=0;
			time_count_chassis++;
			if(Error_Check.statu[LOST_LEFT_FRONT_Angle]==0&&Error_Check.statu[LOST_RIGHT_FRONT_Angle]==0&&Error_Check.statu[LOST_RIGHT_BACK_Angle]==0&&Error_Check.statu[LOST_LEFT_BACK_Angle]==0)
			{
				SetWorkState(NORMAL_STATE);
			}
			else
			{
				SetWorkState(CHECK_STATE);
			}
			if(time_count_chassis>3000)
			{
				time_count_chassis=0;
				NVIC_SystemReset();
			}
			break;
		}
		case PROTECT_STATE:					  //�ж�����������   ��ƿ���
		{
			static u32 time_count_transmission=0;
			time_count_transmission++;
			if(Error_Check.statu[LOST_TRANSMISSION]==0)
			{
				SetWorkState(NORMAL_STATE);
			}
			else
			{
				SetWorkState(CHECK_STATE);
			}
			if(time_count_transmission>3000)  //��ν����ģʽ ϵͳ����
			{
					time_count_transmission=0;
					NVIC_SystemReset();
			}
			break;
		}
		case LOST_CAN2_STATE:				  //���̵ƽ�����˸  CAN2����������ݳ���
		{
			static u32 time_count_can2=0;
			time_count_can2++;
			if(Error_Check.statu[LOST_KICK]==0)
			{
				SetWorkState(NORMAL_STATE);
			}
			else
			{
				SetWorkState(CHECK_STATE);
			}
			if(time_count_can2>3000)
			{
				time_count_can2=0;
				NVIC_SystemReset();
			}
			break;
		}*/
		default:
			break;
	}
}


/******************LEDָʾ��**********************/

extern u32 led_num;
void Led_State(void)
{
	switch(GetWorkState())
	{
		case PREPARE_STATE:                //Ԥ��ģʽ
		{
			LED_GREEN_ON();                //�̵Ƴ��� 
			LED_RED_OFF();
			break;
		}
		case CHECK_STATE:                  //�Լ�ģʽ������ִ��󣬿�ͨ����λ�޸�         �̵Ƴ���
		{
			LED_GREEN_ON();
			LED_RED_OFF();
			break;
		}
		case NORMAL_STATE:                 //��������ģʽ�����л�˫ģʽ�������ǿ���/���λ�ÿ��ƣ����������β����ٴα궨�����л��������ǿ���
		{	
			if(led_num%10==0)
			{
				LED_RED_OFF();             //�̵ƿ���
				LED_GREEN_ON();
			}
			else 
			{			
				LED_RED_OFF();
			    LED_GREEN_OFF();
			}
			break;
		}
		case PROTECT_STATE:                 
		{
			if(led_num%10==0)
			{
				LED_RED_OFF();              //��ƿ���
				LED_GREEN_OFF();
			}
			else 
			{
				LED_RED_ON();
				LED_GREEN_OFF();
			}
			break;
		}
		case LOST_CAN2_STATE:			    //���̵ƽ�����˸
		{
			if(led_num%10==0)
			{
				LED_RED_OFF();
				LED_GREEN_ON();
			}
			else 
			{
				LED_RED_ON();
			    LED_GREEN_OFF();
			}
			break;
		}
		case ERROR_STATE:                   //����ģʽ
		{
			LED_RED_ON();                   //��Ƴ�������
			LED_GREEN_OFF();
			break;
		}
		default:
			LED_RED_OFF();                  //��ȫ��
			LED_GREEN_OFF();
			break;
	}
}

/***********************--����ģʽ--**********************/
void SetWorkState(WorkState_e state) 		//�趨����ģʽ
{
    workState = state;
}


WorkState_e GetWorkState(void)          	//�õ�����ģʽ
{
	return workState;
}
