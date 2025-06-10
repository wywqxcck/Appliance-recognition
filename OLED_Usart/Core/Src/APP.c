#include "APP.h"

//ѡ�����ʾ
uint8_t gImage_11[32] = { /* 0X22,0X01,0X10,0X00,0X10,0X00, */
	0X00,0X80,0XC0,0XC0,0XE0,0XF0,0XF0,0XF8,0XF8,0XE0,0XE0,0XE0,0XE0,0XE0,0XE0,0X00,
	0X00,0X01,0X03,0X03,0X07,0X0F,0X1F,0X1F,0X1F,0X07,0X07,0X07,0X07,0X07,0X07,0X00,
};
//ѡ������
uint8_t gImage_Null[32] = { /* 0X22,0X01,0X10,0X00,0X10,0X00, */
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
};

char *Ele_Arr[ELE_MAX] = {"�׳��","������","�����","�����","��ˮ��","�� ��"};//�õ�������
char *Mune_Arr[MENU_MAX] = {"ѧϰģʽ","���ģʽ","����ģʽ","��ʱģʽ"};//�õ�������
char *Set_Arr[SET_MAX] = {"������","���ʣ�"};//�õ�������
char *RX_Cmd[ 9 ] = {"U:", "I:","PA:","PF:","EA:","ID:","study ok","clear ok","I_PA_VPT:"};//NRF������Ϣ

float parameter_data[PARAMETER_NUM] = { 0 };//�����
char *Debug_Cmd[ DEBUG_CMD_NUM ] = { "parameter:","ID:","help",};//������Ϣ
float VPT_Data[SET_MAX] = {0.5,800};


void Menu_Interface(void);
void Menu_Study(void);
void Menu_Detection(void);
void Menu_Set(void);
void Timing_Set(void);

int8_t Current_ID = -1;//����ʹ�õ���ID
uint8_t my_index = 0,index_num = 0,old_index_num = 0;//����ѡ����ƶ�
int8_t direction = STRT;  //��ת����
uint8_t taskstudy = 0;  //ѧϰģʽ���
uint8_t taskIndex = 0;	//����������
uint8_t Cell = 0;					//��ص���
uint8_t send_flage = NO_STUDY;   //ѧϰ״̬
uint8_t set_flage = NO_SET;
uint8_t timing_flage = TIMING_SET;
uint8_t blank = 0;
uint8_t toggle_flage = 0;
uint8_t gears = 1;
uint16_t timing_time = 10;
uint16_t rot_speed = 0;
uint32_t power_flage = 0;



//������ȱ�
Menu_table_t taskTable[] =
{
    //�˵����溯�� -- һ������
    {Interface_Mune, Study_Mune, Interface_Mune, Interface_Mune, Interface_Mune, Menu_Interface}, 

        //�˵����溯�� -- ��������
        {Study_Mune, Study_Mune, Study_Mune, Study_Mune, Interface_Mune, Menu_Study},
        {Detection_Mune, Detection_Mune, Detection_Mune, Detection_Mune, Interface_Mune, Menu_Detection},
				{Set_Mune, Set_Mune, Set_Mune, Set_Mune, Interface_Mune, Menu_Set},
				{Timing_Mune, Timing_Mune, Timing_Mune, Timing_Mune, Interface_Mune, Timing_Set},
};


/**
  * @brief  ΢����ʱ����
  * @param  ��ʱnus΢��
  * @retval ��
  */
void delay_us(uint32_t nus)
{
	__HAL_TIM_SET_COUNTER(DLY_TIM_Handle, 0);
	__HAL_TIM_ENABLE(DLY_TIM_Handle);
	while (__HAL_TIM_GET_COUNTER(DLY_TIM_Handle) < nus)
	{
	}
	__HAL_TIM_DISABLE(DLY_TIM_Handle);
}


/**
  * @brief  ѡ����ƶ�
  * @param  ����ѡ����ƶ�old��ǰλ��newĿ��λ��
  * @retval ��
  */
void arrows_to(uint8_t old, uint8_t new)
{
	if(old == new){
	 oled_draw_bmp(ARROWS_X,old,16,16,gImage_11);
	}
	else if(old > new){
		while(old != new){
			oled_draw_bmp(ARROWS_X,old,16,16,gImage_Null);
			old--;
			oled_draw_bmp(ARROWS_X,old,16,16,gImage_11);
			To_Send_Buff();
		}
	}
	else{
		while(old != new){
			oled_draw_bmp(ARROWS_X,old,16,16,gImage_Null);
			old++;
			oled_draw_bmp(ARROWS_X,old,16,16,gImage_11);
			To_Send_Buff();
		}
	}
}                        


