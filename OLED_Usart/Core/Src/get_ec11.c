#include "get_ec11.h"

int8_t Get_EC11(void)
{
	int8_t data = -1;
	if(__HAL_TIM_GET_COUNTER(&EC11_TIME) != 0) {
		char Dir = __HAL_TIM_IS_TIM_COUNTING_DOWN(&EC11_TIME); //读取转动方向：0为正、1为负
		if(Dir){
			data = 0;
		}
		else{
			data = 1;
		}
		power_flage = 0;
		__HAL_TIM_GET_COUNTER(&EC11_TIME) = 0; //计数器值重新置位
	}
	return data;
}
	
