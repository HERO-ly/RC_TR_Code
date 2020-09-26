#include "main.h"

/*--------------------
创建人:
	李烨

工程名:
	RC_8电机底盘驱动

创建时间:
	2019/12/9

修改时间:
	2019/12/24

程序功能:
	1.利用不同闪烁方式的LED灯表示程序不同的状态;

--------------------*/
/*流程:
	BSP初始化各种外设->使能了定时器与CAN1->开始标定初始化角度->初始化CAN2与8个PID参数->进入遥控器控制
*/
motoinfo moto_dir_ctl[5];				//方向电机反馈信号
motoinfo moto_speed_ctl[4];				//动力电机反馈信号
motoinfo moto_dir_ctl_temp;
motoinfo moto_speed_ctl_temp;
int main(void)
{
	BSP_Init();
	while(1)
	{
		;
	}
}



