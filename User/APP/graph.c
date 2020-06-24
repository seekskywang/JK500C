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
float G_Data[16][500];//温度曲线数据




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
	range = (YHLIMIT - YLLIMIT) / 7;
	u8 i;
	u16 j;
	char timetemp[100];
	enrate = 350/(float)(YHLIMIT - YLLIMIT);
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
	if(LANG == chs)
	{
		LCD_DisplayStringLine(0,10, "<曲线图>");
	}else if(LANG == eng){
		LCD_DisplayStringLine(0,10, "<GRAPH>");
	}
//	LCD_DisplayStringLine(5,26, "曲线图");
	
	DrawMenu();
	Drawgraphmenu();
	DrawUnit();
//	DrawCharge();
	
	LCD_SetTextColor(LCD_COLOR_RED);
	LCD_SetBackColor(LCD_COLOR_BACK);
	for(i = 0;i < 7;i ++)
	{
		sprintf(buf,"%d",(int)(YHLIMIT - range * i));
		DISP_CNL_S(40 + 50*i,5/*90*/,(uint8_t* )buf);
	}
	
	
	DrawGridLine();
	
	for(j = 0; j < count - 1; j++)
	{
		if(CH1_SW == ch_on && G_Data[0][j] < YHLIMIT && G_Data[0][j] > YLLIMIT && G_Data[0][j+1] < YHLIMIT && G_Data[0][j+1] > YLLIMIT)
		{
			LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BACK);
//			if(G_Data[0][j] > YHLIMIT)
//			{
//				G_Data[0][j] = YHLIMIT;
//			}
//			if(G_Data[0][j+1] > YHLIMIT)
//			{
//				G_Data[0][j+1] = YHLIMIT;
//			}
			LCD_DrawUniLine(111-80 + j,(int)(400-((G_Data[0][j] - YLLIMIT)*enrate)),112 - 80 + j,(400-((G_Data[0][j+1] - YLLIMIT)*enrate)));
		}
		if(CH2_SW == ch_on && G_Data[1][j] < YHLIMIT && G_Data[1][j] > YLLIMIT && G_Data[1][j+1] < YHLIMIT && G_Data[1][j+1] > YLLIMIT)
		{         
			LCD_SetColors(LCD_COLOR_GREEN,LCD_COLOR_BACK);
			LCD_DrawUniLine(111-80 + j,(int)(400-((G_Data[1][j] - YLLIMIT)*enrate)),112 - 80 + j,(400-((G_Data[1][j+1] - YLLIMIT)*enrate)));
		}
		if(CH3_SW == ch_on && G_Data[2][j] < YHLIMIT && G_Data[2][j] > YLLIMIT && G_Data[2][j+1] < YHLIMIT && G_Data[2][j+1] > YLLIMIT)
		{
			LCD_SetColors(LCD_COLOR_MAGENTA,LCD_COLOR_BACK);
			LCD_DrawUniLine(111-80 + j,(int)(400-((G_Data[2][j] - YLLIMIT)*enrate)),112 - 80 + j,(400-((G_Data[2][j+1] - YLLIMIT)*enrate)));
		}
		if(CH4_SW == ch_on && G_Data[3][j] < YHLIMIT && G_Data[3][j] > YLLIMIT && G_Data[3][j+1] < YHLIMIT && G_Data[3][j+1] > YLLIMIT)
		{
			LCD_SetColors(LCD_COLOR_CYAN,LCD_COLOR_BACK);
			LCD_DrawUniLine(111-80 + j,(int)(400-((G_Data[3][j] - YLLIMIT)*enrate)),112 - 80 + j,(400-((G_Data[3][j+1] - YLLIMIT)*enrate)));
		}		
		if(CH5_SW == ch_on && G_Data[4][j] < YHLIMIT && G_Data[4][j] > YLLIMIT && G_Data[4][j+1] < YHLIMIT && G_Data[4][j+1] > YLLIMIT)
		{
			LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_BACK);
			LCD_DrawUniLine(111-80 + j,(int)(400-((G_Data[4][j] - YLLIMIT)*enrate)),112 - 80 + j,(400-((G_Data[4][j+1] - YLLIMIT)*enrate)));
		}
		if(CH6_SW == ch_on && G_Data[5][j] < YHLIMIT && G_Data[5][j] > YLLIMIT && G_Data[5][j+1] < YHLIMIT && G_Data[5][j+1] > YLLIMIT)
		{
			LCD_SetColors(LCD_COLOR_HLT,LCD_COLOR_BACK);
			LCD_DrawUniLine(111-80 + j,(int)(400-((G_Data[5][j] - YLLIMIT)*enrate)),112 - 80 + j,(400-((G_Data[5][j+1] - YLLIMIT)*enrate)));
		}
		if(CH7_SW == ch_on && G_Data[6][j] < YHLIMIT && G_Data[6][j] > YLLIMIT && G_Data[6][j+1] < YHLIMIT && G_Data[6][j+1] > YLLIMIT)
		{
			LCD_SetColors(LCD_COLOR_BT,LCD_COLOR_BACK);
			LCD_DrawUniLine(111-80 + j,(int)(400-((G_Data[6][j] - YLLIMIT)*enrate)),112 - 80 + j,(400-((G_Data[6][j+1] - YLLIMIT)*enrate)));
		}
		if(CH8_SW == ch_on && G_Data[7][j] < YHLIMIT && G_Data[7][j] > YLLIMIT && G_Data[7][j+1] < YHLIMIT && G_Data[7][j+1] > YLLIMIT)
		{
			LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_BACK);
			LCD_DrawUniLine(111-80 + j,(int)(400-((G_Data[7][j] - YLLIMIT)*enrate)),112 - 80 + j,(400-((G_Data[7][j+1] - YLLIMIT)*enrate)));
		}
		if(CHNUM > 8)
		{
			if(CH9_SW == ch_on && G_Data[8][j] < YHLIMIT && G_Data[8][j] > YLLIMIT && G_Data[8][j+1] < YHLIMIT && G_Data[8][j+1] > YLLIMIT)
			{
				LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_BACK);
				LCD_DrawUniLine(111-80 + j,(int)(400-((G_Data[8][j] - YLLIMIT)*enrate)),112 - 80 + j,(400-((G_Data[8][j+1] - YLLIMIT)*enrate)));
			}
			if(CH10_SW == ch_on && G_Data[1][9] < YHLIMIT && G_Data[1][9] > YLLIMIT && G_Data[9][j+1] < YHLIMIT && G_Data[9][j+1] > YLLIMIT)
			{
				LCD_SetColors(LCD_COLOR_GR1,LCD_COLOR_BACK);
				LCD_DrawUniLine(111-80 + j,(int)(400-((G_Data[9][j] - YLLIMIT)*enrate)),112 - 80 + j,(400-((G_Data[9][j+1] - YLLIMIT)*enrate)));
			}
			if(CH11_SW == ch_on && G_Data[10][j] < YHLIMIT && G_Data[10][j] > YLLIMIT && G_Data[10][j+1] < YHLIMIT && G_Data[10][j+1] > YLLIMIT)
			{
				LCD_SetColors(LCD_COLOR_GR2,LCD_COLOR_BACK);
				LCD_DrawUniLine(111-80 + j,(int)(400-((G_Data[10][j] - YLLIMIT)*enrate)),112 - 80 + j,(400-((G_Data[10][j+1] - YLLIMIT)*enrate)));
			}
			if(CH12_SW == ch_on && G_Data[11][j] < YHLIMIT && G_Data[11][j] > YLLIMIT && G_Data[11][j+1] < YHLIMIT && G_Data[11][j+1] > YLLIMIT)
			{
				LCD_SetColors(LCD_COLOR_GR3,LCD_COLOR_BACK);
				LCD_DrawUniLine(111-80 + j,(int)(400-((G_Data[11][j] - YLLIMIT)*enrate)),112 - 80 + j,(400-((G_Data[11][j+1] - YLLIMIT)*enrate)));
			}
			if(CH13_SW == ch_on && G_Data[12][j] < YHLIMIT && G_Data[12][j] > YLLIMIT && G_Data[12][j+1] < YHLIMIT && G_Data[12][j+1] > YLLIMIT)
			{
				LCD_SetColors(LCD_COLOR_GR4,LCD_COLOR_BACK);
				LCD_DrawUniLine(111-80 + j,(int)(400-((G_Data[12][j] - YLLIMIT)*enrate)),112 - 80 + j,(400-((G_Data[12][j+1] - YLLIMIT)*enrate)));
			}
			if(CH14_SW == ch_on && G_Data[13][j] < YHLIMIT && G_Data[13][j] > YLLIMIT && G_Data[13][j+1] < YHLIMIT && G_Data[13][j+1] > YLLIMIT)
			{
				LCD_SetColors(LCD_COLOR_GR5,LCD_COLOR_BACK);
				LCD_DrawUniLine(111-80 + j,(int)(400-((G_Data[13][j] - YLLIMIT)*enrate)),112 - 80 + j,(400-((G_Data[13][j+1] - YLLIMIT)*enrate)));
			}
			if(CH15_SW == ch_on && G_Data[14][j] < YHLIMIT && G_Data[14][j] > YLLIMIT && G_Data[14][j+1] < YHLIMIT && G_Data[14][j+1] > YLLIMIT)
			{
				LCD_SetColors(LCD_COLOR_GR6,LCD_COLOR_BACK);
				LCD_DrawUniLine(111-80 + j,(int)(400-((G_Data[14][j] - YLLIMIT)*enrate)),112 - 80 + j,(400-((G_Data[14][j+1] - YLLIMIT)*enrate)));
			}
			if(CH16_SW == ch_on && G_Data[15][j] < YHLIMIT && G_Data[15][j] > YLLIMIT && G_Data[15][j+1] < YHLIMIT && G_Data[15][j+1] > YLLIMIT)
			{
				LCD_SetColors(LCD_COLOR_GR7,LCD_COLOR_BACK);
				LCD_DrawUniLine(111-80 + j,(int)(400-((G_Data[15][j] - YLLIMIT)*enrate)),112 - 80 + j,(400-((G_Data[15][j+1] - YLLIMIT)*enrate)));
			}
		}
	}
	
	if(count < 50)
	{
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[0][4],
			time_buf[0][5],
			time_buf[0][6]);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		DISP_CNL_S(402,80-80,(uint8_t *)timetemp);
	}
	
	if(count >= 50 && count < 100)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[0][4],
			time_buf[0][5],
			time_buf[0][6]);
		DISP_CNL_S(402,80-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[1][4],
			time_buf[1][5],
			time_buf[1][6]);
		DISP_CNL_S(420,130-80,(uint8_t *)timetemp);
	}
	
	if(count >= 100 && count < 150)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[0][4],
			time_buf[0][5],
			time_buf[0][6]);		
		DISP_CNL_S(402,80-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[1][4],
			time_buf[1][5],
			time_buf[1][6]);
		DISP_CNL_S(420,130-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[2][4],
			time_buf[2][5],
			time_buf[2][6]);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		DISP_CNL_S(402,180-80,(uint8_t *)timetemp);
	}
	
	if(count >= 150 && count < 200)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[0][4],
			time_buf[0][5],
			time_buf[0][6]);		
		DISP_CNL_S(402,80-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[1][4],
			time_buf[1][5],
			time_buf[1][6]);
		DISP_CNL_S(420,130-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[2][4],
			time_buf[2][5],
			time_buf[2][6]);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		DISP_CNL_S(402,180-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[3][4],
			time_buf[3][5],
			time_buf[3][6]);
		DISP_CNL_S(420,230-80,(uint8_t *)timetemp);
	}
	
	if(count >= 200 && count < 250)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[0][4],
			time_buf[0][5],
			time_buf[0][6]);		
		DISP_CNL_S(402,80-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[1][4],
			time_buf[1][5],
			time_buf[1][6]);
		DISP_CNL_S(420,130-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[2][4],
			time_buf[2][5],
			time_buf[2][6]);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		DISP_CNL_S(402,180-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[3][4],
			time_buf[3][5],
			time_buf[3][6]);
		DISP_CNL_S(420,230-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[4][4],
			time_buf[4][5],
			time_buf[4][6]);
		DISP_CNL_S(402,280-80,(uint8_t *)timetemp);
	}
	
	if(count >= 250 && count < 300)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[0][4],
			time_buf[0][5],
			time_buf[0][6]);		
		DISP_CNL_S(402,80-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[1][4],
			time_buf[1][5],
			time_buf[1][6]);
		DISP_CNL_S(420,130-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[2][4],
			time_buf[2][5],
			time_buf[2][6]);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		DISP_CNL_S(402,180-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[3][4],
			time_buf[3][5],
			time_buf[3][6]);
		DISP_CNL_S(420,230-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[4][4],
			time_buf[4][5],
			time_buf[4][6]);
		DISP_CNL_S(402,280-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[5][4],
			time_buf[5][5],
			time_buf[5][6]);
		DISP_CNL_S(420,330-80,(uint8_t *)timetemp);
	}
	
	if(count >= 300 && count < 350)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[0][4],
			time_buf[0][5],
			time_buf[0][6]);		
		DISP_CNL_S(402,80-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[1][4],
			time_buf[1][5],
			time_buf[1][6]);
		DISP_CNL_S(420,130-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[2][4],
			time_buf[2][5],
			time_buf[2][6]);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		DISP_CNL_S(402,180-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[3][4],
			time_buf[3][5],
			time_buf[3][6]);
		DISP_CNL_S(420,230-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[4][4],
			time_buf[4][5],
			time_buf[4][6]);
		DISP_CNL_S(402,280-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[5][4],
			time_buf[5][5],
			time_buf[5][6]);
		DISP_CNL_S(420,330-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[6][4],
			time_buf[6][5],
			time_buf[6][6]);
		DISP_CNL_S(402,380-80,(uint8_t *)timetemp);
	}
	
	if(count >= 350 && count < 400)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[0][4],
			time_buf[0][5],
			time_buf[0][6]);		
		DISP_CNL_S(402,80-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[1][4],
			time_buf[1][5],
			time_buf[1][6]);
		DISP_CNL_S(420,130-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[2][4],
			time_buf[2][5],
			time_buf[2][6]);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		DISP_CNL_S(402,180-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[3][4],
			time_buf[3][5],
			time_buf[3][6]);
		DISP_CNL_S(420,230-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[4][4],
			time_buf[4][5],
			time_buf[4][6]);
		DISP_CNL_S(402,280-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[5][4],
			time_buf[5][5],
			time_buf[5][6]);
		DISP_CNL_S(420,330-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[6][4],
			time_buf[6][5],
			time_buf[6][6]);
		DISP_CNL_S(402,380-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[7][4],
			time_buf[7][5],
			time_buf[7][6]);
		DISP_CNL_S(420,430-80,(uint8_t *)timetemp);
	}
	
	if(count >= 400 && count < 450)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[0][4],
			time_buf[0][5],
			time_buf[0][6]);		
		DISP_CNL_S(402,80-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[1][4],
			time_buf[1][5],
			time_buf[1][6]);
		DISP_CNL_S(420,130-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[2][4],
			time_buf[2][5],
			time_buf[2][6]);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		DISP_CNL_S(402,180-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[3][4],
			time_buf[3][5],
			time_buf[3][6]);
		DISP_CNL_S(420,230-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[4][4],
			time_buf[4][5],
			time_buf[4][6]);
		DISP_CNL_S(402,280-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[5][4],
			time_buf[5][5],
			time_buf[5][6]);
		DISP_CNL_S(420,330-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[6][4],
			time_buf[6][5],
			time_buf[6][6]);
		DISP_CNL_S(402,380-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[7][4],
			time_buf[7][5],
			time_buf[7][6]);
		DISP_CNL_S(420,430-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[8][4],
			time_buf[8][5],
			time_buf[8][6]);
		DISP_CNL_S(402,480-80,(uint8_t *)timetemp);
	}
	
	if(count >= 450 && count < 500)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[0][4],
			time_buf[0][5],
			time_buf[0][6]);		
		DISP_CNL_S(402,80-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[1][4],
			time_buf[1][5],
			time_buf[1][6]);
		DISP_CNL_S(420,130-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[2][4],
			time_buf[2][5],
			time_buf[2][6]);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		DISP_CNL_S(402,180-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[3][4],
			time_buf[3][5],
			time_buf[3][6]);
		DISP_CNL_S(420,230-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[4][4],
			time_buf[4][5],
			time_buf[4][6]);
		DISP_CNL_S(402,280-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[5][4],
			time_buf[5][5],
			time_buf[5][6]);
		DISP_CNL_S(420,330-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[6][4],
			time_buf[6][5],
			time_buf[6][6]);
		DISP_CNL_S(402,380-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[7][4],
			time_buf[7][5],
			time_buf[7][6]);
		DISP_CNL_S(420,430-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[8][4],
			time_buf[8][5],
			time_buf[8][6]);
		DISP_CNL_S(402,480-80,(uint8_t *)timetemp);
		
		sprintf(timetemp,"%0.2d:%0.2d:%0.2d",
			time_buf[9][4],
			time_buf[9][5],
			time_buf[9][6]);
		DISP_CNL_S(420,530-80,(uint8_t *)timetemp);
	}
	
	
	LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_RED);
	LCD_DrawLine(540,100-30,10,LCD_DIR_HORIZONTAL);
	LCD_SetBackColor(LCD_COLOR_BACK);
	DISP_CNL_S(90-30,10+540,"001");
	
	LCD_SetColors(LCD_COLOR_GREEN,LCD_COLOR_GREEN);
	LCD_DrawLine(540,120-30,10,LCD_DIR_HORIZONTAL);
	LCD_SetBackColor(LCD_COLOR_BACK);
	DISP_CNL_S(110-30,10+540,"002");
	
	LCD_SetColors(LCD_COLOR_MAGENTA,LCD_COLOR_GREEN);
	LCD_DrawLine(540,140-30,10,LCD_DIR_HORIZONTAL);
	LCD_SetBackColor(LCD_COLOR_BACK);
	DISP_CNL_S(130-30,10+540,"003");
	
	LCD_SetColors(LCD_COLOR_CYAN,LCD_COLOR_GREEN);
	LCD_DrawLine(540,160-30,10,LCD_DIR_HORIZONTAL);
	LCD_SetBackColor(LCD_COLOR_BACK);
	DISP_CNL_S(150-30,10+540,"004");
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_GREEN);
	LCD_DrawLine(540,180-30,10,LCD_DIR_HORIZONTAL);
	LCD_SetBackColor(LCD_COLOR_BACK);
	DISP_CNL_S(170-30,10+540,"005");
	
	LCD_SetColors(LCD_COLOR_HLT,LCD_COLOR_GREEN);
	LCD_DrawLine(540,200-30,10,LCD_DIR_HORIZONTAL);
	LCD_SetBackColor(LCD_COLOR_BACK);
	DISP_CNL_S(190-30,10+540,"006");
	
	LCD_SetColors(LCD_COLOR_BT,LCD_COLOR_GREEN);
	LCD_DrawLine(540,220-30,10,LCD_DIR_HORIZONTAL);
	LCD_SetBackColor(LCD_COLOR_BACK);
	DISP_CNL_S(210-30,10+540,"007");
	
	LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_GREEN);
	LCD_DrawLine(540,240-30,10,LCD_DIR_HORIZONTAL);
	LCD_SetBackColor(LCD_COLOR_BACK);
	DISP_CNL_S(230-30,10+540,"008");
	
	if(CHNUM > 8)
	{
		LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_GREEN);
		LCD_DrawLine(540,260-30,10,LCD_DIR_HORIZONTAL);
		LCD_SetBackColor(LCD_COLOR_BACK);
		DISP_CNL_S(250-30,10+540,"009");
		
		LCD_SetColors(LCD_COLOR_GR1,LCD_COLOR_GREEN);
		LCD_DrawLine(540,280-30,10,LCD_DIR_HORIZONTAL);
		LCD_SetBackColor(LCD_COLOR_BACK);
		DISP_CNL_S(270-30,10+540,"010");
		
		LCD_SetColors(LCD_COLOR_GR2,LCD_COLOR_GREEN);
		LCD_DrawLine(540,300-30,10,LCD_DIR_HORIZONTAL);
		LCD_SetBackColor(LCD_COLOR_BACK);
		DISP_CNL_S(290-30,10+540,"011");
		
		LCD_SetColors(LCD_COLOR_GR3,LCD_COLOR_GREEN);
		LCD_DrawLine(540,320-30,10,LCD_DIR_HORIZONTAL);
		LCD_SetBackColor(LCD_COLOR_BACK);
		DISP_CNL_S(310-30,10+540,"012");
		
		LCD_SetColors(LCD_COLOR_GR4,LCD_COLOR_GREEN);
		LCD_DrawLine(540,340-30,10,LCD_DIR_HORIZONTAL);
		LCD_SetBackColor(LCD_COLOR_BACK);
		DISP_CNL_S(330-30,10+540,"013");
		
		LCD_SetColors(LCD_COLOR_GR5,LCD_COLOR_GREEN);
		LCD_DrawLine(540,360-30,10,LCD_DIR_HORIZONTAL);
		LCD_SetBackColor(LCD_COLOR_BACK);
		DISP_CNL_S(350-30,10+540,"014");
		
		LCD_SetColors(LCD_COLOR_GR6,LCD_COLOR_GREEN);
		LCD_DrawLine(540,380-30,10,LCD_DIR_HORIZONTAL);
		LCD_SetBackColor(LCD_COLOR_BACK);
		DISP_CNL_S(370-30,10+540,"015");
		
		LCD_SetColors(LCD_COLOR_GR7,LCD_COLOR_GREEN);
		LCD_DrawLine(540,400-30,10,LCD_DIR_HORIZONTAL);
		LCD_SetBackColor(LCD_COLOR_BACK);
		DISP_CNL_S(390-30,10+540,"016");
	}



	page_flag = graph;
}

