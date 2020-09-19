#ifndef __DMA1_H
#define __DMA1_H

#include "sys.h"

#define  USART1_DMA_RC_BUF_LEN               30u  
#define  RC_FRAME_LENGTH                     20u

void USART3_IRQHandler(void);
void USART3_DMA_Config(uint32_t baudrate);
void mainboard_send(void);
void Mainboard_DMA_Send(const u8 *Data,u8 len);

#endif


