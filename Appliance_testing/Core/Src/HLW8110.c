#include "HLW8110.h"


HLW8110_Usart HLW8110_Usart_Struct;


uint32_t	U16_RMSIAC_RegData; 			// A通道电流转换系数
uint32_t	U16_RMSIBC_RegData; 			// B通道电流转换系数
uint32_t	U16_RMSUC_RegData; 				// 电压通道转换系数
uint32_t	U16_PowerPAC_RegData; 		// A通道功率转换系数
uint32_t	U16_PowerPBC_RegData; 		// B通道功率转换系数
uint32_t	U16_PowerSC_RegData; 			// 视在功率转换系数,如果选择A通道，则是A通道视在功率转换系数。A和B通道只能二者选其一
uint32_t	U16_EnergyAC_RegData; 		// A通道有功电能(量)转换系数 
uint32_t	U16_EnergyBC_RegData; 		// A通道有功电能(量)转换系数
uint32_t	U16_CheckSUM_RegData; 		// 转换系数的CheckSum
uint32_t	U16_CheckSUM_Data; 				// 转换系数计算出来的CheckSum



/*=========================================================================================================
 * Function : HLW8110_Init_Inf HLW8110_Init(void)
 * Describe : 初始化HW110模块
 * Input    : none
 * Return   : 初始化状态以及错误信息
 * Record   : 2024/09/11
==========================================================================================================*/
HLW8110_Init_Inf HLW8110_Init(void)
{
	HLW8110_Init_Inf err = HLW8110_OK;
	HAL_UART_Receive_IT(&HLW8110_USART, &HLW8110_Usart_Struct.rx_byte, 1);
	Clear_Rx_Data();
	
	HLW8110_WriteREG_EN();//打开写保护
	HAL_Delay(10);
	
	//电流通道A设置命令，指定当前用于计算视在功率、功率因数、相角、瞬时有功功率、瞬时视在功率和有功功率过载的信号指示 的通道为通道A	
	HLW8110_Set_Channel_A();
	HAL_Delay(10);
	
	if(HLW8110_Write_Reg(REG_SYSCON_ADDR,0x0A04) != HLW8110_OK){//开启A通道，关闭B通道，电压通道PGA = 1，电流通道PGA = 16
		return SYS_Con_Err;
	}
	
	if(HLW8110_Write_Reg(REG_EMUCON_ADDR,0x0001) != HLW8110_OK){	//1，使能PFA 脉冲输出和有功电能寄存器累加；
		return EMU_Con1_Err;
	}
		
	if(HLW8110_Write_Reg(REG_EMUCON2_ADDR,0x0065) != HLW8110_OK){	//0x0001是EMUCON2的默认值，waveEn = 1,zxEn = 1，A通道电量寄存器，读后不清0，EPA_CB = 1；打开功率因素检测
		return EMU_Con2_Err;
	}
		
	HLW8110_WriteREG_DIS();//关闭写保护
	
	err = Judge_CheckSum_HLW8110_Calfactor();//验证地址0x70-0x77地址的系数和
#if PRINTF_DEBUG
		printf("A通道电流转换系数:%x\r\n" ,U16_RMSIAC_RegData);
		HAL_Delay(1);
		printf("B通道电流转换系数:%x\r\n" ,U16_RMSIBC_RegData);
		HAL_Delay(1);
		printf("电压通道转换系数:%x\r\n" ,U16_RMSUC_RegData);
		HAL_Delay(1);
		printf("A通道功率转换系数:%x\r\n" ,U16_PowerPAC_RegData);
		HAL_Delay(1);
		printf("B通道功率转换系数:%x\r\n" ,U16_PowerPBC_RegData);
		HAL_Delay(1);
		printf("视在功率转换系数:%x\r\n" ,U16_PowerSC_RegData);
		HAL_Delay(1);
		printf("A通道电量转换系数:%x\r\n" ,U16_EnergyAC_RegData);
		HAL_Delay(1);
		printf("B通道电量转换系数:%x\r\n" ,U16_EnergyBC_RegData);
		HAL_Delay(10);
		printf("转换系数:%x\r\n" ,U16_CheckSUM_RegData);
		HAL_Delay(10);
		printf("转换系数计算出的:%x\r\n" ,U16_CheckSUM_Data);
		Clear_Rx_Data();
#endif
	return err;
}



