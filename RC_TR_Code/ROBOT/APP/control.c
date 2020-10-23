#include "main.h"
/*************************
函数名：Control
函数功能：主控制函数 状态机切换
描述： control_task()包含函数有保护监测函数Check_Task() 工作状态切换函数Work_Execute() LED模式切换函数Led_State()
并判断小车是否为正常工作模式 每20ms向上位机传输一次数据 
工作执行函数Work_Execute()包括以下七个模式：
	预备模式(PREPARE_STATE)：绿灯常亮 没实际作用 有必要可添加。
	自检模式(CHECK_STATE)：绿灯常亮 300ms后进入标定模式 如果进入自检模式3000次(每进一次time_count加一) 那么进行软件复位
	标定模式(CALI_STATE)：绿灯常亮 执行标定函数 经过1s标定结束 进入正常操作模式
	正常操作模式(NORMAL_STATE)：绿灯慢闪 进入Work_start()函数 

	错误模式(ERROR_STATE):红灯常亮 只用于判定底盘是否正常 先将所有输出置零 再次监测底盘 若底盘有问题 则跳回自检模式 没有问题 则跳回正常模式
	如果进入错误模式3000次 那么进行软件复位

	保护模式(PROTECT_STATE):红灯快闪 只用于判定底盘能否从上位机收到数据 先将所有输出置零 再次监测上位机接收数据 若底盘有问题 则跳回自检模式 没有问题 则跳回正常模式
	如果进入保护模式3000次 那么进行软件复位

	CAN2丢失模式(LOST_CAN2_STATE)：红绿灯交替闪烁 只用于CAN2能否接收到数据 先将所有输出置零 再次监测CAN2接收数据 若底盘有问题 则跳回自检模式 没有问题 则跳回正常模式
	如果进入CAN2丢失模式3000次 那么进行软件复位
未测试 2020/5/26
*************************/
WorkState_e workState=PREPARE_STATE;		  //初始为预备模式
						  

u32 time_1ms_count=0;						  //1ms一次 自检模式专用时间计时
u8 Workstate=0;								  //工作标志位 1为正常工作模式 0为其他模式


void control_task()
{	
	static u8 time=0;//系统总时间1ms一次
	time_1ms_count++; 
	time++;
	Check_Task();                             //保护检测
	Work_Execute();	                          //错误保护，工作状态自动执行
	Led_State();                              //led模式切换
	
	if(GetWorkState()==NORMAL_STATE)		  //判断是否为正常工作模式
	{
		Workstate=1;
	}
	else
	{
		Workstate=0;
	}
	
	if(time%10==0)
	{
		mainboard_send();				  //向上位机发送数据
		time=0;
	}
}	


/***************工作执行**********************/
void Work_Execute(void)
{
	switch(GetWorkState())
	{	
		case PREPARE_STATE:                   //预备模式 绿灯常亮
		{
			StopMove();
			SetWorkState(NORMAL_STATE);		  //初始化, 其实应该是设置到CHECK_STATE, 进行检测, 成功后再到NORMAL_STATE, 现在调试中, 就放着
			break;
		}
		case CHECK_STATE:                     //自检模式，如出现错误，可通过软复位修复  通信是否断开 电机反馈的值是否正常  绿灯常亮
		{	
			static u32 time_count=0;
			time_count++;	
			if(time_1ms_count>300)
			{	
				SetWorkState(NORMAL_STATE);
				if(time_count>3000)			  //回到自检模式多次
				{
					time_count=0;
					NVIC_SystemReset();
				}						
			}
			break;
		}
		case NORMAL_STATE:                    //正常操作模式，执行上位机指令  绿灯慢闪
		{
			Move_Mode_Check();
			break;
		}/*
		case ERROR_STATE:					  //判定底盘是否正常	红灯常亮
		{
			static u32 time_count_chassis=0;
			time_count_chassis++;
			if(Error_Check.statu[LOST_LEFT_FRONT_Angle]==0&&Error_Check.statu[LOST_RIGHT_FRONT_Angle]==0&&Error_Check.statu[LOST_RIGHT_BACK_Angle]==0&&Error_Check.statu[LOST_LEFT_BACK_Angle]==0)
			{
				SetWorkState(NORMAL_STATE);
			}
			else
			{
				SetWorkState(CHECK_STATE);
			}
			if(time_count_chassis>3000)
			{
				time_count_chassis=0;
				NVIC_SystemReset();
			}
			break;
		}
		case PROTECT_STATE:					  //判定主副板连接   红灯快闪
		{
			static u32 time_count_transmission=0;
			time_count_transmission++;
			if(Error_Check.statu[LOST_TRANSMISSION]==0)
			{
				SetWorkState(NORMAL_STATE);
			}
			else
			{
				SetWorkState(CHECK_STATE);
			}
			if(time_count_transmission>3000)  //多次进入该模式 系统重置
			{
					time_count_transmission=0;
					NVIC_SystemReset();
			}
			break;
		}
		case LOST_CAN2_STATE:				  //红绿灯交替闪烁  CAN2电机接受数据出错
		{
			static u32 time_count_can2=0;
			time_count_can2++;
			if(Error_Check.statu[LOST_KICK]==0)
			{
				SetWorkState(NORMAL_STATE);
			}
			else
			{
				SetWorkState(CHECK_STATE);
			}
			if(time_count_can2>3000)
			{
				time_count_can2=0;
				NVIC_SystemReset();
			}
			break;
		}*/
		default:
			break;
	}
}


/******************LED指示灯**********************/

extern u32 led_num;
void Led_State(void)
{
	switch(GetWorkState())
	{
		case PREPARE_STATE:                //预备模式
		{
			LED_GREEN_ON();                //绿灯常亮 
			LED_RED_OFF();
			break;
		}
		case CHECK_STATE:                  //自检模式，如出现错误，可通过软复位修复         绿灯常开
		{
			LED_GREEN_ON();
			LED_RED_OFF();
			break;
		}
		case NORMAL_STATE:                 //正常操作模式，可切换双模式，陀螺仪控制/电机位置控制，陀螺仪需多次采样再次标定，再切换到陀螺仪控制
		{	
			if(led_num%10==0)
			{
				LED_RED_OFF();             //绿灯快闪
				LED_GREEN_ON();
			}
			else 
			{			
				LED_RED_OFF();
			    LED_GREEN_OFF();
			}
			break;
		}
		case PROTECT_STATE:                 
		{
			if(led_num%10==0)
			{
				LED_RED_OFF();              //红灯快闪
				LED_GREEN_OFF();
			}
			else 
			{
				LED_RED_ON();
				LED_GREEN_OFF();
			}
			break;
		}
		case LOST_CAN2_STATE:			    //红绿灯交替闪烁
		{
			if(led_num%10==0)
			{
				LED_RED_OFF();
				LED_GREEN_ON();
			}
			else 
			{
				LED_RED_ON();
			    LED_GREEN_OFF();
			}
			break;
		}
		case ERROR_STATE:                   //错误模式
		{
			LED_RED_ON();                   //红灯常亮报警
			LED_GREEN_OFF();
			break;
		}
		default:
			LED_RED_OFF();                  //灯全灭
			LED_GREEN_OFF();
			break;
	}
}

/***********************--工作模式--**********************/
void SetWorkState(WorkState_e state) 		//设定工作模式
{
    workState = state;
}


WorkState_e GetWorkState(void)          	//得到工作模式
{
	return workState;
}
