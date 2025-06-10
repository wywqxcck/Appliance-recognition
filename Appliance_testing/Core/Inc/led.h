#ifndef __LED_H_
#define __LED_H_

#include "main.h"
#include "gpio.h"


typedef enum{
	LED_ON,
	LED_OFF,
	LED_Blink,
}LED_STATE;

typedef struct{
	void (*Set_Mode)(LED_STATE data);
	LED_STATE (*Get_Mode)(void);
	void (*Set_Blink)(uint16_t data);
	uint16_t (*Get_Blink)(void);
	void (*Con) (void);
}LED_Typedef;


extern LED_Typedef LED_Struct;

#endif






