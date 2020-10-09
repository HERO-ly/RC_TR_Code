#include "main.h"

/********接受外部陀螺仪数据********/

uint8_t ni_rx_buffer[2][15];
u8 mainboard_tx_buffer[14]={0};
u8 mainboard_DMA_Tx_flag = 0;
void USART3_DMA_Config(uint32_t baudrate)	
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	USART_InitTypeDef usart3;
	
	//结构体初始化
	NVIC_InitTypeDef nvic;
	DMA_InitTypeDef dma;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_USART3);
	
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_10 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	USART_DeInit(USART3);
	USART_StructInit(&usart3);
	usart3.USART_BaudRate = baudrate;
	usart3.USART_WordLength = USART_WordLength_8b;
	usart3.USART_StopBits = USART_StopBits_1;
	usart3.USART_Parity = USART_Parity_No;
	usart3.USART_Mode = USART_Mode_Rx| USART_Mode_Tx;
	usart3.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART3,&usart3);
	
	
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
	USART_Cmd(USART3,ENABLE);
	
	nvic.NVIC_IRQChannel = USART3_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 1;
	nvic.NVIC_IRQChannelSubPriority = 2;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	DMA_DeInit(DMA1_Stream1);
	DMA_DeInit(DMA1_Stream3);
	
	//DMA2 CLOCK ENABLE
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	//DMA CONFIG
	dma.DMA_Channel = DMA_Channel_4;
	dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
	dma.DMA_Memory0BaseAddr = (uint32_t)&(ni_rx_buffer[0][0]);
	dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dma.DMA_BufferSize = 20;
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma.DMA_Mode = DMA_Mode_Circular;
	dma.DMA_Priority = DMA_Priority_VeryHigh;
	dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
	dma.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	dma.DMA_MemoryBurst =  DMA_MemoryBurst_Single;
	dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream1,&dma);     
	
	DMA_DoubleBufferModeConfig(DMA1_Stream1,(uint32_t)&ni_rx_buffer[0][0],DMA_Memory_0); //first used memory configuration
  DMA_DoubleBufferModeCmd(DMA1_Stream1, ENABLE);
	DMA_Cmd(DMA1_Stream1,ENABLE); 
	
	dma.DMA_Channel = DMA_Channel_4;
	dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
	dma.DMA_Memory0BaseAddr = (uint32_t)&(mainboard_tx_buffer[0]);
	dma.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma.DMA_BufferSize = 20;
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma.DMA_Mode = DMA_Mode_Normal;
	dma.DMA_Priority = DMA_Priority_Medium;
	dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
	dma.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	dma.DMA_MemoryBurst =  DMA_MemoryBurst_Single;
	dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

	DMA_Init(DMA1_Stream3,&dma);  
	DMA_Cmd(DMA1_Stream3,DISABLE);  	
	//DMA STREAM2 TRANSPORT COMPLETE INTERRUPT ENABLE
	//DMA_ITConfig(DMA2_Stream2,DMA_IT_TC,ENABLE);
	//DMA STREAM2 ENABLE
	DMA_ClearITPendingBit(DMA1_Stream1, DMA_IT_TCIF1);
	DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF3);
	
	DMA_ITConfig(DMA1_Stream3, DMA_IT_TC,ENABLE);
	//DMA中断
//	nvic.NVIC_IRQChannel = DMA2_Stream2_IRQn;
	nvic.NVIC_IRQChannel = DMA1_Stream3_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 1;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
}
void DMA1_Stream3_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_Stream3, DMA_IT_TCIF3) != RESET)
	{
		DMA_ClearFlag(DMA1_Stream3, DMA_IT_TCIF3);
		DMA_Cmd(DMA1_Stream3, DISABLE);
		
		USART_ITConfig(USART3,USART_IT_TC,ENABLE);
		
	}
}
u32 DMA_time=0;
u32 last_time=0;
void USART3_IRQHandler(void)
{
	extern u32 time;
	
	if(USART_GetITStatus(USART3, USART_IT_TC) != RESET)
	{
		USART_ITConfig(USART3,USART_IT_TC,DISABLE);
		mainboard_DMA_Tx_flag = 0;
	}
	
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
	{
		//clear the idle pending flag 
		(void)USART3->SR;
		(void)USART3->DR;
		
		
		DMA_time=time-last_time;
		last_time=time;
		if(DMA_GetCurrentMemoryTarget(DMA1_Stream1)==0)
		{
			DMA_Cmd(DMA1_Stream1,DISABLE);
			DMA1_Stream1->NDTR=(uint16_t)RC_FRAME_LENGTH ;
			DMA_DoubleBufferModeConfig(DMA1_Stream1,(uint32_t)&ni_rx_buffer[1][0],DMA_Memory_1);
			DMA_Cmd(DMA1_Stream1,ENABLE);
			Ni_analysis(ni_rx_buffer[0]);
			LostCountFeed(&Error_Check.count[9]);	//检测上下位机是否正常通信
		}
		else
		{
			DMA_Cmd(DMA1_Stream1,DISABLE);
			DMA1_Stream1->NDTR=(uint16_t)RC_FRAME_LENGTH ;
			DMA_DoubleBufferModeConfig(DMA1_Stream1,(uint32_t)&ni_rx_buffer[0][0],DMA_Memory_0);
			DMA_Cmd(DMA1_Stream1,ENABLE );
			Ni_analysis(ni_rx_buffer[1]);
			LostCountFeed(&Error_Check.count[9]);	//检测上下位机是否正常通信
		}
	}
}

void mainboard_send(void)   		       //主板数据发送
{
	extern u8 Workstate;
	extern TR_Control_t TR_Control;
	extern u8 Mode2_state;
	
	u8 main_send_data[15]={0};
	main_send_data[0]=0xCE; 				//帧头
	main_send_data[1]=0x03;					//数据长度
	main_send_data[2]=Workstate;			//工作标志位   为0，则正在进行自检或出现错误情况
	main_send_data[3]=TR_Control.TR_state;			//工作模式位    对应相应的工作模式	
	main_send_data[4]=Mode2_state;
	main_send_data[5]=Get_CRC8_Check(main_send_data,5);
	Mainboard_DMA_Send(main_send_data,6);
}

void Mainboard_DMA_Send(const u8 *Data,u8 len)
{
	while(mainboard_DMA_Tx_flag);
	mainboard_DMA_Tx_flag = 1;
	DMA_Cmd(DMA1_Stream3,DISABLE); 
	memcpy(mainboard_tx_buffer,Data,len);
	DMA_SetCurrDataCounter(DMA1_Stream3, len);
	
	DMA_Cmd(DMA1_Stream3,ENABLE); 
}




