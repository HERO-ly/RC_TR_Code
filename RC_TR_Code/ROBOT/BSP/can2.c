#include "main.h"

/*--------------------
函数功能:
	初始化CAN,发送信号控制方向马达
	引发中断, 读取电机状态信息

引脚使用:
	(AF)PB5->CAN_Rx (AF)PB6->CAN_Tx

CAN通道使用:
	CAN2

中断优先级:
	抢占1, 执行0
	
修改时间:
	2019/12/10
--------------------*/
void CAN2_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
	GPIO_InitTypeDef	GPIO_InitStructure; 
	CAN_InitTypeDef 	CAN_InitStructure;
  	CAN_FilterInitTypeDef	CAN_FilterInitStructure;
   	NVIC_InitTypeDef	NVIC_InitStructure;
	
    //使能相关时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);				//使能PORTB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);				//使能CAN1时钟	
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);				//使能CAN2时钟	
	
    //初始化GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5| GPIO_Pin_6;				//PB5,PB6是对应CAN2的RX, TX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;						//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;						//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;					//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						//上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);								//初始化PB5,PB6
	
	//引脚复用映射配置
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_CAN2); 				//GPIOB5复用为CAN2
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_CAN2);				//GPIOB6复用为CAN2
	  
  	//CAN单元设置
   	CAN_InitStructure.CAN_TTCM=DISABLE;									//非时间触发通信模式   
  	CAN_InitStructure.CAN_ABOM=DISABLE;									//软件自动离线管理	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;									//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure.CAN_NART=ENABLE;									//禁止报文自动传送 
  	CAN_InitStructure.CAN_RFLM=DISABLE;									//报文不锁定,新的覆盖旧的  
  	CAN_InitStructure.CAN_TXFP=DISABLE;									//优先级由报文标识符决定 
  	CAN_InitStructure.CAN_Mode= mode;									//模式设置 
  	CAN_InitStructure.CAN_SJW=tsjw;										//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1;										//Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;										//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;								//分频系数(Fdiv)为brp+1	
  	CAN_Init(CAN2, &CAN_InitStructure);									//初始化CAN2 
    
		//配置过滤器
 	CAN_FilterInitStructure.CAN_FilterNumber=14;						//过滤器0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;		//ID掩码
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;		//32位 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;					//32位ID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;				//32位MASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;	//过滤器0关联到FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; 				//激活过滤器0
  	CAN_FilterInit(&CAN_FilterInitStructure);							//滤波器初始化
		
	
	CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);								//FIFO0消息挂号中断允许.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;					//接受中断
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     		//主优先级为1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            		//次优先级为1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
	
}

/*--------------------
函数功能:
	CAN2中断执行函数, 读取方向马达状态

中断条件:
	CAN2邮箱溢出中断

修改时间:
	2019/12/10
--------------------*/

extern u32 StdId_Num;
extern Error_check_t Error_Check;
void CAN2_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;
	
	StdId_Num=RxMessage.StdId;
	CAN_Receive(CAN2,CAN_FIFO0, &RxMessage);				//从邮箱读取数据
	StdId_Num=RxMessage.StdId;
	if (StdId_Num >=0x201 && StdId_Num <= 0x204)
	{
		RX_Analysis_Angle(RxMessage.Data,StdId_Num);
		LostCountFeed(&Error_Check.count[StdId_Num-0x201]);	//检测是否CAN2掉线
	}
	CAN_ClearITPendingBit(CAN2,CAN_IT_FMP0);				//清除中断标志位
}

/*--------------------
函数功能:
	CAN2发送信号给方向马达

修改时间:
	2019/12/10
--------------------*/

s8 Tx_Msg_Angle[8]={0};
void CAN2_Send_Angle(s8* msg,u8 len)
{	
	u8 mbox;
	u16 i=0;

	CanTxMsg TxMessage;
	TxMessage.StdId=0x200;	 		// 标准标识符为0
	TxMessage.ExtId=0;	 			// 设置扩展标示符（29位）
	TxMessage.IDE=0;		 		// 使用扩展标识符
	TxMessage.RTR=0;		 		// 消息类型为数据帧，一帧8位
	TxMessage.DLC=len;				// 发送两帧信息
	
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];		// 第一帧信息          
	mbox= CAN_Transmit(CAN2, &TxMessage);   
	i=0;
	while((CAN_TransmitStatus(CAN2, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
}

void CAN2_Send_Angle_Kick(s8* msg,u8 len)
{	
	u8 mbox;
	u16 i=0;

	CanTxMsg TxMessage;
	TxMessage.StdId=0x1ff;	 		// 标准标识符为0
	TxMessage.ExtId=0;	 			// 设置扩展标示符（29位）
	TxMessage.IDE=0;		 		// 使用扩展标识符
	TxMessage.RTR=0;		 		// 消息类型为数据帧，一帧8位
	TxMessage.DLC=len;				// 发送两帧信息
	
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];		// 第一帧信息          
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0;
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
}


