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
//	函数名称：	ESP8266_Clear
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
static void ESP8266_Clear(void)
{
	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

//==========================================================
//	函数名称：	ESP8266_WaitRecive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	ESP8266_REV_OK-接收完成		ESP8266_REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
static _Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return ESP8266_REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		esp8266_cnt = 0;							//清0接收计数
			
		return ESP8266_REV_OK;								//返回接收完成标志
	}
		
	esp8266_cntPre = esp8266_cnt;					//置为相同
	
	return ESP8266_REV_WAIT;								//返回接收未完成标志

}

//==========================================================
//	函数名称：	ESP8266_SendCmd
//
//	函数功能：	发送命令
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 40;
	if(cmd != NULL)
		HAL_UART_Transmit(&ESP8266_USART, (unsigned char *)cmd, strlen((const char *)cmd),1000);
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == ESP8266_REV_OK)							//如果收到数据
		{
			if(parse_wifi_info((char*)esp8266_buf,ESP8266_DataStruct.esp8266_id,sizeof(ESP8266_DataStruct.esp8266_id),
				ESP8266_DataStruct.esp8266_pass,sizeof(ESP8266_DataStruct.esp8266_pass)) == 1){
					osEventFlagsSet(myEvent01Handle, WRITE_W25Q64_EVEN);//内容存储
			}
			if(strstr((const char *)esp8266_buf, res) != NULL)		//如果检索到关键词
			{
				ESP8266_Clear();									//清空缓存
				return 0;
			}
		}
		osDelay(10);
	}
	return 1;
}



//==========================================================
//	函数名称：	ESP8266_SendData
//
//	函数功能：	发送数据
//
//	入口参数：	data：数据
//				len：长度
//
//	返回参数：	无
//
//	说明：		
//==========================================================
static void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[72];
	
	ESP8266_Clear();								//清空接收缓存
	sprintf(cmdBuf, "AT+CIPSEND=0,%d\r\n", len);		//发送命令
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//收到‘>’时可以发送数据
	{
		HAL_UART_Transmit(&ESP8266_USART, data, len,1000);		//发送设备连接请求数据
	}


}



// 函数：提取 msg= 后的值
static char *extractMsgValue(unsigned char *buffer) {
	char *ptrMsg = strstr((char*)buffer, "msg="); // 查找 "msg="
	if (ptrMsg != NULL) {
		ptrMsg += strlen("msg="); // 移动指针到 "msg=" 后面
		char *msgEnd = strchr(ptrMsg, '&'); // 查找下一个 '&'
		if (msgEnd != NULL) {
			*msgEnd = '\0'; // 结束字符串
		}
		uint8_t len = strlen(ptrMsg);
		if (len >= 3 && ptrMsg[len - 1] == '\n' && ptrMsg[len - 2] == '\r' && ptrMsg[len - 3] == '\r') {
				ptrMsg[len - 3] = '\0'; // 去掉 \r\n
		}
		return ptrMsg; // 返回 msg 的值
	}
	return NULL; // 未找到
}

// 函数：获取 IPD
static unsigned char *ESP8266_GetIPD(unsigned short timeOut) {
	do {
		osDelay(10); //延时等待
		if (ESP8266_WaitRecive() == ESP8266_REV_OK) { // 如果接收完成
			char *msgValue = extractMsgValue(esp8266_buf); // 提取 msg= 后的值
			if (msgValue != NULL) {
//				printf("msgValue = %s\n", msgValue); // 打印接收到的数据
				return (unsigned char *)msgValue; // 如果找到，返回 msg 的值
			}
		}
	} while (timeOut--); // 直到超时
	return NULL; // 超时还未找到，返回空指针
}



//==========================================================
// 函数名称：ESP8266_GetCIPStatus
// 函数功能：获取ESP8266的连接状态（AT+CIPSTATUS），返回整数状态码
// 入口参数：无
// 返回参数：状态码（见下方枚举/说明），-1表示失败/超时
// 说明：支持常见状态码解析，未识别的状态返回-2
//==========================================================
uint8_t ESP8266_GetCIPStatus(void) {
    uint8_t statusCode = 255;
    ESP8266_Clear();                          // 清空接收缓存
    
    // 发送 AT+CIPSTATUS 命令
			HAL_UART_Transmit(&ESP8266_USART, (unsigned char *)"AT+CIPSTATUS\r\n",strlen((const char *)"AT+CIPSTATUS\r\n"),1000);
			uint8_t timeout = 50;           // 50*10ms=500ms超时
			while (timeout--) {
					if (ESP8266_WaitRecive() == ESP8266_REV_OK) {
						printf("esp8266_buf = %s\r\n",esp8266_buf);
							// 解析 "STATUS:" 后的数值
							char* ptr = strstr((char*)esp8266_buf, "STATUS:");
							if (ptr != NULL) {
									ptr += strlen("STATUS:");   // 移动到状态值起始位置
									// 跳过可能的空格/换行符
									while (*ptr == ' ' || *ptr == '\r' || *ptr == '\n') ptr++;
									
									// 提取整数状态码
									if (*ptr >= '0' && *ptr <= '9') {
											statusCode = atoi(ptr);
											// 验证常见状态码范围（0-5）
											if (statusCode <= 5) {
													break;
											} else {
													statusCode = 255;  // 非法状态码（非0-5）
											}
									} else {
											statusCode = 255;      // 非数值状态
									}
							}
					}
					osDelay(10);                       // 延时10ms
			}
    ESP8266_Clear();                          // 清空缓存
    return statusCode;
}


//==========================================================
//	函数名称：	ESP8266_Init
//
//	函数功能：	初始化ESP8266
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
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
//	函数名称：	ESP8266_Con
//
//	函数功能：	串口接收数据处理
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		放入串口中断回调函数
//==========================================================

static void ESP8266_Con(void)
{
		if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //防止串口被刷爆
		esp8266_buf[esp8266_cnt++] = esp8266_buffer_buffer;
		HAL_UART_Receive_IT(&ESP8266_USART,&esp8266_buffer_buffer,1);
}



