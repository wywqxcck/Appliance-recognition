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
	if(Debug_Usart_Data_Struct.rx_len == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return DEBUG_READ_WAIT;
		
	if(Debug_Usart_Data_Struct.rx_len == debug_usart_cntPre || Debug_Usart_Data_Struct.rx_flage == DEBUG_READ_OK)//如果上一次的值和这次相同，则说明接收完毕
	{
		Debug_Usart_Data_Struct.rx_data[Debug_Usart_Data_Struct.rx_len++] = '\0';
		return DEBUG_READ_OK;								//返回接收完成标志
	}
		
	debug_usart_cntPre = Debug_Usart_Data_Struct.rx_len;					//置为相同
	
	return DEBUG_READ_WAIT;								//返回接收未完成标志

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
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0){}//循环发送,直到发送完毕   
    USART1->DR = (uint8_t) ch;      
	return ch;
}
#endif



