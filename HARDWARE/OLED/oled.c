#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "delay.h"
#include "string.h"
#include "stdio.h"


//OLED���Դ�
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
u8 OLED_GRAM[128][8];	 

//�����Դ浽LCD		 
void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
	}   
}
#if OLED_MODE==1	//8080����
//ͨ��ƴ�յķ�����OLED���һ��8λ����
//data:Ҫ���������
void OLED_Data_Out(u8 data)
{
	u16 dat=data&0X0F;
	GPIOC->ODR&=~(0XF<<6);		//���6~9
	GPIOC->ODR|=dat<<6;			//D[3:0]-->PC[9:6]
	GPIO_Write(GPIOC,dat<<6);
	PCout(11)=(data>>4)&0X01;	//D4
	PBout(6)=(data>>5)&0X01;	//D5
	PEout(5)=(data>>6)&0X01;	//D6
	PEout(6)=(data>>7)&0X01;	//D7 
} 
//��SSD1306д��һ���ֽڡ�
//dat:Ҫд�������/����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
void OLED_WR_Byte(u8 dat,u8 cmd)
{
	OLED_Data_Out(dat);	    
 	OLED_RS=cmd;
	OLED_CS=0;	   
	OLED_WR=0;	 
	OLED_WR=1;
	OLED_CS=1;	  
	OLED_RS=1;	 
} 	    	    
#else
//��SSD1306д��һ���ֽڡ�
//dat:Ҫд�������/����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
	u8 i;			  
	OLED_RS=cmd; //д���� 
	OLED_CS=0;		  
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK=0;
		if(dat&0x80)OLED_SDIN=1;
		else OLED_SDIN=0;
		OLED_SCLK=1;
		dat<<=1;   
	}				 
	OLED_CS=1;		  
	OLED_RS=1;   	  
} 
#endif
	  	  
//����OLED��ʾ    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//������ʾ
}
//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���				   
 void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}
//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,���;1,���	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//������ʾ
}
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 12/16/24
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ		 
    for(t=0;t<csize;t++)
    {   
		if(size==12)temp=asc2_1206o[chr][t]; 	 	//����1206����
		else if(size==16)temp=asc2_1608o[chr][t];	//����1608����
		else if(size==24)temp=asc2_2412o[chr][t];	//����2412����
		else return;								//û�е��ֿ�
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}
//m^n����
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 
//��ʾ�ַ���
//x,y:�������  
//size:�����С 
//*p:�ַ�����ʼ��ַ 
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)
{	
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    }  
	
}	
//��ʼ��SSD1306					    
void OLED_Init(void)
{ 	 		 
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��PORTA~E,PORTGʱ��
  
#if OLED_MODE==1		//ʹ��8080����ģʽ		
	
	//GPIO��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7 ;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_11;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��	
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	
	GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_5;	
	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	
	GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��	
 
	
 	OLED_WR=1;
	OLED_RD=1; 
#else					//ʹ��4��SPI ����ģʽ

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��		

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	
	GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��		
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	
	GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��
	
	OLED_SDIN=1;
	OLED_SCLK=1;
#endif
	OLED_CS=1;
	OLED_RS=1;	 
	
	OLED_RST=0;
	delay_ms(100);
	OLED_RST=1; 
					  
	OLED_WR_Byte(0xAE,OLED_CMD); //�ر���ʾ
	OLED_WR_Byte(0xD5,OLED_CMD); //����ʱ�ӷ�Ƶ����,��Ƶ��
	OLED_WR_Byte(80,OLED_CMD);   //[3:0],��Ƶ����;[7:4],��Ƶ��
	OLED_WR_Byte(0xA8,OLED_CMD); //��������·��
	OLED_WR_Byte(0X3F,OLED_CMD); //Ĭ��0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD); //������ʾƫ��
	OLED_WR_Byte(0X00,OLED_CMD); //Ĭ��Ϊ0

	OLED_WR_Byte(0x40,OLED_CMD); //������ʾ��ʼ�� [5:0],����.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //��ɱ�����
	OLED_WR_Byte(0x14,OLED_CMD); //bit2������/�ر�
	OLED_WR_Byte(0x20,OLED_CMD); //�����ڴ��ַģʽ
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	OLED_WR_Byte(0xA1,OLED_CMD); //���ض�������,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0,OLED_CMD); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	OLED_WR_Byte(0xDA,OLED_CMD); //����COMӲ����������
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]����
		 
	OLED_WR_Byte(0x81,OLED_CMD); //�Աȶ�����
	OLED_WR_Byte(0xEF,OLED_CMD); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
	OLED_WR_Byte(0xD9,OLED_CMD); //����Ԥ�������
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //����VCOMH ��ѹ����
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	OLED_WR_Byte(0xA6,OLED_CMD); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //������ʾ	 
	OLED_Clear();
}  

