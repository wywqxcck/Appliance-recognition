#include "HLW8110.h"


HLW8110_Usart HLW8110_Usart_Struct;


uint32_t	U16_RMSIAC_RegData; 			// Aͨ������ת��ϵ��
uint32_t	U16_RMSIBC_RegData; 			// Bͨ������ת��ϵ��
uint32_t	U16_RMSUC_RegData; 				// ��ѹͨ��ת��ϵ��
uint32_t	U16_PowerPAC_RegData; 		// Aͨ������ת��ϵ��
uint32_t	U16_PowerPBC_RegData; 		// Bͨ������ת��ϵ��
uint32_t	U16_PowerSC_RegData; 			// ���ڹ���ת��ϵ��,���ѡ��Aͨ��������Aͨ�����ڹ���ת��ϵ����A��Bͨ��ֻ�ܶ���ѡ��һ
uint32_t	U16_EnergyAC_RegData; 		// Aͨ���й�����(��)ת��ϵ�� 
uint32_t	U16_EnergyBC_RegData; 		// Aͨ���й�����(��)ת��ϵ��
uint32_t	U16_CheckSUM_RegData; 		// ת��ϵ����CheckSum
uint32_t	U16_CheckSUM_Data; 				// ת��ϵ�����������CheckSum



/*=========================================================================================================
 * Function : HLW8110_Init_Inf HLW8110_Init(void)
 * Describe : ��ʼ��HW110ģ��
 * Input    : none
 * Return   : ��ʼ��״̬�Լ�������Ϣ
 * Record   : 2024/09/11
==========================================================================================================*/
HLW8110_Init_Inf HLW8110_Init(void)
{
	HLW8110_Init_Inf err = HLW8110_OK;
	HAL_UART_Receive_IT(&HLW8110_USART, &HLW8110_Usart_Struct.rx_byte, 1);
	Clear_Rx_Data();
	
	HLW8110_WriteREG_EN();//��д����
	HAL_Delay(10);
	
	//����ͨ��A�������ָ����ǰ���ڼ������ڹ��ʡ�������������ǡ�˲ʱ�й����ʡ�˲ʱ���ڹ��ʺ��й����ʹ��ص��ź�ָʾ ��ͨ��Ϊͨ��A	
	HLW8110_Set_Channel_A();
	HAL_Delay(10);
	
	if(HLW8110_Write_Reg(REG_SYSCON_ADDR,0x0A04) != HLW8110_OK){//����Aͨ�����ر�Bͨ������ѹͨ��PGA = 1������ͨ��PGA = 16
		return SYS_Con_Err;
	}
	
	if(HLW8110_Write_Reg(REG_EMUCON_ADDR,0x0001) != HLW8110_OK){	//1��ʹ��PFA ����������й����ܼĴ����ۼӣ�
		return EMU_Con1_Err;
	}
		
	if(HLW8110_Write_Reg(REG_EMUCON2_ADDR,0x0065) != HLW8110_OK){	//0x0001��EMUCON2��Ĭ��ֵ��waveEn = 1,zxEn = 1��Aͨ�������Ĵ�����������0��EPA_CB = 1���򿪹������ؼ��
		return EMU_Con2_Err;
	}
		
	HLW8110_WriteREG_DIS();//�ر�д����
	
	err = Judge_CheckSum_HLW8110_Calfactor();//��֤��ַ0x70-0x77��ַ��ϵ����
#if PRINTF_DEBUG
		printf("Aͨ������ת��ϵ��:%x\r\n" ,U16_RMSIAC_RegData);
		HAL_Delay(1);
		printf("Bͨ������ת��ϵ��:%x\r\n" ,U16_RMSIBC_RegData);
		HAL_Delay(1);
		printf("��ѹͨ��ת��ϵ��:%x\r\n" ,U16_RMSUC_RegData);
		HAL_Delay(1);
		printf("Aͨ������ת��ϵ��:%x\r\n" ,U16_PowerPAC_RegData);
		HAL_Delay(1);
		printf("Bͨ������ת��ϵ��:%x\r\n" ,U16_PowerPBC_RegData);
		HAL_Delay(1);
		printf("���ڹ���ת��ϵ��:%x\r\n" ,U16_PowerSC_RegData);
		HAL_Delay(1);
		printf("Aͨ������ת��ϵ��:%x\r\n" ,U16_EnergyAC_RegData);
		HAL_Delay(1);
		printf("Bͨ������ת��ϵ��:%x\r\n" ,U16_EnergyBC_RegData);
		HAL_Delay(10);
		printf("ת��ϵ��:%x\r\n" ,U16_CheckSUM_RegData);
		HAL_Delay(10);
		printf("ת��ϵ���������:%x\r\n" ,U16_CheckSUM_Data);
		Clear_Rx_Data();
#endif
	return err;
}



