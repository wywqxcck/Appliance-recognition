#include "APP.h"


char *Debug_Cmd[ DEBUG_CMD_NUM ] = { "List","help","enter_study","quit_study","Switch ON","Switch OFF","Switch?","ID:","Switch Toggle","Clear:",
																		"I_PA_VPT?","I_PA_VPT:","Timing:"};//调试信息
float VPT_Arr[VPT_NUM] = { 3,2200 };//关闭阈值
float EA_NUM = 1.214014;


/*=========================================================================================================
 * Function : uint8_t Read_HLW8110_Electrical(float* data)
 * Describe : 读取HW110模块信息
 * Input    : data:读取后数据保存位置
 * Return   : OK:读取完成		ERR:读取失败
 * Record   : 2024/10/09
==========================================================================================================*/
uint8_t Read_HLW8110_Electrical(float* data)
{
	static uint8_t power_count = 0;
	data[U] = HLW8110_Read_U();
	data[I] = HLW8110_Read_IA();
	data[PA] = HLW8110_Read_PA();
	data[PF] = HLW8110_Read_PF();
	data[EA] = HLW8110_Read_EA();
	EA_NUM += data[EA];
	data[EA] = EA_NUM;
	VPT_Monitor(data);
	if(data[U] < 190){
		power_count++;
	}
	else{
		power_count = 0;
	}
	if(power_count >= 3){
		//osEventFlagsSet(myEvent01Handle, POWER_DOWN_EVEN);
	}
	if(data[PA] < PA_MIN){
		return Err;
	}
	return OK;
}



/*=========================================================================================================
 * Function : void VPT_Monitor(float* data)
 * Describe : 检测是否超过阈值
 * Input    : data:需要检测的数据
 * Return   : nono
 * Record   : 2024/10/09
==========================================================================================================*/
void VPT_Monitor(float* data)
{
	if(data[I] > VPT_Arr[VPT_I] || data[PA] > VPT_Arr[VPT_PA]){
		Switch_Struct.Set(OFF);
	}
	if(data[I] > VPT_Arr[VPT_I]){
		printf("data[I] = %f\r\nVPT_Arr[VPT_I] = %f\r\n",data[I],VPT_Arr[VPT_I]);
		printf("I_ERR\r\n");
	}
	if(data[PA] > VPT_Arr[VPT_PA]){
		printf("PA_ERR\r\n");
	}
}



/*=========================================================================================================
 * Function : void Printf_parameter(float* data)
 * Describe : 电参数输出（通过串口和2.4G发送）
 * Input    : data:需要输出的数据
 * Return   : nono
 * Record   : 2024/10/09
==========================================================================================================*/
void Printf_parameter(float* data)
{
	uint8_t NRF_TX_Data[TX_PLOAD_WIDTH];
//	printf("\r\nU:%.2f\r\n",data[U]);
//	printf("I:%f\r\n",data[I]);
//	printf("PA:%f\r\n",data[PA]);
//	printf("PF:%.2f\r\n",data[PF]);
//	printf("EA:%f\r\n",data[EA]);
	NRF24L01_TX_Mode();
	osDelay(100);
	sprintf((char*)NRF_TX_Data,"U:%.2f",data[U]);//电压
	NRF24L01_TxPacket(NRF_TX_Data);
	memset(NRF_TX_Data,0,sizeof(NRF_TX_Data));
	osDelay(100);
	sprintf((char*)NRF_TX_Data,"I:%.2f",data[I]);//电流
	NRF24L01_TxPacket(NRF_TX_Data);
	memset(NRF_TX_Data,0,sizeof(NRF_TX_Data));
	osDelay(100);
	sprintf((char*)NRF_TX_Data,"PA:%.2f",data[PA]);//功率
	NRF24L01_TxPacket(NRF_TX_Data);
	memset(NRF_TX_Data,0,sizeof(NRF_TX_Data));
	osDelay(100);
	sprintf((char*)NRF_TX_Data,"PF:%.2f",data[PF]);//功率因数
	NRF24L01_TxPacket(NRF_TX_Data);
	memset(NRF_TX_Data,0,sizeof(NRF_TX_Data));
	osDelay(100);
	sprintf((char*)NRF_TX_Data,"EA:%f",data[EA]);//电能
	NRF24L01_TxPacket(NRF_TX_Data);
	memset(NRF_TX_Data,0,sizeof(NRF_TX_Data));
	osDelay(100);
	NRF24L01_RX_Mode();
}



