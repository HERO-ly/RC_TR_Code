#include "main.h"

TR_Control_t TR_Control=TR_DATA_DEFAULT;
u8 CRC_result,end_fame=0;

void Ni_analysis(uint8_t *ni_rx_buffer)
{
	int i=0;
	while((ni_rx_buffer[i]!=0XA5)&&(i<=8))					//读下位机操作
	{
		i++;
	}
	end_fame=ni_rx_buffer[i+9];
	CRC_result = Get_CRC8_Check(&ni_rx_buffer[i],9);
	if(CRC_result==end_fame)
	{
		TR_Control.TR_state=ni_rx_buffer[i+2];						//读取TR数据
		TR_Control.V_x=(ni_rx_buffer[i+3]<<8)|ni_rx_buffer[i+4];
		TR_Control.V_y=(ni_rx_buffer[i+5]<<8)|ni_rx_buffer[i+6];
		TR_Control.V_w=(ni_rx_buffer[i+7]<<8)|ni_rx_buffer[i+8];
		i+=12;
		//LostCountFeed(&Error_Check.count[7]);	//上位机下位机通信检测
	}
	else
		i+=12;
}
s32 TV_y=0;		//调试
s32 TV_x=0;		//调试
s32 TV_w=0;
void Speed_analysis()
{
	extern s32 Origin_Angle[4];
	extern s32 TragetSpeed[4];	//目标速度.来源 pid.c
	extern s32 TragetAngle[4];	//目标角度.来源 pid.c
	extern u32 AngleProportion;
	s32 Tar_Speed=0;
	s32 Target_angle=0;
	
	if(TV_x==0&&TV_y==0&&TV_w==0)
//	if(TR_Control.V_x==0&&TR_Control.V_y==0&&TR_Control.V_w==0)	
	{
		StopMove();
		return ;
	}else if(TV_w)
	// else if(TR_Control.V_w)
	{
		Target_angle=45*AngleProportion;
		TragetSpeed[0]=TV_w;
		TragetSpeed[1]=-TV_w;
		TragetSpeed[2]=-TV_w;
		TragetSpeed[3]=TV_w;
		TragetAngle[0]=Origin_Angle[0]+Target_angle;
		TragetAngle[1]=Origin_Angle[1]-Target_angle;
		TragetAngle[2]=Origin_Angle[2]+Target_angle;
		TragetAngle[3]=Origin_Angle[3]-Target_angle;
		Limit_Speed_Angle();
		return ;
	}
	
//	if(TV_y!=0&&TV_x==0)
	if(TR_Control.V_y!=0&&TR_Control.V_x==0)
	{
//		TragetSpeed[0]=TV_y;
//		TragetSpeed[1]=-TV_y;
//		TragetSpeed[2]=-TV_y;
//		TragetSpeed[3]=TV_y;
		
		TragetSpeed[0]=TR_Control.V_y;
		TragetSpeed[1]=-TR_Control.V_y;
		TragetSpeed[2]=-TR_Control.V_y;
		TragetSpeed[3]=TR_Control.V_y;
		Target_angle=0;
	}
	
//	if(TV_x!=0&&TV_y==0)
	if(TR_Control.V_x!=0&&TR_Control.V_y==0)
	{
		Target_angle=90*AngleProportion;
//		if(TV_x>0)
		if(TR_Control.V_x>0)
		{
//			TragetSpeed[0]=TV_x;
//			TragetSpeed[1]=-TV_x;
//			TragetSpeed[2]=-TV_x;
//			TragetSpeed[3]=TV_x;
			
			TragetSpeed[0]=TR_Control.V_x;
			TragetSpeed[1]=-TR_Control.V_x;
			TragetSpeed[2]=-TR_Control.V_x;
			TragetSpeed[3]=TR_Control.V_x;
		}else
		{
			Target_angle=-Target_angle;
//			TragetSpeed[0]=-TV_x;
//			TragetSpeed[1]=TV_x;
//			TragetSpeed[2]=TV_x;
//			TragetSpeed[3]=-TV_x;
			
			TragetSpeed[0]=-TR_Control.V_x;
			TragetSpeed[1]=TR_Control.V_x;
			TragetSpeed[2]=TR_Control.V_x;
			TragetSpeed[3]=-TR_Control.V_x;
		}
	}
	
//	if(TV_x!=0&&TV_y!=0)
	if(TR_Control.V_x!=0&&TR_Control.V_y!=0)
	{
//		Tar_Speed=sqrt(TV_x*TV_x+TV_y*TV_y);
		Tar_Speed=sqrt(TR_Control.V_x*TR_Control.V_x+TR_Control.V_y*TR_Control.V_y);
//		Target_angle=atan(1.0*TV_x/TV_y)*In_Angle*AngleProportion;
		Target_angle=atan(1.0*TR_Control.V_x/TR_Control.V_y)*In_Angle*AngleProportion;
		if(Target_angle>0)
		{
//			if(TV_x>0)
			if(TR_Control.V_x>0)
			{
				TragetSpeed[0]=Tar_Speed;
				TragetSpeed[1]=-Tar_Speed;
				TragetSpeed[2]=-Tar_Speed;
				TragetSpeed[3]=Tar_Speed;
			}else
			{
				TragetSpeed[0]=-Tar_Speed;
				TragetSpeed[1]=Tar_Speed;
				TragetSpeed[2]=Tar_Speed;
				TragetSpeed[3]=-Tar_Speed;
			}
		}else
		{
//			if(TV_x>0)
			if(TR_Control.V_x>0)
			{
				TragetSpeed[0]=-Tar_Speed;
				TragetSpeed[1]=Tar_Speed;
				TragetSpeed[2]=Tar_Speed;
				TragetSpeed[3]=-Tar_Speed;
			}else
			{
				TragetSpeed[0]=Tar_Speed;
				TragetSpeed[1]=-Tar_Speed;
				TragetSpeed[2]=-Tar_Speed;
				TragetSpeed[3]=Tar_Speed;
			}
		}
	}
//	TragetSpeed[i]=sqrt(TR_Control.V_x*TR_Control.V_x+TR_Control.V_y*TR_Control.V_y);	//暂定为每个轮子的速度大小方向一样, 计算xy矢量和
//	Target_angle=atan(1.0*TR_Control.V_x/TR_Control.V_y)*In_Angle*AngleProportion;			//计算需要转向的角度

	TragetAngle[0]=Origin_Angle[0]+Target_angle;
	TragetAngle[1]=Origin_Angle[1]+Target_angle;
	TragetAngle[2]=Origin_Angle[2]+Target_angle;
	TragetAngle[3]=Origin_Angle[3]+Target_angle;
	Limit_Speed_Angle();
}

