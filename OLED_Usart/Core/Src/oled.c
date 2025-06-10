#include "oled.h"
#include "stdlib.h"
#include "font.h"  	 
#include "APP.h"


//构建OLED帧缓冲区
uint8_t OLED_GRAM[8][128];
uint8_t OLD_OLED_GRAM[8][128];

//若#define S_I2C 则使用软件iic
//OLED 的显存
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 	
 
/**********************************************
//Software IIC Start
**********************************************/
#if S_I2C

void IIC_Start(void)
{
	OLED_SCLK_Set();
	OLED_SDIN_Set();
	OLED_SDIN_Clr();
	OLED_SCLK_Clr();
}
/**********************************************
//Software IIC Stop
**********************************************/
void IIC_Stop(void)
{
	OLED_SCLK_Set();
	OLED_SDIN_Clr();
	OLED_SDIN_Set();
}
/**********************************************
//Software IIC Ack
**********************************************/
void IIC_Wait_Ack(void)
{
	OLED_SCLK_Set() ;
	OLED_SCLK_Clr();
}
/**********************************************
// IIC Write byte
**********************************************/
void Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
	unsigned char m,da;
	da=IIC_Byte;
	OLED_SCLK_Clr();
	for(i=0;i<8;i++)		
	{
		m=da;
		m=m&0x80;
		if(m==0x80)
		{
			OLED_SDIN_Set();
		}
		else 
		OLED_SDIN_Clr();
		da=da<<1;
		OLED_SCLK_Set();
		OLED_SCLK_Clr();
	}
}

#endif
/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
	#if S_I2C
	IIC_Start();
	Write_IIC_Byte(0x78);            //Slave address,SA0=0
	IIC_Wait_Ack();	
	Write_IIC_Byte(0x00);			//write command
	IIC_Wait_Ack();	
	Write_IIC_Byte(IIC_Command); 
	IIC_Wait_Ack();	
	IIC_Stop();
	#else
	HAL_I2C_Mem_Write(&hi2c1,0x78,0x00,I2C_MEMADD_SIZE_8BIT,&IIC_Command,1,100);
	#endif
}
/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
	#if S_I2C
	IIC_Start();
	Write_IIC_Byte(0x78);			//Slave address,SA0=0
	IIC_Wait_Ack();	
	Write_IIC_Byte(0x40);			//write data
	IIC_Wait_Ack();	
	Write_IIC_Byte(IIC_Data);
	IIC_Wait_Ack();	
	IIC_Stop();
	#else
	HAL_I2C_Mem_Write(&hi2c1,0x78,0x40,I2C_MEMADD_SIZE_8BIT,&IIC_Data,1,100);
	#endif
}


/**********************************************
//写入函数
**********************************************/	 
void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
	if(cmd)
	{
		Write_IIC_Data(dat);
	}
	else 
	{
		Write_IIC_Command(dat);	
	}
}

 

/**********************************************
//Set Position
//坐标设置
**********************************************/
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
}   	  

/**********************************************
//Turn on OLED display  
//开启OLED显示    
**********************************************/
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}   
/**********************************************
//Turn off OLED display
//关闭OLED显示    
**********************************************/
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	
/**********************************************
//清屏函数,清完屏,整个屏幕是黑色的!  
**********************************************/	   			 
void OLED_Clear(void)  
{  		    
	for(uint8_t i = 0;i < 8;i++){
		for(uint16_t j = 0;j < 128;j++){
			OLED_GRAM[i][j] = 0;
		}
	}
}


/**
 * @brief 在OLED上绘制位图
 *
 * 根据给定的位图参数，在指定位置绘制相应的图像。
 *
 * @param x 位图左上角的X坐标。
 * @param y 位图左上角的Y坐标。
 * @param l 位图的长度。
 * @param w 位图的宽度。
 */
void oled_draw_bmp(uint8_t x, uint8_t y, uint8_t l, uint8_t w,uint8_t* BMP) {
    // 计算位图每行的字节数，flag用于标记是否有剩余的不足一个字节的位
    uint8_t flag = (l % 8 == 0) ? 0 : 1;
    // 计算位图总共有多少行
    uint8_t rows = l >> 3;
    // 循环遍历每一行
    for (uint8_t row = 0; row < rows + flag; row++) {
        // 循环遍历每一列
        for (uint8_t column = 0; column < w; column++) {
            // 获取位图中相应位置的字节数据
            uint8_t bmp_byte = BMP[row * w + column];
            // 循环遍历字节中的每个位
            for (uint8_t bit = 0; bit < 8; bit++) {
                // 检查当前位是否为1，如果是则在OLED上绘制点
                if (bmp_byte & (1 << bit)) {
                    OLED_DrawPoint(x + column, y + row * 8 + bit);//这里为你的画点函数
                }
				else{
					OLED_ClearPoint(x + column, y + row * 8 + bit);
				}
            }
        }
    }
}

