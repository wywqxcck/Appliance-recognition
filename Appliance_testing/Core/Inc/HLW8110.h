#ifndef __HLW8110_H_
#define __HLW8110_H_


#include "main.h"
#include "usart.h"
#include "stdio.h"


#define HLW8110_USART 						huart2
#define RX_MAX										32


#define D_CAL_U		967/1000		//��ѹУ��ϵ��
#define D_CAL_A_I	1000/1000		//Aͨ������У��ϵ��
#define D_CAL_A_P	967/1000		//Aͨ������У��ϵ��
#define D_CAL_A_E	1000/1000		//Aͨ������У��ϵ��


#define REG_SYSCON_ADDR         0x00						//ϵͳ���ƼĴ���
#define REG_EMUCON_ADDR					0x01						//�������ƼĴ���1
#define REG_EMUCON2_ADDR        0x13						//�������ƼĴ���2

#define REG_RMSIA_ADDR          0x24						//Aͨ��������Чֵ
#define REG_RMSU_ADDR 					0x26 						//��ѹ��Чֵ
#define REG_POWER_PA_ADDR       0x2C						//������Чֵ
#define REG_PF_ADDR             0x27						//��������ֵ
#define REG_ENERGY_PA_ADDR			0x28						//ͨ�� A �й�����


#define REG_CHECKSUM_ADDR				0x6f						//У��ϵ��У���
#define REG_RMS_IAC_ADDR				0x70						//����ͨ�� A ��Чֵת��ϵ��
#define REG_RMS_IBC_ADDR				0x71						//����ͨ�� B ��Чֵת��ϵ��
#define REG_RMS_UC_ADDR					0x72						//��ѹͨ�� U ��Чֵת��ϵ��
#define REG_POWER_PAC_ADDR			0x73						//����ͨ�� A �й�����ת��ϵ��
#define REG_POWER_PBC_ADDR			0x74						//����ͨ�� B �й�����ת��ϵ��
#define REG_POWER_SC_ADDR				0x75						//���ڹ���ת��ϵ��
#define REG_ENERGY_AC_ADDR			0x76						//A ͨ������ת��ϵ��
#define REG_ENERGY_BC_ADDR			0x77						//B ͨ������ת��ϵ��


#define PRINTF_DEBUG 						0


//HLW8110��ʼ��������Ϣ
typedef enum HLW8110_Init_Inf{
	HLW8110_OK,										//��ʼ���ɹ�	
	SYS_Con_Err,									//ϵͳ���������ô���
	EMU_Con1_Err,									//�������ƼĴ���1���ô���
	EMU_Con2_Err,									//�������ƼĴ���2���ô���
	IA_Err,												//Aͨ����������
	IB_Err,												//Bͨ����������
	U_Err,												//U��ѹͨ������
	PA_Err,												//Aͨ���й����ʹ���
	PB_Err,												//Bͨ���й����ʹ���
	PS_Err,												//���ڹ��ʹ���
	EA_Err,												//Aͨ����������
	EB_Err,												//Bͨ����������
	CS_Err,												//����ͼĴ�������
	SUM_Err,											//����ļ�������ȡ�Ĳ�ͬ
	UNKNOWN_Err,									//δ֪����
}HLW8110_Init_Inf;



//���ڵ�ǰ״̬
enum RX_Sta{
	Err,													//���ڽ���ʧ��
	OK,														//���ڽ������
	REV,													//�������ڽ���
};


//�������ݴ���
typedef struct{
	uint8_t rx_byte;							//�������ݻ�����
	uint8_t rx_data[RX_MAX];			//���յ�����
	uint8_t rx_len : 4;						//��Ҫ�������ݵĳ���
	uint8_t rx_index : 4;					//Ŀǰ���յĳ���
	enum RX_Sta rx_sta;						//Ŀǰ���ڵ�״̬
}HLW8110_Usart;


uint8_t HLW8110_Read_Reg(uint8_t add_reg,uint8_t len,uint32_t timOut);
HLW8110_Init_Inf Judge_CheckSum_HLW8110_Calfactor(void);
HLW8110_Init_Inf HLW8110_Write_Reg(uint8_t add_reg,uint16_t data);
void HLW8110_Set_Channel_A(void);
void HLW8110_WriteREG_DIS(void);
void HLW8110_WriteREG_EN(void);
float HLW8110_Read_U(void);
float HLW8110_Read_IA(void);
float HLW8110_Read_PA(void);
float HLW8110_Read_PF(void);
float HLW8110_Read_EA(void);
HLW8110_Init_Inf HLW8110_Init(void);
void Clear_Rx_Data(void);
void HLW8110_Con(void);
void HLW8110_Reset(void);


#endif