/*=========================================================================================================
 * Function : int8_t Hex_To_Dec(const char* data)
 * Describe : 提取字符串中的数字（有符号）
 * Input    : data:需要提取的字符串
 * Return   : 字符串的数据 0xFF表示失败
 * Record   : 2024/10/09
==========================================================================================================*/
int8_t Hex_To_Dec(const char* data)
{
	int8_t temp = 0;
	_Bool flage = 0;
	if(*data == '-'){//先检测正负
			data++;
			flage = 1;
	}
	if((*data >= '0' && *data <= '9')){//提取数字
		while(*data >= '0' && *data <= '9'){
			temp *= 10;
			temp += (*data - 48);
			data++;
		}
    if(flage == 1)//负数乘等-1
       temp *= -1;
		return temp;
	}
	else{
		return 0xFF; 
	}
}



/*=========================================================================================================
 * Function : void Infor_analysis(const char* data)
 * Describe : 2.4G接收数据解析
 * Input    : data:需要解析的数据
 * Return   : nono
 * Record   : 2024/10/09
==========================================================================================================*/
void Infor_analysis(const char* data)
{
	uint8_t i;
	int8_t id = -1;
	uint16_t time = 0;
	for(i = 0;i < DEBUG_CMD_NUM; i++)
	{
     if(( _Bool ) strstr ( data, Debug_Cmd[i] ))
		 break;
	}
	switch(i){
		case ENTER_STUDY:
			printf("study mode\r\n");
			NRF24L01_TX_Mode();
			NRF24L01_TxPacket((uint8_t*)"OK");
			NRF24L01_RX_Mode();
			osEventFlagsSet(myEvent01Handle, ENTER_STUDY_EVEN);
			break;
		case QUIT_STUDY:
			NRF24L01_TX_Mode();
			NRF24L01_TxPacket((uint8_t*)"OK");
			NRF24L01_RX_Mode();
			osEventFlagsSet(myEvent01Handle, QUIT_STUDY_EVEN);
			break;
		case SWITCH_ON:
			Switch_Struct.Set(ON);
			printf("Set OK\r\n");
			break;
		case SWITCH_OFF:
			Switch_Struct.Set(OFF);
			printf("Set OK\r\n");
			break;
		case ID:
			NRF24L01_TX_Mode();
			NRF24L01_TxPacket((uint8_t*)"OK");
			NRF24L01_RX_Mode();
			for(i = 0;i < DEBUG_DATA_MAX;i++){
				if(data[i] == ':')
					break;
			}
			if(i < DEBUG_DATA_MAX){
				id = Hex_To_Dec(&(data[i+1]));
				printf("study id:%d\r\n",id);
			}
			osMessageQueuePut(IDQueueHandle,&id,0,0);
			break;
		case SWITCH_TOGGLE:
			NRF24L01_TX_Mode();
			NRF24L01_TxPacket((uint8_t*)"OK");
			NRF24L01_RX_Mode();
			Switch_Struct.Set(!Switch_Struct.Get());
			break;
		case CLEAR_ID:{
			NRF24L01_TX_Mode();
			NRF24L01_TxPacket((uint8_t*)"OK");
			NRF24L01_RX_Mode();
			char* temp = strchr(data, ':');
			id = Hex_To_Dec(temp+1);
			printf("id = %d\r\n",id);
			Clear_W25Q(id);
			NRF_Send((uint8_t*)"clear ok",10);
			break;
		}
		case I_PA_VPT:{
			uint8_t NRF_TX_Data[TX_PLOAD_WIDTH];
			sprintf((char*)NRF_TX_Data,"I_PA_VPT:%.2f|%.0f",VPT_Arr[VPT_I],VPT_Arr[VPT_PA]);//电能
			NRF_Send(NRF_TX_Data,10);
			break;
		}
		case READ_I_PA_VPT:{
			NRF24L01_TX_Mode();
			NRF24L01_TxPacket((uint8_t*)"OK");
			NRF24L01_RX_Mode();
			char* temp1 = strchr(data,':');
			VPT_Arr[VPT_I] = atof(temp1+1);
			char* temp2 = strchr(data,'|');
			VPT_Arr[VPT_PA] = atof(temp2+1);
			osEventFlagsSet(myEvent01Handle, WRITE_W25Q64_EVEN);
			break;
		}
		case TIMING:{
			NRF24L01_TX_Mode();
			NRF24L01_TxPacket((uint8_t*)"OK");
			NRF24L01_RX_Mode();
			char* temp = strchr(data,':');
			time = atof(temp+1);
			if(time == 0){
				printf("timing err\r\n");
				break;
			}
			printf("start timing:%ds\r\n",time);
			osMessageQueueReset(TimeingQueueHandle);
			if(osMessageQueuePut(TimeingQueueHandle,&time,0,0) == osOK){
				osEventFlagsSet(myEvent01Handle, TIME_START_EVEN);
			}
			else{
				printf("Set Timing Err\r\n");
			}
			break;
		}
	}
}



