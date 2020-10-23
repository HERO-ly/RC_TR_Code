#include "main.h"

/*--------------------
函数功能:
	初始化TIM3,发出定时器中断控制CAN发出PID信号

定时器使用:
	TIM3

中断优先级:
	抢占0, 执行2

修改时间:
	2019/12/7
--------------------*/

void TIM3_CAN_SEND_Init(u32 arr,u32 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitTypeStructre;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);					//时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);					//时钟使能
	
	GPIO_InitTypeStructre.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitTypeStructre.GPIO_OType=GPIO_OType_PP;
	GPIO_InitTypeStructre.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitTypeStructre.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitTypeStructre.GPIO_Speed=GPIO_High_Speed;
	GPIO_Init(GPIOA,&GPIO_InitTypeStructre);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3);
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;			//分频方式1
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;		//计数方式1, 从小到大
	TIM_TimeBaseInitStructure.TIM_Period=arr;							//重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;						//分频系数
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);					//初始化
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);							//设定中断触发
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OC1Init(TIM3,&TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM14在CCR1上的预装载寄存器
	TIM_ARRPreloadConfig(TIM3,	ENABLE);//ARPE使能 
	
	TIM_Cmd(TIM3,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;						//设置定时器3为中断源
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;						//使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;				//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;					//分组优先级2
	NVIC_Init(&NVIC_InitStructure);
	
}



void TIM2_PWM_Init(u32 arr,u32 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	GPIO_InitTypeDef GPIO_InitTypeStructre;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);					//时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);					//时钟使能
	
	GPIO_InitTypeStructre.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitTypeStructre.GPIO_OType=GPIO_OType_PP;
	GPIO_InitTypeStructre.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitTypeStructre.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitTypeStructre.GPIO_Speed=GPIO_High_Speed;
	GPIO_Init(GPIOA,&GPIO_InitTypeStructre);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM2);
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;			//分频方式1
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;		//计数方式1, 从小到大
	TIM_TimeBaseInitStructure.TIM_Period=arr;							//重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;						//分频系数
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);					//初始化
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OC1Init(TIM2,&TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //使能TIM14在CCR1上的预装载寄存器
	TIM_ARRPreloadConfig(TIM2,	ENABLE);//ARPE使能 
	
	TIM_Cmd(TIM2,ENABLE);
	
}

/*--------------------
函数功能:
	TIM3中断执行函数, 执行PID计算并发出PID信号

中断条件:
	TIM3溢出中断

修改时间:
	2019/12/9
--------------------*/
u32 time  = 0;							//系统总时间1ms一次
u32 led_num=0;							//LED灯交替闪烁的计数
u8 Angle_Init_flag=0;					//标定过程标识符
u8 flag_key=0;

void TIM3_IRQHandler()
{
	static u8 Start_all=0;

	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)		//定时器中断
	{
		if(!Start_all)
		{
			Start_all=1;
		}
		else											//如果按下按键了, 进入代码正常运行
		{
			if(Angle_Init_flag!=2) //判断是否已经完成标定了
			{
				Angle_Init();
			}	
			else
			{
				control_task();							//执行状态机
				time++;									//系统时间++
				led_num++;								//LED闪烁时间++
				if(led_num>=600) 
				{
					led_num=0;
				}
			} 
		}
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);		//清除标识符
}



