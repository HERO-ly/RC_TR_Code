#include "main.h"

void Ball_Check_PC2_Init()										//�������GPIO��ʹ��
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	//ʹ��GPIOCʱ��
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;				//����ģʽ, �ݶ�, ��ʱ�����Ҫһ������һ�����?
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;					//PC0
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;			//����
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;		//100Mhz
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
}
