#include "oled.h"
#include "stdlib.h"
#include "font.h"  	 
#include "APP.h"


//����OLED֡������
uint8_t OLED_GRAM[8][128];
uint8_t OLD_OLED_GRAM[8][128];

//��#define S_I2C ��ʹ�����iic
//OLED ���Դ�
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
//д�뺯��
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
//��������
**********************************************/
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
}   	  

/**********************************************
//Turn on OLED display  
//����OLED��ʾ    
**********************************************/
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}   
/**********************************************
//Turn off OLED display
//�ر�OLED��ʾ    
**********************************************/
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	
/**********************************************
//��������,������,������Ļ�Ǻ�ɫ��!  
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
 * @brief ��OLED�ϻ���λͼ
 *
 * ���ݸ�����λͼ��������ָ��λ�û�����Ӧ��ͼ��
 *
 * @param x λͼ���Ͻǵ�X���ꡣ
 * @param y λͼ���Ͻǵ�Y���ꡣ
 * @param l λͼ�ĳ��ȡ�
 * @param w λͼ�Ŀ�ȡ�
 */
void oled_draw_bmp(uint8_t x, uint8_t y, uint8_t l, uint8_t w,uint8_t* BMP) {
    // ����λͼÿ�е��ֽ�����flag���ڱ���Ƿ���ʣ��Ĳ���һ���ֽڵ�λ
    uint8_t flag = (l % 8 == 0) ? 0 : 1;
    // ����λͼ�ܹ��ж�����
    uint8_t rows = l >> 3;
    // ѭ������ÿһ��
    for (uint8_t row = 0; row < rows + flag; row++) {
        // ѭ������ÿһ��
        for (uint8_t column = 0; column < w; column++) {
            // ��ȡλͼ����Ӧλ�õ��ֽ�����
            uint8_t bmp_byte = BMP[row * w + column];
            // ѭ�������ֽ��е�ÿ��λ
            for (uint8_t bit = 0; bit < 8; bit++) {
                // ��鵱ǰλ�Ƿ�Ϊ1�����������OLED�ϻ��Ƶ�
                if (bmp_byte & (1 << bit)) {
                    OLED_DrawPoint(x + column, y + row * 8 + bit);//����Ϊ��Ļ��㺯��
                }
				else{
					OLED_ClearPoint(x + column, y + row * 8 + bit);
				}
            }
        }
    }
}

/**********************************************
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//chr��Ҫ��ʾ���ַ�		 
//size:ѡ������ 16/12  
**********************************************/	  
void OLED_ShowChar(u8 x,u8 y,u8 chr)
{      	
	unsigned char c=0,i=0;	
	c = chr - ' ';//�õ�ƫ�ƺ��ֵ��Ϊʲô��ƫ�ƿɲ鿴 ASCII ��		
	if(x>Max_Column-1){x=0;y=y+2;}
	
	for(i=0;i<8;i++)
		OLED_GRAM[y][x + i] = F8X16[c*16+i];
	y+=1;
	for(i=0;i<8;i++)
		OLED_GRAM[y][x + i] = F8X16[c*16+i+8];
}


