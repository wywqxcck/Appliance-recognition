#include "debug_usart.h"


static void Debug_Usart_Init(void (*cmd)(const char* data));
static _Bool Deubu_Usart_Read(char *data);
static void Debug_Usart_Con(void);
static void Debug_Usart_Clear(void);

#if !DEBUG_DMA_IDLE
`static _Bool Debug_Usart_WaitRecive(void);
#endif


uint8_t debug_usart_data;
_Bool debug_usart_falge,debug_usart_rev;

Debug_Usart_Typedef Debug_Usart_Struct = {
	.Init = Debug_Usart_Init,
	.Read = Deubu_Usart_Read,
	.Con = Debug_Usart_Con,
	.Clear = Debug_Usart_Clear,
	.Cmd = NULL,
};

Debug_Usart_Data Debug_Usart_Data_Struct = { 0 };


static void Debug_Usart_Clear(void)
{
	memset(Debug_Usart_Data_Struct.rx_data, 0, sizeof(Debug_Usart_Data_Struct.rx_data));
	Debug_Usart_Data_Struct.rx_flage = DEBUG_READ_WAIT;
	Debug_Usart_Data_Struct.rx_len = 0;
}


static void Debug_Usart_Init(void (*cmd)(const char* data))
{
#if !DEBUG_DMA_IDLE
	HAL_UART_Receive_IT(&DEBUG_USART,&debug_usart_data,1); 
#else
	HAL_UARTEx_ReceiveToIdle_DMA(&DEBUG_USART, Debug_Usart_Data_Struct.rx_data, DEBUG_DATA_MAX);
#endif
	Debug_Usart_Clear();
	Debug_Usart_Struct.Cmd = cmd;
}			


#if !DEBUG_DMA_IDLE
static _Bool Debug_Usart_WaitRecive(void)
{
	static uint16_t debug_usart_cntPre = 0;
	if(Debug_Usart_Data_Struct.rx_len == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return DEBUG_READ_WAIT;
		
	if(Debug_Usart_Data_Struct.rx_len == debug_usart_cntPre || Debug_Usart_Data_Struct.rx_flage == DEBUG_READ_OK)//�����һ�ε�ֵ�������ͬ����˵���������
	{
		Debug_Usart_Data_Struct.rx_data[Debug_Usart_Data_Struct.rx_len++] = '\0';
		return DEBUG_READ_OK;								//���ؽ�����ɱ�־
	}
		
	debug_usart_cntPre = Debug_Usart_Data_Struct.rx_len;					//��Ϊ��ͬ
	
	return DEBUG_READ_WAIT;								//���ؽ���δ��ɱ�־

}
#endif




static _Bool Deubu_Usart_Read(char *data)
{
#if !DEBUG_DMA_IDLE
	if(Debug_Usart_WaitRecive() == DEBUG_READ_OK){
			memcpy(data,Debug_Usart_Data_Struct.rx_data, Debug_Usart_Data_Struct.rx_len);
			Debug_Usart_Clear();
			return DEBUG_READ_OK;
	}
	else{
		data = NULL;
		return DEBUG_READ_WAIT;
	}
#else
	if(Debug_Usart_Data_Struct.rx_flage == DEBUG_READ_OK){
		memcpy(data,Debug_Usart_Data_Struct.rx_data, Debug_Usart_Data_Struct.rx_len);
		Debug_Usart_Clear();
		return DEBUG_READ_OK;
	}
	else{
		data = NULL;
		return DEBUG_READ_WAIT;
	}
#endif
}




#if !DEBUG_DMA_IDLE

static void Debug_Usart_Con(void)
{
	static _Bool debug_usart_rev = 0;
	if(Debug_Usart_Data_Struct.rx_len >= DEBUG_DATA_MAX)
		Debug_Usart_Data_Struct.rx_len = 0;
	Debug_Usart_Data_Struct.rx_data[Debug_Usart_Data_Struct.rx_len++] = debug_usart_data;
	if(debug_usart_data == '\r'){
		debug_usart_rev = 1;
	}
	if(debug_usart_rev == 1 && debug_usart_data == '\n'){
		if(Debug_Usart_Struct.Cmd != NULL){
			Debug_Usart_Data_Struct.rx_flage = DEBUG_READ_OK;
			Debug_Usart_Struct.Cmd((const char*)Debug_Usart_Data_Struct.rx_data);
		}
		debug_usart_rev = 0;
	}
	HAL_UART_Receive_IT(&DEBUG_USART,&debug_usart_data,1); 
}
#else

static void Debug_Usart_Con(void)
{
	HAL_UART_DMAStop(&DEBUG_USART);
	Debug_Usart_Data_Struct.rx_len = DEBUG_DATA_MAX - __HAL_DMA_GET_COUNTER(&DEBUG_HDMA_RX);
	Debug_Usart_Data_Struct.rx_flage = DEBUG_READ_OK;
	Debug_Usart_Data_Struct.rx_data[Debug_Usart_Data_Struct.rx_len++] = '\0';
	if(Debug_Usart_Struct.Cmd != NULL)
		Debug_Usart_Struct.Cmd((const char*)Debug_Usart_Data_Struct.rx_data);
	HAL_UARTEx_ReceiveToIdle_DMA(&DEBUG_USART, Debug_Usart_Data_Struct.rx_data, DEBUG_DATA_MAX);
}


#endif

#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0){}//ѭ������,ֱ���������   
    USART1->DR = (uint8_t) ch;      
	return ch;
}
#endif



