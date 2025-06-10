#ifndef __APP_H_
#define __APP_H_

#include "main.h"
#include "oled.h"
#include "stdio.h"
#include "get_ec11.h"
#include "debug_usart.h"
#include "math.h"
#include "tim.h"
#include "adc.h"
#include "24l01.h"


#define ELE_MAX									6//最大电器数量
#define MENU_MAX									4//最大电器数量
#define SET_MAX									2//最大电器数量
#define DEBUG_CMD_NUM							3//调试信息数量
#define PARAMETER_NUM							5//电参数数量
#define ARROWS_X								110//选项标位置
#define DLY_TIM_Handle							(&htim1)//定时器延时微妙级函数



typedef struct
{
	uint8_t CurrentNum;	//当前索引序号:页码
	uint8_t Enter;		//确认键
	uint8_t Next;		//下一个
  uint8_t Last;		//上一个
	uint8_t Return;		//返回键
	void (*Current_Operation)(void);//当前操作(函数指针)
}Menu_table_t;

enum{//调试信息
	PARAMETER,
	ID,
	Help,
}; 

enum{//界面编号
	Interface_Mune,
	Study_Mune,
	Detection_Mune,
	Set_Mune,
	Timing_Mune,
};

enum{//电参数编号
	U,
	I,
	PA,
	PF,
	EA,
	READ_ID,
	STUDY_OK,
	CLEAR_OK,
	I_PA_VPT,
};

enum{
	NO_STUDY,
	ING_STUDY,
	YES_STUDY,
	OK_STUDY,
	CLEAR_STUDY,
};

enum{
	NO_SET,
	YES_SET,
	ING_SET,
	OK_SET,
};

enum{
	TIMING_SET,
	TIMING_SEND,
};



void Usart_Debug_Run(const char* data);//串口回调函数
void Analytic_Parameter_NRF(char* data);
uint8_t NRF_Send(uint8_t* data,uint8_t tim_Count);
uint8_t Get_ADC (uint16_t count);
extern Menu_table_t taskTable[];
extern uint8_t taskstudy,my_index,index_num,old_index_num,Cell,send_flage,blank,set_flage,timing_flage,toggle_flage,taskIndex,gears;
extern int8_t direction;
extern uint32_t power_flage;
extern uint16_t timing_time,rot_speed;

#endif