/*=========================================================================================================
 * Function : void HLW8110_Send_Data(uint8_t* data,uint8_t len)
 * Describe : 串口发送
 * Input    : data:发送的数据指针   len:发送数据的长度
 * Return   : none
 * Record   : 2024/09/11
==========================================================================================================*/
void HLW8110_Send_Data(uint8_t* data,uint8_t len)
{
	HAL_UART_Transmit(&HLW8110_USART, data, (uint16_t)len, 1000);
}


/*=========================================================================================================
 * Function : void Clear_RxBuf(void)
 * Describe : 在准备接收串口数据前，清空接收缓存器的数据
 * Input    : none
 * Return   : none
 * Record   : 2024/09/11
=========================================================================================================*/
void Clear_Rx_Data(void)
{
	for(uint16_t i = 0;i < RX_MAX;i++){
		HLW8110_Usart_Struct.rx_data[i] = 0;
	}
	HLW8110_Usart_Struct.rx_index = 0;
	HLW8110_Usart_Struct.rx_len = 0;
	HLW8110_Usart_Struct.rx_sta = REV;
}


/*=========================================================================================================
 * Function : uint8_t HLW8110_checkSum_Read(uint8_t add_reg)
 * Describe : 读取HLW8110寄存器的值后进行校验和
 * Input    : 寄存器地址
 * Return   : 校验值
 * Record   : 2024/09/11
==========================================================================================================*/
uint8_t HLW8110_checkSum_Read(uint8_t add_reg)
{
	uint8_t i;
	uint8_t Temp_u8_checksum;
	uint32_t a;

	a = 0x0000;
	Temp_u8_checksum = 0;
	for (i = 0; i< (HLW8110_Usart_Struct.rx_len-1); i++)
	{
			a += HLW8110_Usart_Struct.rx_data[i];
	}
	a = a + 0xa5 + add_reg;
	a = ~a;
	Temp_u8_checksum = a & 0xff;
	return Temp_u8_checksum;
}



/*=========================================================================================================
 * Function : uint8_t HLW8110_checkSum_Write(uint8_t* data,uint8_t len)
 * Describe : 发送时读数据进行计算校验和
 * Input    : data:需要校验的数据   len:数据的长度
 * Return   : 校验值
 * Record   : 2024/09/11
==========================================================================================================*/
uint8_t HLW8110_checkSum_Write(uint8_t* data,uint8_t len)
{
	uint8_t i;
	uint8_t Temp_u8_checksum;
	uint32_t a;

	a = 0x0000;
	Temp_u8_checksum = 0;
	for (i = 0; i < len; i++)
	{
		a += data[i];
	}
	a = ~a;
	Temp_u8_checksum = a & 0xff;
	
	return Temp_u8_checksum;
}



/*=========================================================================================================
 * Function : void HLW8110_Set_Channel_A(void)
 * Describe : 设置视在功率、功率因数、相角、瞬时有功功率、瞬时视在功率和有功功率过载的信号指示 的通道为通道A	
 * Input    : none
 * Return   : none
 * Record   : 2024/09/11
==========================================================================================================*/
void HLW8110_Set_Channel_A(void)
{
	uint8_t tx_data[4];
	
	tx_data[0] = 0xA5;
	tx_data[1] = 0xEA;
	tx_data[2] = 0x5A;
	tx_data[3] = HLW8110_checkSum_Write(tx_data,3);
	HLW8110_Send_Data(tx_data,4);
}


/*=========================================================================================================
 * Function : void HLW8110_WriteREG_EN(void)
 * Describe : 使能写保护
 * Input    : none
 * Return   : none
 * Record   : 2024/09/11
==========================================================================================================*/
void HLW8110_WriteREG_EN(void)
{
	uint8_t tx_data[4];
	
	tx_data[0] = 0xA5;
	tx_data[1] = 0xEA;
	tx_data[2] = 0xE5;
	tx_data[3] = HLW8110_checkSum_Write(tx_data,3);
	HLW8110_Send_Data(tx_data,4);
}