/**********************************************
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//chr：要显示的字符		 
//size:选择字体 16/12  
**********************************************/	  
void OLED_ShowChar(u8 x,u8 y,u8 chr)
{      	
	unsigned char c=0,i=0;	
	c = chr - ' ';//得到偏移后的值，为什么做偏移可查看 ASCII 表		
	if(x>Max_Column-1){x=0;y=y+2;}
	
	for(i=0;i<8;i++)
		OLED_GRAM[y][x + i] = F8X16[c*16+i];
	y+=1;
	for(i=0;i<8;i++)
		OLED_GRAM[y][x + i] = F8X16[c*16+i+8];
}


/**********************************************
//m^n函数
**********************************************/	  
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}	
/**********************************************
//显示2个数字
//x:0~127
//y:0~63 
//num:数值(0~4294967295);
//len :数字的位数
//size:字体大小
**********************************************/	  			  	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(16/2)*t,y,' ');
				continue;
			}
			else 
				enshow=1; 	 
		}
	 	OLED_ShowChar(x+(16/2)*t,y,temp+'0'); 
	}
} 

//显示负数、浮点数字
//x,y :起点坐标
//num :要显示的数字
//len :数字的位数，包括小数点 len=4时显示x.xx
//size:字体大小
//mode:0,反色显示;1,正常显示
//@n，小数点后位数，默认为2

void OLED_ShowFNum(u8 x,u8 y,float num,u8 len,u8 size)
{         	
	u8 t,temp,i=0,m=0,n=2;
	u8 enshow=0,pointshow=0;
	uint16_t k;
	len--;
	if(size==8)m=2;
	if(num<0)
	{
	num=-num;
	i=1;     //负数标志	
	}	
	k=num*oled_pow(10,n); //此处为显示一位小数*10转化为整数
	for(t=0;t<len;t++)
	{
		temp=(k/oled_pow(10,len-t-1 ))%10;
		if(enshow==0&&t<(len-2))
		{
			if(temp==0)
			{
				if(((k/oled_pow(10,len-t-2)%10)!=0)&&(i==1))//判断是否为负数且在最高位前一位
				{
					OLED_ShowChar(x+(size/2+m)*t,y,'-');
									

					i=0;	                              //清除判断后一位的标志
				}else
		    		OLED_ShowChar(x+(size/2+m)*t,y,'0');//如果没到数字就显示0
									

				continue;
			}else enshow=1;		//此处是判断是否要显示数字	
		}
		if(t==len-n)//判断是否为最后一位的前一位（显示一位小数）
		{
			OLED_ShowChar(x+(size/2+m)*t,y,'.');
      		OLED_ShowChar(x+(size/2+m)*(t+1),y,temp+'0');
			pointshow=1;
			continue;
		}
		if(pointshow==1){	
			OLED_ShowChar(x+(size/2+m)*(t+1),y,temp+'0');
			
		}else	
	 		OLED_ShowChar(x+(size/2+m)*t,y,temp+'0');
		//一位一位显示下去
	 }
}
/**********************************************
//显示一个字符号串
//Char——Size影响的是两字符间间距
**********************************************/
void OLED_ShowString(u8 x,u8 y,uint8_t *chr)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		
		OLED_ShowChar(x,y,chr[j]);
		x+=16/2;
		if(x>120){x=0;y+=2;}
			j++;
	}
}


//选项标移动刷新节约时间
void To_Send_Buff(void)
{
	uint8_t x = ARROWS_X,y = 0;
	for(uint8_t i = 0;i < 8;i++){
		OLED_Set_Pos(x,y++);
		for(uint16_t j = ARROWS_X;j < ARROWS_X + 16;j++){
			OLED_WR_Byte(OLED_GRAM[i][j],OLED_DATA);
		}
	}
}

//OLED整屏刷新
void Oled_Send_Buff(void)
{
	uint8_t x = 0,y = 0;
	for(uint8_t i = 0;i < 8;i++){
		OLED_Set_Pos(x,y++);
		for(uint16_t j = 0;j < 128;j++){
			OLED_WR_Byte(OLED_GRAM[i][j],OLED_DATA);
		}
	}
}

//获取居中位置
uint8_t Oled_Center(uint8_t* data)
{
	uint8_t temp = 0;
	while(*data != '\0'){
		temp += 16;
		if(*data == ' ')
			data+=1;
		else
			data += 2;
	}
	temp = 128 - temp;
	temp /= 2;
	return temp;
}


