#include "esp8266.h"


static void ESP8266_SendData(unsigned char *data, unsigned short len);
static void ESP8266_Init(void);
static unsigned char *ESP8266_GetIPD(unsigned short timeOut);
static void ESP8266_Con(void);
_Bool ESP8266_SendCmd(char *cmd, char *res);


ESP8266_DataTypeDef ESP8266_DataStruct = {
	.esp8266_pass = "19337839060",
	.esp8266_id = "Lemon",
};

Bemfa_DataTypedef Bemfa_DataStruct = {
	.ip = "bemfa.com",
	.port = "8344",
	.uid = "c4174f887b424c35bf2da9d275d0530d",
	.name = "test",
};

ESP8266_Typedef ESP8266_Struct = {
	.Init = ESP8266_Init,
	.Read_Data = ESP8266_GetIPD,
	.Send_Data = ESP8266_SendData,
	.SendCmd = ESP8266_SendCmd,
	.Con = ESP8266_Con,
};

unsigned char esp8266_buf[ESP8266_BUF_MAX];
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;
uint8_t esp8266_buffer_buffer=0;

//==========================================================
//	�������ƣ�	ESP8266_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
static void ESP8266_Clear(void)
{
	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

//==========================================================
//	�������ƣ�	ESP8266_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	ESP8266_REV_OK-�������		ESP8266_REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
static _Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return ESP8266_REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		esp8266_cnt = 0;							//��0���ռ���
			
		return ESP8266_REV_OK;								//���ؽ�����ɱ�־
	}
		
	esp8266_cntPre = esp8266_cnt;					//��Ϊ��ͬ
	
	return ESP8266_REV_WAIT;								//���ؽ���δ��ɱ�־

}

//==========================================================
//	�������ƣ�	ESP8266_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 40;
	if(cmd != NULL)
		HAL_UART_Transmit(&ESP8266_USART, (unsigned char *)cmd, strlen((const char *)cmd),1000);
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == ESP8266_REV_OK)							//����յ�����
		{
			if(parse_wifi_info((char*)esp8266_buf,ESP8266_DataStruct.esp8266_id,sizeof(ESP8266_DataStruct.esp8266_id),
				ESP8266_DataStruct.esp8266_pass,sizeof(ESP8266_DataStruct.esp8266_pass)) == 1){
					osEventFlagsSet(myEvent01Handle, WRITE_W25Q64_EVEN);//���ݴ洢
			}
			if(strstr((const char *)esp8266_buf, res) != NULL)		//����������ؼ���
			{
				ESP8266_Clear();									//��ջ���
				return 0;
			}
		}
		osDelay(10);
	}
	return 1;
}



//==========================================================
//	�������ƣ�	ESP8266_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����		
//==========================================================
static void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[72];
	
	ESP8266_Clear();								//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=0,%d\r\n", len);		//��������
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//�յ���>��ʱ���Է�������
	{
		HAL_UART_Transmit(&ESP8266_USART, data, len,1000);		//�����豸������������
	}


}



// ��������ȡ msg= ���ֵ
static char *extractMsgValue(unsigned char *buffer) {
	char *ptrMsg = strstr((char*)buffer, "msg="); // ���� "msg="
	if (ptrMsg != NULL) {
		ptrMsg += strlen("msg="); // �ƶ�ָ�뵽 "msg=" ����
		char *msgEnd = strchr(ptrMsg, '&'); // ������һ�� '&'
		if (msgEnd != NULL) {
			*msgEnd = '\0'; // �����ַ���
		}
		uint8_t len = strlen(ptrMsg);
		if (len >= 3 && ptrMsg[len - 1] == '\n' && ptrMsg[len - 2] == '\r' && ptrMsg[len - 3] == '\r') {
				ptrMsg[len - 3] = '\0'; // ȥ�� \r\n
		}
		return ptrMsg; // ���� msg ��ֵ
	}
	return NULL; // δ�ҵ�
}