/*=========================================================================================================
 * Function : void HLW8110_WriteREG_DIS(void)
 * Describe : 失能写保护
 * Input    : none
 * Return   : none
 * Record   : 2024/09/11
==========================================================================================================*/
void HLW8110_WriteREG_DIS(void)
{
	uint8_t tx_data[4];
	
	tx_data[0] = 0xA5;
	tx_data[1] = 0xEA;
	tx_data[2] = 0xDC;
	tx_data[3] = HLW8110_checkSum_Write(tx_data,3);
	HLW8110_Send_Data(tx_data,4);
}



/*=========================================================================================================
 * Function : HLW8110_Init_Inf Judge_CheckSum_HLW8110_Calfactor(void)
 * Describe : 验证地址0x70-0x77地址的系数和
 * Input    : none
 * Return   : 验证成功还是失败和失败信息
 * Record   : 2024/09/11
==========================================================================================================*/
HLW8110_Init_Inf Judge_CheckSum_HLW8110_Calfactor(void)
{
	uint32_t a = 0;
	HLW8110_Init_Inf err = HLW8110_OK;
 
  //读取RmsIAC、RmsIBC、RmsUC、PowerPAC、PowerPBC、PowerSC、EnergAc、EnergBc的值
	if(HLW8110_Read_Reg(REG_RMS_IAC_ADDR,2,50) == OK){//读取 A 通道电流转换系数
		U16_RMSIAC_RegData = (HLW8110_Usart_Struct.rx_data[0]<<8) + HLW8110_Usart_Struct.rx_data[1];
	}
	else{
		return IA_Err;
	}
	
	if(HLW8110_Read_Reg(REG_RMS_IBC_ADDR,2,50) == OK){//读取 B 通道电流转换系数
		U16_RMSIBC_RegData = (HLW8110_Usart_Struct.rx_data[0]<<8) + HLW8110_Usart_Struct.rx_data[1];
	}
	else{
		return IB_Err;
	}
	
	if(HLW8110_Read_Reg(REG_RMS_UC_ADDR,2,50) == OK){//读取电压转换系数
		U16_RMSUC_RegData = (HLW8110_Usart_Struct.rx_data[0]<<8) + HLW8110_Usart_Struct.rx_data[1];
	}
	else{
		return U_Err;
	}

	if(HLW8110_Read_Reg(REG_POWER_PAC_ADDR,2,50) == OK){//读取 A 通道有功功率转换系数
		U16_PowerPAC_RegData = (HLW8110_Usart_Struct.rx_data[0]<<8) + HLW8110_Usart_Struct.rx_data[1];
	}
	else{
		return PA_Err;
	}
	
	if(HLW8110_Read_Reg(REG_POWER_PBC_ADDR,2,50) == OK){//读取 B 通道有功功率转换系数
		U16_PowerPBC_RegData = (HLW8110_Usart_Struct.rx_data[0]<<8) + HLW8110_Usart_Struct.rx_data[1];
	}
	else{
		return PB_Err;
	}
	
	if(HLW8110_Read_Reg(REG_POWER_SC_ADDR,2,50) == OK){//读取视在功率转换系数
		U16_PowerSC_RegData = (HLW8110_Usart_Struct.rx_data[0]<<8) + HLW8110_Usart_Struct.rx_data[1];
	}
	else{
		return PS_Err;
	}
	
	if(HLW8110_Read_Reg(REG_ENERGY_AC_ADDR,2,50) == OK){//读取 A 通道能量转换系数
		U16_EnergyAC_RegData = (HLW8110_Usart_Struct.rx_data[0]<<8) + HLW8110_Usart_Struct.rx_data[1];
	}
	else{
		return EA_Err;
	}
	
	if(HLW8110_Read_Reg(REG_ENERGY_BC_ADDR,2,50) == OK){//读取 B 通道能量转换系数
		U16_EnergyBC_RegData = (HLW8110_Usart_Struct.rx_data[0]<<8) + HLW8110_Usart_Struct.rx_data[1];
	}
	else{
		return EB_Err;
	}
	
	if(HLW8110_Read_Reg(REG_CHECKSUM_ADDR,2,50) == OK){//读取系统校验和
		U16_CheckSUM_RegData = (HLW8110_Usart_Struct.rx_data[0]<<8) + HLW8110_Usart_Struct.rx_data[1];
	}
	else{
		return CS_Err;
	}
	
  a = ~(0xffff + U16_RMSIAC_RegData + U16_RMSIBC_RegData + U16_RMSUC_RegData + 
        U16_PowerPAC_RegData + U16_PowerPBC_RegData + U16_PowerSC_RegData + 
          U16_EnergyAC_RegData + U16_EnergyBC_RegData);
	
  U16_CheckSUM_Data = a & 0xffff;
	
	if ( U16_CheckSUM_Data == U16_CheckSUM_RegData){
		err = HLW8110_OK;
	}
	else{
		err = SUM_Err;
	}
	return err;
}