/*=========================================================================================================
 * Function : void Write_W25Q(void)
 * Describe : 将数据写入W25Q
 * Input    : nono
 * Return   : nono
 * Record   : 2024/10/09
==========================================================================================================*/
void Write_W25Q(void)
{
	//数据太多将数据分为4个区域
	uint8_t arr0[256] = "";
	uint8_t arr1[256] = "";
	uint8_t arr2[256] = "";
	uint8_t Write_EA_NUM[20] = "";
	uint8_t Write_I_PA_VPT[20] = "";
	uint8_t Write_id_pass[50] = "";
	char temp[30];//用于链接的中间变量
	
	
	//清空电器数据
	for(uint8_t k = 0;k < NUM_CLASS;k++){
		for(uint8_t i = 0;i < NUM_FEATURES;i++){//需要删除的数据清零
			for(uint8_t j = 0;j < NUM_FEATURES;j++){
				X_train[(k*3)+j][i] = 0XFFFF;
			}
		}
	}
	
	//将数据写入存储字符串
	for(uint8_t i = 0;i < NUM_SAMPLES / 3;i++){
		for(uint8_t j = 0;j < NUM_FEATURES;j++){
			sprintf(temp,"%d,%d=%.2f|",i,j,X_train[i][j]);
			strcat((char*)arr0,temp);
		}
	}
	for(uint8_t i = NUM_SAMPLES / 3;i < 2 * (NUM_SAMPLES / 3);i++){
		for(uint8_t j = 0;j < NUM_FEATURES;j++){
			sprintf(temp,"%d,%d=%.2f|",i,j,X_train[i][j]);
			strcat((char*)arr1,temp);
		}
	}
	for(uint8_t i = 2 * (NUM_SAMPLES / 3);i < NUM_SAMPLES;i++){
		for(uint8_t j = 0;j < NUM_FEATURES;j++){
			sprintf(temp,"%d,%d=%.2f|",i,j,X_train[i][j]);
			strcat((char*)arr2,temp);
		}
	}
	sprintf((char*)Write_EA_NUM,"EA_NUM:%f",EA_NUM);
	sprintf((char*)Write_I_PA_VPT,"I_PA_VPT:%.2f|%.0f",VPT_Arr[VPT_I],VPT_Arr[VPT_PA]);
	sprintf((char*)Write_id_pass,"ssid:%s\r\npassword:%s\r\n",ESP8266_DataStruct.esp8266_id,ESP8266_DataStruct.esp8266_pass);
	W25Q64_SectorErase(0x000000);//先擦除在写入
  W25Q64_PageProgram(0x000000, arr0, 255);
  W25Q64_PageProgram(0x000100, arr1, 255);
  W25Q64_PageProgram(0x000200, arr2, 255);
	W25Q64_PageProgram(0x000300, Write_EA_NUM, sizeof(Write_EA_NUM));
	W25Q64_PageProgram(0x000400, Write_I_PA_VPT, sizeof(Write_I_PA_VPT));
	W25Q64_PageProgram(0x000500, Write_id_pass, sizeof(Write_id_pass));
	printf("%s\r\n",Write_EA_NUM);
	printf("%s\r\n",Write_I_PA_VPT);
	printf("%s\r\n",Write_id_pass);
}



/*=========================================================================================================
 * Function : void parse_data(char *str, int *i, int *j, float *value)
 * Describe : 提取字符串的数据位置数据
 * Input    : str:待提取字符串  i/j:数据的位置  value:提取的数据
 * Return   : nono
 * Record   : 2024/10/09
==========================================================================================================*/
void parse_data(char *str, int *i, int *j, float *value) {
    // 查找逗号和等号
    char *comma = strchr(str, ',');
    char *equal = strchr(str, '=');
    if (comma && equal) {
        *i = atoi(str);
        *j = atoi(comma + 1);
        *value = atof(equal + 1);
    }
}



