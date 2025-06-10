#include "led.h"


static uint16_t Get_Led_Blink(void);
static void Set_Led_Blink(uint16_t data);
static void Led_Con(void);
static LED_STATE Get_Led_Mode(void);
static void Set_Led_Mode(LED_STATE data);
static void Led_Toggle(void);



static LED_STATE Led_state = LED_OFF;
uint16_t blink_time = 200;


LED_Typedef LED_Struct = {
	.Set_Mode = Set_Led_Mode,
	.Get_Mode = Get_Led_Mode,
	.Get_Blink = Get_Led_Blink,
	.Set_Blink = Set_Led_Blink,
	.Con = Led_Con,
};
	

static void Led_On(void)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_RESET);
} 

static void Led_Off(void)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_SET);
} 

static void Led_Toggle(void)
{
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

static LED_STATE Get_Led_Mode(void)
{
	return Led_state;
}

static void Set_Led_Mode(LED_STATE data)
{
	Led_state = data;
}


static uint16_t Get_Led_Blink(void)
{
	return blink_time;
}

static void Set_Led_Blink(uint16_t data)
{
	blink_time = data;
}

static void Led_Con(void)
{
	static uint32_t count = 0;
	switch(Get_Led_Mode()){
		case LED_OFF:Led_Off();break;
		case LED_ON:Led_On();break;
		case LED_Blink: 
				count++;
				if(count >= blink_time){
					Led_Toggle();
					count = 0;
				}
			break;
	}
}

