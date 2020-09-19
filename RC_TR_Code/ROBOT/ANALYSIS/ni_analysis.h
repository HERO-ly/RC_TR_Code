#ifndef __NI_ANALYSIS_H
#define __NI_ANALYSIS_H

#include "sys.h"

 typedef struct {       
	int16_t V_x;
	int16_t V_y;
	int16_t V_w;
	uint8_t TR_state; 
}TR_Control_t; 

#define TR_DATA_DEFAULT \
{\
	0,0,0,0\
}\
	
void Ni_analysis(uint8_t *ni_rx_buffer);
void speed_analysis(TR_Control_t* PR_Control);
void Speed_analysis(void);
#endif


