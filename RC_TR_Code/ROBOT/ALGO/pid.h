#ifndef __PID_H
#define __PID_H

#include "sys.h"

typedef struct pid_init_val{
	
	float Kp;
	float Ki;
	float Kd;
	
	float error;
	float error_last;
	float error_max;
	float dead_line;
	
	float intergral;
	float intergral_max;
	
	float derivative;
	
	float output;
	float output_max;
	
}PID;

void Circle_Init(u8 Mode_Angle_Speed);
void SEND_PID(void);
void PID_Init(PID *pid, float Kp, float Ki, float Kd, float error_max, float dead_line, float intergral_max, float output_max);
void PID_General_Cal(PID *pid, float fdbV, float tarV,u8 moto_num,s8 *Tx_msg);
void Abs_Angle_Init(void);
void PID_Kick_Send(s32 Tar);
void Kick_Init(void);
void DIS_Kick_Init(void);
#endif 