//显示一个中文字符
void OLED_Show_One_Chinese(u8 x,u8 y,uint8_t* data)
{
	uint16_t num;
	num = sizeof(cfont16)/sizeof(typFNT_GB16);
	for(uint8_t i = 0;i < num;i++){
		if(*data == cfont16[i].Index[0] && *(data+1) == cfont16[i].Index[1]){
			for(uint8_t j = 0;j < 16;j++){
				OLED_GRAM[y][x + j] = cfont16[i].Msk[j];
			}
			y += 1;
			for(uint8_t j = 0;j < 16;j++){
				OLED_GRAM[y][x + j] = cfont16[i].Msk[j+16];
			}
		}
	}
}
/**********************************************
//显示汉字
**********************************************/
void OLED_ShowCHinese(u8 x,u8 y,uint8_t* data)
{      			    
	while(*data != '\0'){
		if(x > 112){
			y += 2;
			x = 0;
		}
		if(y > 6)
			return;
		if(*data == ' '){
			OLED_ShowString(x,y,(uint8_t*)"  ");
			x += 16;
			data += 1;
			continue;
		}
		OLED_Show_One_Chinese(x,y,data);
		x += 16;
		data += 2;
	}
}


//画点 x:0~127 y:0~63
void OLED_DrawPoint(u8 x,u8 y)
{
	u8 i,m,n;
	i=y/8;//算出为第几页
	m=y%8;//算出点亮第几个像素点
	n=1<<m;//将此像素点转换为8位的数据
	OLED_GRAM[i][x]|=n;//将此像素点加上原来的数据
}
 
//清除一个点 x:0~127  y:0~63
void OLED_ClearPoint(u8 x,u8 y)
{
	u8 i,m,n;
	i=y/8;//算出为第几页
	m=y%8;//算出点亮第几个像素点
	n=1<<m;//将此像素点转换为8位的数据
	OLED_GRAM[i][x]&=~n;
}

 
/**********************************************
//初始化SSD1306		
**********************************************/	    
void OLED_Init(void)
{ 	
	HAL_Delay(500);
	OLED_WR_Byte(0xAE,OLED_CMD);//关闭显示（进入睡眠模式）
	OLED_WR_Byte(0x02,OLED_CMD);//设置低列地址
	OLED_WR_Byte(0x10,OLED_CMD);//设置高列地址
	OLED_WR_Byte(0xD5,OLED_CMD);//设置显示时钟分频值/震荡频率（刷新率）
	OLED_WR_Byte(0x80,OLED_CMD);//设置分割比率，设置时钟为100帧/秒
	OLED_WR_Byte(0xA8,OLED_CMD);//设置复用率（0-63）
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty	
	OLED_WR_Byte(0xD3,OLED_CMD);//设置显示偏移一位映射RAM计数器(0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//不偏移
	OLED_WR_Byte(0x40,OLED_CMD);//设置起始地址  设置映射RAM显示起始行(0x00~0x3F)
	OLED_WR_Byte(0xA1,OLED_CMD);//设置左右方向显示  0xa1正常 0xa0左右反置
	OLED_WR_Byte(0xC8,OLED_CMD);//设置上下方向显示 0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xDA,OLED_CMD);//设置列引脚硬件配置
	OLED_WR_Byte(0x12,OLED_CMD);//12864->0x12;128*32->0x02;
	OLED_WR_Byte(0x81,OLED_CMD);//设置对比度控制寄存器
	OLED_WR_Byte(0xCF,OLED_CMD);//设置对比度控制为0xcf（0x00-0xff）
	OLED_WR_Byte(0xD9,OLED_CMD);//设置充电周期
	OLED_WR_Byte(0xF1,OLED_CMD);//设置预充电为15个时钟，放电为1个时钟
	OLED_WR_Byte(0xDB,OLED_CMD);//设置VCOMH反压值
	OLED_WR_Byte(0x30,OLED_CMD);//设置VCOM取消选择电平
	OLED_WR_Byte(0x20,OLED_CMD);//设置页面寻址模式(0x00/0x01/0x02)
	OLED_WR_Byte(0xA6,OLED_CMD);//设置正常显示
	OLED_WR_Byte(0x8D,OLED_CMD);//设置电荷泵
	OLED_WR_Byte(0x14,OLED_CMD);//设置电荷泵开启/(0x10)禁用
	OLED_WR_Byte(0xAF,OLED_CMD);//开启显示（进入工作模式）
	OLED_Clear();
	Oled_Send_Buff();
}


