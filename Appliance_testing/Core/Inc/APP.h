#ifndef __APP_H_
#define __APP_H_


#include "main.h"
#include "HLW8110.h"
#include "KNN.h"
#include "math.h"
#include "debug_usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Switch.h"
#include "cmsis_os.h"
#include "led.h"
#include "24L01.h"
#include <stdlib.h>
#include "esp8266.h"
#include "w25qxx.h"




#define READ_DATA_NUM												5//读取参数个数
#define VPT_NUM															2//阈值个数
#define DEBUG_CMD_NUM												13//调试信息数量
#define PA_MIN															1//功率最小值
#define READ_DATA_TIME											50//读取电参数时间间隔
#define PRINT_LIST_EVEN	    								(1 << 0)//打印事件
#define ENTER_STUDY_EVEN    								(1 << 1)//进入学习模式事件
#define QUIT_STUDY_EVEN    									(1 << 2)//退出学习模式事件
#define WRITE_W25Q64_EVEN    								(1 << 3)//保存模式事件
#define POWER_DOWN_EVEN		    							(1 << 4)//掉电模式事件
#define SWITCH_TOGGLE_EVEN		    					(1 << 5)//按键切换事件
#define WIFI_REST_EVEN		    							(1 << 6)//WIFI重启事件
#define TIME_START_EVEN		    					  (1 << 7)//定时时开始
#define TIME_OUT_EVEN		    							  (1 << 8)//定时时间到

extern osEventFlagsId_t myEvent01Handle;
extern osMessageQueueId_t IDQueueHandle;
extern osMutexId_t SPIMutexHandle;
extern osTimerId_t Timing_TimerHandle;
extern osMessageQueueId_t Clear_IDHandle;
extern osMessageQueueId_t Wifi_DataHandle;
extern osMessageQueueId_t TimeingQueueHandle;

uint8_t Read_HLW8110_Electrical(float* data);
void Printf_parameter(float* data);
void Usart_Debug_Run(const char* data);
void Infor_analysis(const char* data);
void Write_W25Q(void);
void Read_W25Q(void);
void VPT_Monitor(float* data);
uint8_t NRF_Send(uint8_t* data,uint8_t tim_Count);
void Clear_W25Q(uint8_t num);
void Wifi_Read(char* data);
void Wifi_Send(float* parameter,uint8_t id);
void extract_data(char *input);
uint8_t parse_wifi_info(const char *input, char *ssid, size_t ssid_size, char *password, size_t password_size);


//调试信息
enum{
	List,
	Help,
	ENTER_STUDY,
	QUIT_STUDY,
	SWITCH_ON,
	SWITCH_OFF,
	SWITCH_GET,
	ID,
	SWITCH_TOGGLE,
	CLEAR_ID,
	I_PA_VPT,
	READ_I_PA_VPT,
	TIMING,
}; 



//电参数对应位置
enum{
	U,
	I,
	PA,
	PF,
	EA,
};


enum{
	VPT_I,
	VPT_PA,
};


#endif