/*=========================================================================================================
 * Function :HLW8110_Init_Inf  HLW8110_Write_Reg(uint8_t add_reg,uint16_t data)
 * Describe : 写寄存器命令
 * Input    : add_reg:寄存器地址   data:写入的数据
 * Return   : 写入是否完成
 * Record   : 2024/09/11
==========================================================================================================*/
HLW8110_Init_Inf HLW8110_Write_Reg(uint8_t add_reg,uint16_t data)
{
	HLW8110_Init_Inf err = HLW8110_OK;
	uint8_t tx_data[5];
	tx_data[0] = 0xA5;
	tx_data[1] = add_reg | 0x80;
	tx_data[2] = (uint8_t)(data >> 8);
	tx_data[3] = (uint8_t)data;
	tx_data[4] = HLW8110_checkSum_Write(tx_data,4);
	HLW8110_Send_Data(tx_data,5);
	HAL_Delay(10);
	if(HLW8110_Read_Reg(add_reg,2,50) == OK){
		uint16_t temp = (HLW8110_Usart_Struct.rx_data[0]<<8) + HLW8110_Usart_Struct.rx_data[1];
		if(temp != data){
			err = UNKNOWN_Err;
		}
	}
	return err;
}



/*=========================================================================================================
 * Function : uint8_t HLW8110_Read_Reg(uint8_t add_reg,uint8_t len,uint32_t timOut)
 * Describe : 读取寄存器数据
 * Input    : add_reg:寄存器地址   len:寄存器数据长度   timeOunt:超时时间
 * Return   : 读取是否成成功 OK:成功  Err:失败
 * Record   : 2024/09/11
==========================================================================================================*/
uint8_t HLW8110_Read_Reg(uint8_t add_reg,uint8_t len,uint32_t timOut)
{
	uint8_t tx_data[2];
	tx_data[0] = 0XA5;
	tx_data[1] = add_reg;
	Clear_Rx_Data();
	HLW8110_Usart_Struct.rx_len = len + 1;
	HLW8110_Send_Data(tx_data,2);
	
	while(timOut--){
		if(HLW8110_Usart_Struct.rx_sta == OK)
			if(HLW8110_Usart_Struct.rx_data[HLW8110_Usart_Struct.rx_len - 1] == HLW8110_checkSum_Read(add_reg))
			return OK;
		HAL_Delay(1);
	}
	return Err;
}



/*=========================================================================================================
 * Function : void HLW8110_Reset(void)
 * Describe : 芯片复位指令函数
 * Input    : none
 * Return   : none
 * Record   : 2024/09/12
==========================================================================================================*/
void HLW8110_Reset(void)
{
	uint8_t tx_data[4];
	tx_data[0] = 0xa5;
	tx_data[1] = 0xea;
	tx_data[2] = 0x96;
	tx_data[3] = HLW8110_checkSum_Write(tx_data,3);
	HLW8110_Send_Data(tx_data,4);
}