// ��������ȡ IPD
static unsigned char *ESP8266_GetIPD(unsigned short timeOut) {
	do {
		osDelay(10); //��ʱ�ȴ�
		if (ESP8266_WaitRecive() == ESP8266_REV_OK) { // ����������
			char *msgValue = extractMsgValue(esp8266_buf); // ��ȡ msg= ���ֵ
			if (msgValue != NULL) {
//				printf("msgValue = %s\n", msgValue); // ��ӡ���յ�������
				return (unsigned char *)msgValue; // ����ҵ������� msg ��ֵ
			}
		}
	} while (timeOut--); // ֱ����ʱ
	return NULL; // ��ʱ��δ�ҵ������ؿ�ָ��
}



//==========================================================
// �������ƣ�ESP8266_GetCIPStatus
// �������ܣ���ȡESP8266������״̬��AT+CIPSTATUS������������״̬��
// ��ڲ�������
// ���ز�����״̬�루���·�ö��/˵������-1��ʾʧ��/��ʱ
// ˵����֧�ֳ���״̬�������δʶ���״̬����-2
//==========================================================
uint8_t ESP8266_GetCIPStatus(void) {
    uint8_t statusCode = 255;
    ESP8266_Clear();                          // ��ս��ջ���
    
    // ���� AT+CIPSTATUS ����
			HAL_UART_Transmit(&ESP8266_USART, (unsigned char *)"AT+CIPSTATUS\r\n",strlen((const char *)"AT+CIPSTATUS\r\n"),1000);
			uint8_t timeout = 50;           // 50*10ms=500ms��ʱ
			while (timeout--) {
					if (ESP8266_WaitRecive() == ESP8266_REV_OK) {
						printf("esp8266_buf = %s\r\n",esp8266_buf);
							// ���� "STATUS:" �����ֵ
							char* ptr = strstr((char*)esp8266_buf, "STATUS:");
							if (ptr != NULL) {
									ptr += strlen("STATUS:");   // �ƶ���״ֵ̬��ʼλ��
									// �������ܵĿո�/���з�
									while (*ptr == ' ' || *ptr == '\r' || *ptr == '\n') ptr++;
									
									// ��ȡ����״̬��
									if (*ptr >= '0' && *ptr <= '9') {
											statusCode = atoi(ptr);
											// ��֤����״̬�뷶Χ��0-5��
											if (statusCode <= 5) {
													break;
											} else {
													statusCode = 255;  // �Ƿ�״̬�루��0-5��
											}
									} else {
											statusCode = 255;      // ����ֵ״̬
									}
							}
					}
					osDelay(10);                       // ��ʱ10ms
			}
    ESP8266_Clear();                          // ��ջ���
    return statusCode;
}


