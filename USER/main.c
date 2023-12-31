#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "usmart.h"  
#include "sram.h"   
#include "malloc.h" 
#include "w25qxx.h"    
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	
#include "usmart.h"	
#include "touch.h"		
#include "usart3.h"
#include "common.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "oled.h"

//天气信号端口:80	
#define WEATHER_PORTNUM 	"80"
//天气服务器IP
#define WEATHER_SERVERIP 	"116.62.81.138"

//时间端口号
#define TIME_PORTNUM			"80"
//时间服务器IP
#define TIME_SERVERIP			"www.beijing-time.org"


u8 atk_8266_wifista_config(void)
{
	u8 *p;
	p=mymalloc(SRAMIN,50);	     //存wifi_ssid, wifi_password
	POINT_COLOR=RED;
	
	while(atk_8266_send_cmd("AT","OK",20))
	{
		atk_8266_quit_trans();//退出透传
		//atk_8266_send_cmd("AT+CIPMODE=0","OK",200);  //关闭透传模式	
		delay_ms(800);
	}
	while(atk_8266_send_cmd("ATE0","OK",20));//关闭回显
	atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//设置WIFI STA模式
	atk_8266_send_cmd("AT+RST","OK",20);		//DHCP服务器关闭(仅AP模式有效) 
	delay_ms(1000);         //延时3S等待重启成功
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	
	//设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!! 
	atk_8266_send_cmd("AT+CIPMUX=0","OK",20);  //单链接模式
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//设置无线参数:ssid,密码
	printf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);
	while(atk_8266_send_cmd(p,"WIFI GOT IP",300));					//连接目标路由器,并且获得IP
	
	myfree(SRAMIN,p);		//释放内存 
	return 0;		
}


u8 parse_now_weather(void)
{
	printf("begin parse_now_weather\n");
	char *pr,*strlocation, *strweather, *strtemp, *strtime;
	u8 size = 0;
	int len;
	u8 res;
	u8 temperature;
	pr = mymalloc(SRAMIN,1000);
	strlocation = mymalloc(SRAMIN,50);
	strweather = mymalloc(SRAMIN,50);
	strtemp = mymalloc(SRAMIN,50);
	strtime = mymalloc(SRAMIN,50);
	memset(pr,0,1000);
	memset(strlocation,0,50);
	memset(strweather,0,50);
	memset(strtemp,0,50);
	memset(strtime,0,50);
	
	cJSON* root;
	cJSON *pSub;
	cJSON *arrayItem;
	cJSON *pItem;
	cJSON *pSubItem;
	cJSON *pChildItem;
	root = mymalloc(SRAMIN,sizeof(cJSON));
	pSub = mymalloc(SRAMIN,sizeof(cJSON));
	pItem = mymalloc(SRAMIN,sizeof(cJSON));
	pSubItem = mymalloc(SRAMIN,sizeof(cJSON));
	pChildItem = mymalloc(SRAMIN,sizeof(cJSON));
	arrayItem = mymalloc(SRAMIN,sizeof(cJSON));
	
	
	//获取json对象
	root = cJSON_Parse((const char*)USART3_RX_BUF);
	if (root != NULL)
	{
		printf("begin GetObjectItem results\n");
		//获取results对应的值
		pSub = cJSON_GetObjectItem(root, "results");
		if (pSub != NULL)
		{
			printf("begin GetArrayItem 0\n");
			//根据下标获取json对象数组中的对象
			arrayItem = cJSON_GetArrayItem(pSub, 0);
			pr = cJSON_Print(arrayItem);
			pItem = cJSON_Parse(pr);
			
			if (pItem != NULL)
			{
				printf("begin GetObjectItem location\n");
				/*-------------------------------城市名称---------------------------*/
				//根据location键获取对应的值(cjson对象)
				pSubItem =cJSON_GetObjectItem(pItem, "location");
				if (pSubItem != NULL)
				{
					printf("begin GetObjectItem location name\n");
					pChildItem = cJSON_GetObjectItem(pSubItem, "name");
					if (pChildItem != NULL)
					{
						printf("begin cout location name\n");
						sprintf(strlocation,"%s", pChildItem->valuestring);
						printf("%s\n", pChildItem->valuestring);
							//OLED_Refresh_Gram();
							//OLED_ShowString(0,0, strlocation, 16);
					}
				}
				/*-------------------------------天气信息---------------------------*/
				pSubItem = cJSON_GetObjectItem(pItem,"now");
				if(pSubItem != NULL)
				{
					pChildItem = cJSON_GetObjectItem(pSubItem,"text");  
					if(pChildItem != NULL)
					{
						printf("begin cout weather\n");
						sprintf(strweather,"%s", pChildItem->valuestring);
						printf("%s\n", pChildItem->valuestring);
					}
					pChildItem = cJSON_GetObjectItem(pSubItem,"temperature");    
					if(pChildItem != NULL)
					{
						printf("begin cout temperature\n");
						sprintf(strtemp,"%s", pChildItem->valuestring);
						printf("%s\n", pChildItem->valuestring);
					}
			  }
				/*-------------------------------更新时间--------------------------*/
				pSubItem = cJSON_GetObjectItem(pItem,"last_update");  
				if(pSubItem != NULL)		
				{
						printf("begin cout time\n");
						sprintf(strtime,"%s", pSubItem->valuestring);
						printf("%s\n", pSubItem->valuestring);
				}
		  }
	}

	while(1)
	{
		OLED_Refresh_Gram();
	  OLED_ShowString(0,0, strlocation, 12);
		 OLED_ShowString(0,16, strweather, 12);
		 OLED_ShowString(60,16, strtemp, 12);
		OLED_ShowString(75,16, "*C", 12);
		 OLED_ShowString(0,32, strtime, 12);
	}
	
	
	cJSON_Delete(root);
	myfree(SRAMIN,root);
	myfree(SRAMIN,pSub);
	myfree(SRAMIN,pItem);
	myfree(SRAMIN,pSubItem);
	myfree(SRAMIN,pChildItem);
	myfree(SRAMIN,arrayItem);
	myfree(SRAMIN,pr);
	myfree(SRAMIN,strlocation);
	myfree(SRAMIN,strweather);
	myfree(SRAMIN,strtemp);
	myfree(SRAMIN,strtime);
	
	return 0;
}
}


