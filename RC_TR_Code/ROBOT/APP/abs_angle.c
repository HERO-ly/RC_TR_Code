#include "main.h"

extern motoinfo moto_dir_ctl[4];				//�ⲿ���� �������ĵ������
extern s32 Origin_Angle[4];						//�ⲿ���� �������궨��ĳ�ʼλ��
extern s32 TragetSpeed[4];						//�ⲿ���� Ŀ���ٶȳ�ʼ��

u16 Moto_TouchDown[4]={0};						//������׼���
u16 Abs_Angle_Init_Count=0;						//������־, ���ڴ��ױ궨��, ����10�������
s32 AngleProportion=0;							//ÿ���Ƕȶ�Ӧ��Ŀ��Ƕ�ֵ

void Angle_Init()
{
	extern u8 Angle_Init_flag;										//�ⲿ���� �궨�����жϷ�, �������ڵ�һ�α궨���ǵڶ��α궨
	
	if(Angle_Init_flag==0)											//���Ϊ0, �����ڵ�һ�α궨������
	{
		if (Moto_TouchDown[0]>=5&&Moto_TouchDown[1]>=5&&			//ֻ�е��������Ӷ�����5�κ�, ��Ϊ��ʱ�������궨λ��
			Moto_TouchDown[2]>=5&&Moto_TouchDown[3]>=5)
			{
				Abs_Angle_Init_Count++;								//����ɾ��, ԭ���Ǵ�����Ϊ����, �ô��벻Ҫ��ô�����е�֮�������
				if (Abs_Angle_Init_Count==10)
				{
					Moto_TouchDown[0]=0;							//�Ѵ��׼�������, ׼���ڶ��α궨
					Moto_TouchDown[1]=0;
					Moto_TouchDown[2]=0;
					Moto_TouchDown[3]=0;
					moto_dir_ctl[0].abs_angle=0;					//�����ڵ�λ����Ϊ���ԽǶ�0
					moto_dir_ctl[1].abs_angle=0;
					moto_dir_ctl[2].abs_angle=0;
					moto_dir_ctl[3].abs_angle=0;
					TragetSpeed[0]=-TragetSpeed[0];					//��תĿ���ٶ�, �õ������һ���궨��
					TragetSpeed[1]=-TragetSpeed[1];
					TragetSpeed[2]=-TragetSpeed[2];
					TragetSpeed[3]=-TragetSpeed[3];
					Angle_Init_flag=1;								//�ı�궨���̷���, ��ʾ�Ѿ���ɵ�һ�α궨
					Abs_Angle_Init_Count=0;							//��������
				}
			}
			else
			{
				 Abs_Angle_Init();									//��4�����ӻ�ûͬʱ����5�����ϵ�ʱ��, ��������PID�ٶȻ����, ͬʱ�ж��Ƿ񴥵�
			}
	}else if(Angle_Init_flag==1)									//�жϱ궨�����Ƿ����ڶ��׶�
	{
		if (Moto_TouchDown[0]>=5&&Moto_TouchDown[1]>=5&&			//ͬ��
			Moto_TouchDown[2]>=5&&Moto_TouchDown[3]>=5)
			{
				Abs_Angle_Init_Count++;
				if (Abs_Angle_Init_Count==10)
				{
					CAN1_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,3,CAN_Mode_Normal);		//��ʼ��CAN1, ���Է��Ͷ�������ź���
					Origin_Angle[0]=moto_dir_ctl[0].abs_angle/2;	//��¼���ξ��ԽǶȵĲ�ֵ, ��Ϊ�������ڵı궨��Χ, ��2�����ָ��ǰ����ԭʼλ��
					Origin_Angle[1]=moto_dir_ctl[1].abs_angle/2;
					Origin_Angle[2]=moto_dir_ctl[2].abs_angle/2;
					Origin_Angle[3]=moto_dir_ctl[3].abs_angle/2;
					/*
					8.6����д��, ����*/
					Origin_Angle_check();
					/*
					���Origin_Angle�����Ƿ���ִ���, ������ִ�������, ��֤AngleProportion���ݲ�ʧ��
					*/
					//AngleProportion=(Origin_Angle[0]+Origin_Angle[1]+Origin_Angle[2]+Origin_Angle[3])/4/150;
					//AngleProportion ͨ���궨�����4�����ӵ�ƫת�Ƕ�Ϊ���, Ȼ�����ֵ��ӳ��Ե������, �ٳ���ת���Ƕ�
					Circle_Init(Speed_Mode);						//��ʼ��λ�û����ٶȻ�
					Angle_Init_flag=2;								//��ʾ�궨���̽���, ���Խ���״̬������
					USART3_DMA_Config(115200);;						//��ʼ������, ���Ժ���λ��ͨ��
					StopMove();										//��Ŀ���ٶȷ�����Ϊԭ�ش����ĳ�ʼֵ
				}
			}
			else 
			{
				Abs_Angle_Init();									//ͬ��
			}
	}
		
}

void Moto_TouchCheck(u8 Moto_Num)
{
	extern u16 Moto_TouchDown[4];
	if (moto_dir_ctl[Moto_Num].speed==0)	//�����ʱ������ٶ�Ϊ0, ��ô��Ϊ����Ѿ�ת���궨λ��
	{
		Moto_TouchDown[Moto_Num]++;
	}
}

void Origin_Angle_check()
{
	int i=0;
	s32 temp[4]={0};							//�ݴ�, ������Ч��Origin_Angle����ֵ����
	s32 temp_AngleProportion=0;					//�ݴ�, �ۼ���Чֵ, ����temp_AngleProportion������
	u8 div=0;									//�ݴ�, �����м�����Чֵ
	
	for(i=0;i<4;i++)
	{
		if((Origin_Angle[i]>60000&&Origin_Angle[i]<75000)||(Origin_Angle[i]<-60000&&Origin_Angle[i]>-75000))
		{										//�����Χ���Ҽ������м�¼��������, �������ʵ����ݾ���ֵӦ����67000��68000, �Ժ�����˺�����ʵ���С��Χ
			if(Origin_Angle[i]<0)				//ȡ����ֵ
			{
				temp[i]=-Origin_Angle[i];
			}
			else
			{
				temp[i]=Origin_Angle[i];
			}
		}
		else
		{
			temp[i]=-1;							//����Чλ����Ϊ-1, ��ʵ����������ߴ�һ�����, �����������Чֵ˵���궨��������, ϵͳ�Զ���λ
		}
	}
	for(i=0;i<4;i++)
	{
		if(temp[i]!=0&&temp[i]!=-1)
		{
			temp_AngleProportion+=temp[i];		//�ۼ���Чֵ
			div++;
		}
	}AngleProportion=temp_AngleProportion/div/150;	//������Чֵ
}