/*=========================================================================================================
 * Function : uint8_t parse_wifi_info(const char *input, char *ssid, size_t ssid_size, char *password, size_t password_size)
 * Describe : 读取W25Q内数据
 * Input    : input:解析的数据 ssid:解析后ID的保存位置 ssid_size:ID的最大字节 password:解析后密码的保存位置 password_size:密码的最大长度
 * Return   : 是否解析成功 0:解析失败 1:解析成功
 * Record   : 2024/11/26
==========================================================================================================*/
uint8_t parse_wifi_info(const char *input, char *ssid, size_t ssid_size, char *password, size_t password_size)
{
    const char *ssid_ptr = strstr(input, "ssid:");
    const char *password_ptr = strstr(input, "password:");
    if (ssid_ptr && password_ptr) {
        ssid_ptr += 5;      // 跳过 "ssid:" 的长度
        password_ptr += 9;  // 跳过 "password:" 的长度

        // 解析 ssid
        const char *ssid_end = strstr(ssid_ptr,"\r\n");
        if (ssid_end && (ssid_end - ssid_ptr) < ssid_size) {
            strncpy(ssid, ssid_ptr, ssid_end - ssid_ptr);
            ssid[ssid_end - ssid_ptr] = '\0';  // 添加字符串终止符
        } else {
            strncpy(ssid, ssid_ptr, ssid_size - 1);
            ssid[ssid_size - 1] = '\0';
        }
        // 解析 password
        const char *password_end = strstr(password_ptr,"\r\n");
        if (password_end) {
            strncpy(password, password_ptr, password_end - password_ptr);
            password[password_end - password_ptr] = '\0';  // 添加字符串终止符
        } else {
            strncpy(password, password_ptr, password_size - 1);
            password[password_size - 1] = '\0';
        }
				return 1;
    }
		else{
			return 0;
		}
}



/*=========================================================================================================
 * Function : void Read_W25Q(void)
 * Describe : 读取W25Q内数据
 * Input    : nono
 * Return   : nono
 * Record   : 2024/10/09
==========================================================================================================*/
void Read_W25Q(void)
{
	uint8_t arr[3][256];//先读取三块的内容存储区
	uint8_t temp[20];
	uint8_t set_temp[20];
	uint8_t wifi_data[50] = "";
	uint8_t data[1024];//内容合并存储区
  W25Q64_ReadData(0x000000, arr[0], 255);//先读取三块的内容
	W25Q64_ReadData(0x000100, arr[1], 255);
	W25Q64_ReadData(0x000200, arr[2], 255);
	W25Q64_ReadData(0x000300, temp, 20);
	W25Q64_ReadData(0x000400, set_temp, 20);
	W25Q64_ReadData(0x000500, wifi_data, 50);
	
	
	strcat((char*)data,(char*)arr[0]);//内容合并
	strcat((char*)data,(char*)arr[1]);
	strcat((char*)data,(char*)arr[2]);
	char *token = strtok((char *)data, "|");//寻找分隔符
	while (token != NULL) {
		int i, j;
		float value;
		parse_data(token,&i, &j, &value);
		X_train[i][j] = value;//数据读取存储
		token = strtok(NULL, "|");
	}
	for(uint8_t i = 0;i < NUM_SAMPLES;i++){//数据输出
		for(uint8_t j = 0;j < NUM_FEATURES;j++){
			printf("X_train[%d][%d] = %.2f\t",i,j,X_train[i][j]);
		}
		printf("\r\n");
		if(i%3 == 2){
			printf("\r\n");
		}
	}
	
	token = strchr((char*)temp, ':');
	EA_NUM = atof(token + 1);
	
	char* temp1 = strchr((char*)set_temp,':');
	VPT_Arr[VPT_I] = atof(temp1+1);
	char* temp2 = strchr((char*)set_temp,'|');
	VPT_Arr[VPT_PA] = atof(temp2+1);
	printf("EA_NUM = %f\r\nVPT_I = %.2fA\r\nVPT_PA = %.0f\r\n",EA_NUM,VPT_Arr[VPT_I],VPT_Arr[VPT_PA]);
	parse_wifi_info((char*)wifi_data,ESP8266_DataStruct.esp8266_id,sizeof(ESP8266_DataStruct.esp8266_id),ESP8266_DataStruct.esp8266_pass,sizeof(ESP8266_DataStruct.esp8266_pass));
	printf("ssid:%s\r\npassword:%s\r\n",ESP8266_DataStruct.esp8266_id,ESP8266_DataStruct.esp8266_pass);
}



