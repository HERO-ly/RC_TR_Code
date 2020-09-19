#include "main.h"

void Ball_Check_PC2_Init()										//激光检测的GPIO口使能
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	//使能GPIOC时钟
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;				//输入模式, 暂定, 到时候可能要一个输入一个输出?
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;					//PC0
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;			//下拉
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;		//100Mhz
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
}