/*=========================================================================================================
 * Function : float HLW8110_Read_U(void)
 * Describe : 读取电压值
 * Input    : none
 * Return   : 电压值
 * Record   : 2024/09/11
==========================================================================================================*/
float HLW8110_Read_U(void)
{
	if(HLW8110_Read_Reg(REG_RMSU_ADDR,3,50) == OK){
		uint32_t U_Arr;
		float F_AC_V;
		U_Arr = (unsigned long)(HLW8110_Usart_Struct.rx_data[0]<<16) + 
						(unsigned long)(HLW8110_Usart_Struct.rx_data[1]<<8) + 
						(unsigned long)(HLW8110_Usart_Struct.rx_data[2]);
		Clear_Rx_Data();
		if ((U_Arr & 0x800000) == 0x800000){
		 	F_AC_V = 0;
	  }
		else{
			F_AC_V =  (float)U_Arr;
			F_AC_V = F_AC_V * U16_RMSUC_RegData;  
			F_AC_V = F_AC_V / 0x400000;       
			F_AC_V = F_AC_V / 1;  						// 1 = 电压系数
			F_AC_V = F_AC_V / 100;    				//计算出a = 22083.12mV,a/100表示220.8312V，电压转换成V
			F_AC_V = F_AC_V * D_CAL_U;				//D_CAL_U是校正系数，默认是1,		
		}
		return F_AC_V;
	}
	else{
		return Err;
	}
}


/*=========================================================================================================
 * Function : float HLW8110_Read_IA(void)
 * Describe : 读取A通道电流值
 * Input    : none
 * Return   : 电流值
 * Record   : 2024/09/11
==========================================================================================================*/
float HLW8110_Read_IA(void)
{
	if(HLW8110_Read_Reg(REG_RMSIA_ADDR,3,50) == OK){
		uint32_t IA_Arr;
		float F_AC_IA;
		IA_Arr = (unsigned long)(HLW8110_Usart_Struct.rx_data[0]<<16) + 
						 (unsigned long)(HLW8110_Usart_Struct.rx_data[1]<<8) + 
						 (unsigned long)(HLW8110_Usart_Struct.rx_data[2]);
		Clear_Rx_Data();
						 
		if ((IA_Arr & 0x800000) == 0x800000){
		 	F_AC_IA = 0;
	  }
	  else
	  {
	  	F_AC_IA = (float)IA_Arr;
	  	F_AC_IA = F_AC_IA * U16_RMSIAC_RegData;
		  F_AC_IA  = F_AC_IA / 0x800000;                     //电流计算出来的浮点数单位是mA,比如5003.12 
	  	F_AC_IA = F_AC_IA / 1;  								// 1 = 电流系数
		  F_AC_IA = F_AC_IA / 1000;              //a= 5003ma,a/1000 = 5.003A,单位转换成A
		  F_AC_IA = F_AC_IA * D_CAL_A_I;				//D_CAL_A_I是校正系数，默认是1
	  }
		return F_AC_IA;
	}
	else{
		return Err;
	}
}




/*=========================================================================================================
 * Function : float HLW8110_Read_PA(void)
 * Describe : 读取A通道功率值
 * Input    : none
 * Return   : 功率值
 * Record   : 2024/09/15
==========================================================================================================*/
float HLW8110_Read_PA(void)
{
	if(HLW8110_Read_Reg(REG_POWER_PA_ADDR,4,50) == OK){
		uint32_t PA_Arr;
		float F_AC_P,b;
		PA_Arr = (unsigned long)(HLW8110_Usart_Struct.rx_data[0]<<24) + 
						 (unsigned long)(HLW8110_Usart_Struct.rx_data[1]<<16) + 
						 (unsigned long)(HLW8110_Usart_Struct.rx_data[2] << 8) + 
						 (unsigned long)(HLW8110_Usart_Struct.rx_data[3]);
		
		Clear_Rx_Data();
		if (PA_Arr > 0x80000000)
		{
			b = ~PA_Arr;
			F_AC_P = (float)b;
		}
		else
		 F_AC_P =  (float)PA_Arr;

		F_AC_P = F_AC_P * U16_PowerPAC_RegData;
		F_AC_P = F_AC_P / 0x80000000;             
		F_AC_P = F_AC_P / 1;  										// 1 = 电流系数
		F_AC_P = F_AC_P / 1;  										// 1 = 电压系数
		F_AC_P = F_AC_P * D_CAL_A_P;						//D_CAL_A_P是校正系数，默认是1
		return F_AC_P;//单位为W,比如算出来5000.123，表示5000.123W
	}
	
	else{
		return Err;
	}
}




