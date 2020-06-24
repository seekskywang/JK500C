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



void page_gset(void)
{
	u8 i,j;
	char buf[5];
  /*初始化后默认使用前景层*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*默认设置不透明	，该函数参数为不透明度，范围 0-0xff ，0为全透明，0xff为不透明*/
    LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BACK);
	/*经过LCD_SetLayer(LCD_FOREGROUND_LAYER)函数后，
	以下液晶操作都在前景层刷新，除非重新调用过LCD_SetLayer函数设置背景层*/
	
	DrawMenu();
	Drawgsethmenu();
	
	if(LANG == chs)
	{
		LCD_SetTextColor(LCD_COLOR_HLT);
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(0,10, "<      >");
		LCD_DisplayStringLine(5,26, "曲线设置");
		
		LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(50,10,"Y-上");
		LCD_DisplayStringLine(54,69,"限");
		LCD_DisplayStringLine(90,10,"Y-");
		LCD_DisplayStringLine(94,42,"下限");
		LCD_DisplayStringLine(134,10,"采样点");
//		LCD_DisplayStringLine(174,10,"通道状态");
	}else{
		LCD_SetTextColor(LCD_COLOR_HLT);
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(3,10, "<GRAPH SETUP>");
		
		LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(50,13,"Y-HIGH");
		LCD_DisplayStringLine(90,10,"Y-LOW");
		LCD_DisplayStringLine(130,10,"MULTI");
//		LCD_DisplayStringLine(170,10,"CHANNEL STATUS");
	}
	
	yhfocus_on(YHLIMIT);
	ylfocus_off(YLLIMIT);
	multifocus_off(MULTI);
	
	if(LANG == chs)
	{
		DrawInstruction("Y轴上限");
	}else{
		DrawInstruction("Y-High Value");
	}
	
//	LCD_SetBackColor(LCD_COLOR_BACK);
//	LCD_SetTextColor(LCD_COLOR_YELLOW);
//	for(j = 0;j < 4;j++)
//	{
//		for(i = 1 + j*10;i < 11 + j*10 ;i++)
//		{
//			sprintf(buf,"%03d",i);
//			DISP_CNL_M(210 + j*50,10+(i-j*10-1)*60,(uint8_t*)buf);
//		}
//	}
	
	
	page_flag = graphset;
	op_flag = set_yhigh;
	bit_flag = 1;
}
