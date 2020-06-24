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

extern union 
{
   unsigned char FLAG_VAL8;
	
   struct 
   {
       unsigned char  BR:3;    //亮度
       unsigned char  DM:3;    //降低亮度
       
   }BIT_FLAG;
}FLAG8;

void page_sys(void)
{
	
  /*初始化后默认使用前景层*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*默认设置不透明	，该函数参数为不透明度，范围 0-0xff ，0为全透明，0xff为不透明*/
    LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BACK);
	/*经过LCD_SetLayer(LCD_FOREGROUND_LAYER)函数后，
	以下液晶操作都在前景层刷新，除非重新调用过LCD_SetLayer函数设置背景层*/
	
	char buf[10];
	
	DrawMenu();
	Drawsysmenu();
	if(LANG == chs)
	{
		LCD_SetTextColor(LCD_COLOR_HLT);  		
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(0,10, "<      >");
		LCD_DisplayStringLine(5,26, "系统设置");
		
		LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(50,13,"LANGUAGE");
		LCD_DisplayStringLine(94,10,"日期/时间");
//		LCD_DisplayStringLine(134,10,"账号");
		LCD_DisplayStringLine(134,10,"背光");
		LCD_DisplayStringLine(174,10,"降低亮度");
		LCD_DisplayStringLine(214,10,"触摸屏");
	}else{
		LCD_SetTextColor(LCD_COLOR_HLT);  		
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(3,10, "<SYSTEM CONFIG>");
		
		LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(50,13,"LANGUAGE");
		LCD_DisplayStringLine(90,10,"DATE/TIME");
//		LCD_DisplayStringLine(130,10,"ACCOUNT");
		LCD_DisplayStringLine(130,10,"BRIGHTNESS");
		LCD_DisplayStringLine(170,10,"DIM BRIGHT");
		LCD_DisplayStringLine(210,10,"TOUCH PANEL");
	}
	
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	if(LANG == chs)
	{
		LCD_DisplayStringLine(50,170,"CHINESE");
		LCD_SetTextColor(LCD_COLOR_YELLOW);
		LCD_SetBackColor(LCD_COLOR_BACK);
		if(BRTS == L0)
		{
			sprintf(buf,"%d",10);
		}else if(BRTS == L1){
			sprintf(buf,"%d",25);
		}else if(BRTS == L2){
			sprintf(buf,"%d",50);
		}else if(BRTS == L3){
			sprintf(buf,"%d",75);
		}else if(BRTS == L4){
			sprintf(buf,"%d",100);
		}		
		LCD_DisplayStringLine(130,170,(uint8_t*)buf);
		if(DIM == DOFF)
		{
			LCD_DisplayStringLine(174,170,"关闭");
		}else if(DIM == D5){
			sprintf(buf,"%d",5);
			LCD_DisplayStringLine(172,170,(uint8_t*)buf);
		}else if(DIM == D10){
			sprintf(buf,"%d",10);
			LCD_DisplayStringLine(172,170,(uint8_t*)buf);
		}else if(DIM == D15){
			sprintf(buf,"%d",15);
			LCD_DisplayStringLine(172,170,(uint8_t*)buf);
		}else if(DIM == D30){
			sprintf(buf,"%d",30);
			LCD_DisplayStringLine(172,170,(uint8_t*)buf);
		}
		if(TOUCH == op_on)
		{
			LCD_DisplayStringLine(214,170,"打开");
		}else{
			LCD_DisplayStringLine(214,170,"关闭");
		}
	}else{
		LCD_DisplayStringLine(50,170,"ENGLISH");
		LCD_SetTextColor(LCD_COLOR_YELLOW);
		LCD_SetBackColor(LCD_COLOR_BACK);
		if(BRTS == L0)
		{
			brightness = 5;
			sprintf(buf,"%d",10);
		}else if(BRTS == L1){
			brightness = 15;
			sprintf(buf,"%d",25);
		}else if(BRTS == L2){
			brightness = 30;
			sprintf(buf,"%d",50);
		}else if(BRTS == L3){
			brightness = 45;
			sprintf(buf,"%d",75);
		}else if(BRTS == L4){
			brightness = 60;
			sprintf(buf,"%d",100);
		}		
		LCD_DisplayStringLine(130,170,(uint8_t*)buf);
		if(DIM == DOFF)
		{
			LCD_DisplayStringLine(172,170,"OFF");
		}else if(DIM == D5){
			sprintf(buf,"%d",5);
			LCD_DisplayStringLine(172,170,(uint8_t*)buf);
		}else if(DIM == D10){
			sprintf(buf,"%d",10);
			LCD_DisplayStringLine(172,170,(uint8_t*)buf);
		}else if(DIM == D15){
			sprintf(buf,"%d",15);
			LCD_DisplayStringLine(172,170,(uint8_t*)buf);
		}else if(DIM == D30){
			sprintf(buf,"%d",30);
			LCD_DisplayStringLine(172,170,(uint8_t*)buf);
		}
		
		if(TOUCH == op_on)
		{
			LCD_DisplayStringLine(212,170,"ON");
		}else{
			LCD_DisplayStringLine(212,170,"OFF");
		}
	}
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	LCD_DisplayStringLine(130,230,"%");
	if(DIM == DOFF)
	{
		LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
		LCD_DrawFullRect(230,172,55,32);
	}else{
		LCD_DisplayStringLine(172,230,"Min");
	}
	
	if(LANG == chs)
	{
		DrawInstruction("系统语言选择");
	}else{
		DrawInstruction("Select system language");
	}
	page_flag = sysset;
	op_flag = set_lang;
}
