#include "Switch.h"


uint8_t switch_flage = OFF;
uint8_t old_switch_flage = OFF;


static void Switch_Init(void);
static void Switch_Set(uint8_t);
static uint8_t Switch_Get(void);
static void Switch_Con(void);

Switch_Typedef Switch_Struct = {
	.Init = Switch_Init,
	.Set = Switch_Set,
	.Get = Switch_Get,
	.Con = Switch_Con,
};



static void Switch_Init(void)
{
	if(switch_flage == ON){
		HAL_GPIO_WritePin(GPIOA, MDINA_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, MDINB_Pin, GPIO_PIN_SET);
		HAL_Delay(200);
		HAL_GPIO_WritePin(GPIOA, MDINA_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, MDINB_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(Switch_Led_GPIO_Port,Switch_Led_Pin,GPIO_PIN_RESET);
	}
	else{
		HAL_GPIO_WritePin(GPIOA, MDINB_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, MDINA_Pin, GPIO_PIN_SET);
		HAL_Delay(200);
		HAL_GPIO_WritePin(GPIOA, MDINA_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, MDINB_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(Switch_Led_GPIO_Port,Switch_Led_Pin,GPIO_PIN_SET);
	}
	old_switch_flage = switch_flage;
}


static void Switch_Set(uint8_t data)
{
	switch_flage = data;
}

static uint8_t Switch_Get(void)
{
	return switch_flage;
}

static void Switch_Con(void)
{
	if(old_switch_flage != switch_flage){
		static uint8_t Count_Switch = 0;
		if(switch_flage == ON){
			HAL_GPIO_WritePin(GPIOA, MDINA_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, MDINB_Pin, GPIO_PIN_SET);
		}
		else{
			HAL_GPIO_WritePin(GPIOA, MDINB_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, MDINA_Pin, GPIO_PIN_SET);
		}
		Count_Switch++;
		if(Count_Switch >= 120){
			HAL_GPIO_WritePin(Switch_Led_GPIO_Port,Switch_Led_Pin,(GPIO_PinState)(!switch_flage));
			HAL_GPIO_WritePin(GPIOA, MDINA_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, MDINB_Pin, GPIO_PIN_SET);
			old_switch_flage = switch_flage;
			Count_Switch = 0;
			
		}
	}
}