/**
  * @brief  �˵�����1����
  * @param  ��
  * @retval ��
  */
void Menu_Interface(void)
{
	if(send_flage != NO_STUDY){
		if(NRF_Send((uint8_t*)"quit_study",5) == 1){
			send_flage = NO_STUDY;
		}
		else{
			
		}
	}
	char temp[] = "�������";
	OLED_ShowCHinese(20,0,(uint8_t *)temp);
	sprintf(temp,"%d%%  ",Cell);
	OLED_ShowString(90,0,(uint8_t *)temp);
	if (direction == FWD){//��ת		
		old_index_num = index_num;
		if(index_num >= 1){
			my_index++;
			if(my_index >= MENU_MAX - 2){
				my_index = MENU_MAX - 2;
			}
		}
		else{
			index_num++;
		}
		printf("taskstudy = %d\r\n",taskstudy);
		direction = STRT;
	}
	else if(direction == REV){//��ת
		old_index_num = index_num;
		if(index_num <= MENU_MAX && index_num > 0){
			index_num--;
		}
		else if(index_num == 0){
			index_num = 0;
			if(my_index > 0)
				my_index--;
		}
		printf("taskstudy = %d\r\n",taskstudy);
		direction = STRT;
	}
	arrows_to((old_index_num+1)*24,(index_num+1)*24);
	old_index_num = index_num;
	OLED_ShowCHinese(0,3,(uint8_t *)Mune_Arr[my_index]);
	OLED_ShowCHinese(0,6,(uint8_t *)Mune_Arr[my_index+1]);
	Oled_Send_Buff();

}




/**
  * @brief  ѧϰ����
  * @param  ��
  * @retval ��
  */
void Menu_Study(void)
{
	uint8_t TX_Arr[TX_PLOAD_WIDTH];
	uint8_t RX_Arr[RX_PLOAD_WIDTH];
	if(send_flage == NO_STUDY){//�ӷ�ѧϰ����ѧϰ���߲���
		if(NRF_Send((uint8_t*)"enter_study",5) == 1){//����ɹ�
			send_flage = YES_STUDY;
		}else{//����ʧ��
			send_flage = NO_STUDY;
		}
	}
	if(send_flage == YES_STUDY){//ѧϰ����ѡ��ѧϰ�ĵ���
    uint8_t temp[20];
    if (direction == FWD){//��ת
			old_index_num = index_num;
			if(index_num >= 3){
				my_index++;
				if(my_index >= ELE_MAX - 4){
					my_index = ELE_MAX - 4;
				}
			}
			else{
				index_num++;
			}
			direction = STRT;
    }
    else if(direction == REV){//��ת
			old_index_num = index_num;
			if(index_num <= 3 && index_num > 0){
				index_num--;
			}
			else if(index_num == 0){
				index_num = 0;
				if(my_index > 0)
					my_index--;
			}
			direction = STRT;
    }
    //��ͷˢ��
    arrows_to(old_index_num*16,index_num*16);
    old_index_num = index_num;

    sprintf((char*)temp,"%d.",my_index + 1);
    OLED_ShowString(0,0,temp);
    sprintf((char*)temp,"%d.",my_index + 2);
    OLED_ShowString(0,2,temp);
    sprintf((char*)temp,"%d.",my_index + 3);
    OLED_ShowString(0,4,temp);
    sprintf((char*)temp,"%d.",my_index + 4);
    OLED_ShowString(0,6,temp);
    OLED_ShowCHinese(20,0,(uint8_t *)Ele_Arr[my_index]);
    OLED_ShowCHinese(20,2,(uint8_t *)Ele_Arr[my_index+1]);
    OLED_ShowCHinese(20,4,(uint8_t *)Ele_Arr[my_index+2]);
    OLED_ShowCHinese(20,6,(uint8_t *)Ele_Arr[my_index+3]);
    Oled_Send_Buff();
	}
	else if(send_flage == ING_STUDY){//����ѧϰ�ȴ�ѧϰ���
		sprintf((char*)TX_Arr,"ID:%d",taskstudy);
		if(NRF_Send((uint8_t*)TX_Arr,50) == 1){//����ѧϰ�ɹ�
			OLED_ShowCHinese(0,0,(uint8_t *)"ѧϰ��"); 
			OLED_ShowString(32,0,(uint8_t *)"...");
			Oled_Send_Buff();
			while(send_flage != YES_STUDY){
				if(NRF24L01_RxPacket(RX_Arr) == 0){
					HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
					Analytic_Parameter_NRF((char*)RX_Arr);
					for(uint8_t i = 0;i < 32;i++){
						RX_Arr[i] = 0;
					}
				}
			}
		}else{
			send_flage = YES_STUDY;
		}
	}	
	else if(send_flage == CLEAR_STUDY){//�������ȴ�������
		sprintf((char*)TX_Arr,"Clear:%d",taskstudy);
		while(NRF_Send(TX_Arr,5) != 1);
		OLED_ShowString(0,0,(uint8_t *)"Clear...");
		Oled_Send_Buff();
		HAL_Delay(500);
		while(send_flage != YES_STUDY){
			if(NRF24L01_RxPacket(RX_Arr) == 0){
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
				Analytic_Parameter_NRF((char*)RX_Arr);
				for(uint8_t i = 0;i < 32;i++){
					RX_Arr[i] = 0;
				}
			}
		}
	}
}



