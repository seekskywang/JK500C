/**
  ******************************************************************************
  * @file    sysytem.c
  * @author  fire
  * @version V1.0
  * @date    2018-04-29
  * @brief   手持多路温度测试仪系统配置界面
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./led/bsp_led.h"
#include "./lcd/bsp_lcd.h"
#include "./key/bsp_key.h"
#include "./beep/bsp_beep.h"
#include "./tim/bsp_basic_tim.h"
#include "jk508.h"
#include "./RTC/bsp_rtc.h"

float range;
float enrate;
float G_Data[500];

extern union 
{
   unsigned char FLAG_VAL7;
	
   struct 
   {
       unsigned char  FN:2;    //字体
       unsigned char  SP:2;    //速度
       unsigned char  BD:3;    //波特
       unsigned char  LG:1;	   //语言
   }BIT_FLAG;
}FLAG7;


void page_graph(void)
{
	range = (YHLIMIT - YLLIMIT) / 7.0;
	u8 i;
	u16 j;
	char timetemp[100];
	enrate = 350.0/(float)(YHLIMIT - YLLIMIT);
	char buf[10];
	
	
   /*初始化后默认使用前景层*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*默认设置不透明	，该函数参数为不透明度，范围 0-0xff ，0为全透明，0xff为不透明*/
    LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BACK);
	/*经过LCD_SetLayer(LCD_FOREGROUND_LAYER)函数后，
	以下液晶操作都在前景层刷新，除非重新调用过LCD_SetLayer函数设置背景层*/
	LCD_SetTextColor(LCD_COLOR_HLT);  	
	LCD_SetBackColor(LCD_COLOR_BACK);
	LCD_DisplayStringLine(0,10, "<曲线图>");
