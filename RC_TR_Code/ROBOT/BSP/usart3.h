#ifndef __USART3_H
#define __USART3_H
#include "stm32f4xx.h"
#include "usart3_analysis.h"
#include "dma2.h"

void USART3_DMA_Config(uint32_t baudrate);
void USART3_IRQHandler(void);

#endif

