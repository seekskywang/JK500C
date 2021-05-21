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


void page_sysinfo(void)
{
	char buf[10];
  /*初始化后默认使用前景层*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*默认设置不透明	，该函数参数为不透明度，范围 0-0xff ，0为全透明，0xff为不透明*/
    LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BACK);
	/*经过LCD_SetLayer(LCD_FOREGROUND_LAYER)函数后，
	以下液晶操作都在前景层刷新，除非重新调用过LCD_SetLayer函数设置背景层*/
	
	
	DrawMenu();
	Drawsysinfomenu();
	if(LANG == chs)
	{
		LCD_SetTextColor(LCD_COLOR_HLT);  		
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(0,10, "<      >");
		LCD_DisplayStringLine(5,26, "系统信息");
		
		LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(50,10,"型号");
		
		LCD_DisplayStringLine(134,10,"传感器型号");
		LCD_DisplayStringLine(174,10,"通道数");
		LCD_DisplayStringLine(214,10,"仪器版本");
		LCD_DisplayStringLine(254,10,"操作系统");
		LCD_DisplayStringLine(294,10,"USB接口");
		LCD_DisplayStringLine(334,10,"仪器序列号");
	}else{
		LCD_SetTextColor(LCD_COLOR_HLT);  		
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(3,10, "<SYSTEM INFOMATION>");
		
		LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(50,13,"MODEL");
		LCD_DisplayStringLine(130,10,"SENSOR MODEL");
		LCD_DisplayStringLine(170,10,"CHAN AMOUNT");
		LCD_DisplayStringLine(210,10,"FW VERSION");
		LCD_DisplayStringLine(250,10,"OS");
		LCD_DisplayStringLine(290,10,"USB I/F");
		LCD_DisplayStringLine(330,10,"SERIAL NO.");
	}
	
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_SetBackColor(LCD_COLOR_BACK);
	if(LANG == chs)
	{
		if(CHNUM == 8)
		{
			LCD_DisplayStringLine(50,170,"JK500C");
		}else if(CHNUM == 16){
			LCD_DisplayStringLine(50,170,"JK500C");
		}
		LCD_DisplayStringLine(94,170,"手持多路温度测试仪");
		LCD_DisplayStringLine(130,170,"T,K,J,N,E,S,R,B");
		sprintf(buf,"%03d",CHNUM);
		LCD_DisplayStringLine(170,170,(uint8_t *)buf);
		LCD_DisplayStringLine(210,170,"REV 1.6");
		LCD_DisplayStringLine(250,170,"");
		LCD_DisplayStringLine(290,170,"REV A0");
		LCD_DisplayStringLine(330,170,(uint8_t *)SN);
		
	}else{
		if(CHNUM == 8)
		{		
			LCD_DisplayStringLine(50,220,"JK500C Handheld");
		}else if(CHNUM == 16){
			LCD_DisplayStringLine(50,170,"JK500C Handheld");
		}
		LCD_DisplayStringLine(90,220,"Muti-Channel Temp Meter");
		LCD_DisplayStringLine(130,220,"T,K,J,N,E,S,R,B");
		sprintf(buf,"%03d",CHNUM);
		LCD_DisplayStringLine(170,220,(uint8_t *)buf);
		LCD_DisplayStringLine(210,220,"REV 1.6");
		LCD_DisplayStringLine(250,220,"");
		LCD_DisplayStringLine(290,220,"REV A0");
		LCD_DisplayStringLine(330,220,(uint8_t *)SN);
	}
	//1.4修改触摸校准不保存的问题
	//1.6修改最大最小值判断算法
	if(LANG == chs)
	{
		DrawInstruction("系统信息页");
	}else{
		DrawInstruction("System information page");
	}
	page_flag = sysinfo;
}
