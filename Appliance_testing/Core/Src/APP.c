#include "APP.h"


char *Debug_Cmd[ DEBUG_CMD_NUM ] = { "List","help","enter_study","quit_study","Switch ON","Switch OFF","Switch?","ID:","Switch Toggle","Clear:",
																		"I_PA_VPT?","I_PA_VPT:","Timing:"};//������Ϣ
float VPT_Arr[VPT_NUM] = { 3,2200 };//�ر���ֵ
float EA_NUM = 1.214014;


/*=========================================================================================================
 * Function : uint8_t Read_HLW8110_Electrical(float* data)
 * Describe : ��ȡHW110ģ����Ϣ
 * Input    : data:��ȡ�����ݱ���λ��
 * Return   : OK:��ȡ���		ERR:��ȡʧ��
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
 * Describe : ����Ƿ񳬹���ֵ
 * Input    : data:��Ҫ��������
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
 * Describe : ����������ͨ�����ں�2.4G���ͣ�
 * Input    : data:��Ҫ���������
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
	sprintf((char*)NRF_TX_Data,"U:%.2f",data[U]);//��ѹ
	NRF24L01_TxPacket(NRF_TX_Data);
	memset(NRF_TX_Data,0,sizeof(NRF_TX_Data));
	osDelay(100);
	sprintf((char*)NRF_TX_Data,"I:%.2f",data[I]);//����
	NRF24L01_TxPacket(NRF_TX_Data);
	memset(NRF_TX_Data,0,sizeof(NRF_TX_Data));
	osDelay(100);
	sprintf((char*)NRF_TX_Data,"PA:%.2f",data[PA]);//����
	NRF24L01_TxPacket(NRF_TX_Data);
	memset(NRF_TX_Data,0,sizeof(NRF_TX_Data));
	osDelay(100);
	sprintf((char*)NRF_TX_Data,"PF:%.2f",data[PF]);//��������
	NRF24L01_TxPacket(NRF_TX_Data);
	memset(NRF_TX_Data,0,sizeof(NRF_TX_Data));
	osDelay(100);
	sprintf((char*)NRF_TX_Data,"EA:%f",data[EA]);//����
	NRF24L01_TxPacket(NRF_TX_Data);
	memset(NRF_TX_Data,0,sizeof(NRF_TX_Data));
	osDelay(100);
	NRF24L01_RX_Mode();
}



/*=========================================================================================================
 * Function : int8_t Hex_To_Dec(const char* data)
 * Describe : ��ȡ�ַ����е����֣��з��ţ�
 * Input    : data:��Ҫ��ȡ���ַ���
 * Return   : �ַ��������� 0xFF��ʾʧ��
 * Record   : 2024/10/09
==========================================================================================================*/
int8_t Hex_To_Dec(const char* data)
{
	int8_t temp = 0;
	_Bool flage = 0;
	if(*data == '-'){//�ȼ������
			data++;
			flage = 1;
	}
	if((*data >= '0' && *data <= '9')){//��ȡ����
		while(*data >= '0' && *data <= '9'){
			temp *= 10;
			temp += (*data - 48);
			data++;
		}
    if(flage == 1)//�����˵�-1
       temp *= -1;
		return temp;
	}
	else{
		return 0xFF; 
	}
}



