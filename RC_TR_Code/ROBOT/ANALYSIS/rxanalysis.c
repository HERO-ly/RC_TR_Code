#include "main.h"

/*--------------------
��������:
	����CAN�յ��ĵ������

�޸�ʱ��:
	2019/12/9
--------------------*/
s32 last_angle_temp[5]={0};


void RX_Analysis_Angle(u8 *msg,u32 StdId_Num)
{
	extern motoinfo moto_dir_ctl[5];
	extern motoinfo moto_dir_ctl_temp;
	u16 temp=0;
	
	moto_dir_ctl[StdId_Num-0x201].present_angle=msg[1];temp=msg[0];
	moto_dir_ctl[StdId_Num-0x201].present_angle=moto_dir_ctl[StdId_Num-0x201].present_angle|(temp<<8);
	if (moto_dir_ctl[StdId_Num-0x201].speed!=0)
		{
		/*---------------
			ʵ�ֹ���:
				�ж�����Խ�����, �����þ����ٶ�
		---------------*/
		if (moto_dir_ctl[StdId_Num-0x201].present_angle-last_angle_temp[StdId_Num-0x201] < -4096)
		{
			moto_dir_ctl[StdId_Num-0x201].abs_angle += 8192+moto_dir_ctl[StdId_Num-0x201].present_angle-last_angle_temp[StdId_Num-0x201];
		}
		/*---------------
			ʵ�ֹ���:
				�жϷ���Խ�����, �����þ����ٶ�
		---------------*/
		if (moto_dir_ctl[StdId_Num-0x201].present_angle-last_angle_temp[StdId_Num-0x201] > 4096)
		{
			moto_dir_ctl[StdId_Num-0x201].abs_angle -= 8192-moto_dir_ctl[StdId_Num-0x201].present_angle+last_angle_temp[StdId_Num-0x201];
		}
		/*---------------
			ʵ�ֹ���:
				�ж�û��Խ������, �����þ����ٶ�
		---------------*/
		if (moto_dir_ctl[StdId_Num-0x201].present_angle-last_angle_temp[StdId_Num-0x201] > -4096 &&
			moto_dir_ctl[StdId_Num-0x201].present_angle-last_angle_temp[StdId_Num-0x201] < 4096)
		{
			moto_dir_ctl[StdId_Num-0x201].abs_angle += moto_dir_ctl[StdId_Num-0x201].present_angle-last_angle_temp[StdId_Num-0x201];
		}
	}
		
	moto_dir_ctl[StdId_Num-0x201].speed=msg[3];temp=msg[2];
	moto_dir_ctl[StdId_Num-0x201].speed=moto_dir_ctl[StdId_Num-0x201].speed|(temp<<8);
	moto_dir_ctl[StdId_Num-0x201].electric=msg[5];temp=msg[4];
	moto_dir_ctl[StdId_Num-0x201].electric=moto_dir_ctl[StdId_Num-0x201].electric|(temp<<8);
	moto_dir_ctl[StdId_Num-0x201].temtpratrue=msg[6];
	last_angle_temp[StdId_Num-0x201]=moto_dir_ctl[StdId_Num-0x201].present_angle;
	
	if(StdId_Num==0x204) moto_dir_ctl_temp=moto_dir_ctl[StdId_Num-0x201];
}

void RX_Analysis_Speed(u8 *msg,u32 StdId_Num)
{
	extern motoinfo moto_speed_ctl[4];
	extern motoinfo moto_speed_ctl_temp;
	
	u16 temp=0;
	moto_speed_ctl[StdId_Num-0x201].present_angle=msg[1];temp=msg[0];
	moto_speed_ctl[StdId_Num-0x201].present_angle=moto_speed_ctl[StdId_Num-0x201].present_angle|(temp<<8);
	moto_speed_ctl[StdId_Num-0x201].speed=msg[3];temp=msg[2];
	moto_speed_ctl[StdId_Num-0x201].speed=moto_speed_ctl[StdId_Num-0x201].speed|(temp<<8);
	moto_speed_ctl[StdId_Num-0x201].electric=msg[5];temp=msg[4];
	moto_speed_ctl[StdId_Num-0x201].electric=moto_speed_ctl[StdId_Num-0x201].electric|(temp<<8);
	moto_speed_ctl[StdId_Num-0x201].temtpratrue=msg[6];
	if(StdId_Num==0x201) moto_speed_ctl_temp=moto_speed_ctl[StdId_Num-0x201];
}


