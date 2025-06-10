#ifndef __EC11_H_
#define __EC11_H_

#include "main.h"
#include "tim.h"
#include "APP.h"

#if 0

#define FWD   	1		//正转
#define REV   	0		//反转
#define STRT 	-1		//停止
#else

#define FWD   	0		//正转
#define REV   	1		//反转
#define STRT 	  -1		//停止

#endif

#define EC11_TIME htim2




int8_t Get_EC11(void);



#endif

