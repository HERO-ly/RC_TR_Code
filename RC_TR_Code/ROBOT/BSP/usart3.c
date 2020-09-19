#include "usart3.h"

/********接受外部陀螺仪数据********/

unsigned char imu_rx_buffer[2][20]={0};

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
	
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	USART_DeInit(USART3);
	USART_StructInit(&usart3);
	usart3.USART_BaudRate = baudrate;
	usart3.USART_WordLength = USART_WordLength_8b;
	usart3.USART_StopBits = USART_StopBits_1;
	usart3.USART_Parity = USART_Parity_Even;
	usart3.USART_Mode = USART_Mode_Rx ;
	usart3.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART3,&usart3);
	
	
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
	USART_Cmd(USART3,ENABLE);
	//////////////////////////////////////////////////////////////
	
	
	DMA_DeInit(DMA1_Stream1);
	//DMA2 CLOCK ENABLE
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	//DMA CONFIG
	dma.DMA_Channel = DMA_Channel_4;
	dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
	dma.DMA_Memory0BaseAddr = (uint32_t)&(imu_rx_buffer[0][0]);
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
	
	DMA_DoubleBufferModeConfig(DMA1_Stream1,(uint32_t)&imu_rx_buffer[0][0],DMA_Memory_0); //first used memory configuration
    DMA_DoubleBufferModeCmd(DMA1_Stream1, ENABLE);
	
	DMA_Init(DMA1_Stream1,&dma);                                                                                                                                                                            
	//DMA STREAM2 TRANSPORT COMPLETE INTERRUPT ENABLE
	//DMA_ITConfig(DMA2_Stream2,DMA_IT_TC,ENABLE);
	//DMA STREAM2 ENABLE
	DMA_Cmd(DMA1_Stream1,ENABLE); 
	//DMA中断
//	nvic.NVIC_IRQChannel = DMA2_Stream2_IRQn;
	nvic.NVIC_IRQChannel = USART3_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 1;	//2
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
}


void USART3_IRQHandler(void)
{
	u16 tmp_DataCount = 0;
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
	{
		//clear the idle pending flag 
		(void)USART3->SR;
		(void)USART3->DR;
		
		tmp_DataCount =RC_FRAME_LENGTH - DMA_GetCurrDataCounter(DMA1_Stream1);
		if(DMA_GetCurrentMemoryTarget(DMA1_Stream1)==0)
		{
			DMA_Cmd(DMA1_Stream1,DISABLE);
			DMA1_Stream1->NDTR=(uint16_t)RC_FRAME_LENGTH ;
			DMA_DoubleBufferModeConfig(DMA1_Stream1,(uint32_t)&imu_rx_buffer[1][0],DMA_Memory_1);
			DMA_Cmd(DMA1_Stream1,ENABLE);
			//if(tmp_DataCount >=18)
			usart3_analysis(imu_rx_buffer[0],tmp_DataCount);
		}
		else
		{
			DMA_Cmd(DMA1_Stream1,DISABLE);
			DMA1_Stream1->NDTR=(uint16_t)RC_FRAME_LENGTH ;
			DMA_DoubleBufferModeConfig(DMA1_Stream1,(uint32_t)&imu_rx_buffer[0][0],DMA_Memory_0);
			DMA_Cmd(DMA1_Stream1,ENABLE );
			//if(tmp_DataCount >=18)
				usart3_analysis(imu_rx_buffer[1],tmp_DataCount);
		}
	}
}

