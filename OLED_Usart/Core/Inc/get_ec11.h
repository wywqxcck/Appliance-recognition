#ifndef __EC11_H_
#define __EC11_H_

#include "main.h"
#include "tim.h"
#include "APP.h"

#if 0

#define FWD   	1		//��ת
#define REV   	0		//��ת
#define STRT 	-1		//ֹͣ
#else

#define FWD   	0		//��ת
#define REV   	1		//��ת
#define STRT 	  -1		//ֹͣ

#endif

#define EC11_TIME htim2




int8_t Get_EC11(void);



#endif

