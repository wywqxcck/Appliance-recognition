#ifndef __DEUBG_USART_H_
#define __DEUBG_USART_H_

#include "main.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"

#define DEBUG_DMA_IDLE						1
#define DEBUG_USART 						huart1
#define DEBUG_READ_OK		    			1
#define DEBUG_READ_WAIT		  				0
#define DEBUG_DATA_MAX						32
#define DEBUG_SWITCH   					    1//打印开关 打开：1  关闭：0



#if DEBUG_DMA_IDLE
	extern DMA_HandleTypeDef hdma_usart1_rx;
	#define DEBUG_HDMA_RX							hdma_usart1_rx
#endif



#define myPrintf(...) \
do{                                   				\
		if(DEBUG_SWITCH){													\
			printf("[%s()-%d]",__func__,__LINE__); 	\
			printf(__VA_ARGS__);              		 	\
		}																					\
}while(0)


typedef struct{
	uint8_t rx_data[DEBUG_DATA_MAX];
	uint16_t rx_len : 15;
	uint16_t rx_flage : 1;
}Debug_Usart_Data;



typedef struct{
	void (*Init)(void (*cmd)(const char* data));
	void (*Clear)(void);
	_Bool (*Read)(char* data);
	void (*Con) (void);
	void (*Cmd)(const char* data);
}Debug_Usart_Typedef;




extern Debug_Usart_Typedef Debug_Usart_Struct;

#endif