/*=========================================================================================================
 * Function : void Clear_W25Q(uint8_t num)
 * Describe : 删除某个id的数据
 * Input    : num:需要删除的id
 * Return   : nono
 * Record   : 2024/10/09
==========================================================================================================*/
void Clear_W25Q(uint8_t num)
{
	for(uint8_t i = 0;i < NUM_FEATURES;i++){//需要删除的数据清零
		for(uint8_t j = 0;j < NUM_FEATURES;j++){
			X_train[(num*3)+j][i] = 0XFFFF;
		}
	}
	osEventFlagsSet(myEvent01Handle, WRITE_W25Q64_EVEN);//写入
}



/*=========================================================================================================
 * Function : uint8_t NRF_Send(uint8_t* data,uint8_t tim_Count)
 * Describe : 2.4G发送针对于需要检测的数据
 * Input    : data:发送数据 tim_Count:检测次数
 * Return   : 1:发送成功 0:发送失败
 * Record   : 2024/10/09
==========================================================================================================*/
uint8_t NRF_Send(uint8_t* data,uint8_t tim_Count)
{
	uint8_t RX_Arr[RX_PLOAD_WIDTH];
	while(tim_Count--){
		NRF24L01_TX_Mode();
		NRF24L01_TxPacket(data);//发送数据
		NRF24L01_RX_Mode();
		osDelay(100);
		while(NRF24L01_RxPacket(RX_Arr) == 0);//等待接收完成
		printf("TX_Arr = %s\r\nRX_Arr = %s\r\n",data,RX_Arr);
		if(strstr((char*)RX_Arr,"OK") != NULL){
			return 1;
		}
	}
	return 0;
}



/*=========================================================================================================
 * Function : void Wifi_Send(float* parameter,uint8_t id)
 * Describe : 将读取信息发送至WIFI线程
 * Input    : parameter:电参数 id:用电器ID
 * Return   : nono
 * Record   : 2024/10/22
==========================================================================================================*/
void Wifi_Send(float* parameter,uint8_t id)
{
	char* data = pvPortMalloc(256);
	osStatus_t err = osError;
	if(data != NULL){
		sprintf(data,"cmd=2&uid=%s&topic=%s&msg=*U:%.2f|I:%.2f|PA:%.2f|PF:%.2f|EA:%f|ID:%d|I_VPT:%.2f|PA_VPT:%.2f|Switch:%d\r\n",
						Bemfa_DataStruct.uid,Bemfa_DataStruct.name,
						parameter[U],parameter[I],parameter[PA],parameter[PF],parameter[EA],id,
						VPT_Arr[VPT_I],VPT_Arr[VPT_PA],Switch_Struct.Get());
//		printf("wifi data len:%d\r\n",strlen(data));
		osMessageQueueReset(Wifi_DataHandle);
		err = osMessageQueuePut(Wifi_DataHandle,&data,0,10);
		if(err != osOK){
			printf("Wifi Send err:%d\r\n",err);
		}
		vPortFree(data);
	}
}



/*=========================================================================================================
 * Function : void extract_data(char *input)
 * Describe : 将WIFI接收的数据解析
 * Input    : input:需要解析的数据
 * Return   : nono
 * Record   : 2024/10/22
==========================================================================================================*/
void extract_data(char *input)
{
    float voltage = 0.0, current = 0.0, power = 0.0, power_factor = 0.0, energy = 0.0;
    // 遍历输入字符串
		if(input[0] != '#'){
			char *ptr = input;
			while (*ptr != '\0') {
					char key[3]; // 只需要两个字符加一个终止符
					float value;

					// 读取键值对
					if (sscanf(ptr, "%2[^:]:%f", key, &value) == 2) {
							if (strcmp(key, "U") == 0) {
									voltage = value;
							} else if (strcmp(key, "I") == 0) {
									current = value;
							} else if (strcmp(key, "PA") == 0) {
									power = value;
							} else if (strcmp(key, "PF") == 0) {
									power_factor = value;
							} else if (strcmp(key, "EA") == 0) {
									energy = value;
							}
					}
					// 移动指针到下一个分隔符
					ptr = strchr(ptr, '|');
					if (ptr != NULL) {
							ptr++; // 跳过分隔符
					} else {
							break; // 没有找到更多分隔符，退出循环
					}
			}
			// 打印赋值后的结果
			printf("Voltage: %.2f V\r\n", voltage);
			printf("Current: %.2f A\r\n", current);
			printf("Power: %.2f W\r\n", power);
			printf("Power Factor: %.2f\r\n", power_factor);
			printf("Energy: %.6f kWh\r\n", energy);
	}
	else{
		Infor_analysis(input+1);
	}
}



