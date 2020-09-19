#ifndef __CONTROL_H
#define __CONTROL_H

#include "sys.h"

typedef enum 
{
	PREPARE_STATE,         	//�ϵ��ʼ��
	CHECK_STATE,           	//�Լ�״̬  �ڴ˽׶��ܹ�ͨ���ں�������������ⷢ��ʱ�����Զ��ں�����
	KICK_UP,				//�������Ͻ�����
	KICK_OUT,				//�������ͷŵ���
	NORMAL_STATE,          	//��������״̬����̨ʹ�����������ݵ���
	
	LOST_CAN1_STATE,		//CAN1���ݶ�ʧ, �������ٶȵ�����ִ���
	LOST_CAN2_STATE,       	//CAN2���ݶ�ʧ, �����Ʒ��������ִ���
	
	PROTECT_STATE,         	//����״̬���ڳ���ѭ��ʱִ�У�����⵽�������쳣���ߴ������쳣ʱ�����ģʽ
	ERROR_STATE,           	//Ϊ���̵�����ִ���
	STOP_STATE,            	//���ִ���ǿ�ƹر��������
}WorkState_e;

void control_Task(void);
void Work_State_Change(void);
void Work_Execute(void);
void Led_State(void);
void OUTPUT_OFF(void);


void control_task(void);
WorkState_e GetWorkState(void);
void SetWorkState(WorkState_e state);

#endif