/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
void Menu_Detection(void)
{
    uint8_t temp[20];
    if(Current_ID >= 0)
        OLED_ShowCHinese(Oled_Center((uint8_t *)Ele_Arr[Current_ID]),0,(uint8_t *)Ele_Arr[Current_ID]);//��ʾ��ǰ�õ���
    else{
        OLED_ShowString(Oled_Center((uint8_t *)"  NULL  "),0,(uint8_t *)"    NULL    ");
    }
    //��ʾ��ǰ�����
    sprintf((char*)temp,"U:%.2f I:%.2f  ",parameter_data[U],parameter_data[I]);
    OLED_ShowString(0,2,temp);
    sprintf((char*)temp,"PA:%.2f PF:%.2f  ",parameter_data[PA],parameter_data[PF]);
    OLED_ShowString(0,4,temp);
    sprintf((char*)temp,"EA:%f     ",parameter_data[EA]);
    OLED_ShowString(0,6,temp);
    Oled_Send_Buff();
}

/**
  * @brief  ���ý���
  * @param  ��
  * @retval ��
  */
void Menu_Set(void)
{
	char temp[20] = "����ģʽ";
	OLED_ShowCHinese(Oled_Center((uint8_t *)temp),0,(uint8_t *)temp);
	if(set_flage != NO_SET){
		sprintf(temp,"%.2f  ",VPT_Data[0]);
		OLED_ShowString(50,3,(uint8_t*)temp);
		sprintf(temp,"%.0f  ",VPT_Data[1]);
		OLED_ShowString(50,6,(uint8_t*)temp);
	}
	if(set_flage == NO_SET){
		TX_Mode();
		NRF24L01_TxPacket((uint8_t*)"I_PA_VPT?");
		RX_Mode();
	}
	else if(set_flage == YES_SET){
		if (direction == FWD){//��ת		
			old_index_num = index_num;
			index_num = 1;
			direction = STRT;
		}
		else if(direction == REV){//��ת
			old_index_num = index_num;
			index_num = 0;
			direction = STRT;
		}
		arrows_to((old_index_num+1)*24,(index_num+1)*24);
		old_index_num = index_num;
	}
	else if(set_flage == ING_SET){
		if(direction == FWD){//��ת		
			if(index_num == 0){
				VPT_Data[index_num]+=0.1;
				if(VPT_Data[index_num] >= 10){
					VPT_Data[index_num] = 10;
				}
			}
			else{
				VPT_Data[index_num]+=100;
				if(VPT_Data[index_num] >= 2200){
					VPT_Data[index_num] = 2200;
				}
			}
			direction = STRT;
		}
		else if(direction == REV){//��ת
			if(index_num == 0){
				VPT_Data[index_num]-=0.1;
				if(VPT_Data[index_num] < 0){
					VPT_Data[index_num] = 0;
				}
			}
			else{
				VPT_Data[index_num]-=100;
				if(VPT_Data[index_num] < 0){
					VPT_Data[index_num] = 0;
				}
			}
			direction = STRT;
		}
		if(blank){
			sprintf(temp,"       ");
			OLED_ShowString(50,(index_num + 1) * 3,(uint8_t*)temp);
		}
	}
	else if(set_flage == OK_SET){
		printf("����:%.2f\t����:%.0f\r\n",VPT_Data[0],VPT_Data[1]);
		uint8_t NRF_TX_Data[TX_PLOAD_WIDTH];
		sprintf((char*)NRF_TX_Data,"I_PA_VPT:%.2f|%.0f",VPT_Data[0],VPT_Data[1]);//����
		if(NRF_Send(NRF_TX_Data,10) == 1){
			set_flage = YES_SET;
		}
		else{
			set_flage = ING_SET;
		}
	}
	OLED_ShowCHinese(0,3,(uint8_t *)Set_Arr[0]);
	OLED_ShowCHinese(0,6,(uint8_t *)Set_Arr[1]);
	Oled_Send_Buff();
}