u8 get_current_weather(void)
{
	u8 *p;
	u8 res;
	p=mymalloc(SRAMIN,40);							
	sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",WEATHER_SERVERIP,WEATHER_PORTNUM);    //?????IP?PORT,??TCP??
	res = atk_8266_send_cmd(p,"OK",500);//??TCP????
	if(res==1)
	{
		myfree(SRAMIN,p);
		return 1;
	}
	delay_ms(300);
	atk_8266_send_cmd("AT+CIPMODE=1","OK",100);      //�??????

	USART3_RX_STA=0;
	atk_8266_send_cmd("AT+CIPSEND","OK",100);         //????
	printf("start trans...\r\n");
	//??get??:????+????
	u3_printf("GET https://api.seniverse.com/v3/weather/now.json?key=SQAoPs1t5MIsRuE3Z&location=chongqing&language=en&unit=c\n\n");	
	delay_ms(20);

	USART3_RX_STA=0;	
	delay_ms(1000);

	if(USART3_RX_STA&0X8000)		//??????
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//?????
	} 
	
	parse_now_weather();     //CJSON??

	
	atk_8266_quit_trans();//????
	atk_8266_send_cmd("AT+CIPCLOSE","OK",50);         //??TCP??
	myfree(SRAMIN,p);
	return 0;
}


extern unsigned char logo[];

int main(void)
{        
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	usart3_init(115200);  //初始化串口3波特率为115200
	LED_Init();					//初始化LED  
	OLED_Init();		
	
	OLED_DrawBMP(40,0,96,7,logo);
	delay_ms(1000);
	
  int i = 0;
  while (i <= 8) 
  {
					drawProgressBar(i, 8);
					OLED_Refresh_Gram();
					delay_ms(500);
					i++;
  };
	
	OLED_Clear() ;
	
	atk_8266_wifista_config();
	get_current_weather();

//	while(1)
//	{
//		OLED_Refresh_Gram();
//		 OLED_ShowString(0,0, "chongqing", 12);
//		OLED_ShowString(0,16, "overcast", 12);
//			OLED_ShowString(100,16, "11", 12);
//		OLED_ShowString(0,32, "2022-12-03T15:20:12+08:00", 12);
//	}

}













