#ifndef __OLED_H
#define __OLED_H			  	 
#include "sys.h"
#include "stdlib.h"	    

//OLEDģʽ����
//0: 4�ߴ���ģʽ  ��ģ���BS1��BS2����GND��
//1: ����8080ģʽ ��ģ���BS1��BS2����VCC��
#define OLED_MODE 	1 
		    						  
//-----------------OLED�˿ڶ���----------------  					   
#define OLED_CS 	PBout(7)
#define OLED_RST  PGout(15)	
#define OLED_RS 	PDout(6)
#define OLED_WR 	PAout(4)		  
#define OLED_RD 	PDout(7)
 
//ʹ��4�ߴ��нӿ�ʱʹ�� 
#define OLED_SCLK 	PCout(6)
#define OLED_SDIN 	PCout(7)
		     
#define OLED_CMD  	0		//д����
#define OLED_DATA 	1		//д����
//OLED�����ú���
void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);		   
		
		
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size);	 
void OLED_Set_Pos(unsigned char x, unsigned char y);		//���㺯��
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char no,unsigned char Chn[][16]); //������ָ��λ����ʾ���ֺ���
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]); //��ʾͼƬ����
void OLED_ShowPicture(u8 x,u8 y,u8 index,u8 mode);
void OLED_DrawGIF(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1, unsigned char k, int m, unsigned char GIF[][512]); //��ʾ��ͼ��δ���ԣ�
	void drawProgressBar(int t, int n);

#endif  
	 