void Timing_Set(void)
{
	char temp[20] = "";
	if(timing_flage == TIMING_SET){
		if (direction == FWD){//��ת		
			timing_time += gears;
			rot_speed++;
			OLED_Clear();
		}
		else if(direction == REV){//��ת
			if(timing_time >= gears){
				timing_time -= gears;
			}
			else{
				timing_time = 0;
			}
			rot_speed++;
			OLED_Clear();
		}
		OLED_ShowCHinese(Oled_Center((uint8_t *)Mune_Arr[3]),0,(uint8_t *)Mune_Arr[3]);
		direction = STRT;
		sprintf(temp,"timing: %d s",timing_time);
		OLED_ShowString(Oled_Center((uint8_t *)temp),4,(uint8_t *)temp);
		Oled_Send_Buff();
	}
	else if(timing_flage == TIMING_SEND){
		uint8_t NRF_TX_Data[TX_PLOAD_WIDTH];
		sprintf((char*)NRF_TX_Data,"Timing:%d",timing_time);//��ʱ
		if(NRF_Send(NRF_TX_Data,10) == 1){
			timing_flage = TIMING_SET;
		}
		else{
			timing_flage = TIMING_SEND;
		}
	}
}


/**
  * @brief  ����ص���
  * @param  ������
  * @retval �����ٷֱ�
  */
uint8_t Get_ADC (uint16_t count)
{
	float data = 0;
	uint32_t ADC_Val = 0;
	uint16_t i;
	for(i = 0;i < count;i++){
		HAL_ADC_Start(&hadc1);
		if(HAL_OK ==HAL_ADC_PollForConversion(&hadc1,50))
		{
				ADC_Val += HAL_ADC_GetValue(&hadc1);
		}
	}
	ADC_Val /= count;
	data= (float)ADC_Val * (3.3 / 1980);
	ADC_Val = data * 100;
	ADC_Val = ADC_Val * 1.25 - 412;
	if(ADC_Val >= 100){
		return 100;
	}	
	return (uint8_t)ADC_Val;
}


/**
  * @brief  �ַ���ת������
  * @param  dataĿ���ַ���
  * @retval �ַ����к��еĸ�����
  */
float Hex_To_Float(const char* data)
{
	printf("data= %s\r\n",data);
	if(*data >= '0' && *data <= '9'){
		float temp = 0;
    uint8_t i = 0;
		while(*data >= '0' && *data <= '9'){
			temp *= 10;
			temp += (*data - 48);
			data++;
		}
		if(*data == '.'){
			data++;
			while(*data >= '0' && *data <= '9'){
				temp *= 10;
				temp += (*data - 48);
				data++;
				i++;
			}
		}
		return temp / pow(10,i);
	}
	else{
		return 0; 
	}
}



/**
  * @brief  �ַ���ת����
  * @param  dataĿ���ַ���
  * @retval �ַ����к��е�����
  */
int8_t Hex_To_Dec(const char* data)
{
    int8_t temp = 0;
    uint8_t flage = 0;
    if(*data == '-'){
        data++;
        flage = 1;
    }
		if((*data >= '0' && *data <= '9')){
			while(*data >= '0' && *data <= '9'){
				temp *= 10;
				temp += (*data - 48);
				
				data++;
		}
    if(flage == 1)
      temp *= -1;
		}
		return temp;
}


uint8_t NRF_Send(uint8_t* data,uint8_t tim_Count)
{
	uint8_t RX_Arr[RX_PLOAD_WIDTH];
	while(tim_Count--){
		TX_Mode();
		NRF24L01_TxPacket(data);
		RX_Mode();
		HAL_Delay(80);
		printf("data = %s\r\n",data);
		while(NRF24L01_RxPacket(RX_Arr) == 0);
		printf("RX_Arr1 = %s\r\n",RX_Arr);
		if(strstr((char*)RX_Arr,"OK") != NULL){
			return 1;
		}
	}
	return 0;
}


