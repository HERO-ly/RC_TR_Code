#include "main.h"

/*--------------------
��������:
	��ʼ��CAN,�����źſ��ƶ������
	�����ж�, ��ȡ���״̬��Ϣ

����ʹ��:
	(AF)PA11->CAN_Rx (AF)PA12->CAN_Tx

CANͨ��ʹ��:
	CAN1

�ж����ȼ�:
	��ռ1, ִ��0
	
�޸�ʱ��:
	2019/12/10
--------------------*/
void CAN1_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
	GPIO_InitTypeDef	GPIO_InitStructure; 
	CAN_InitTypeDef 	CAN_InitStructure;
  	CAN_FilterInitTypeDef	CAN_FilterInitStructure;
   	NVIC_InitTypeDef	NVIC_InitStructure;
	
    //ʹ�����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��PORTAʱ��	                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��	
	
    //��ʼ��GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��PA11,PA12
	
	  //���Ÿ���ӳ������
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1); //GPIOA11����ΪCAN1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1); //GPIOA12����ΪCAN1
	  
  	//CAN��Ԫ����
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	//����Զ����߹���	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
  	CAN_InitStructure.CAN_NART=ENABLE;	//��ֹ�����Զ����� 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
  	CAN_InitStructure.CAN_Mode= mode;	 //ģʽ���� 
  	CAN_InitStructure.CAN_SJW=tsjw;	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;  //��Ƶϵ��(Fdiv)Ϊbrp+1	
  	CAN_Init(CAN1, &CAN_InitStructure);   // ��ʼ��CAN1 
    
		//���ù�����
 	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //������0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������0
  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
		
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�Ϊ1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
	
}

/*--------------------
��������:
	CAN1�ж�ִ�к���, ��ȡ�������״̬

�ж�����:
	CAN1��������ж�

�޸�ʱ��:
	2019/12/10
--------------------*/

u32 StdId_Num=0;
extern Error_check_t Error_Check;
void CAN1_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;
	
	CAN_Receive(CAN1,CAN_FIFO0, &RxMessage);					//�������ȡ����
	StdId_Num=RxMessage.StdId;
	if (StdId_Num >=0x201 && StdId_Num <= 0x204)
	{
		RX_Analysis_Speed(RxMessage.Data,StdId_Num);
		LostCountFeed(&Error_Check.count[StdId_Num-0x201+0x5]);	//����Ƿ�CAN1����
	}
	else if(StdId_Num==0x205)
	{
		RX_Analysis_Angle(RxMessage.Data,StdId_Num);
	}
	CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);					//����жϱ�־λ
}

/*--------------------
��������:
	CAN1�����źŸ��������

�޸�ʱ��:
	2019/12/10
--------------------*/

s8 Tx_Msg_Speed[8]={0};

void CAN1_Send_Speed(s8* msg,u8 len)
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
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0;
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
}