/*=========================================================================================================
 * Function : float HLW8110_Read_PF(void)
 * Describe : 读取功率因数
 * Input    : none
 * Return   : 功率因数
 * Record   : 2024/09/15
==========================================================================================================*/
float HLW8110_Read_PF(void)
{
	if(HLW8110_Read_Reg(REG_PF_ADDR,3,50) == OK){
		uint32_t PF_Arr;
		float F_AC_PF;
		PF_Arr = (unsigned long)(HLW8110_Usart_Struct.rx_data[0]<<16) + 
						 (unsigned long)(HLW8110_Usart_Struct.rx_data[1]<<8) + 
						 (unsigned long)(HLW8110_Usart_Struct.rx_data[2]);
		
		Clear_Rx_Data();
		
		if (PF_Arr>0x800000){//为负，容性负载
			F_AC_PF = (float)(0xffffff - PF_Arr + 1)/0x7fffff;
		}
		else{
			F_AC_PF = (float)PF_Arr/0x7fffff;
		}

		//功率因素*100，最大为100，最小负100
		return F_AC_PF;
	}
	else{
		return Err;
	}
}



/*=========================================================================================================
 * Function : float HLW8110_Read_PF(void)
 * Describe : 读取有功电能
 * Input    : none
 * Return   : 有功电能
 * Record   : 2024/09/17
==========================================================================================================*/
float HLW8110_Read_EA(void)
{
	if(HLW8110_Read_Reg(REG_ENERGY_PA_ADDR,3,50) == OK){
		uint32_t EA_Arr;
		float F_AC_EA;
		EA_Arr = (unsigned long)(HLW8110_Usart_Struct.rx_data[0]<<16) + 
						 (unsigned long)(HLW8110_Usart_Struct.rx_data[1]<<8) + 
						 (unsigned long)(HLW8110_Usart_Struct.rx_data[2]);
		
		Clear_Rx_Data();
		
		F_AC_EA = (float)EA_Arr;	
		F_AC_EA = F_AC_EA * U16_EnergyAC_RegData;
		F_AC_EA = F_AC_EA / 0x20000000;             //电量单位是0.001KWH,比如算出来是2.002,表示2.002KWH    
		F_AC_EA = F_AC_EA / 1;  										// 1 = 电流系数
		F_AC_EA = F_AC_EA / 1;  										// 1 = 电压系数
		F_AC_EA = F_AC_EA * D_CAL_A_E;     			//D_CAL_A_E是校正系数，默认是1
		return F_AC_EA;
	}
	else{
		return Err;
	}
}


/*=========================================================================================================
 * Function : void HLW8110_Con(void)
 * Describe : HLW8110数据读取控制放入与HLW8110相连的串口中断
 * Input    : none
 * Return   : none
 * Record   : 2024/09/11
==========================================================================================================*/
void HLW8110_Con(void)
{
	if(HLW8110_Usart_Struct.rx_index < HLW8110_Usart_Struct.rx_len){
		HLW8110_Usart_Struct.rx_data[HLW8110_Usart_Struct.rx_index++] = HLW8110_Usart_Struct.rx_byte;
		if(HLW8110_Usart_Struct.rx_index == HLW8110_Usart_Struct.rx_len)
			HLW8110_Usart_Struct.rx_sta = OK;
		else
			HLW8110_Usart_Struct.rx_sta = REV;
	}
	else{
		HLW8110_Usart_Struct.rx_sta = Err;
		HLW8110_Usart_Struct.rx_index = 0;
	}
	HAL_UART_Receive_IT(&HLW8110_USART, &HLW8110_Usart_Struct.rx_byte, 1); 
}

