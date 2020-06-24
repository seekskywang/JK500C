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

u8 passverify;
char SN[8];
char SearchBuffer[8];
const uint8_t calch[][3] =
{
	{"001"},
	{"002"},
	{"003"},
	{"004"},
	{"005"},
	{"006"},
	{"007"},
	{"008"},
	{"009"},
	{"010"},
	{"011"},
	{"012"},
	{"013"},
	{"014"},
	{"015"},
	{"016"},
};

u8 caltype;

void page_factory(void)
{
	char buf[10];
  /*初始化后默认使用前景层*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*默认设置不透明	，该函数参数为不透明度，范围 0-0xff ，0为全透明，0xff为不透明*/
    LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BACK);
	/*经过LCD_SetLayer(LCD_FOREGROUND_LAYER)函数后，
	以下液晶操作都在前景层刷新，除非重新调用过LCD_SetLayer函数设置背景层*/
	
	if(passverify == 0)
	{
		input_pass("0");
		del_num();
	}else{
		DrawMenu();
		Drawsysinfomenu();

		LCD_SetTextColor(LCD_COLOR_HLT);  		
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(0,10, "<      >");
		LCD_DisplayStringLine(5,26, "出厂设置");
		
		LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
		LCD_SetBackColor(LCD_COLOR_BACK);

		LCD_DisplayStringLine(54,10,"通道数");
		LCD_DisplayStringLine(94,10,"仪器序列号");
		LCD_DisplayStringLine(134,10,"校准通道");
		LCD_DisplayStringLine(174,10,"传感器类型");
	
		
		Disp_Factory();
	
	}
	page_flag = factory;
}

void Disp_Factory(void)
{
	char buf[10];
	
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_SetBackColor(LCD_COLOR_BACK);
	sprintf(buf,"%03d",CHNUM);
	LCD_DisplayStringLine(50,170,(uint8_t *)buf);
	LCD_DisplayStringLine(90,170,(uint8_t *)SN);
	
}