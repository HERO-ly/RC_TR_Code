#include "main.h"

/*--------------------
��������:
	��ʼ��KEY1,����KEY1��ȡ���ߵ�ƽ,�ɿ�KEY1��ȡ���͵�ƽ

����ʹ��:
	PB0->KEY1
	
�޸�ʱ��:
	2019/12/6
--------------------*/
void KEY_Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);				//ʹ��GPIOBʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;							//KEY1��Ӧ����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;						//��ͨ����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;					//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;						//����, �е����, Ϊɶ��������, �������
	GPIO_Init(GPIOB, &GPIO_InitStructure);								//��ʼ��GPIOB0
	
}


