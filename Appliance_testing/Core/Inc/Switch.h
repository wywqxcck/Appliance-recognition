#ifndef __SWITCH_H_
#define __SWITCH_H_


#include "main.h"
#include "cmsis_os.h"

#define ON  			1
#define OFF				0


typedef struct{
	void (*Init)(void);
	void (*Set)(uint8_t data);
	uint8_t (*Get)(void);
	void (*Con)(void);
}Switch_Typedef;

extern Switch_Typedef Switch_Struct;


#endif
