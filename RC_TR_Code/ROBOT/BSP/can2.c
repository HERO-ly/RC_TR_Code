#include "main.h"

/*--------------------
��������:
	��ʼ��CAN,�����źſ��Ʒ������
	�����ж�, ��ȡ���״̬��Ϣ

����ʹ��:
	(AF)PB5->CAN_Rx (AF)PB6->CAN_Tx

CANͨ��ʹ��:
	CAN2

�ж����ȼ�:
	��ռ1, ִ��0
	
�޸�ʱ��:
	2019/12/10
--------------------*/
void CAN2_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
	GPIO_InitTypeDef	GPIO_InitStructure; 
	CAN_InitTypeDef 	CAN_InitStructure;
  	CAN_FilterInitTypeDef	CAN_FilterInitStructure;
   	NVIC_InitTypeDef	NVIC_InitStructure;
	
    //ʹ�����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);				//ʹ��PORTBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);				//ʹ��CAN1ʱ��	
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);				//ʹ��CAN2ʱ��	
	
    //��ʼ��GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5| GPIO_Pin_6;				//PB5,PB6�Ƕ�ӦCAN2��RX, TX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;						//���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;						//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;					//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						//����
    GPIO_Init(GPIOB, &GPIO_InitStructure);								//��ʼ��PB5,PB6
	
	//���Ÿ���ӳ������
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_CAN2); 				//GPIOB5����ΪCAN2
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_CAN2);				//GPIOB6����ΪCAN2
	  
  	//CAN��Ԫ����
   	CAN_InitStructure.CAN_TTCM=DISABLE;									//��ʱ�䴥��ͨ��ģʽ   
  	CAN_InitStructure.CAN_ABOM=DISABLE;									//����Զ����߹���	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;									//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
  	CAN_InitStructure.CAN_NART=ENABLE;									//��ֹ�����Զ����� 
  	CAN_InitStructure.CAN_RFLM=DISABLE;									//���Ĳ�����,�µĸ��Ǿɵ�  
  	CAN_InitStructure.CAN_TXFP=DISABLE;									//���ȼ��ɱ��ı�ʶ������ 
  	CAN_InitStructure.CAN_Mode= mode;									//ģʽ���� 
  	CAN_InitStructure.CAN_SJW=tsjw;										//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1;										//Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;										//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;								//��Ƶϵ��(Fdiv)Ϊbrp+1	
  	CAN_Init(CAN2, &CAN_InitStructure);									//��ʼ��CAN2 
    
		//���ù�����
 	CAN_FilterInitStructure.CAN_FilterNumber=14;						//������0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;		//ID����
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;		//32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;					//32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;				//32λMASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;	//������0������FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; 				//���������0
  	CAN_FilterInit(&CAN_FilterInitStructure);							//�˲�����ʼ��
		
	
	CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);								//FIFO0��Ϣ�Һ��ж�����.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;					//�����ж�
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     		//�����ȼ�Ϊ1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            		//�����ȼ�Ϊ1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
	
}

/*--------------------
��������:
	CAN2�ж�ִ�к���, ��ȡ�������״̬

�ж�����:
	CAN2��������ж�

�޸�ʱ��:
	2019/12/10
--------------------*/

extern u32 StdId_Num;
extern Error_check_t Error_Check;
void CAN2_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;
	
	StdId_Num=RxMessage.StdId;
	CAN_Receive(CAN2,CAN_FIFO0, &RxMessage);				//�������ȡ����
	StdId_Num=RxMessage.StdId;
	if (StdId_Num >=0x201 && StdId_Num <= 0x205)
	{
		RX_Analysis_Angle(RxMessage.Data,StdId_Num);
		//LostCountFeed(&Error_Check.count[StdId_Num-0x201]);	//����Ƿ�CAN2����
	}
	CAN_ClearITPendingBit(CAN2,CAN_IT_FMP0);				//����жϱ�־λ
}

/*--------------------
��������:
	CAN2�����źŸ��������

�޸�ʱ��:
	2019/12/10
--------------------*/

s8 Tx_Msg_Angle[8]={0};
void CAN2_Send_Angle(s8* msg,u8 len)
{	
	u8 mbox;
	u16 i=0;

	CanTxMsg TxMessage;
	TxMessage.StdId=0x200;	 		// ��׼��ʶ��Ϊ0
	TxMessage.ExtId=0;	 			// ������չ��ʾ����29λ��
	TxMessage.IDE=0;		 		// ʹ����չ��ʶ��
	TxMessage.RTR=0;		 		// ��Ϣ����Ϊ����֡��һ֡8λ
	TxMessage.DLC=len;				// ������֡��Ϣ
	
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];		// ��һ֡��Ϣ          
	mbox= CAN_Transmit(CAN2, &TxMessage);   
	i=0;
	while((CAN_TransmitStatus(CAN2, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
}

void CAN2_Send_Angle_Kick(s8* msg,u8 len)
{	
	u8 mbox;
	u16 i=0;

	CanTxMsg TxMessage;
	TxMessage.StdId=0x1ff;	 		// ��׼��ʶ��Ϊ0
	TxMessage.ExtId=0;	 			// ������չ��ʾ����29λ��
	TxMessage.IDE=0;		 		// ʹ����չ��ʶ��
	TxMessage.RTR=0;		 		// ��Ϣ����Ϊ����֡��һ֡8λ
	TxMessage.DLC=len;				// ������֡��Ϣ
	
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];		// ��һ֡��Ϣ          
	mbox= CAN_Transmit(CAN2, &TxMessage);   
	i=0;
	while((CAN_TransmitStatus(CAN2, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
}