/*=========================================================================================================
 * Function : void Usart_Debug_Run(const char* data)
 * Describe : 串口接收回调函数
 * Input    : data:接受的数据
 * Return   : nono
 * Record   : 2024/10/09
==========================================================================================================*/
void Usart_Debug_Run(const char* data)
{
	uint8_t i;
	int8_t id = -1;
	uint16_t time = 0;
	for(i = 0;i < DEBUG_CMD_NUM; i++)
	{
     if(( _Bool ) strstr ( data, Debug_Cmd[i] ))
		 break;
	}
	switch(i){
		case List:
			osEventFlagsSet(myEvent01Handle, PRINT_LIST_EVEN);
			break;
		case Help:
			for(uint8_t k = 0;k < DEBUG_CMD_NUM;k++){
				printf("%d.%s",k+1,Debug_Cmd[k]);
				if( (k + 1) % 3 ){
					if(strlen(Debug_Cmd[k]) > 12)
						printf("\t");
					else if(strlen(Debug_Cmd[k]) < 5)
						printf("\t\t\t");
					else
						printf("\t\t");
				}
				else{
					printf("\r\n");
				}
			}
			break;
		case ENTER_STUDY:
			osEventFlagsSet(myEvent01Handle, ENTER_STUDY_EVEN);
			break;
		case QUIT_STUDY:
			osEventFlagsSet(myEvent01Handle, QUIT_STUDY_EVEN);
			break;
		case SWITCH_ON:
			Switch_Struct.Set(ON);
			printf("Set OK\r\n");
			break;
		case SWITCH_OFF:
			Switch_Struct.Set(OFF);
			printf("Set OK\r\n");
			break;
		case SWITCH_GET:
			printf("Switch = %dON:1  OFF:0\r\n",Switch_Struct.Get());
			break;
		case ID:
			for(i = 0;i < DEBUG_DATA_MAX;i++){
				if(data[i] == ':')
					break;
			}
			if(i < DEBUG_DATA_MAX){
				id = Hex_To_Dec(&(data[i+1]));
				printf("%d",id);
			}
			osMessageQueuePut(IDQueueHandle,&id,0,0);
			break;
		case SWITCH_TOGGLE:
			Switch_Struct.Set(!Switch_Struct.Get());
			break;
		case CLEAR_ID:{
			char* temp = strchr(data, ':');
			id = Hex_To_Dec(temp+1);
			printf("id = %d\r\n",id);
			Clear_W25Q(id);
			break;
		}
		case TIMING:{
			char* temp = strchr(data,':');
			time = atof(temp+1);
			if(time == 0){
				printf("timing err");
				break;
			}
			printf("start timing:%ds",time);
			osMessageQueueReset(TimeingQueueHandle);
			if(osMessageQueuePut(TimeingQueueHandle,&time,0,0) == osOK){
				osEventFlagsSet(myEvent01Handle, TIME_START_EVEN);
			}
			else{
				printf("Set Timing Err\r\n");
			}
			break;
		}
		default:
			myPrintf("\r\nERR\r\n请输入一下命令以回车结束:\r\n");
			for(uint8_t k = 0;k < DEBUG_CMD_NUM;k++){
				printf("%d.%s",k+1,Debug_Cmd[k]);
				if( (k + 1) % 3 ){
					if(strlen(Debug_Cmd[k]) > 12)
						printf("\t");
					else if(strlen(Debug_Cmd[k]) < 5)
						printf("\t\t\t");
					else
						printf("\t\t");
				}
				else{
					printf("\r\n");
				}
			}
			break;
	}
	printf("\r\n");
}





