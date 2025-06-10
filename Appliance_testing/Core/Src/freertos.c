/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "KNN.h"
#include "HLW8110.h"
#include "APP.h"
#include "Key.h" 
#include "24L01.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/*	./Keil5_disp_size_bar_v0.3.exe	*/
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for TestTask */
osThreadId_t TestTaskHandle;
const osThreadAttr_t TestTask_attributes = {
  .name = "TestTask",
  .stack_size = 704 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ReadHLW8110Task */
osThreadId_t ReadHLW8110TaskHandle;
const osThreadAttr_t ReadHLW8110Task_attributes = {
  .name = "ReadHLW8110Task",
  .stack_size = 192 * 4,
  .priority = (osPriority_t) osPriorityLow2,
};
/* Definitions for DetectionTask */
osThreadId_t DetectionTaskHandle;
const osThreadAttr_t DetectionTask_attributes = {
  .name = "DetectionTask",
  .stack_size = 192 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for StudyTask */
osThreadId_t StudyTaskHandle;
const osThreadAttr_t StudyTask_attributes = {
  .name = "StudyTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for NRFRaedTask */
osThreadId_t NRFRaedTaskHandle;
const osThreadAttr_t NRFRaedTask_attributes = {
  .name = "NRFRaedTask",
  .stack_size = 300 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal7,
};
/* Definitions for WifiTask */
osThreadId_t WifiTaskHandle;
const osThreadAttr_t WifiTask_attributes = {
  .name = "WifiTask",
  .stack_size = 300 * 4,
  .priority = (osPriority_t) osPriorityNormal6,
};
/* Definitions for HLW8110_Data */
osMessageQueueId_t HLW8110_DataHandle;
const osMessageQueueAttr_t HLW8110_Data_attributes = {
  .name = "HLW8110_Data"
};
/* Definitions for IDQueue */
osMessageQueueId_t IDQueueHandle;
const osMessageQueueAttr_t IDQueue_attributes = {
  .name = "IDQueue"
};
/* Definitions for Clear_ID */
osMessageQueueId_t Clear_IDHandle;
const osMessageQueueAttr_t Clear_ID_attributes = {
  .name = "Clear_ID"
};
/* Definitions for Wifi_Data */
osMessageQueueId_t Wifi_DataHandle;
const osMessageQueueAttr_t Wifi_Data_attributes = {
  .name = "Wifi_Data"
};
/* Definitions for TimeingQueue */
osMessageQueueId_t TimeingQueueHandle;
const osMessageQueueAttr_t TimeingQueue_attributes = {
  .name = "TimeingQueue"
};
/* Definitions for Timer01 */
osTimerId_t Timer01Handle;
const osTimerAttr_t Timer01_attributes = {
  .name = "Timer01"
};
/* Definitions for Timing_Timer */
osTimerId_t Timing_TimerHandle;
const osTimerAttr_t Timing_Timer_attributes = {
  .name = "Timing_Timer"
};
/* Definitions for SPIMutex */
osMutexId_t SPIMutexHandle;
const osMutexAttr_t SPIMutex_attributes = {
  .name = "SPIMutex"
};
/* Definitions for myEvent01 */
osEventFlagsId_t myEvent01Handle;
const osEventFlagsAttr_t myEvent01_attributes = {
  .name = "myEvent01"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    // ��¼������ջ�����������
    printf("Stack overflow in task: %s\n", pcTaskName);

    // ������ѭ����ֹͣ������
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

/* USER CODE END FunctionPrototypes */

void Test_Task(void *argument);
void Read_HLW8110_Task(void *argument);
void Detection_Task(void *argument);
void Study_Task(void *argument);
void NRF_Raed_Task(void *argument);
void Wifi_Task(void *argument);
void Timer_1ms(void *argument);
void Timing_Con(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of SPIMutex */
  SPIMutexHandle = osMutexNew(&SPIMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of Timer01 */
  Timer01Handle = osTimerNew(Timer_1ms, osTimerPeriodic, NULL, &Timer01_attributes);

  /* creation of Timing_Timer */
  Timing_TimerHandle = osTimerNew(Timing_Con, osTimerOnce, NULL, &Timing_Timer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
	osTimerStart(Timer01Handle,1);
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of HLW8110_Data */
  HLW8110_DataHandle = osMessageQueueNew (5, sizeof(float), &HLW8110_Data_attributes);

  /* creation of IDQueue */
  IDQueueHandle = osMessageQueueNew (1, sizeof(int8_t), &IDQueue_attributes);

  /* creation of Clear_ID */
  Clear_IDHandle = osMessageQueueNew (1, sizeof(int8_t), &Clear_ID_attributes);

  /* creation of Wifi_Data */
  Wifi_DataHandle = osMessageQueueNew (1, sizeof(char*), &Wifi_Data_attributes);

  /* creation of TimeingQueue */
  TimeingQueueHandle = osMessageQueueNew (16, sizeof(uint16_t), &TimeingQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of TestTask */
  TestTaskHandle = osThreadNew(Test_Task, NULL, &TestTask_attributes);

  /* creation of ReadHLW8110Task */
  ReadHLW8110TaskHandle = osThreadNew(Read_HLW8110_Task, NULL, &ReadHLW8110Task_attributes);

  /* creation of DetectionTask */
  DetectionTaskHandle = osThreadNew(Detection_Task, NULL, &DetectionTask_attributes);

  /* creation of StudyTask */
  StudyTaskHandle = osThreadNew(Study_Task, NULL, &StudyTask_attributes);

  /* creation of NRFRaedTask */
  NRFRaedTaskHandle = osThreadNew(NRF_Raed_Task, NULL, &NRFRaedTask_attributes);

  /* creation of WifiTask */
  WifiTaskHandle = osThreadNew(Wifi_Task, NULL, &WifiTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* creation of myEvent01 */
  myEvent01Handle = osEventFlagsNew(&myEvent01_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
	vTaskSuspend(StudyTaskHandle);
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_Test_Task */
/**
  * @brief  Function implementing the TestTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Test_Task */
void Test_Task(void *argument)
{
  /* USER CODE BEGIN Test_Task */
	char InfoBuffer[1024];
	uint32_t r_event;
	osStatus_t err = osError;
	int8_t id = -1;
  /* Infinite loop */
  for(;;)
  {
	  r_event = osEventFlagsWait(myEvent01Handle,             /* �¼������� */ 
                              PRINT_LIST_EVEN | ENTER_STUDY_EVEN | QUIT_STUDY_EVEN | WRITE_W25Q64_EVEN | POWER_DOWN_EVEN | SWITCH_TOGGLE_EVEN 
															| WIFI_REST_EVEN | TIME_OUT_EVEN | TIME_START_EVEN,    /* �����������Ȥ���¼� */ 
                              osFlagsWaitAny,           /* �˳�ʱ����¼�λ��ͬʱ�������Ȥ�������¼� */ 
                              osWaitForever);           /* ָ����ʱ�¼�,һֱ�� */
		
		if((r_event & PRINT_LIST_EVEN) == PRINT_LIST_EVEN){//�������״̬
			vTaskList(InfoBuffer);
			printf("=================================================\r\n");
			printf("������        ����״̬ ���ȼ�  ʣ��ջ  ����� \r\n");
			printf("=================================================\r\n");
			printf("%s\r\n", InfoBuffer);
			printf("  B������  R������  D��ɾ��  S����ͣ  X������ \r\n");
			printf("=================================================\r\n");
			printf("=================================================\r\n");
		}
		else if((r_event & ENTER_STUDY_EVEN) == ENTER_STUDY_EVEN){//����ѧϰģʽ
			osMessageQueueGet(IDQueueHandle,&id,0,0);//����ѧϰģʽ֮ǰ��ȡ�߻�������id
			vTaskSuspend(DetectionTaskHandle);
			vTaskResume(StudyTaskHandle);
		}
		else if((r_event & QUIT_STUDY_EVEN) == QUIT_STUDY_EVEN){//�˳�ѧϰģʽ
			vTaskSuspend(StudyTaskHandle);
			vTaskResume(DetectionTaskHandle);
		}
		else if((r_event & WRITE_W25Q64_EVEN) == WRITE_W25Q64_EVEN){//���ݱ���
				err = osMutexAcquire(SPIMutexHandle,2000);//��ȡ������
				if(err == osOK){
					Write_W25Q();//���ݴ洢
					osMutexRelease(SPIMutexHandle);//�ͷŻ�����
				}
				else{
					printf("Test_Task Get Mutex SPI Err:%d\r\n",err);
				}
		}
		else if((r_event & POWER_DOWN_EVEN) == POWER_DOWN_EVEN){//�ϵ����
				err = osMutexAcquire(SPIMutexHandle,2000);//��ȡ������
				if(err == osOK){
					Write_W25Q();//���ݴ洢
					vTaskSuspend(DetectionTaskHandle);
					vTaskSuspend(StudyTaskHandle);
					vTaskSuspend(NRFRaedTaskHandle);
					vTaskSuspend(ReadHLW8110TaskHandle);
					vTaskSuspend(WifiTaskHandle);
					LED_Struct.Set_Mode(LED_OFF);
					osMutexRelease(SPIMutexHandle);//�ͷŻ�����
					printf("RUN STOP!!!\r\n");
					NRF_LowPower_Mode();
					HAL_PWR_EnterSTANDBYMode();
				}
				else{
					printf("Test_Task Get Mutex SPI Err:%d\r\n",err);
				}
		}
		else if((r_event & SWITCH_TOGGLE_EVEN) == SWITCH_TOGGLE_EVEN){//�����л�
				err = osMutexAcquire(SPIMutexHandle,2000);//��ȡ������
				if(err == osOK){
					Switch_Struct.Set(!Switch_Struct.Get());
					osMutexRelease(SPIMutexHandle);//�ͷŻ�����
				}
				else{
					printf("Test_Task Get Mutex SPI Err:%d\r\n",err);
				}
		}
		else if((r_event & WIFI_REST_EVEN) == WIFI_REST_EVEN){//WIFI����
			osThreadTerminate(WifiTaskHandle);
			WifiTaskHandle = NULL;
			WifiTaskHandle = osThreadNew(Wifi_Task, NULL, &WifiTask_attributes);
		}
		else if((r_event & TIME_START_EVEN) == TIME_START_EVEN){//��ʱ��ʼ
				err = osMutexAcquire(SPIMutexHandle,2000);//��ȡ������
				if(err == osOK){
					uint16_t time = 0;
					if(osMessageQueueGet(TimeingQueueHandle,&time,0,10) == osOK){
						Switch_Struct.Set(ON);
						if(osTimerIsRunning(Timing_TimerHandle) == 0){
							osTimerStart(Timing_TimerHandle,1000 * time);
						}
						else{
							osTimerStop(Timing_TimerHandle);
							osTimerStart(Timing_TimerHandle,1000 * time);
						}
					}
					else{
						
					}
					osMutexRelease(SPIMutexHandle);//�ͷŻ�����
				}
				else{
					printf("Test_Task Get Mutex SPI Err:%d\r\n",err);
				}
		}
		else if((r_event & TIME_OUT_EVEN) == TIME_OUT_EVEN){//��ʱʱ�䵽
				err = osMutexAcquire(SPIMutexHandle,2000);//��ȡ������
				if(err == osOK){
					printf("time out\r\n");
					Switch_Struct.Set(OFF);
					osMutexRelease(SPIMutexHandle);//�ͷŻ�����
				}
				else{
					printf("Test_Task Get Mutex SPI Err:%d\r\n",err);
				}
		}
    osDelay(10);
  }
  /* USER CODE END Test_Task */
}

/* USER CODE BEGIN Header_Read_HLW8110_Task */
/**
* @brief Function implementing the ReadHLW8110Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Read_HLW8110_Task */
void Read_HLW8110_Task(void *argument)
{
  /* USER CODE BEGIN Read_HLW8110_Task */
	float data[READ_DATA_NUM];
	uint32_t count = 0;
	osStatus_t err = osOK;
  /* Infinite loop */
  for(;;)
  {
		if(count % READ_DATA_TIME == 0){//���뷢��һ��
			if(Read_HLW8110_Electrical(data) == OK){//��ȡ������ɹ�
				for(uint8_t i = U;i <= EA;i++){//�ӵ�ѹ�����ܽ��з���
					err = osMessageQueuePut(HLW8110_DataHandle,(float*)(&data[i]),0,100);
					if(err != osOK){//����ʧ�����������Ϣ
						printf("Read_HLW8110_Task Send Queue HLW8110 Data ERR:%d\r\n",err);
					}
				}
			}
			else{//δ���������
				err = osMessageQueuePut(HLW8110_DataHandle,(float*)(&data[U]),0,100);
				if(err != osOK){//����ʧ�����������Ϣ
					printf("Read_HLW8110_Task Send Queue HLW8110 Data ERR:%d\r\n",err);
				}
				for(uint8_t i = I;i < EA;i++){//�ӵ�ѹ�����ܽ��з���
					data[i] = 0;
					err = osMessageQueuePut(HLW8110_DataHandle,(float*)(&data[i]),0,100);
					if(err != osOK){//����ʧ�����������Ϣ
						printf("Read_HLW8110_Task Send Queue HLW8110 Data ERR:%d\r\n",err);
					}
				}
				err = osMessageQueuePut(HLW8110_DataHandle,(float*)(&data[EA]),0,100);
				if(err != osOK){//����ʧ�����������Ϣ
					printf("Read_HLW8110_Task Send Queue HLW8110 Data ERR:%d\r\n",err);
				}
			}
		}
		else{
			Read_HLW8110_Electrical(data);
		}
		count++;
    osDelay(1);
  }
  /* USER CODE END Read_HLW8110_Task */
}

/* USER CODE BEGIN Header_Detection_Task */
/**
* @brief Function implementing the DetectionTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Detection_Task */
void Detection_Task(void *argument)
{
  /* USER CODE BEGIN Detection_Task */
	uint8_t queue_num;
	osStatus_t err = osError;
	float data[READ_DATA_NUM];
	int8_t id = -1;
	uint8_t NRF_TX_Data[TX_PLOAD_WIDTH];
  /* Infinite loop */
  for(;;)
  {
		queue_num = osMessageQueueGetCount(HLW8110_DataHandle);
		if(queue_num == READ_DATA_NUM){//����д����Ͻ��л�ȡ�����
			for(uint8_t i = 0;i < READ_DATA_NUM;i++){
				err = osMessageQueueGet(HLW8110_DataHandle,(float*)(&data[i]),0,10);
				if(err != osOK){
					printf("Detection_Task Get Queue HLW8110 Data Err:%d\r\n",err);
				}
			}
			id = predict(&data[I],NUM_FEATURES);//���е���ƥ��ӵ�����ʼ
			Wifi_Send(data,id);
			err = osMutexAcquire(SPIMutexHandle,10);//��ȡ������
			if(err == osOK){
				Printf_parameter(data);//�������Ϣ���
				NRF24L01_TX_Mode();
				sprintf((char*)NRF_TX_Data,"ID:%d",id);//ID
				NRF24L01_TxPacket(NRF_TX_Data);
				NRF24L01_RX_Mode();
				osMutexRelease(SPIMutexHandle);//�ͷŻ�����
			}
			else{
				printf("Detection_Task Get Mutex SPI Err:%d\r\n",err);
			}
		}
    osDelay(10);
  }
  /* USER CODE END Detection_Task */
}

/* USER CODE BEGIN Header_Study_Task */
/**
* @brief Function implementing the StudyTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Study_Task */
void Study_Task(void *argument)
{
  /* USER CODE BEGIN Study_Task */
	int8_t id = -1;
	uint8_t queue_num;
	static uint8_t count = 0;
	float data[READ_DATA_NUM];
	osStatus_t err = osError;
  /* Infinite loop */
  for(;;)
  {
		if(id == -1){//û��ID������ȴ��ӻ�������Ҫѧϰ��ID
			while(err != osOK){
				queue_num = osMessageQueueGetCount(HLW8110_DataHandle);//��ȡ�������еĵ����
				for(uint8_t i = 0;i < queue_num;i++){//���������������
					err = osMessageQueueGet(HLW8110_DataHandle,(float*)(&data[i]),0,0);
					if(err != osOK){
						printf("Study_Task Get Queue HLW8110 Data Err:%d\r\n",err);
					}
				}
				err = osMessageQueueGet(IDQueueHandle,&id,0,1500);//��ȡ��Ҫѧϰ��ID.
			}
			printf("study_task id :%d\r\n",id);
		}
		queue_num = osMessageQueueGetCount(HLW8110_DataHandle);//��ȡ�����
		if(queue_num == READ_DATA_NUM && id != -1){
			for(uint8_t i = 0;i < READ_DATA_NUM;i++){
				err = osMessageQueueGet(HLW8110_DataHandle,(float*)(&data[i]),0,0);
				if(err != osOK){
					printf("Study_Task Get Queue HLW8110 Data Err:%d\r\n",err);
				}
			}
			//Printf_parameter(data);//��������
			if(data[PA] >= PA_MIN){//��¼��Ч����
				for(uint8_t i = 0;i < NUM_FEATURES;i++){//��������Ž�ƥ�����ݿ�
					X_train[count + (id * NUM_FEATURES)][i] = data[i+1];
					printf("X_train[%d][%d]%f\r\n",count + (id * NUM_FEATURES),i,X_train[count + (id * NUM_FEATURES)][i]);
				}
				count++;//��¼�����˼�������
				if(count >= NUM_SAMPLES / NUM_CLASS){
					id = -1;
					count = 0;
					err = osMutexAcquire(SPIMutexHandle,osWaitForever);//��ȡ������
					if(err == osOK){
						osEventFlagsSet(myEvent01Handle, WRITE_W25Q64_EVEN);//���ݴ洢
						while(NRF_Send((uint8_t*)"study ok",10) != 1){osDelay(10);}//����ѧϰ���
						osMutexRelease(SPIMutexHandle);//�ͷŻ�����
						err = osError;
					}
					else{
						printf("Study_Task Get Mutex SPI Err:%d\r\n",err);
					}
				}
			}
			else{
				printf("PA min err\r\n");
			}
		}
    osDelay(10);
  }
  /* USER CODE END Study_Task */
}

/* USER CODE BEGIN Header_NRF_Raed_Task */
/**
* @brief Function implementing the NRFRaedTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_NRF_Raed_Task */
void NRF_Raed_Task(void *argument)
{
  /* USER CODE BEGIN NRF_Raed_Task */
	uint8_t NRF_RX_Data[RX_PLOAD_WIDTH];
	osStatus_t err = osError;
  /* Infinite loop */
  for(;;)
  {
		err = osMutexAcquire(SPIMutexHandle,1000);//��ȡ������
		if(err == osOK){
			if(NRF24L01_RxPacket(NRF_RX_Data) == 0){
				printf("NRF_RX_Data = %s\r\n",NRF_RX_Data);
				Infor_analysis((char*)NRF_RX_Data);
				memset(NRF_RX_Data,0,sizeof(NRF_RX_Data));
			}
			osMutexRelease(SPIMutexHandle);//�ͷŻ�����
		}
		else{
			printf("NRF_Raed_Task Get Mutex SPI Err:%d\r\n",err);
		}
    osDelay(10);
  }
  /* USER CODE END NRF_Raed_Task */
}

/* USER CODE BEGIN Header_Wifi_Task */
/**
* @brief Function implementing the WifiTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Wifi_Task */
void Wifi_Task(void *argument)
{
  /* USER CODE BEGIN Wifi_Task */
	LED_Struct.Set_Mode(LED_Blink);
	LED_Struct.Set_Blink(400);
	ESP8266_Struct.Init();
	LED_Struct.Set_Mode(LED_ON);
	uint8_t ping_count = 0,time_out = 0;
	uint16_t send_count = 0;
	osStatus_t err = osError;
	unsigned char* read_data = NULL;
	char* send_data = pvPortMalloc(256);
  /* Infinite loop */
  for(;;)
  {
		ping_count++;//ping����
		read_data = ESP8266_Struct.Read_Data(0);//��ȡWIFI���յ�����
		if(read_data != NULL){//��ȡ�ɹ����н���
			err = osMutexAcquire(SPIMutexHandle,1000);//��ȡ������
			if(err == osOK){
				printf("read_data = %s\r\n",read_data);
				extract_data((char*)read_data);//����WIFI���ݽ���
				read_data = NULL;
				osMutexRelease(SPIMutexHandle);//�ͷŻ�����
			}
			else{
				printf("Wifi_Task Get Mutex SPI Err:%d\r\n",err);
			}
		}
		err = osMessageQueueGet(Wifi_DataHandle,&send_data,0,0);
		if(err == osOK){//�����ϴ�
			send_count = 0;
			if(ESP8266_Struct.SendCmd(send_data, "cmd=2&res=1")){
				printf("send err\r\n");
				LED_Struct.Set_Mode(LED_OFF);
			}
			else{
				LED_Struct.Set_Mode(LED_ON);
			}
		}
		else{
			send_count++;
			if(send_count >= 500){//WIFI���ݽ���ʧ��
				printf("Wifi_DataHandle Read Err:%d\r\n",err);
				err = osMessageQueueDelete(Wifi_DataHandle);//��ɾ������
				if(err != osOK){//ɾ��ʧ��
					printf("Wifi_DataHandle Delete err:%d\r\n",err);
				}
				else{//ɾ���ɹ�
					printf("Wifi_DataHandle Delete ok\r\n");
				}
				Wifi_DataHandle = NULL;
				Wifi_DataHandle = osMessageQueueNew (1, sizeof(char*), &Wifi_Data_attributes);//��������
				if(Wifi_DataHandle != NULL){
					printf("Wifi_DataHandle New ok\r\n");
				}
				else{
					printf("Wifi_DataHandle New err\r\n");
				}
				send_count = 0;
			}
		}
		if(ping_count >= 100){//����ping
			if(ESP8266_Struct.SendCmd("ping\r\n", "cmd=0&res=1")){
				printf("ping err\r\n");
				time_out++;
				//printf("sta = %d\r\n",ESP8266_GetCIPStatus());
				LED_Struct.Set_Mode(LED_OFF);
			}
			else{
				time_out = 0;
				ping_count = 0;
				LED_Struct.Set_Mode(LED_ON);
			}
			
		}
		if(time_out >= 5){
			
			osEventFlagsSet(myEvent01Handle, WIFI_REST_EVEN);//��ʱ����WIFI��λ
		}
    osDelay(10);
  }
  /* USER CODE END Wifi_Task */
}

/* Timer_1ms function */
void Timer_1ms(void *argument)
{
  /* USER CODE BEGIN Timer_1ms */
	static uint8_t Count_Key = 0;
	Count_Key++;
	if(Count_Key >= 5){
		button_ticks();
		Count_Key = 0;
	}
	LED_Struct.Con();
	Switch_Struct.Con();
  /* USER CODE END Timer_1ms */
}

/* Timing_Con function */
void Timing_Con(void *argument)
{
  /* USER CODE BEGIN Timing_Con */
	osEventFlagsSet(myEvent01Handle, TIME_OUT_EVEN);
  /* USER CODE END Timing_Con */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

