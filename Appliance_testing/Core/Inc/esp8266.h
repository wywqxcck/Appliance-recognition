#ifndef _ESP8266_H_
#define _ESP8266_H_


#include "main.h"

//Ӳ������
#include "usart.h"
#include "debug_usart.h"
#include "cmsis_os.h"
#include "APP.h"

//C��
#include <string.h>
#include <stdio.h>


#define ESP8266_REV_OK				0	//������ɱ�־
#define ESP8266_REV_WAIT			1	//����δ��ɱ�־
#define ESP8266_BUF_MAX  			512
#define ESP8266_USART  				huart3


typedef struct{//ESP8266����WiFi��Ϣ
	char esp8266_pass[20];
	char esp8266_id[10];
}ESP8266_DataTypeDef;

typedef struct{
	char* ip;
	char* port;
	char* uid;
	char* name;
}Bemfa_DataTypedef;

typedef struct{
	void (*Init)(void);
	void (*Send_Data)(unsigned char *data, unsigned short len);
	unsigned char* (*Read_Data)(unsigned short timeOut);
	_Bool (*SendCmd)(char *cmd, char *res);
	void (*Con)(void);
}ESP8266_Typedef;

uint8_t ESP8266_GetCIPStatus(void);

extern ESP8266_Typedef ESP8266_Struct;
extern ESP8266_DataTypeDef ESP8266_DataStruct;
extern Bemfa_DataTypedef Bemfa_DataStruct;

#endif