/*
	@brief			��������д�����ʼ�С���
	@param			x�� �е���ʼ�͵�ַ����ʼ�ߵ�ַ��0x00~0x0f��������ʼ�е͵�ַ����ҳѰַģʽ����
						0x10~0x1f��������ʼ�иߵ�ַ����ҳѰַģʽ��
					y����ʼҳ��ַ 0~7
	@retval			��
 */
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);//д��ҳ��ַ
	OLED_WR_Byte((x&0x0f),OLED_CMD);  //д���еĵ�ַ  �Ͱ��ֽ�
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);//д���еĵ�ַ �߰��ֽ�
}   	     	  

/*
	@brief			��ʾ����
	@param			x����ʼ�У�һ������ռ16��
					y����ʼҳ��һ������ռ��ҳ
					no����������
					m:���������С
					Chn[][m]:��������
	@retval			��
 */

void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char no,unsigned char Chn[][16])
{      			    
	unsigned char t,adder=0; //�������

	OLED_Set_Pos(x,y+1);	//�� x y ��ʼ���㣬�Ȼ���һҳ
    for(t=0;t<16;t++) //ѭ��16�Σ�����һҳ��16��
		{
			OLED_WR_Byte(Chn[2*no][t],1);//��no������λ�õĵ�һҳ16�еĵ�
			adder+=1; //�����ַ��1
     	}	
		OLED_Set_Pos(x,y); //���ڶ�ҳ
    for(t=0;t<16;t++)//ѭ��16�Σ����ڶ�ҳ��16��
		{	
			OLED_WR_Byte(Chn[2*no+1][t],1);//��no������λ�õĵڶ�ҳ16�еĵ�
			adder+=1;//�����ַ��1
        }					
}

/*
	@brief			��ʾͼƬ
	@param			x0����ʼ�е�ַ
					y0����ʼҳ��ַ
					x1����ֹ�е�ַ
					y1����ֹҳ��ַ
					BMP[]�����ͼƬ���������
	@retval			��
 */
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
 	unsigned int j=0; //�������
 	unsigned char x,y; //�������
  
 	if(y1%8==0) y=y1/8;   //�ж���ֹҳ�Ƿ�Ϊ8��������
 	 else y=y1/8+1;

		for(y=y0;y<y1;y++) //����ʼҳ��ʼ��������ֹҳ
		{
			OLED_Set_Pos(x0,y); //��ҳ����ʼ�п�ʼ��
   			for(x=x0;x<x1;x++) //��x1 - x0 ��
	    		{
	    			OLED_WR_Byte(BMP[j++],OLED_DATA);	//��ͼƬ�ĵ�    	
	    		}
		}
}  

//x:0~127
//y:0~63
//index:????(???C?????????)
//mode:0,????;1,????   4

extern unsigned char gImage_logo[1][1024];

void OLED_ShowPicture(u8 x,u8 y,u8 index,u8 mode)
{                  
    u16 temp,t,t1;
    u8 y0=y;                  
    for(t=0;t<1024;t++)                                //????
    {   
        temp=gImage_logo[index][t]; //??C???????????temp, ????????OLED????                                
        for(t1=0;t1<8;t1++)                        //?????????????
        {
            if(temp&0x80)OLED_DrawPoint(x,y,mode);
            else OLED_DrawPoint(x,y,!mode);
            temp<<=1;
            y++;
            if((y-y0)==64)                        //?y?????,?y??????.x???1
            {
                y=y0;
                x++;
                break;
            }
        }   
    }         
}

/*
	@brief			��ʾ��ͼ
	@param			x0����ʼ�е�ַ
							y0����ʼҳ��ַ
							x1����ֹ�е�ַ
							y1����ֹҳ��ַ
							k: ֡����
							m: ��֡�����С
							BMP[][m]����Ŷ�ͼ���������
	@retval			��
 */
void OLED_DrawGIF(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1, unsigned char k, int m, unsigned char GIF[][512])
{
	unsigned int j=0; //�������
 	unsigned char x,y,i; //�������
  
 	if(y1%8==0) y=y1/8;   //�ж���ֹҳ�Ƿ�Ϊ8��������
 	 else y=y1/8+1;
	for (i=0;i<k;i++) //�ӵ�һ֡��ʼ��
	{
		j = 0;
		for(y=y0;y<y1;y++) //����ʼҳ��ʼ��������ֹҳ
		{
			OLED_Set_Pos(x0,y); //��ҳ����ʼ�п�ʼ��
   			
			for(x=x0;x<x1;x++) //��x1 - x0 ��
	    		{
						
	    			OLED_WR_Byte(GIF[i][j++],OLED_DATA);	//��ͼƬ�ĵ�    	
	    		}
		}
		delay_ms(80);//��Ϊ���쿨�٣�����

	}
}

	void drawProgressBar(int t, int n)
{
	OLED_ShowString(0,0,"Progress:",16);
	int i =0;
	float f = 12.5 * t;
	char a[10];
	sprintf(a, "%.1f%%", f);
	OLED_ShowString(0,24,"[",12);
	for (; i < n; i++)
	{
		if (i < t)
		{
			OLED_ShowString(12 + i * 12,24,"*",12);	
		}
	}
	
	OLED_ShowString(112,24,"]",12);
	OLED_ShowString(88,48,a,12);
};

