void Analytic_Parameter_NRF(char* data)
{
	uint8_t i = 0;
	for(i = 0;i < 8; i++)
	{
        if(( _Bool ) strstr ( data, RX_Cmd[i] ))
		    break;
	}
	printf("i:%dRX_DATA:%s\r\n",i,data);
	switch(i){
		case U:parameter_data[U] = Hex_To_Float((char*)(&data[2]));break;
		case I:parameter_data[I] = Hex_To_Float((char*)(&data[2]));break;
		case PA:parameter_data[PA] = Hex_To_Float((char*)(&data[3]));break;
		case PF:parameter_data[PF] = Hex_To_Float((char*)(&data[3]));break;
		case EA:parameter_data[EA] = Hex_To_Float((char*)(&data[3]));break;
		case READ_ID:Current_ID = Hex_To_Dec((char*)(&data[3]));break;
		case STUDY_OK:
		case CLEAR_OK:OLED_Clear();send_flage = YES_STUDY;
			TX_Mode();
			NRF24L01_TxPacket((uint8_t*)"OK");
			RX_Mode();
			break;
		case I_PA_VPT:{
			char* temp1 = strchr(data,':');
			VPT_Data[0] = Hex_To_Float(temp1+1);
			char* temp2 = strchr(data,'|');
			VPT_Data[1] = Hex_To_Float(temp2+1);
			printf("\r\ndata = %s\r\n",data);
			set_flage = YES_SET;
			TX_Mode();
			NRF24L01_TxPacket((uint8_t*)"OK");
			RX_Mode();
			break;
		}
	}
	printf("VPT_Data = %.2f\r\n",VPT_Data[0]);
	printf("VPT_Data = %.2f\r\n",VPT_Data[1]);
//	printf("PA = %.2f\r\n",parameter_data[PA]);
//	printf("PF = %.2f\r\n",parameter_data[PF]);
//	printf("EA = %f\r\n",parameter_data[EA]);
//	printf("ID = %d\r\n",Current_ID);
}



/**
  * @brief  ��������������ַ����еĵ������Ϣ��ȡ��
  * @param  data��Ҫ�������ַ���
  * @retval ��
  */
void Analytic_Parameter(char* data)
{
    uint8_t i = 0;
    for(i = 0;i < DEBUG_DATA_MAX;i++){
        if(data[i] == 'U')
            break;
    }
    if(i != DEBUG_DATA_MAX)
        parameter_data[U] = Hex_To_Float((char*)(&data[i+2]));

    for(i = 0;i < DEBUG_DATA_MAX;i++){
        if(data[i] == 'I')
            break;
    }
    if(i != DEBUG_DATA_MAX)
        parameter_data[I] = Hex_To_Float((char*)(&data[i+2]));

    for(i = 0;i < DEBUG_DATA_MAX;i++){
        if(data[i] == 'P' && data[i + 1] == 'A')
            break;
    }
    if(i != DEBUG_DATA_MAX)
        parameter_data[PA] = Hex_To_Float((char*)(&data[i+3]));


    for(i = 0;i < DEBUG_DATA_MAX;i++){
        if(data[i] == 'P' && data[i + 1] == 'F')
            break;
    }
    if(i != DEBUG_DATA_MAX)
        parameter_data[PF] = Hex_To_Float((char*)(&data[i+3]));

    for(i = 0;i < DEBUG_DATA_MAX;i++){
        if(data[i] == 'E' && data[i + 1] == 'A')
            break;
    }
    if(i != DEBUG_DATA_MAX)
        parameter_data[EA] = Hex_To_Float((char*)(&data[i+3]));

    printf("U = %.2f\r\n",parameter_data[U]);
    printf("I = %.2f\r\n",parameter_data[I]);
    printf("PA = %.2f\r\n",parameter_data[PA]);
    printf("PF = %.2f\r\n",parameter_data[PF]);
    printf("EA = %.2f\r\n",parameter_data[EA]);
}



/**
  * @brief  ���ڽ��ջص����������ڷ��������ڽ��յ����ݣ�
  * @param  data���ڽ��յ����ַ���
  * @retval ��
  */
void Usart_Debug_Run(const char* data)
{
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	uint8_t i;
  char raed_data_temp[DEBUG_DATA_MAX];
	for(i = 0;i < DEBUG_CMD_NUM; i++)
	{
        if(( _Bool ) strstr ( data, Debug_Cmd[i] ))
		    break;
	}
	switch(i){
		case PARAMETER:
				Debug_Usart_Struct.Read(raed_data_temp);
				Analytic_Parameter(raed_data_temp);
				break;
		case ID:
				Debug_Usart_Struct.Read(raed_data_temp);
				for(i = 0;i < DEBUG_DATA_MAX;i++){
						if(raed_data_temp[i] == ':')
								break;
				}
				if(i < DEBUG_DATA_MAX)
						Current_ID = Hex_To_Dec(&(raed_data_temp[i+1]));
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
	}
}