/*=========================================================================================================
 * Function : void Infor_analysis(const char* data)
 * Describe : 2.4G�������ݽ���
 * Input    : data:��Ҫ����������
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
			sprintf((char*)NRF_TX_Data,"I_PA_VPT:%.2f|%.0f",VPT_Arr[VPT_I],VPT_Arr[VPT_PA]);//����
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
 * Describe : ������д��W25Q
 * Input    : nono
 * Return   : nono
 * Record   : 2024/10/09
==========================================================================================================*/
void Write_W25Q(void)
{
	//����̫�ཫ���ݷ�Ϊ4������
	uint8_t arr0[256] = "";
	uint8_t arr1[256] = "";
	uint8_t arr2[256] = "";
	uint8_t Write_EA_NUM[20] = "";
	uint8_t Write_I_PA_VPT[20] = "";
	uint8_t Write_id_pass[50] = "";
	char temp[30];//�������ӵ��м����
	
	
	//��յ�������
	for(uint8_t k = 0;k < NUM_CLASS;k++){
		for(uint8_t i = 0;i < NUM_FEATURES;i++){//��Ҫɾ������������
			for(uint8_t j = 0;j < NUM_FEATURES;j++){
				X_train[(k*3)+j][i] = 0XFFFF;
			}
		}
	}
	
	//������д��洢�ַ���
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
	W25Q64_SectorErase(0x000000);//�Ȳ�����д��
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
 * Describe : ��ȡ�ַ���������λ������
 * Input    : str:����ȡ�ַ���  i/j:���ݵ�λ��  value:��ȡ������
 * Return   : nono
 * Record   : 2024/10/09
==========================================================================================================*/
void parse_data(char *str, int *i, int *j, float *value) {
    // ���Ҷ��ź͵Ⱥ�
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
 * Describe : ��ȡW25Q������
 * Input    : input:���������� ssid:������ID�ı���λ�� ssid_size:ID������ֽ� password:����������ı���λ�� password_size:�������󳤶�
 * Return   : �Ƿ�����ɹ� 0:����ʧ�� 1:�����ɹ�
 * Record   : 2024/11/26
==========================================================================================================*/
uint8_t parse_wifi_info(const char *input, char *ssid, size_t ssid_size, char *password, size_t password_size)
{
    const char *ssid_ptr = strstr(input, "ssid:");
    const char *password_ptr = strstr(input, "password:");
    if (ssid_ptr && password_ptr) {
        ssid_ptr += 5;      // ���� "ssid:" �ĳ���
        password_ptr += 9;  // ���� "password:" �ĳ���

        // ���� ssid
        const char *ssid_end = strstr(ssid_ptr,"\r\n");
        if (ssid_end && (ssid_end - ssid_ptr) < ssid_size) {
            strncpy(ssid, ssid_ptr, ssid_end - ssid_ptr);
            ssid[ssid_end - ssid_ptr] = '\0';  // ����ַ�����ֹ��
        } else {
            strncpy(ssid, ssid_ptr, ssid_size - 1);
            ssid[ssid_size - 1] = '\0';
        }
        // ���� password
        const char *password_end = strstr(password_ptr,"\r\n");
        if (password_end) {
            strncpy(password, password_ptr, password_end - password_ptr);
            password[password_end - password_ptr] = '\0';  // ����ַ�����ֹ��
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
 * Describe : ��ȡW25Q������
 * Input    : nono
 * Return   : nono
 * Record   : 2024/10/09
==========================================================================================================*/
void Read_W25Q(void)
{
	uint8_t arr[3][256];//�ȶ�ȡ��������ݴ洢��
	uint8_t temp[20];
	uint8_t set_temp[20];
	uint8_t wifi_data[50] = "";
	uint8_t data[1024];//���ݺϲ��洢��
  W25Q64_ReadData(0x000000, arr[0], 255);//�ȶ�ȡ���������
	W25Q64_ReadData(0x000100, arr[1], 255);
	W25Q64_ReadData(0x000200, arr[2], 255);
	W25Q64_ReadData(0x000300, temp, 20);
	W25Q64_ReadData(0x000400, set_temp, 20);
	W25Q64_ReadData(0x000500, wifi_data, 50);
	
	
	strcat((char*)data,(char*)arr[0]);//���ݺϲ�
	strcat((char*)data,(char*)arr[1]);
	strcat((char*)data,(char*)arr[2]);
	char *token = strtok((char *)data, "|");//Ѱ�ҷָ���
	while (token != NULL) {
		int i, j;
		float value;
		parse_data(token,&i, &j, &value);
		X_train[i][j] = value;//���ݶ�ȡ�洢
		token = strtok(NULL, "|");
	}
	for(uint8_t i = 0;i < NUM_SAMPLES;i++){//�������
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
 * Describe : ɾ��ĳ��id������
 * Input    : num:��Ҫɾ����id
 * Return   : nono
 * Record   : 2024/10/09
==========================================================================================================*/
void Clear_W25Q(uint8_t num)
{
	for(uint8_t i = 0;i < NUM_FEATURES;i++){//��Ҫɾ������������
		for(uint8_t j = 0;j < NUM_FEATURES;j++){
			X_train[(num*3)+j][i] = 0XFFFF;
		}
	}
	osEventFlagsSet(myEvent01Handle, WRITE_W25Q64_EVEN);//д��
}



/*=========================================================================================================
 * Function : uint8_t NRF_Send(uint8_t* data,uint8_t tim_Count)
 * Describe : 2.4G�����������Ҫ��������
 * Input    : data:�������� tim_Count:������
 * Return   : 1:���ͳɹ� 0:����ʧ��
 * Record   : 2024/10/09
==========================================================================================================*/
uint8_t NRF_Send(uint8_t* data,uint8_t tim_Count)
{
	uint8_t RX_Arr[RX_PLOAD_WIDTH];
	while(tim_Count--){
		NRF24L01_TX_Mode();
		NRF24L01_TxPacket(data);//��������
		NRF24L01_RX_Mode();
		osDelay(100);
		while(NRF24L01_RxPacket(RX_Arr) == 0);//�ȴ��������
		printf("TX_Arr = %s\r\nRX_Arr = %s\r\n",data,RX_Arr);
		if(strstr((char*)RX_Arr,"OK") != NULL){
			return 1;
		}
	}
	return 0;
}



/*=========================================================================================================
 * Function : void Wifi_Send(float* parameter,uint8_t id)
 * Describe : ����ȡ��Ϣ������WIFI�߳�
 * Input    : parameter:����� id:�õ���ID
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
 * Describe : ��WIFI���յ����ݽ���
 * Input    : input:��Ҫ����������
 * Return   : nono
 * Record   : 2024/10/22
==========================================================================================================*/
void extract_data(char *input)
{
    float voltage = 0.0, current = 0.0, power = 0.0, power_factor = 0.0, energy = 0.0;
    // ���������ַ���
		if(input[0] != '#'){
			char *ptr = input;
			while (*ptr != '\0') {
					char key[3]; // ֻ��Ҫ�����ַ���һ����ֹ��
					float value;

					// ��ȡ��ֵ��
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
					// �ƶ�ָ�뵽��һ���ָ���
					ptr = strchr(ptr, '|');
					if (ptr != NULL) {
							ptr++; // �����ָ���
					} else {
							break; // û���ҵ�����ָ������˳�ѭ��
					}
			}
			// ��ӡ��ֵ��Ľ��
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
 * Describe : ���ڽ��ջص�����
 * Input    : data:���ܵ�����
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
			myPrintf("\r\nERR\r\n������һ�������Իس�����:\r\n");
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





