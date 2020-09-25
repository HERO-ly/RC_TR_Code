#include "main.h"

extern motoinfo moto_dir_ctl[4];				//外部引用 方向电机的电机数据
extern s32 Origin_Angle[4];						//外部引用 方向电机标定后的初始位置
extern s32 TragetSpeed[4];						//外部引用 目标速度初始化

u16 Moto_TouchDown[4]={0};						//电机触底计数
u16 Abs_Angle_Init_Count=0;						//计数标志, 用在触底标定后, 连续10毫秒待机
s32 AngleProportion=0;							//每个角度对应的目标角度值

void Angle_Init()
{
	extern u8 Angle_Init_flag;										//外部引用 标定过程判断符, 看看是在第一次标定还是第二次标定
	
	if(Angle_Init_flag==0)											//如果为0, 代表还在第一次标定过程中
	{
		if (Moto_TouchDown[0]>=5&&Moto_TouchDown[1]>=5&&			//只有当所有轮子都触底5次后, 认为此时电机到达标定位置
			Moto_TouchDown[2]>=5&&Moto_TouchDown[3]>=5)
			{
				Abs_Angle_Init_Count++;								//可以删除, 原来是打算作为待机, 让代码不要那么快运行到之后的内容
				if (Abs_Angle_Init_Count==10)
				{
					Moto_TouchDown[0]=0;							//把触底计数清零, 准备第二次标定
					Moto_TouchDown[1]=0;
					Moto_TouchDown[2]=0;
					Moto_TouchDown[3]=0;
					moto_dir_ctl[0].abs_angle=0;					//把现在的位置设为绝对角度0
					moto_dir_ctl[1].abs_angle=0;
					moto_dir_ctl[2].abs_angle=0;
					moto_dir_ctl[3].abs_angle=0;
					TragetSpeed[0]=-TragetSpeed[0];					//反转目标速度, 让电机打到另一个标定上
					TragetSpeed[1]=-TragetSpeed[1];
					TragetSpeed[2]=-TragetSpeed[2];
					TragetSpeed[3]=-TragetSpeed[3];
					Angle_Init_flag=1;								//改变标定过程符号, 表示已经完成第一次标定
					Abs_Angle_Init_Count=0;							//计数清零
				}
			}
			else
			{
				 Abs_Angle_Init();									//当4个轮子还没同时触底5次以上的时候, 继续进行PID速度环输出, 同时判断是否触底
			}
	}else if(Angle_Init_flag==1)									//判断标定过程是否进入第二阶段
	{
		if (Moto_TouchDown[0]>=5&&Moto_TouchDown[1]>=5&&			//同上
			Moto_TouchDown[2]>=5&&Moto_TouchDown[3]>=5)
			{
				Abs_Angle_Init_Count++;
				if (Abs_Angle_Init_Count==10)
				{
					CAN1_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,3,CAN_Mode_Normal);		//初始化CAN1, 可以发送动力电机信号了
					Origin_Angle[0]=moto_dir_ctl[0].abs_angle/2;	//记录两次绝对角度的差值, 即为车轮现在的标定范围, 除2后就是指向前方的原始位置
					Origin_Angle[1]=moto_dir_ctl[1].abs_angle/2;
					Origin_Angle[2]=moto_dir_ctl[2].abs_angle/2;
					Origin_Angle[3]=moto_dir_ctl[3].abs_angle/2;
					/*
					8.6晚上写的, 待测*/
					Origin_Angle_check();
					/*
					检查Origin_Angle数据是否出现错误, 如果出现错误则丢弃, 保证AngleProportion数据不失真
					*/
					//AngleProportion=(Origin_Angle[0]+Origin_Angle[1]+Origin_Angle[2]+Origin_Angle[3])/4/150;
					//AngleProportion 通过标定计算出4个轮子的偏转角度为多大, 然后绝对值相加除以电机总数, 再除以转动角度
					Circle_Init(Speed_Mode);						//初始化位置环和速度环
					Angle_Init_flag=2;								//表示标定过程结束, 可以进入状态机环节
					USART3_DMA_Config(115200);;						//初始化串口, 可以和上位机通信
					StopMove();										//把目标速度方向设为原地待定的初始值
				}
			}
			else 
			{
				Abs_Angle_Init();									//同上
			}
	}
		
}

void Moto_TouchCheck(u8 Moto_Num)
{
	extern u16 Moto_TouchDown[4];
	if (moto_dir_ctl[Moto_Num].speed==0)	//如果此时电机的速度为0, 那么认为电机已经转到标定位置
	{
		Moto_TouchDown[Moto_Num]++;
	}
}

void Origin_Angle_check()
{
	int i=0;
	s32 temp[4]={0};							//暂存, 储存有效的Origin_Angle绝对值数据
	s32 temp_AngleProportion=0;					//暂存, 累加有效值, 计算temp_AngleProportion的数据
	u8 div=0;									//暂存, 计数有几个有效值
	
	for(i=0;i<4;i++)
	{
		if((Origin_Angle[i]>60000&&Origin_Angle[i]<75000)||(Origin_Angle[i]<-60000&&Origin_Angle[i]>-75000))
		{										//这个范围是我检测过程中记录过的数据, 正常合适的数据绝对值应该在67000到68000, 以后合适了后可以适当缩小范围
			if(Origin_Angle[i]<0)				//取绝对值
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
			temp[i]=-1;							//是无效位就设为-1, 其实还可以在这边打一个检测, 如果发现有无效值说明标定出现问题, 系统自动复位
		}
	}
	for(i=0;i<4;i++)
	{
		if(temp[i]!=0&&temp[i]!=-1)
		{
			temp_AngleProportion+=temp[i];		//累加有效值
			div++;
		}
	}AngleProportion=temp_AngleProportion/div/150;	//计算有效值
}


