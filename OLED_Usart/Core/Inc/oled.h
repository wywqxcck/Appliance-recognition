#ifndef __OLED_H
#define __OLED_H	
 
 
#include "main.h"
#include "stdlib.h"
 
#define S_I2C 	0	//软件模拟 I2C 接口开关

#if !S_I2C

#include "i2c.h"

#endif
 
 
#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    						  

#define OLED_CMD  0	//写指令
#define OLED_DATA 1	//写数据
				 
#if S_I2C

//-----------------OLED IIC 软件模拟驱动接口----------------  					   
#define OLED_SCLK_Clr() HAL_GPIO_WritePin(SCL_GPIO_Port,SCL_Pin,GPIO_PIN_RESET)//SCL
#define OLED_SCLK_Set() HAL_GPIO_WritePin(SCL_GPIO_Port,SCL_Pin,GPIO_PIN_SET)
 
#define OLED_SDIN_Clr() HAL_GPIO_WritePin(SDA_GPIO_Port,SDA_Pin,GPIO_PIN_RESET)//SDA
#define OLED_SDIN_Set() HAL_GPIO_WritePin(SDA_GPIO_Port,SDA_Pin,GPIO_PIN_SET)

#endif
 
 
#define u8 unsigned char
#define u32 unsigned int
 
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Wait_Ack(void);
void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void Write_IIC_Byte(unsigned char IIC_Byte);
 
void OLED_WR_Byte(unsigned dat,unsigned cmd);  
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);

void OLED_ShowChar(u8 x,u8 y,u8 chr);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len);
void OLED_ShowFNum(u8 x,u8 y,float num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y,uint8_t *chr);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(u8 x,u8 y,uint8_t* data);
uint8_t Oled_Center(uint8_t* data);
void Oled_Send_Buff(void);
void OLED_DrawPoint(u8 x,u8 y);
void OLED_ClearPoint(u8 x,u8 y);
void oled_draw_bmp(uint8_t x, uint8_t y, uint8_t l, uint8_t w,uint8_t* BMP);
void To_Send_Buff(void);
				
#endif