/*=========================================================================================================
 * Function : void HLW8110_Send_Data(uint8_t* data,uint8_t len)
 * Describe : ���ڷ���
 * Input    : data:���͵�����ָ��   len:�������ݵĳ���
 * Return   : none
 * Record   : 2024/09/11
==========================================================================================================*/
void HLW8110_Send_Data(uint8_t* data,uint8_t len)
{
	HAL_UART_Transmit(&HLW8110_USART, data, (uint16_t)len, 1000);
}


/*=========================================================================================================
 * Function : void Clear_RxBuf(void)
 * Describe : ��׼�����մ�������ǰ����ս��ջ�����������
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
 * Describe : ��ȡHLW8110�Ĵ�����ֵ�����У���
 * Input    : �Ĵ�����ַ
 * Return   : У��ֵ
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
 * Describe : ����ʱ�����ݽ��м���У���
 * Input    : data:��ҪУ�������   len:���ݵĳ���
 * Return   : У��ֵ
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
 * Describe : �������ڹ��ʡ�������������ǡ�˲ʱ�й����ʡ�˲ʱ���ڹ��ʺ��й����ʹ��ص��ź�ָʾ ��ͨ��Ϊͨ��A	
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
 * Describe : ʹ��д����
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
 * Describe : ʧ��д����
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
 * Describe : ��֤��ַ0x70-0x77��ַ��ϵ����
 * Input    : none
 * Return   : ��֤�ɹ�����ʧ�ܺ�ʧ����Ϣ
 * Record   : 2024/09/11
==========================================================================================================*/
HLW8110_Init_Inf Judge_CheckSum_HLW8110_Calfactor(void)
{
	uint32_t a = 0;
	HLW8110_Init_Inf err = HLW8110_OK;
 
  //��ȡRmsIAC��RmsIBC��RmsUC��PowerPAC��PowerPBC��PowerSC��EnergAc��EnergBc��ֵ
	if(HLW8110_Read_Reg(REG_RMS_IAC_ADDR,2,50) == OK){//��ȡ A ͨ������ת��ϵ��
		U16_RMSIAC_RegData = (HLW8110_Usart_Struct.rx_data[0]<<8) + HLW8110_Usart_Struct.rx_data[1];
	}
	else{
		return IA_Err;
	}
	
	if(HLW8110_Read_Reg(REG_RMS_IBC_ADDR,2,50) == OK){//��ȡ B ͨ������ת��ϵ��
		U16_RMSIBC_RegData = (HLW8110_Usart_Struct.rx_data[0]<<8) + HLW8110_Usart_Struct.rx_data[1];
	}
	else{
		return IB_Err;
	}
	
	if(HLW8110_Read_Reg(REG_RMS_UC_ADDR,2,50) == OK){//��ȡ��ѹת��ϵ��
		U16_RMSUC_RegData = (HLW8110_Usart_Struct.rx_data[0]<<8) + HLW8110_Usart_Struct.rx_data[1];
	}
	else{
		return U_Err;
	}

	if(HLW8110_Read_Reg(REG_POWER_PAC_ADDR,2,50) == OK){//��ȡ A ͨ���й�����ת��ϵ��
		U16_PowerPAC_RegData = (HLW8110_Usart_Struct.rx_data[0]<<8) + HLW8110_Usart_Struct.rx_data[1];
	}
	else{
		return PA_Err;
	}
	
	if(HLW8110_Read_Reg(REG_POWER_PBC_ADDR,2,50) == OK){//��ȡ B ͨ���й�����ת��ϵ��
		U16_PowerPBC_RegData = (HLW8110_Usart_Struct.rx_data[0]<<8) + HLW8110_Usart_Struct.rx_data[1];
	}
	else{
		return PB_Err;
	}
	
	if(HLW8110_Read_Reg(REG_POWER_SC_ADDR,2,50) == OK){//��ȡ���ڹ���ת��ϵ��
		U16_PowerSC_RegData = (HLW8110_Usart_Struct.rx_data[0]<<8) + HLW8110_Usart_Struct.rx_data[1];
	}
	else{
		return PS_Err;
	}
	
	if(HLW8110_Read_Reg(REG_ENERGY_AC_ADDR,2,50) == OK){//��ȡ A ͨ������ת��ϵ��
		U16_EnergyAC_RegData = (HLW8110_Usart_Struct.rx_data[0]<<8) + HLW8110_Usart_Struct.rx_data[1];
	}
	else{
		return EA_Err;
	}
	
	if(HLW8110_Read_Reg(REG_ENERGY_BC_ADDR,2,50) == OK){//��ȡ B ͨ������ת��ϵ��
		U16_EnergyBC_RegData = (HLW8110_Usart_Struct.rx_data[0]<<8) + HLW8110_Usart_Struct.rx_data[1];
	}
	else{
		return EB_Err;
	}
	
	if(HLW8110_Read_Reg(REG_CHECKSUM_ADDR,2,50) == OK){//��ȡϵͳУ���
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
 * Describe : д�Ĵ�������
 * Input    : add_reg:�Ĵ�����ַ   data:д�������
 * Return   : д���Ƿ����
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
 * Describe : ��ȡ�Ĵ�������
 * Input    : add_reg:�Ĵ�����ַ   len:�Ĵ������ݳ���   timeOunt:��ʱʱ��
 * Return   : ��ȡ�Ƿ�ɳɹ� OK:�ɹ�  Err:ʧ��
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
 * Describe : оƬ��λָ���
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
 * Describe : ��ȡ��ѹֵ
 * Input    : none
 * Return   : ��ѹֵ
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
			F_AC_V = F_AC_V / 1;  						// 1 = ��ѹϵ��
			F_AC_V = F_AC_V / 100;    				//�����a = 22083.12mV,a/100��ʾ220.8312V����ѹת����V
			F_AC_V = F_AC_V * D_CAL_U;				//D_CAL_U��У��ϵ����Ĭ����1,		
		}
		return F_AC_V;
	}
	else{
		return Err;
	}
}


