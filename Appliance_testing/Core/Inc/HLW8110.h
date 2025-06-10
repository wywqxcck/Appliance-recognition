#ifndef __HLW8110_H_
#define __HLW8110_H_


#include "main.h"
#include "usart.h"
#include "stdio.h"


#define HLW8110_USART 						huart2
#define RX_MAX										32


#define D_CAL_U		967/1000		//电压校正系数
#define D_CAL_A_I	1000/1000		//A通道电流校正系数
#define D_CAL_A_P	967/1000		//A通道功率校正系数
#define D_CAL_A_E	1000/1000		//A通道电能校正系数


#define REG_SYSCON_ADDR         0x00						//系统控制寄存器
#define REG_EMUCON_ADDR					0x01						//计量控制寄存器1
#define REG_EMUCON2_ADDR        0x13						//计量控制寄存器2

#define REG_RMSIA_ADDR          0x24						//A通道电流有效值
#define REG_RMSU_ADDR 					0x26 						//电压有效值
#define REG_POWER_PA_ADDR       0x2C						//功率有效值
#define REG_PF_ADDR             0x27						//功率因数值
#define REG_ENERGY_PA_ADDR			0x28						//通道 A 有功电能


#define REG_CHECKSUM_ADDR				0x6f						//校表系数校验和
#define REG_RMS_IAC_ADDR				0x70						//电流通道 A 有效值转换系数
#define REG_RMS_IBC_ADDR				0x71						//电流通道 B 有效值转换系数
#define REG_RMS_UC_ADDR					0x72						//电压通道 U 有效值转换系数
#define REG_POWER_PAC_ADDR			0x73						//电流通道 A 有功功率转换系数
#define REG_POWER_PBC_ADDR			0x74						//电流通道 B 有功功率转换系数
#define REG_POWER_SC_ADDR				0x75						//视在功率转换系数
#define REG_ENERGY_AC_ADDR			0x76						//A 通道能量转换系数
#define REG_ENERGY_BC_ADDR			0x77						//B 通道能量转换系数


#define PRINTF_DEBUG 						0


//HLW8110初始化错误信息
typedef enum HLW8110_Init_Inf{
	HLW8110_OK,										//初始化成功	
	SYS_Con_Err,									//系统控制器设置错误
	EMU_Con1_Err,									//计量控制寄存器1设置错误
	EMU_Con2_Err,									//计量控制寄存器2设置错误
	IA_Err,												//A通道电流故障
	IB_Err,												//B通道电流故障
	U_Err,												//U电压通道故障
	PA_Err,												//A通道有功功率故障
	PB_Err,												//B通道有功功率故障
	PS_Err,												//视在功率故障
	EA_Err,												//A通道能量故障
	EB_Err,												//B通道能量故障
	CS_Err,												//检验和寄存器故障
	SUM_Err,											//计算的检验和与读取的不同
	UNKNOWN_Err,									//未知错误
}HLW8110_Init_Inf;



//串口当前状态
enum RX_Sta{
	Err,													//串口接收失败
	OK,														//串口接收完成
	REV,													//串口正在接收
};


//串口数据处理
typedef struct{
	uint8_t rx_byte;							//接收数据缓冲区
	uint8_t rx_data[RX_MAX];			//接收的数据
	uint8_t rx_len : 4;						//需要接收数据的长度
	uint8_t rx_index : 4;					//目前接收的长度
	enum RX_Sta rx_sta;						//目前串口的状态
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


