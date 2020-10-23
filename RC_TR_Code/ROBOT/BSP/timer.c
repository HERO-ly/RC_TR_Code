#include "main.h"

/*--------------------
��������:
	��ʼ��TIM3,������ʱ���жϿ���CAN����PID�ź�

��ʱ��ʹ��:
	TIM3

�ж����ȼ�:
	��ռ0, ִ��2

�޸�ʱ��:
	2019/12/7
--------------------*/

void TIM3_CAN_SEND_Init(u32 arr,u32 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitTypeStructre;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);					//ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);					//ʱ��ʹ��
	
	GPIO_InitTypeStructre.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitTypeStructre.GPIO_OType=GPIO_OType_PP;
	GPIO_InitTypeStructre.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitTypeStructre.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitTypeStructre.GPIO_Speed=GPIO_High_Speed;
	GPIO_Init(GPIOA,&GPIO_InitTypeStructre);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3);
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;			//��Ƶ��ʽ1
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;		//������ʽ1, ��С����
	TIM_TimeBaseInitStructure.TIM_Period=arr;							//��װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;						//��Ƶϵ��
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);					//��ʼ��
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);							//�趨�жϴ���
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OC1Init(TIM3,&TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM3,	ENABLE);//ARPEʹ�� 
	
	TIM_Cmd(TIM3,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;						//���ö�ʱ��3Ϊ�ж�Դ
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;						//ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;				//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;					//�������ȼ�2
	NVIC_Init(&NVIC_InitStructure);
	
}



void TIM2_PWM_Init(u32 arr,u32 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	GPIO_InitTypeDef GPIO_InitTypeStructre;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);					//ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);					//ʱ��ʹ��
	
	GPIO_InitTypeStructre.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitTypeStructre.GPIO_OType=GPIO_OType_PP;
	GPIO_InitTypeStructre.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitTypeStructre.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitTypeStructre.GPIO_Speed=GPIO_High_Speed;
	GPIO_Init(GPIOA,&GPIO_InitTypeStructre);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM2);
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;			//��Ƶ��ʽ1
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;		//������ʽ1, ��С����
	TIM_TimeBaseInitStructure.TIM_Period=arr;							//��װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;						//��Ƶϵ��
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);					//��ʼ��
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OC1Init(TIM2,&TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM2,	ENABLE);//ARPEʹ�� 
	
	TIM_Cmd(TIM2,ENABLE);
	
}

/*--------------------
��������:
	TIM3�ж�ִ�к���, ִ��PID���㲢����PID�ź�

�ж�����:
	TIM3����ж�

�޸�ʱ��:
	2019/12/9
--------------------*/
u32 time  = 0;							//ϵͳ��ʱ��1msһ��
u32 led_num=0;							//LED�ƽ�����˸�ļ���
u8 Angle_Init_flag=0;					//�궨���̱�ʶ��
u8 flag_key=0;

void TIM3_IRQHandler()
{
	static u8 Start_all=0;

	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)		//��ʱ���ж�
	{
		if(!Start_all)
		{
			Start_all=1;
		}
		else											//������°�����, ���������������
		{
			if(Angle_Init_flag!=2) //�ж��Ƿ��Ѿ���ɱ궨��
			{
				Angle_Init();
			}	
			else
			{
				control_task();							//ִ��״̬��
				time++;									//ϵͳʱ��++
				led_num++;								//LED��˸ʱ��++
				if(led_num>=600) 
				{
					led_num=0;
				}
			} 
		}
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);		//�����ʶ��
}