/*=========================================================================================================
 * Function : float HLW8110_Read_IA(void)
 * Describe : ��ȡAͨ������ֵ
 * Input    : none
 * Return   : ����ֵ
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
		  F_AC_IA  = F_AC_IA / 0x800000;                     //������������ĸ�������λ��mA,����5003.12 
	  	F_AC_IA = F_AC_IA / 1;  								// 1 = ����ϵ��
		  F_AC_IA = F_AC_IA / 1000;              //a= 5003ma,a/1000 = 5.003A,��λת����A
		  F_AC_IA = F_AC_IA * D_CAL_A_I;				//D_CAL_A_I��У��ϵ����Ĭ����1
	  }
		return F_AC_IA;
	}
	else{
		return Err;
	}
}




/*=========================================================================================================
 * Function : float HLW8110_Read_PA(void)
 * Describe : ��ȡAͨ������ֵ
 * Input    : none
 * Return   : ����ֵ
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
		F_AC_P = F_AC_P / 1;  										// 1 = ����ϵ��
		F_AC_P = F_AC_P / 1;  										// 1 = ��ѹϵ��
		F_AC_P = F_AC_P * D_CAL_A_P;						//D_CAL_A_P��У��ϵ����Ĭ����1
		return F_AC_P;//��λΪW,���������5000.123����ʾ5000.123W
	}
	
	else{
		return Err;
	}
}




/*=========================================================================================================
 * Function : float HLW8110_Read_PF(void)
 * Describe : ��ȡ��������
 * Input    : none
 * Return   : ��������
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
		
		if (PF_Arr>0x800000){//Ϊ�������Ը���
			F_AC_PF = (float)(0xffffff - PF_Arr + 1)/0x7fffff;
		}
		else{
			F_AC_PF = (float)PF_Arr/0x7fffff;
		}

		//��������*100�����Ϊ100����С��100
		return F_AC_PF;
	}
	else{
		return Err;
	}
}



/*=========================================================================================================
 * Function : float HLW8110_Read_PF(void)
 * Describe : ��ȡ�й�����
 * Input    : none
 * Return   : �й�����
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
		F_AC_EA = F_AC_EA / 0x20000000;             //������λ��0.001KWH,�����������2.002,��ʾ2.002KWH    
		F_AC_EA = F_AC_EA / 1;  										// 1 = ����ϵ��
		F_AC_EA = F_AC_EA / 1;  										// 1 = ��ѹϵ��
		F_AC_EA = F_AC_EA * D_CAL_A_E;     			//D_CAL_A_E��У��ϵ����Ĭ����1
		return F_AC_EA;
	}
	else{
		return Err;
	}
}


/*=========================================================================================================
 * Function : void HLW8110_Con(void)
 * Describe : HLW8110���ݶ�ȡ���Ʒ�����HLW8110�����Ĵ����ж�
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