//	LCD_DisplayStringLine(5,26, "曲线图");
	
	DrawMenu();
	Drawgraphmenu();
	
	LCD_SetTextColor(LCD_COLOR_RED);
	LCD_SetBackColor(LCD_COLOR_BACK);
	for(i = 0;i < 7;i ++)
	{
		sprintf(buf,"%d",(int)(YHLIMIT - range * i));
		DISP_CNL_S(40 + 50*i,90,(uint8_t* )buf);
	}
	
	
	DrawGridLine();
	for(j = 0; j < count - 1; j++)
	{
		LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BACK);
		LCD_DrawUniLine(111 + j,(int)(400-((G_Data[j] - YLLIMIT)*enrate)),112+j,(400-((G_Data[j+1] - YLLIMIT)*enrate)));
	}
	
	if(count < 50)
	{
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[0]/3600,
			(time_buf[0]/60)%60,
			time_buf[0]%60);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		DISP_CNL_S(402,80,(uint8_t *)timetemp);
	}
	
	if(count >= 50 && count < 100)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[0]/3600,
			(time_buf[0]/60)%60,
			time_buf[0]%60);
		DISP_CNL_S(402,80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[1]/3600,
			(time_buf[1]/60)%60,
			time_buf[1]%60);
		DISP_CNL_S(420,130,(uint8_t *)timetemp);
	}
	
	if(count >= 100 && count < 150)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[0]/3600,
			(time_buf[0]/60)%60,
			time_buf[0]%60);		
		DISP_CNL_S(402,80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[1]/3600,
			(time_buf[1]/60)%60,
			time_buf[1]%60);
		DISP_CNL_S(420,130,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[2]/3600,
			(time_buf[2]/60)%60,
			time_buf[2]%60);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		DISP_CNL_S(402,180,(uint8_t *)timetemp);
	}
	
	if(count >= 150 && count < 200)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[0]/3600,
			(time_buf[0]/60)%60,
			time_buf[0]%60);		
		DISP_CNL_S(402,80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[1]/3600,
			(time_buf[1]/60)%60,
			time_buf[1]%60);
		DISP_CNL_S(420,130,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[2]/3600,
			(time_buf[2]/60)%60,
			time_buf[2]%60);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		DISP_CNL_S(402,180,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[3]/3600,
			(time_buf[3]/60)%60,
			time_buf[3]%60);
		DISP_CNL_S(420,230,(uint8_t *)timetemp);
	}
	
	if(count >= 200 && count < 250)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[0]/3600,
			(time_buf[0]/60)%60,
			time_buf[0]%60);		
		DISP_CNL_S(402,80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[1]/3600,
			(time_buf[1]/60)%60,
			time_buf[1]%60);
		DISP_CNL_S(420,130,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[2]/3600,
			(time_buf[2]/60)%60,
			time_buf[2]%60);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		DISP_CNL_S(402,180,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[3]/3600,
			(time_buf[3]/60)%60,
			time_buf[3]%60);
		DISP_CNL_S(420,230,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[4]/3600,
			(time_buf[4]/60)%60,
			time_buf[4]%60);
		DISP_CNL_S(402,280,(uint8_t *)timetemp);
	}
	
	if(count >= 250 && count < 300)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[0]/3600,
			(time_buf[0]/60)%60,
			time_buf[0]%60);		
		DISP_CNL_S(402,80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[1]/3600,
			(time_buf[1]/60)%60,
			time_buf[1]%60);
		DISP_CNL_S(420,130,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[2]/3600,
			(time_buf[2]/60)%60,
			time_buf[2]%60);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		DISP_CNL_S(402,180,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[3]/3600,
			(time_buf[3]/60)%60,
			time_buf[3]%60);
		DISP_CNL_S(420,230,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[4]/3600,
			(time_buf[4]/60)%60,
			time_buf[4]%60);
		DISP_CNL_S(402,280,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[5]/3600,
			(time_buf[5]/60)%60,
			time_buf[5]%60);
		DISP_CNL_S(420,330,(uint8_t *)timetemp);
	}
	
	if(count >= 300 && count < 350)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[0]/3600,
			(time_buf[0]/60)%60,
			time_buf[0]%60);		
		DISP_CNL_S(402,80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[1]/3600,
			(time_buf[1]/60)%60,
			time_buf[1]%60);
		DISP_CNL_S(420,130,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[2]/3600,
			(time_buf[2]/60)%60,
			time_buf[2]%60);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		DISP_CNL_S(402,180,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[3]/3600,
			(time_buf[3]/60)%60,
			time_buf[3]%60);
		DISP_CNL_S(420,230,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[4]/3600,
			(time_buf[4]/60)%60,
			time_buf[4]%60);
		DISP_CNL_S(402,280,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[5]/3600,
			(time_buf[5]/60)%60,
			time_buf[5]%60);
		DISP_CNL_S(420,330,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[6]/3600,
			(time_buf[6]/60)%60,
			time_buf[6]%60);
		DISP_CNL_S(402,380,(uint8_t *)timetemp);
	}
	
	if(count >= 350 && count < 400)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[0]/3600,
			(time_buf[0]/60)%60,
			time_buf[0]%60);		
		DISP_CNL_S(402,80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[1]/3600,
			(time_buf[1]/60)%60,
			time_buf[1]%60);
		DISP_CNL_S(420,130,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[2]/3600,
			(time_buf[2]/60)%60,
			time_buf[2]%60);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		DISP_CNL_S(402,180,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[3]/3600,
			(time_buf[3]/60)%60,
			time_buf[3]%60);
		DISP_CNL_S(420,230,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[4]/3600,
			(time_buf[4]/60)%60,
			time_buf[4]%60);
		DISP_CNL_S(402,280,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[5]/3600,
			(time_buf[5]/60)%60,
			time_buf[5]%60);
		DISP_CNL_S(420,330,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[6]/3600,
			(time_buf[6]/60)%60,
			time_buf[6]%60);
		DISP_CNL_S(402,380,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[7]/3600,
			(time_buf[7]/60)%60,
			time_buf[7]%60);
		DISP_CNL_S(420,430,(uint8_t *)timetemp);
	}
	
	if(count >= 400 && count < 450)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[0]/3600,
			(time_buf[0]/60)%60,
			time_buf[0]%60);		
		DISP_CNL_S(402,80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[1]/3600,
			(time_buf[1]/60)%60,
			time_buf[1]%60);
		DISP_CNL_S(420,130,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[2]/3600,
			(time_buf[2]/60)%60,
			time_buf[2]%60);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		DISP_CNL_S(402,180,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[3]/3600,
			(time_buf[3]/60)%60,
			time_buf[3]%60);
		DISP_CNL_S(420,230,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[4]/3600,
			(time_buf[4]/60)%60,
			time_buf[4]%60);
		DISP_CNL_S(402,280,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[5]/3600,
			(time_buf[5]/60)%60,
			time_buf[5]%60);
		DISP_CNL_S(420,330,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[6]/3600,
			(time_buf[6]/60)%60,
			time_buf[6]%60);
		DISP_CNL_S(402,380,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[7]/3600,
			(time_buf[7]/60)%60,
			time_buf[7]%60);
		DISP_CNL_S(420,430,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[8]/3600,
			(time_buf[8]/60)%60,
			time_buf[8]%60);
		DISP_CNL_S(402,480,(uint8_t *)timetemp);
	}
	
	if(count >= 450 && count < 500)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[0]/3600,
			(time_buf[0]/60)%60,
			time_buf[0]%60);		
		DISP_CNL_S(402,80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[1]/3600,
			(time_buf[1]/60)%60,
			time_buf[1]%60);
		DISP_CNL_S(420,130,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[2]/3600,
			(time_buf[2]/60)%60,
			time_buf[2]%60);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		DISP_CNL_S(402,180,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[3]/3600,
			(time_buf[3]/60)%60,
			time_buf[3]%60);
		DISP_CNL_S(420,230,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[4]/3600,
			(time_buf[4]/60)%60,
			time_buf[4]%60);
		DISP_CNL_S(402,280,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[5]/3600,
			(time_buf[5]/60)%60,
			time_buf[5]%60);
		DISP_CNL_S(420,330,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[6]/3600,
			(time_buf[6]/60)%60,
			time_buf[6]%60);
		DISP_CNL_S(402,380,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[7]/3600,
			(time_buf[7]/60)%60,
			time_buf[7]%60);
		DISP_CNL_S(420,430,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[8]/3600,
			(time_buf[8]/60)%60,
			time_buf[8]%60);
		DISP_CNL_S(402,480,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[9]/3600,
			(time_buf[9]/60)%60,
			time_buf[9]%60);
		DISP_CNL_S(420,530,(uint8_t *)timetemp);
	}
	
	
	LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_RED);
	LCD_DrawLine(5,100,10,LCD_DIR_HORIZONTAL);
	LCD_SetBackColor(LCD_COLOR_BACK);
	DISP_CNL_S(90,20,"001");
	
	LCD_SetColors(LCD_COLOR_GREEN,LCD_COLOR_GREEN);
	LCD_DrawLine(5,120,10,LCD_DIR_HORIZONTAL);
	LCD_SetBackColor(LCD_COLOR_BACK);
	DISP_CNL_S(110,20,"002");
	
	LCD_SetColors(LCD_COLOR_MAGENTA,LCD_COLOR_GREEN);
	LCD_DrawLine(5,140,10,LCD_DIR_HORIZONTAL);
	LCD_SetBackColor(LCD_COLOR_BACK);
	DISP_CNL_S(130,20,"003");
	
	LCD_SetColors(LCD_COLOR_CYAN,LCD_COLOR_GREEN);
	LCD_DrawLine(5,160,10,LCD_DIR_HORIZONTAL);
	LCD_SetBackColor(LCD_COLOR_BACK);
	DISP_CNL_S(150,20,"004");
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_GREEN);
	LCD_DrawLine(5,180,10,LCD_DIR_HORIZONTAL);
	LCD_SetBackColor(LCD_COLOR_BACK);
	DISP_CNL_S(170,20,"005");
	
	LCD_SetColors(LCD_COLOR_HLT,LCD_COLOR_GREEN);
	LCD_DrawLine(5,200,10,LCD_DIR_HORIZONTAL);
	LCD_SetBackColor(LCD_COLOR_BACK);
	DISP_CNL_S(190,20,"006");
	
	LCD_SetColors(LCD_COLOR_BT,LCD_COLOR_GREEN);
	LCD_DrawLine(5,220,10,LCD_DIR_HORIZONTAL);
	LCD_SetBackColor(LCD_COLOR_BACK);
	DISP_CNL_S(210,20,"007");
	
	LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_GREEN);
	LCD_DrawLine(5,240,10,LCD_DIR_HORIZONTAL);
	LCD_SetBackColor(LCD_COLOR_BACK);
	DISP_CNL_S(230,20,"008");



	page_flag = graph;
}

void Draw_graph(void)
{
	
	enrate = 350.0/(float)(YHLIMIT - YLLIMIT);
	if(page_flag == graph && G_Data[count] < YHLIMIT && G_Data[count] > YLLIMIT)
	{
		LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BACK);
		if(count == 0)
			
		{
			LCD_DrawUniLine(111,(int)(400-((ch1_temp - YLLIMIT)*enrate)),112,(int)(400-((ch1_temp - YLLIMIT)*enrate)));
		}else{
			LCD_DrawUniLine(111+count,(int)(400-((G_Data[count-1] - YLLIMIT)*enrate))/*400-G_Data[count-1]*/,112+count,(400-((G_Data[count] - YLLIMIT)*enrate)));
		}
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
	}
}