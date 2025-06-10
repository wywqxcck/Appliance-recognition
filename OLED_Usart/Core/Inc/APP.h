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


#define ELE_MAX									6//����������
#define MENU_MAX									4//����������
#define SET_MAX									2//����������
#define DEBUG_CMD_NUM							3//������Ϣ����
#define PARAMETER_NUM							5//���������
#define ARROWS_X								110//ѡ���λ��
#define DLY_TIM_Handle							(&htim1)//��ʱ����ʱ΢�����



typedef struct
{
	uint8_t CurrentNum;	//��ǰ�������:ҳ��
	uint8_t Enter;		//ȷ�ϼ�
	uint8_t Next;		//��һ��
  uint8_t Last;		//��һ��
	uint8_t Return;		//���ؼ�
	void (*Current_Operation)(void);//��ǰ����(����ָ��)
}Menu_table_t;

enum{//������Ϣ
	PARAMETER,
	ID,
	Help,
}; 

enum{//������
	Interface_Mune,
	Study_Mune,
	Detection_Mune,
	Set_Mune,
	Timing_Mune,
};

enum{//��������
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



void Usart_Debug_Run(const char* data);//���ڻص�����
void Analytic_Parameter_NRF(char* data);
uint8_t NRF_Send(uint8_t* data,uint8_t tim_Count);
uint8_t Get_ADC (uint16_t count);
extern Menu_table_t taskTable[];
extern uint8_t taskstudy,my_index,index_num,old_index_num,Cell,send_flage,blank,set_flage,timing_flage,toggle_flage,taskIndex,gears;
extern int8_t direction;
extern uint32_t power_flage;
extern uint16_t timing_time,rot_speed;

#endif