//==========================================================
//	�������ƣ�	ESP8266_Init
//
//	�������ܣ�	��ʼ��ESP8266
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
static void ESP8266_Init(void)
{
	char temp[256];
	uint8_t time_out = 0;
	uint8_t wifi_sta = 255;
	
	HAL_UART_Receive_IT(&ESP8266_USART,&esp8266_buffer_buffer,1);
	ESP8266_Clear();
	myPrintf("+++\r\n");
	
	while(ESP8266_SendCmd("+++", "+++"))
		osDelay(500);
	HAL_UART_Transmit(&ESP8266_USART, (unsigned char *)"AT+RST\r\n", strlen((const char *)"AT+RST\r\n"),1000);
		osDelay(500);
	
	HAL_UART_Transmit(&ESP8266_USART, (unsigned char *)"AT+CWQAP\r\n", strlen((const char *)"AT+CWQAP\r\n"),1000);
		osDelay(500);
	
	HAL_UART_Transmit(&ESP8266_USART, (unsigned char *)"AT+CIPCLOSE\r\n", strlen((const char *)"AT+CIPCLOSE\r\n"),1000);
		osDelay(500);
	
	myPrintf("1. AT\r\n");
	while(ESP8266_SendCmd("AT\r\n", "OK"))
		osDelay(500);
	
	myPrintf("2. CWMODE\r\n");
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
		osDelay(500);
	
	
	if(strcmp(ESP8266_DataStruct.esp8266_id ,"NULL") != 0){
		LED_Struct.Set_Blink(1000);
		myPrintf("3. CONNECT\r\n");
		sprintf(temp,"AT+CWJAP=\"%s\",\"%s\"\r\n",ESP8266_DataStruct.esp8266_id,ESP8266_DataStruct.esp8266_pass);
		while(wifi_sta != 2){
			if(wifi_sta == 5){
				HAL_UART_Transmit(&ESP8266_USART, (unsigned char *)"AT+CIPCLOSE\r\n", strlen((const char *)"AT+CIPCLOSE\r\n"),1000);
				osDelay(500);
			}
			time_out++;
			if(time_out >= 10){
				osEventFlagsSet(myEvent01Handle, WIFI_REST_EVEN);
			}
			HAL_UART_Transmit(&ESP8266_USART, (unsigned char *)temp, strlen((const char *)temp),1000);
			osDelay(2000);
			wifi_sta = ESP8266_GetCIPStatus();
		}
	}
	else{
		LED_Struct.Set_Blink(100);
		myPrintf("3. CWSTARTSMART\r\n");
		while(ESP8266_SendCmd("AT+CWSTARTSMART\r\n", "OK"))
			osDelay(500);
		while(ESP8266_SendCmd(NULL, "smartconfig connected wifi"))
			osDelay(500);
		while(ESP8266_SendCmd("AT+CWSTOPSMART\r\n", "OK"))
			osDelay(500);
	}
	
	LED_Struct.Set_Blink(400);
	
	myPrintf("4. AT+CIPMODE\r\n");
	time_out = 0;
	while(ESP8266_SendCmd("AT+CIPMODE=1\r\n", "OK")){
		time_out++;
		if(time_out >= 10){
			osEventFlagsSet(myEvent01Handle, WIFI_REST_EVEN);
		}
		osDelay(500);
	}
	
	myPrintf("5. AT+CIPSTART\r\n");
	time_out = 0;
	sprintf(temp,"AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",Bemfa_DataStruct.ip,Bemfa_DataStruct.port);
	while(ESP8266_SendCmd(temp, "ALREADY CONNECTED")){
		time_out++;
		if(time_out >= 10){
			osEventFlagsSet(myEvent01Handle, WIFI_REST_EVEN);
		}
		osDelay(500);
	}

	myPrintf("6. AT+CIPSEND\r\n");
	time_out = 0;
	while(ESP8266_SendCmd("AT+CIPSEND\r\n", ">")){
		time_out++;
		if(time_out >= 10){
			osEventFlagsSet(myEvent01Handle, WIFI_REST_EVEN);
		}
		osDelay(500);
	}
	
	myPrintf("7. AT+CIPSERVER\r\n");
	time_out = 0;
	sprintf(temp,"cmd=1&uid=%s&topic=%s\r\n",Bemfa_DataStruct.uid,Bemfa_DataStruct.name);
	while(ESP8266_SendCmd(temp,"cmd=1&res=1")){
		time_out++;
		if(time_out >= 10){
			osEventFlagsSet(myEvent01Handle, WIFI_REST_EVEN);
		}
		osDelay(500);
	}
	
	myPrintf("8. ESP8266 Init OK\r\n");

}

//==========================================================
//	�������ƣ�	ESP8266_Con
//
//	�������ܣ�	���ڽ������ݴ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		���봮���жϻص�����
//==========================================================

static void ESP8266_Con(void)
{
		if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //��ֹ���ڱ�ˢ��
		esp8266_buf[esp8266_cnt++] = esp8266_buffer_buffer;
		HAL_UART_Receive_IT(&ESP8266_USART,&esp8266_buffer_buffer,1);
}



