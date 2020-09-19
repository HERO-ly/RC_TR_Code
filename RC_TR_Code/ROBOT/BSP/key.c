#include "main.h"

/*--------------------
函数功能:
	初始化KEY1,按下KEY1读取到高电平,松开KEY1读取到低电平

引脚使用:
	PB0->KEY1
	
修改时间:
	2019/12/6
--------------------*/
void KEY_Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);				//使能GPIOB时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;							//KEY1对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;						//普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;					//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;						//下拉, 有点奇怪, 为啥是下拉呢, 待定检查
	GPIO_Init(GPIOB, &GPIO_InitStructure);								//初始化GPIOB0
	
}