void Draw_graph(void)
{
	
	enrate = 350/(float)(YHLIMIT - YLLIMIT);
	if(CH1_SW == ch_on && page_flag == graph && G_Data[0][count]< YHLIMIT && G_Data[0][count]> YLLIMIT && G_Data[0][count-1]< YHLIMIT && G_Data[0][count-1]> YLLIMIT)
	{
		LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BACK);
		if(count == 0)			
		{
			LCD_DrawUniLine(111-80,(int)(400-((CH1TEMP - COR1 - YLLIMIT)*enrate)),112-80,(int)(400-((CH1TEMP - COR1 - YLLIMIT)*enrate)));
		}else{	
			LCD_DrawUniLine(111-80+count,(int)(400-((G_Data[0][count-1] - YLLIMIT)*enrate))/*400-G_Data[count-1]*/,112-80+count,(400-((G_Data[0][count] - YLLIMIT)*enrate)));
		}
	}
	if(CH2_SW == ch_on && page_flag == graph && G_Data[1][count] < YHLIMIT && G_Data[1][count] > YLLIMIT && G_Data[1][count-1]< YHLIMIT && G_Data[1][count-1]> YLLIMIT)
	{
		LCD_SetColors(LCD_COLOR_GREEN,LCD_COLOR_BACK);
		if(count == 0)			
		{
			LCD_DrawUniLine(111-80,(int)(400-((CH2TEMP - COR2 - YLLIMIT)*enrate)),112-80,(int)(400-((CH2TEMP - COR2 - YLLIMIT)*enrate)));
		}else{	
			LCD_DrawUniLine(111-80+count,(int)(400-((G_Data[1][count-1]- YLLIMIT)*enrate))/*400-G_Data[count-1]*/,112-80+count,(400-((G_Data[1][count]- YLLIMIT)*enrate)));
		}
	}
	if(CH3_SW == ch_on && page_flag == graph && G_Data[2][count] < YHLIMIT && G_Data[2][count] > YLLIMIT && G_Data[2][count-1]< YHLIMIT && G_Data[2][count-1]> YLLIMIT)
	{
		LCD_SetColors(LCD_COLOR_MAGENTA,LCD_COLOR_BACK);
		if(count == 0)
			
		{
			LCD_DrawUniLine(111-80,(int)(400-((CH3TEMP - COR3 - YLLIMIT)*enrate)),112-80,(int)(400-((CH3TEMP - COR3 - YLLIMIT)*enrate)));
		}else{	
			LCD_DrawUniLine(111-80+count,(int)(400-((G_Data[2][count-1] - YLLIMIT)*enrate))/*400-G_Data[count-1]*/,112-80+count,(400-((G_Data[2][count] - YLLIMIT)*enrate)));
		}		
	}
	if(CH4_SW == ch_on && page_flag == graph && G_Data[3][count] < YHLIMIT && G_Data[3][count]  > YLLIMIT && G_Data[3][count-1]< YHLIMIT && G_Data[3][count-1]> YLLIMIT)
	{
		LCD_SetColors(LCD_COLOR_CYAN,LCD_COLOR_BACK);
		if(count == 0)
			
		{
			LCD_DrawUniLine(111-80,(int)(400-((CH4TEMP - COR4  - YLLIMIT)*enrate)),112-80,(int)(400-((CH4TEMP - COR4  - YLLIMIT)*enrate)));
		}else{	
			LCD_DrawUniLine(111-80+count,(int)(400-((G_Data[3][count-1]  - YLLIMIT)*enrate))/*400-G_Data[count-1]*/,112-80+count,(400-((G_Data[3][count]  - YLLIMIT)*enrate)));
		}		
	}
	if(CH5_SW == ch_on && page_flag == graph && G_Data[4][count]  < YHLIMIT && G_Data[4][count] > YLLIMIT && G_Data[4][count-1]< YHLIMIT && G_Data[4][count-1]> YLLIMIT)
	{
		LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_BACK);
		if(count == 0)
			
		{
			LCD_DrawUniLine(111-80,(int)(400-((CH5TEMP - COR5 - YLLIMIT)*enrate)),112-80,(int)(400-((CH5TEMP - COR5 - YLLIMIT)*enrate)));
		}else{	
			LCD_DrawUniLine(111-80+count,(int)(400-((G_Data[4][count-1] - YLLIMIT)*enrate))/*400-G_Data[count-1]*/,112-80+count,(400-((G_Data[4][count] - YLLIMIT)*enrate)));
		}		
	}
	if(CH6_SW == ch_on && page_flag == graph && G_Data[5][count] < YHLIMIT && G_Data[5][count] > YLLIMIT && G_Data[5][count-1]< YHLIMIT && G_Data[5][count-1]> YLLIMIT)
	{
		LCD_SetColors(LCD_COLOR_HLT,LCD_COLOR_BACK);
		if(count == 0)
			
		{
			LCD_DrawUniLine(111-80,(int)(400-((CH6TEMP - COR6 - YLLIMIT)*enrate)),112-80,(int)(400-((CH6TEMP - COR6 - YLLIMIT)*enrate)));
		}else{	
			LCD_DrawUniLine(111-80+count,(int)(400-((G_Data[5][count-1] - YLLIMIT)*enrate))/*400-G_Data[count-1]*/,112-80+count,(400-((G_Data[5][count] - YLLIMIT)*enrate)));
		}		
	}
	if(CH7_SW == ch_on && page_flag == graph && G_Data[6][count] < YHLIMIT && G_Data[6][count] > YLLIMIT && G_Data[6][count-1]< YHLIMIT && G_Data[6][count-1]> YLLIMIT)
	{
		LCD_SetColors(LCD_COLOR_BT,LCD_COLOR_BACK);
		if(count == 0)
			
		{
			LCD_DrawUniLine(111-80,(int)(400-((CH7TEMP - COR7 - YLLIMIT)*enrate)),112-80,(int)(400-((CH7TEMP - COR7 - YLLIMIT)*enrate)));
		}else{	
			LCD_DrawUniLine(111-80+count,(int)(400-((G_Data[6][count-1] - YLLIMIT)*enrate))/*400-G_Data[count-1]*/,112-80+count,(400-((G_Data[6][count] - YLLIMIT)*enrate)));
		}		
	}
	if(CH8_SW == ch_on && page_flag == graph && G_Data[7][count] < YHLIMIT && G_Data[7][count] > YLLIMIT && G_Data[7][count-1]< YHLIMIT && G_Data[7][count-1]> YLLIMIT)
	{
		LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_BACK);
		if(count == 0)
			
		{
			LCD_DrawUniLine(111-80,(int)(400-((CH8TEMP - COR8 - YLLIMIT)*enrate)),112-80,(int)(400-((CH8TEMP - COR8 - YLLIMIT)*enrate)));
		}else{	
			LCD_DrawUniLine(111-80+count,(int)(400-((G_Data[7][count-1] - YLLIMIT)*enrate))/*400-G_Data[count-1]*/,112-80+count,(400-((G_Data[7][count] - YLLIMIT)*enrate)));
		}		
	}
	if(CHNUM > 8)
	{
		if(CH9_SW == ch_on && page_flag == graph && G_Data[8][count] < YHLIMIT && G_Data[8][count] > YLLIMIT && G_Data[8][count-1]< YHLIMIT && G_Data[8][count-1]> YLLIMIT)
		{
			LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_BACK);
			if(count == 0)
				
			{
				LCD_DrawUniLine(111-80,(int)(400-((CH9TEMP - COR9 - YLLIMIT)*enrate)),112-80,(int)(400-((CH9TEMP - COR9 - YLLIMIT)*enrate)));
			}else{	
				LCD_DrawUniLine(111-80+count,(int)(400-((G_Data[8][count-1] - YLLIMIT)*enrate))/*400-G_Data[count-1]*/,112-80+count,(400-((G_Data[8][count] - YLLIMIT)*enrate)));
			}		
		}
		if(CH10_SW == ch_on && page_flag == graph && G_Data[9][count] < YHLIMIT && G_Data[9][count] > YLLIMIT && G_Data[9][count-1]< YHLIMIT && G_Data[9][count-1]> YLLIMIT)
		{
			LCD_SetColors(LCD_COLOR_GR1,LCD_COLOR_BACK);
			if(count == 0)
				
			{
				LCD_DrawUniLine(111-80,(int)(400-((CH10TEMP - COR10 - YLLIMIT)*enrate)),112-80,(int)(400-((CH10TEMP - COR10 - YLLIMIT)*enrate)));
			}else{	
				LCD_DrawUniLine(111-80+count,(int)(400-((G_Data[9][count-1] - YLLIMIT)*enrate))/*400-G_Data[count-1]*/,112-80+count,(400-((G_Data[9][count] - YLLIMIT)*enrate)));
			}		
		}
		if(CH11_SW == ch_on && page_flag == graph && G_Data[10][count] < YHLIMIT && G_Data[10][count] > YLLIMIT && G_Data[10][count-1]< YHLIMIT && G_Data[10][count-1]> YLLIMIT)
		{
			LCD_SetColors(LCD_COLOR_GR2,LCD_COLOR_BACK);
			if(count == 0)
				
			{
				LCD_DrawUniLine(111-80,(int)(400-((CH11TEMP - COR11 - YLLIMIT)*enrate)),112-80,(int)(400-((CH11TEMP - COR11 - YLLIMIT)*enrate)));
			}else{	
				LCD_DrawUniLine(111-80+count,(int)(400-((G_Data[10][count-1] - YLLIMIT)*enrate))/*400-G_Data[count-1]*/,112-80+count,(400-((G_Data[10][count] - YLLIMIT)*enrate)));
			}		
		}
		if(CH12_SW == ch_on && page_flag == graph && G_Data[11][count] < YHLIMIT && G_Data[11][count] > YLLIMIT && G_Data[11][count-1]< YHLIMIT && G_Data[11][count-1]> YLLIMIT)
		{
			LCD_SetColors(LCD_COLOR_GR3,LCD_COLOR_BACK);
			if(count == 0)
				
			{
				LCD_DrawUniLine(111-80,(int)(400-((CH12TEMP - COR12 - YLLIMIT)*enrate)),112-80,(int)(400-((CH12TEMP - COR12 - YLLIMIT)*enrate)));
			}else{	
				LCD_DrawUniLine(111-80+count,(int)(400-((G_Data[11][count-1] - YLLIMIT)*enrate))/*400-G_Data[count-1]*/,112-80+count,(400-((G_Data[11][count] - YLLIMIT)*enrate)));
			}		
		}
		if(CH13_SW == ch_on && page_flag == graph && G_Data[12][count] < YHLIMIT && G_Data[12][count] > YLLIMIT && G_Data[12][count-1]< YHLIMIT && G_Data[12][count-1]> YLLIMIT)
		{
			LCD_SetColors(LCD_COLOR_GR4,LCD_COLOR_BACK);
			if(count == 0)
				
			{
				LCD_DrawUniLine(111-80,(int)(400-((CH13TEMP - COR13 - YLLIMIT)*enrate)),112-80,(int)(400-((CH13TEMP - COR13 - YLLIMIT)*enrate)));
			}else{	
				LCD_DrawUniLine(111-80+count,(int)(400-((G_Data[12][count-1] - YLLIMIT)*enrate))/*400-G_Data[count-1]*/,112-80+count,(400-((G_Data[12][count] - YLLIMIT)*enrate)));
			}		
		}
		if(CH14_SW == ch_on && page_flag == graph && G_Data[13][count] < YHLIMIT && G_Data[13][count] > YLLIMIT && G_Data[13][count-1]< YHLIMIT && G_Data[13][count-1]> YLLIMIT)
		{
			LCD_SetColors(LCD_COLOR_GR5,LCD_COLOR_BACK);
			if(count == 0)
				
			{
				LCD_DrawUniLine(111-80,(int)(400-((CH14TEMP - COR14 - YLLIMIT)*enrate)),112-80,(int)(400-((CH14TEMP - COR14 - YLLIMIT)*enrate)));
			}else{	
				LCD_DrawUniLine(111-80+count,(int)(400-((G_Data[13][count-1] - YLLIMIT)*enrate))/*400-G_Data[count-1]*/,112-80+count,(400-((G_Data[13][count] - YLLIMIT)*enrate)));
			}		
		}
		if(CH15_SW == ch_on && page_flag == graph && G_Data[14][count] < YHLIMIT && G_Data[14][count] > YLLIMIT && G_Data[14][count-1]< YHLIMIT && G_Data[14][count-1]> YLLIMIT)
		{
			LCD_SetColors(LCD_COLOR_GR6,LCD_COLOR_BACK);
			if(count == 0)
				
			{
				LCD_DrawUniLine(111-80,(int)(400-((CH15TEMP - COR15 - YLLIMIT)*enrate)),112-80,(int)(400-((CH15TEMP - COR15 - YLLIMIT)*enrate)));
			}else{	
				LCD_DrawUniLine(111-80+count,(int)(400-((G_Data[14][count-1] - YLLIMIT)*enrate))/*400-G_Data[count-1]*/,112-80+count,(400-((G_Data[14][count] - YLLIMIT)*enrate)));
			}		
		}
		if(CH16_SW == ch_on && page_flag == graph && G_Data[15][count] < YHLIMIT && G_Data[15][count] > YLLIMIT && G_Data[15][count-1]< YHLIMIT && G_Data[15][count-1]> YLLIMIT)
		{
			LCD_SetColors(LCD_COLOR_GR7,LCD_COLOR_BACK);
			if(count == 0)
				
			{
				LCD_DrawUniLine(111-80,(int)(400-((CH16TEMP - COR16 - YLLIMIT)*enrate)),112-80,(int)(400-((CH16TEMP - COR16 - YLLIMIT)*enrate)));
			}else{	
				LCD_DrawUniLine(111-80+count,(int)(400-((G_Data[15][count-1] - YLLIMIT)*enrate))/*400-G_Data[count-1]*/,112-80+count,(400-((G_Data[15][count] - YLLIMIT)*enrate)));
			}		
		}
	}
//	LCD_SetTextColor(LCD_COLOR_WHITE);
//	LCD_SetBackColor(LCD_COLOR_BACK);
}