/**********************************************
//m^n����
**********************************************/	  
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}	
/**********************************************
//��ʾ2������
//x:0~127
//y:0~63 
//num:��ֵ(0~4294967295);
//len :���ֵ�λ��
//size:�����С
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

//��ʾ��������������
//x,y :�������
//num :Ҫ��ʾ������
//len :���ֵ�λ��������С���� len=4ʱ��ʾx.xx
//size:�����С
//mode:0,��ɫ��ʾ;1,������ʾ
//@n��С�����λ����Ĭ��Ϊ2

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
	i=1;     //������־	
	}	
	k=num*oled_pow(10,n); //�˴�Ϊ��ʾһλС��*10ת��Ϊ����
	for(t=0;t<len;t++)
	{
		temp=(k/oled_pow(10,len-t-1 ))%10;
		if(enshow==0&&t<(len-2))
		{
			if(temp==0)
			{
				if(((k/oled_pow(10,len-t-2)%10)!=0)&&(i==1))//�ж��Ƿ�Ϊ�����������λǰһλ
				{
					OLED_ShowChar(x+(size/2+m)*t,y,'-');
									

					i=0;	                              //����жϺ�һλ�ı�־
				}else
		    		OLED_ShowChar(x+(size/2+m)*t,y,'0');//���û�����־���ʾ0
									

				continue;
			}else enshow=1;		//�˴����ж��Ƿ�Ҫ��ʾ����	
		}
		if(t==len-n)//�ж��Ƿ�Ϊ���һλ��ǰһλ����ʾһλС����
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
		//һλһλ��ʾ��ȥ
	 }
}
/**********************************************
//��ʾһ���ַ��Ŵ�
//Char����SizeӰ��������ַ�����
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


//ѡ����ƶ�ˢ�½�Լʱ��
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

//OLED����ˢ��
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

//��ȡ����λ��
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


//��ʾһ�������ַ�
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
//��ʾ����
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


//���� x:0~127 y:0~63
void OLED_DrawPoint(u8 x,u8 y)
{
	u8 i,m,n;
	i=y/8;//���Ϊ�ڼ�ҳ
	m=y%8;//��������ڼ������ص�
	n=1<<m;//�������ص�ת��Ϊ8λ������
	OLED_GRAM[i][x]|=n;//�������ص����ԭ��������
}
 
//���һ���� x:0~127  y:0~63
void OLED_ClearPoint(u8 x,u8 y)
{
	u8 i,m,n;
	i=y/8;//���Ϊ�ڼ�ҳ
	m=y%8;//��������ڼ������ص�
	n=1<<m;//�������ص�ת��Ϊ8λ������
	OLED_GRAM[i][x]&=~n;
}

 
/**********************************************
//��ʼ��SSD1306		
**********************************************/	    
void OLED_Init(void)
{ 	
	HAL_Delay(500);
	OLED_WR_Byte(0xAE,OLED_CMD);//�ر���ʾ������˯��ģʽ��
	OLED_WR_Byte(0x02,OLED_CMD);//���õ��е�ַ
	OLED_WR_Byte(0x10,OLED_CMD);//���ø��е�ַ
	OLED_WR_Byte(0xD5,OLED_CMD);//������ʾʱ�ӷ�Ƶֵ/��Ƶ�ʣ�ˢ���ʣ�
	OLED_WR_Byte(0x80,OLED_CMD);//���÷ָ���ʣ�����ʱ��Ϊ100֡/��
	OLED_WR_Byte(0xA8,OLED_CMD);//���ø����ʣ�0-63��
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty	
	OLED_WR_Byte(0xD3,OLED_CMD);//������ʾƫ��һλӳ��RAM������(0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//��ƫ��
	OLED_WR_Byte(0x40,OLED_CMD);//������ʼ��ַ  ����ӳ��RAM��ʾ��ʼ��(0x00~0x3F)
	OLED_WR_Byte(0xA1,OLED_CMD);//�������ҷ�����ʾ  0xa1���� 0xa0���ҷ���
	OLED_WR_Byte(0xC8,OLED_CMD);//�������·�����ʾ 0xc0���·��� 0xc8����
	OLED_WR_Byte(0xDA,OLED_CMD);//����������Ӳ������
	OLED_WR_Byte(0x12,OLED_CMD);//12864->0x12;128*32->0x02;
	OLED_WR_Byte(0x81,OLED_CMD);//���öԱȶȿ��ƼĴ���
	OLED_WR_Byte(0xCF,OLED_CMD);//���öԱȶȿ���Ϊ0xcf��0x00-0xff��
	OLED_WR_Byte(0xD9,OLED_CMD);//���ó������
	OLED_WR_Byte(0xF1,OLED_CMD);//����Ԥ���Ϊ15��ʱ�ӣ��ŵ�Ϊ1��ʱ��
	OLED_WR_Byte(0xDB,OLED_CMD);//����VCOMH��ѹֵ
	OLED_WR_Byte(0x30,OLED_CMD);//����VCOMȡ��ѡ���ƽ
	OLED_WR_Byte(0x20,OLED_CMD);//����ҳ��Ѱַģʽ(0x00/0x01/0x02)
	OLED_WR_Byte(0xA6,OLED_CMD);//����������ʾ
	OLED_WR_Byte(0x8D,OLED_CMD);//���õ�ɱ�
	OLED_WR_Byte(0x14,OLED_CMD);//���õ�ɱÿ���/(0x10)����
	OLED_WR_Byte(0xAF,OLED_CMD);//������ʾ�����빤��ģʽ��
	OLED_Clear();
	Oled_Send_Buff();
}


