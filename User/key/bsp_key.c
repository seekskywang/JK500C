/**
  ******************************************************************************
  * @file    bsp_key.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   按键应用bsp（扫描模式）
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./key/bsp_key.h" 
#include "./lcd/bsp_lcd.h"
#include "./beep/bsp_beep.h"
#include "./flash/bsp_spi_flash.h"
#include "./tim/bsp_basic_tim.h"
#include "./RTC/bsp_rtc.h"
#include "./ch376/ch376.h"
#include "./FILESYS/FILESYS.h"
#include "jk508.h"
#include "./usart/bsp_debug_usart.h"
#include "sdio/bsp_sdio_sd.h"

u8 key_value;
extern u16 count;
u8 count_flag = 0;
u8 page_flag;
u8 op_flag;
u8 ch_page;
u8 tc_type;
u8 op_sw;
u8 spt_page;
u8 cor_page;
u8 date_page;
u8 time_page;
u8 bit_flag = 1;
u8 dot_flag = 0;
u8 neg_flag = 0;
u8 input_flag = 0;
u16 hpage;
char data[17];
float offset[40];
u8 keystat;
u8 date_time[6] = {0,1,1,0,0,0};
u8 tcpage;
u8 fileflag;
u8 tcflag = 0;
SD_Error watchres;
extern union 
{
   unsigned char FLAG_VAL;
		
   struct 
   {
       unsigned char  SW1:1;
       unsigned char  SW2:1;
       unsigned char  SW3:1;
       unsigned char  SW4:1;
       unsigned char  SW5:1;
       unsigned char  SW6:1;
       unsigned char  SW7:1;
       unsigned char  SW8:1; 
   }BIT_FLAG;
}FLAG1;

extern union 
{
   unsigned char FLAG_VAL;
		
   struct 
   {
       unsigned char  SW9:1;
       unsigned char  SW10:1;
       unsigned char  SW11:1;
       unsigned char  SW12:1;
       unsigned char  SW13:1;
       unsigned char  SW14:1;
       unsigned char  SW15:1;
       unsigned char  SW16:1;  
   }BIT_FLAG;
}FLAG2;

extern union 
{
   unsigned char FLAG_VAL;
		
   struct 
   {
       unsigned char  SW17:1;
       unsigned char  SW18:1;
       unsigned char  SW19:1;
       unsigned char  SW20:1;
       unsigned char  SW21:1;
       unsigned char  SW22:1;
       unsigned char  SW23:1;
       unsigned char  SW24:1;  
   }BIT_FLAG;
}FLAG3;

extern union 
{
   unsigned char FLAG_VAL;
		
   struct 
   {
       unsigned char  SW25:1;
       unsigned char  SW26:1;
       unsigned char  SW27:1;
       unsigned char  SW28:1;
       unsigned char  SW29:1;
       unsigned char  SW30:1;
       unsigned char  SW31:1;
       unsigned char  SW32:1;
   }BIT_FLAG;
}FLAG4;

extern union 
{
   unsigned char FLAG_VAL;
		
   struct 
   {
       unsigned char  SW33:1;
       unsigned char  SW34:1;
       unsigned char  SW35:1;
       unsigned char  SW36:1;
       unsigned char  SW37:1;
       unsigned char  SW38:1;
       unsigned char  SW39:1;	
       unsigned char  SW40:1;
   }BIT_FLAG;
}FLAG5;

extern union 
{
   unsigned char FLAG_VAL6;
	
   struct 
   {
       unsigned char  TC:4;    //热电偶类型
       unsigned char  FT:1;    //分选/比较
       unsigned char  BP:1;    //讯响
       unsigned char  UT:2;	   //单位
   }BIT_FLAG;
}FLAG6;

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

const uint8_t TC_Menu[][6]=
{
	{"TC-T"},
	{"TC-K"},
	{"TC-J"},
	{"TC-N"},
	{"TC-E"},
	{"TC-S"},
	{"TC-R"},
	{"TC-B"},
	{"PT100"},	
	{"    "},
	{"    "},
	{"    "},
//	{"PT"},
//	{"1~5V"},
//	{" "},
//	{"1V"},
//	{"1V"},
//	{"1V"},
//	{"18"},
//	{"AM"},
};


/// 不精确的延时
void Key_Delay(__IO u32 nCount)
{
	for(; nCount != 0; nCount--);
}


 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
//static void NVIC_Configuration(void)
//{
//  NVIC_InitTypeDef NVIC_InitStructure;
//  
//  /* 配置NVIC为优先级组1 */
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
//  
//  /* 配置中断源：按键1 */
//  NVIC_InitStructure.NVIC_IRQChannel = P6_EXTI_IRQ;
//  /* 配置抢占优先级：1 */
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  /* 配置子优先级：1 */
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//  /* 使能中断通道 */
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//  
//}


void DrawType(u8 p)
{
	u8 i;
	const u8 (*pt)[6];
	pt = TC_Menu;
	
	DrawMenu();//重绘菜单栏
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BLACK);
	for(i=0;i<4;i++)
	{
		LCD_DisplayStringLine(450,35+i*120,(uint8_t *)pt[i+4*(p-1)]);
	}

	if(LANG == chs)
	{
		LCD_DisplayStringLine(450,550,"更多");
	}else{
		LCD_DisplayStringLine(450,550,"MORE");
	}
	tcpage = p;
}

/*得到焦点*/
void focus_on(u8 flag)
{
	if(flag == ch_on)
	{
		LCD_SetBackColor(LCD_COLOR_HLT);
		LCD_SetTextColor(LCD_COLOR_BLACK);
	}else{
		LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
		LCD_SetTextColor(LCD_COLOR_BLACK);
	}	
}

/*失去焦点*/
void focus_off(u8 flag)
{
	switch(page_flag)
	{
		case display:
		{
			if(flag == ch_on)
			{	
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_HLT);
			}else{
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			}
		}break;
		
		case settings:
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_YELLOW);
		}break;
	}
}

void focus_on1(void)
{
	switch(page_flag)
	{		
		case settings:
		{			
			LCD_SetBackColor(LCD_COLOR_YELLOW);
			LCD_SetTextColor(LCD_COLOR_BLACK);
		}break;
		case sysset:
		{
			LCD_SetBackColor(LCD_COLOR_YELLOW);
			LCD_SetTextColor(LCD_COLOR_BLACK);
		}break;
		case display:
		{
			LCD_SetBackColor(LCD_COLOR_YELLOW);
			LCD_SetTextColor(LCD_COLOR_BLACK);
		}break;
	}
}

void focus_off1(void)
{
	switch(page_flag)
	{		 
		case settings:
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_YELLOW);
		}break;
		case sysset:
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_YELLOW);
		}break;
		case display:
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_YELLOW);
		}break;
	}
}

void minfocus_on(u8 c,float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(153,40+c*40,96,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,201,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,185,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,185,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,169,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,169,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
	}
	if(LANG == chs)
	{
		DrawInstruction("下限设置");
	}else{
		DrawInstruction("Low limit");
	}
	clear_input();
}

void minfocus_off(u8 c,float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(153,40+c*40,96,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,201,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,185,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,185,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,169,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,169,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
	}
}
void cal_disp(u8 c,float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
//	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
//	LCD_DrawFullRect(153,40+c*40,96,31);
//	LCD_SetTextColor(LCD_COLOR_YELLOW);
//	LCD_SetBackColor(LCD_COLOR_BACK);
	
//	if(i == 0)
//	{
//		if(lmt >= 0){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,201,(uint8_t* )buf);
//		}else{
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,185,(uint8_t* )buf);
//		}
//	}else if(i >=1 && i < 10){
//		if(lmt >= 0){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,185,(uint8_t* )buf);
//		}else{
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,169,(uint8_t* )buf);
//		}
//	}else if(i >=10 && i < 100){
//		if(lmt >= 0){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,169,(uint8_t* )buf);
//		}else{
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
//		}
//	}else if(i >=100 && i < 1000){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
//	}
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			strcat(buf,"   ");
			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			strcat(buf,"  ");
			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			strcat(buf,"  ");
			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			strcat(buf," ");
			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			strcat(buf," ");
			LCD_DisplayStringLine(40+c*40,169,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
	}
}

void maxfocus_on(u8 c,float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(323,40+c*40,96,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,371,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,355,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,355,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,339,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,339,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,323,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,323,(uint8_t* )buf);
	}
	if(LANG == chs)
	{
		DrawInstruction("上限设置");
	}else{
		DrawInstruction("High limit");
	}		
	clear_input();
}

void maxfocus_off(u8 c,float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(323,40+c*40,96,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,371,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,355,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,355,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,339,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,339,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,323,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,323,(uint8_t* )buf);
	}
}

void yhfocus_on(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(200,50,96,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,248,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,232,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,232,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,216,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,216,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,200,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,200,(uint8_t* )buf);
	}
	if(LANG == chs)
	{
		DrawInstruction("Y轴上限");
	}else{
		DrawInstruction("Y-High Value");
	}		
	clear_input();
}

void yhfocus_off(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(200,50,96,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,248,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,232,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,232,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,216,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,216,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,200,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,200,(uint8_t* )buf);
	}
}

void ylfocus_on(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(200,90,96,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,248,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,232,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,232,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,216,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,216,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,200,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,200,(uint8_t* )buf);
	}
	if(LANG == chs)
	{
		DrawInstruction("Y轴下限");
	}else{
		DrawInstruction("Y-Low Value");
	}		
	clear_input();
}

void ylfocus_off(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(200,90,96,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,248,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,232,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,232,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,216,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,216,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,200,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,200,(uint8_t* )buf);
	}
}

void multifocus_on(int lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(200,130,96,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(i == 0)
	{		
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(130,280,(uint8_t* )buf);
	
	}else if(i >=1 && i < 10){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(130,264,(uint8_t* )buf);
	}else if(i >=10 && i < 100){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(130,248,(uint8_t* )buf);
	}
	if(LANG == chs)
	{
		DrawInstruction("采样数");
	}else{
		DrawInstruction("Multi Value");
	}		
	clear_input();
}

void multifocus_off(int lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(200,130,96,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	if(i == 0)
	{		
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(130,280,(uint8_t* )buf);
	}else if(i >=1 && i < 10){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(130,264,(uint8_t* )buf);
	}else if(i >=10 && i < 100){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(130,248,(uint8_t* )buf);
	}
}

void stimefocus_on(int lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(118,317,48,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(i == 0)
	{		
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(317,150,(uint8_t* )buf);
	
	}else if(i >=1 && i < 10){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(317,134,(uint8_t* )buf);
	}else if(i >=10 && i < 100){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(317,118,(uint8_t* )buf);
	}
	if(LANG == chs)
	{
		DrawInstruction("记录间隔时间");
	}else{
		DrawInstruction("Record interval");
	}		
	clear_input();
}

void stimefocus_off(int lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(118,317,48,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	if(i == 0)
	{		
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(317,150,(uint8_t* )buf);
	}else if(i >=1 && i < 10){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(317,134,(uint8_t* )buf);
	}else if(i >=10 && i < 100){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(317,118,(uint8_t* )buf);
	}
}

//输入数字
void input_num(char* num)
{
	if(bit_flag < 18){
		if(bit_flag == 1)
		{
			LCD_SetColors(LCD_COLOR_LIGHTGREY,LCD_COLOR_LIGHTGREY);
			LCD_DrawRect(224,405,250,30);
			LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_WHITE);
			LCD_DrawFullRect(225,406,249,29);
			
			LCD_SetColors(LCD_COLOR_LIGHTGREY,LCD_COLOR_LIGHTGREY);
			LCD_DrawRect(312,408,150,24); 
			LCD_SetBackColor(LCD_COLOR_WHITE);
			LCD_SetTextColor(LCD_COLOR_BLACK);
			DISP_CNL_S(409,240,"输入数据");
		}
		
		strcat(data,num);
		input_flag = 1;
		if(dot_flag != 0 && strcmp(num,".") == 0)
		{
			
		}else{			
			LCD_SetBackColor(LCD_COLOR_WHITE);
			LCD_SetTextColor(LCD_COLOR_WHITE);
			DISP_CNL_S(410,314 + bit_flag*8," ");
			LCD_SetBackColor(LCD_COLOR_WHITE);
			LCD_SetTextColor(LCD_COLOR_BLACK);
			DISP_CNL_S(410,306 + bit_flag*8,(uint8_t*)num);
			LCD_SetBackColor(LCD_COLOR_BLACK);
			LCD_SetTextColor(LCD_COLOR_BLACK);
			DISP_CNL_S(410,314 + bit_flag*8," ");
			
			if(dot_flag == 0 && strcmp(num,".") == 0)
			{
				dot_flag = bit_flag;
			}
			bit_flag ++;
		}
	}
}

//输入密码
void input_pass(char* num)
{
	if(bit_flag < 18){
		if(bit_flag == 1)
		{
			LCD_SetColors(LCD_COLOR_LIGHTGREY,LCD_COLOR_LIGHTGREY);
			LCD_DrawRect(224,405,250,30);
			LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_WHITE);
			LCD_DrawFullRect(225,406,249,29);
			
			LCD_SetColors(LCD_COLOR_LIGHTGREY,LCD_COLOR_LIGHTGREY);
			LCD_DrawRect(312,408,150,24); 
			LCD_SetBackColor(LCD_COLOR_WHITE);
			LCD_SetTextColor(LCD_COLOR_BLACK);
			DISP_CNL_S(409,240,"输入密码");
		}
		
		strcat(data,num);
		input_flag = 1;
		if(dot_flag != 0 && strcmp(num,".") == 0)
		{
			
		}else{			
			LCD_SetBackColor(LCD_COLOR_WHITE);
			LCD_SetTextColor(LCD_COLOR_WHITE);
			DISP_CNL_S(410,314 + bit_flag*8," ");
			LCD_SetBackColor(LCD_COLOR_WHITE);
			LCD_SetTextColor(LCD_COLOR_BLACK);
			DISP_CNL_S(410,306 + bit_flag*8,(uint8_t*)num);
			LCD_SetBackColor(LCD_COLOR_BLACK);
			LCD_SetTextColor(LCD_COLOR_BLACK);
			DISP_CNL_S(410,314 + bit_flag*8," ");
			
			if(dot_flag == 0 && strcmp(num,".") == 0)
			{
				dot_flag = bit_flag;
			}
			bit_flag ++;
		}
	}
}

//输入序列号
void input_sn(char* num)
{
	if(bit_flag < 9){
		if(bit_flag == 1)
		{
			LCD_SetColors(LCD_COLOR_LIGHTGREY,LCD_COLOR_LIGHTGREY);
			LCD_DrawRect(224,405,250,30);
			LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_WHITE);
			LCD_DrawFullRect(225,406,249,29);
			
			LCD_SetColors(LCD_COLOR_LIGHTGREY,LCD_COLOR_LIGHTGREY);
			LCD_DrawRect(312,408,150,24); 
			LCD_SetBackColor(LCD_COLOR_WHITE);
			LCD_SetTextColor(LCD_COLOR_BLACK);
			DISP_CNL_S(409,240,"输入SN");
		}
		
		strcat(SN,num);
		input_flag = 1;
		if(dot_flag != 0 && strcmp(num,".") == 0)
		{
			
		}else{			
			LCD_SetBackColor(LCD_COLOR_WHITE);
			LCD_SetTextColor(LCD_COLOR_WHITE);
			DISP_CNL_S(410,314 + bit_flag*8," ");
			LCD_SetBackColor(LCD_COLOR_WHITE);
			LCD_SetTextColor(LCD_COLOR_BLACK);
			DISP_CNL_S(410,306 + bit_flag*8,(uint8_t*)num);
			LCD_SetBackColor(LCD_COLOR_BLACK);
			LCD_SetTextColor(LCD_COLOR_BLACK);
			DISP_CNL_S(410,314 + bit_flag*8," ");
			
			if(dot_flag == 0 && strcmp(num,".") == 0)
			{
				dot_flag = bit_flag;
			}
			bit_flag ++;
		}
	}
}

//输入序列号
void input_search(char* num)
{
	if(bit_flag < 9){
		if(bit_flag == 1)
		{
			memset(SearchBuffer,0,8);
			LCD_SetColors(LCD_COLOR_LIGHTGREY,LCD_COLOR_LIGHTGREY);
			LCD_DrawRect(224,405,250,30);
			LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_WHITE);
			LCD_DrawFullRect(225,406,249,29);
			
			LCD_SetColors(LCD_COLOR_LIGHTGREY,LCD_COLOR_LIGHTGREY);
//			LCD_DrawRect(312,408,150,24); 
			LCD_DrawRect(312,408,20,24);
			LCD_DrawRect(352,408,20,24);
			LCD_DrawRect(392,408,20,24); 
			LCD_DrawRect(432,408,20,24);  			
			
			LCD_SetBackColor(LCD_COLOR_WHITE);
			LCD_SetTextColor(LCD_COLOR_BLACK);
			DISP_CNL_S(411,230,"输入时间");
			DISP_CNL_S(410,295,"20");
			DISP_CNL_S(411,334,"年");
			DISP_CNL_S(411,374,"月");
			DISP_CNL_S(411,414,"日");
			DISP_CNL_S(411,454,"时");
		}
		
		
		input_flag = 1;
		if(dot_flag != 0 && strcmp(num,".") == 0)
		{
			
		}else{
			strcat(SearchBuffer,num);
			if(bit_flag < 3)//年
			{
				LCD_SetBackColor(LCD_COLOR_WHITE);
				LCD_SetTextColor(LCD_COLOR_BLACK);
				DISP_CNL_S(410,306 + bit_flag*8,(uint8_t*)num);
				if(bit_flag < 2)
				{
					LCD_SetBackColor(LCD_COLOR_WHITE);
					LCD_SetTextColor(LCD_COLOR_WHITE);
					DISP_CNL_S(410,314 + bit_flag*8," ");		
					LCD_SetBackColor(LCD_COLOR_BLACK);
					LCD_SetTextColor(LCD_COLOR_BLACK);
					DISP_CNL_S(410,314 + bit_flag*8," ");
				}else if(bit_flag == 2){
					LCD_SetBackColor(LCD_COLOR_WHITE);
					LCD_SetTextColor(LCD_COLOR_WHITE);
					DISP_CNL_S(410,354," ");		
					LCD_SetBackColor(LCD_COLOR_BLACK);
					LCD_SetTextColor(LCD_COLOR_BLACK);
					DISP_CNL_S(410,354," ");
				}
			}else if(bit_flag >= 3 && bit_flag < 5){//月
				LCD_SetBackColor(LCD_COLOR_WHITE);
				LCD_SetTextColor(LCD_COLOR_BLACK);
				DISP_CNL_S(410,346 + (bit_flag-2)*8,(uint8_t*)num);
				if(bit_flag < 4)
				{
					LCD_SetBackColor(LCD_COLOR_WHITE);
					LCD_SetTextColor(LCD_COLOR_WHITE);
					DISP_CNL_S(410,354 + (bit_flag-2)*8," ");				
					LCD_SetBackColor(LCD_COLOR_BLACK);
					LCD_SetTextColor(LCD_COLOR_BLACK);
					DISP_CNL_S(410,354 + (bit_flag-2)*8," ");
				}else if(bit_flag == 4){
					LCD_SetBackColor(LCD_COLOR_WHITE);
					LCD_SetTextColor(LCD_COLOR_WHITE);
					DISP_CNL_S(410,394," ");		
					LCD_SetBackColor(LCD_COLOR_BLACK);
					LCD_SetTextColor(LCD_COLOR_BLACK);
					DISP_CNL_S(410,394," ");
				}
			}else if(bit_flag >= 5 && bit_flag < 7){//日
				LCD_SetBackColor(LCD_COLOR_WHITE);
				LCD_SetTextColor(LCD_COLOR_BLACK);
				DISP_CNL_S(410,386 + (bit_flag-4)*8,(uint8_t*)num);
				if(bit_flag < 6)
				{
					LCD_SetBackColor(LCD_COLOR_WHITE);
					LCD_SetTextColor(LCD_COLOR_WHITE);
					DISP_CNL_S(410,394 + (bit_flag-4)*8," ");				
					LCD_SetBackColor(LCD_COLOR_BLACK);
					LCD_SetTextColor(LCD_COLOR_BLACK);
					DISP_CNL_S(410,394 + (bit_flag-4)*8," ");
				}else if(bit_flag == 6){
					LCD_SetBackColor(LCD_COLOR_WHITE);
					LCD_SetTextColor(LCD_COLOR_WHITE);
					DISP_CNL_S(410,434," ");		
					LCD_SetBackColor(LCD_COLOR_BLACK);
					LCD_SetTextColor(LCD_COLOR_BLACK);
					DISP_CNL_S(410,434," ");
				}
			}else if(bit_flag >= 7 && bit_flag < 9){//时
				LCD_SetBackColor(LCD_COLOR_WHITE);
				LCD_SetTextColor(LCD_COLOR_BLACK);
				DISP_CNL_S(410,426 + (bit_flag-6)*8,(uint8_t*)num);
				if(bit_flag < 8)
				{
					LCD_SetBackColor(LCD_COLOR_WHITE);
					LCD_SetTextColor(LCD_COLOR_WHITE);
					DISP_CNL_S(410,434 + (bit_flag-6)*8," ");
					
					LCD_SetBackColor(LCD_COLOR_BLACK);
					LCD_SetTextColor(LCD_COLOR_BLACK);
					DISP_CNL_S(410,434 + (bit_flag-6)*8," ");
				}
			}
			
			if(dot_flag == 0 && strcmp(num,".") == 0)
			{
				dot_flag = bit_flag;
			}
			bit_flag ++;
		}
	}
}

//清除输入框
void clear_input(void)
{
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(224,405,251,31);
	bit_flag = 1;
	dot_flag = 0;
	input_flag = 0;
	memset(data, '\0', sizeof(data));
}

//删除数字
void del_num(void)
{
	if(bit_flag > 1)
	{
		LCD_SetBackColor(LCD_COLOR_WHITE);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		DISP_CNL_S(410,306 + bit_flag*8," ");
		LCD_SetBackColor(LCD_COLOR_BLACK);
		LCD_SetTextColor(LCD_COLOR_BLACK);
		DISP_CNL_S(410,298 + bit_flag*8," ");
		bit_flag --;
		data[bit_flag-1] = '\0';
		if(bit_flag == dot_flag)
		{
			dot_flag = 0;
		}
		if(bit_flag == 2)
		{
			neg_flag = 0;
		}
	}else if(bit_flag == 1){
		input_num("-");
		neg_flag = 1;
	}
}
//删除序列号
void del_sn(void)
{
	if(bit_flag > 1)
	{
		LCD_SetBackColor(LCD_COLOR_WHITE);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		DISP_CNL_S(410,306 + bit_flag*8," ");
		LCD_SetBackColor(LCD_COLOR_BLACK);
		LCD_SetTextColor(LCD_COLOR_BLACK);
		DISP_CNL_S(410,298 + bit_flag*8," ");
		bit_flag --;
		SN[bit_flag-1] = '\0';
		if(bit_flag == dot_flag)
		{
			dot_flag = 0;
		}
		if(bit_flag == 2)
		{
			neg_flag = 0;
		}
	}else if(bit_flag == 1){
		input_num("-");
		neg_flag = 1;
	}
}
//设置日期

void plus_year(void)
{
//	RTC_DateTypeDef RTC_DateStructure;
		
	
	YEAR = YEAR + 1;
	if(YEAR > 99)
	{
		YEAR = 0;
	}
	RTC_DateStructure.RTC_Date = DATE;
	RTC_DateStructure.RTC_Month = MONTH;
	RTC_DateStructure.RTC_Year = YEAR;
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void minus_year(void)
{
//	RTC_DateTypeDef RTC_DateStructure;
		
	
	YEAR = YEAR - 1;
	if(YEAR < 1)
	{
		YEAR = 1;
	}   
	RTC_DateStructure.RTC_Date = DATE;
	RTC_DateStructure.RTC_Month = MONTH;
	RTC_DateStructure.RTC_Year = YEAR;
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void plus_mon(void)
{
//	RTC_DateTypeDef RTC_DateStructure;
	
	MONTH++;
	if(MONTH > 12)
	{
		MONTH = 1;
	}    
	RTC_DateStructure.RTC_Date = DATE;
	RTC_DateStructure.RTC_Month = MONTH;         
	RTC_DateStructure.RTC_Year = YEAR;
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void minus_mon(void)
{
//	RTC_DateTypeDef RTC_DateStructure;
	
	MONTH--;
	if(MONTH <1)
	{
		MONTH = 12;
	}    
	RTC_DateStructure.RTC_Date = DATE;
	RTC_DateStructure.RTC_Month = MONTH;         
	RTC_DateStructure.RTC_Year = YEAR;
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void plus_day(void)
{
//	RTC_DateTypeDef RTC_DateStructure;
	
	DATE++;
	if(DATE > 31)
	{
		DATE = 1;
	}    
	RTC_DateStructure.RTC_Date = DATE;
	RTC_DateStructure.RTC_Month = MONTH;         
	RTC_DateStructure.RTC_Year = YEAR;
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void minus_day(void)
{
//	RTC_DateTypeDef RTC_DateStructure;
	
	DATE--;
	if(DATE < 1)
	{
		DATE = 31;
	}
	RTC_DateStructure.RTC_Date = DATE;
	RTC_DateStructure.RTC_Month = MONTH;         
	RTC_DateStructure.RTC_Year = YEAR;
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void plus_hour(void)
{
//	RTC_TimeTypeDef RTC_TimeStructure;
	
	HOURS++;
	if(HOURS > 23)
	{
		HOURS = 0;
	}
	
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = HOURS;        
	RTC_TimeStructure.RTC_Minutes = MINUTES;      
	RTC_TimeStructure.RTC_Seconds = SECONDS;      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void minus_hour(void)
{
//	RTC_TimeTypeDef RTC_TimeStructure;
	
	HOURS--;
	if(HOURS == 255)
	{
		HOURS = 23;
	}
	
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = HOURS;        
	RTC_TimeStructure.RTC_Minutes = MINUTES;      
	RTC_TimeStructure.RTC_Seconds = SECONDS;      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void plus_min(void)
{
//	RTC_TimeTypeDef RTC_TimeStructure;
	
	MINUTES++;
	if(MINUTES > 59)
	{
		MINUTES = 0;
	}
	
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = HOURS;        
	RTC_TimeStructure.RTC_Minutes = MINUTES;      
	RTC_TimeStructure.RTC_Seconds = SECONDS;      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void minus_min(void)
{
//	RTC_TimeTypeDef RTC_TimeStructure;
	
	MINUTES--;
	if(MINUTES == 255)
	{
		MINUTES = 59;
	}
	
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = HOURS;        
	RTC_TimeStructure.RTC_Minutes = MINUTES;      
	RTC_TimeStructure.RTC_Seconds = SECONDS;      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void plus_sec(void)
{
//	RTC_TimeTypeDef RTC_TimeStructure;
	
	SECONDS++;
	if(SECONDS > 59)
	{
		SECONDS = 0;
	}
	
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = HOURS;        
	RTC_TimeStructure.RTC_Minutes = MINUTES;      
	RTC_TimeStructure.RTC_Seconds = SECONDS;      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void minus_sec(void)
{
//	RTC_TimeTypeDef RTC_TimeStructure;
	
	SECONDS--;
	if(SECONDS == 255)
	{
		SECONDS = 59;
	}
	
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = HOURS;        
	RTC_TimeStructure.RTC_Minutes = MINUTES;      
	RTC_TimeStructure.RTC_Seconds = SECONDS;      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

/*背光设置*/
void brt_set(u8 key)
{
	DrawMenu();//重绘菜单栏
	Drawsysmenu();
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(170,130,55,32);
	if(press != 1)
	{
		focus_on1();
	}else{
		focus_off1();
	}
	if(key == 1)
	{
		LCD_DisplayStringLine(130,170,"10");
		BRTS = L0;
		brightness = 5;
	}else if(key == 2){
		LCD_DisplayStringLine(130,170,"25");
		BRTS = L1;
		brightness = 15;
	}else if(key == 3){
		LCD_DisplayStringLine(130,170,"50");
		BRTS = L2;
		brightness = 30;
	}else if(key == 4){
		LCD_DisplayStringLine(130,170,"75");
		BRTS = L3;
		brightness = 45;
	}else if(key == 5){
		LCD_DisplayStringLine(130,170,"100");
		BRTS = L4;
		brightness = 60;
	}
	TIM_PWMOUTPUT_Config(brightness);
	op_sw = op_off;
	Save_flag();
}

/*降低亮度设置*/
void dim_set(u8 key)
{
	DrawMenu();//重绘菜单栏
	Drawsysmenu();
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(170,172,55,32);
	if(press != 1)
	{
		focus_on1();
	}else{
		focus_off1();
	}
	if(LANG == chs)
	{
		if(key == 1)
		{
			LCD_DisplayStringLine(174,170,"关闭");
			LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
			LCD_DrawFullRect(230,172,55,32);
			DIM = DOFF;
		}else if(key == 2){
			LCD_DisplayStringLine(172,170,"5");
			LCD_SetTextColor(LCD_COLOR_YELLOW);
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_DisplayStringLine(172,230,"Min");
			DIM = D5;
		}else if(key == 3){
			LCD_DisplayStringLine(172,170,"10");
			LCD_SetTextColor(LCD_COLOR_YELLOW);
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_DisplayStringLine(172,230,"Min");
			DIM = D10;
		}else if(key == 4){
			LCD_DisplayStringLine(172,170,"15");
			LCD_SetTextColor(LCD_COLOR_YELLOW);
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_DisplayStringLine(172,230,"Min");
			DIM = D15;
		}else if(key == 5){
			LCD_DisplayStringLine(172,170,"30");
			LCD_SetTextColor(LCD_COLOR_YELLOW);
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_DisplayStringLine(172,230,"Min");
			DIM = D30;
		}
	}else if(LANG == eng){
		if(key == 1)
		{
			LCD_DisplayStringLine(174,170,"OFF");
			LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
			LCD_DrawFullRect(230,172,55,32);
			DIM = DOFF;
		}else if(key == 2){
			LCD_DisplayStringLine(174,170,"5");
			LCD_SetTextColor(LCD_COLOR_YELLOW);
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_DisplayStringLine(172,230,"Min");
			DIM = D5;
		}else if(key == 3){
			LCD_DisplayStringLine(174,170,"10");
			LCD_SetTextColor(LCD_COLOR_YELLOW);
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_DisplayStringLine(172,230,"Min");
			DIM = D10;
		}else if(key == 4){
			LCD_DisplayStringLine(174,170,"15");
			LCD_SetTextColor(LCD_COLOR_YELLOW);
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_DisplayStringLine(172,230,"Min");
			DIM = D15;
		}else if(key == 5){
			LCD_DisplayStringLine(174,170,"30");
			LCD_SetTextColor(LCD_COLOR_YELLOW);
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_DisplayStringLine(172,230,"Min");
			DIM = D30;
		}
	}
	op_sw = op_off;
	Save_flag();
}
/*触摸屏设置*/
void touch_set(u8 key)
{
	DrawMenu();//重绘菜单栏
	Drawsysmenu();
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(170,210,55,32);
	if(press != 1)
	{
		focus_on1();
	}else{
		focus_off1();
	}
	if(LANG == chs)
	{
		if(key == 1)
		{
			LCD_DisplayStringLine(214,170,"打开");
			TOUCH = op_on;
		}else if(key == 2){
			LCD_DisplayStringLine(214,170,"关闭");
			TOUCH = op_off;
		}
	}else if(LANG == eng){
		if(key == 1)
		{
			LCD_DisplayStringLine(212,170,"ON");
			TOUCH = op_on;
		}else if(key == 2){
			LCD_DisplayStringLine(212,170,"OFF");
			TOUCH = op_off;
		}
	}
	op_sw = op_off;
	Save_flag();
}

void Search_Bit(void)
{
	u16 i;
	u32 ymd;
	u8 y1,y2,y3,m1,d1;
	y1 = (data[0]-48)*10+(data[1]-48);
	y2 = (data[2]-48)*10+(data[3]-48);
	m1 = (data[4]-48)*10+data[5]-48;
	d1 = (data[6]-48)*10+data[7]-48; 
	char timetemp[100];
	if(bit_flag == 9)
	{		
		for(i=1;i<1000;i++)
		{
			Read_time(i);
			if(y1 == histime[0][0] && y2 == histime[0][1] && m1 == histime[0][2] && d1 == histime[0][3])
			{
				clear_input();
				hpage = i;
//				hispage(hpage);
				break;
			}
		}
//		clear_input();
	}
	
}



//输入开始时间
void Input_start(u8 num)
{
	char buf[10];

	if(bit_flag == 1)
	{
		if(num > 2)
		{
			num = 2;
		}
		STARTH = num*10;
		bit_flag++;
	}else if( bit_flag == 2){
		STARTH = STARTH + num;
		if(STARTH > 23)
		{
			STARTH = 23;
		}
		bit_flag++;
	}else if(bit_flag == 3){
		if(num > 6)
		{
			num = 6;
		}
		STARTM = num*10;
		bit_flag++;
	}else if(bit_flag == 4){
		STARTM = STARTM + num;
		if(STARTM > 59)
		{
			STARTM = 59;
		}
		bit_flag = 1;
	}
	focus_on1();
	sprintf(buf,"%0.2d:%0.2d",
			STARTH,
			STARTM);
	LCD_DisplayStringLine(208,150,(uint8_t *)buf);
	Save_flag();
}

//输入结束时间
void Input_end(u8 num)
{
	char buf[10];

	if(bit_flag == 1)
	{
		if(num > 2)
		{
			num = 2;
		}
		ENDH = num*10;
		bit_flag++;
	}else if( bit_flag == 2){
		ENDH = ENDH + num;
		if(ENDH > 23)
		{
			ENDH = 23;
		}
		bit_flag++;
	}else if(bit_flag == 3){
		if(num > 6)
		{
			num = 6;
		}
		ENDM = num*10;
		bit_flag++;
	}else if(bit_flag == 4){
		ENDM = ENDM + num;
		if(ENDM > 59)
		{
			ENDM = 59;
		}
		bit_flag = 1;
	}
	focus_on1();
	sprintf(buf,"%0.2d:%0.2d",
			ENDH,
			ENDM);
	LCD_DisplayStringLine(248,150,(uint8_t *)buf);
	Save_flag();
}
/**
  * @brief  配置按键用到的I/O口
  * @param  无
  * @retval 无
  */
void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
//	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_AHB1PeriphClockCmd(P1_GPIO_CLK|P2_GPIO_CLK|P3_GPIO_CLK|P4_GPIO_CLK|P5_GPIO_CLK|P6_GPIO_CLK,ENABLE);	/*开启按键GPIO口的时钟*/
	
//	 /* 配置 NVIC */
//	NVIC_Configuration();
	  
	GPIO_InitStructure.GPIO_Pin = P1_PIN;/*选择按键的引脚*/     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;/*设置引脚为输出模式*/   
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;/*设置引脚的输出类型为推挽输出*/    
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; /*设置引脚为上拉模式*/	  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;/*设置引脚速率为2MHz */    
	GPIO_Init(P1_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/


  
	GPIO_InitStructure.GPIO_Pin = P2_PIN;/*选择按键的引脚*/       
	GPIO_Init(P2_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/

	
	GPIO_InitStructure.GPIO_Pin = P3_PIN;/*选择按键的引脚*/       
	GPIO_Init(P3_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/

	

	GPIO_InitStructure.GPIO_Pin = P4_PIN;/*选择按键的引脚*/       
	GPIO_Init(P4_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/

	
	GPIO_InitStructure.GPIO_Pin = P5_PIN;/*选择按键的引脚*/       
	GPIO_Init(P5_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/

	
	GPIO_InitStructure.GPIO_Pin = P6_PIN;/*选择按键的引脚*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;/*设置引脚为输入模式*/   
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;/*设置引脚下拉*/
	GPIO_Init(P6_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
	
//	/* 连接 EXTI 中断源 到P6引脚 */
//	SYSCFG_EXTILineConfig(P6_EXTI_PORTSOURCE,P6_EXTI_PINSOURCE);
//	
//	/* 选择 EXTI 中断源 */
//	EXTI_InitStructure.EXTI_Line = P6_EXTI_LINE;
//	/* 中断模式 */
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	/* 下降沿触发 */
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
//	/* 使能中断/事件线 */
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);
}

//扫描键值
void Key_Scan(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = P1_PIN;/*选择按键的引脚*/     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;/*设置引脚为输出模式*/   
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;/*设置引脚的输出类型为推挽输出*/    
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; /*设置引脚为上拉模式*/	  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;/*设置引脚速率为2MHz */    
	GPIO_Init(P1_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/


  
	GPIO_InitStructure.GPIO_Pin = P2_PIN;/*选择按键的引脚*/       
	GPIO_Init(P2_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/

	
	GPIO_InitStructure.GPIO_Pin = P3_PIN;/*选择按键的引脚*/
	GPIO_Init(P3_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/

	

	GPIO_InitStructure.GPIO_Pin = P4_PIN;/*选择按键的引脚*/       
	GPIO_Init(P4_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/

	
	GPIO_InitStructure.GPIO_Pin = P5_PIN;/*选择按键的引脚*/       
	GPIO_Init(P5_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/

	
	GPIO_InitStructure.GPIO_Pin = P6_PIN;/*选择按键的引脚*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;/*设置引脚为输入模式*/   
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;/*设置引脚下拉*/
	GPIO_Init(P6_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化          按键*/
	
	GPIO_SetBits(P1_GPIO_PORT,P1_PIN);/*P1脚置高*/
	GPIO_SetBits(P2_GPIO_PORT,P2_PIN);/*P2脚置高*/
	GPIO_SetBits(P3_GPIO_PORT,P3_PIN);/*P3脚置高*/
	GPIO_SetBits(P4_GPIO_PORT,P4_PIN);/*P4脚置高*/
	GPIO_SetBits(P5_GPIO_PORT,P5_PIN);/*P5脚置高*/
//	Key_Delay(0XFFFF);
	if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
	{
		GPIO_InitStructure.GPIO_Pin = P2_PIN;/*选择按键的引脚*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;/*设置引脚为输入模式*/   
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;/*设置引脚下拉*/
		GPIO_Init(P2_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P3_PIN;/*选择按键的引脚*/
		GPIO_Init(P3_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P4_PIN;/*选择按键的引脚*/
		GPIO_Init(P4_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P5_PIN;/*选择按键的引脚*/
		GPIO_Init(P5_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		Key_Delay(0XFF);
		if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
		{
			
//			Key_Delay(0x9FFFF);
			if(GPIO_ReadInputDataBit(P2_GPIO_PORT,P2_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P2_GPIO_PORT,P2_PIN))
				{
					key_value = 1;//ESC
				}
			}
			else if(GPIO_ReadInputDataBit(P3_GPIO_PORT,P3_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P3_GPIO_PORT,P3_PIN))
				{
					key_value = 2;//ENTER
				}
			}
			else if(GPIO_ReadInputDataBit(P4_GPIO_PORT,P4_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P4_GPIO_PORT,P4_PIN))
				{
					key_value = 3;//RIGHT
				}
			}
			else if(GPIO_ReadInputDataBit(P5_GPIO_PORT,P5_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P5_GPIO_PORT,P5_PIN))
				{
					key_value = 4;
				}
			}
			else if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
				{
					key_value = 5;//FUNC_5
				}
			}
		}
		GPIO_InitStructure.GPIO_Pin = P2_PIN;/*选择按键的引脚*/     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;/*设置引脚为输出模式*/   
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;/*设置引脚的输出类型为推挽输出*/    
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; /*设置引脚为上拉模式*/	  
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;/*设置引脚速率为2MHz */    
		GPIO_Init(P2_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_SetBits(P2_GPIO_PORT,P2_PIN);/*P2脚置高*/
		GPIO_InitStructure.GPIO_Pin = P1_PIN;/*选择按键的引脚*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;/*设置引脚为输入模式*/   
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;/*设置引脚下拉*/
		GPIO_Init(P1_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P3_PIN;/*选择按键的引脚*/
		GPIO_Init(P3_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P4_PIN;/*选择按键的引脚*/
		GPIO_Init(P4_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P5_PIN;/*选择按键的引脚*/
		GPIO_Init(P5_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
				
		Key_Delay(0xFF);
		if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
		{
			if(GPIO_ReadInputDataBit(P1_GPIO_PORT,P1_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P1_GPIO_PORT,P1_PIN))
				{
					key_value = 6;//FUNC_4
				}
			}			
			else if(GPIO_ReadInputDataBit(P3_GPIO_PORT,P3_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P3_GPIO_PORT,P3_PIN))
				{
					key_value = 7;//0
				}
				}
			else if(GPIO_ReadInputDataBit(P4_GPIO_PORT,P4_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P4_GPIO_PORT,P4_PIN))
				{
					key_value = 8;//DOWN
				}
			}
			else if(GPIO_ReadInputDataBit(P5_GPIO_PORT,P5_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P5_GPIO_PORT,P5_PIN))
				{
					key_value = 9;//dot
				}
			}
			else if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
				{
					key_value = 10;//BACKSPACE
				}
			}
		}
				
			
			
			
			
		GPIO_InitStructure.GPIO_Pin = P3_PIN;/*选择按键的引脚*/     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;/*设置引脚为输出模式*/   
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;/*设置引脚的输出类型为推挽输出*/    
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; /*设置引脚为上拉模式*/	  
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;/*设置引脚速率为2MHz */    
		GPIO_Init(P3_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_SetBits(P3_GPIO_PORT,P3_PIN);/*P2脚置高*/
		GPIO_InitStructure.GPIO_Pin = P1_PIN;/*选择按键的引脚*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;/*设置引脚为输入模式*/   
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;/*设置引脚下拉*/
		GPIO_Init(P1_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P2_PIN;/*选择按键的引脚*/
		GPIO_Init(P2_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P4_PIN;/*选择按键的引脚*/
		GPIO_Init(P4_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P5_PIN;/*选择按键的引脚*/
		GPIO_Init(P5_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		Key_Delay(0XFF);
		if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
		{
			if(GPIO_ReadInputDataBit(P1_GPIO_PORT,P1_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P1_GPIO_PORT,P1_PIN))
				{
					key_value = 11;//FUNC_3
				}
			}
			else if(GPIO_ReadInputDataBit(P2_GPIO_PORT,P2_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P2_GPIO_PORT,P2_PIN))
				{
					key_value = 12;//3
				}
			}
			else if(GPIO_ReadInputDataBit(P4_GPIO_PORT,P4_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P4_GPIO_PORT,P4_PIN))
				{
					key_value = 13;//BACKUP
				}
			}
			else if(GPIO_ReadInputDataBit(P5_GPIO_PORT,P5_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P5_GPIO_PORT,P5_PIN))
				{
					key_value = 14;//9
				}
			}
			else if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
				{
					key_value = 15;//6
				}
			}
		}
			
		GPIO_InitStructure.GPIO_Pin = P4_PIN;/*选择按键的引脚*/     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;/*设置引脚为输出模式*/   
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;/*设置引脚的输出类型为推挽输出*/    
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; /*设置引脚为上拉模式*/	  
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;/*设置引脚速率为2MHz */    
		GPIO_Init(P4_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_SetBits(P4_GPIO_PORT,P4_PIN);/*P2脚置高*/
		GPIO_InitStructure.GPIO_Pin = P1_PIN;/*选择按键的引脚*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;/*设置引脚为输入模式*/   
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;/*设置引脚下拉*/
		GPIO_Init(P1_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P2_PIN;/*选择按键的引脚*/
		GPIO_Init(P2_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P3_PIN;/*选择按键的引脚*/
		GPIO_Init(P3_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P5_PIN;/*选择按键的引脚*/
		GPIO_Init(P5_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		Key_Delay(0XFF);
			
		if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
		{
			if(GPIO_ReadInputDataBit(P1_GPIO_PORT,P1_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P1_GPIO_PORT,P1_PIN))
				{
					key_value = 16;//FUNC_2
				}
			}
			else if(GPIO_ReadInputDataBit(P2_GPIO_PORT,P2_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P2_GPIO_PORT,P2_PIN))
				{
					key_value = 17;//2
				}
			}
			else if(GPIO_ReadInputDataBit(P3_GPIO_PORT,P3_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P3_GPIO_PORT,P3_PIN))
				{
					key_value = 18;//5
				}
			}
			else if(GPIO_ReadInputDataBit(P5_GPIO_PORT,P5_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P5_GPIO_PORT,P5_PIN))
				{
					key_value = 19;//8
				}
			}
			else if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
				{
					key_value = 20;//UP
				}
			}
		}
			
		GPIO_InitStructure.GPIO_Pin = P5_PIN;/*选择按键的引脚*/     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;/*设置引脚为输出模式*/   
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;/*设置引脚的输出类型为推挽输出*/    
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; /*设置引脚为上拉模式*/	  
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;/*设置引脚速率为2MHz */    
		GPIO_Init(P5_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_SetBits(P5_GPIO_PORT,P5_PIN);/*P2脚置高*/
		GPIO_InitStructure.GPIO_Pin = P1_PIN;/*选择按键的引脚*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;/*设置引脚为输入模式*/   
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;/*设置引脚下拉*/
		GPIO_Init(P1_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P2_PIN;/*选择按键的引脚*/
		GPIO_Init(P2_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P3_PIN;/*选择按键的引脚*/
		GPIO_Init(P3_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P4_PIN;/*选择按键的引脚*/
		GPIO_Init(P4_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		Key_Delay(0XFF);
		if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
		{
			if(GPIO_ReadInputDataBit(P1_GPIO_PORT,P1_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P1_GPIO_PORT,P1_PIN))
				{
					key_value = 21;//FUNC_1
				}
			}
			else if(GPIO_ReadInputDataBit(P2_GPIO_PORT,P2_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P2_GPIO_PORT,P2_PIN))
				{
					key_value = 22;//1
				}
			}
			else if(GPIO_ReadInputDataBit(P3_GPIO_PORT,P3_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P3_GPIO_PORT,P3_PIN))
				{
					key_value = 23;//4
				}
			}
			else if(GPIO_ReadInputDataBit(P4_GPIO_PORT,P4_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P4_GPIO_PORT,P4_PIN))
				{
					key_value = 24;//LEFT
				}
			}
			else if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
				{
					key_value = 25;//7
				}
			}
		}

	}else{
		key_value = 0XFF;
	}
	
}
void Key_Handle(void)
{
	ButtonSound();//按键音
	switch(key_value)
	{
		case KEY_1:
		{
			KEY1_HANDLE();
		}
		break;
		case KEY_2:
		{
			KEY2_HANDLE();
		}
		break;
		case KEY_3:
		{
			KEY3_HANDLE();
		}
		break;
		case KEY_4:
		{
			KEY4_HANDLE();
		}
		break;
		case KEY_5:
		{
			KEY5_HANDLE();
		}
		break;
		case KEY_6:
		{
			KEY6_HANDLE();
		}
		break;
		case KEY_7:
		{
			KEY7_HANDLE();
		}
		break;
		case KEY_8:
		{
			KEY8_HANDLE();
		}
		break;
		case KEY_9:
		{
			KEY9_HANDLE();
		}
		break;
		case KEY_0:
		{
			KEY0_HANDLE();
		}
		break;
		case KEY_ACC:
		{
			ACC_HANDLE();
		}
		break;
		case KEY_BACK:
		{
			BACK_HANDLE();
		}
		break;
		case KEY_DOT:
		{
			DOT_HANDLE();
		}
		break;
		case KEY_UP:
		{
			UP_HANDLE();
		}
		break;
		case KEY_DOWN:
		{
			DOWN_HANDLE();
		}
		break;
		case KEY_LEFT:
		{
			LEFT_HANDLE();
		}
		break;
		case KEY_RIGHT:
		{
			RIGHT_HANDLE();
		}
		break;
		case KEY_ENTER:
		{
			ENTER_HANDLE();
			count_flag = 1;
		}
		break;
		case KEY_ESC:
		{
			ESC_HANDLE();
		}
		break;
		case KEY_FUNC1:
		{					
			FUNC1_HANDLE();
		}
		break;
		case KEY_FUNC2:
		{
			FUNC2_HANDLE();
		}
		break;
		case KEY_FUNC3:
		{
			FUNC3_HANDLE();
		}
		break;
		case KEY_FUNC4:
		{
			FUNC4_HANDLE();
		}
		break;
		case KEY_FUNC5:
		{
			FUNC5_HANDLE();
		}
		break;
		default:break;
	}
}
////按键功能
//void Key_Function(void)
//{	
//	static u8 key_old;
//	static u8 key_new;
//	static u8 longflag;
//	static u16 presscount;
//	key_new = key_value;
//	if(key_value != 0XFF)
//	{		
//		if(key_old != key_new && keystat == 0)
//		{
//			Key_Handle();
//			keystat = 1;
//			key_old = key_value;
//		}else if(key_old == key_new && keystat == 1){
//			presscount ++;
//			if(longflag == 0)
//			{
//				if(presscount == 150)
//				{
//					Key_Handle();
//					longflag = 1;
//					presscount = 0;
//				}
//			}else{
//				if(presscount == 50)
//				{
//					Key_Handle();
//					presscount = 0;
//				}
//			}
//			
//		}			
//	}else{
//		keystat = 0;
//		key_value = 0xff;
//		key_old = 0;
//		presscount = 0;
//		longflag = 0;
//	}		
//	

//}
//按键功能
void Key_Function(void)
{	
	static u8 key_old;
	static u8 key_new;
	key_new = key_value;
	if(key_value != 0XFF)
	{		
		if(key_old != key_new && keystat == 0)
		{
			ButtonSound();//按键音
			switch(key_value)
			{
				case KEY_1:
				{
					KEY1_HANDLE();
				}
				break;
				case KEY_2:
				{
					KEY2_HANDLE();
				}
				break;
				case KEY_3:
				{
					KEY3_HANDLE();
				}
				break;
				case KEY_4:
				{
					KEY4_HANDLE();
				}
				break;
				case KEY_5:
				{
					KEY5_HANDLE();
				}
				break;
				case KEY_6:
				{
					KEY6_HANDLE();
				}
				break;
				case KEY_7:
				{
					KEY7_HANDLE();
				}
				break;
				case KEY_8:
				{
					KEY8_HANDLE();
				}
				break;
				case KEY_9:
				{
					KEY9_HANDLE();
				}
				break;
				case KEY_0:
				{
					KEY0_HANDLE();
				}
				break;
				case KEY_ACC:
				{
					ACC_HANDLE();
				}
				break;
				case KEY_BACK:
				{
					BACK_HANDLE();
				}
				break;
				case KEY_DOT:
				{
					DOT_HANDLE();
				}
				break;
				case KEY_UP:
				{
					UP_HANDLE();
				}
				break;
				case KEY_DOWN:
				{
					DOWN_HANDLE();
				}
				break;
				case KEY_LEFT:
				{
					LEFT_HANDLE();
				}
				break;
				case KEY_RIGHT:
				{
					RIGHT_HANDLE();
				}
				break;
				case KEY_ENTER:
				{
					ENTER_HANDLE();
					count_flag = 1;
				}
				break;
				case KEY_ESC:
				{
					ESC_HANDLE();
				}
				break;
				case KEY_FUNC1:
				{					
					FUNC1_HANDLE();
				}
				break;
				case KEY_FUNC2:
				{
					FUNC2_HANDLE();
				}
				break;
				case KEY_FUNC3:
				{
					FUNC3_HANDLE();
				}
				break;
				case KEY_FUNC4:
				{
					FUNC4_HANDLE();
				}
				break;
				case KEY_FUNC5:
				{
					FUNC5_HANDLE();
				}
				break;
				default:break;
			}
			keystat = 1;
			key_old = key_value;
		}			
	}else{
		keystat = 0;
		key_value = 0xff;
		key_old = 0;
	}		
	

}

/*按键功能函数FUNC_1*/
void FUNC1_HANDLE(void)
{
	const u8 (*pt)[6];
	pt = TC_Menu;
	
	switch(page_flag)
	{
		case factory:
		{
			page_home();
		}break;
		case display:
		{
			switch(op_flag)
			{
				case type_1:
				{
					DrawMenu();//重绘菜单栏
					Drawhomemenu();
					LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BLUE);
					LCD_DrawFullRect(96,47,100,31);
					LCD_SetTextColor(LCD_COLOR_BLACK);
					LCD_SetBackColor(LCD_COLOR_YELLOW);										
					op_flag = home_type;
					TCTYPE = TCT;
					LCD_DisplayStringLine(47,100,(uint8_t *)pt[TCTYPE-1]);
					Save_flag();
					tcflag = TCTYPE+1;
					}break;
				case type_2:
				{
					DrawMenu();//重绘菜单栏
					Drawhomemenu();
					LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BLUE);
					LCD_DrawFullRect(96,47,100,31);
					LCD_SetTextColor(LCD_COLOR_BLACK);
					LCD_SetBackColor(LCD_COLOR_YELLOW);
					op_flag = home_type;
					TCTYPE = TCE;
					LCD_DisplayStringLine(47,100,(uint8_t *)pt[TCTYPE-1]);
					Save_flag();
					tcflag = TCTYPE+1;					
				}break;
				case type_3:
				{
					DrawMenu();//重绘菜单栏
					Drawhomemenu();
					LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BLUE);
					LCD_DrawFullRect(96,47,100,31);
					LCD_SetTextColor(LCD_COLOR_BLACK);
					LCD_SetBackColor(LCD_COLOR_YELLOW);
					op_flag = home_type;
					TCTYPE = PT100;
					LCD_DisplayStringLine(47,100,(uint8_t *)pt[TCTYPE-1]);
					Save_flag();
					tcflag = TCTYPE+1;
				}break;
				case set_font:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(47,290,"小");
						}else{
							LCD_DisplayStringLine(47,290,"S");
						}
						FONT = small;
						op_sw = op_off;
						Save_flag();
						page_home();
					}else{
						page_home();
					}
				}break;
				case home_ch1:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH1TYPE = TCT;
						}else if(tcpage == 2){
							CH1TYPE = TCE;
						}else if(tcpage == 3){
							CH1TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH9TYPE = TCT;
						}else if(tcpage == 2){
							CH9TYPE = TCE;
						}else if(tcpage == 3){
							CH9TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch2:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH2TYPE = TCT;
						}else if(tcpage == 2){
							CH2TYPE = TCE;
						}else if(tcpage == 3){
							CH2TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH10TYPE = TCT;
						}else if(tcpage == 2){
							CH10TYPE = TCE;
						}else if(tcpage == 3){
							CH10TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch3:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH3TYPE = TCT;
						}else if(tcpage == 2){
							CH3TYPE = TCE;
						}else if(tcpage == 3){
							CH3TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH11TYPE = TCT;
						}else if(tcpage == 2){
							CH11TYPE = TCE;
						}else if(tcpage == 3){
							CH11TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch4:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH4TYPE = TCT;
						}else if(tcpage == 2){
							CH4TYPE = TCE;
						}else if(tcpage == 3){
							CH4TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH12TYPE = TCT;
						}else if(tcpage == 2){
							CH12TYPE = TCE;
						}else if(tcpage == 3){
							CH12TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch5:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH5TYPE = TCT;
						}else if(tcpage == 2){
							CH5TYPE = TCE;
						}else if(tcpage == 3){
							CH5TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH13TYPE = TCT;
						}else if(tcpage == 2){
							CH13TYPE = TCE;
						}else if(tcpage == 3){
							CH13TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch6:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH6TYPE = TCT;
						}else if(tcpage == 2){
							CH6TYPE = TCE;
						}else if(tcpage == 3){
							CH6TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH14TYPE = TCT;
						}else if(tcpage == 2){
							CH14TYPE = TCE;
						}else if(tcpage == 3){
							CH14TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch7:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH7TYPE = TCT;
						}else if(tcpage == 2){
							CH7TYPE = TCE;
						}else if(tcpage == 3){
							CH7TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH15TYPE = TCT;
						}else if(tcpage == 2){
							CH15TYPE = TCE;
						}else if(tcpage == 3){
							CH15TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch8:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH8TYPE = TCT;
						}else if(tcpage == 2){
							CH8TYPE = TCE;
						}else if(tcpage == 3){
							CH8TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH16TYPE = TCT;
						}else if(tcpage == 2){
							CH16TYPE = TCE;
						}else if(tcpage == 3){
							CH16TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch9:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH9TYPE = TCT;
						}else if(tcpage == 2){
							CH9TYPE = TCE;
						}else if(tcpage == 3){
							CH9TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch10:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH10TYPE = TCT;
						}else if(tcpage == 2){
							CH10TYPE = TCE;
						}else if(tcpage == 3){
							CH10TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch11:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH11TYPE = TCT;
						}else if(tcpage == 2){
							CH11TYPE = TCE;
						}else if(tcpage == 3){
							CH11TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch12:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH12TYPE = TCT;
						}else if(tcpage == 2){
							CH12TYPE = TCE;
						}else if(tcpage == 3){
							CH12TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch13:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH13TYPE = TCT;
						}else if(tcpage == 2){
							CH13TYPE = TCE;
						}else if(tcpage == 3){
							CH13TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch14:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH14TYPE = TCT;
						}else if(tcpage == 2){
							CH14TYPE = TCE;
						}else if(tcpage == 3){
							CH14TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch15:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH15TYPE = TCT;
						}else if(tcpage == 2){
							CH15TYPE = TCE;
						}else if(tcpage == 3){
							CH15TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch16:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH16TYPE = TCT;
						}else if(tcpage == 2){
							CH16TYPE = TCE;
						}else if(tcpage == 3){
							CH16TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				default:
				{
					page_home();
				}break;
			}				
		}break;
		case sysset:
		{
			if(op_sw == op_on)
			{
				switch(op_flag)
				{
					case set_lang:
					{
//						DrawMenu();
//						Drawsysmenu();						
						LCD_SetTextColor(LCD_COLOR_BLACK);
						LCD_SetBackColor(LCD_COLOR_YELLOW);
						LCD_DisplayStringLine(50,170,"CHINESE");
						LANG = chs;
						page_sys();
						op_sw =op_off;
						Save_flag();
					}break;
					case set_date:
					{
						if(date_page == page1)
						{
							plus_year();
						}else{
							plus_day();
						}
					}break;
					case set_time:
					{
						if(time_page == page1)
						{
							plus_hour();							
						}else{
							plus_sec();
						}
					}break;
					case set_brt:
					{
						brt_set(1);
					}break;
					case set_dim:
					{
						dim_set(1);
					}break;
					case set_touch:
					{
						touch_set(1);
					}break;
				}
			}else{
				page_home();
			}
		}break;
		case settings:
		{
			switch(op_flag)
			{
				case set_filter:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(150,50,48,31);
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,150,"打开");
						}else{
							LCD_DisplayStringLine(50,150,"ON");
						}
						FILTER = ft_on;
						op_sw = op_off;
						Save_flag();
					}else{
						page_home();
					}
				}break;
				case set_beep:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(150,90,48,31);
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(90,150,"打开");
						}else{
							LCD_DisplayStringLine(90,150,"ON");
						}
						BEEP = beep_on;
						op_sw = op_off;
						Save_flag();
					}else{
						page_home();
					}
				}break;
				case repeat:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(500,130,48,31);
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(130,500,"打开");
						}else{
							LCD_DisplayStringLine(130,500,"ON");
						}
						REPEAT = rtp_on;
						op_sw = op_off;
						Save_flag();
					}else{
						page_home();
					}
				}break;
				case timetrig:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(150,170,48,31);
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(170,150,"打开");
						}else{
							LCD_DisplayStringLine(170,150,"ON");
						}
						TIMETIRG = trig_on;
						op_sw = op_off;
						Save_flag();
					}else{
						page_home();
					}
				}break;
				case set_unit:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
						LCD_DrawFullRect(150,127,24,31);
						LCD_SetTextColor(LCD_COLOR_BLACK);
						DISP_CNL_S(127,150,"o");
						LCD_DisplayStringLine(127,155,"C");
						UNIT = C;
						op_sw = op_off;
						Save_flag();
						DrawUnit();
					}else{
						page_home();
					}
				}break;
				
				case set_spd:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,500,"快速");
						}else{
							LCD_DisplayStringLine(50,500,"FAST");
						}
						SPEED = fast;
						op_sw = op_off;
						Save_flag();
					}else{
						page_home();
					}
				}break;
				case set_baud:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(500,87,96,31);
						focus_on1();
						LCD_DisplayStringLine(87,500,"9600");
						BAUD = b9600;
						op_sw = op_off;
						Save_flag();
					}else{
						page_home();
					}
				}break;
				
				default:
				{
					page_home();
				}break;
			}				
		}break;
		case separation:
		{
			page_home();
		}break;
		case sysinfo:
		{
			page_home();
		}break;
		case graph:
		{
			page_home();
		}break;
		case graphset:
		{
			page_home();
		}break;
		case history:
		{
			page_home();
		}break;
		case calibrate:
		{
			page_home();
		}break;
	}
}

/*按键功能函数FUNC_2*/
void FUNC2_HANDLE(void)
{
	const u8 (*pt)[6];
	pt = TC_Menu;
	switch(page_flag)
	{
		case display:
		{
			switch(op_flag)
			{
				case type_1:
				{
					DrawMenu();//重绘菜单栏
					Drawhomemenu();
					LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BLUE);
					LCD_DrawFullRect(96,47,100,31);
					LCD_SetTextColor(LCD_COLOR_BLACK);
					LCD_SetBackColor(LCD_COLOR_YELLOW);
					op_flag = home_type;
					TCTYPE = TCK;
					LCD_DisplayStringLine(47,100,(uint8_t *)pt[TCTYPE-1]);
					Save_flag();
					tcflag = TCTYPE+1;
				}break;
				case type_2:
				{
					DrawMenu();//重绘菜单栏
					Drawhomemenu();
					LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BLUE);
					LCD_DrawFullRect(96,47,100,31);
					LCD_SetTextColor(LCD_COLOR_BLACK);
					LCD_SetBackColor(LCD_COLOR_YELLOW);
					op_flag = home_type;
					TCTYPE = TCS;
					LCD_DisplayStringLine(47,100,(uint8_t *)pt[TCTYPE-1]);
					Save_flag();
					tcflag = TCTYPE+1;
				}break;
				case set_font:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(47,290,"中");
						}else{
							LCD_DisplayStringLine(47,290,"M");
						}
						FONT = middle;
						op_sw = op_off;
						Save_flag();
						page_home();
					}else{
						page_graph();
					}
				}break;
				case home_ch1:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH1TYPE = TCK;
						}else if(tcpage == 2){
							CH1TYPE = TCS;
						}else if(tcpage == 3){
//							CH1TYPE = V1;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH9TYPE = TCK;
						}else if(tcpage == 2){
							CH9TYPE = TCS;
						}else if(tcpage == 3){
//							CH9TYPE = V1;
						}
					}
					tcflag = 1;
				}break;
				case home_ch2:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH2TYPE = TCK;
						}else if(tcpage == 2){
							CH2TYPE = TCS;
						}else if(tcpage == 3){
//							CH2TYPE = V1;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH10TYPE = TCK;
						}else if(tcpage == 2){
							CH10TYPE = TCS;
						}else if(tcpage == 3){
//							CH10TYPE = V1;
						}
					}
					tcflag = 1;
				}break;
				case home_ch3:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH3TYPE = TCK;
						}else if(tcpage == 2){
							CH3TYPE = TCS;
						}else if(tcpage == 3){
//							CH3TYPE = V1;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH11TYPE = TCK;
						}else if(tcpage == 2){
							CH11TYPE = TCS;
						}else if(tcpage == 3){
//							CH11TYPE = V1;
						}
					}
					tcflag = 1;
				}break;
				case home_ch4:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH4TYPE = TCK;
						}else if(tcpage == 2){
							CH4TYPE = TCS;
						}else if(tcpage == 3){
//							CH4TYPE = V1;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH12TYPE = TCK;
						}else if(tcpage == 2){
							CH12TYPE = TCS;
						}else if(tcpage == 3){
//							CH12TYPE = V1;
						}
					}
					tcflag = 1;
				}break;
				case home_ch5:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH5TYPE = TCK;
						}else if(tcpage == 2){
							CH5TYPE = TCS;
						}else if(tcpage == 3){
//							CH5TYPE = V1;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH13TYPE = TCK;
						}else if(tcpage == 2){
							CH13TYPE = TCS;
						}else if(tcpage == 3){
//							CH13TYPE = V1;
						}
					}
					tcflag = 1;
				}break;
				case home_ch6:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH6TYPE = TCK;
						}else if(tcpage == 2){
							CH6TYPE = TCS;
						}else if(tcpage == 3){
//							CH6TYPE = V1;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH14TYPE = TCK;
						}else if(tcpage == 2){
							CH14TYPE = TCS;
						}else if(tcpage == 3){
//							CH14TYPE = V1;
						}
					}
					tcflag = 1;
				}break;
				case home_ch7:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH7TYPE = TCK;
						}else if(tcpage == 2){
							CH7TYPE = TCS;
						}else if(tcpage == 3){
//							CH7TYPE = V1;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH15TYPE = TCK;
						}else if(tcpage == 2){
							CH15TYPE = TCS;
						}else if(tcpage == 3){
//							CH15TYPE = V1;
						}
					}
					tcflag = 1;
				}break;
				case home_ch8:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH8TYPE = TCK;
						}else if(tcpage == 2){
							CH8TYPE = TCS;
						}else if(tcpage == 3){
//							CH8TYPE = V1;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH16TYPE = TCK;
						}else if(tcpage == 2){
							CH16TYPE = TCS;
						}else if(tcpage == 3){
//							CH16TYPE = V1;
						}
					}
					tcflag = 1;
				}break;
				case home_ch9:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH9TYPE = TCK;
						}else if(tcpage == 2){
							CH9TYPE = TCS;
						}else if(tcpage == 3){
//							CH9TYPE = V1;
						}
					}
					tcflag = 1;
				}break;
				case home_ch10:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH10TYPE = TCK;
						}else if(tcpage == 2){
							CH10TYPE = TCS;
						}else if(tcpage == 3){
//							CH10TYPE = V1;
						}
					}
					tcflag = 1;
				}break;
				case home_ch11:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH11TYPE = TCK;
						}else if(tcpage == 2){
							CH11TYPE = TCS;
						}else if(tcpage == 3){
//							CH11TYPE = V1;
						}
					}
					tcflag = 1;
				}break;
				case home_ch12:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH12TYPE = TCK;
						}else if(tcpage == 2){
							CH12TYPE = TCS;
						}else if(tcpage == 3){
//							CH12TYPE = V1;
						}
					}
					tcflag = 1;
				}break;
				case home_ch13:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH13TYPE = TCK;
						}else if(tcpage == 2){
							CH13TYPE = TCS;
						}else if(tcpage == 3){
//							CH13TYPE = V1;
						}
					}
					tcflag = 1;
				}break;
				case home_ch14:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH14TYPE = TCK;
						}else if(tcpage == 2){
							CH14TYPE = TCS;
						}else if(tcpage == 3){
//							CH14TYPE = V1;
						}
					}
					tcflag = 1;
				}break;
				case home_ch15:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH15TYPE = TCK;
						}else if(tcpage == 2){
							CH15TYPE = TCS;
						}else if(tcpage == 3){
//							CH15TYPE = V1;
						}
					}
					tcflag = 1;
				}break;
				case home_ch16:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH16TYPE = TCK;
						}else if(tcpage == 2){
							CH16TYPE = TCS;
						}else if(tcpage == 3){
//							CH16TYPE = V1;
						}
					}
					tcflag = 1;
				}break;
				default:
				{
					page_graph();
				}break;
			}
		}break;
		case sysset:
		{
			if(op_sw == op_on)
			{
				switch(op_flag)
				{
					case set_lang:
					{
//						DrawMenu();
//						Drawsysmenu();
						LCD_SetTextColor(LCD_COLOR_BLACK);
						LCD_SetBackColor(LCD_COLOR_YELLOW);
						LCD_DisplayStringLine(50,170+10,"ENGLISH");
						LANG = eng;
						page_sys();
						op_sw = op_off;
						Save_flag();
					}break;
					case set_date:
					{
						if(date_page == page1)
						{
							minus_year();
						}else{
							minus_day();
						}
					}break;
					case set_time:
					{
						if(time_page == page1)
						{
							minus_hour();
						}else{
							minus_sec();
						}
					}break;
					case set_brt:
					{
						brt_set(2);
					}break;
					case set_dim:
					{
						dim_set(2);
					}break;
					case set_touch:
					{
						touch_set(2);
					}break;
				}
			}else{
				page_graph();
			}				
		}break;
		case settings:
		{
			switch(op_flag)
			{
				case set_filter:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,150,"关闭");
						}else{
							LCD_DisplayStringLine(50,150,"OFF");
						}
						FILTER = ft_off;
						op_sw = op_off;
						Save_flag();
					}else{
						page_graph();
					}
				}break;
				case set_beep:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(90,150,"关闭");
						}else{
							LCD_DisplayStringLine(90,150,"OFF");
						}
						BEEP = beep_off;
						op_sw = op_off;
						Save_flag();
					}else{
						page_graph();
					}
				}break;
				case repeat:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(130,500,"关闭");
						}else{
							LCD_DisplayStringLine(130,500,"OFF");
						}
						REPEAT = rtp_off;
						op_sw = op_off;
						Save_flag();
					}else{
						page_graph();
					}
				}break;
				case timetrig:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(150,170+10,48,31);
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(170,150,"关闭");
						}else{
							LCD_DisplayStringLine(170,150,"OFF");
						}
						TIMETIRG = trig_off;
						op_sw = op_off;
						Save_flag();
					}else{
						page_home();
					}
				}break;
				case set_unit:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
						LCD_DrawFullRect(150,127,24,31);
						LCD_SetTextColor(LCD_COLOR_BLACK);					
						LCD_DisplayStringLine(127,155,"K");
						UNIT = K;
						op_sw = op_off;
						Save_flag();
						DrawUnit();
					}else{
						page_graph();
					}
				}break;
				
				case set_spd:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(500,50,64,31);
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,500,"中速");
						}else{
							LCD_DisplayStringLine(50,500,"MED");
						}
						SPEED = moderate;
						op_sw = op_off;
						Save_flag();
					}else{
						page_graph();
					}
				}break;
				case set_baud:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(500,87,96,31);
						focus_on1();
						LCD_DisplayStringLine(87,500,"19200");
						BAUD = b19200;
						op_sw = op_off;
						Save_flag();
					}else{
						page_graph();
					}
				}break;
				
				default:
				{
					page_graph();
				}break;
			}				
		}break;
		case sysinfo:
		{
			page_graph();
		}break;
		case graph:
		{
//			page_his();
		}break;
		case graphset:
		{
			page_graph();
		}break;
		case separation:
		{
			page_graph();
		}break;
		case history:
		{
			Read_His_Data(1);
		}break;
		case hisfile:
		{
			READ_HIS_FOLDER();
		}break;
		case calibrate:
		{
			page_graph();
		}break;
	}
}

/*按键功能函数FUNC_3*/
void FUNC3_HANDLE(void)
{
	const u8 (*pt)[6];
	pt = TC_Menu;
	switch(page_flag)
	{
		case display:
		{
			switch(op_flag)
			{
				case type_1:
				{
					DrawMenu();//重绘菜单栏
					Drawhomemenu();
					LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BLUE);
					LCD_DrawFullRect(96,47,100,31);
					LCD_SetTextColor(LCD_COLOR_BLACK);
					LCD_SetBackColor(LCD_COLOR_YELLOW);
					op_flag = home_type;
					TCTYPE = TCJ;
					LCD_DisplayStringLine(47,100,(uint8_t *)pt[TCTYPE-1]);
					Save_flag();
					tcflag = TCTYPE+1;
				}break;
				case type_2:
				{
					DrawMenu();//重绘菜单栏
					Drawhomemenu();
					LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BLUE);
					LCD_DrawFullRect(96,47,100,31);
					LCD_SetTextColor(LCD_COLOR_BLACK);
					LCD_SetBackColor(LCD_COLOR_YELLOW);
					op_flag = home_type;
					TCTYPE = TCR;
					LCD_DisplayStringLine(47,100,(uint8_t *)pt[TCTYPE-1]);
					Save_flag();
					tcflag = TCTYPE+1;
				}break;
				case set_font:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(47,290,"大");
						}else{
							LCD_DisplayStringLine(47,290,"L");
						}
						FONT = big;
						op_sw = op_off;
						Save_flag();
						page_home();
					}else{
						page_sys();
					}
				}break;
				case home_ch1:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH1TYPE = TCJ;
						}else if(tcpage == 2){
							CH1TYPE = TCR;
						}else if(tcpage == 3){
							CH1TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH9TYPE = TCJ;
						}else if(tcpage == 2){
							CH9TYPE = TCR;
						}else if(tcpage == 3){
							CH9TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch2:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH2TYPE = TCJ;
						}else if(tcpage == 2){
							CH2TYPE = TCR;
						}else if(tcpage == 3){
							CH2TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH10TYPE = TCJ;
						}else if(tcpage == 2){
							CH10TYPE = TCR;
						}else if(tcpage == 3){
							CH10TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch3:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH3TYPE = TCJ;
						}else if(tcpage == 2){
							CH3TYPE = TCR;
						}else if(tcpage == 3){
							CH3TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH11TYPE = TCJ;
						}else if(tcpage == 2){
							CH11TYPE = TCR;
						}else if(tcpage == 3){
							CH11TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch4:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH4TYPE = TCJ;
						}else if(tcpage == 2){
							CH4TYPE = TCR;
						}else if(tcpage == 3){
							CH4TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH12TYPE = TCJ;
						}else if(tcpage == 2){
							CH12TYPE = TCR;
						}else if(tcpage == 3){
							CH12TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch5:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH5TYPE = TCJ;
						}else if(tcpage == 2){
							CH5TYPE = TCR;
						}else if(tcpage == 3){
							CH5TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH13TYPE = TCJ;
						}else if(tcpage == 2){
							CH13TYPE = TCR;
						}else if(tcpage == 3){
							CH13TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch6:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH6TYPE = TCJ;
						}else if(tcpage == 2){
							CH6TYPE = TCR;
						}else if(tcpage == 3){
							CH6TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH14TYPE = TCJ;
						}else if(tcpage == 2){
							CH14TYPE = TCR;
						}else if(tcpage == 3){
							CH14TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch7:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH7TYPE = TCJ;
						}else if(tcpage == 2){
							CH7TYPE = TCR;
						}else if(tcpage == 3){
							CH7TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH15TYPE = TCJ;
						}else if(tcpage == 2){
							CH15TYPE = TCR;
						}else if(tcpage == 3){
							CH15TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch8:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH8TYPE = TCJ;
						}else if(tcpage == 2){
							CH8TYPE = TCR;
						}else if(tcpage == 3){
							CH8TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH16TYPE = TCJ;
						}else if(tcpage == 2){
							CH16TYPE = TCR;
						}else if(tcpage == 3){
							CH16TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch9:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH9TYPE = TCJ;
						}else if(tcpage == 2){
							CH9TYPE = TCR;
						}else if(tcpage == 3){
							CH9TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch10:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH10TYPE = TCJ;
						}else if(tcpage == 2){
							CH10TYPE = TCR;
						}else if(tcpage == 3){
							CH10TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch11:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH11TYPE = TCJ;
						}else if(tcpage == 2){
							CH11TYPE = TCR;
						}else if(tcpage == 3){
							CH11TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch12:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH12TYPE = TCJ;
						}else if(tcpage == 2){
							CH12TYPE = TCR;
						}else if(tcpage == 3){
							CH12TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch13:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH13TYPE = TCJ;
						}else if(tcpage == 2){
							CH13TYPE = TCR;
						}else if(tcpage == 3){
							CH13TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch14:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH14TYPE = TCJ;
						}else if(tcpage == 2){
							CH14TYPE = TCR;
						}else if(tcpage == 3){
							CH14TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch15:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH15TYPE = TCJ;
						}else if(tcpage == 2){
							CH15TYPE = TCR;
						}else if(tcpage == 3){
							CH15TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch16:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH16TYPE = TCJ;
						}else if(tcpage == 2){
							CH16TYPE = TCR;
						}else if(tcpage == 3){
							CH16TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				default:
				{
					page_sys();
				}break;
			}
		}break;
		case sysset:
		{
			if(op_sw == op_on)
			{
				switch(op_flag)
				{
					case set_date:
					{
						if(date_page == page1)
						{
							plus_mon();
						}
					}break;
					case set_time:
					{
						if(time_page == page1)
						{
							plus_min();
						}
					}break;				
					case set_brt:
					{
						brt_set(3);
					}break;
					case set_dim:
					{
						dim_set(3);
					}break;
				}
			}else{
				TP_Adjust();
			}
		}break;
		case settings:
		{
			switch(op_flag)
			{
				case set_unit:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
						LCD_DrawFullRect(150,127,24,31);
						LCD_SetTextColor(LCD_COLOR_BLACK);					
						DISP_CNL_S(127,150,"o");
						LCD_DisplayStringLine(127,155,"F");
						UNIT = F;
						op_sw = op_off;
						Save_flag();
						DrawUnit();
					}else{
						page_sys();
					}
				}break;
				
				case set_spd:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,500,"慢速");
						}else{
							LCD_DisplayStringLine(50,500,"SLOW");
						}
						SPEED = slow;
						op_sw = op_off;
						Save_flag();
					}else{
						page_sys();
					}
				}break;
				case set_baud:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(500,87,96,31);
						focus_on1();
						LCD_DisplayStringLine(87,500,"38400");
						BAUD = b38400;
						op_sw = op_off;
						Save_flag();
					}else{
						page_sys();
					}
				}break;
				default:
				{
					page_sys();
				}break;
			}
		}break;
		case separation:
		{
			page_sys();
		}break;
		case sysinfo:
		{
			page_sys();
		}break;
		case graph:
		{
			page_sys();
		}break;
		case graphset:
		{
			page_sys();
		}break;
		case history:
		{
			if(hispage > 0)
			{
				hispage--;
				Read_His_Data(hispage);
				Draw_His_Graph();
			}
		}break;
		case calibrate:
		{
			page_sys();
		}break;
	}
}

/*按键功能函数FUNC_4*/
void FUNC4_HANDLE(void)
{
	const u8 (*pt)[6];
	pt = TC_Menu;
	switch(page_flag)
	{
		case display:
		{
			switch(op_flag)
			{
				case type_1:
				{
					DrawMenu();//重绘菜单栏
					Drawhomemenu();
					LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BLUE);
					LCD_DrawFullRect(96,47,100,31);
					LCD_SetTextColor(LCD_COLOR_BLACK);
					LCD_SetBackColor(LCD_COLOR_YELLOW);
					op_flag = home_type;
					TCTYPE = TCN;
					LCD_DisplayStringLine(47,100,(uint8_t *)pt[TCTYPE-1]);
					Save_flag();
					tcflag = TCTYPE+1;
				}break;
				case type_2:
				{
					DrawMenu();//重绘菜单栏
					Drawhomemenu();
					LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BLUE);
					LCD_DrawFullRect(96,47,100,31);
					LCD_SetTextColor(LCD_COLOR_BLACK);
					LCD_SetBackColor(LCD_COLOR_YELLOW);
					op_flag = home_type;
					TCTYPE = TCB;
					LCD_DisplayStringLine(47,100,(uint8_t *)pt[TCTYPE-1]);
					Save_flag();
					tcflag = TCTYPE+1;
				}break;
				case home_ch1:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH1TYPE = TCN;
						}else if(tcpage == 2){
							CH1TYPE = TCB;
						}else if(tcpage == 3){
							CH1TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH9TYPE = TCN;
						}else if(tcpage == 2){
							CH9TYPE = TCB;
						}else if(tcpage == 3){
							CH9TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch2:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH2TYPE = TCN;
						}else if(tcpage == 2){
							CH2TYPE = TCB;
						}else if(tcpage == 3){
							CH2TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH10TYPE = TCN;
						}else if(tcpage == 2){
							CH10TYPE = TCB;
						}else if(tcpage == 3){
							CH10TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch3:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH3TYPE = TCN;
						}else if(tcpage == 2){
							CH3TYPE = TCB;
						}else if(tcpage == 3){
							CH3TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH11TYPE = TCN;
						}else if(tcpage == 2){
							CH11TYPE = TCB;
						}else if(tcpage == 3){
							CH11TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch4:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH4TYPE = TCN;
						}else if(tcpage == 2){
							CH4TYPE = TCB;
						}else if(tcpage == 3){
							CH4TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH12TYPE = TCN;
						}else if(tcpage == 2){
							CH12TYPE = TCB;
						}else if(tcpage == 3){
							CH12TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch5:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH5TYPE = TCN;
						}else if(tcpage == 2){
							CH5TYPE = TCB;
						}else if(tcpage == 3){
							CH5TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH13TYPE = TCN;
						}else if(tcpage == 2){
							CH13TYPE = TCB;
						}else if(tcpage == 3){
							CH13TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch6:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH6TYPE = TCN;
						}else if(tcpage == 2){
							CH6TYPE = TCB;
						}else if(tcpage == 3){
							CH6TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH14TYPE = TCN;
						}else if(tcpage == 2){
							CH14TYPE = TCB;
						}else if(tcpage == 3){
							CH14TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch7:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH7TYPE = TCN;
						}else if(tcpage == 2){
							CH7TYPE = TCB;
						}else if(tcpage == 3){
							CH7TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH15TYPE = TCN;
						}else if(tcpage == 2){
							CH15TYPE = TCB;
						}else if(tcpage == 3){
							CH15TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch8:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH8TYPE = TCN;
						}else if(tcpage == 2){
							CH8TYPE = TCB;
						}else if(tcpage == 3){
							CH8TYPE = PT100;
						}
					}else if(ch_page == page2){
						if(tcpage == 1)
						{
							CH16TYPE = TCN;
						}else if(tcpage == 2){
							CH16TYPE = TCB;
						}else if(tcpage == 3){
							CH16TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch9:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH9TYPE = TCN;
						}else if(tcpage == 2){
							CH9TYPE = TCB;
						}else if(tcpage == 3){
							CH9TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch10:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH10TYPE = TCN;
						}else if(tcpage == 2){
							CH10TYPE = TCB;
						}else if(tcpage == 3){
							CH10TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch11:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH11TYPE = TCN;
						}else if(tcpage == 2){
							CH11TYPE = TCB;
						}else if(tcpage == 3){
							CH11TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch12:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH12TYPE = TCN;
						}else if(tcpage == 2){
							CH12TYPE = TCB;
						}else if(tcpage == 3){
							CH12TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch13:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH13TYPE = TCN;
						}else if(tcpage == 2){
							CH13TYPE = TCB;
						}else if(tcpage == 3){
							CH13TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch14:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH14TYPE = TCN;
						}else if(tcpage == 2){
							CH14TYPE = TCB;
						}else if(tcpage == 3){
							CH14TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch15:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH15TYPE = TCN;
						}else if(tcpage == 2){
							CH15TYPE = TCB;
						}else if(tcpage == 3){
							CH15TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				case home_ch16:
				{
					if(ch_page == page1)
					{
						if(tcpage == 1)
						{
							CH16TYPE = TCN;
						}else if(tcpage == 2){
							CH16TYPE = TCB;
						}else if(tcpage == 3){
							CH16TYPE = PT100;
						}
					}
					tcflag = 1;
				}break;
				default:
				{
					page_set();
				}break;
			}
		}break;
		case sysset:
		{
			if(op_sw == op_on)
			{
				switch(op_flag)
				{
					case set_date:
					{
						if(date_page == page1)
						{
							minus_mon();
						}
					}break;
					case set_time:
					{
						if(time_page == page1)
						{
							minus_min();
						}
					}break;				
					case set_brt:
					{
						brt_set(4);
					}break;
					case set_dim:
					{
						dim_set(4);
					}break;
				}
			}else{
					page_set();
			}
		}break;
		case settings:
		{
			switch(op_flag)
			{
				case set_baud:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(500,87,96,31);
						focus_on1();
						LCD_DisplayStringLine(87,500,"57600");
						BAUD = b57600;
						op_sw = op_off;
						Save_flag();
					}else{
						page_set();
					}
				}break;
				default:
				{
					page_cal();
				}break;
			}
		}break;
		case separation:
		{
			page_set();
		}break;
		case sysinfo:
		{
			page_set();
		}break;
		case graph:
		{
			page_set();
		}break;
		case graphset:
		{
			page_set();
		}break;
		case history:
		{
			if(hispage < BlockNum.Num[0]-1)
			{
				hispage++;
				Read_His_Data(hispage);
				Draw_His_Graph();
			}
		}break;
		case calibrate:
		{
			page_set();
		}break;
	}
}

/*按键功能函数FUNC_5*/
void FUNC5_HANDLE(void)
{
	static u16 i;
	switch(page_flag)
	{
		case display:
		{
			switch(op_flag)
			{
				case type_1:
				{
					DrawType(2);
//					DrawMenu();//重绘菜单栏
//					LCD_SetTextColor(LCD_COLOR_YELLOW);
//					LCD_SetBackColor(LCD_COLOR_BLACK);
//					LCD_DisplayStringLine(450,35,"TC-E");
//					LCD_DisplayStringLine(450,157,"TC-S");
//					LCD_DisplayStringLine(450,295,"TC-R");
//					LCD_DisplayStringLine(450,420,"TC-B");
//					if(LANG == chs)
//					{
//						LCD_DisplayStringLine(450,550,"更多");
//					}else{
//						LCD_DisplayStringLine(450,550,"MORE");
//					}
					op_flag = type_2;						
				}break;
				case type_2:
				{
//					DrawType(3);
//					op_flag = type_3;
					DrawType(1);
					op_flag = type_1;					
				}break;
//				case type_3:
//				{
//					DrawType(1);
//					op_flag = type_1;						
//				}break;
				case home_ch1:
				{
					if(tcpage == 1)
					{
						DrawType(2);
						tcpage = 2;
					}else if(tcpage == 2){
						DrawType(1);
						tcpage = 1;
					}else if(tcpage == 3){
						DrawType(1);
						tcpage = 1;
					}
				}break;
				case home_ch2:
				{
					if(tcpage == 1)
					{
						DrawType(2);
						tcpage = 2;
					}else if(tcpage == 2){
						DrawType(1);
						tcpage = 1;
					}else if(tcpage == 3){
						DrawType(1);
						tcpage = 1;
					}
				}break;
				case home_ch3:
				{
					if(tcpage == 1)
					{
						DrawType(2);
						tcpage = 2;
					}else if(tcpage == 2){
						DrawType(1);
						tcpage = 1;
					}else if(tcpage == 3){
						DrawType(1);
						tcpage = 1;
					}
				}break;
				case home_ch4:
				{
					if(tcpage == 1)
					{
						DrawType(2);
						tcpage = 2;
					}else if(tcpage == 2){
						DrawType(1);
						tcpage = 1;
					}else if(tcpage == 3){
						DrawType(1);
						tcpage = 1;
					}
				}break;
				case home_ch5:
				{
					if(tcpage == 1)
					{
						DrawType(2);
						tcpage = 2;
					}else if(tcpage == 2){
						DrawType(1);
						tcpage = 1;
					}else if(tcpage == 3){
						DrawType(1);
						tcpage = 1;
					}
				}break;
				case home_ch6:
				{
					if(tcpage == 1)
					{
						DrawType(2);
						tcpage = 2;
					}else if(tcpage == 2){
						DrawType(1);
						tcpage = 1;
					}else if(tcpage == 3){
						DrawType(1);
						tcpage = 1;
					}
				}break;
				case home_ch7:
				{
					if(tcpage == 1)
					{
						DrawType(2);
						tcpage = 2;
					}else if(tcpage == 2){
						DrawType(1);
						tcpage = 1;
					}else if(tcpage == 3){
						DrawType(1);
						tcpage = 1;
					}
				}break;
				case home_ch8:
				{
					if(tcpage == 1)
					{
						DrawType(2);
						tcpage = 2;
					}else if(tcpage == 2){
						DrawType(1);
						tcpage = 1;
					}else if(tcpage == 3){
						DrawType(1);
						tcpage = 1;
					}
				}break;
				case home_ch9:
				{
					if(tcpage == 1)
					{
						DrawType(2);
						tcpage = 2;
					}else if(tcpage == 2){
						DrawType(1);
						tcpage = 1;
					}else if(tcpage == 3){
						DrawType(1);
						tcpage = 1;
					}
				}break;
				case home_ch10:
				{
					if(tcpage == 1)
					{
						DrawType(2);
						tcpage = 2;
					}else if(tcpage == 2){
						DrawType(1);
						tcpage = 1;
					}else if(tcpage == 3){
						DrawType(1);
						tcpage = 1;
					}
				}break;
				case home_ch11:
				{
					if(tcpage == 1)
					{
						DrawType(2);
						tcpage = 2;
					}else if(tcpage == 2){
						DrawType(1);
						tcpage = 1;
					}else if(tcpage == 3){
						DrawType(1);
						tcpage = 1;
					}
				}break;
				case home_ch12:
				{
					if(tcpage == 1)
					{
						DrawType(2);
						tcpage = 2;
					}else if(tcpage == 2){
						DrawType(1);
						tcpage = 1;
					}else if(tcpage == 3){
						DrawType(1);
						tcpage = 1;
					}
				}break;
				case home_ch13:
				{
					if(tcpage == 1)
					{
						DrawType(2);
						tcpage = 2;
					}else if(tcpage == 2){
						DrawType(1);
						tcpage = 1;
					}else if(tcpage == 3){
						DrawType(1);
						tcpage = 1;
					}
				}break;
				case home_ch14:
				{
					if(tcpage == 1)
					{
						DrawType(2);
						tcpage = 2;
					}else if(tcpage == 2){
						DrawType(1);
						tcpage = 1;
					}else if(tcpage == 3){
						DrawType(1);
						tcpage = 1;
					}
				}break;
				case home_ch15:
				{
					if(tcpage == 1)
					{
						DrawType(2);
						tcpage = 2;
					}else if(tcpage == 2){
						DrawType(1);
						tcpage = 1;
					}else if(tcpage == 3){
						DrawType(1);
						tcpage = 1;
					}
				}break;
				case home_ch16:
				{
					if(tcpage == 1)
					{
						DrawType(2);
						tcpage = 2;
					}else if(tcpage == 2){
						DrawType(1);
						tcpage = 1;
					}else if(tcpage == 3){
						DrawType(1);
						tcpage = 1;
					}
				}break;
				default:
				{
					
					if(FONT == big && CHNUM > 8)
					{
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(10,100,600,310);
						if(ch_page == page1 && CHNUM > 8)
						{
							
							LCD_SetBackColor(LCD_COLOR_BACK);
							LCD_SetTextColor(LCD_COLOR_HLT);
							if(CH9_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(100,10,"009");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(100,10,"009");
							}
							
							if(CH10_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(170+10,10,"010");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(170+10,10,"010");
							}
							
							if(CH11_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(240+20,10,"011");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(240+20,10,"011");
							}
							
							if(CH12_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(310+30,10,"012");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(310+30,10,"012");
							}
							
							if(CH13_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(100,330,"013");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(100,330,"013");
							}
							
							if(CH14_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(170+10,330,"014");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(170+10,330,"014");
							}
							
							if(CH15_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(240+20,330,"015");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(240+20,330,"015");
							}
							
							if(CH16_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(310+30,330,"016");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(310+30,330,"016");
							}
							ch_page = page2;
						}else if(ch_page == page2 && CHNUM == 16){
							LCD_SetBackColor(LCD_COLOR_BACK);
							LCD_SetTextColor(LCD_COLOR_HLT);
							if(CH1_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(100,10,"001");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(100,10,"001");
							}
							
							if(CH2_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(170+10,10,"002");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(170+10,10,"002");
							}
							
							if(CH3_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(240+20,10,"003");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(240+20,10,"003");
							}
							
							if(CH4_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(310+30,10,"004");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(310+30,10,"004");
							}
							
							if(CH5_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(100,330,"005");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(100,330,"005");
							}
							
							if(CH6_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(170+10,330,"006");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(170+10,330,"006");
							}
							
							if(CH7_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(240+20,330,"007");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(240+20,330,"007");
							}
							
							if(CH8_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(310+30,330,"008");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(310+30,330,"008");
							}
							ch_page = page1;
						}else if(ch_page == page2 && CHNUM > 16){
							LCD_SetBackColor(LCD_COLOR_BACK);
							LCD_SetTextColor(LCD_COLOR_HLT);
							if(CH17_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(100,10,"017");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(100,10,"017");
							}
							
							if(CH18_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(170+10,10,"018");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(170+10,10,"018");
							}
							
							if(CH19_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(240+20,10,"019");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(240+20,10,"019");
							}
							
							if(CH20_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(310+30,10,"020");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(310+30,10,"020");
							}
							
							if(CH21_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(100,330,"021");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(100,330,"021");
							}
							
							if(CH22_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(170+10,330,"022");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(170+10,330,"022");
							}
							
							if(CH23_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(240+20,330,"023");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(240+20,330,"023");
							}
							
							if(CH24_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(310+30,330,"024");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(310+30,330,"024");
							}
							ch_page = page3;
						}else if(ch_page == page3){
							if(CH25_SW == ch_off && CHNUM > 16)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(100,10,"025");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(100,10,"025");
							}
							
							if(CH26_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(170+10,10,"026");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(170+10,10,"026");
							}
							
							if(CH27_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(240+20,10,"027");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(240+20,10,"027");
							}
							
							if(CH28_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(310+30,10,"028");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(310+30,10,"028");
							}
							
							if(CH29_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(100,330,"029");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(100,330,"029");
							}
						
							if(CH30_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(170+10,330,"030");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(170+10,330,"030");
							}
							
							if(CH31_SW == ch_off){
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(240+20,330,"031");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(240+20,330,"031");
							}
							
							if(CH32_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(310+30,330,"032");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(310+30,330,"032");
							}
							ch_page = page4;
						}else if(ch_page == page4 && CHNUM > 16){
							if(CH33_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(100,10,"033");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(100,10,"033");
							}
							
							if(CH34_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(170+10,10,"034");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(170+10,10,"034");
							}
							
							if(CH35_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(240+20,10,"035");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(240+20,10,"035");
							}
								
							if(CH36_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(310+30,10,"036");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(310+30,10,"036");
							}
								
							if(CH37_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(100,330,"037");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(100,330,"037");
							}
								
							if(CH38_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(170+10,330,"038");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(170+10,330,"038");
							}
								
							if(CH39_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(240+20,330,"039");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(240+20,330,"039");
							}
								
							if(CH40_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(310+30,330,"040");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(310+30,330,"040");
							}
							ch_page = page5;
						}else if(ch_page == page5 && CHNUM > 16){
							if(CH1_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(100,10,"001");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(100,10,"001");
							}
							
							if(CH2_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(170+10,10,"002");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(170+10,10,"002");
							}
							
							if(CH3_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(240+20,10,"003");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(240+20,10,"003");
							}
							
							if(CH4_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(310+30,10,"004");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(310+30,10,"004");
							}
							
							if(CH5_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(100,330,"005");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(100,330,"005");
							}
							
							if(CH6_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(170+10,330,"006");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(170+10,330,"006");
							}
							
							if(CH7_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(240+20,330,"007");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(240+20,330,"007");
							}
							
							if(CH8_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								LCD_DisplayStringLine_48(310+30,330,"008");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								LCD_DisplayStringLine_48(310+30,330,"008");
							}
							ch_page = page1;
						}
					}else if(FONT == middle){
						if(ch_page == page1 && CHNUM > 24)
						{
							LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
							LCD_DrawFullRect(10,100,550,310);
							
							if(CH25_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(100,10,"025");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(100,10,"025");
							}
							
							if(CH26_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(140,10,"026");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(140,10,"026");
							}
							
							if(CH27_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(180,10,"027");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(180,10,"027");
							}
							
							if(CH28_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(220,10,"028");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(220,10,"028");
							}
							
							if(CH29_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(260,10,"029");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(260,10,"029");
							}
							
							if(CH30_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(300,10,"030");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(300,10,"030");
							}
							
							if(CH31_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(340,10,"031");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(340,10,"031");
							}
							
							if(CH32_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(380,10,"032");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(380,10,"032");
							}
							
							if(CH33_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(100,210,"033");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(100,210,"033");
							}
							
							if(CH34_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(140,210,"034");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(140,210,"034");
							}
							
							if(CH35_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(180,210,"035");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(180,210,"035");
							}
							
							if(CH36_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(220,210,"036");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(220,210,"036");
							}
							
							if(CH37_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(260,210,"037");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(260,210,"037");
							}
							
							if(CH38_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(300,210,"038");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(300,210,"038");
							}
							
							if(CH39_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(340,210,"039");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(340,210,"039");
							}
							
							if(CH40_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(380,210,"040");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(380,210,"040");
							}
							
							ch_page = page2;
						}else if(ch_page == page2){
							if(CH1_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(100,10,"001");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(100,10,"001");
							}
							
							if(CH2_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(140,10,"002");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(140,10,"002");
							}
							
							if(CH3_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(180,10,"003");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(180,10,"003");
							}
							
							if(CH4_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(220,10,"004");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(220,10,"004");
							}
							
							if(CH5_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(260,10,"005");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(260,10,"005");
							}
							
							if(CH6_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(300,10,"006");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(300,10,"006");
							}
							
							if(CH7_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(340,10,"007");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(340,10,"007");
							}
							
							if(CH8_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(380,10,"008");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(380,10,"008");
							}
							
							if(CH9_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(100,210,"009");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(100,210,"009");
							}
							
							if(CH10_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(140,210,"010");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(140,210,"010");
							}
							
							if(CH11_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(180,210,"011");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(180,210,"011");
							}
							
							if(CH12_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(220,210,"012");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(220,210,"012");
							}
							
							if(CH13_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(260,210,"013");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(260,210,"013");
							}
							
							if(CH14_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(300,210,"014");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(300,210,"014");
							}
							
							if(CH15_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(340,210,"015");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(340,210,"015");
							}
							
							if(CH16_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(380,210,"016");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(380,210,"016");
							}
							
							if(CH17_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(100,410,"017");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(100,410,"017");
							}
							
							if(CH18_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(140,410,"018");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(140,410,"018");
							}
							
							if(CH19_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(180,410,"019");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(180,410,"019");
							}
							
							if(CH20_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(220,410,"020");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(220,410,"020");
							}
							
							if(CH21_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(260,410,"021");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(260,410,"021");
							}
							
							if(CH22_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(300,410,"022");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(300,410,"022");
							}
							
							if(CH23_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(340,410,"023");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(340,410,"023");
							}
							
							if(CH24_SW == ch_off)
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
								DISP_CNL_M(380,410,"024");
							}else{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_HLT);
								DISP_CNL_M(380,410,"024");
							}
							
							ch_page = page1;
						}
					}
					
					if((FONT == big || FONT == middle)&&CHNUM > 8){
						LCD_SetTextColor(LCD_COLOR_BLACK);
						LCD_SetBackColor(LCD_COLOR_YELLOW);
						if(TCTYPE == TCK)
						{
							LCD_DisplayStringLine(47,100,"TC-K");
						}else if(TCTYPE == TCT){
							LCD_DisplayStringLine(47,100,"TC-T");
						}else if(TCTYPE == TCJ){
							LCD_DisplayStringLine(47,100,"TC-J");
						}else if(TCTYPE == TCN){
							LCD_DisplayStringLine(47,100,"TC-N");
						}else if(TCTYPE == TCE){
							LCD_DisplayStringLine(47,100,"TC-E");
						}else if(TCTYPE == TCS){
							LCD_DisplayStringLine(47,100,"TC-S");
						}else if(TCTYPE == TCR){
							LCD_DisplayStringLine(47,100,"TC-R");
						}else if(TCTYPE == TCB){
							LCD_DisplayStringLine(47,100,"TC-B");
						}else if(TCTYPE == PT100){
							LCD_DisplayStringLine(47,100,"PT100");
						}else if(TCTYPE == V1){
							LCD_DisplayStringLine(47,100,"1~5V");
						}
						op_flag = home_type;
						focus_off1();
						if(FONT == big){
							if(LANG == chs)
							{
								LCD_DisplayStringLine(47,290,"大");
							}else{
								LCD_DisplayStringLine(47,290,"L");
							}
						}else if(FONT == middle){
							if(LANG == chs)
							{
								LCD_DisplayStringLine(47,290,"中");
							}else{
								LCD_DisplayStringLine(47,290,"M");
							}
						}else if(FONT == small){
							if(LANG == chs)
							{
								LCD_DisplayStringLine(47,290,"小");
							}else{
								LCD_DisplayStringLine(47,290,"S");
							}
						}
					}
					Disp_Type();
				}break;
			}
		}break;
		case sysset:
		{
			switch(op_flag)
			{
				case set_date:
				{
					if(op_sw == op_on)
					{
						if(date_page == page1)
						{
							DrawMenu();
							Drawdate2();
							date_page = page2;
						}else{
							DrawMenu();
							Drawdate();
							date_page = page1;
						}
					}else{
						page_sysinfo();
					}
				}break;
				case set_time:
				{
					if(op_sw == op_on)
					{
						if(time_page == page1)
						{
							DrawMenu();
							Drawtime2();
							time_page = page2;
						}else{
							DrawMenu();
							Drawtime();
							time_page = page1;
						}
					}else{
						page_sysinfo();
					}
				}break;
				case set_brt:
				{
					if(op_sw == op_on)
					{
						brt_set(5);
					}else{
						page_sysinfo();
					}
				}break;
				case set_dim:
				{
					if(op_sw == op_on)
					{
						dim_set(5);
					}else{
						page_sysinfo();
					}
				}break;
				default:
				{
					page_sysinfo();
				}break;
			}
		}break;
		case settings:
		{
			switch(op_flag)
			{
				case set_baud:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						focus_on1();
						LCD_DisplayStringLine(87,500,"115200");
						BAUD = b115200;
						op_sw = op_off;
						Save_flag();
					}else{
						page_sep();
					}
				}break;
				default:
				{
					if(op_sw == op_off)
					{
						page_sep();
					}
				}break;
			}
		}break;
		case separation:
		{
			if(CHNUM > 8)
			{
				switch(spt_page)
				{
					case page1:
					{
						LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
						LCD_SetBackColor(LCD_COLOR_BACK);
						LCD_DisplayStringLine(80,10,"009");
						LCD_DisplayStringLine(120,10,"010");
						LCD_DisplayStringLine(160,10,"011");
						LCD_DisplayStringLine(200,10,"012");
						LCD_DisplayStringLine(240,10,"013");
						LCD_DisplayStringLine(280,10,"014");
						LCD_DisplayStringLine(320,10,"015");
						LCD_DisplayStringLine(360,10,"016");
						minfocus_on(1,CH9MIN);
						minfocus_off(2,CH10MIN);
						minfocus_off(3,CH11MIN);
						minfocus_off(4,CH12MIN);
						minfocus_off(5,CH13MIN);
						minfocus_off(6,CH14MIN);
						minfocus_off(7,CH15MIN);
						minfocus_off(8,CH16MIN);
						
						maxfocus_off(1,CH9MAX);
						maxfocus_off(2,CH10MAX);
						maxfocus_off(3,CH11MAX);
						maxfocus_off(4,CH12MAX);
						maxfocus_off(5,CH13MAX);
						maxfocus_off(6,CH14MAX);
						maxfocus_off(7,CH15MAX);
						maxfocus_off(8,CH16MAX);
						
						op_flag = set_min1;
						spt_page = page2;
					}break;
					case page2:
					{
						if(CHNUM > 16)
						{
							LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
							LCD_SetBackColor(LCD_COLOR_BACK);
							LCD_DisplayStringLine(80,10,"017");
							LCD_DisplayStringLine(120,10,"018");
							LCD_DisplayStringLine(160,10,"019");
							LCD_DisplayStringLine(200,10,"020");
							LCD_DisplayStringLine(240,10,"021");
							LCD_DisplayStringLine(280,10,"022");
							LCD_DisplayStringLine(320,10,"023");
							LCD_DisplayStringLine(360,10,"024");
							minfocus_on(1,CH17MIN);
							minfocus_off(2,CH18MIN);
							minfocus_off(3,CH19MIN);
							minfocus_off(4,CH20MIN);
							minfocus_off(5,CH21MIN);
							minfocus_off(6,CH22MIN);
							minfocus_off(7,CH23MIN);
							minfocus_off(8,CH24MIN);
							
							maxfocus_off(1,CH17MAX);
							maxfocus_off(2,CH18MAX);
							maxfocus_off(3,CH19MAX);
							maxfocus_off(4,CH20MAX);
							maxfocus_off(5,CH21MAX);
							maxfocus_off(6,CH22MAX);
							maxfocus_off(7,CH23MAX);
							maxfocus_off(8,CH24MAX);
							
							op_flag = set_min1;
							spt_page = page3;
						}else{
							LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
							LCD_SetBackColor(LCD_COLOR_BACK);
							LCD_DisplayStringLine(80,10,"001");
							LCD_DisplayStringLine(120,10,"002");
							LCD_DisplayStringLine(160,10,"003");
							LCD_DisplayStringLine(200,10,"004");
							LCD_DisplayStringLine(240,10,"005");
							LCD_DisplayStringLine(280,10,"006");
							LCD_DisplayStringLine(320,10,"007");
							LCD_DisplayStringLine(360,10,"008");
							minfocus_on(1,CH1MIN);
							minfocus_off(2,CH2MIN);
							minfocus_off(3,CH3MIN);
							minfocus_off(4,CH4MIN);
							minfocus_off(5,CH5MIN);
							minfocus_off(6,CH6MIN);
							minfocus_off(7,CH7MIN);
							minfocus_off(8,CH8MIN);
							
							maxfocus_off(1,CH1MAX);
							maxfocus_off(2,CH2MAX);
							maxfocus_off(3,CH3MAX);
							maxfocus_off(4,CH4MAX);
							maxfocus_off(5,CH5MAX);
							maxfocus_off(6,CH6MAX);
							maxfocus_off(7,CH7MAX);
							maxfocus_off(8,CH8MAX);
							
							op_flag = set_min1;
							spt_page = page1;
						}
					}break;
					case page3:
					{
						LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
						LCD_SetBackColor(LCD_COLOR_BACK);
						LCD_DisplayStringLine(80,10,"025");
						LCD_DisplayStringLine(120,10,"026");
						LCD_DisplayStringLine(160,10,"027");
						LCD_DisplayStringLine(200,10,"028");
						LCD_DisplayStringLine(240,10,"029");
						LCD_DisplayStringLine(280,10,"030");
						LCD_DisplayStringLine(320,10,"031");
						LCD_DisplayStringLine(360,10,"032");
						minfocus_on(1,CH25MIN);
						minfocus_off(2,CH26MIN);
						minfocus_off(3,CH27MIN);
						minfocus_off(4,CH28MIN);
						minfocus_off(5,CH29MIN);
						minfocus_off(6,CH30MIN);
						minfocus_off(7,CH31MIN);
						minfocus_off(8,CH32MIN);
						
						maxfocus_off(1,CH25MAX);
						maxfocus_off(2,CH26MAX);
						maxfocus_off(3,CH27MAX);
						maxfocus_off(4,CH28MAX);
						maxfocus_off(5,CH29MAX);
						maxfocus_off(6,CH30MAX);
						maxfocus_off(7,CH31MAX);
						maxfocus_off(8,CH32MAX);
						
						op_flag = set_min1;
						spt_page = page4;
					}break;
					case page4:
					{
						LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
						LCD_SetBackColor(LCD_COLOR_BACK);
						LCD_DisplayStringLine(80,10,"033");
						LCD_DisplayStringLine(120,10,"034");
						LCD_DisplayStringLine(160,10,"035");
						LCD_DisplayStringLine(200,10,"036");
						LCD_DisplayStringLine(240,10,"037");
						LCD_DisplayStringLine(280,10,"038");
						LCD_DisplayStringLine(320,10,"039");
						LCD_DisplayStringLine(360,10,"040");
						minfocus_on(1,CH33MIN);
						minfocus_off(2,CH34MIN);
						minfocus_off(3,CH35MIN);
						minfocus_off(4,CH36MIN);
						minfocus_off(5,CH37MIN);
						minfocus_off(6,CH38MIN);
						minfocus_off(7,CH39MIN);
						minfocus_off(8,CH40MIN);
						
						maxfocus_off(1,CH33MAX);
						maxfocus_off(2,CH34MAX);
						maxfocus_off(3,CH35MAX);
						maxfocus_off(4,CH36MAX);
						maxfocus_off(5,CH37MAX);
						maxfocus_off(6,CH38MAX);
						maxfocus_off(7,CH39MAX);
						maxfocus_off(8,CH40MAX);
						
						op_flag = set_min1;
						spt_page = page5;
					}break;
					case page5:
					{
						LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
						LCD_SetBackColor(LCD_COLOR_BACK);
						LCD_DisplayStringLine(80,10,"001");
						LCD_DisplayStringLine(120,10,"002");
						LCD_DisplayStringLine(160,10,"003");
						LCD_DisplayStringLine(200,10,"004");
						LCD_DisplayStringLine(240,10,"005");
						LCD_DisplayStringLine(280,10,"006");
						LCD_DisplayStringLine(320,10,"007");
						LCD_DisplayStringLine(360,10,"008");
						minfocus_on(1,CH1MIN);
						minfocus_off(2,CH2MIN);
						minfocus_off(3,CH3MIN);
						minfocus_off(4,CH4MIN);
						minfocus_off(5,CH5MIN);
						minfocus_off(6,CH6MIN);
						minfocus_off(7,CH7MIN);
						minfocus_off(8,CH8MIN);
						
						maxfocus_off(1,CH1MAX);
						maxfocus_off(2,CH2MAX);
						maxfocus_off(3,CH3MAX);
						maxfocus_off(4,CH4MAX);
						maxfocus_off(5,CH5MAX);
						maxfocus_off(6,CH6MAX);
						maxfocus_off(7,CH7MAX);
						maxfocus_off(8,CH8MAX);
						
						op_flag = set_min1;
						spt_page = page1;
					}break;
				}
			}
		}break;
		case calibrate:
		{
			if(CHNUM > 8)
			{
				switch(cor_page)
				{
					case page1:
					{
						LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
						LCD_SetBackColor(LCD_COLOR_BACK);
						LCD_DisplayStringLine(80,10,"009");
						LCD_DisplayStringLine(120,10,"010");
						LCD_DisplayStringLine(160,10,"011");
						LCD_DisplayStringLine(200,10,"012");
						LCD_DisplayStringLine(240,10,"013");
						LCD_DisplayStringLine(280,10,"014");
						LCD_DisplayStringLine(320,10,"015");
						LCD_DisplayStringLine(360,10,"016");
						minfocus_off(1,CH9TEMP);
						minfocus_off(2,CH10TEMP);
						minfocus_off(3,CH11TEMP);
						minfocus_off(4,CH12TEMP);
						minfocus_off(5,CH13TEMP);
						minfocus_off(6,CH14TEMP);
						minfocus_off(7,CH15TEMP);
						minfocus_off(8,CH16TEMP);
						
						maxfocus_on(1,COR9);
						maxfocus_off(2,COR10);
						maxfocus_off(3,COR11);
						maxfocus_off(4,COR12);
						maxfocus_off(5,COR13);
						maxfocus_off(6,COR14);
						maxfocus_off(7,COR15);
						maxfocus_off(8,COR16);
						
						op_flag = set_c1;
						cor_page = page2;
					}break;
					case page2:
					{
						if(CHNUM > 16)
						{
							LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
							LCD_SetBackColor(LCD_COLOR_BACK);
							LCD_DisplayStringLine(80,10,"017");
							LCD_DisplayStringLine(120,10,"018");
							LCD_DisplayStringLine(160,10,"019");
							LCD_DisplayStringLine(200,10,"020");
							LCD_DisplayStringLine(240,10,"021");
							LCD_DisplayStringLine(280,10,"022");
							LCD_DisplayStringLine(320,10,"023");
							LCD_DisplayStringLine(360,10,"024");
							minfocus_off(1,CH17TEMP);
							minfocus_off(2,CH18TEMP);
							minfocus_off(3,CH19TEMP);
							minfocus_off(4,CH20TEMP);
							minfocus_off(5,CH21TEMP);
							minfocus_off(6,CH22TEMP);
							minfocus_off(7,CH23TEMP);
							minfocus_off(8,CH24TEMP);
							
							maxfocus_on(1,COR17);
							maxfocus_off(2,COR18);
							maxfocus_off(3,COR19);
							maxfocus_off(4,COR20);
							maxfocus_off(5,COR21);
							maxfocus_off(6,COR22);
							maxfocus_off(7,COR23);
							maxfocus_off(8,COR24);
							
							op_flag = set_c1;
							cor_page = page3;
						}else{
							LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
							LCD_SetBackColor(LCD_COLOR_BACK);
							LCD_DisplayStringLine(80,10,"001");
							LCD_DisplayStringLine(120,10,"002");
							LCD_DisplayStringLine(160,10,"003");
							LCD_DisplayStringLine(200,10,"004");
							LCD_DisplayStringLine(240,10,"005");
							LCD_DisplayStringLine(280,10,"006");
							LCD_DisplayStringLine(320,10,"007");
							LCD_DisplayStringLine(360,10,"008");
							minfocus_off(1,CH1TEMP);
							minfocus_off(2,CH2TEMP);
							minfocus_off(3,CH3TEMP);
							minfocus_off(4,CH4TEMP);
							minfocus_off(5,CH5TEMP);
							minfocus_off(6,CH6TEMP);
							minfocus_off(7,CH7TEMP);
							minfocus_off(8,CH8TEMP);
							
							maxfocus_on(1,COR1);
							maxfocus_off(2,COR2);
							maxfocus_off(3,COR3);
							maxfocus_off(4,COR4);
							maxfocus_off(5,COR5);
							maxfocus_off(6,COR6);
							maxfocus_off(7,COR7);
							maxfocus_off(8,COR8);
							
							op_flag = set_min1;
							cor_page = page1;
						}
					}break;
					case page3:
					{
						LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
						LCD_SetBackColor(LCD_COLOR_BACK);
						LCD_DisplayStringLine(80,10,"025");
						LCD_DisplayStringLine(120,10,"026");
						LCD_DisplayStringLine(160,10,"027");
						LCD_DisplayStringLine(200,10,"028");
						LCD_DisplayStringLine(240,10,"029");
						LCD_DisplayStringLine(280,10,"030");
						LCD_DisplayStringLine(320,10,"031");
						LCD_DisplayStringLine(360,10,"032");
						minfocus_off(1,CH25TEMP);
						minfocus_off(2,CH26TEMP);
						minfocus_off(3,CH27TEMP);
						minfocus_off(4,CH28TEMP);
						minfocus_off(5,CH29TEMP);
						minfocus_off(6,CH30TEMP);
						minfocus_off(7,CH31TEMP);
						minfocus_off(8,CH32TEMP);
						
						maxfocus_on(1,COR25);
						maxfocus_off(2,COR26);
						maxfocus_off(3,COR27);
						maxfocus_off(4,COR28);
						maxfocus_off(5,COR29);
						maxfocus_off(6,COR30);
						maxfocus_off(7,COR31);
						maxfocus_off(8,COR32);
						
						op_flag = set_c1;
						cor_page = page4;
					}break;
					case page4:
					{
						LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
						LCD_SetBackColor(LCD_COLOR_BACK);
						LCD_DisplayStringLine(80,10,"033");
						LCD_DisplayStringLine(120,10,"034");
						LCD_DisplayStringLine(160,10,"035");
						LCD_DisplayStringLine(200,10,"036");
						LCD_DisplayStringLine(240,10,"037");
						LCD_DisplayStringLine(280,10,"038");
						LCD_DisplayStringLine(320,10,"039");
						LCD_DisplayStringLine(360,10,"040");
						minfocus_off(1,CH33TEMP);
						minfocus_off(2,CH34TEMP);
						minfocus_off(3,CH35TEMP);
						minfocus_off(4,CH36TEMP);
						minfocus_off(5,CH37TEMP);
						minfocus_off(6,CH38TEMP);
						minfocus_off(7,CH39TEMP);
						minfocus_off(8,CH40TEMP);
						
						maxfocus_on(1,COR33);
						maxfocus_off(2,COR34);
						maxfocus_off(3,COR35);
						maxfocus_off(4,COR36);
						maxfocus_off(5,COR37);
						maxfocus_off(6,COR38);
						maxfocus_off(7,COR39);
						maxfocus_off(8,COR40);
						
						op_flag = set_c1;
						cor_page = page5;
					}break;
					case page5:
					{
						LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
						LCD_SetBackColor(LCD_COLOR_BACK);
						LCD_DisplayStringLine(80,10,"001");
						LCD_DisplayStringLine(120,10,"002");
						LCD_DisplayStringLine(160,10,"003");
						LCD_DisplayStringLine(200,10,"004");
						LCD_DisplayStringLine(240,10,"005");
						LCD_DisplayStringLine(280,10,"006");
						LCD_DisplayStringLine(320,10,"007");
						LCD_DisplayStringLine(360,10,"008");
						minfocus_off(1,CH1TEMP);
						minfocus_off(2,CH2TEMP);
						minfocus_off(3,CH3TEMP);
						minfocus_off(4,CH4TEMP);
						minfocus_off(5,CH5TEMP);
						minfocus_off(6,CH6TEMP);
						minfocus_off(7,CH7TEMP);
						minfocus_off(8,CH8TEMP);
						
						maxfocus_on(1,COR1);
						maxfocus_off(2,COR2);
						maxfocus_off(3,COR3);
						maxfocus_off(4,COR4);
						maxfocus_off(5,COR5);
						maxfocus_off(6,COR6);
						maxfocus_off(7,COR7);
						maxfocus_off(8,COR8);
						
						op_flag = set_min1;
						cor_page = page1;
					}break;
				}
			}
			if(LANG == chs)
			{
				DrawInstruction("修正值");
			}else{
				DrawInstruction("Input Temperature");
			}
		}break;
		case sysinfo:
		{
			JumpBoot(55);
		}break;
		case graph:
		{
			page_gset();
		}break;
		case graphset:
		{
			page_gset();
		}break;
		case history:
		{
			bit_flag = 1;
			input_search("0");
			del_num();
		}break;
		case hisfile:
		{
			Create_His_File();
		}break;
	}
}

/*按键功能函数ENTER*/
void ENTER_HANDLE(void)
{
	switch(page_flag)
	{
		case display:
		{
			switch(op_flag)
			{
				case home_type:
				{
					DrawType(1);
//					DrawMenu();//重绘菜单栏
//					LCD_SetTextColor(LCD_COLOR_YELLOW);
//					LCD_SetBackColor(LCD_COLOR_BLACK);
//					LCD_DisplayStringLine(450,35,"TC-T");
//					LCD_DisplayStringLine(450,157,"TC-K");
//					LCD_DisplayStringLine(450,295,"TC-J");
//					LCD_DisplayStringLine(450,420,"TC-N");
//					if(LANG == chs)
//					{
//						LCD_DisplayStringLine(450,550,"更多");
//					}else{
//						LCD_DisplayStringLine(450,550,"MORE");
//					}
					op_flag = type_1;
				}break;
				case set_font:
				{
					DrawMenu();//重绘菜单栏
					LCD_SetTextColor(LCD_COLOR_YELLOW);
					LCD_SetBackColor(LCD_COLOR_BLACK);
					if(LANG == chs)
					{
						LCD_DisplayStringLine(445,48,"小");
						LCD_DisplayStringLine(445,173,"中");
						LCD_DisplayStringLine(445,298,"大");
					}else{
						LCD_DisplayStringLine(445,48,"S");
						LCD_DisplayStringLine(445,173,"M");
						LCD_DisplayStringLine(445,298,"L");
					}
					op_sw = op_on;
				}break;
				case home_ch1:
				{
					
					if(ch_page == page1)
					{
						if(FONT == big)
						{
							if(CH1_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(100,10,"001");
								CH1_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(100,10,"001");
								CH1_SW = ch_on;
							}
						}else if(FONT == middle){
							if(CH1_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(100,10,"001");
								CH1_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(100,10,"001");
								CH1_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH1_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(100,10,"001");
								CH1_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(100,10,"001");
								CH1_SW = ch_on;
							}
						}
					}
					else if(ch_page == page2){
						if(FONT == big)
						{
							if(CH9_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(100,10,"009");
								CH9_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(100,10,"009");
								CH9_SW = ch_on;
							}
						}else if(FONT == middle){
							if(CH25_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(100,10,"025");
								CH25_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(100,10,"025");
								CH25_SW = ch_on;
							}
						}
					}else if(ch_page == page3){
						if(CH17_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(100,10,"017");
							CH17_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(100,10,"017");
							CH17_SW = ch_on;
						}						
					}else if(ch_page == page4){
						if(CH25_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(100,10,"025");
							CH25_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(100,10,"025");
							CH25_SW = ch_on;
						}
					}else if(ch_page == page5){
						if(CH33_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(100,10,"033");
							CH33_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(100,10,"033");
							CH33_SW = ch_on;
						}
					}
					Save_flag();
				}break;
				case home_ch2:
				{
					if(ch_page == page1)
					{
						if(FONT == big)
						{
							if(CH2_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(170+10,10,"002");
								CH2_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(170+10,10,"002");
								CH2_SW = ch_on;
							}
						}else if(FONT == middle){
							if(CH2_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(140,10,"002");
								CH2_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(140,10,"002");
								CH2_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH2_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(140,10,"002");
								CH2_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(140,10,"002");
								CH2_SW = ch_on;
							}
						}

					}
					else if(ch_page == page2){
						if(FONT == big)
						{
							if(CH10_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(170+10,10,"010");
								CH10_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(170+10,10,"010");
								CH10_SW = ch_on;
							}
						}else if(FONT == middle){
							if(CH26_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(140,10,"026");
								CH26_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(140,10,"026");
								CH26_SW = ch_on;
							}
						}
					}else if(ch_page == page3){
						if(CH18_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(170+10,10,"018");
							CH18_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(170+10,10,"018");
							CH18_SW = ch_on;
						}						
					}else if(ch_page == page4){
						if(CH26_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(170+10,10,"026");
							CH26_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(170+10,10,"026");
							CH26_SW = ch_on;
						}
					}else if(ch_page == page5){
						if(CH34_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(170+10,10,"034");
							CH34_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(170+10,10,"034");
							CH34_SW = ch_on;
						}
					}
					Save_flag();
				}break;
				case home_ch3:
				{
					if(ch_page == page1)
					{
						if(FONT == big)
						{
							if(CH3_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(240+20,10,"003");
								CH3_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(240+20,10,"003");
								CH3_SW = ch_on;
							}
						}else if(FONT == middle){
							if(CH3_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(180,10,"003");
								CH3_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(180,10,"003");
								CH3_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH3_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(180,10,"003");
								CH3_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(180,10,"003");
								CH3_SW = ch_on;
							}
						}
					}
					else if(ch_page == page2){
						if(FONT == big)
						{
							if(CH11_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(240+20,10,"011");
								CH11_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(240+20,10,"011");
								CH11_SW = ch_on;
							}
						}else if(FONT == middle){
							if(CH27_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(180,10,"027");
								CH27_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(180,10,"027");
								CH27_SW = ch_on;
							}
						}
					}else if(ch_page == page3){
						if(CH19_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(240+20,10,"019");
							CH19_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(240+20,10,"019");
							CH19_SW = ch_on;
						}						
					}else if(ch_page == page4){
						if(CH27_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(240+20,10,"027");
							CH27_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(240+20,10,"027");
							CH27_SW = ch_on;
						}
					}else if(ch_page == page5){
						if(CH35_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(240+20,10,"035");
							CH35_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(240+20,10,"035");
							CH35_SW = ch_on;
						}
					}
					Save_flag();
				}break;
				case home_ch4:
				{
					if(ch_page == page1)
					{
						if(FONT == big)
						{
							if(CH4_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(310+30,10,"004");
								CH4_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(310+30,10,"004");
								CH4_SW = ch_on;
							}
						}else if(FONT == middle){
							if(CH4_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(220,10,"004");
								CH4_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(220,10,"004");
								CH4_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH4_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(220,10,"004");
								CH4_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(220,10,"004");
								CH4_SW = ch_on;
							}
						}
					}
					else if(ch_page == page2){
						if(FONT == big)
						{
							if(CH12_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(310+30,10,"012");
								CH12_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(310+30,10,"012");
								CH12_SW = ch_on;
							}
						}else if(FONT == middle){
							if(CH28_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(220,10,"028");
								CH28_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(220,10,"028");
								CH28_SW = ch_on;
							}
						}
					}else if(ch_page == page3){
						if(CH20_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(310+30,10,"020");
							CH20_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(310+30,10,"020");
							CH20_SW = ch_on;
						}						
					}else if(ch_page == page4){
						if(CH28_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(310+30,10,"028");
							CH28_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(310+30,10,"028");
							CH28_SW = ch_on;
						}
					}else if(ch_page == page5){
						if(CH36_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(310+30,10,"036");
							CH36_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(310+30,10,"036");
							CH36_SW = ch_on;
						}
					}
					Save_flag();
				}break;
				case home_ch5:
				{
					if(ch_page == page1)
					{
						if(FONT == big)
						{
							if(CH5_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(100,330,"005");
								CH5_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(100,330,"005");
								CH5_SW = ch_on;
							}
						}else if(FONT == middle){
							if(CH5_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(260,10,"005");
								CH5_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(260,10,"005");
								CH5_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH5_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(260,10,"005");
								CH5_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(260,10,"005");
								CH5_SW = ch_on;
							}
						}
					}
					else if(ch_page == page2){
						if(FONT == big)
						{
							if(CH13_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(100,330,"013");
								CH13_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(100,330,"013");
								CH13_SW = ch_on;
							}
						}else if(FONT == middle){
							if(CH29_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(260,10,"029");
								CH29_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(260,10,"029");
								CH29_SW = ch_on;
							}
						}
					}else if(ch_page == page3){
						if(CH21_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(100,330,"021");
							CH21_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(100,330,"021");
							CH21_SW = ch_on;
						}						
					}else if(ch_page == page4){
						if(CH29_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(100,330,"029");
							CH29_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(100,330,"029");
							CH29_SW = ch_on;
						}
					}else if(ch_page == page5){
						if(CH37_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(100,330,"037");
							CH37_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(100,330,"037");
							CH37_SW = ch_on;
						}
					}
					Save_flag();
				}break;
				case home_ch6:
				{
					if(ch_page == page1)
					{
						if(FONT == big)
						{
							if(CH6_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(170+10,330,"006");
								CH6_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(170+10,330,"006");
								CH6_SW = ch_on;
							}
						}else if(FONT == middle){
							if(CH6_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(300,10,"006");
								CH6_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(300,10,"006");
								CH6_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH6_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(300,10,"006");
								CH6_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(300,10,"006");
								CH6_SW = ch_on;
							}
						}
					}
					else if(ch_page == page2){
						if(FONT == big)
						{
							if(CH14_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(170+10,330,"014");
								CH14_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(170+10,330,"014");
								CH14_SW = ch_on;
							}
						}else if(FONT == middle){
							if(CH30_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(300,10,"030");
								CH30_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(300,10,"030");
								CH30_SW = ch_on;
							}
						}
					}else if(ch_page == page3){
						if(CH22_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(170+10,330,"022");
							CH22_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(170+10,330,"022");
							CH22_SW = ch_on;
						}
					}else if(ch_page == page4){
						if(CH30_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(170+10,330,"030");
							CH30_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(170+10,330,"030");
							CH30_SW = ch_on;
						}
					}else if(ch_page == page5){
						if(CH38_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(170+10,330,"038");
							CH38_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(170+10,330,"038");
							CH38_SW = ch_on;
						}
					}
					Save_flag();
				}break;
				case home_ch7:
				{
					if(ch_page == page1)
					{
						if(FONT == big)
						{
							if(CH7_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(240+20,330,"007");
								CH7_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(240+20,330,"007");
								CH7_SW = ch_on;
							}
						}else if(FONT == middle){
							if(CH7_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(340,10,"006");
								CH7_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(340,10,"007");
								CH7_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH7_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(340,10,"007");
								CH7_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(340,10,"007");
								CH7_SW = ch_on;
							}
						}
					}
					else if(ch_page == page2){
						if(FONT == big)
						{
							if(CH15_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(240+20,330,"015");
								CH15_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(240+20,330,"015");
								CH15_SW = ch_on;
							}
						}else if(FONT == middle){
							if(CH31_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(340,10,"031");
								CH31_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(340,10,"031");
								CH31_SW = ch_on;
							}
						}
					}else if(ch_page == page3){
						if(CH23_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(240+20,330,"023");
							CH23_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(240+20,330,"023");
							CH23_SW = ch_on;
						}						
					}else if(ch_page == page4){
						if(CH31_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(240+20,330,"031");
							CH31_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(240+20,330,"031");
							CH31_SW = ch_on;
						}
					}else if(ch_page == page5){
						if(CH39_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(240+20,330,"039");
							CH39_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(240+20,330,"039");
							CH39_SW = ch_on;
						}
					}
					Save_flag();
				}break;
				case home_ch8:
				{
					if(ch_page == page1)
					{
						if(FONT == big)
						{
							if(CH8_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(310+30,330,"008");
								CH8_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(310+30,330,"008");
								CH8_SW = ch_on;
							}
						}else if(FONT == middle){
							if(CH8_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(380,10,"008");
								CH8_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(380,10,"008");
								CH8_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH8_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(380,10,"008");
								CH8_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(380,10,"008");
								CH8_SW = ch_on;
							}
						}
					}
					else if(ch_page == page2){
						if(FONT == big)
						{
							if(CH16_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(310+30,330,"016");
								CH16_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								LCD_DisplayStringLine_48(310+30,330,"016");
								CH16_SW = ch_on;
							}
						}else if(FONT == middle){
							if(CH32_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(380,10,"032");
								CH32_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(380,10,"032");
								CH32_SW = ch_on;
							}
						}
					}else if(ch_page == page3){
						if(CH24_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(310+30,330,"024");
							CH24_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(310+30,330,"024");
							CH24_SW = ch_on;
						}						
					}else if(ch_page == page4){
						if(CH32_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(310+30,330,"032");
							CH32_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(310+30,330,"032");
							CH32_SW = ch_on;
						}
					}else if(ch_page == page5){
						if(CH40_SW == ch_on)
						{
							LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(310+30,330,"040");
							CH40_SW = ch_off;
						}else{
							LCD_SetBackColor(LCD_COLOR_HLT);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine_48(310+30,330,"040");
							CH40_SW = ch_on;
						}
					}
					Save_flag();
				}break;
				case home_ch9:
				{
					if(ch_page == page1)
					{
						if(FONT == middle)
						{
							if(CH9_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(100,210,"009");
								CH9_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(100,210,"009");
								CH9_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH9_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(100,134,"009");
								CH9_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(100,134,"009");
								CH9_SW = ch_on;
							}
						}
					}else if(ch_page == page2){
						if(FONT == middle)
						{
							if(CH33_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(100,210,"033");
								CH33_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(100,210,"033");
								CH33_SW = ch_on;
							}
						}else if(FONT == small){
							
						}
					}
					Save_flag();
				}break;
				case home_ch10:
				{
					if(ch_page == page1)
					{
						if(FONT == middle)
						{
							if(CH10_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(140,210,"010");
								CH10_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(140,210,"010");
								CH10_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH10_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(140,134,"010");
								CH10_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(140,134,"010");
								CH10_SW = ch_on;
							}
						}
					}else if(ch_page == page2){
						if(FONT == middle)
						{
							if(CH34_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(140,210,"034");
								CH34_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(140,210,"034");
								CH34_SW = ch_on;
							}
						}else if(FONT == small){
							
						}
					}
					Save_flag();
				}break;
				case home_ch11:
				{
					if(ch_page == page1)
					{
						if(FONT == middle)
						{
							if(CH11_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(180,210,"011");
								CH11_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(180,210,"011");
								CH11_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH11_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(180,134,"011");
								CH11_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(180,134,"011");
								CH11_SW = ch_on;
							}
						}
					}else if(ch_page == page2){
						if(FONT == middle)
						{
							if(CH35_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(180,210,"035");
								CH35_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(180,210,"035");
								CH35_SW = ch_on;
							}
						}else if(FONT == small){
							
						}
					}
					Save_flag();
				}break;
				case home_ch12:
				{
					if(ch_page == page1)
					{
						if(FONT == middle)
						{
							if(CH12_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(220,210,"012");
								CH12_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(220,210,"012");
								CH12_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH12_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(220,134,"012");
								CH12_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(220,134,"012");
								CH12_SW = ch_on;
							}
						}
					}else if(ch_page == page2){
						if(FONT == middle)
						{
							if(CH36_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(220,210,"036");
								CH36_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(220,210,"036");
								CH36_SW = ch_on;
							}
						}else if(FONT == small){
							
						}
					}
					Save_flag();
				}break;
				case home_ch13:
				{
					if(ch_page == page1)
					{
						if(FONT == middle)
						{
							if(CH13_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(260,210,"013");
								CH13_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(260,210,"013");
								CH13_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH13_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(260,134,"013");
								CH13_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(260,134,"013");
								CH13_SW = ch_on;
							}
						}
					}else if(ch_page == page2){
						if(FONT == middle)
						{
							if(CH37_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(260,210,"037");
								CH37_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(260,210,"037");
								CH37_SW = ch_on;
							}
						}else if(FONT == small){
							
						}
					}
					Save_flag();
				}break;
				case home_ch14:
				{
					if(ch_page == page1)
					{
						if(FONT == middle)
						{
							if(CH14_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(300,210,"014");
								CH14_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(300,210,"014");
								CH14_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH14_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(300,134,"014");
								CH14_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(300,134,"014");
								CH14_SW = ch_on;
							}
						}
					}else if(ch_page == page2){
						if(FONT == middle)
						{
							if(CH38_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(300,210,"038");
								CH38_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(300,210,"038");
								CH38_SW = ch_on;
							}
						}else if(FONT == small){
							
						}
					}
					Save_flag();
				}break;
				case home_ch15:
				{
					if(ch_page == page1)
					{
						if(FONT == middle)
						{
							if(CH15_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(340,210,"015");
								CH15_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(340,210,"015");
								CH15_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH15_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(340,134,"015");
								CH15_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(340,134,"015");
								CH15_SW = ch_on;
							}
						}
					}else if(ch_page == page2){
						if(FONT == middle)
						{
							if(CH39_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(340,210,"039");
								CH39_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(340,210,"039");
								CH39_SW = ch_on;
							}
						}else if(FONT == small){
							
						}
					}
					Save_flag();
				}break;
				case home_ch16:
				{
					if(ch_page == page1)
					{
						if(FONT == middle)
						{
							if(CH16_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(380,210,"016");
								CH16_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(380,210,"016");
								CH16_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH16_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(380,134,"016");
								CH16_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(380,134,"016");
								CH16_SW = ch_on;
							}
						}
					}else if(ch_page == page2){
						if(FONT == middle)
						{
							if(CH40_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(380,210,"040");
								CH40_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(380,210,"040");
								CH40_SW = ch_on;
							}
						}else if(FONT == small){
							
						}
					}
					Save_flag();
				}break;
				case home_ch17:
				{
					if(ch_page == page1)
					{
						if(FONT == middle)
						{
							if(CH17_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(100,410,"017");
								CH17_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(100,410,"017");
								CH17_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH17_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(100,258,"017");
								CH17_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(100,258,"017");
								CH17_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;				
				case home_ch18:
				{
					if(ch_page == page1)
					{
						if(FONT == middle)
						{
							if(CH18_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(140,410,"018");
								CH18_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(140,410,"018");
								CH18_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH18_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(140,258,"018");
								CH18_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(140,258,"018");
								CH18_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch19:
				{
					if(ch_page == page1)
					{
						if(FONT == middle)
						{
							if(CH19_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(180,410,"019");
								CH19_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(180,410,"019");
								CH19_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH19_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(180,258,"019");
								CH19_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(180,258,"019");
								CH19_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch20:
				{
					if(ch_page == page1)
					{
						if(FONT == middle)
						{
							if(CH20_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(220,410,"020");
								CH20_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(220,410,"020");
								CH20_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH20_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(220,258,"020");
								CH20_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(220,258,"020");
								CH20_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch21:
				{
					if(ch_page == page1)
					{
						if(FONT == middle)
						{
							if(CH21_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(260,410,"021");
								CH21_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(260,410,"021");
								CH21_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH21_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(260,258,"021");
								CH21_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(260,258,"021");
								CH21_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch22:
				{
					if(ch_page == page1)
					{
						if(FONT == middle)
						{
							if(CH22_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(300,410,"022");
								CH22_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(300,410,"022");
								CH22_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH22_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(300,258,"022");
								CH22_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(300,258,"022");
								CH22_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch23:
				{
					if(ch_page == page1)
					{
						if(FONT == middle)
						{
							if(CH23_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(340,410,"023");
								CH23_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(340,410,"023");
								CH23_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH23_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(340,258,"023");
								CH23_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(340,258,"023");
								CH23_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch24:
				{
					if(ch_page == page1)
					{
						if(FONT == middle)
						{
							if(CH24_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(380,410,"024");
								CH24_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_M(380,410,"024");
								CH24_SW = ch_on;
							}
						}else if(FONT == small){
							if(CH24_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(380,258,"024");
								CH24_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(380,258,"024");
								CH24_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch25:
				{
					if(ch_page == page1)
					{
						if(FONT == small){
							if(CH25_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(100,382,"025");
								CH25_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(100,382,"025");
								CH25_SW = ch_on;
							}
						}
					}
				}break;
				case home_ch26:
				{
					if(ch_page == page1)
					{
						if(FONT == small){
							if(CH26_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(140,382,"026");
								CH26_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(140,382,"026");
								CH26_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch27:
				{
					if(ch_page == page1)
					{
						if(FONT == small){
							if(CH27_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(180,382,"027");
								CH27_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(180,382,"027");
								CH27_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch28:
				{
					if(ch_page == page1)
					{
						if(FONT == small){
							if(CH28_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(220,382,"028");
								CH28_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(220,382,"028");
								CH28_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch29:
				{
					if(ch_page == page1)
					{
						if(FONT == small){
							if(CH29_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(260,382,"029");
								CH29_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(260,382,"029");
								CH29_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch30:
				{
					if(ch_page == page1)
					{
						if(FONT == small){
							if(CH30_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(300,382,"030");
								CH30_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(300,382,"030");
								CH30_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch31:
				{
					if(ch_page == page1)
					{
						if(FONT == small){
							if(CH31_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(340,382,"031");
								CH31_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(340,382,"031");
								CH31_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch32:
				{
					if(ch_page == page1)
					{
						if(FONT == small){
							if(CH32_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(380,382,"032");
								CH32_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(380,382,"032");
								CH32_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch33:
				{
					if(ch_page == page1)
					{
						if(FONT == small){
							if(CH33_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(100,506,"033");
								CH33_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(100,506,"033");
								CH33_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch34:
				{
					if(ch_page == page1)
					{
						if(FONT == small){
							if(CH34_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(140,506,"034");
								CH34_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(140,506,"034");
								CH34_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch35:
				{
					if(ch_page == page1)
					{
						if(FONT == small){
							if(CH35_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(180,506,"035");
								CH35_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(180,506,"035");
								CH35_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch36:
				{
					if(ch_page == page1)
					{
						if(FONT == small){
							if(CH36_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(220,506,"036");
								CH36_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(220,506,"036");
								CH36_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch37:
				{
					if(ch_page == page1)
					{
						if(FONT == small){
							if(CH37_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(260,506,"037");
								CH37_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(260,506,"037");
								CH37_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch38:
				{
					if(ch_page == page1)
					{
						if(FONT == small){
							if(CH38_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(300,506,"038");
								CH38_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(300,506,"038");
								CH38_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch39:
				{
					if(ch_page == page1)
					{
						if(FONT == small){
							if(CH39_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(340,506,"039");
								CH39_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(340,506,"039");
								CH39_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
				case home_ch40:
				{
					if(ch_page == page1)
					{
						if(FONT == small){
							if(CH40_SW == ch_on)
							{
								LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(380,506,"040");
								CH40_SW = ch_off;
							}else{
								LCD_SetBackColor(LCD_COLOR_HLT);
								LCD_SetTextColor(LCD_COLOR_BLACK);
								DISP_CNL_S(380,506,"040");
								CH40_SW = ch_on;
							}
						}
					}
					Save_flag();
				}break;
			}
		}break;
		case settings:
		{
			switch(op_flag)
			{
				case set_filter:
				{
					DrawMenu();//重绘菜单栏
					LCD_SetTextColor(LCD_COLOR_YELLOW);
					LCD_SetBackColor(LCD_COLOR_BLACK);
					if(LANG == chs)
					{
						LCD_DisplayStringLine(445,35,"打开");
						LCD_DisplayStringLine(445,157,"关闭");
					}else{
						LCD_DisplayStringLine(445,35,"ON");
						LCD_DisplayStringLine(445,157,"OFF");
					}
					op_sw = op_on;
				}break;
				case set_beep:
				{
					DrawMenu();//重绘菜单栏
					LCD_SetTextColor(LCD_COLOR_YELLOW);
					LCD_SetBackColor(LCD_COLOR_BLACK);
					if(LANG == chs)
					{
						LCD_DisplayStringLine(445,35,"打开");
						LCD_DisplayStringLine(445,157,"关闭");
					}else{
						LCD_DisplayStringLine(445,35,"ON");
						LCD_DisplayStringLine(445,157,"OFF");
					}
					op_sw = op_on;
				}break;
				case repeat:
				{
					DrawMenu();//重绘菜单栏
					LCD_SetTextColor(LCD_COLOR_YELLOW);
					LCD_SetBackColor(LCD_COLOR_BLACK);
					if(LANG == chs)
					{
						LCD_DisplayStringLine(445,35,"打开");
						LCD_DisplayStringLine(445,157,"关闭");
					}else{
						LCD_DisplayStringLine(445,35,"ON");
						LCD_DisplayStringLine(445,157,"OFF");
					}
					op_sw = op_on;
				}break;
				case timetrig:
				{
					DrawMenu();//重绘菜单栏
					LCD_SetTextColor(LCD_COLOR_YELLOW);
					LCD_SetBackColor(LCD_COLOR_BLACK);
					if(LANG == chs)
					{
						LCD_DisplayStringLine(445,35,"打开");
						LCD_DisplayStringLine(445,157,"关闭");
					}else{
						LCD_DisplayStringLine(445,35,"ON");
						LCD_DisplayStringLine(445,157,"OFF");
					}
					op_sw = op_on;
				}break;
				case set_unit:
				{
					DrawMenu();//重绘菜单栏
					LCD_SetTextColor(LCD_COLOR_YELLOW);
					LCD_SetBackColor(LCD_COLOR_BLACK);
					DISP_CNL_S(445,48,"o");
					LCD_DisplayStringLine(445,53,"C");
					LCD_DisplayStringLine(445,173,"K");
					DISP_CNL_S(445,298,"o");
					LCD_DisplayStringLine(445,303,"F");
					op_sw = op_on;
				}break;
				
				case set_spd:
				{
					DrawMenu();//重绘菜单栏
					LCD_SetTextColor(LCD_COLOR_YELLOW);
					LCD_SetBackColor(LCD_COLOR_BLACK);
					if(LANG == chs)
					{
						LCD_DisplayStringLine(445,35,"快速");
						LCD_DisplayStringLine(445,157,"中速");
						LCD_DisplayStringLine(445,292,"慢速");
					}else{
						LCD_DisplayStringLine(445,35,"FAST");
						LCD_DisplayStringLine(445,157,"MED");
						LCD_DisplayStringLine(445,292,"SLOW");
					}
					op_sw = op_on;
				}break;
				case set_baud:
				{
					DrawMenu();//重绘菜单栏
					LCD_SetTextColor(LCD_COLOR_YELLOW);
					LCD_SetBackColor(LCD_COLOR_BLACK);
					LCD_DisplayStringLine(445,30,"9600");
					LCD_DisplayStringLine(445,150,"19200");
					LCD_DisplayStringLine(445,280,"38400");
					LCD_DisplayStringLine(445,405,"57600");
					LCD_DisplayStringLine(445,525,"115200");
					op_sw = op_on;
				}break;
				case set_timer:
				{
					SAVETIME = atoi(data);
					if(SAVETIME > 255)
					{
						SAVETIME = 255;
					}
					stimefocus_on(SAVETIME);
					Save_flag();
				}break;
			}
		}break;
		case separation:
		{
			u8 k;
			char dest[6];
			if(input_flag == 1)
			{
				if(op_flag < 64)
				{
					k = spt_page * 8 + (op_flag - 56);
					if(dot_flag == 0){
						TempLLimits[k] = atoi(data);					
					}else if(dot_flag != 0){
						memset(dest, '\0', sizeof(dest));
						strncpy(dest,data,dot_flag + 1);
						TempLLimits[k] = atof(dest);
					}
					if(TempLLimits[k] > TempHLimits[k] || TempLLimits[k] == TempHLimits[k] || TempLLimits[k] < -200)
					{
						TempLLimits[k] = -200;
					}
					minfocus_on(op_flag - 55,TempLLimits[k]);
				}else{
					k = spt_page * 8 + (op_flag - 64);
					if(dot_flag == 0){
						TempHLimits[k] = atoi(data);
					}else if(dot_flag != 0){
						memset(dest, '\0', sizeof(dest));
						strncpy(dest,data,dot_flag + 1);
						TempHLimits[k] = atof(dest);
					}
					if(TempHLimits[k] < TempLLimits[k] || TempHLimits[k] == TempLLimits[k] || TempHLimits[k] > 1800)
					{
						TempHLimits[k] = 1800;
					}
					maxfocus_on(op_flag - 63,TempHLimits[k]);
				}
				Save_flag();
			}
		}break;
		case sysset:
		{
			switch(op_flag)
			{
				case set_lang:
				{
					DrawMenu();
					Drawlang();
					op_sw = op_on;
				}break;
				case set_date:
				{
					DrawMenu();
					Drawdate();
					op_sw = op_on;
				}break;
				case set_time:
				{
					DrawMenu();
					Drawtime();
					op_sw = op_on;
				}break;
				case set_brt:
				{
					DrawMenu();
					DrawBrt();
					op_sw = op_on;
				}break;
				case set_dim:
				{
					DrawMenu();
					DrawDim();
					op_sw = op_on;
				}break;
				case set_touch:
				{
					DrawMenu();
					DrawTouch();
					op_sw = op_on;
				}break;
			}
		}break;
		case graph:
		{
			
		}break;
		case graphset:
		{
			char dest[6];
			switch(op_flag)
			{
				case set_yhigh:
				{
					if(dot_flag == 0){
						YHLIMIT = atoi(data);					
					}else if(dot_flag != 0){
						memset(dest, '\0', sizeof(dest));
						strncpy(dest,data,dot_flag + 1);
						YHLIMIT = atof(dest);
					}
					if(YHLIMIT > 1800 || YHLIMIT < YLLIMIT)
					{
						YHLIMIT = 1800;
					}
					yhfocus_on(YHLIMIT);
					Save_flag();
				}break;
				case set_ylow:
				{
					if(dot_flag == 0){
						YLLIMIT = atoi(data);					
					}else if(dot_flag != 0){
						memset(dest, '\0', sizeof(dest));
						strncpy(dest,data,dot_flag + 1);
						YHLIMIT = atof(dest);
					}
					if(YLLIMIT > YHLIMIT || YLLIMIT < -200)
					{
						YLLIMIT = -200;
					}
					ylfocus_on(YLLIMIT);
					Save_flag();
				}break;
				case set_multi:
				{
					MULTI = atoi(data);
					if(MULTI > 300)
					{
						MULTI = 300;
					}
					if(MULTI < 1)
					{
						MULTI = 1;
					}
					multifocus_on(MULTI);
					Save_flag();
				}break;
			}
		}break;	
		case calibrate:
		{
			u8 k;
			char dest[6];			
			if(input_flag == 1)
			{
				k = cor_page * 8 + (op_flag - 83);
				if(dot_flag == 0){
					Correction[k] = ch_temp[k] - atoi(data);
				}else if(dot_flag != 0){
					memset(dest, '\0', sizeof(dest));
					strncpy(dest,data,dot_flag + 1);
					Correction[k] = ch_temp[k] - atof(dest);
				}
				if(Correction[k] < -2200 || Correction[k] > 2200)
				{
					Correction[k] = 0;
				}
//				else{
//					offset[k] = ch_temp[k] - Correction[k];
//					ch_temp[k]-=offset[k];
//				}
				maxfocus_on(op_flag - 82,Correction[k]);				
			}
			Save_flag();
		}break;
		case history:
		{
			
		}break;
		case factory:
		{
			if(passverify == 0 && data[0] == '2' && data[1] == '0' && data[2] == '1' && data[3] == '9' && data[4] == '0' && data[5] == '5' && data[6] == '0' && data[7] == '8')
			{
				clear_input();
				passverify = 1;
				page_factory();
			}else{
				memcpy(SN,data,sizeof(SN));
				clear_input();
				Save_flag();
				LCD_SetTextColor(LCD_COLOR_WHITE);
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_DisplayStringLine(90,170,(uint8_t *)SN);
			}
		}
	}
}

/*按键功能函数UP*/
void UP_HANDLE(void)
{
	char buf[10];
	switch(page_flag)
	{
		case display:
		{
			switch(op_flag)
			{
				case home_type:
				{
					if(FONT == big)
					{
						LCD_SetTextColor(LCD_COLOR_YELLOW);
						LCD_SetBackColor(LCD_COLOR_BACK);
						if(TCTYPE == TCK)
						{
							LCD_DisplayStringLine(47,100,"TC-K");
						}else if(TCTYPE == TCT){
							LCD_DisplayStringLine(47,100,"TC-T");
						}else if(TCTYPE == TCJ){
							LCD_DisplayStringLine(47,100,"TC-J");
						}else if(TCTYPE == TCN){
							LCD_DisplayStringLine(47,100,"TC-N");
						}else if(TCTYPE == TCE){
							LCD_DisplayStringLine(47,100,"TC-E");
						}else if(TCTYPE == TCS){
							LCD_DisplayStringLine(47,100,"TC-S");
						}else if(TCTYPE == TCR){
							LCD_DisplayStringLine(47,100,"TC-R");
						}else if(TCTYPE == TCB){
							LCD_DisplayStringLine(47,100,"TC-B");
						}else if(TCTYPE == PT100){
							LCD_DisplayStringLine(47,100,"PT100");
						}else if(TCTYPE == V1){
							LCD_DisplayStringLine(47,100,"1~5V");
						}					
						
						
						
						if(ch_page == page1)
						{
							focus_on(CH8_SW);
							LCD_DisplayStringLine_48(310+30,10,"004");
						}else if(ch_page == page2){
							focus_on(CH12_SW);
							LCD_DisplayStringLine_48(310+30,10,"012");
						}else if(ch_page == page3){
							focus_on(CH20_SW);
							LCD_DisplayStringLine_48(310+30,10,"020");
						}else if(ch_page == page4){
							focus_on(CH28_SW);
							LCD_DisplayStringLine_48(310+30,10,"028");
						}else if(ch_page == page5){
							focus_on(CH36_SW);
							LCD_DisplayStringLine_48(310+30,10,"036");
						}
						if(LANG == chs)
						{
							DrawInstruction("通道设置");
						}else{
							DrawInstruction("Channel set");
						}
						op_flag = home_ch4;
					}else if(FONT == middle){
						LCD_SetTextColor(LCD_COLOR_YELLOW);
						LCD_SetBackColor(LCD_COLOR_BACK);
						if(TCTYPE == TCK)
						{
							LCD_DisplayStringLine(47,100,"TC-K");
						}else if(TCTYPE == TCT){
							LCD_DisplayStringLine(47,100,"TC-T");
						}else if(TCTYPE == TCJ){
							LCD_DisplayStringLine(47,100,"TC-J");
						}else if(TCTYPE == TCN){
							LCD_DisplayStringLine(47,100,"TC-N");
						}else if(TCTYPE == TCE){
							LCD_DisplayStringLine(47,100,"TC-E");
						}else if(TCTYPE == TCS){
							LCD_DisplayStringLine(47,100,"TC-S");
						}else if(TCTYPE == TCR){
							LCD_DisplayStringLine(47,100,"TC-R");
						}else if(TCTYPE == TCB){
							LCD_DisplayStringLine(47,100,"TC-B");
						}else if(TCTYPE == PT100){
							LCD_DisplayStringLine(47,100,"PT100");
						}else if(TCTYPE == V1){
							LCD_DisplayStringLine(47,100,"1~5V");
						}
						
						if(ch_page == page1)
						{
							focus_on(CH8_SW);
							DISP_CNL_M(380,10,"008");
						}else if(ch_page == page2){
							focus_on(CH32_SW);
							DISP_CNL_M(380,10,"032");
						}
						if(LANG == chs)
						{
							DrawInstruction("通道设置");
						}else{
							DrawInstruction("Channel set");
						}
						op_flag = home_ch8;
					}else if(FONT == small){
						LCD_SetTextColor(LCD_COLOR_YELLOW);
						LCD_SetBackColor(LCD_COLOR_BACK);
						if(TCTYPE == TCK)
						{
							LCD_DisplayStringLine(47,100,"TC-K");
						}else if(TCTYPE == TCT){
							LCD_DisplayStringLine(47,100,"TC-T");
						}else if(TCTYPE == TCJ){
							LCD_DisplayStringLine(47,100,"TC-J");
						}else if(TCTYPE == TCN){
							LCD_DisplayStringLine(47,100,"TC-N");
						}else if(TCTYPE == TCE){
							LCD_DisplayStringLine(47,100,"TC-E");
						}else if(TCTYPE == TCS){
							LCD_DisplayStringLine(47,100,"TC-S");
						}else if(TCTYPE == TCR){
							LCD_DisplayStringLine(47,100,"TC-R");
						}else if(TCTYPE == TCB){
							LCD_DisplayStringLine(47,100,"TC-B");
						}else if(TCTYPE == PT100){
							LCD_DisplayStringLine(47,100,"PT100");
						}else if(TCTYPE == V1){
							LCD_DisplayStringLine(47,100,"1~5V");
						}
						
						if(ch_page == page1)
						{
							focus_on(CH8_SW);
							DISP_CNL_S(380,10,"008");
						}
						if(LANG == chs)
						{
							DrawInstruction("通道设置");
						}else{
							DrawInstruction("Channel set");
						}
						op_flag = home_ch8;
					}
					DrawType(1);
				}break;
				case home_ch1:
				{
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_off(CH1_SW);
							LCD_DisplayStringLine_48(100,10,"001");
						}else if(ch_page == page2){
							focus_off(CH9_SW);
							LCD_DisplayStringLine_48(100,10,"009");
						}else if(ch_page == page3){
							focus_off(CH17_SW);
							LCD_DisplayStringLine_48(100,10,"017");
						}else if(ch_page == page4){
							focus_off(CH25_SW);
							LCD_DisplayStringLine_48(100,10,"025");
						}else if(ch_page == page5){
							focus_off(CH33_SW);
							LCD_DisplayStringLine_48(100,10,"033");
						}										
						LCD_SetTextColor(LCD_COLOR_BLACK);
						LCD_SetBackColor(LCD_COLOR_YELLOW);
						if(TCTYPE == TCK)
						{
							LCD_DisplayStringLine(47,100,"TC-K");
						}else if(TCTYPE == TCT){
							LCD_DisplayStringLine(47,100,"TC-T");
						}else if(TCTYPE == TCJ){
							LCD_DisplayStringLine(47,100,"TC-J");
						}else if(TCTYPE == TCN){
							LCD_DisplayStringLine(47,100,"TC-N");
						}else if(TCTYPE == TCE){
							LCD_DisplayStringLine(47,100,"TC-E");
						}else if(TCTYPE == TCS){
							LCD_DisplayStringLine(47,100,"TC-S");
						}else if(TCTYPE == TCR){
							LCD_DisplayStringLine(47,100,"TC-R");
						}else if(TCTYPE == TCB){
							LCD_DisplayStringLine(47,100,"TC-B");
						}else if(TCTYPE == PT100){
							LCD_DisplayStringLine(47,100,"PT100");
						}else if(TCTYPE == V1){
							LCD_DisplayStringLine(47,100,"1~5V");
						}						
						if(LANG == chs)
						{
							DrawInstruction("热电偶型号选择");
						}else{
							DrawInstruction("Thermocouple model");
						}
						op_flag = home_type;
					}else if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH1_SW);
							DISP_CNL_M(100,10,"001");
						}else if(ch_page == page2){
							focus_off(CH25_SW);
							DISP_CNL_M(100,10,"025");
						}
						
						LCD_SetTextColor(LCD_COLOR_BLACK);
						LCD_SetBackColor(LCD_COLOR_YELLOW);
						if(TCTYPE == TCK)
						{
							LCD_DisplayStringLine(47,100,"TC-K");
						}else if(TCTYPE == TCT){
							LCD_DisplayStringLine(47,100,"TC-T");
						}else if(TCTYPE == TCJ){
							LCD_DisplayStringLine(47,100,"TC-J");
						}else if(TCTYPE == TCN){
							LCD_DisplayStringLine(47,100,"TC-N");
						}else if(TCTYPE == TCE){
							LCD_DisplayStringLine(47,100,"TC-E");
						}else if(TCTYPE == TCS){
							LCD_DisplayStringLine(47,100,"TC-S");
						}else if(TCTYPE == TCR){
							LCD_DisplayStringLine(47,100,"TC-R");
						}else if(TCTYPE == TCB){
							LCD_DisplayStringLine(47,100,"TC-B");
						}else if(TCTYPE == PT100){
							LCD_DisplayStringLine(47,100,"PT100");
						}else if(TCTYPE == V1){
							LCD_DisplayStringLine(47,100,"1~5V");
						}
						if(LANG == chs)
						{
							DrawInstruction("热电偶型号选择");
						}else{
							DrawInstruction("Thermocouple model");
						}
						op_flag = home_type;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH1_SW);
							DISP_CNL_S(100,10,"001");
						}
						
						LCD_SetTextColor(LCD_COLOR_BLACK);
						LCD_SetBackColor(LCD_COLOR_YELLOW);
						if(TCTYPE == TCK)
						{
							LCD_DisplayStringLine(47,100,"TC-K");
						}else if(TCTYPE == TCT){
							LCD_DisplayStringLine(47,100,"TC-T");
						}else if(TCTYPE == TCJ){
							LCD_DisplayStringLine(47,100,"TC-J");
						}else if(TCTYPE == TCN){
							LCD_DisplayStringLine(47,100,"TC-N");
						}else if(TCTYPE == TCE){
							LCD_DisplayStringLine(47,100,"TC-E");
						}else if(TCTYPE == TCS){
							LCD_DisplayStringLine(47,100,"TC-S");
						}else if(TCTYPE == TCR){
							LCD_DisplayStringLine(47,100,"TC-R");
						}else if(TCTYPE == TCB){
							LCD_DisplayStringLine(47,100,"TC-B");
						}else if(TCTYPE == PT100){
							LCD_DisplayStringLine(47,100,"PT100");
						}else if(TCTYPE == V1){
							LCD_DisplayStringLine(47,100,"1~5V");
						}
						if(LANG == chs)
						{
							DrawInstruction("热电偶型号选择");
						}else{
							DrawInstruction("Thermocouple model");
						}
						op_flag = home_type;
					}
					Drawhomemenu();
					
				}break;
				case home_ch2:
				{
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_off(CH2_SW);
							LCD_DisplayStringLine_48(170+10,10,"002");
						}else if(ch_page == page2){
							focus_off(CH10_SW);
							LCD_DisplayStringLine_48(170+10,10,"010");
						}else if(ch_page == page3){
							focus_off(CH18_SW);
							LCD_DisplayStringLine_48(170+10,10,"018");
						}else if(ch_page == page4){
							focus_off(CH26_SW);
							LCD_DisplayStringLine_48(170+10,10,"026");
						}else if(ch_page == page5){
							focus_off(CH34_SW);
							LCD_DisplayStringLine_48(170+10,10,"034");
						}	
						

						if(ch_page == page1)
						{
							focus_on(CH1_SW);
							LCD_DisplayStringLine_48(100,10,"001");
						}else if(ch_page == page2){
							focus_on(CH9_SW);
							LCD_DisplayStringLine_48(100,10,"009");
						}else if(ch_page == page3){
							focus_on(CH17_SW);
							LCD_DisplayStringLine_48(100,10,"017");
						}else if(ch_page == page4){
							focus_on(CH25_SW);
							LCD_DisplayStringLine_48(100,10,"025");
						}else if(ch_page == page5){
							focus_on(CH33_SW);
							LCD_DisplayStringLine_48(100,10,"033");
						}
						
						op_flag = home_ch1;
					}else if(FONT == middle){																		
						if(ch_page == page1)
						{
							focus_on(CH1_SW);
							DISP_CNL_M(100,10,"001");
							focus_off(CH2_SW);
							DISP_CNL_M(140,10,"002");
						}else if(ch_page == page2){
							focus_on(CH25_SW);
							DISP_CNL_M(100,10,"025");
							focus_off(CH26_SW);
							DISP_CNL_M(140,10,"026");
						}
						op_flag = home_ch1;
					}else if(FONT == small){																		
						if(ch_page == page1)
						{
							focus_on(CH1_SW);
							DISP_CNL_S(100,10,"001");
							focus_off(CH2_SW);
							DISP_CNL_S(140,10,"002");
						}
						op_flag = home_ch1;
					}
					DrawType(1);
				}break;
				case home_ch3:
				{
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_off(CH3_SW);
							LCD_DisplayStringLine_48(240+20,10,"003");
						}else if(ch_page == page2){
							focus_off(CH11_SW);
							LCD_DisplayStringLine_48(240+20,10,"011");
						}else if(ch_page == page3){
							focus_off(CH19_SW);
							LCD_DisplayStringLine_48(240+20,10,"019");
						}else if(ch_page == page4){
							focus_off(CH27_SW);
							LCD_DisplayStringLine_48(240+20,10,"027");
						}else if(ch_page == page5){
							focus_off(CH35_SW);
							LCD_DisplayStringLine_48(240+20,10,"035");
						}
						
						

						if(ch_page == page1)
						{
							focus_on(CH2_SW);
							LCD_DisplayStringLine_48(170+10,10,"002");
						}else if(ch_page == page2){
							focus_on(CH10_SW);
							LCD_DisplayStringLine_48(170+10,10,"010");
						}else if(ch_page == page3){
							focus_on(CH18_SW);
							LCD_DisplayStringLine_48(170+10,10,"018");
						}else if(ch_page == page4){
							focus_on(CH26_SW);
							LCD_DisplayStringLine_48(170+10,10,"026");
						}else if(ch_page == page5){
							focus_on(CH34_SW);
							LCD_DisplayStringLine_48(170+10,10,"034");
						}
						
						op_flag = home_ch2;
					}else if(FONT == middle){																		
						if(ch_page == page1)
						{
							focus_on(CH2_SW);
							DISP_CNL_M(140,10,"002");
							focus_off(CH3_SW);
							DISP_CNL_M(180,10,"003");
						}else if(ch_page == page2){
							focus_on(CH26_SW);
							DISP_CNL_M(140,10,"026");
							focus_off(CH27_SW);
							DISP_CNL_M(180,10,"027");
						}
						op_flag = home_ch2;
					}else if(FONT == small){																		
						if(ch_page == page1)
						{
							focus_on(CH2_SW);
							DISP_CNL_S(140,10,"002");
							focus_off(CH3_SW);
							DISP_CNL_S(180,10,"003");
						}
						op_flag = home_ch2;
					}
					DrawType(1);
				}break;
				case home_ch4:
				{
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_off(CH4_SW);
							LCD_DisplayStringLine_48(310+30,10,"004");
						}else if(ch_page == page2){
							focus_off(CH12_SW);
							LCD_DisplayStringLine_48(310+30,10,"012");
						}else if(ch_page == page3){
							focus_off(CH20_SW);
							LCD_DisplayStringLine_48(310+30,10,"020");
						}else if(ch_page == page4){
							focus_off(CH28_SW);
							LCD_DisplayStringLine_48(310+30,10,"028");
						}else if(ch_page == page5){
							focus_off(CH36_SW);
							LCD_DisplayStringLine_48(310+30,10,"036");
						}
						

						if(ch_page == page1)
						{
							focus_on(CH3_SW);
							LCD_DisplayStringLine_48(240+20,10,"003");
						}else if(ch_page == page2){
							focus_on(CH11_SW);
							LCD_DisplayStringLine_48(240+20,10,"011");
						}else if(ch_page == page3){
							focus_on(CH19_SW);
							LCD_DisplayStringLine_48(240+20,10,"019");
						}else if(ch_page == page4){
							focus_on(CH27_SW);
							LCD_DisplayStringLine_48(240+20,10,"027");
						}else if(ch_page == page5){
							focus_on(CH35_SW);
							LCD_DisplayStringLine_48(240+20,10,"035");
						}
						
						op_flag = home_ch3;
					}else if(FONT == middle){																		
						if(ch_page == page1)
						{
							focus_on(CH3_SW);
							DISP_CNL_M(180,10,"003");
							focus_off(CH4_SW);
							DISP_CNL_M(220,10,"004");
						}else if(ch_page == page2){
							focus_on(CH27_SW);
							DISP_CNL_M(180,10,"027");
							focus_off(CH28_SW);
							DISP_CNL_M(220,10,"022");
						}
						op_flag = home_ch3;
					}else if(FONT == small){																		
						if(ch_page == page1)
						{
							focus_on(CH3_SW);
							DISP_CNL_S(180,10,"003");
							focus_off(CH4_SW);
							DISP_CNL_S(220,10,"004");
						}
						op_flag = home_ch3;
					}
					DrawType(1);
				}break;
				case home_ch5:
				{
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_off(CH5_SW);
							LCD_DisplayStringLine_48(100,330,"005");
						}else if(ch_page == page2){
							focus_off(CH13_SW);
							LCD_DisplayStringLine_48(100,330,"013");
						}else if(ch_page == page3){
							focus_off(CH21_SW);
							LCD_DisplayStringLine_48(100,330,"021");
						}else if(ch_page == page4){
							focus_off(CH29_SW);
							LCD_DisplayStringLine_48(100,330,"029");
						}else if(ch_page == page5){
							focus_off(CH37_SW);
							LCD_DisplayStringLine_48(100,330,"037");
						}
						

						if(ch_page == page1)
						{
							focus_on(CH8_SW);
							LCD_DisplayStringLine_48(310+30,330,"008");
						}else if(ch_page == page2){
							focus_on(CH16_SW);
							LCD_DisplayStringLine_48(310+30,330,"016");
						}else if(ch_page == page3){
							focus_on(CH24_SW);
							LCD_DisplayStringLine_48(310+30,330,"024");
						}else if(ch_page == page4){
							focus_on(CH32_SW);
							LCD_DisplayStringLine_48(310+30,330,"032");
						}else if(ch_page == page5){
							focus_on(CH40_SW);
							LCD_DisplayStringLine_48(310+30,330,"040");
						}
						
						op_flag = home_ch8;
					}else if(FONT == middle){																		
						if(ch_page == page1)
						{
							focus_on(CH4_SW);
							DISP_CNL_M(220,10,"004");
							focus_off(CH5_SW);
							DISP_CNL_M(260,10,"005");
						}else if(ch_page == page2){
							focus_on(CH28_SW);
							DISP_CNL_M(220,10,"028");
							focus_off(CH29_SW);
							DISP_CNL_M(260,10,"029");
						}
						op_flag = home_ch4;
					}else if(FONT == small){																		
						if(ch_page == page1)
						{
							focus_on(CH4_SW);
							DISP_CNL_S(220,10,"004");
							focus_off(CH5_SW);
							DISP_CNL_S(260,10,"005");
						}
						op_flag = home_ch4;
					}
					DrawType(1);
				}break;
				case home_ch6:
				{
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_off(CH6_SW);
							LCD_DisplayStringLine_48(170+10,330,"006");
						}else if(ch_page == page2){
							focus_off(CH14_SW);
							LCD_DisplayStringLine_48(170+10,330,"014");
						}else if(ch_page == page3){
							focus_off(CH22_SW);
							LCD_DisplayStringLine_48(170+10,330,"022");
						}else if(ch_page == page4){
							focus_off(CH30_SW);
							LCD_DisplayStringLine_48(170+10,330,"030");
						}else if(ch_page == page5){
							focus_off(CH38_SW);
							LCD_DisplayStringLine_48(170+10,330,"038");
						}
						
						if(ch_page == page1)
						{
							focus_on(CH5_SW);
							LCD_DisplayStringLine_48(100,330,"005");
						}else if(ch_page == page2){
							focus_on(CH13_SW);
							LCD_DisplayStringLine_48(100,330,"013");
						}else if(ch_page == page3){
							focus_on(CH21_SW);
							LCD_DisplayStringLine_48(100,330,"021");
						}else if(ch_page == page4){
							focus_on(CH29_SW);
							LCD_DisplayStringLine_48(100,330,"029");
						}else if(ch_page == page5){
							focus_on(CH37_SW);
							LCD_DisplayStringLine_48(100,330,"037");
						}
						
						op_flag = home_ch5;
					}else if(FONT == middle){																		
						if(ch_page == page1)
						{
							focus_on(CH5_SW);
							DISP_CNL_M(260,10,"005");
							focus_off(CH6_SW);
							DISP_CNL_M(300,10,"006");
						}else if(ch_page == page2){
							focus_on(CH29_SW);
							DISP_CNL_M(260,10,"029");
							focus_off(CH30_SW);
							DISP_CNL_M(300,10,"030");
						}
						op_flag = home_ch5;
					}else if(FONT == small){																		
						if(ch_page == page1)
						{
							focus_on(CH5_SW);
							DISP_CNL_S(260,10,"005");
							focus_off(CH6_SW);
							DISP_CNL_S(300,10,"006");
						}
						op_flag = home_ch5;
					}
					DrawType(1);
				}break;
				case home_ch7:
				{
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_off(CH7_SW);
							LCD_DisplayStringLine_48(240+20,330,"007");
						}else if(ch_page == page2){
							focus_off(CH15_SW);
							LCD_DisplayStringLine_48(240+20,330,"015");
						}else if(ch_page == page3){
							focus_off(CH23_SW);
							LCD_DisplayStringLine_48(240+20,330,"023");
						}else if(ch_page == page4){
							focus_off(CH31_SW);
							LCD_DisplayStringLine_48(240+20,330,"031");
						}else if(ch_page == page5){
							focus_off(CH39_SW);
							LCD_DisplayStringLine_48(240+20,330,"039");
						}
						

						if(ch_page == page1)
						{
							focus_on(CH6_SW);
							LCD_DisplayStringLine_48(170+10,330,"006");
						}else if(ch_page == page2){
							focus_on(CH14_SW);
							LCD_DisplayStringLine_48(170+10,330,"014");
						}else if(ch_page == page3){
							focus_on(CH22_SW);
							LCD_DisplayStringLine_48(170+10,330,"022");
						}else if(ch_page == page4){
							focus_on(CH30_SW);
							LCD_DisplayStringLine_48(170+10,330,"030");
						}else if(ch_page == page5){
							focus_on(CH38_SW);
							LCD_DisplayStringLine_48(170+10,330,"038");
						}
						
						op_flag = home_ch6;
					}else if(FONT == middle){																		
						if(ch_page == page1)
						{
							focus_on(CH6_SW);
							DISP_CNL_M(300,10,"006");
							focus_off(CH7_SW);
							DISP_CNL_M(340,10,"007");
						}else if(ch_page == page2){
							focus_on(CH30_SW);
							DISP_CNL_M(300,10,"030");
							focus_off(CH31_SW);
							DISP_CNL_M(340,10,"031");
						}
						op_flag = home_ch6;
					}else if(FONT == small){																		
						if(ch_page == page1)
						{
							focus_on(CH6_SW);
							DISP_CNL_S(300,10,"006");
							focus_off(CH7_SW);
							DISP_CNL_S(340,10,"007");
						}
						op_flag = home_ch6;
					}
					DrawType(1);
				}break;
				case home_ch8:
				{
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_off(CH8_SW);
							LCD_DisplayStringLine_48(310+30,330,"008");
						}else if(ch_page == page2){
							focus_off(CH16_SW);
							LCD_DisplayStringLine_48(310+30,330,"016");
						}else if(ch_page == page3){
							focus_off(CH24_SW);
							LCD_DisplayStringLine_48(310+30,330,"024");
						}else if(ch_page == page4){
							focus_off(CH32_SW);
							LCD_DisplayStringLine_48(310+30,330,"032");
						}else if(ch_page == page5){
							focus_off(CH40_SW);
							LCD_DisplayStringLine_48(310+30,330,"040");
						}
						
						if(ch_page == page1)
						{
							focus_on(CH7_SW);
							LCD_DisplayStringLine_48(240+20,330,"007");
						}else if(ch_page == page2){
							focus_on(CH15_SW);
							LCD_DisplayStringLine_48(240+20,330,"015");
						}else if(ch_page == page3){
							focus_on(CH23_SW);
							LCD_DisplayStringLine_48(240+20,330,"023");
						}else if(ch_page == page4){
							focus_on(CH31_SW);
							LCD_DisplayStringLine_48(240+20,330,"031");
						}else if(ch_page == page5){
							focus_on(CH39_SW);
							LCD_DisplayStringLine_48(240+20,330,"039");
						}
						
						op_flag = home_ch7;
					}else if(FONT == middle){																		
						if(ch_page == page1)
						{
							focus_on(CH7_SW);
							DISP_CNL_M(340,10,"007");
							focus_off(CH8_SW);
							DISP_CNL_M(380,10,"008");
						}else if(ch_page == page2){
							focus_on(CH31_SW);
							DISP_CNL_M(340,10,"031");
							focus_off(CH32_SW);
							DISP_CNL_M(380,10,"032");
						}
						op_flag = home_ch7;
					}else if(FONT == small){																		
						if(ch_page == page1)
						{
							focus_on(CH7_SW);
							DISP_CNL_S(340,10,"007");
							focus_off(CH8_SW);
							DISP_CNL_S(380,10,"008");
						}
						op_flag = home_ch7;
					}
					DrawType(1);
				}break;
				case home_ch9:
				{
					if(FONT == middle)
					{
						if(ch_page == page1)
						{
							focus_on(CH16_SW);
							DISP_CNL_M(380,210,"016");
							focus_off(CH9_SW);
							DISP_CNL_M(100,210,"009");
						}else if(ch_page == page2){
							focus_on(CH40_SW);
							DISP_CNL_M(380,210,"040");
							focus_off(CH33_SW);
							DISP_CNL_M(100,210,"033");
						}
						op_flag = home_ch16;
					}else if(FONT == small){																		
						if(ch_page == page1)
						{
							focus_on(CH16_SW);
							DISP_CNL_S(380,134,"016");							
							focus_off(CH9_SW);
							DISP_CNL_S(100,134,"009");
						}
						op_flag = home_ch16;
					}
					DrawType(1);
				}break;
				case home_ch10:
				{
					if(FONT == middle)
					{
						if(ch_page == page1)
						{
							focus_on(CH9_SW);
							DISP_CNL_M(100,210,"009");
							focus_off(CH10_SW);
							DISP_CNL_M(140,210,"010");
						}else if(ch_page == page2){
							focus_on(CH33_SW);
							DISP_CNL_M(100,210,"033");
							focus_off(CH34_SW);
							DISP_CNL_M(140,210,"034");
						}
						op_flag = home_ch9;
					}else if(FONT == small){																		
						if(ch_page == page1)
						{
							focus_on(CH9_SW);
							DISP_CNL_S(100,134,"009");							
							focus_off(CH10_SW);
							DISP_CNL_S(140,134,"010");
						}
						op_flag = home_ch9;
					}
				}break;
				case home_ch11:
				{
					if(FONT == middle)
					{
						if(ch_page == page1)
						{
							focus_on(CH10_SW);
							DISP_CNL_M(140,210,"010");
							focus_off(CH11_SW);
							DISP_CNL_M(180,210,"011");
						}else if(ch_page == page2){
							focus_on(CH34_SW);
							DISP_CNL_M(140,210,"034");
							focus_off(CH35_SW);
							DISP_CNL_M(180,210,"035");
						}
						op_flag = home_ch10;
					}else if(FONT == small){																		
						if(ch_page == page1)
						{
							focus_on(CH10_SW);
							DISP_CNL_S(140,134,"010");							
							focus_off(CH11_SW);
							DISP_CNL_S(180,134,"011");
						}
						op_flag = home_ch10;
					}	
					DrawType(1);
				}break;
				case home_ch12:
				{
					if(FONT == middle)
					{
						if(ch_page == page1)
						{
							focus_on(CH11_SW);
							DISP_CNL_M(180,210,"011");
							focus_off(CH12_SW);
							DISP_CNL_M(220,210,"012");
						}else if(ch_page == page2){
							focus_on(CH35_SW);
							DISP_CNL_M(180,210,"035");
							focus_off(CH36_SW);
							DISP_CNL_M(220,210,"036");
						}
						op_flag = home_ch11;
					}else if(FONT == small){																		
						if(ch_page == page1)
						{
							focus_on(CH11_SW);
							DISP_CNL_S(180,134,"011");							
							focus_off(CH12_SW);
							DISP_CNL_S(220,134,"012");
						}
						op_flag = home_ch11;
					}
					DrawType(1);
				}break;
				case home_ch13:
				{
					if(FONT == middle)
					{
						if(ch_page == page1)
						{
							focus_on(CH12_SW);
							DISP_CNL_M(220,210,"012");
							focus_off(CH13_SW);
							DISP_CNL_M(260,210,"013");
						}else if(ch_page == page2){
							focus_on(CH36_SW);
							DISP_CNL_M(220,210,"036");
							focus_off(CH37_SW);
							DISP_CNL_M(260,210,"037");
						}
						op_flag = home_ch12;
					}else if(FONT == small){																		
						if(ch_page == page1)
						{
							focus_on(CH12_SW);
							DISP_CNL_S(220,134,"012");							
							focus_off(CH13_SW);
							DISP_CNL_S(260,134,"013");
						}
						op_flag = home_ch12;
					}
					DrawType(1);
				}break;
				case home_ch14:
				{
					if(FONT == middle)
					{
						if(ch_page == page1)
						{
							focus_on(CH13_SW);
							DISP_CNL_M(260,210,"013");
							focus_off(CH14_SW);
							DISP_CNL_M(300,210,"014");
						}else if(ch_page == page2){
							focus_on(CH37_SW);
							DISP_CNL_M(260,210,"037");
							focus_off(CH38_SW);
							DISP_CNL_M(300,210,"038");
						}
						op_flag = home_ch13;
					}else if(FONT == small){																		
						if(ch_page == page1)
						{
							focus_on(CH13_SW);
							DISP_CNL_S(260,134,"013");							
							focus_off(CH14_SW);
							DISP_CNL_S(300,134,"014");
						}
						op_flag = home_ch13;
					}
					DrawType(1);
				}break;
				case home_ch15:
				{
					if(FONT == middle)
					{
						if(ch_page == page1)
						{
							focus_on(CH14_SW);
							DISP_CNL_M(300,210,"014");
							focus_off(CH15_SW);
							DISP_CNL_M(340,210,"015");
						}else if(ch_page == page2){
							focus_on(CH38_SW);
							DISP_CNL_M(300,210,"038");
							focus_off(CH39_SW);
							DISP_CNL_M(340,210,"039");
						}
						op_flag = home_ch14;
					}else if(FONT == small){																		
						if(ch_page == page1)
						{
							focus_on(CH14_SW);
							DISP_CNL_S(300,134,"014");							
							focus_off(CH15_SW);
							DISP_CNL_S(340,134,"015");
						}
						op_flag = home_ch14;
					}
					DrawType(1);
				}break;
				case home_ch16:
				{
					if(FONT == middle)
					{
						if(ch_page == page1)
						{
							focus_on(CH15_SW);
							DISP_CNL_M(340,210,"015");
							focus_off(CH16_SW);
							DISP_CNL_M(380,210,"016");
						}else if(ch_page == page2){
							focus_on(CH39_SW);
							DISP_CNL_M(340,210,"039");
							focus_off(CH40_SW);
							DISP_CNL_M(380,210,"040");
						}
						op_flag = home_ch15;
					}else if(FONT == small){																		
						if(ch_page == page1)
						{
							focus_on(CH15_SW);
							DISP_CNL_S(340,134,"015");							
							focus_off(CH16_SW);
							DISP_CNL_S(380,134,"016");
						}
						op_flag = home_ch15;
					}
					DrawType(1);
				}break;
				case home_ch17:
				{
					if(FONT == middle)
					{
						if(ch_page == page1)
						{
							focus_off(CH17_SW);
							DISP_CNL_M(100,410,"017");
							focus_on(CH24_SW);
							DISP_CNL_M(380,410,"024");
							op_flag = home_ch24;
						}				
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH17_SW);
							DISP_CNL_S(100,258,"017");
							focus_on(CH24_SW);
							DISP_CNL_S(380,258,"024");
							op_flag = home_ch24;
						}
					}
					DrawType(1);
				}break;
				case home_ch18:
				{
					if(FONT == middle)
					{
						if(ch_page == page1)
						{
							focus_off(CH18_SW);
							DISP_CNL_M(140,410,"018");
							focus_on(CH17_SW);
							DISP_CNL_M(100,410,"017");
							op_flag = home_ch17;
						}
						
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH18_SW);
							DISP_CNL_S(140,258,"018");
							focus_on(CH17_SW);
							DISP_CNL_S(100,258,"017");
							op_flag = home_ch17;
						}
					}
					DrawType(1);
				}break;
				case home_ch19:
				{
					if(FONT == middle)
					{
						if(ch_page == page1)
						{
							focus_off(CH19_SW);
							DISP_CNL_M(180,410,"019");
							focus_on(CH18_SW);
							DISP_CNL_M(140,410,"018");
							op_flag = home_ch18;
						}
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH19_SW);
							DISP_CNL_S(180,258,"019");
							focus_on(CH18_SW);
							DISP_CNL_S(140,258,"018");
							op_flag = home_ch18;
						}
					}
					DrawType(1);
				}break;
				case home_ch20:
				{
					if(FONT == middle)
					{
						if(ch_page == page1)
						{
							focus_off(CH20_SW);
							DISP_CNL_M(220,410,"020");
							focus_on(CH19_SW);
							DISP_CNL_M(180,410,"019");
							op_flag = home_ch19;
						}
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH20_SW);
							DISP_CNL_S(220,258,"020");
							focus_on(CH19_SW);
							DISP_CNL_S(180,258,"019");
							op_flag = home_ch19;
						}
					}
					DrawType(1);
				}break;
				case home_ch21:
				{
					if(FONT == middle)
					{
						if(ch_page == page1)
						{
							focus_off(CH21_SW);
							DISP_CNL_M(260,410,"021");
							focus_on(CH20_SW);
							DISP_CNL_M(220,410,"020");
							op_flag = home_ch20;
						}
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH21_SW);
							DISP_CNL_S(260,258,"021");
							focus_on(CH20_SW);
							DISP_CNL_S(220,258,"020");
							op_flag = home_ch20;
						}
					}
					DrawType(1);
				}break;
				case home_ch22:
				{
					if(FONT == middle)
					{
						if(ch_page == page1)
						{
							focus_off(CH22_SW);
							DISP_CNL_M(300,410,"022");
							focus_on(CH21_SW);
							DISP_CNL_M(260,410,"021");
							op_flag = home_ch21;
						}
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH22_SW);
							DISP_CNL_S(300,258,"022");
							focus_on(CH21_SW);
							DISP_CNL_S(260,258,"021");
							op_flag = home_ch21;
						}
					}
					DrawType(1);
				}break;
				case home_ch23:
				{
					if(FONT == middle)
					{
						if(ch_page == page1)
						{
							focus_off(CH23_SW);
							DISP_CNL_M(340,410,"023");
							focus_on(CH22_SW);
							DISP_CNL_M(300,410,"022");
							op_flag = home_ch22;
						}
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH23_SW);
							DISP_CNL_S(340,258,"023");
							focus_on(CH22_SW);
							DISP_CNL_S(300,258,"022");
							op_flag = home_ch22;
						}
					}
					DrawType(1);
				}break;
				case home_ch24:
				{
					if(FONT == middle)
					{
						if(ch_page == page1)
						{
							focus_off(CH24_SW);
							DISP_CNL_M(380,410,"024");
							focus_on(CH23_SW);
							DISP_CNL_M(340,410,"023");
							op_flag = home_ch23;
						}
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH24_SW);
							DISP_CNL_S(380,258,"024");
							focus_on(CH23_SW);
							DISP_CNL_S(340,258,"023");
							op_flag = home_ch23;
						}
					}
					DrawType(1);
				}break;
				case home_ch25:
				{
					if(FONT == small)
					{
						if(ch_page == page1)
						{
							focus_off(CH25_SW);
							DISP_CNL_S(100,382,"025");
							focus_on(CH32_SW);
							DISP_CNL_S(380,382,"032");
							op_flag = home_ch32;
						}
					}
					DrawType(1);
				}break;
				case home_ch26:
				{
					if(FONT == small)
					{
						if(ch_page == page1)
						{
							focus_off(CH26_SW);
							DISP_CNL_S(140,382,"026");
							focus_on(CH25_SW);
							DISP_CNL_S(100,382,"025");
							op_flag = home_ch25;
						}
					}
					DrawType(1);
				}break;
				case home_ch27:
				{
					if(FONT == small)
					{
						if(ch_page == page1)
						{
							focus_off(CH27_SW);
							DISP_CNL_S(180,382,"027");
							focus_on(CH26_SW);
							DISP_CNL_S(140,382,"026");
							op_flag = home_ch26;
						}
					}
					DrawType(1);
				}break;
				case home_ch28:
				{
					if(FONT == small)
					{
						if(ch_page == page1)
						{
							focus_off(CH28_SW);
							DISP_CNL_S(220,382,"028");
							focus_on(CH27_SW);
							DISP_CNL_S(180,382,"027");
							op_flag = home_ch27;
						}
					}
					DrawType(1);
				}break;
				case home_ch29:
				{
					if(FONT == small)
					{
						if(ch_page == page1)
						{
							focus_off(CH29_SW);
							DISP_CNL_S(260,382,"029");
							focus_on(CH28_SW);
							DISP_CNL_S(220,382,"028");
							op_flag = home_ch28;
						}
					}
					DrawType(1);
				}break;
				case home_ch30:
				{
					if(FONT == small)
					{
						if(ch_page == page1)
						{
							focus_off(CH30_SW);
							DISP_CNL_S(300,382,"030");
							focus_on(CH29_SW);
							DISP_CNL_S(260,382,"029");
							op_flag = home_ch29;
						}
					}
					DrawType(1);
				}break;
				case home_ch31:
				{
					if(FONT == small)
					{
						if(ch_page == page1)
						{
							focus_off(CH31_SW);
							DISP_CNL_S(340,382,"031");
							focus_on(CH30_SW);
							DISP_CNL_S(300,382,"030");
							op_flag = home_ch30;
						}
					}
					DrawType(1);
				}break;
				case home_ch32:
				{
					if(FONT == small)
					{
						if(ch_page == page1)
						{
							focus_off(CH32_SW);
							DISP_CNL_S(380,382,"032");
							focus_on(CH31_SW);
							DISP_CNL_S(340,382,"031");
							op_flag = home_ch31;
						}
					}
					DrawType(1);
				}break;
				case home_ch33:
				{
					if(FONT == small)
					{
						if(ch_page == page1)
						{
							focus_off(CH33_SW);
							DISP_CNL_S(100,506,"033");
							focus_on(CH40_SW);
							DISP_CNL_S(380,506,"040");
							op_flag = home_ch40;
						}
					}
					DrawType(1);
				}break;
				case home_ch34:
				{
					if(FONT == small)
					{
						if(ch_page == page1)
						{
							focus_off(CH34_SW);
							DISP_CNL_S(140,506,"034");
							focus_on(CH33_SW);
							DISP_CNL_S(100,506,"033");
							op_flag = home_ch33;
						}
					}
					DrawType(1);
				}break;
				case home_ch35:
				{
					if(FONT == small)
					{
						if(ch_page == page1)
						{
							focus_off(CH35_SW);
							DISP_CNL_S(180,506,"035");
							focus_on(CH34_SW);
							DISP_CNL_S(140,506,"034");
							op_flag = home_ch34;
						}
					}
					DrawType(1);
				}break;
				case home_ch36:
				{
					if(FONT == small)
					{
						if(ch_page == page1)
						{
							focus_off(CH36_SW);
							DISP_CNL_S(220,506,"036");
							focus_on(CH35_SW);
							DISP_CNL_S(180,506,"035");
							op_flag = home_ch35;
						}
					}
					DrawType(1);
				}break;
				case home_ch37:
				{
					if(FONT == small)
					{
						if(ch_page == page1)
						{
							focus_off(CH37_SW);
							DISP_CNL_S(260,506,"037");
							focus_on(CH36_SW);
							DISP_CNL_S(220,506,"036");
							op_flag = home_ch36;
						}
					}
					DrawType(1);
				}break;
				case home_ch38:
				{
					if(FONT == small)
					{
						if(ch_page == page1)
						{
							focus_off(CH38_SW);
							DISP_CNL_S(300,506,"038");
							focus_on(CH37_SW);
							DISP_CNL_S(260,506,"037");
							op_flag = home_ch37;
						}
					}
					DrawType(1);
				}break;
				case home_ch39:
				{
					if(FONT == small)
					{
						if(ch_page == page1)
						{
							focus_off(CH39_SW);
							DISP_CNL_S(340,506,"039");
							focus_on(CH38_SW);
							DISP_CNL_S(300,506,"038");
							op_flag = home_ch38;
						}
					}
					DrawType(1);
				}break;
				case home_ch40:
				{
					if(FONT == small)
					{
						if(ch_page == page1)
						{
							focus_off(CH40_SW);
							DISP_CNL_S(380,506,"040");
							focus_on(CH39_SW);
							DISP_CNL_S(340,506,"039");
							op_flag = home_ch39;
						}
					}
					DrawType(1);
				}break;
			}
		}break;
		
		case settings:
		{
			switch(op_flag)
			{
				case set_beep:
				{
					focus_off1();
					if(BEEP == beep_off){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(90,150,"关闭");
						}else{
							LCD_DisplayStringLine(90,150,"OFF");
						}
					}else{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(90,150,"打开");
						}else{
							LCD_DisplayStringLine(90,150,"ON");
						}
					}
					
					focus_on1();
					if(FILTER == ft_off){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,150,"关闭");
						}else{
							LCD_DisplayStringLine(50,150,"OFF");
						}
					}else{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,150,"打开");
						}else{
							LCD_DisplayStringLine(50,150,"ON");
						}
					}
					if(LANG == chs)
					{
						DrawInstruction("比较器设置");
					}else{
						DrawInstruction("Comparator ON/OFF");
					}
					op_flag = set_filter;	
				}break;
				case set_unit:
				{
					LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
					LCD_DrawFullRect(150,127,24,31);
					if(UNIT == C){
						LCD_SetTextColor(LCD_COLOR_YELLOW);
						DISP_CNL_S(127,150,"o");
						LCD_DisplayStringLine(127,155,"C");
					}else if(UNIT == F){
						LCD_SetTextColor(LCD_COLOR_YELLOW);
						DISP_CNL_S(127,150,"o");
						LCD_DisplayStringLine(127,155,"F");
					}else{
						LCD_SetTextColor(LCD_COLOR_YELLOW);
						LCD_DisplayStringLine(127,155,"K");
					}
					
					focus_on1();
					if(BEEP == beep_off){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(90,150,"关闭");
						}else{
							LCD_DisplayStringLine(90,150,"OFF");
						}
					}else{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(90,150,"打开");
						}else{
							LCD_DisplayStringLine(90,150,"ON");
						}
					}
					if(LANG == chs)
					{
						DrawInstruction("蜂鸣器设置");
					}else{
						DrawInstruction("Buzzer ON/OFF");
					}
					op_flag = set_beep;
				}break;
				case timetrig:
				{
					focus_off1();
					if(TIMETIRG == trig_off){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(170,150,"关闭");
						}else{
							LCD_DisplayStringLine(170,150,"OFF");
						}
					}else{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(170,150,"打开");
						}else{
							LCD_DisplayStringLine(170,150,"ON");
						}
					}
					LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
					LCD_DrawFullRect(150,127,24,31);
					if(UNIT == C){
						LCD_SetTextColor(LCD_COLOR_BLACK);
//						LCD_SetBackColor(LCD_COLOR_YELLOW);
						DISP_CNL_S(127,150,"o");
						LCD_DisplayStringLine(127,155,"C");
					}else if(UNIT == F){
						LCD_SetTextColor(LCD_COLOR_BLACK);
						DISP_CNL_S(127,150,"o");
						LCD_DisplayStringLine(127,155,"F");
					}else{
						LCD_SetTextColor(LCD_COLOR_BLACK);
						LCD_DisplayStringLine(127,155,"K");
					}
					if(LANG == chs)
					{
						DrawInstruction("温度单位选择");
					}else{
						DrawInstruction("Select temperature unit");
					}
					op_flag = set_unit;
//					focus_off1();
//					sprintf(buf,"%0.2d:%0.2d",
//							STARTH,
//							STARTM);
//					LCD_DisplayStringLine(208,150,(uint8_t *)buf);
//					if(LANG == chs)
//					{
//						DrawInstruction("采集开始时间");
//					}else{
//						DrawInstruction("START");
//					}
//					op_flag = starttime;
//					stimefocus_on(SAVETIME);
//					if(LANG == chs)
//					{
//						DrawInstruction("记录间隔时间");
//					}else{
//						DrawInstruction("Record interval");
//					}
//					op_flag = set_timer;
				}break;		
				case starttime:
				{
					focus_off1();
					sprintf(buf,"%0.2d:%0.2d",
							STARTH,
							STARTM);
					LCD_DisplayStringLine(208,150,(uint8_t *)buf);
					focus_on1();
					if(TIMETIRG == trig_off){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(170,150,"关闭");
						}else{
							LCD_DisplayStringLine(170,150,"OFF");
						}
					}else{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(170,150,"打开");
						}else{
							LCD_DisplayStringLine(170,150,"ON");
						}
					}
//					sprintf(buf,"%0.2d:%0.2d",
//							ENDH,
//							ENDM);
//					LCD_DisplayStringLine(248,150,(uint8_t *)buf);
					if(LANG == chs)
					{
						DrawInstruction("定时采集");
					}else{
						DrawInstruction("Trig Timer");
					}
					op_flag = timetrig;
				}break;
				case endtime:
				{
					focus_off1();
					sprintf(buf,"%0.2d:%0.2d",
							ENDH,
							ENDM);
					LCD_DisplayStringLine(248,150,(uint8_t *)buf);
					
					focus_on1();
					sprintf(buf,"%0.2d:%0.2d",
							STARTH,
							STARTM);
					LCD_DisplayStringLine(208,150,(uint8_t *)buf);
					if(LANG == chs)
					{
						DrawInstruction("采集开始时间");
					}else{
						DrawInstruction("START");
					}
//					if(LANG == chs)
//					{
//						DrawInstruction("记录间隔时间");
//					}else{
//						DrawInstruction("Record interval");
//					}
					bit_flag = 1;
					op_flag = starttime;
				}break;
				case repeat:
				{
					focus_on1();
					if(BAUD == b9600)
					{
						LCD_DisplayStringLine(87,500,"9600");
					}else if(BAUD == b19200){
						LCD_DisplayStringLine(87,500,"19200");
					}else if(BAUD == b38400){
						LCD_DisplayStringLine(87,500,"38400");
					}else if(BAUD == b57600){
						LCD_DisplayStringLine(87,500,"57600");
					}else if(BAUD == b115200){
						LCD_DisplayStringLine(87,500,"115200");
					}
					
					focus_off1();
					if(REPEAT == rtp_off){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(130,500,"关闭");
						}else{
							LCD_DisplayStringLine(130,500,"OFF");
						}
					}else{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(130,500,"打开");
						}else{
							LCD_DisplayStringLine(130,500,"ON");
						}
					}
//					LCD_DisplayStringLine(322,516,"S");
					if(LANG == chs)
					{
						DrawInstruction("波特率设置");
					}else{
						DrawInstruction("Select Baud rate");
					}
					op_flag = set_baud;
				}break;
//				case set_font:
//				{
//					focus_off1();
//					if(FONT == big){
//						if(LANG == chs)
//						{
//							LCD_DisplayStringLine(170+10,150,"大");
//						}else{
//							LCD_DisplayStringLine(170+10,150,"L");
//						}
//					}else if(FONT == middle){
//						if(LANG == chs)
//						{
//							LCD_DisplayStringLine(170+10,150,"中");
//						}else{
//							LCD_DisplayStringLine(170+10,150,"M");
//						}
//					}else if(FONT == small){
//						if(LANG == chs)
//						{
//							LCD_DisplayStringLine(170+10,150,"小");
//						}else{
//							LCD_DisplayStringLine(170+10,150,"S");
//						}
//					}
//					
//					
////					focus_on1();
//					LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
//					LCD_DrawFullRect(150,127,24,31);
//					if(UNIT == C){
//						LCD_SetTextColor(LCD_COLOR_BLACK);
////						LCD_SetBackColor(LCD_COLOR_YELLOW);
//						DISP_CNL_S(127,150,"o");
//						LCD_DisplayStringLine(127,155,"C");
//					}else if(UNIT == F){
//						LCD_SetTextColor(LCD_COLOR_BLACK);
//						DISP_CNL_S(127,150,"o");
//						LCD_DisplayStringLine(127,155,"F");
//					}else{
//						LCD_SetTextColor(LCD_COLOR_BLACK);
//						LCD_DisplayStringLine(127,155,"K");
//					}
//					if(LANG == chs)
//					{
//						DrawInstruction("温度单位选择");
//					}else{
//						DrawInstruction("Select temperature unit");
//					}
//					op_flag = set_unit;	
//				}break;				
//				case set_file:
//				{
//					focus_off1();
//					LCD_DisplayStringLine(317,150,"AUTO");
//					
////					focus_on1();
//					LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
//					LCD_DrawFullRect(150,127,24,31);
//					if(UNIT == C){
//						LCD_SetTextColor(LCD_COLOR_BLACK);
////						LCD_SetBackColor(LCD_COLOR_YELLOW);
//						DISP_CNL_S(127,150,"o");
//						LCD_DisplayStringLine(127,155,"C");
//					}else if(UNIT == F){
//						LCD_SetTextColor(LCD_COLOR_BLACK);
//						DISP_CNL_S(127,150,"o");
//						LCD_DisplayStringLine(127,155,"F");
//					}else{
//						LCD_SetTextColor(LCD_COLOR_BLACK);
//						LCD_DisplayStringLine(127,155,"K");
//					}
//					if(LANG == chs)
//					{
//						DrawInstruction("温度单位选择");
//					}else{
//						DrawInstruction("Select temperature unit");
//					}
//					op_flag = set_unit;					
//				}break;
				case set_baud:
				{
					focus_off1();
					if(BAUD == b9600)
					{
						LCD_DisplayStringLine(87,500,"9600");
					}else if(BAUD == b19200){
						LCD_DisplayStringLine(87,500,"19200");
					}else if(BAUD == b38400){
						LCD_DisplayStringLine(87,500,"38400");
					}else if(BAUD == b57600){
						LCD_DisplayStringLine(87,500,"57600");
					}else if(BAUD == b115200){
						LCD_DisplayStringLine(87,500,"115200");
					}
					
					focus_on1();
					if(SPEED == fast)
					{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,500,"快速");
						}else{
							LCD_DisplayStringLine(50,500,"FAST");
						}
					}else if(SPEED == moderate){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,500,"中速");
						}else{
							LCD_DisplayStringLine(50,500,"MED");
						}
					}else if(SPEED == slow){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,500,"慢速");
						}else{
							LCD_DisplayStringLine(50,500,"SLOW");
						}
					}
					if(LANG == chs)
					{
						DrawInstruction("采样速度设置");
					}else{
						DrawInstruction("Setup sampling rate");
					}
					op_flag = set_spd;
				}break;
				case set_timer:
				{
					stimefocus_off(SAVETIME);
//					LCD_DisplayStringLine(317,150,"1");
//					LCD_DisplayStringLine(322,516,"S");
					focus_on1();
					sprintf(buf,"%0.2d:%0.2d",
							ENDH,
							ENDM);
					LCD_DisplayStringLine(248,150,(uint8_t *)buf);
					if(LANG == chs)
					{
						DrawInstruction("采集结束时间");
					}else{
						DrawInstruction("END");
					}
					bit_flag = 1;
					op_flag = endtime;	
//					LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
//					LCD_DrawFullRect(150,127,24,31);
//					if(UNIT == C){
//						LCD_SetTextColor(LCD_COLOR_BLACK);
////						LCD_SetBackColor(LCD_COLOR_YELLOW);
//						DISP_CNL_S(127,150,"o");
//						LCD_DisplayStringLine(127,155,"C");
//					}else if(UNIT == F){
//						LCD_SetTextColor(LCD_COLOR_BLACK);
//						DISP_CNL_S(127,150,"o");
//						LCD_DisplayStringLine(127,155,"F");
//					}else{
//						LCD_SetTextColor(LCD_COLOR_BLACK);
//						LCD_DisplayStringLine(127,155,"K");
//					}
//					if(LANG == chs)
//					{
//						DrawInstruction("温度单位选择");
//					}else{
//						DrawInstruction("Select temperature unit");
//					}
//					op_flag = set_unit;	
				}break;
				
			}
		}break;
		case separation:
		{
			switch(op_flag)
			{
				case set_min2:
				{
					if(spt_page == page1)
					{
						minfocus_off(2,CH2MIN);
						minfocus_on(1,CH1MIN);
					}else if(spt_page == page2){
						minfocus_off(2,CH10MIN);
						minfocus_on(1,CH9MIN);
					}else if(spt_page == page3){
						minfocus_off(2,CH18MIN);
						minfocus_on(1,CH17MIN);
					}else if(spt_page == page4){
						minfocus_off(2,CH26MIN);
						minfocus_on(1,CH25MIN);
					}else if(spt_page == page5){
						minfocus_off(2,CH34MIN);
						minfocus_on(1,CH33MIN);
					}
					op_flag = set_min1;
				}break;
				case set_min3:
				{
					if(spt_page == page1)
					{
						minfocus_off(3,CH3MIN);
						minfocus_on(2,CH2MIN);
					}else if(spt_page == page2){
						minfocus_off(3,CH11MIN);
						minfocus_on(2,CH10MIN);
					}else if(spt_page == page3){
						minfocus_off(3,CH19MIN);
						minfocus_on(2,CH18MIN);
					}else if(spt_page == page4){
						minfocus_off(3,CH27MIN);
						minfocus_on(2,CH26MIN);
					}else if(spt_page == page5){
						minfocus_off(3,CH35MIN);
						minfocus_on(2,CH34MIN);
					}
					op_flag = set_min2;
				}break;
				case set_min4:
				{
					if(spt_page == page1)
					{
						minfocus_off(4,CH4MIN);
						minfocus_on(3,CH3MIN);
					}else if(spt_page == page2){
						minfocus_off(4,CH12MIN);
						minfocus_on(3,CH11MIN);
					}else if(spt_page == page3){
						minfocus_off(4,CH20MIN);
						minfocus_on(3,CH19MIN);
					}else if(spt_page == page4){
						minfocus_off(4,CH28MIN);
						minfocus_on(3,CH27MIN);
					}else if(spt_page == page5){
						minfocus_off(4,CH36MIN);
						minfocus_on(3,CH35MIN);
					}
					op_flag = set_min3;
				}break;
				case set_min5:
				{
					if(spt_page == page1)
					{
						minfocus_off(5,CH5MIN);
						minfocus_on(4,CH4MIN);
					}else if(spt_page == page2){
						minfocus_off(5,CH13MIN);
						minfocus_on(4,CH12MIN);
					}else if(spt_page == page3){
						minfocus_off(5,CH21MIN);
						minfocus_on(4,CH20MIN);
					}else if(spt_page == page4){
						minfocus_off(5,CH29MIN);
						minfocus_on(4,CH28MIN);
					}else if(spt_page == page5){
						minfocus_off(5,CH37MIN);
						minfocus_on(4,CH36MIN);
					}
					op_flag = set_min4;
				}break;
				case set_min6:
				{
					if(spt_page == page1)
					{
						minfocus_off(6,CH6MIN);
						minfocus_on(5,CH5MIN);
					}else if(spt_page == page2){
						minfocus_off(6,CH14MIN);
						minfocus_on(5,CH13MIN);
					}else if(spt_page == page3){
						minfocus_off(6,CH22MIN);
						minfocus_on(5,CH21MIN);
					}else if(spt_page == page4){
						minfocus_off(6,CH30MIN);
						minfocus_on(5,CH29MIN);
					}else if(spt_page == page5){
						minfocus_off(6,CH38MIN);
						minfocus_on(5,CH37MIN);
					}
					op_flag = set_min5;
				}break;
				case set_min7:
				{
					if(spt_page == page1)
					{
						minfocus_off(7,CH7MIN);
						minfocus_on(6,CH6MIN);
					}else if(spt_page == page2){
						minfocus_off(7,CH15MIN);
						minfocus_on(6,CH14MIN);
					}else if(spt_page == page3){
						minfocus_off(7,CH23MIN);
						minfocus_on(6,CH22MIN);
					}else if(spt_page == page4){
						minfocus_off(7,CH31MIN);
						minfocus_on(6,CH30MIN);
					}else if(spt_page == page5){
						minfocus_off(7,CH39MIN);
						minfocus_on(6,CH38MIN);
					}
					op_flag = set_min6;
				}break;
				case set_min8:
				{
					if(spt_page == page1)
					{
						minfocus_off(8,CH8MIN);
						minfocus_on(7,CH7MIN);
					}else if(spt_page == page2){
						minfocus_off(8,CH16MIN);
						minfocus_on(7,CH15MIN);
					}else if(spt_page == page3){
						minfocus_off(8,CH24MIN);
						minfocus_on(7,CH23MIN);
					}else if(spt_page == page4){
						minfocus_off(8,CH32MIN);
						minfocus_on(7,CH31MIN);
					}else if(spt_page == page5){
						minfocus_off(8,CH40MIN);
						minfocus_on(7,CH39MIN);
					}
					op_flag = set_min7;
				}break;
				case set_max2:
				{
					if(spt_page == page1)
					{
						maxfocus_off(2,CH2MAX);
						maxfocus_on(1,CH1MAX);
					}else if(spt_page == page2){
						maxfocus_off(2,CH10MAX);
						maxfocus_on(1,CH9MAX);
					}else if(spt_page == page3){
						maxfocus_off(2,CH18MAX);
						maxfocus_on(1,CH17MAX);
					}else if(spt_page == page4){
						maxfocus_off(2,CH26MAX);
						maxfocus_on(1,CH25MAX);
					}else if(spt_page == page5){
						maxfocus_off(2,CH34MAX);
						maxfocus_on(1,CH33MAX);
					}
					op_flag = set_max1;
				}break;
				case set_max3:
				{
					if(spt_page == page1)
					{
						maxfocus_off(3,CH3MAX);
						maxfocus_on(2,CH2MAX);
					}else if(spt_page == page2){
						maxfocus_off(3,CH11MAX);
						maxfocus_on(2,CH10MAX);
					}else if(spt_page == page3){
						maxfocus_off(3,CH19MAX);
						maxfocus_on(2,CH18MAX);
					}else if(spt_page == page4){
						maxfocus_off(3,CH27MAX);
						maxfocus_on(2,CH26MAX);
					}else if(spt_page == page5){
						maxfocus_off(3,CH35MAX);
						maxfocus_on(2,CH34MAX);
					}
					op_flag = set_max2;
				}break;
				case set_max4:
				{
					if(spt_page == page1)
					{
						maxfocus_off(4,CH4MAX);
						maxfocus_on(3,CH3MAX);
					}else if(spt_page == page2){
						maxfocus_off(4,CH12MAX);
						maxfocus_on(3,CH11MAX);
					}else if(spt_page == page3){
						maxfocus_off(4,CH20MAX);
						maxfocus_on(3,CH19MAX);
					}else if(spt_page == page4){
						maxfocus_off(4,CH28MAX);
						maxfocus_on(3,CH27MAX);
					}else if(spt_page == page5){
						maxfocus_off(4,CH36MAX);
						maxfocus_on(3,CH35MAX);
					}
					op_flag = set_max3;
				}break;
				case set_max5:
				{
					if(spt_page == page1)
					{
						maxfocus_off(5,CH5MAX);
						maxfocus_on(4,CH4MAX);
					}else if(spt_page == page2){
						maxfocus_off(5,CH13MAX);
						maxfocus_on(4,CH12MAX);
					}else if(spt_page == page3){
						maxfocus_off(5,CH21MAX);
						maxfocus_on(4,CH20MAX);
					}else if(spt_page == page4){
						maxfocus_off(5,CH29MAX);
						maxfocus_on(4,CH28MAX);
					}else if(spt_page == page5){
						maxfocus_off(5,CH37MAX);
						maxfocus_on(4,CH36MAX);
					}
					op_flag = set_max4;
				}break;
				case set_max6:
				{
					if(spt_page == page1)
					{
						maxfocus_off(6,CH6MAX);
						maxfocus_on(5,CH5MAX);
					}else if(spt_page == page2){
						maxfocus_off(6,CH14MAX);
						maxfocus_on(5,CH13MAX);
					}else if(spt_page == page3){
						maxfocus_off(6,CH22MAX);
						maxfocus_on(5,CH21MAX);
					}else if(spt_page == page4){
						maxfocus_off(6,CH30MAX);
						maxfocus_on(5,CH29MAX);
					}else if(spt_page == page5){
						maxfocus_off(6,CH38MAX);
						maxfocus_on(5,CH37MAX);
					}
					op_flag = set_max5;
				}break;
				case set_max7:
				{
					if(spt_page == page1)
					{
						maxfocus_off(7,CH7MAX);
						maxfocus_on(6,CH6MAX);
					}else if(spt_page == page2){
						maxfocus_off(7,CH15MAX);
						maxfocus_on(6,CH14MAX);
					}else if(spt_page == page3){
						maxfocus_off(7,CH23MAX);
						maxfocus_on(6,CH22MAX);
					}else if(spt_page == page4){
						maxfocus_off(7,CH31MAX);
						maxfocus_on(6,CH30MAX);
					}else if(spt_page == page5){
						maxfocus_off(7,CH39MAX);
						maxfocus_on(6,CH38MAX);
					}
					op_flag = set_max6;
				}break;
				case set_max8:
				{
					if(spt_page == page1)
					{
						maxfocus_off(8,CH8MAX);
						maxfocus_on(7,CH7MAX);
					}else if(spt_page == page2){
						maxfocus_off(8,CH16MAX);
						maxfocus_on(7,CH15MAX);
					}else if(spt_page == page3){
						maxfocus_off(8,CH24MAX);
						maxfocus_on(7,CH23MAX);
					}else if(spt_page == page4){
						maxfocus_off(8,CH32MAX);
						maxfocus_on(7,CH31MAX);
					}else if(spt_page == page5){
						maxfocus_off(8,CH40MAX);
						maxfocus_on(7,CH39MAX);
					}
					op_flag = set_max7;
				}break;
			}
		}break;
		case calibrate:
		{
			switch(op_flag)
			{				
				case set_c2:
				{
					if(cor_page == page1)
					{
						maxfocus_off(2,COR2);
						maxfocus_on(1,COR1);
					}else if(cor_page == page2){
						maxfocus_off(2,COR10);
						maxfocus_on(1,COR9);
					}else if(cor_page == page3){
						maxfocus_off(2,COR18);
						maxfocus_on(1,COR17);
					}else if(cor_page == page4){
						maxfocus_off(2,COR26);
						maxfocus_on(1,COR25);
					}else if(cor_page == page5){
						maxfocus_off(2,COR34);
						maxfocus_on(1,COR33);
					}
					op_flag = set_c1;
				}break;
				case set_c3:
				{
					if(cor_page == page1)
					{
						maxfocus_off(3,COR3);
						maxfocus_on(2,COR2);
					}else if(cor_page == page2){
						maxfocus_off(3,COR11);
						maxfocus_on(2,COR10);
					}else if(cor_page == page3){
						maxfocus_off(3,COR19);
						maxfocus_on(2,COR18);
					}else if(cor_page == page4){
						maxfocus_off(3,COR27);
						maxfocus_on(2,COR26);
					}else if(cor_page == page5){
						maxfocus_off(3,COR35);
						maxfocus_on(2,COR34);
					}
					op_flag = set_c2;
				}break;
				case set_c4:
				{
					if(cor_page == page1)
					{
						maxfocus_off(4,COR4);
						maxfocus_on(3,COR3);
					}else if(cor_page == page2){
						maxfocus_off(4,COR12);
						maxfocus_on(3,COR11);
					}else if(cor_page == page3){
						maxfocus_off(4,COR20);
						maxfocus_on(3,COR19);
					}else if(cor_page == page4){
						maxfocus_off(4,COR28);
						maxfocus_on(3,COR27);
					}else if(cor_page == page5){
						maxfocus_off(4,COR36);
						maxfocus_on(3,COR35);
					}
					op_flag = set_c3;
				}break;
				case set_c5:
				{
					if(cor_page == page1)
					{
						maxfocus_off(5,COR5);
						maxfocus_on(4,COR4);
					}else if(cor_page == page2){
						maxfocus_off(5,COR13);
						maxfocus_on(4,COR12);
					}else if(cor_page == page3){
						maxfocus_off(5,COR21);
						maxfocus_on(4,COR20);
					}else if(cor_page == page4){
						maxfocus_off(5,COR29);
						maxfocus_on(4,COR28);
					}else if(cor_page == page5){
						maxfocus_off(5,COR37);
						maxfocus_on(4,COR36);
					}
					op_flag = set_c4;
				}break;
				case set_c6:
				{
					if(cor_page == page1)
					{
						maxfocus_off(6,COR6);
						maxfocus_on(5,COR5);
					}else if(cor_page == page2){
						maxfocus_off(6,COR14);
						maxfocus_on(5,COR13);
					}else if(cor_page == page3){
						maxfocus_off(6,COR22);
						maxfocus_on(5,COR21);
					}else if(cor_page == page4){
						maxfocus_off(6,COR30);
						maxfocus_on(5,COR29);
					}else if(cor_page == page5){
						maxfocus_off(6,COR38);
						maxfocus_on(5,COR37);
					}
					op_flag = set_c5;
				}break;
				case set_c7:
				{
					if(cor_page == page1)
					{
						maxfocus_off(7,COR7);
						maxfocus_on(6,COR6);
					}else if(cor_page == page2){
						maxfocus_off(7,COR15);
						maxfocus_on(6,COR14);
					}else if(cor_page == page3){
						maxfocus_off(7,COR23);
						maxfocus_on(6,COR22);
					}else if(cor_page == page4){
						maxfocus_off(7,COR31);
						maxfocus_on(6,COR30);
					}else if(cor_page == page5){
						maxfocus_off(7,COR39);
						maxfocus_on(6,COR38);
					}
					op_flag = set_c6;
				}break;
				case set_c8:
				{
					if(cor_page == page1)
					{
						maxfocus_off(8,COR8);
						maxfocus_on(7,COR7);
					}else if(cor_page == page2){
						maxfocus_off(8,COR16);
						maxfocus_on(7,COR15);
					}else if(cor_page == page3){
						maxfocus_off(8,COR24);
						maxfocus_on(7,COR23);
					}else if(cor_page == page4){
						maxfocus_off(8,COR32);
						maxfocus_on(7,COR31);
					}else if(cor_page == page5){
						maxfocus_off(8,COR40);
						maxfocus_on(7,COR39);
					}
					op_flag = set_c7;
				}break;
			}
			if(LANG == chs)
			{
				DrawInstruction("修正值");
			}else{
				DrawInstruction("Input Temperature");
			}
		}break;
		case sysset:
		{
			switch(op_flag)
			{
				case set_date:
				{
					focus_on1();
					if(LANG == chs)
					{
						LCD_DisplayStringLine(50,170,"CHINESE");
						DrawInstruction("系统语言选择");
					}else{
						LCD_DisplayStringLine(50,170,"ENGLISH");
						DrawInstruction("Select system language");
					}					
					op_flag = set_lang;
				}break;
				case set_brt:
				{
					focus_off1();
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
					if(LANG == chs)
					{
						DrawInstruction("降低亮度时间");
					}else{
						DrawInstruction("Reduce brightness");
					}
					op_flag = set_date;
				}break;
				case set_dim:
				{
					focus_off1();
					if(LANG == chs)
					{
						if(DIM == DOFF)
						{
//							brightness = 10;
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
					}else{
						if(DIM == DOFF)
						{
//							brightness = 10;
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
					}	
					focus_on1();
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
					if(LANG == chs)
					{
						DrawInstruction("背光亮度设置");
					}else{
						DrawInstruction("Set brightness");
					}
					
					op_flag = set_brt;
				}break;
				case set_touch:
				{
					focus_on1();
					if(LANG == chs)
					{
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
					}else if(LANG  == eng){
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
					}
					focus_off1();
					if(LANG == chs)
					{
						if(TOUCH == op_on)
						{
							LCD_DisplayStringLine(214,170,"打开");
						}else{
							LCD_DisplayStringLine(214,170,"关闭");
						}
					}else if(LANG  == eng){
						if(TOUCH == op_on)
						{
							LCD_DisplayStringLine(212,170,"ON");
						}else{
							LCD_DisplayStringLine(212,170,"OFF");
						}
					}
					op_flag = set_dim;
				}break;
			}
		}break;
		case graphset:
		{
			switch(op_flag)
			{
				case set_ylow:
				{
					ylfocus_off(YLLIMIT);
					yhfocus_on(YHLIMIT);
					op_flag = set_yhigh;
				}break;
				case set_multi:
				{
					multifocus_off(MULTI);
					ylfocus_on(YLLIMIT);					
					op_flag = set_ylow;
				}break;
			}
		}break;
		case history:
		{
//			if(dirflag > 1)
//			{
//				dirflag--;
//			}
//			DISP_HIS_FOLDER();
		}break;
	}
}

/*按键功能函数DOWN*/
void DOWN_HANDLE(void)
{
	char buf[10];
	switch(page_flag)
	{
		case factory:
		{
			if(CHNUM == 8)
			{
				CHNUM = 16;
			}else if(CHNUM == 16){
				CHNUM = 8;
			}
			Save_flag();
			LCD_SetTextColor(LCD_COLOR_WHITE);
			LCD_SetBackColor(LCD_COLOR_BACK);
			sprintf(buf,"%03d",CHNUM);
			LCD_DisplayStringLine(50,170,(uint8_t *)buf);
		}break;
		case display:
		{
			switch(op_flag)
			{
				case home_type:
				{
					LCD_SetTextColor(LCD_COLOR_YELLOW);
					LCD_SetBackColor(LCD_COLOR_BACK);
					if(TCTYPE == TCK)
					{
						LCD_DisplayStringLine(47,100,"TC-K");
					}else if(TCTYPE == TCT){
						LCD_DisplayStringLine(47,100,"TC-T");
					}else if(TCTYPE == TCJ){
						LCD_DisplayStringLine(47,100,"TC-J");
					}else if(TCTYPE == TCN){
						LCD_DisplayStringLine(47,100,"TC-N");
					}else if(TCTYPE == TCE){
						LCD_DisplayStringLine(47,100,"TC-E");
					}else if(TCTYPE == TCS){
						LCD_DisplayStringLine(47,100,"TC-S");
					}else if(TCTYPE == TCR){
						LCD_DisplayStringLine(47,100,"TC-R");
					}else if(TCTYPE == TCB){
						LCD_DisplayStringLine(47,100,"TC-B");
					}else if(TCTYPE == PT100){
						LCD_DisplayStringLine(47,100,"PT100");
					}else if(TCTYPE == V1){
						LCD_DisplayStringLine(47,100,"1~5V");
					}
					
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_on(CH1_SW);
							LCD_DisplayStringLine_48(100,10,"001");
						}else if(ch_page == page2){
							focus_on(CH9_SW);
							LCD_DisplayStringLine_48(100,10,"009");
						}else if(ch_page == page3){
							focus_on(CH17_SW);
							LCD_DisplayStringLine_48(100,10,"017");
						}else if(ch_page == page4){
							focus_on(CH25_SW);
							LCD_DisplayStringLine_48(100,10,"025");
						}else if(ch_page == page5){
							focus_on(CH33_SW);
							LCD_DisplayStringLine_48(100,10,"033");
						}
						if(LANG == chs)
						{
							DrawInstruction("通道设置");
						}else{
							DrawInstruction("Channel set");
						}					
						op_flag = home_ch1;
					}else if(FONT == middle){
						if(ch_page == page1)
						{
							focus_on(CH1_SW);
							DISP_CNL_M(100,10,"001");
						}else if(ch_page == page2){
							focus_on(CH25_SW);
							DISP_CNL_M(100,10,"025");
						}
						op_flag = home_ch1;
						if(LANG == chs)
						{
							DrawInstruction("通道设置");
						}else{
							DrawInstruction("Channel set");
						}
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_on(CH1_SW);
							DISP_CNL_S(100,10,"001");
						}
						op_flag = home_ch1;
						if(LANG == chs)
						{
							DrawInstruction("通道设置");
						}else{
							DrawInstruction("Channel set");
						}
					}
					DrawType(1);
				}break;
				case home_ch1:
				{
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_off(CH1_SW);
							LCD_DisplayStringLine_48(100,10,"001");
						}else if(ch_page == page2){
							focus_off(CH9_SW);
							LCD_DisplayStringLine_48(100,10,"009");
						}else if(ch_page == page3){
							focus_off(CH17_SW);
							LCD_DisplayStringLine_48(100,10,"017");
						}else if(ch_page == page4){
							focus_off(CH25_SW);
							LCD_DisplayStringLine_48(100,10,"025");
						}else if(ch_page == page5){
							focus_off(CH33_SW);
							LCD_DisplayStringLine_48(100,10,"033");
						}	
						

						if(ch_page == page1)
						{
							focus_on(CH2_SW);
							LCD_DisplayStringLine_48(170+10,10,"002");
						}else if(ch_page == page2){
							focus_on(CH10_SW);
							LCD_DisplayStringLine_48(170+10,10,"010");
						}else if(ch_page == page3){
							focus_on(CH18_SW);
							LCD_DisplayStringLine_48(170+10,10,"018");
						}else if(ch_page == page4){
							focus_on(CH26_SW);
							LCD_DisplayStringLine_48(170+10,10,"026");
						}else if(ch_page == page5){
							focus_on(CH34_SW);
							LCD_DisplayStringLine_48(170+10,10,"034");
						}
						
						op_flag = home_ch2;
					}else if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH1_SW);
							DISP_CNL_M(100,10,"001");
							focus_on(CH2_SW);
							DISP_CNL_M(140,10,"002");
						}else if(ch_page == page2){
							focus_off(CH25_SW);
							DISP_CNL_M(100,10,"025");
							focus_on(CH26_SW);
							DISP_CNL_M(140,10,"026");
						}
						op_flag = home_ch2;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH1_SW);
							DISP_CNL_S(100,10,"001");
							focus_on(CH2_SW);
							DISP_CNL_S(140,10,"002");
						}
						op_flag = home_ch2;
					}
					DrawType(1);
				}break;
				case home_ch2:
				{
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_off(CH2_SW);
							LCD_DisplayStringLine_48(170+10,10,"002");
						}else if(ch_page == page2){
							focus_off(CH10_SW);
							LCD_DisplayStringLine_48(170+10,10,"010");
						}else if(ch_page == page3){
							focus_off(CH18_SW);
							LCD_DisplayStringLine_48(170+10,10,"018");
						}else if(ch_page == page4){
							focus_off(CH26_SW);
							LCD_DisplayStringLine_48(170+10,10,"026");
						}else if(ch_page == page5){
							focus_off(CH34_SW);
							LCD_DisplayStringLine_48(170+10,10,"034");
						}
						

						if(ch_page == page1)
						{
							focus_on(CH3_SW);
							LCD_DisplayStringLine_48(240+20,10,"003");
						}else if(ch_page == page2){
							focus_on(CH11_SW);
							LCD_DisplayStringLine_48(240+20,10,"011");
						}else if(ch_page == page3){
							focus_on(CH19_SW);
							LCD_DisplayStringLine_48(240+20,10,"019");
						}else if(ch_page == page4){
							focus_on(CH27_SW);
							LCD_DisplayStringLine_48(240+20,10,"027");
						}else if(ch_page == page5){
							focus_on(CH35_SW);
							LCD_DisplayStringLine_48(240+20,10,"035");
						}
						
						op_flag = home_ch3;
					}else if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH2_SW);
							DISP_CNL_M(140,10,"002");
							focus_on(CH3_SW);
							DISP_CNL_M(180,10,"003");
						}else if(ch_page == page2){
							focus_off(CH26_SW);
							DISP_CNL_M(140,10,"026");
							focus_on(CH27_SW);
							DISP_CNL_M(180,10,"027");
						}
						op_flag = home_ch3;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH2_SW);
							DISP_CNL_S(140,10,"002");
							focus_on(CH3_SW);
							DISP_CNL_S(180,10,"003");
						}
						op_flag = home_ch3;
					}
					DrawType(1);
				}break;
				case home_ch3:
				{
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_off(CH3_SW);
							LCD_DisplayStringLine_48(240+20,10,"003");
						}else if(ch_page == page2){
							focus_off(CH11_SW);
							LCD_DisplayStringLine_48(240+20,10,"011");
						}else if(ch_page == page3){
							focus_off(CH19_SW);
							LCD_DisplayStringLine_48(240+20,10,"019");
						}else if(ch_page == page4){
							focus_off(CH27_SW);
							LCD_DisplayStringLine_48(240+20,10,"027");
						}else if(ch_page == page5){
							focus_off(CH35_SW);
							LCD_DisplayStringLine_48(240+20,10,"035");
						}
						
						if(ch_page == page1)
						{
							focus_on(CH4_SW);
							LCD_DisplayStringLine_48(310+30,10,"004");
						}else if(ch_page == page2){
							focus_on(CH12_SW);
							LCD_DisplayStringLine_48(310+30,10,"012");
						}else if(ch_page == page3){
							focus_on(CH20_SW);
							LCD_DisplayStringLine_48(310+30,10,"020");
						}else if(ch_page == page4){
							focus_on(CH28_SW);
							LCD_DisplayStringLine_48(310+30,10,"028");
						}else if(ch_page == page5){
							focus_on(CH36_SW);
							LCD_DisplayStringLine_48(310+30,10,"036");
						}
						
						op_flag = home_ch4;
					}else if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH3_SW);
							DISP_CNL_M(180,10,"003");
							focus_on(CH4_SW);
							DISP_CNL_M(220,10,"004");
						}else if(ch_page == page2){
							focus_off(CH27_SW);
							DISP_CNL_M(180,10,"027");
							focus_on(CH28_SW);
							DISP_CNL_M(220,10,"028");
						}
						op_flag = home_ch4;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH3_SW);
							DISP_CNL_S(180,10,"003");
							focus_on(CH4_SW);
							DISP_CNL_S(220,10,"004");
						}
						op_flag = home_ch4;
					}
					DrawType(1);
				}break;
				case home_ch4:
				{
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_off(CH4_SW);
							LCD_DisplayStringLine_48(310+30,10,"004");
						}else if(ch_page == page2){
							focus_off(CH12_SW);
							LCD_DisplayStringLine_48(310+30,10,"012");
						}else if(ch_page == page3){
							focus_off(CH20_SW);
							LCD_DisplayStringLine_48(310+30,10,"020");
						}else if(ch_page == page4){
							focus_off(CH28_SW);
							LCD_DisplayStringLine_48(310+30,10,"028");
						}else if(ch_page == page5){
							focus_off(CH36_SW);
							LCD_DisplayStringLine_48(310+30,10,"036");
						}
						
						LCD_SetTextColor(LCD_COLOR_BLACK);
						LCD_SetBackColor(LCD_COLOR_YELLOW);
						if(TCTYPE == TCK)
						{
							LCD_DisplayStringLine(47,100,"TC-K");
						}else if(TCTYPE == TCT){
							LCD_DisplayStringLine(47,100,"TC-T");
						}else if(TCTYPE == TCJ){
							LCD_DisplayStringLine(47,100,"TC-J");
						}else if(TCTYPE == TCN){
							LCD_DisplayStringLine(47,100,"TC-N");
						}else if(TCTYPE == TCE){
							LCD_DisplayStringLine(47,100,"TC-E");
						}else if(TCTYPE == TCS){
							LCD_DisplayStringLine(47,100,"TC-S");
						}else if(TCTYPE == TCR){
							LCD_DisplayStringLine(47,100,"TC-R");
						}else if(TCTYPE == TCB){
							LCD_DisplayStringLine(47,100,"TC-B");
						}else if(TCTYPE == PT100){
							LCD_DisplayStringLine(47,100,"PT100");
						}else if(TCTYPE == V1){
							LCD_DisplayStringLine(47,100,"1~5V");
						}
						if(LANG == chs)
						{
							DrawInstruction("热电偶型号选择");
						}else{
							DrawInstruction("Thermocouple model");
						}
						Drawhomemenu();
						op_flag = home_type;
					}else if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH4_SW);
							DISP_CNL_M(220,10,"004");
							focus_on(CH5_SW);
							DISP_CNL_M(260,10,"005");
						}else if(ch_page == page2){
							focus_off(CH28_SW);
							DISP_CNL_M(220,10,"028");
							focus_on(CH29_SW);
							DISP_CNL_M(260,10,"029");
						}
						DrawType(1);
						op_flag = home_ch5;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH4_SW);
							DISP_CNL_S(220,10,"004");
							focus_on(CH5_SW);
							DISP_CNL_S(260,10,"005");
						}
						DrawType(1);
						op_flag = home_ch5;
					}
					
				}break;
				case home_ch5:
				{
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_off(CH5_SW);
							LCD_DisplayStringLine_48(100,330,"005");
						}else if(ch_page == page2){
							focus_off(CH13_SW);
							LCD_DisplayStringLine_48(100,330,"013");
						}else if(ch_page == page3){
							focus_off(CH21_SW);
							LCD_DisplayStringLine_48(100,330,"021");
						}else if(ch_page == page4){
							focus_off(CH29_SW);
							LCD_DisplayStringLine_48(100,330,"029");
						}else if(ch_page == page5){
							focus_off(CH37_SW);
							LCD_DisplayStringLine_48(100,330,"037");
						}
						
						if(ch_page == page1)
						{
							focus_on(CH6_SW);
							LCD_DisplayStringLine_48(170+10,330,"006");
						}else if(ch_page == page2){
							focus_on(CH14_SW);
							LCD_DisplayStringLine_48(170+10,330,"014");
						}else if(ch_page == page3){
							focus_on(CH22_SW);
							LCD_DisplayStringLine_48(170+10,330,"022");
						}else if(ch_page == page4){
							focus_on(CH30_SW);
							LCD_DisplayStringLine_48(170+10,330,"030");
						}else if(ch_page == page5){
							focus_on(CH38_SW);
							LCD_DisplayStringLine_48(170+10,330,"038");
						}
						
						op_flag = home_ch6;
					}else if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH5_SW);
							DISP_CNL_M(260,10,"005");
							focus_on(CH6_SW);
							DISP_CNL_M(300,10,"006");
						}else if(ch_page == page2){
							focus_off(CH29_SW);
							DISP_CNL_M(260,10,"029");
							focus_on(CH30_SW);
							DISP_CNL_M(300,10,"030");
						}
						op_flag = home_ch6;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH5_SW);
							DISP_CNL_S(260,10,"005");
							focus_on(CH6_SW);
							DISP_CNL_S(300,10,"006");
						}
						op_flag = home_ch6;
					}
					DrawType(1);
				}break;
				case home_ch6:
				{
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_off(CH6_SW);
							LCD_DisplayStringLine_48(170+10,330,"006");
						}else if(ch_page == page2){
							focus_off(CH14_SW);
							LCD_DisplayStringLine_48(170+10,330,"014");
						}else if(ch_page == page3){
							focus_off(CH22_SW);
							LCD_DisplayStringLine_48(170+10,330,"022");
						}else if(ch_page == page4){
							focus_off(CH30_SW);
							LCD_DisplayStringLine_48(170+10,330,"030");
						}else if(ch_page == page5){
							focus_off(CH38_SW);
							LCD_DisplayStringLine_48(170+10,330,"038");
						}
						
						if(ch_page == page1)
						{
							focus_on(CH7_SW);
							LCD_DisplayStringLine_48(240+20,330,"007");
						}else if(ch_page == page2){
							focus_on(CH15_SW);
							LCD_DisplayStringLine_48(240+20,330,"015");
						}else if(ch_page == page3){
							focus_on(CH23_SW);
							LCD_DisplayStringLine_48(240+20,330,"023");
						}else if(ch_page == page4){
							focus_on(CH31_SW);
							LCD_DisplayStringLine_48(240+20,330,"031");
						}else if(ch_page == page5){
							focus_on(CH39_SW);
							LCD_DisplayStringLine_48(240+20,330,"039");
						}
						
						op_flag = home_ch7;
					}else if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH6_SW);
							DISP_CNL_M(300,10,"006");
							focus_on(CH7_SW);
							DISP_CNL_M(340,10,"007");
						}else if(ch_page == page2){
							focus_off(CH30_SW);
							DISP_CNL_M(300,10,"030");
							focus_on(CH31_SW);
							DISP_CNL_M(340,10,"031");
						}
						op_flag = home_ch7;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH6_SW);
							DISP_CNL_S(300,10,"006");
							focus_on(CH7_SW);
							DISP_CNL_S(340,10,"007");
						}
						op_flag = home_ch7;
					}
					DrawType(1);
				}break;
				case home_ch7:
				{
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_off(CH7_SW);
							LCD_DisplayStringLine_48(240+20,330,"007");
						}else if(ch_page == page2){
							focus_off(CH15_SW);
							LCD_DisplayStringLine_48(240+20,330,"015");
						}else if(ch_page == page3){
							focus_off(CH23_SW);
							LCD_DisplayStringLine_48(240+20,330,"023");
						}else if(ch_page == page4){
							focus_off(CH31_SW);
							LCD_DisplayStringLine_48(240+20,330,"031");
						}else if(ch_page == page5){
							focus_off(CH39_SW);
							LCD_DisplayStringLine_48(240+20,330,"039");
						}
						
						if(ch_page == page1)
						{
							focus_on(CH8_SW);
							LCD_DisplayStringLine_48(310+30,330,"008");
						}else if(ch_page == page2){
							focus_on(CH16_SW);
							LCD_DisplayStringLine_48(310+30,330,"016");
						}else if(ch_page == page3){
							focus_on(CH24_SW);
							LCD_DisplayStringLine_48(310+30,330,"024");
						}else if(ch_page == page4){
							focus_on(CH32_SW);
							LCD_DisplayStringLine_48(310+30,330,"032");
						}else if(ch_page == page5){
							focus_on(CH40_SW);
							LCD_DisplayStringLine_48(310+30,330,"040");
						}
						
						op_flag = home_ch8;
					}else if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH7_SW);
							DISP_CNL_M(340,10,"007");
							focus_on(CH8_SW);
							DISP_CNL_M(380,10,"008");
						}else if(ch_page == page2){
							focus_off(CH31_SW);
							DISP_CNL_M(340,10,"031");
							focus_on(CH32_SW);
							DISP_CNL_M(380,10,"032");
						}
						op_flag = home_ch8;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH7_SW);
							DISP_CNL_S(340,10,"007");
							focus_on(CH8_SW);
							DISP_CNL_S(380,10,"008");
						}
						op_flag = home_ch8;
					}
					DrawType(1);
				}break;
				case home_ch8:
				{
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_off(CH8_SW);
							LCD_DisplayStringLine_48(310+30,330,"008");
						}else if(ch_page == page2){
							focus_off(CH16_SW);
							LCD_DisplayStringLine_48(310+30,330,"016");
						}else if(ch_page == page3){
							focus_off(CH24_SW);
							LCD_DisplayStringLine_48(310+30,330,"024");
						}else if(ch_page == page4){
							focus_off(CH32_SW);
							LCD_DisplayStringLine_48(310+30,330,"032");
						}else if(ch_page == page5){
							focus_off(CH40_SW);
							LCD_DisplayStringLine_48(310+30,330,"040");
						}
						
						if(ch_page == page1)
						{
							focus_on(CH5_SW);
							LCD_DisplayStringLine_48(100,330,"005");
						}else if(ch_page == page2){
							focus_on(CH13_SW);
							LCD_DisplayStringLine_48(100,330,"013");
						}else if(ch_page == page3){
							focus_on(CH21_SW);
							LCD_DisplayStringLine_48(100,330,"021");
						}else if(ch_page == page4){
							focus_on(CH29_SW);
							LCD_DisplayStringLine_48(100,330,"029");
						}else if(ch_page == page5){
							focus_on(CH37_SW);
							LCD_DisplayStringLine_48(100,330,"037");
						}
						
						op_flag = home_ch5;
					}else if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH8_SW);
							DISP_CNL_M(380,10,"008");
							
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_SetBackColor(LCD_COLOR_YELLOW);
							if(TCTYPE == TCK)
							{
								LCD_DisplayStringLine(47,100,"TC-K");
							}else if(TCTYPE == TCT){
								LCD_DisplayStringLine(47,100,"TC-T");
							}else if(TCTYPE == TCJ){
								LCD_DisplayStringLine(47,100,"TC-J");
							}else if(TCTYPE == TCN){
								LCD_DisplayStringLine(47,100,"TC-N");
							}else if(TCTYPE == TCE){
								LCD_DisplayStringLine(47,100,"TC-E");
							}else if(TCTYPE == TCS){
								LCD_DisplayStringLine(47,100,"TC-S");
							}else if(TCTYPE == TCR){
								LCD_DisplayStringLine(47,100,"TC-R");
							}else if(TCTYPE == TCB){
								LCD_DisplayStringLine(47,100,"TC-B");
							}else if(TCTYPE == PT100){
								LCD_DisplayStringLine(47,100,"PT100");
							}else if(TCTYPE == V1){
								LCD_DisplayStringLine(47,100,"1~5V");
							}
							if(LANG == chs)
							{
								DrawInstruction("热电偶型号选择");
							}else{
								DrawInstruction("Thermocouple model");
							}
							Drawhomemenu();
							op_flag = home_type;
						}else if(ch_page == page2){
							focus_off(CH32_SW);
							DISP_CNL_M(380,10,"032");
							focus_on(CH25_SW);
							DISP_CNL_M(100,10,"025");
							op_flag = home_ch1;
						}
						
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH8_SW);
							DISP_CNL_S(380,10,"008");
							
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_SetBackColor(LCD_COLOR_YELLOW);
							if(TCTYPE == TCK)
							{
								LCD_DisplayStringLine(47,100,"TC-K");
							}else if(TCTYPE == TCT){
								LCD_DisplayStringLine(47,100,"TC-T");
							}else if(TCTYPE == TCJ){
								LCD_DisplayStringLine(47,100,"TC-J");
							}else if(TCTYPE == TCN){
								LCD_DisplayStringLine(47,100,"TC-N");
							}else if(TCTYPE == TCE){
								LCD_DisplayStringLine(47,100,"TC-E");
							}else if(TCTYPE == TCS){
								LCD_DisplayStringLine(47,100,"TC-S");
							}else if(TCTYPE == TCR){
								LCD_DisplayStringLine(47,100,"TC-R");
							}else if(TCTYPE == TCB){
								LCD_DisplayStringLine(47,100,"TC-B");
							}else if(TCTYPE == PT100){
								LCD_DisplayStringLine(47,100,"PT100");
							}else if(TCTYPE == V1){
								LCD_DisplayStringLine(47,100,"1~5V");
							}
							if(LANG == chs)
							{
								DrawInstruction("热电偶型号选择");
							}else{
								DrawInstruction("Thermocouple model");
							}
							op_flag = home_type;
						}
						Drawhomemenu();
					}
					
				}break;
				case home_ch9:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH9_SW);
							DISP_CNL_M(100,210,"009");
							focus_on(CH10_SW);
							DISP_CNL_M(140,210,"010");
						}else if(ch_page == page2){
							focus_off(CH33_SW);
							DISP_CNL_M(100,210,"033");
							focus_on(CH34_SW);
							DISP_CNL_M(140,210,"034");
						}
						op_flag = home_ch10;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH9_SW);
							DISP_CNL_S(100,134,"009");
							focus_on(CH10_SW);
							DISP_CNL_S(140,134,"010");
						}
						op_flag = home_ch10;
					}
					DrawType(1);
				}break;
				case home_ch10:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH10_SW);
							DISP_CNL_M(140,210,"010");
							focus_on(CH11_SW);
							DISP_CNL_M(180,210,"011");
						}else if(ch_page == page2){
							focus_off(CH34_SW);
							DISP_CNL_M(140,210,"034");
							focus_on(CH35_SW);
							DISP_CNL_M(180,210,"035");
						}
						op_flag = home_ch11;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH10_SW);
							DISP_CNL_S(140,134,"010");
							focus_on(CH11_SW);
							DISP_CNL_S(180,134,"011");
						}
						op_flag = home_ch11;
					}
					DrawType(1);
				}break;
				case home_ch11:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH11_SW);
							DISP_CNL_M(180,210,"011");
							focus_on(CH12_SW);
							DISP_CNL_M(220,210,"012");
						}else if(ch_page == page2){
							focus_off(CH35_SW);
							DISP_CNL_M(180,210,"035");
							focus_on(CH35_SW);
							DISP_CNL_M(220,210,"036");
						}
						op_flag = home_ch12;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH11_SW);
							DISP_CNL_S(180,134,"011");
							focus_on(CH12_SW);
							DISP_CNL_S(220,134,"012");
						}
						op_flag = home_ch12;
					}
					DrawType(1);
				}break;
				case home_ch12:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH12_SW);
							DISP_CNL_M(220,210,"012");
							focus_on(CH13_SW);
							DISP_CNL_M(260,210,"013");
						}else if(ch_page == page2){
							focus_off(CH36_SW);
							DISP_CNL_M(220,210,"036");
							focus_on(CH37_SW);
							DISP_CNL_M(260,210,"037");
						}
						op_flag = home_ch13;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH12_SW);
							DISP_CNL_S(220,134,"012");
							focus_on(CH13_SW);
							DISP_CNL_S(260,134,"013");
						}
						op_flag = home_ch13;
					}
					DrawType(1);
				}break;
				case home_ch13:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH13_SW);
							DISP_CNL_M(260,210,"013");
							focus_on(CH14_SW);
							DISP_CNL_M(300,210,"014");
						}else if(ch_page == page2){
							focus_off(CH37_SW);
							DISP_CNL_M(260,210,"037");
							focus_on(CH38_SW);
							DISP_CNL_M(300,210,"038");
						}
						op_flag = home_ch14;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH13_SW);
							DISP_CNL_S(260,134,"013");
							focus_on(CH14_SW);
							DISP_CNL_S(300,134,"014");
						}
						op_flag = home_ch14;
					}
					DrawType(1);
				}break;
				case home_ch14:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH14_SW);
							DISP_CNL_M(300,210,"014");
							focus_on(CH15_SW);
							DISP_CNL_M(340,210,"015");
						}else if(ch_page == page2){
							focus_off(CH38_SW);
							DISP_CNL_M(300,210,"038");
							focus_on(CH39_SW);
							DISP_CNL_M(340,210,"039");
						}
						op_flag = home_ch15;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH14_SW);
							DISP_CNL_S(300,134,"014");
							focus_on(CH15_SW);
							DISP_CNL_S(340,134,"015");
						}
						op_flag = home_ch15;
					}
					DrawType(1);
				}break;
				case home_ch15:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH15_SW);
							DISP_CNL_M(340,210,"015");
							focus_on(CH15_SW);
							DISP_CNL_M(380,210,"016");
						}else if(ch_page == page2){
							focus_off(CH39_SW);
							DISP_CNL_M(340,210,"039");
							focus_on(CH40_SW);
							DISP_CNL_M(380,210,"040");
						}
						op_flag = home_ch16;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH15_SW);
							DISP_CNL_S(340,134,"015");
							focus_on(CH16_SW);
							DISP_CNL_S(380,134,"016");
						}
						op_flag = home_ch16;
					}
					DrawType(1);
				}break;
				case home_ch16:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH16_SW);
							DISP_CNL_M(380,210,"016");
							focus_on(CH9_SW);
							DISP_CNL_M(100,210,"009");
						}else if(ch_page == page2){
							focus_off(CH40_SW);
							DISP_CNL_M(380,210,"040");
							focus_on(CH33_SW);
							DISP_CNL_M(100,210,"033");
						}
						op_flag = home_ch9;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH16_SW);
							DISP_CNL_S(380,134,"016");
							focus_on(CH9_SW);
							DISP_CNL_S(100,134,"009");
						}
						op_flag = home_ch9;
					}
					DrawType(1);
				}break;
				case home_ch17:
				{
					if(FONT == middle){
						focus_off(CH17_SW);
						DISP_CNL_M(100,410,"017");
						focus_on(CH18_SW);
						DISP_CNL_M(140,410,"018");
						op_flag = home_ch18;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH17_SW);
							DISP_CNL_S(100,258,"017");
							focus_on(CH18_SW);
							DISP_CNL_S(140,258,"018");
						}
						op_flag = home_ch18;
					}
					DrawType(1);
				}break;
				case home_ch18:
				{
					if(FONT == middle){
						focus_off(CH18_SW);
						DISP_CNL_M(140,410,"018");
						focus_on(CH19_SW);
						DISP_CNL_M(180,410,"019");
						op_flag = home_ch19;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH18_SW);
							DISP_CNL_S(140,258,"018");
							focus_on(CH19_SW);
							DISP_CNL_S(180,258,"019");
						}
						op_flag = home_ch19;
					}
					DrawType(1);
				}break;
				case home_ch19:
				{
					if(FONT == middle){
						focus_off(CH19_SW);
						DISP_CNL_M(180,410,"019");
						focus_on(CH20_SW);
						DISP_CNL_M(220,410,"020");
						op_flag = home_ch20;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH19_SW);
							DISP_CNL_S(180,258,"019");
							focus_on(CH20_SW);
							DISP_CNL_S(220,258,"020");
						}
						op_flag = home_ch20;
					}
					DrawType(1);
				}break;
				case home_ch20:
				{
					if(FONT == middle){
						focus_off(CH20_SW);
						DISP_CNL_M(220,410,"020");
						focus_on(CH21_SW);
						DISP_CNL_M(260,410,"021");
						op_flag = home_ch21;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH20_SW);
							DISP_CNL_S(220,258,"020");
							focus_on(CH21_SW);
							DISP_CNL_S(260,258,"021");
						}
						op_flag = home_ch21;
					}
					DrawType(1);
				}break;
				case home_ch21:
				{
					if(FONT == middle){
						focus_off(CH21_SW);
						DISP_CNL_M(260,410,"021");
						focus_on(CH22_SW);
						DISP_CNL_M(300,410,"022");
						op_flag = home_ch22;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH21_SW);
							DISP_CNL_S(260,258,"021");
							focus_on(CH22_SW);
							DISP_CNL_S(300,258,"022");
						}
						op_flag = home_ch22;
					}
					DrawType(1);
				}break;
				case home_ch22:
				{
					if(FONT == middle){
						focus_off(CH22_SW);
						DISP_CNL_M(300,410,"022");
						focus_on(CH23_SW);
						DISP_CNL_M(340,410,"023");
						op_flag = home_ch23;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH22_SW);
							DISP_CNL_S(300,258,"022");
							focus_on(CH23_SW);
							DISP_CNL_S(340,258,"023");
						}
						op_flag = home_ch23;
					}
					DrawType(1);
				}break;
				case home_ch23:
				{
					if(FONT == middle){
						focus_off(CH23_SW);
						DISP_CNL_M(340,410,"023");
						focus_on(CH24_SW);
						DISP_CNL_M(380,410,"024");
						op_flag = home_ch24;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH23_SW);
							DISP_CNL_S(340,258,"023");
							focus_on(CH24_SW);
							DISP_CNL_S(380,258,"024");
						}
						op_flag = home_ch24;
					}
					DrawType(1);
				}break;
				case home_ch24:
				{
					if(FONT == middle){
						focus_off(CH24_SW);
						DISP_CNL_M(380,410,"024");
						focus_on(CH17_SW);
						DISP_CNL_M(100,410,"017");
						op_flag = home_ch17;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH24_SW);
							DISP_CNL_S(380,258,"024");
							focus_on(CH17_SW);
							DISP_CNL_S(100,258,"017");
						}
						op_flag = home_ch17;
					}
					DrawType(1);
				}break;
				case home_ch25:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH25_SW);
							DISP_CNL_S(100,382,"025");
							focus_on(CH26_SW);
							DISP_CNL_S(140,382,"026");
						}
						op_flag = home_ch26;
					}
					DrawType(1);
				}break;
				case home_ch26:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH26_SW);
							DISP_CNL_S(140,382,"026");
							focus_on(CH27_SW);
							DISP_CNL_S(180,382,"027");
						}
						op_flag = home_ch27;
					}
					DrawType(1);
				}break;
				case home_ch27:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH27_SW);
							DISP_CNL_S(180,382,"027");
							focus_on(CH28_SW);
							DISP_CNL_S(220,382,"028");
						}
						op_flag = home_ch28;
					}
					DrawType(1);
				}break;
				case home_ch28:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH28_SW);
							DISP_CNL_S(220,382,"028");
							focus_on(CH29_SW);
							DISP_CNL_S(260,382,"029");
						}
						op_flag = home_ch29;
					}
					DrawType(1);
				}break;
				case home_ch29:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH29_SW);
							DISP_CNL_S(260,382,"029");
							focus_on(CH30_SW);
							DISP_CNL_S(300,382,"030");
						}
						op_flag = home_ch30;
					}
					DrawType(1);
				}break;
				case home_ch30:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH30_SW);
							DISP_CNL_S(300,382,"030");
							focus_on(CH31_SW);
							DISP_CNL_S(340,382,"031");
						}
						op_flag = home_ch31;
					}
					DrawType(1);
				}break;				
				case home_ch31:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH31_SW);
							DISP_CNL_S(340,382,"031");
							focus_on(CH32_SW);
							DISP_CNL_S(380,382,"032");
						}
						op_flag = home_ch32;
					}
					DrawType(1);
				}break;
				case home_ch32:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH32_SW);
							DISP_CNL_S(380,382,"032");
							focus_on(CH25_SW);
							DISP_CNL_S(100,382,"025");
						}
						op_flag = home_ch25;
					}
					DrawType(1);
				}break;
				case home_ch33:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH33_SW);
							DISP_CNL_S(100,506,"033");
							focus_on(CH34_SW);
							DISP_CNL_S(140,506,"034");
						}
						op_flag = home_ch34;
					}
					DrawType(1);
				}break;
				case home_ch34:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH34_SW);
							DISP_CNL_S(140,506,"034");
							focus_on(CH35_SW);
							DISP_CNL_S(180,506,"035");
						}
						op_flag = home_ch35;
					}
					DrawType(1);
				}break;
				case home_ch35:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH35_SW);
							DISP_CNL_S(180,506,"035");
							focus_on(CH36_SW);
							DISP_CNL_S(220,506,"036");
						}
						op_flag = home_ch36;
					}
					DrawType(1);
				}break;
				case home_ch36:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH36_SW);
							DISP_CNL_S(220,506,"036");
							focus_on(CH37_SW);
							DISP_CNL_S(260,506,"037");
						}
						op_flag = home_ch37;
					}
					DrawType(1);
				}break;
				case home_ch37:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH37_SW);
							DISP_CNL_S(260,506,"037");
							focus_on(CH38_SW);
							DISP_CNL_S(300,506,"038");
						}
						op_flag = home_ch38;
					}
					DrawType(1);
				}break;
				case home_ch38:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH38_SW);
							DISP_CNL_S(300,506,"038");
							focus_on(CH39_SW);
							DISP_CNL_S(340,506,"039");
						}
						op_flag = home_ch39;
					}
					DrawType(1);
				}break;
				case home_ch39:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH39_SW);
							DISP_CNL_S(340,506,"039");
							focus_on(CH40_SW);
							DISP_CNL_S(380,506,"040");
						}
						op_flag = home_ch40;
					}
					DrawType(1);
				}break;
				case home_ch40:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH40_SW);
							DISP_CNL_S(380,506,"040");
							focus_on(CH33_SW);
							DISP_CNL_S(100,506,"033");
						}
						op_flag = home_ch33;
					}
					DrawType(1);
				}break;
			}
		}break;
		
		case settings:
		{
			switch(op_flag)
			{
				case set_filter:
				{
					focus_off1();
					if(FILTER == ft_off){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,150,"关闭");
						}else{
							LCD_DisplayStringLine(50,150,"OFF");
						}
					}else{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,150,"打开");
						}else{
							LCD_DisplayStringLine(50,150,"ON");
						}
					}
					
					focus_on1();
					if(BEEP == beep_off){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(90,150,"关闭");
						}else{
							LCD_DisplayStringLine(90,150,"OFF");
						}
					}else{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(90,150,"打开");
						}else{
							LCD_DisplayStringLine(90,150,"ON");
						}
					}
					if(LANG == chs)
					{
						DrawInstruction("蜂鸣器设置");
					}else{
						DrawInstruction("Buzzer ON/OFF");
					}
					op_flag = set_beep;
				}break;				
				case set_beep:
				{
					focus_off1();
					if(BEEP == beep_off){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(90,150,"关闭");
						}else{
							LCD_DisplayStringLine(90,150,"OFF");
						}
					}else{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(90,150,"打开");
						}else{
							LCD_DisplayStringLine(90,150,"ON");
						}
					}
					
					LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
					LCD_DrawFullRect(150,127,24,31);
					if(UNIT == C){
						LCD_SetTextColor(LCD_COLOR_BLACK);
						DISP_CNL_S(127,150,"o");
						LCD_DisplayStringLine(127,155,"C");
					}else if(UNIT == F){
						LCD_SetTextColor(LCD_COLOR_BLACK);
						DISP_CNL_S(127,150,"o");
						LCD_DisplayStringLine(127,155,"F");
					}else{
						LCD_SetTextColor(LCD_COLOR_BLACK);
						LCD_DisplayStringLine(127,155,"K");
					}
					if(LANG == chs)
					{
						DrawInstruction("温度单位选择");
					}else{
						DrawInstruction("Select temperature unit");
					}
					op_flag = set_unit;					
				}break;
				case set_unit:
				{
					LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
					LCD_DrawFullRect(150,127,24,31);
					if(UNIT == C){
						LCD_SetTextColor(LCD_COLOR_YELLOW);
						DISP_CNL_S(127,150,"o");
						LCD_DisplayStringLine(127,155,"C");
					}else if(UNIT == F){
						LCD_SetTextColor(LCD_COLOR_YELLOW);
						DISP_CNL_S(127,150,"o");
						LCD_DisplayStringLine(127,155,"F");
					}else{
						LCD_SetTextColor(LCD_COLOR_YELLOW);
						LCD_DisplayStringLine(127,155,"K");
					}
					
					focus_on1();
					if(TIMETIRG == trig_off){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(170,150,"关闭");
						}else{
							LCD_DisplayStringLine(170,150,"OFF");
						}
					}else{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(170,150,"打开");
						}else{
							LCD_DisplayStringLine(170,150,"ON");
						}
					}
					if(LANG == chs)
					{
						DrawInstruction("定时采集");
					}else{
						DrawInstruction("Trig Timer");
					}
					op_flag = timetrig;
					
					
				}break;
				case timetrig:
				{
					focus_off1();
					if(TIMETIRG == trig_off){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(170,150,"关闭");
						}else{
							LCD_DisplayStringLine(170,150,"OFF");
						}
					}else{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(170,150,"打开");
						}else{
							LCD_DisplayStringLine(170,150,"ON");
						}
					}
					
					focus_on1();
					sprintf(buf,"%0.2d:%0.2d",
							STARTH,
							STARTM);
					LCD_DisplayStringLine(208,150,(uint8_t *)buf);
					if(LANG == chs)
					{
						DrawInstruction("采集开始时间");
					}else{
						DrawInstruction("START");
					}
					bit_flag = 1;
					op_flag = starttime;
//					stimefocus_on(SAVETIME);
//					if(LANG == chs)
//					{
//						DrawInstruction("记录间隔时间");
//					}else{
//						DrawInstruction("Record interval");
//					}
//					op_flag = set_timer;
				}break;		
				case starttime:
				{
					focus_off1();
					sprintf(buf,"%0.2d:%0.2d",
							STARTH,
							STARTM);
					LCD_DisplayStringLine(208,150,(uint8_t *)buf);
					focus_on1();
					sprintf(buf,"%0.2d:%0.2d",
							ENDH,
							ENDM);
					LCD_DisplayStringLine(248,150,(uint8_t *)buf);
					if(LANG == chs)
					{
						DrawInstruction("采集结束时间");
					}else{
						DrawInstruction("END");
					}
					bit_flag = 1;
					op_flag = endtime;
				}break;
				case endtime:
				{
					focus_off1();
					sprintf(buf,"%0.2d:%0.2d",
							ENDH,
							ENDM);
					LCD_DisplayStringLine(248,150,(uint8_t *)buf);
					
					stimefocus_on(SAVETIME);
					if(LANG == chs)
					{
						DrawInstruction("记录间隔时间");
					}else{
						DrawInstruction("Record interval");
					}
					op_flag = set_timer;
				}break;
				case set_spd:
				{
					focus_off1();
					if(SPEED == fast)
					{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,500,"快速");
						}else{
							LCD_DisplayStringLine(50,500,"FAST");
						}
					}else if(SPEED == moderate){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,500,"中速");
						}else{
							LCD_DisplayStringLine(50,500,"MED");
						}
					}else if(SPEED == slow){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,500,"慢速");
						}else{
							LCD_DisplayStringLine(50,500,"SLOW");
						}
					}
					
					focus_on1();
					if(BAUD == b9600)
					{
						LCD_DisplayStringLine(87,500,"9600");
					}else if(BAUD == b19200){
						LCD_DisplayStringLine(87,500,"19200");
					}else if(BAUD == b38400){
						LCD_DisplayStringLine(87,500,"38400");
					}else if(BAUD == b57600){
						LCD_DisplayStringLine(87,500,"57600");
					}else if(BAUD == b115200){
						LCD_DisplayStringLine(87,500,"115200");
					}
					if(LANG == chs)
					{
						DrawInstruction("波特率设置");
					}else{
						DrawInstruction("Select Baud rate");
					}
					op_flag = set_baud;
				}break;
				case set_baud:
				{
					focus_off1();
					if(BAUD == b9600)
					{
						LCD_DisplayStringLine(87,500,"9600");
					}else if(BAUD == b19200){
						LCD_DisplayStringLine(87,500,"19200");
					}else if(BAUD == b38400){
						LCD_DisplayStringLine(87,500,"38400");
					}else if(BAUD == b57600){
						LCD_DisplayStringLine(87,500,"57600");
					}else if(BAUD == b115200){
						LCD_DisplayStringLine(87,500,"115200");
					}
					
					focus_on1();
					if(REPEAT == rtp_off){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(130,500,"关闭");
						}else{
							LCD_DisplayStringLine(130,500,"OFF");
						}
					}else{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(130,500,"打开");
						}else{
							LCD_DisplayStringLine(130,500,"ON");
						}
					}
//					LCD_DisplayStringLine(322,516,"S");
					if(LANG == chs)
					{
						DrawInstruction("重复采集");
					}else{
						DrawInstruction("REPEAT");
					}
					op_flag = repeat;
				}break;
			}
		}break;
		case separation:
		{
			switch(op_flag)
			{
				case set_min1:
				{
					if(spt_page == page1)
					{
						minfocus_on(2,CH2MIN);
						minfocus_off(1,CH1MIN);
					}else if(spt_page == page2){
						minfocus_on(2,CH10MIN);
						minfocus_off(1,CH9MIN);
					}else if(spt_page == page3){
						minfocus_on(2,CH18MIN);
						minfocus_off(1,CH17MIN);
					}else if(spt_page == page4){
						minfocus_on(2,CH26MIN);
						minfocus_off(1,CH25MIN);
					}else if(spt_page == page5){
						minfocus_on(2,CH34MIN);
						minfocus_off(1,CH33MIN);
					}
					op_flag = set_min2;
				}break;
				case set_min2:
				{
					if(spt_page == page1)
					{
						minfocus_off(2,CH2MIN);
						minfocus_on(3,CH3MIN);
					}else if(spt_page == page2){
						minfocus_off(2,CH10MIN);
						minfocus_on(3,CH11MIN);
					}else if(spt_page == page3){
						minfocus_off(2,CH18MIN);
						minfocus_on(3,CH19MIN);
					}else if(spt_page == page4){
						minfocus_off(2,CH26MIN);
						minfocus_on(3,CH27MIN);
					}else if(spt_page == page5){
						minfocus_off(2,CH34MIN);
						minfocus_on(3,CH35MIN);
					}
					op_flag = set_min3;
				}break;
				case set_min3:
				{
					if(spt_page == page1)
					{
						minfocus_off(3,CH3MIN);
						minfocus_on(4,CH4MIN);
					}else if(spt_page == page2){
						minfocus_off(3,CH11MIN);
						minfocus_on(4,CH12MIN);
					}else if(spt_page == page3){
						minfocus_off(3,CH19MIN);
						minfocus_on(4,CH20MIN);
					}else if(spt_page == page4){
						minfocus_off(3,CH27MIN);
						minfocus_on(4,CH28MIN);
					}else if(spt_page == page5){
						minfocus_off(3,CH35MIN);
						minfocus_on(4,CH36MIN);
					}
					op_flag = set_min4;
				}break;
				case set_min4:
				{
					if(spt_page == page1)
					{
						minfocus_off(4,CH4MIN);
						minfocus_on(5,CH5MIN);
					}else if(spt_page == page2){
						minfocus_off(4,CH12MIN);
						minfocus_on(5,CH13MIN);
					}else if(spt_page == page3){
						minfocus_off(4,CH20MIN);
						minfocus_on(5,CH21MIN);
					}else if(spt_page == page4){
						minfocus_off(4,CH28MIN);
						minfocus_on(5,CH29MIN);
					}else if(spt_page == page5){
						minfocus_off(4,CH36MIN);
						minfocus_on(5,CH37MIN);
					}
					op_flag = set_min5;
				}break;
				case set_min5:
				{
					if(spt_page == page1)
					{
						minfocus_off(5,CH5MIN);
						minfocus_on(6,CH6MIN);
					}else if(spt_page == page2){
						minfocus_off(5,CH13MIN);
						minfocus_on(6,CH14MIN);
					}else if(spt_page == page3){
						minfocus_off(5,CH21MIN);
						minfocus_on(6,CH22MIN);
					}else if(spt_page == page4){
						minfocus_off(5,CH29MIN);
						minfocus_on(6,CH30MIN);
					}else if(spt_page == page5){
						minfocus_off(5,CH37MIN);
						minfocus_on(6,CH38MIN);
					}
					op_flag = set_min6;
				}break;
				case set_min6:
				{
					if(spt_page == page1)
					{
						minfocus_off(6,CH6MIN);
						minfocus_on(7,CH7MIN);
					}else if(spt_page == page2){
						minfocus_off(6,CH14MIN);
						minfocus_on(7,CH15MIN);
					}else if(spt_page == page3){
						minfocus_off(6,CH22MIN);
						minfocus_on(7,CH23MIN);
					}else if(spt_page == page4){
						minfocus_off(6,CH30MIN);
						minfocus_on(7,CH31MIN);
					}else if(spt_page == page5){
						minfocus_off(6,CH38MIN);
						minfocus_on(7,CH39MIN);
					}
					op_flag = set_min7;
				}break;
				case set_min7:
				{
					if(spt_page == page1)
					{
						minfocus_off(7,CH7MIN);
						minfocus_on(8,CH8MIN);
					}else if(spt_page == page2){
						minfocus_off(7,CH15MIN);
						minfocus_on(8,CH16MIN);
					}else if(spt_page == page3){
						minfocus_off(7,CH23MIN);
						minfocus_on(8,CH24MIN);
					}else if(spt_page == page4){
						minfocus_off(7,CH31MIN);
						minfocus_on(8,CH32MIN);
					}else if(spt_page == page5){
						minfocus_off(7,CH39MIN);
						minfocus_on(8,CH40MIN);
					}
					op_flag = set_min8;
				}break;
				case set_max1:
				{
					if(spt_page == page1)
					{
						maxfocus_on(2,CH2MAX);
						maxfocus_off(1,CH1MAX);
					}else if(spt_page == page2){
						maxfocus_on(2,CH10MAX);
						maxfocus_off(1,CH9MAX);
					}else if(spt_page == page3){
						maxfocus_on(2,CH18MAX);
						maxfocus_off(1,CH17MAX);
					}else if(spt_page == page4){
						maxfocus_on(2,CH26MAX);
						maxfocus_off(1,CH25MAX);
					}else if(spt_page == page5){
						maxfocus_on(2,CH34MAX);
						maxfocus_off(1,CH33MAX);
					}
					op_flag = set_max2;
				}break;
				case set_max2:
				{
					if(spt_page == page1)
					{
						maxfocus_off(2,CH2MAX);
						maxfocus_on(3,CH3MAX);
					}else if(spt_page == page2){
						maxfocus_off(2,CH10MAX);
						maxfocus_on(3,CH11MAX);
					}else if(spt_page == page3){
						maxfocus_off(2,CH18MAX);
						maxfocus_on(3,CH19MAX);
					}else if(spt_page == page4){
						maxfocus_off(2,CH26MAX);
						maxfocus_on(3,CH27MAX);
					}else if(spt_page == page5){
						maxfocus_off(2,CH34MAX);
						maxfocus_on(3,CH35MAX);
					}
					op_flag = set_max3;
				}break;
				case set_max3:
				{
					if(spt_page == page1)
					{
						maxfocus_off(3,CH3MAX);
						maxfocus_on(4,CH4MAX);
					}else if(spt_page == page2){
						maxfocus_off(3,CH11MAX);
						maxfocus_on(4,CH12MAX);
					}else if(spt_page == page3){
						maxfocus_off(3,CH19MAX);
						maxfocus_on(4,CH20MAX);
					}else if(spt_page == page4){
						maxfocus_off(3,CH27MAX);
						maxfocus_on(4,CH28MAX);
					}else if(spt_page == page5){
						maxfocus_off(3,CH35MAX);
						maxfocus_on(4,CH36MAX);
					}
					op_flag = set_max4;
				}break;
				case set_max4:
				{
					if(spt_page == page1)
					{
						maxfocus_off(4,CH4MAX);
						maxfocus_on(5,CH5MAX);
					}else if(spt_page == page2){
						maxfocus_off(4,CH12MAX);
						maxfocus_on(5,CH13MAX);
					}else if(spt_page == page3){
						maxfocus_off(4,CH20MAX);
						maxfocus_on(5,CH21MAX);
					}else if(spt_page == page4){
						maxfocus_off(4,CH28MAX);
						maxfocus_on(5,CH29MAX);
					}else if(spt_page == page5){
						maxfocus_off(4,CH36MAX);
						maxfocus_on(5,CH37MAX);
					}
					op_flag = set_max5;
				}break;
				case set_max5:
				{
					if(spt_page == page1)
					{
						maxfocus_off(5,CH5MAX);
						maxfocus_on(6,CH6MAX);
					}else if(spt_page == page2){
						maxfocus_off(5,CH13MAX);
						maxfocus_on(6,CH14MAX);
					}else if(spt_page == page3){
						maxfocus_off(5,CH21MAX);
						maxfocus_on(6,CH22MAX);
					}else if(spt_page == page4){
						maxfocus_off(5,CH29MAX);
						maxfocus_on(6,CH30MAX);
					}else if(spt_page == page5){
						maxfocus_off(5,CH37MAX);
						maxfocus_on(6,CH38MAX);
					}
					op_flag = set_max6;
				}break;
				case set_max6:
				{
					if(spt_page == page1)
					{
						maxfocus_off(6,CH6MAX);
						maxfocus_on(7,CH7MAX);
					}else if(spt_page == page2){
						maxfocus_off(6,CH14MAX);
						maxfocus_on(7,CH15MAX);
					}else if(spt_page == page3){
						maxfocus_off(6,CH22MAX);
						maxfocus_on(7,CH23MAX);
					}else if(spt_page == page4){
						maxfocus_off(6,CH30MAX);
						maxfocus_on(7,CH31MAX);
					}else if(spt_page == page5){
						maxfocus_off(6,CH38MAX);
						maxfocus_on(7,CH39MAX);
					}
					op_flag = set_max7;
				}break;
				case set_max7:
				{
					if(spt_page == page1)
					{
						maxfocus_off(7,CH7MAX);
						maxfocus_on(8,CH8MAX);
					}else if(spt_page == page2){
						maxfocus_off(7,CH15MAX);
						maxfocus_on(8,CH16MAX);
					}else if(spt_page == page3){
						maxfocus_off(7,CH23MAX);
						maxfocus_on(8,CH24MAX);
					}else if(spt_page == page4){
						maxfocus_off(7,CH31MAX);
						maxfocus_on(8,CH32MAX);
					}else if(spt_page == page5){
						maxfocus_off(7,CH39MAX);
						maxfocus_on(8,CH40MAX);
					}
					op_flag = set_max8;
				}break;
				
			}
		}break;
		case calibrate:
		{
			switch(op_flag)
			{
				case set_c1:
				{
					if(cor_page == page1)
					{
						maxfocus_on(2,COR2);
						maxfocus_off(1,COR1);
					}else if(cor_page == page2){
						maxfocus_on(2,COR10);
						maxfocus_off(1,COR9);
					}else if(cor_page == page3){
						maxfocus_on(2,COR18);
						maxfocus_off(1,COR17);
					}else if(cor_page == page4){
						maxfocus_on(2,COR26);
						maxfocus_off(1,COR25);
					}else if(cor_page == page5){
						maxfocus_on(2,COR34);
						maxfocus_off(1,COR33);
					}
					op_flag = set_c2;
				}break;
				case set_c2:
				{
					if(cor_page == page1)
					{
						maxfocus_off(2,COR2);
						maxfocus_on(3,COR3);
					}else if(cor_page == page2){
						maxfocus_off(2,COR10);
						maxfocus_on(3,COR11);
					}else if(cor_page == page3){
						maxfocus_off(2,COR18);
						maxfocus_on(3,COR19);
					}else if(cor_page == page4){
						maxfocus_off(2,COR26);
						maxfocus_on(3,COR27);
					}else if(cor_page == page5){
						maxfocus_off(2,COR34);
						maxfocus_on(3,COR35);
					}
					op_flag = set_c3;
				}break;
				case set_c3:
				{
					if(cor_page == page1)
					{
						maxfocus_off(3,COR3);
						maxfocus_on(4,COR4);
					}else if(cor_page == page2){
						maxfocus_off(3,COR11);
						maxfocus_on(4,COR12);
					}else if(cor_page == page3){
						maxfocus_off(3,COR19);
						maxfocus_on(4,COR20);
					}else if(cor_page == page4){
						maxfocus_off(3,COR27);
						maxfocus_on(4,COR28);
					}else if(cor_page == page5){
						maxfocus_off(3,COR35);
						maxfocus_on(4,COR36);
					}
					op_flag = set_c4;
				}break;
				case set_c4:
				{
					if(cor_page == page1)
					{
						maxfocus_off(4,COR4);
						maxfocus_on(5,COR5);
					}else if(cor_page == page2){
						maxfocus_off(4,COR12);
						maxfocus_on(5,COR13);
					}else if(cor_page == page3){
						maxfocus_off(4,COR20);
						maxfocus_on(5,COR21);
					}else if(cor_page == page4){
						maxfocus_off(4,COR28);
						maxfocus_on(5,COR29);
					}else if(cor_page == page5){
						maxfocus_off(4,COR36);
						maxfocus_on(5,COR37);
					}
					op_flag = set_c5;
				}break;
				case set_c5:
				{
					if(cor_page == page1)
					{
						maxfocus_off(5,COR5);
						maxfocus_on(6,COR6);
					}else if(cor_page == page2){
						maxfocus_off(5,COR13);
						maxfocus_on(6,COR14);
					}else if(cor_page == page3){
						maxfocus_off(5,COR21);
						maxfocus_on(6,COR22);
					}else if(cor_page == page4){
						maxfocus_off(5,COR29);
						maxfocus_on(6,COR30);
					}else if(cor_page == page5){
						maxfocus_off(5,COR37);
						maxfocus_on(6,COR38);
					}
					op_flag = set_c6;
				}break;
				case set_c6:
				{
					if(cor_page == page1)
					{
						maxfocus_off(6,COR6);
						maxfocus_on(7,COR7);
					}else if(cor_page == page2){
						maxfocus_off(6,COR14);
						maxfocus_on(7,COR15);
					}else if(cor_page == page3){
						maxfocus_off(6,COR22);
						maxfocus_on(7,COR23);
					}else if(cor_page == page4){
						maxfocus_off(6,COR30);
						maxfocus_on(7,COR31);
					}else if(cor_page == page5){
						maxfocus_off(6,COR38);
						maxfocus_on(7,COR39);
					}
					op_flag = set_c7;
				}break;
				case set_c7:
				{
					if(cor_page == page1)
					{
						maxfocus_off(7,COR7);
						maxfocus_on(8,COR8);
					}else if(cor_page == page2){
						maxfocus_off(7,COR15);
						maxfocus_on(8,COR16);
					}else if(cor_page == page3){
						maxfocus_off(7,COR23);
						maxfocus_on(8,COR24);
					}else if(cor_page == page4){
						maxfocus_off(7,COR31);
						maxfocus_on(8,COR32);
					}else if(cor_page == page5){
						maxfocus_off(7,COR39);
						maxfocus_on(8,COR40);
					}
					op_flag = set_c8;
				}break;
				
			}
			if(LANG == chs)
			{
				DrawInstruction("修正值");
			}else{
				DrawInstruction("Input Temperature");
			}
		}break;
		case sysset:
		{
			switch(op_flag)
			{
				case set_lang:
				{
					focus_off1();
					if(LANG == chs)
					{
						LCD_DisplayStringLine(50,170,"CHINESE");
						DrawInstruction("年-月-日");
					}else{
						LCD_DisplayStringLine(50,170,"ENGLISH");
						DrawInstruction("YEAR-MONTH-DAY");
					}

					op_flag = set_date;
				}break;
				case set_date:
				{
					
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
					focus_on1();					
					LCD_DisplayStringLine(130,170,(uint8_t*)buf);
					if(LANG == chs)
					{
						DrawInstruction("背光亮度设置");
					}else{
						DrawInstruction("Set brightness");
					}
					op_flag = set_brt;
				}break;
				case set_brt:
				{
					focus_off1();
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
					focus_on1();
					if(LANG == chs)
					{
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
					}else if(LANG  == eng){
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
					}
					if(LANG == chs)
					{
						DrawInstruction("降低亮度时间");
					}else{
						DrawInstruction("Reduce brightness");
					}
					op_flag = set_dim;
				}break;
				case set_dim:
				{
					focus_off1();
					if(LANG == chs)
					{
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
					}else if(LANG  == eng){
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
					}
					focus_on1();
					if(LANG == chs)
					{
						if(TOUCH == op_on)
						{
							LCD_DisplayStringLine(214,170,"打开");
						}else{
							LCD_DisplayStringLine(214,170,"关闭");
						}
					}else if(LANG  == eng){
						if(TOUCH == op_on)
						{
							LCD_DisplayStringLine(212,170,"ON");
						}else{
							LCD_DisplayStringLine(212,170,"OFF");
						}
					}
					op_flag = set_touch;
				}break;
			}
		}break;
		case graphset:
		{
			switch(op_flag)
			{
				case set_yhigh:
				{
					yhfocus_off(YHLIMIT);
					ylfocus_on(YLLIMIT);
					op_flag = set_ylow;
				}break;
				case set_ylow:
				{
					ylfocus_off(YLLIMIT);
					multifocus_on(MULTI);
					op_flag = set_multi;
				}break;				
				
			}
		}break;
		case history:
		{
//			if(dirflag < foldernum)
//			{
//				dirflag++;
//			}
//			DISP_HIS_FOLDER();
//			hispage(hpage);
		}break;
	}
}

/*按键功能函数RIGHT*/
void RIGHT_HANDLE(void)
{
	switch(page_flag)
	{
		case display:
		{
			switch(op_flag)
			{
				case home_type:
				{
					focus_off1();
					if(TCTYPE == TCK)
					{
						LCD_DisplayStringLine(47,100,"TC-K");
					}else if(TCTYPE == TCT){
						LCD_DisplayStringLine(47,100,"TC-T");
					}else if(TCTYPE == TCJ){
						LCD_DisplayStringLine(47,100,"TC-J");
					}else if(TCTYPE == TCN){
						LCD_DisplayStringLine(47,100,"TC-N");
					}else if(TCTYPE == TCE){
						LCD_DisplayStringLine(47,100,"TC-E");
					}else if(TCTYPE == TCS){
						LCD_DisplayStringLine(47,100,"TC-S");
					}else if(TCTYPE == TCR){
						LCD_DisplayStringLine(47,100,"TC-R");
					}else if(TCTYPE == TCB){
						LCD_DisplayStringLine(47,100,"TC-B");
					}else if(TCTYPE == PT100){
						LCD_DisplayStringLine(47,100,"PT100");
					}else if(TCTYPE == V1){
						LCD_DisplayStringLine(47,100,"1~5V");
					}
					focus_on1();
					if(FONT == big){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(47,290,"大");
						}else{
							LCD_DisplayStringLine(47,290,"L");
						}
					}else if(FONT == middle){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(47,290,"中");
						}else{
							LCD_DisplayStringLine(47,290,"M");
						}
					}else if(FONT == small){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(47,290,"小");
						}else{
							LCD_DisplayStringLine(47,290,"S");
						}
					}
					op_flag = set_font;
				}break;
				case home_ch1:
				{
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_off(CH1_SW);
							LCD_DisplayStringLine_48(100,10,"001");
						}else if(ch_page == page2){
							focus_off(CH9_SW);
							LCD_DisplayStringLine_48(100,10,"009");
						}else if(ch_page == page3){
							focus_off(CH17_SW);
							LCD_DisplayStringLine_48(100,10,"017");
						}else if(ch_page == page4){
							focus_off(CH25_SW);
							LCD_DisplayStringLine_48(100,10,"025");
						}else if(ch_page == page5){
							focus_off(CH33_SW);
							LCD_DisplayStringLine_48(100,10,"033");
						}
						
						if(ch_page == page1)
						{
							focus_on(CH5_SW);
							LCD_DisplayStringLine_48(100,330,"005");
						}else if(ch_page == page2){
							focus_on(CH13_SW);
							LCD_DisplayStringLine_48(100,330,"013");
						}else if(ch_page == page3){
							focus_on(CH21_SW);
							LCD_DisplayStringLine_48(100,330,"021");
						}else if(ch_page == page4){
							focus_on(CH29_SW);
							LCD_DisplayStringLine_48(100,330,"029");
						}else if(ch_page == page5){
							focus_on(CH37_SW);
							LCD_DisplayStringLine_48(100,330,"037");
						}
						
						op_flag = home_ch5;
					}else if(FONT == middle && CHNUM > 8){
						if(ch_page == page1)
						{
							focus_off(CH1_SW);
							DISP_CNL_M(100,10,"001");
							focus_on(CH9_SW);
							DISP_CNL_M(100,210,"009");
						}else if(ch_page == page2){
							focus_off(CH25_SW);
							DISP_CNL_M(100,10,"025");
							focus_on(CH33_SW);
							DISP_CNL_M(100,210,"033");
						}
						op_flag = home_ch9;
					}else if(FONT == small && CHNUM > 8){
						if(ch_page == page1)
						{
							focus_off(CH1_SW);
							DISP_CNL_S(100,10,"001");
							focus_on(CH9_SW);
							DISP_CNL_S(100,134,"009");
						}
						op_flag = home_ch9;
					}
					
				}break;
				case home_ch2:
				{
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_off(CH2_SW);
							LCD_DisplayStringLine_48(170+10,10,"002");
						}else if(ch_page == page2){
							focus_off(CH10_SW);
							LCD_DisplayStringLine_48(170+10,10,"010");
						}else if(ch_page == page3){
							focus_off(CH18_SW);
							LCD_DisplayStringLine_48(170+10,10,"018");
						}else if(ch_page == page4){
							focus_off(CH26_SW);
							LCD_DisplayStringLine_48(170+10,10,"026");
						}else if(ch_page == page5){
							focus_off(CH34_SW);
							LCD_DisplayStringLine_48(170+10,10,"034");
						}
						
						if(ch_page == page1)
						{
							focus_on(CH6_SW);
							LCD_DisplayStringLine_48(170+10,330,"006");
						}else if(ch_page == page2){
							focus_on(CH14_SW);
							LCD_DisplayStringLine_48(170+10,330,"014");
						}else if(ch_page == page3){
							focus_on(CH22_SW);
							LCD_DisplayStringLine_48(170+10,330,"022");
						}else if(ch_page == page4){
							focus_on(CH30_SW);
							LCD_DisplayStringLine_48(170+10,330,"030");
						}else if(ch_page == page5){
							focus_on(CH38_SW);
							LCD_DisplayStringLine_48(170+10,330,"038");
						}
						
						op_flag = home_ch6;
					}else if(FONT == middle && CHNUM > 8){
						if(ch_page == page1)
						{
							focus_off(CH2_SW);
							DISP_CNL_M(140,10,"002");
							focus_on(CH10_SW);
							DISP_CNL_M(140,210,"010");
						}else if(ch_page == page2){
							focus_off(CH26_SW);
							DISP_CNL_M(140,10,"026");
							focus_on(CH34_SW);
							DISP_CNL_M(140,210,"034");
						}
						op_flag = home_ch10;
					}else if(FONT == small && CHNUM > 8){
						if(ch_page == page1)
						{
							focus_off(CH2_SW);
							DISP_CNL_S(140,10,"002");
							focus_on(CH10_SW);
							DISP_CNL_S(140,134,"010");
						}
						op_flag = home_ch10;
					}
					
				}break;
				case home_ch3:
				{
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_off(CH3_SW);
							LCD_DisplayStringLine_48(240+20,10,"003");
						}else if(ch_page == page2){
							focus_off(CH11_SW);
							LCD_DisplayStringLine_48(240+20,10,"011");
						}else if(ch_page == page3){
							focus_off(CH19_SW);
							LCD_DisplayStringLine_48(240+20,10,"019");
						}else if(ch_page == page4){
							focus_off(CH27_SW);
							LCD_DisplayStringLine_48(240+20,10,"027");
						}else if(ch_page == page5){
							focus_off(CH35_SW);
							LCD_DisplayStringLine_48(240+20,10,"035");
						}
						
						if(ch_page == page1)
						{
							focus_on(CH7_SW);
							LCD_DisplayStringLine_48(240+20,330,"007");
						}else if(ch_page == page2){
							focus_on(CH15_SW);
							LCD_DisplayStringLine_48(240+20,330,"015");
						}else if(ch_page == page3){
							focus_on(CH23_SW);
							LCD_DisplayStringLine_48(240+20,330,"023");
						}else if(ch_page == page4){
							focus_on(CH31_SW);
							LCD_DisplayStringLine_48(240+20,330,"031");
						}else if(ch_page == page5){
							focus_on(CH39_SW);
							LCD_DisplayStringLine_48(240+20,330,"039");
						}
						
						op_flag = home_ch7;
					}else if(FONT == middle && CHNUM > 8){
						if(ch_page == page1)
						{
							focus_off(CH3_SW);
							DISP_CNL_M(180,10,"003");
							focus_on(CH11_SW);
							DISP_CNL_M(180,210,"011");
						}else if(ch_page == page2){
							focus_off(CH27_SW);
							DISP_CNL_M(180,10,"027");
							focus_on(CH35_SW);
							DISP_CNL_M(180,210,"035");
						}
						op_flag = home_ch11;
					}else if(FONT == small && CHNUM > 8){
						if(ch_page == page1)
						{
							focus_off(CH3_SW);
							DISP_CNL_S(180,10,"003");
							focus_on(CH11_SW);
							DISP_CNL_S(180,134,"011");
						}
						op_flag = home_ch11;
					}
					
				}break;
				case home_ch4:
				{
					if(FONT == big)
					{
						if(ch_page == page1)
						{
							focus_off(CH4_SW);
							LCD_DisplayStringLine_48(310+30,10,"004");
						}else if(ch_page == page2){
							focus_off(CH12_SW);
							LCD_DisplayStringLine_48(310+30,10,"012");
						}else if(ch_page == page3){
							focus_off(CH20_SW);
							LCD_DisplayStringLine_48(310+30,10,"020");
						}else if(ch_page == page4){
							focus_off(CH28_SW);
							LCD_DisplayStringLine_48(310+30,10,"028");
						}else if(ch_page == page5){
							focus_off(CH36_SW);
							LCD_DisplayStringLine_48(310+30,10,"036");
						}
						
						if(ch_page == page1)
						{
							focus_on(CH8_SW);
							LCD_DisplayStringLine_48(310+30,330,"008");
						}else if(ch_page == page2){
							focus_on(CH16_SW);
							LCD_DisplayStringLine_48(310+30,330,"016");
						}else if(ch_page == page3){
							focus_on(CH24_SW);
							LCD_DisplayStringLine_48(310+30,330,"024");
						}else if(ch_page == page4){
							focus_on(CH32_SW);
							LCD_DisplayStringLine_48(310+30,330,"032");
						}else if(ch_page == page5){
							focus_on(CH40_SW);
							LCD_DisplayStringLine_48(310+30,330,"040");
						}
						
						op_flag = home_ch8;
					}else if(FONT == middle && CHNUM > 8){
						if(ch_page == page1)
						{
							focus_off(CH4_SW);
							DISP_CNL_M(220,10,"004");
							focus_on(CH12_SW);
							DISP_CNL_M(220,210,"012");
						}else if(ch_page == page2){
							focus_off(CH28_SW);
							DISP_CNL_M(220,10,"028");
							focus_on(CH36_SW);
							DISP_CNL_M(220,210,"036");
						}
						op_flag = home_ch12;
						
					}else if(FONT == small && CHNUM > 8){
						if(ch_page == page1)
						{
							focus_off(CH4_SW);
							DISP_CNL_S(220,10,"004");
							focus_on(CH12_SW);
							DISP_CNL_S(220,134,"012");
						}
						op_flag = home_ch12;
					}
					
				}break;
				case home_ch5:
				{
					if(FONT == middle && CHNUM > 8){
						if(ch_page == page1)
						{
							focus_off(CH5_SW);
							DISP_CNL_M(260,10,"005");
							focus_on(CH13_SW);
							DISP_CNL_M(260,210,"013");
						}else if(ch_page == page2){
							focus_off(CH29_SW);
							DISP_CNL_M(260,10,"029");
							focus_on(CH37_SW);
							DISP_CNL_M(260,210,"037");
						}
						op_flag = home_ch13;
					}else if(FONT == small && CHNUM > 8){
						if(ch_page == page1)
						{
							focus_off(CH5_SW);
							DISP_CNL_S(260,10,"005");
							focus_on(CH13_SW);
							DISP_CNL_S(260,134,"013");
						}
						op_flag = home_ch13;
					}
				}break;
				case home_ch6:
				{
					if(FONT == middle && CHNUM > 8){
						if(ch_page == page1)
						{
							focus_off(CH6_SW);
							DISP_CNL_M(300,10,"006");
							focus_on(CH14_SW);
							DISP_CNL_M(300,210,"014");
						}else if(ch_page == page2){
							focus_off(CH30_SW);
							DISP_CNL_M(300,10,"030");
							focus_on(CH38_SW);
							DISP_CNL_M(300,210,"038");
						}
						op_flag = home_ch14;
					}else if(FONT == small && CHNUM > 8){
						if(ch_page == page1)
						{
							focus_off(CH6_SW);
							DISP_CNL_S(300,10,"006");
							focus_on(CH14_SW);
							DISP_CNL_S(300,134,"014");
						}
						op_flag = home_ch14;
					}
				}break;
				case home_ch7:
				{
					if(FONT == middle && CHNUM > 8){
						if(ch_page == page1)
						{
							focus_off(CH7_SW);
							DISP_CNL_M(340,10,"007");
							focus_on(CH15_SW);
							DISP_CNL_M(340,210,"015");
						}else if(ch_page == page2){
							focus_off(CH31_SW);
							DISP_CNL_M(340,10,"031");
							focus_on(CH39_SW);
							DISP_CNL_M(340,210,"039");
						}
						op_flag = home_ch15;
					}else if(FONT == small && CHNUM > 8){
						if(ch_page == page1)
						{
							focus_off(CH7_SW);
							DISP_CNL_S(340,10,"007");
							focus_on(CH15_SW);
							DISP_CNL_S(340,134,"015");
						}
						op_flag = home_ch15;
					}
				}break;
				case home_ch8:
				{
					if(FONT == middle && CHNUM > 8){
						if(ch_page == page1)
						{
							focus_off(CH8_SW);
							DISP_CNL_M(380,10,"008");
							focus_on(CH16_SW);
							DISP_CNL_M(380,210,"016");
						}else if(ch_page == page2){
							focus_off(CH32_SW);
							DISP_CNL_M(380,10,"032");
							focus_on(CH40_SW);
							DISP_CNL_M(380,210,"040");
						}
						op_flag = home_ch16;
					}else if(FONT == small && CHNUM > 8){
						if(ch_page == page1)
						{
							focus_off(CH8_SW);
							DISP_CNL_S(380,10,"008");
							focus_on(CH16_SW);
							DISP_CNL_S(380,134,"016");
						}
						op_flag = home_ch16;
					}
				}break;
				case home_ch9:
				{
					if(CHNUM > 16){
						if(FONT == middle){
							if(ch_page == page1)
							{
								focus_off(CH9_SW);
								DISP_CNL_M(100,210,"009");
								focus_on(CH17_SW);
								DISP_CNL_M(100,410,"017");
								op_flag = home_ch17;
							}						
						}else if(FONT == small){
							if(ch_page == page1)
							{
								focus_off(CH9_SW);
								DISP_CNL_S(100,134,"009");
								focus_on(CH17_SW);
								DISP_CNL_S(100,258,"017");
							}
							op_flag = home_ch17;
						}
					}
				}break;
				case home_ch10:
				{
					if(CHNUM > 16){
						if(FONT == middle){
							if(ch_page == page1)
							{
								focus_off(CH10_SW);
								DISP_CNL_M(140,210,"010");
								focus_on(CH18_SW);
								DISP_CNL_M(140,410,"018");
								op_flag = home_ch18;
							}
							
						}else if(FONT == small){
							if(ch_page == page1)
							{
								focus_off(CH10_SW);
								DISP_CNL_S(140,134,"010");
								focus_on(CH18_SW);
								DISP_CNL_S(140,258,"018");
							}
							op_flag = home_ch18;
						}
					}
				}break;
				case home_ch11:
				{
					if(CHNUM > 16){
						if(FONT == middle){
							if(ch_page == page1)
							{
								focus_off(CH11_SW);
								DISP_CNL_M(180,210,"011");
								focus_on(CH19_SW);
								DISP_CNL_M(180,410,"019");
								op_flag = home_ch19;
							}
							
						}else if(FONT == small){
							if(ch_page == page1)
							{
								focus_off(CH11_SW);
								DISP_CNL_S(180,134,"011");
								focus_on(CH19_SW);
								DISP_CNL_S(180,258,"019");
							}
							op_flag = home_ch19;
						}
					}
				}break;
				case home_ch12:
				{
					if(CHNUM > 16){
						if(FONT == middle){
							if(ch_page == page1)
							{
								focus_off(CH12_SW);
								DISP_CNL_M(220,210,"012");
								focus_on(CH20_SW);
								DISP_CNL_M(220,410,"020");
								op_flag = home_ch20;
							}
							
						}else if(FONT == small){
							if(ch_page == page1)
							{
								focus_off(CH12_SW);
								DISP_CNL_S(220,134,"012");
								focus_on(CH20_SW);
								DISP_CNL_S(220,258,"020");
							}
							op_flag = home_ch20;
						}
					}
				}break;
				case home_ch13:
				{
					if(CHNUM > 16){
						if(FONT == middle){
							if(ch_page == page1)
							{
								focus_off(CH13_SW);
								DISP_CNL_M(260,210,"013");
								focus_on(CH21_SW);
								DISP_CNL_M(260,410,"021");
								op_flag = home_ch21;
							}
							
						}else if(FONT == small){
							if(ch_page == page1)
							{
								focus_off(CH13_SW);
								DISP_CNL_S(260,134,"013");
								focus_on(CH21_SW);
								DISP_CNL_S(260,258,"021");
							}
							op_flag = home_ch21;
						}
					}
				}break;
				case home_ch14:
				{
					if(CHNUM > 16){
						if(FONT == middle){
							if(ch_page == page1)
							{
								focus_off(CH14_SW);
								DISP_CNL_M(300,210,"014");
								focus_on(CH22_SW);
								DISP_CNL_M(300,410,"022");
								op_flag = home_ch22;
							}
							
						}else if(FONT == small){
							if(ch_page == page1)
							{
								focus_off(CH14_SW);
								DISP_CNL_S(300,134,"014");
								focus_on(CH22_SW);
								DISP_CNL_S(300,258,"022");
							}
							op_flag = home_ch22;
						}
					}
				}break;
				case home_ch15:
				{
					if(CHNUM > 16){
						if(FONT == middle){
							if(ch_page == page1)
							{
								focus_off(CH15_SW);
								DISP_CNL_M(340,210,"015");
								focus_on(CH23_SW);
								DISP_CNL_M(340,410,"023");
								op_flag = home_ch23;
							}
							
						}else if(FONT == small){
							if(ch_page == page1)
							{
								focus_off(CH15_SW);
								DISP_CNL_S(340,134,"015");
								focus_on(CH23_SW);
								DISP_CNL_S(340,258,"023");
							}
							op_flag = home_ch23;
						}
					}
				}break;
				case home_ch16:
				{
					if(CHNUM > 16){
						if(FONT == middle){
							if(ch_page == page1)
							{
								focus_off(CH16_SW);
								DISP_CNL_M(380,210,"016");
								focus_on(CH24_SW);
								DISP_CNL_M(380,410,"024");
								op_flag = home_ch24;
							}
							
						}else if(FONT == small){
							if(ch_page == page1)
							{
								focus_off(CH16_SW);
								DISP_CNL_S(380,134,"016");
								focus_on(CH24_SW);
								DISP_CNL_S(380,258,"024");
							}
							op_flag = home_ch24;
						}
					}
				}break;
				case home_ch17:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH17_SW);
							DISP_CNL_S(100,258,"017");
							focus_on(CH25_SW);
							DISP_CNL_S(100,382,"025");
						}
						op_flag = home_ch25;
					}
				}break;
				case home_ch18:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH18_SW);
							DISP_CNL_S(140,258,"018");
							focus_on(CH26_SW);
							DISP_CNL_S(140,382,"026");
						}
						op_flag = home_ch26;
					}
				}break;
				case home_ch19:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH19_SW);
							DISP_CNL_S(180,258,"019");
							focus_on(CH27_SW);
							DISP_CNL_S(180,382,"027");
						}
						op_flag = home_ch27;
					}
				}break;
				case home_ch20:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH20_SW);
							DISP_CNL_S(220,258,"020");
							focus_on(CH28_SW);
							DISP_CNL_S(220,382,"028");
						}
						op_flag = home_ch28;
					}
				}break;
				case home_ch21:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH21_SW);
							DISP_CNL_S(260,258,"021");
							focus_on(CH29_SW);
							DISP_CNL_S(260,382,"029");
						}
						op_flag = home_ch29;
					}
				}break;
				case home_ch22:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH22_SW);
							DISP_CNL_S(300,258,"022");
							focus_on(CH30_SW);
							DISP_CNL_S(300,382,"030");
						}
						op_flag = home_ch30;
					}
				}break;
				case home_ch23:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH23_SW);
							DISP_CNL_S(340,258,"023");
							focus_on(CH31_SW);
							DISP_CNL_S(340,382,"031");
						}
						op_flag = home_ch31;
					}
				}break;
				case home_ch24:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH24_SW);
							DISP_CNL_S(380,258,"024");
							focus_on(CH32_SW);
							DISP_CNL_S(380,382,"032");
						}
						op_flag = home_ch32;
					}
				}break;
				case home_ch25:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH25_SW);
							DISP_CNL_S(100,382,"025");
							focus_on(CH33_SW);
							DISP_CNL_S(100,506,"033");
						}
						op_flag = home_ch33;
					}
				}break;
				case home_ch26:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH26_SW);
							DISP_CNL_S(140,382,"026");
							focus_on(CH34_SW);
							DISP_CNL_S(140,506,"034");
						}
						op_flag = home_ch34;
					}
				}break;
				case home_ch27:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH27_SW);
							DISP_CNL_S(180,382,"027");
							focus_on(CH35_SW);
							DISP_CNL_S(180,506,"035");
						}
						op_flag = home_ch35;
					}
				}break;
				case home_ch28:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH28_SW);
							DISP_CNL_S(220,382,"028");
							focus_on(CH36_SW);
							DISP_CNL_S(220,506,"036");
						}
						op_flag = home_ch36;
					}
				}break;
				case home_ch29:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH29_SW);
							DISP_CNL_S(260,382,"029");
							focus_on(CH37_SW);
							DISP_CNL_S(260,506,"037");
						}
						op_flag = home_ch37;
					}
				}break;
				case home_ch30:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH30_SW);
							DISP_CNL_S(300,382,"030");
							focus_on(CH38_SW);
							DISP_CNL_S(300,506,"038");
						}
						op_flag = home_ch38;
					}
				}break;
				case home_ch31:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH31_SW);
							DISP_CNL_S(340,382,"031");
							focus_on(CH39_SW);
							DISP_CNL_S(340,506,"039");
						}
						op_flag = home_ch39;
					}
				}break;
				case home_ch32:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH32_SW);
							DISP_CNL_S(380,382,"032");
							focus_on(CH40_SW);
							DISP_CNL_S(380,506,"040");
						}
						op_flag = home_ch40;
					}
				}break;
			}
		}break;
		case settings:
		{
			switch(op_flag)
			{
				
				case set_filter:
				{
					focus_off1();
					if(FILTER == ft_off){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,150,"关闭");
						}else{
							LCD_DisplayStringLine(50,150,"OFF");
						}
					}else{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,150,"打开");
						}else{
							LCD_DisplayStringLine(50,150,"ON");
						}
					}
					
					focus_on1();
					if(SPEED == fast)
					{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,500,"快速");
						}else{
							LCD_DisplayStringLine(50,500,"FAST");
						}
					}else if(SPEED == moderate){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,500,"中速");
						}else{
							LCD_DisplayStringLine(50,500,"MED");
						}
					}else if(SPEED == slow){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,500,"慢速");
						}else{
							LCD_DisplayStringLine(50,500,"SLOW");
						}
					}
					if(LANG == chs)
					{
						DrawInstruction("采样速度设置");
					}else{
						DrawInstruction("Setup sampling rate");
					}
					op_flag = set_spd;
				}break;				
				case set_beep:
				{
					focus_off1();
					if(BEEP == beep_off){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(90,150,"关闭");
						}else{
							LCD_DisplayStringLine(90,150,"OFF");
						}
					}else{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(90,150,"打开");
						}else{
							LCD_DisplayStringLine(90,150,"ON");
						}
					}
					
					focus_on1();
					if(BAUD == b9600)
					{
						LCD_DisplayStringLine(87,500,"9600");
					}else if(BAUD == b19200){
						LCD_DisplayStringLine(87,500,"19200");
					}else if(BAUD == b38400){
						LCD_DisplayStringLine(87,500,"38400");
					}else if(BAUD == b57600){
						LCD_DisplayStringLine(87,500,"57600");
					}else if(BAUD == b115200){
						LCD_DisplayStringLine(87,500,"115200");
					}
					if(LANG == chs)
					{
						DrawInstruction("波特率设置");
					}else{
						DrawInstruction("Select Baud rate");
					}
					op_flag = set_baud;					
				}break;
				case set_unit:
				{
					
					focus_on1();
					if(REPEAT == rtp_off){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(130,500,"关闭");
						}else{
							LCD_DisplayStringLine(130,500,"OFF");
						}
					}else{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(130,500,"打开");
						}else{
							LCD_DisplayStringLine(130,500,"ON");
						}
					}
					
					LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
					LCD_DrawFullRect(150,127,24,31);
					if(UNIT == C){
						LCD_SetTextColor(LCD_COLOR_YELLOW);
						DISP_CNL_S(127,150,"o");
						LCD_DisplayStringLine(127,155,"C");
					}else if(UNIT == F){
						LCD_SetTextColor(LCD_COLOR_YELLOW);
						DISP_CNL_S(127,150,"o");
						LCD_DisplayStringLine(127,155,"F");
					}else{
						LCD_SetTextColor(LCD_COLOR_YELLOW);
						LCD_DisplayStringLine(127,155,"K");
					}
					if(LANG == chs)
					{
						DrawInstruction("重复采集");
					}else{
						DrawInstruction("REPEAT");
					}
					op_flag = repeat;	
				}break;
//				case set_file:
//				{
//					focus_off1();
//					LCD_DisplayStringLine(317,150,"AUTO");
//					
//					focus_on1();
//					LCD_DisplayStringLine(317,500,"1");
////					LCD_DisplayStringLine(322,516,"S");
//					if(LANG == chs)
//					{
//						DrawInstruction("记录间隔时间");
//					}else{
//						DrawInstruction("Record interval");
//					}
//					op_flag = set_timer;				
//				}break;
			}
		}break;
		case separation:
		{
			switch(op_flag)
			{
				case set_min1:
				{
					if(spt_page == page1)
					{
						minfocus_off(1,CH1MIN);
						maxfocus_on(1,CH1MAX);
					}else if(spt_page == page2){
						minfocus_off(1,CH9MIN);
						maxfocus_on(1,CH9MAX);
					}else if(spt_page == page3){
						minfocus_off(1,CH17MIN);
						maxfocus_on(1,CH17MAX);
					}else if(spt_page == page4){
						minfocus_off(1,CH25MIN);
						maxfocus_on(1,CH25MAX);
					}else if(spt_page == page5){
						minfocus_off(1,CH33MIN);
						maxfocus_on(1,CH33MAX);
					}
					op_flag = set_max1;
				}break;
				case set_min2:
				{
					if(spt_page == page1)
					{
						minfocus_off(2,CH2MIN);
						maxfocus_on(2,CH2MAX);
					}else if(spt_page == page2){
						minfocus_off(2,CH10MIN);
						maxfocus_on(2,CH10MAX);
					}else if(spt_page == page3){
						minfocus_off(2,CH18MIN);
						maxfocus_on(2,CH18MAX);
					}else if(spt_page == page4){
						minfocus_off(2,CH26MIN);
						maxfocus_on(2,CH26MAX);
					}else if(spt_page == page5){
						minfocus_off(2,CH34MIN);
						maxfocus_on(2,CH34MAX);
					}
					op_flag = set_max2;
				}break;
				case set_min3:
				{
					if(spt_page == page1)
					{
						minfocus_off(3,CH3MIN);
						maxfocus_on(3,CH3MAX);
					}else if(spt_page == page2){
						minfocus_off(3,CH11MIN);
						maxfocus_on(3,CH11MAX);
					}else if(spt_page == page3){
						minfocus_off(3,CH19MIN);
						maxfocus_on(3,CH19MAX);
					}else if(spt_page == page4){
						minfocus_off(3,CH27MIN);
						maxfocus_on(3,CH27MAX);
					}else if(spt_page == page5){
						minfocus_off(3,CH35MIN);
						maxfocus_on(3,CH35MAX);
					}
					op_flag = set_max3;					
				}break;
				case set_min4:
				{
					if(spt_page == page1)
					{
						minfocus_off(4,CH4MIN);
						maxfocus_on(4,CH4MAX);
					}else if(spt_page == page2){
						minfocus_off(4,CH12MIN);
						maxfocus_on(4,CH12MAX);
					}else if(spt_page == page3){
						minfocus_off(4,CH20MIN);
						maxfocus_on(4,CH20MAX);
					}else if(spt_page == page4){
						minfocus_off(4,CH28MIN);
						maxfocus_on(4,CH28MAX);
					}else if(spt_page == page5){
						minfocus_off(4,CH36MIN);
						maxfocus_on(4,CH36MAX);
					}
					op_flag = set_max4;
				}break;
				case set_min5:
				{
					if(spt_page == page1)
					{
						minfocus_off(5,CH5MIN);
						maxfocus_on(5,CH5MAX);
					}else if(spt_page == page2){
						minfocus_off(5,CH13MIN);
						maxfocus_on(5,CH13MAX);
					}else if(spt_page == page3){
						minfocus_off(5,CH21MIN);
						maxfocus_on(5,CH21MAX);
					}else if(spt_page == page4){
						minfocus_off(5,CH29MIN);
						maxfocus_on(5,CH29MAX);
					}else if(spt_page == page5){
						minfocus_off(5,CH37MIN);
						maxfocus_on(5,CH37MAX);
					}
					op_flag = set_max5;
				}break;
				case set_min6:
				{
					if(spt_page == page1)
					{
						minfocus_off(6,CH6MIN);
						maxfocus_on(6,CH6MAX);
					}else if(spt_page == page2){
						minfocus_off(6,CH14MIN);
						maxfocus_on(6,CH14MAX);
					}else if(spt_page == page3){
						minfocus_off(6,CH22MIN);
						maxfocus_on(6,CH22MAX);
					}else if(spt_page == page4){
						minfocus_off(6,CH30MIN);
						maxfocus_on(6,CH30MAX);
					}else if(spt_page == page5){
						minfocus_off(6,CH38MIN);
						maxfocus_on(6,CH38MAX);
					}
					op_flag = set_max6;
				}break;
				case set_min7:
				{
					if(spt_page == page1)
					{
						minfocus_off(7,CH7MIN);
						maxfocus_on(7,CH7MAX);
					}else if(spt_page == page2){
						minfocus_off(7,CH15MIN);
						maxfocus_on(7,CH15MAX);
					}else if(spt_page == page3){
						minfocus_off(7,CH23MIN);
						maxfocus_on(7,CH23MAX);
					}else if(spt_page == page4){
						minfocus_off(7,CH31MIN);
						maxfocus_on(7,CH31MAX);
					}else if(spt_page == page5){
						minfocus_off(7,CH39MIN);
						maxfocus_on(7,CH39MAX);
					}
					op_flag = set_max7;
				}break;
				case set_min8:
				{
					if(spt_page == page1)
					{
						minfocus_off(8,CH8MIN);
						maxfocus_on(8,CH8MAX);
					}else if(spt_page == page2){
						minfocus_off(8,CH16MIN);
						maxfocus_on(8,CH16MAX);
					}else if(spt_page == page3){
						minfocus_off(8,CH24MIN);
						maxfocus_on(8,CH24MAX);
					}else if(spt_page == page4){
						minfocus_off(8,CH32MIN);
						maxfocus_on(8,CH32MAX);
					}else if(spt_page == page5){
						minfocus_off(8,CH40MIN);
						maxfocus_on(8,CH40MAX);
					}
					op_flag = set_max8;
				}break;
			}
		}break;
		case sysset:
		{
			switch(op_flag)
			{
				case set_date:
				{
					if(LANG == chs)
					{
						DrawInstruction("时-分-秒");
					}else{
						DrawInstruction("HOUR-MINUTE-SECOND");
					}					
					op_flag = set_time;
				}break;
			}
		}break;
		case history:
		{
			if(hiscursor < 495)
			{
				hiscursor ++;
				Draw_His_Graph();
			}
		}break;
				
	}
}

/*按键功能函数LEFT*/
void LEFT_HANDLE(void)
{
	switch(page_flag)
	{
		case display:
		{
			switch(op_flag)
			{
				case set_font:
				{
					focus_on1();
					if(TCTYPE == TCK)
					{
						LCD_DisplayStringLine(47,100,"TC-K");
					}else if(TCTYPE == TCT){
						LCD_DisplayStringLine(47,100,"TC-T");
					}else if(TCTYPE == TCJ){
						LCD_DisplayStringLine(47,100,"TC-J");
					}else if(TCTYPE == TCN){
						LCD_DisplayStringLine(47,100,"TC-N");
					}else if(TCTYPE == TCE){
						LCD_DisplayStringLine(47,100,"TC-E");
					}else if(TCTYPE == TCS){
						LCD_DisplayStringLine(47,100,"TC-S");
					}else if(TCTYPE == TCR){
						LCD_DisplayStringLine(47,100,"TC-R");
					}else if(TCTYPE == TCB){
						LCD_DisplayStringLine(47,100,"TC-B");
					}else if(TCTYPE == PT100){
						LCD_DisplayStringLine(47,100,"PT100");
					}else if(TCTYPE == V1){
						LCD_DisplayStringLine(47,100,"1~5V");
					}
					focus_off1();
					if(FONT == big){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(47,290,"大");
						}else{
							LCD_DisplayStringLine(47,290,"L");
						}
					}else if(FONT == middle){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(47,290,"中");
						}else{
							LCD_DisplayStringLine(47,290,"M");
						}
					}else if(FONT == small){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(47,290,"小");
						}else{
							LCD_DisplayStringLine(47,290,"S");
						}
					}
					op_flag = home_type;
				}break;
				
				case home_ch5:
				{
					if(ch_page == page1 && FONT == big)
					{
						focus_off(CH5_SW);
						LCD_DisplayStringLine_48(100,330,"005");
					}else if(ch_page == page2){
						focus_off(CH13_SW);
						LCD_DisplayStringLine_48(100,330,"013");
					}else if(ch_page == page3){
						focus_off(CH21_SW);
						LCD_DisplayStringLine_48(100,330,"021");
					}else if(ch_page == page4){
						focus_off(CH29_SW);
						LCD_DisplayStringLine_48(100,330,"029");
					}else if(ch_page == page5){
						focus_off(CH37_SW);
						LCD_DisplayStringLine_48(100,330,"037");
					}
					
					if(ch_page == page1 && FONT == big)
					{
						focus_on(CH1_SW);
						LCD_DisplayStringLine_48(100,10,"001");
					}else if(ch_page == page2){
						focus_on(CH9_SW);
						LCD_DisplayStringLine_48(100,10,"009");
					}else if(ch_page == page3){
						focus_on(CH17_SW);
						LCD_DisplayStringLine_48(100,10,"017");
					}else if(ch_page == page4){
						focus_on(CH25_SW);
						LCD_DisplayStringLine_48(100,10,"025");
					}else if(ch_page == page5){
						focus_on(CH33_SW);
						LCD_DisplayStringLine_48(100,10,"033");
					}
					
					op_flag = home_ch1;
				}break;
				case home_ch6:
				{
					if(ch_page == page1 && FONT == big)
					{
						focus_off(CH6_SW);
						LCD_DisplayStringLine_48(170+10,330,"006");
					}else if(ch_page == page2){
						focus_off(CH14_SW);
						LCD_DisplayStringLine_48(170+10,330,"014");
					}else if(ch_page == page3){
						focus_off(CH22_SW);
						LCD_DisplayStringLine_48(170+10,330,"022");
					}else if(ch_page == page4){
						focus_off(CH30_SW);
						LCD_DisplayStringLine_48(170+10,330,"030");
					}else if(ch_page == page5){
						focus_off(CH38_SW);
						LCD_DisplayStringLine_48(170+10,330,"038");
					}
					
					if(ch_page == page1 && FONT == big)
					{
						focus_on(CH2_SW);
						LCD_DisplayStringLine_48(170+10,10,"002");
					}else if(ch_page == page2){
						focus_on(CH10_SW);
						LCD_DisplayStringLine_48(170+10,10,"010");
					}else if(ch_page == page3){
						focus_on(CH18_SW);
						LCD_DisplayStringLine_48(170+10,10,"018");
					}else if(ch_page == page4){
						focus_on(CH26_SW);
						LCD_DisplayStringLine_48(170+10,10,"026");
					}else if(ch_page == page5){
						focus_on(CH34_SW);
						LCD_DisplayStringLine_48(170+10,10,"034");
					}
					
					op_flag = home_ch2;
				}break;
				case home_ch7:
				{
					if(ch_page == page1 && FONT == big)
					{
						focus_off(CH7_SW);
						LCD_DisplayStringLine_48(240+20,330,"007");
					}else if(ch_page == page2){
						focus_off(CH15_SW);
						LCD_DisplayStringLine_48(240+20,330,"015");
					}else if(ch_page == page3){
						focus_off(CH23_SW);
						LCD_DisplayStringLine_48(240+20,330,"023");
					}else if(ch_page == page4){
						focus_off(CH31_SW);
						LCD_DisplayStringLine_48(240+20,330,"031");
					}else if(ch_page == page5){
						focus_off(CH39_SW);
						LCD_DisplayStringLine_48(240+20,330,"039");
					}
					
					if(ch_page == page1 && FONT == big)
					{
						focus_on(CH3_SW);
						LCD_DisplayStringLine_48(240+20,10,"003");
					}else if(ch_page == page2){
						focus_on(CH11_SW);
						LCD_DisplayStringLine_48(240+20,10,"011");
					}else if(ch_page == page3){
						focus_on(CH19_SW);
						LCD_DisplayStringLine_48(240+20,10,"019");
					}else if(ch_page == page4){
						focus_on(CH27_SW);
						LCD_DisplayStringLine_48(240+20,10,"027");
					}else if(ch_page == page5){
						focus_on(CH35_SW);
						LCD_DisplayStringLine_48(240+20,10,"035");
					}
					
					op_flag = home_ch3;
				}break;
				case home_ch8:
				{
					if(ch_page == page1 && FONT == big)
					{
						focus_off(CH8_SW);
						LCD_DisplayStringLine_48(310+30,330,"008");
					}else if(ch_page == page2){
						focus_off(CH16_SW);
						LCD_DisplayStringLine_48(310+30,330,"016");
					}else if(ch_page == page3){
						focus_off(CH24_SW);
						LCD_DisplayStringLine_48(310+30,330,"024");
					}else if(ch_page == page4){
						focus_off(CH32_SW);
						LCD_DisplayStringLine_48(310+30,330,"032");
					}else if(ch_page == page5){
						focus_off(CH40_SW);
						LCD_DisplayStringLine_48(310+30,330,"040");
					}
					
					if(ch_page == page1 && FONT == big)
					{
						focus_on(CH4_SW);
						LCD_DisplayStringLine_48(310+30,10,"004");
					}else if(ch_page == page2){
						focus_on(CH12_SW);
						LCD_DisplayStringLine_48(310+30,10,"012");
					}else if(ch_page == page3){
						focus_on(CH20_SW);
						LCD_DisplayStringLine_48(310+30,10,"020");
					}else if(ch_page == page4){
						focus_on(CH28_SW);
						LCD_DisplayStringLine_48(310+30,10,"028");
					}else if(ch_page == page5){
						focus_on(CH36_SW);
						LCD_DisplayStringLine_48(310+30,10,"036");
					}
					
					op_flag = home_ch4;
				}break;
				case home_ch9:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH9_SW);
							DISP_CNL_M(100,210,"009");
							focus_on(CH1_SW);
							DISP_CNL_M(100,10,"001");
						}else if(ch_page == page2){
							focus_off(CH33_SW);
							DISP_CNL_M(100,210,"033");
							focus_on(CH25_SW);
							DISP_CNL_M(100,10,"025");
						}
						op_flag = home_ch1;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH9_SW);
							DISP_CNL_S(100,134,"009");
							focus_on(CH1_SW);
							DISP_CNL_S(100,10,"001");
						}
						op_flag = home_ch1;
					}
				}break;
				case home_ch10:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH10_SW);
							DISP_CNL_M(140,210,"010");
							focus_on(CH2_SW);
							DISP_CNL_M(140,10,"002");
						}else if(ch_page == page2){
							focus_off(CH34_SW);
							DISP_CNL_M(140,210,"034");
							focus_on(CH26_SW);
							DISP_CNL_M(140,10,"026");
						}
						op_flag = home_ch2;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH10_SW);
							DISP_CNL_S(140,134,"010");
							focus_on(CH2_SW);
							DISP_CNL_S(140,10,"002");
						}
						op_flag = home_ch2;
					}
				}break;
				case home_ch11:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH11_SW);
							DISP_CNL_M(180,210,"011");
							focus_on(CH3_SW);
							DISP_CNL_M(180,10,"003");
						}else if(ch_page == page2){
							focus_off(CH35_SW);
							DISP_CNL_M(180,210,"035");
							focus_on(CH27_SW);
							DISP_CNL_M(180,10,"027");
						}
						op_flag = home_ch3;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH11_SW);
							DISP_CNL_S(180,134,"011");
							focus_on(CH3_SW);
							DISP_CNL_S(180,10,"003");
						}
						op_flag = home_ch3;
					}
				}break;
				case home_ch12:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH12_SW);
							DISP_CNL_M(220,210,"012");
							focus_on(CH4_SW);
							DISP_CNL_M(220,10,"004");
						}else if(ch_page == page2){
							focus_off(CH36_SW);
							DISP_CNL_M(220,210,"036");
							focus_on(CH28_SW);
							DISP_CNL_M(220,10,"028");
						}
						op_flag = home_ch4;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH12_SW);
							DISP_CNL_S(220,134,"012");
							focus_on(CH4_SW);
							DISP_CNL_S(220,10,"004");
						}
						op_flag = home_ch4;
					}
				}break;
				case home_ch13:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH13_SW);
							DISP_CNL_M(260,210,"013");
							focus_on(CH5_SW);
							DISP_CNL_M(260,10,"005");
						}else if(ch_page == page2){
							focus_off(CH37_SW);
							DISP_CNL_M(260,210,"037");
							focus_on(CH29_SW);
							DISP_CNL_M(260,10,"029");
						}
						op_flag = home_ch5;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH13_SW);
							DISP_CNL_S(260,134,"013");
							focus_on(CH5_SW);
							DISP_CNL_S(260,10,"005");
						}
						op_flag = home_ch5;
					}
				}break;
				case home_ch14:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH14_SW);
							DISP_CNL_M(300,210,"014");
							focus_on(CH6_SW);
							DISP_CNL_M(300,10,"006");
						}else if(ch_page == page2){
							focus_off(CH38_SW);
							DISP_CNL_M(300,210,"038");
							focus_on(CH30_SW);
							DISP_CNL_M(300,10,"030");
						}
						op_flag = home_ch6;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH14_SW);
							DISP_CNL_S(300,134,"014");
							focus_on(CH6_SW);
							DISP_CNL_S(300,10,"006");
						}
						op_flag = home_ch6;
					}
				}break;
				case home_ch15:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH15_SW);
							DISP_CNL_M(340,210,"015");
							focus_on(CH7_SW);
							DISP_CNL_M(340,10,"007");
						}else if(ch_page == page2){
							focus_off(CH39_SW);
							DISP_CNL_M(340,210,"039");
							focus_on(CH31_SW);
							DISP_CNL_M(340,10,"031");
						}
						op_flag = home_ch7;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH15_SW);
							DISP_CNL_S(340,134,"015");
							focus_on(CH7_SW);
							DISP_CNL_S(340,10,"007");
						}
						op_flag = home_ch7;
					}
				}break;
				case home_ch16:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH16_SW);
							DISP_CNL_M(380,210,"016");
							focus_on(CH8_SW);
							DISP_CNL_M(380,10,"008");
						}else if(ch_page == page2){
							focus_off(CH40_SW);
							DISP_CNL_M(380,210,"040");
							focus_on(CH32_SW);
							DISP_CNL_M(380,10,"032");
						}
						op_flag = home_ch8;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH16_SW);
							DISP_CNL_S(380,134,"016");
							focus_on(CH8_SW);
							DISP_CNL_S(380,10,"008");
						}
						op_flag = home_ch8;
					}
				}break;
				case home_ch17:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH17_SW);
							DISP_CNL_M(100,410,"017");
							focus_on(CH9_SW);
							DISP_CNL_M(100,210,"009");
						}
						op_flag = home_ch9;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH17_SW);
							DISP_CNL_S(100,258,"017");
							focus_on(CH9_SW);
							DISP_CNL_S(100,134,"009");
						}
						op_flag = home_ch9;
					}
				}break;
				case home_ch18:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH18_SW);
							DISP_CNL_M(140,410,"018");
							focus_on(CH10_SW);
							DISP_CNL_M(140,210,"010");
						}
						op_flag = home_ch10;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH18_SW);
							DISP_CNL_S(140,258,"018");
							focus_on(CH10_SW);
							DISP_CNL_S(140,134,"010");
						}
						op_flag = home_ch10;
					}
				}break;
				case home_ch19:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH19_SW);
							DISP_CNL_M(180,410,"019");
							focus_on(CH11_SW);
							DISP_CNL_M(180,210,"011");
						}
						op_flag = home_ch11;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH19_SW);
							DISP_CNL_S(180,258,"019");
							focus_on(CH11_SW);
							DISP_CNL_S(180,134,"011");
						}
						op_flag = home_ch11;
					}
				}break;
				case home_ch20:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH20_SW);
							DISP_CNL_M(220,410,"020");
							focus_on(CH12_SW);
							DISP_CNL_M(220,210,"012");
						}
						op_flag = home_ch12;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH20_SW);
							DISP_CNL_S(220,258,"020");
							focus_on(CH12_SW);
							DISP_CNL_S(220,134,"012");
						}
						op_flag = home_ch12;
					}
				}break;
				case home_ch21:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH21_SW);
							DISP_CNL_M(260,410,"021");
							focus_on(CH13_SW);
							DISP_CNL_M(260,210,"013");
						}
						op_flag = home_ch13;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH21_SW);
							DISP_CNL_S(260,258,"021");
							focus_on(CH13_SW);
							DISP_CNL_S(260,134,"013");
						}
						op_flag = home_ch13;
					}
				}break;
				case home_ch22:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH22_SW);
							DISP_CNL_M(300,410,"022");
							focus_on(CH14_SW);
							DISP_CNL_M(300,210,"014");
						}
						op_flag = home_ch14;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH22_SW);
							DISP_CNL_S(300,258,"022");
							focus_on(CH14_SW);
							DISP_CNL_S(300,134,"014");
						}
						op_flag = home_ch14;
					}
				}break;
				case home_ch23:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH23_SW);
							DISP_CNL_M(340,410,"023");
							focus_on(CH15_SW);
							DISP_CNL_M(340,210,"015");
						}
						op_flag = home_ch15;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH23_SW);
							DISP_CNL_S(340,258,"023");
							focus_on(CH15_SW);
							DISP_CNL_S(340,134,"015");
						}
						op_flag = home_ch15;
					}
				}break;
				case home_ch24:
				{
					if(FONT == middle){
						if(ch_page == page1)
						{
							focus_off(CH24_SW);
							DISP_CNL_M(380,410,"024");
							focus_on(CH16_SW);
							DISP_CNL_M(380,210,"016");
						}
						op_flag = home_ch16;
					}else if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH24_SW);
							DISP_CNL_S(380,258,"024");
							focus_on(CH16_SW);
							DISP_CNL_S(380,134,"016");
						}
						op_flag = home_ch16;
					}
				}break;
				case home_ch25:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH25_SW);
							DISP_CNL_S(100,382,"025");
							focus_on(CH17_SW);
							DISP_CNL_S(100,258,"017");
						}
						op_flag = home_ch17;
					}
				}break;
				case home_ch26:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH26_SW);
							DISP_CNL_S(140,382,"026");
							focus_on(CH18_SW);
							DISP_CNL_S(140,258,"018");
						}
						op_flag = home_ch18;
					}
				}break;
				case home_ch27:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH27_SW);
							DISP_CNL_S(180,382,"027");
							focus_on(CH19_SW);
							DISP_CNL_S(180,258,"019");
						}
						op_flag = home_ch19;
					}
				}break;
				case home_ch28:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH28_SW);
							DISP_CNL_S(220,382,"028");
							focus_on(CH20_SW);
							DISP_CNL_S(220,258,"020");
						}
						op_flag = home_ch20;
					}
				}break;
				case home_ch29:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH29_SW);
							DISP_CNL_S(260,382,"029");
							focus_on(CH21_SW);
							DISP_CNL_S(260,258,"021");
						}
						op_flag = home_ch21;
					}
				}break;
				case home_ch30:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH30_SW);
							DISP_CNL_S(300,382,"030");
							focus_on(CH22_SW);
							DISP_CNL_S(300,258,"022");
						}
						op_flag = home_ch22;
					}
				}break;
				case home_ch31:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH31_SW);
							DISP_CNL_S(340,382,"031");
							focus_on(CH23_SW);
							DISP_CNL_S(340,258,"023");
						}
						op_flag = home_ch23;
					}
				}break;
				case home_ch32:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH32_SW);
							DISP_CNL_S(380,382,"032");
							focus_on(CH24_SW);
							DISP_CNL_S(380,258,"024");
						}
						op_flag = home_ch24;
					}
				}break;
				case home_ch33:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH33_SW);
							DISP_CNL_S(100,506,"033");
							focus_on(CH25_SW);
							DISP_CNL_S(100,382,"025");
						}
						op_flag = home_ch25;
					}
				}break;
				case home_ch34:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH34_SW);
							DISP_CNL_S(140,506,"034");
							focus_on(CH26_SW);
							DISP_CNL_S(140,382,"026");
						}
						op_flag = home_ch26;
					}
				}break;
				case home_ch35:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH35_SW);
							DISP_CNL_S(180,506,"035");
							focus_on(CH27_SW);
							DISP_CNL_S(180,382,"027");
						}
						op_flag = home_ch27;
					}
				}break;
				case home_ch36:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH36_SW);
							DISP_CNL_S(220,506,"036");
							focus_on(CH28_SW);
							DISP_CNL_S(220,382,"028");
						}
						op_flag = home_ch28;
					}
				}break;
				case home_ch37:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH37_SW);
							DISP_CNL_S(260,506,"037");
							focus_on(CH29_SW);
							DISP_CNL_S(260,382,"029");
						}
						op_flag = home_ch29;
					}
				}break;
				case home_ch38:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH38_SW);
							DISP_CNL_S(300,506,"038");
							focus_on(CH30_SW);
							DISP_CNL_S(300,382,"030");
						}
						op_flag = home_ch30;
					}
				}break;
				case home_ch39:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH39_SW);
							DISP_CNL_S(340,506,"039");
							focus_on(CH31_SW);
							DISP_CNL_S(340,382,"031");
						}
						op_flag = home_ch31;
					}
				}break;
				case home_ch40:
				{
					if(FONT == small){
						if(ch_page == page1)
						{
							focus_off(CH40_SW);
							DISP_CNL_S(380,506,"040");
							focus_on(CH32_SW);
							DISP_CNL_S(380,382,"032");
						}
						op_flag = home_ch32;
					}
				}break;
			}
		}break;
		case settings:
		{
			switch(op_flag)
			{
				case set_spd:
				{
					focus_on1();
					if(FILTER == ft_off){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,150,"关闭");
						}else{
							LCD_DisplayStringLine(50,150,"OFF");
						}
					}else{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,150,"打开");
						}else{
							LCD_DisplayStringLine(50,150,"ON");
						}
					}
					if(LANG == chs)
					{
						DrawInstruction("比较器设置");
					}else{
						DrawInstruction("Comparator ON/OFF");
					}
					
					focus_off1();
					if(SPEED == fast)
					{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,500,"快速");
						}else{
							LCD_DisplayStringLine(50,500,"FAST");
						}
					}else if(SPEED == moderate){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,500,"中速");
						}else{
							LCD_DisplayStringLine(50,500,"MED");
						}
					}else if(SPEED == slow){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,500,"慢速");
						}else{
							LCD_DisplayStringLine(50,500,"SLOW");
						}
					}
					op_flag = set_filter;
				}break;				
				case set_baud:
				{
					focus_on1();
					if(BEEP == beep_off){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(90,150,"关闭");
						}else{
							LCD_DisplayStringLine(90,150,"OFF");
						}
					}else{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(90,150,"打开");
						}else{
							LCD_DisplayStringLine(90,150,"ON");
						}
					}
					if(LANG == chs)
					{
						DrawInstruction("蜂鸣器设置");
					}else{
						DrawInstruction("Buzzer ON/OFF");
					}
					
					focus_off1();
					if(BAUD == b9600)
					{
						LCD_DisplayStringLine(87,500,"9600");
					}else if(BAUD == b19200){
						LCD_DisplayStringLine(87,500,"19200");
					}else if(BAUD == b38400){
						LCD_DisplayStringLine(87,500,"38400");
					}else if(BAUD == b57600){
						LCD_DisplayStringLine(87,500,"57600");
					}else if(BAUD == b115200){
						LCD_DisplayStringLine(87,500,"115200");
					}
					op_flag = set_beep;					
				}break;
				case repeat:
				{
					
					focus_off1();
					if(REPEAT == rtp_off){
						if(LANG == chs)
						{
							LCD_DisplayStringLine(130,500,"关闭");
						}else{
							LCD_DisplayStringLine(130,500,"OFF");
						}
					}else{
						if(LANG == chs)
						{
							LCD_DisplayStringLine(130,500,"打开");
						}else{
							LCD_DisplayStringLine(130,500,"ON");
						}
					}
					
					LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
					LCD_DrawFullRect(150,127,24,31);
					if(UNIT == C){
						LCD_SetTextColor(LCD_COLOR_BLACK);
						DISP_CNL_S(127,150,"o");
						LCD_DisplayStringLine(127,155,"C");
					}else if(UNIT == F){
						LCD_SetTextColor(LCD_COLOR_BLACK);
						DISP_CNL_S(127,150,"o");
						LCD_DisplayStringLine(127,155,"F");
					}else{
						LCD_SetTextColor(LCD_COLOR_BLACK);
						LCD_DisplayStringLine(127,155,"K");
					}
					if(LANG == chs)
					{
						DrawInstruction("温度单位选择");
					}else{
						DrawInstruction("Select temperature unit");
					}
					op_flag = set_unit;	
				}break;
//				case set_timer:
//				{
//					focus_on1();
//					LCD_DisplayStringLine(317,150,"AUTO");
//					if(LANG == chs)
//					{
//						DrawInstruction("输入文件名的前缀");
//					}else{
//						DrawInstruction("Input file name prefix");
//					}
//					
//					focus_off1();
//					LCD_DisplayStringLine(317,500,"1");
////					LCD_DisplayStringLine(322,516,"S");
//					op_flag = set_file;				
//				}break;
			}
		}break;
		case separation:
		{
			switch(op_flag)
			{
				case set_max1:
				{
					if(spt_page == page1)
					{
						minfocus_on(1,CH1MIN);
						maxfocus_off(1,CH1MAX);
					}else if(spt_page == page2){
						minfocus_on(1,CH9MIN);
						maxfocus_off(1,CH9MAX);
					}else if(spt_page == page3){
						minfocus_on(1,CH17MIN);
						maxfocus_off(1,CH17MAX);
					}else if(spt_page == page4){
						minfocus_on(1,CH25MIN);
						maxfocus_off(1,CH25MAX);
					}else if(spt_page == page5){
						minfocus_on(1,CH33MIN);
						maxfocus_off(1,CH33MAX);
					}
					op_flag = set_min1;
				}break;
				case set_max2:
				{
					if(spt_page == page1)
					{
						minfocus_on(2,CH2MIN);
						maxfocus_off(2,CH2MAX);
					}else if(spt_page == page2){
						minfocus_on(2,CH10MIN);
						maxfocus_off(2,CH10MAX);
					}else if(spt_page == page3){
						minfocus_on(2,CH18MIN);
						maxfocus_off(2,CH18MAX);
					}else if(spt_page == page4){
						minfocus_on(2,CH26MIN);
						maxfocus_off(2,CH26MAX);
					}else if(spt_page == page5){
						minfocus_on(2,CH34MIN);
						maxfocus_off(2,CH34MAX);
					}
					op_flag = set_min2;
				}break;
				case set_max3:
				{
					if(spt_page == page1)
					{
						minfocus_on(3,CH3MIN);
						maxfocus_off(3,CH3MAX);
					}else if(spt_page == page2){
						minfocus_on(3,CH11MIN);
						maxfocus_off(3,CH11MAX);
					}else if(spt_page == page3){
						minfocus_on(3,CH19MIN);
						maxfocus_off(3,CH19MAX);
					}else if(spt_page == page4){
						minfocus_on(3,CH27MIN);
						maxfocus_off(3,CH27MAX);
					}else if(spt_page == page5){
						minfocus_on(3,CH35MIN);
						maxfocus_off(3,CH35MAX);
					}
					op_flag = set_min3;					
				}break;
				case set_max4:
				{
					if(spt_page == page1)
					{
						minfocus_on(4,CH4MIN);
						maxfocus_off(4,CH4MAX);
					}else if(spt_page == page2){
						minfocus_on(4,CH12MIN);
						maxfocus_off(4,CH12MAX);
					}else if(spt_page == page3){
						minfocus_on(4,CH20MIN);
						maxfocus_off(4,CH20MAX);
					}else if(spt_page == page4){
						minfocus_on(4,CH28MIN);
						maxfocus_off(4,CH28MAX);
					}else if(spt_page == page5){
						minfocus_on(4,CH36MIN);
						maxfocus_off(4,CH36MAX);
					}
					op_flag = set_min4;
				}break;
				case set_max5:
				{
					if(spt_page == page1)
					{
						minfocus_on(5,CH5MIN);
						maxfocus_off(5,CH5MAX);
					}else if(spt_page == page2){
						minfocus_on(5,CH13MIN);
						maxfocus_off(5,CH13MAX);
					}else if(spt_page == page3){
						minfocus_on(5,CH21MIN);
						maxfocus_off(5,CH21MAX);
					}else if(spt_page == page4){
						minfocus_on(5,CH29MIN);
						maxfocus_off(5,CH29MAX);
					}else if(spt_page == page5){
						minfocus_on(5,CH37MIN);
						maxfocus_off(5,CH37MAX);
					}
					op_flag = set_min5;
				}break;
				case set_max6:
				{
					if(spt_page == page1)
					{
						minfocus_on(6,CH6MIN);
						maxfocus_off(6,CH6MAX);
					}else if(spt_page == page2){
						minfocus_on(6,CH14MIN);
						maxfocus_off(6,CH14MAX);
					}else if(spt_page == page3){
						minfocus_on(6,CH22MIN);
						maxfocus_off(6,CH22MAX);
					}else if(spt_page == page4){
						minfocus_on(6,CH30MIN);
						maxfocus_off(6,CH30MAX);
					}else if(spt_page == page5){
						minfocus_on(6,CH38MIN);
						maxfocus_off(6,CH38MAX);
					}
					op_flag = set_min6;
				}break;
				case set_max7:
				{
					if(spt_page == page1)
					{
						minfocus_on(7,CH7MIN);
						maxfocus_off(7,CH7MAX);
					}else if(spt_page == page2){
						minfocus_on(7,CH15MIN);
						maxfocus_off(7,CH15MAX);
					}else if(spt_page == page3){
						minfocus_on(7,CH23MIN);
						maxfocus_off(7,CH23MAX);
					}else if(spt_page == page4){
						minfocus_on(7,CH31MIN);
						maxfocus_off(7,CH31MAX);
					}else if(spt_page == page5){
						minfocus_on(7,CH39MIN);
						maxfocus_off(7,CH39MAX);
					}
					op_flag = set_min7;
				}break;
				case set_max8:
				{
					if(spt_page == page1)
					{
						minfocus_on(8,CH8MIN);
						maxfocus_off(8,CH8MAX);
					}else if(spt_page == page2){
						minfocus_on(8,CH16MIN);
						maxfocus_off(8,CH16MAX);
					}else if(spt_page == page3){
						minfocus_on(8,CH24MIN);
						maxfocus_off(8,CH24MAX);
					}else if(spt_page == page4){
						minfocus_on(8,CH32MIN);
						maxfocus_off(8,CH32MAX);
					}else if(spt_page == page5){
						minfocus_on(8,CH40MIN);
						maxfocus_off(8,CH40MAX);
					}
					op_flag = set_min8;
				}break;
			}
		}break;
		case sysset:
		{
			switch(op_flag)
			{
				case set_time:
				{
					if(LANG == chs)
					{
						DrawInstruction("年-月-日");
					}else{
						DrawInstruction("YEAR-MONTH-DAY");
					}
					op_flag = set_date;
				}break;
			}
		}break;
		case history:
		{
			if(hiscursor > 0)
			{
				hiscursor --;
				Draw_His_Graph();
			}
		}break;
	}
}

void KEY1_HANDLE(void)
{		
	u8 i;
	switch(page_flag)
	{
		case display:
		{
//			TouchCal();
//			DrawPowOff();
//			LCD_Clear(LCD_COLOR_BLACK);
//			page_flag = 0xff;
//			DrawLogo(200,250);
		}break;
		case separation:
		{
			input_num("1");			
		}break;
		case graphset:
		{
			input_num("1");			
		}break;
		case calibrate:
		{
			input_num("1");			
		}break;
		case history:
		{
			input_search("1");			
		}break;
		case touchcal:
		{
			XYCAL(1);
		}break;
		case settings:
		{
			if(op_flag == set_timer)
			{
				input_num("1");	
			}else if(op_flag == starttime){
				Input_start(1);
			}else if(op_flag == endtime){
				Input_end(1);
			}
		}break;
		case factory:
		{
			if(passverify == 0)
			{
				input_pass("1");	
			}else{
				input_num("1");
			}
		}break;
		default:
		{
//			for(i=0;i<8;i++)
//			{
//				Usart_SendByte(DEBUG_USART,tempreq[i]);//请求温度数据
//			}
		}break;
	}
}

void KEY2_HANDLE(void)
{		
	switch(page_flag)
	{
		case separation:
		{
			input_num("2");
		}break;
		case graphset:
		{
			input_num ("2");
		}break;
		case calibrate:
		{
			input_num("2");			
		}break;
		case history:
		{
			input_search("2");			
		}break;
		case display:
		{
//			BEEP_ON;
//			Delay(0x3ff);
//			BEEP_OFF;
//			 CH376FileClose(TRUE);
		}break;
		case settings:
		{
			if(op_flag == set_timer)
			{
				input_num("2");	
			}else if(op_flag == starttime){
				Input_start(2);
			}else if(op_flag == endtime){
				Input_end(2);
			}
		}break;
		case touchcal:
		{
			XYCAL(2);
		}break;
		case factory:
		{
			if(passverify == 0)
			{
				input_pass("2");	
			}else{
				input_num("2");
			}
		}break;
	}
}

void KEY3_HANDLE(void)
{		
	switch(page_flag)
	{
		case display:
		{
//			BEEP_OFF;
		}break;
		case separation:
		{
			input_num("3");
		}break;
		case graphset:
		{
			input_num("3");
		}break;
		case calibrate:
		{
			input_num("3");			
		}break;
		case history:
		{
			input_search("3");			
		}break;
		case settings:
		{
			if(op_flag == set_timer)
			{
				input_num("3");	
			}else if(op_flag == starttime){
				Input_start(3);
			}else if(op_flag == endtime){
				Input_end(3);
			}
		}break;
		case touchcal:
		{
			XYCAL(3);
		}break;
		case factory:
		{
			if(passverify == 0)
			{
				input_pass("3");	
			}else{
				input_num("3");
			}
		}break;
	}
}

void KEY4_HANDLE(void)
{		
	switch(page_flag)
	{
		case separation:
		{
			input_num("4");
		}break;
		case graphset:
		{
			input_num("4");
		}break;
		case calibrate:
		{
			input_num("4");			
		}break;
		case history:
		{
			input_search("4");			
		}break;
		case settings:
		{
			if(op_flag == set_timer)
			{
				input_num("4");	
			}else if(op_flag == starttime){
				Input_start(4);
			}else if(op_flag == endtime){
				Input_end(4);
			}
		}break;
		case touchcal:
		{
			XYCAL(4);
		}break;
		case factory:
		{
			if(passverify == 0)
			{
				input_pass("4");	
			}else{
				input_num("4");
			}
		}break;
	}
}

void KEY5_HANDLE(void)
{		
	switch(page_flag)
	{
		case separation:
		{
			input_num("5");
		}break;
		case graphset:
		{
			input_num("5");
		}break;
		case calibrate:
		{
			input_num("5");			
		}break;
		case history:
		{
			input_search("5");			
		}break;
		case settings:
		{
			if(op_flag == set_timer)
			{
				input_num("5");	
			}else if(op_flag == starttime){
				Input_start(5);
			}else if(op_flag == endtime){
				Input_end(5);
			}
		}break;
		case touchcal:
		{
			XYCAL(5);
		}break;
		case factory:
		{
			if(passverify == 0)
			{
				input_pass("5");	
			}else{
				input_num("5");
			}
		}break;
	}
}

void KEY6_HANDLE(void)
{		
	switch(page_flag)
	{
		case separation:
		{
			input_num("6");
		}break;
		case graphset:
		{
			input_num("6");
		}break;
		case calibrate:
		{
			input_num("6");			
		}break;
		case history:
		{
			input_search("6");			
		}break;
		case settings:
		{
			if(op_flag == set_timer)
			{
				input_num("6");	
			}else if(op_flag == starttime){
				Input_start(6);
			}else if(op_flag == endtime){
				Input_end(6);
			}
		}break;
		case touchcal:
		{
			XYCAL(6);
		}break;
		case factory:
		{
			if(passverify == 0)
			{
				input_pass("6");	
			}else{
				input_num("6");
			}
		}break;
	}
}

void KEY7_HANDLE(void)
{		
	switch(page_flag)
	{
		case separation:
		{
			input_num("7");
		}break;
		case graphset:
		{
			input_num("7");
		}break;
		case calibrate:
		{
			input_num("7");			
		}break;
		case history:
		{
			input_search("7");			
		}break;
		case settings:
		{
			if(op_flag == set_timer)
			{
				input_num("7");	
			}else if(op_flag == starttime){
				Input_start(7);
			}else if(op_flag == endtime){
				Input_end(7);
			}
		}break;
		case touchcal:
		{
			XYCAL(7);
		}break;
		case factory:
		{
			if(passverify == 0)
			{
				input_pass("7");	
			}else{
				input_num("7");
			}
		}break;
	}
}

void KEY8_HANDLE(void)
{		
	switch(page_flag)
	{
		case separation:
		{
			input_num("8");
		}break;
		case graphset:
		{
			input_num("8");
		}break;
		case calibrate:
		{
			input_num("8");			
		}break;
		case history:
		{
			input_search("8");			
		}break;
		case settings:
		{
			if(op_flag == set_timer)
			{
				input_num("8");	
			}else if(op_flag == starttime){
				Input_start(8);
			}else if(op_flag == endtime){
				Input_end(8);
			}
		}break;
		case touchcal:
		{
			XYCAL(8);
		}break;
		case factory:
		{
			if(passverify == 0)
			{
				input_pass("8");	
			}else{
				input_num("8");
			}
		}break;
	}
}

void KEY9_HANDLE(void)
{		
	switch(page_flag)
	{
		case separation:
		{
			input_num("9");			
		}break;
		case graphset:
		{
			input_num("9");
		}break;
		case calibrate:
		{
			input_num("9");			
		}break;
		case history:
		{
			input_search("9");			
		}break;
		case settings:
		{
			if(op_flag == set_timer)
			{
				input_num("9");	
			}else if(op_flag == starttime){
				Input_start(9);
			}else if(op_flag == endtime){
				Input_end(9);
			}
		}break;
		case factory:
		{
			if(passverify == 0)
			{
				input_pass("9");	
			}else{
				input_num("9");
			}
		}break;
	}
}

void KEY0_HANDLE(void)
{		
	switch(page_flag)
	{
		case separation:
		{
			input_num("0");
		}break;
		case graphset:
		{
			input_num("0");
		}break;
		case calibrate:
		{
			input_num("0");			
		}break;
		case history:
		{
			input_search("0");			
		}break;
		case settings:
		{
			if(op_flag == set_timer)
			{
				input_num("0");	
			}else if(op_flag == starttime){
				Input_start(0);
			}else if(op_flag == endtime){
				Input_end(0);
			}
		}break;
		case factory:
		{
			if(passverify == 0)
			{
				input_pass("0");	
			}else{
				input_num("0");
			}
		}break;
	}
}

void DOT_HANDLE(void)
{		
	switch(page_flag)
	{
		case separation:
		{
			input_num(".");
		}break;
		case graphset:
		{
			input_num(".");
		}break;
		case calibrate:
		{
			input_num(".");			
		}break;
	}
}

void BACK_HANDLE(void)
{		
	switch(page_flag)
	{
		case display:
		{
//			sdtest();
		}break;
		case separation:
		{
			del_num();
		}break;
		case graphset:
		{
			del_num();
		}break;
		case calibrate:
		{
			del_num();			
		}break;
		case history:
		{
			del_num();			
		}break;
		case settings:
		{
			if(op_flag == set_timer)
			{
				del_num();	
			}
		}break;
		case factory:
		{
			if(passverify == 0)
			{
				del_num();	
			}else{
				del_num();
			}
		}break;
	}
}
void ESC_HANDLE(void)
{
	switch(page_flag)
	{
		case display:
		{
			if(op_flag == type_1 || op_flag == type_2 || op_flag == type_3)
			{
				op_flag = home_type;
				DrawMenu();
				Drawhomemenu();
			}
		}break;
		case sysset:
		{
			DrawMenu();
			Drawsysmenu();
			op_sw = op_off;
		}break;
		case settings:
		{
			DrawMenu();
			Drawsetmenu();
			op_sw = op_off;
		}break;
		case calibrate:
		{
			
		}break;
	}
}
void ACC_HANDLE(void)
{
//	u8 i;
	
	//	SPI_Flash_PowerDown();
	//		SPI_Flash_WAKEUP();
	//	for(i=1;i<11;i++)
	//	{
	//		SPI_FLASH_SectorErase(i);
	//	}
	//	Save_Sflag();
	switch(page_flag)
	{
		case calibrate:
		{
			u8 k;
			k = cor_page * 8 + (op_flag - 83);
			Correction[k] = 0;
			maxfocus_on(op_flag - 82,Correction[k]);
			Save_flag();
		}break;
		case display:
		{
			page_home();
		}break;
		case poweron:
		{
			page_factory();
		}break;
		case sysinfo:
		{
			TP_Adjust();
//			TouchCal();
//			JumpBoot(55);
		}break;
		case history:
		{
			
		}break;
		default:break;
	}
}


void Utest(void)
{
	u8 buf[200];
	u8 i;
	static u8 filename[64];
	static u8 TarName[64];
	static u8 s;
	
	static u32 udcount;
	if(fileflag == 0)
	{
		fileflag = 1;
		udcount = 0;
		sprintf((char *)filename, "/%02d%02d%02d%02d.XLS",MONTH
											,DATE
											,HOURS
											,MINUTES); /* 目标文件名 */
		strcpy((char *)TarName,filename);
//		s = CH376FileCreatePath("\\TEST.XLS");
		s = CH376FileCreatePath(TarName);
		if(s != 0x14)
		{
			//fileflag = 0;
		}
		
		switch(TCTYPE)
		{
			case TCT:
			{
				sprintf((char *)buf,"热电偶类型:T-%d",TCT);
			}break;
			case TCK:
			{
				sprintf((char *)buf,"热电偶类型:K-%d",TCK);
			}break;
			case TCJ:
			{
				sprintf((char *)buf,"热电偶类型:J-%d",TCJ);
			}break;
			case TCN:
			{
				sprintf((char *)buf,"热电偶类型:N-%d",TCN);
			}break;
			case TCE:
			{
				sprintf((char *)buf,"热电偶类型:E-%d",TCE);
			}break;
			case TCS:
			{
				sprintf((char *)buf,"热电偶类型:S-%d",TCS);
			}break;
			case TCR:
			{
				sprintf((char *)buf,"热电偶类型:R-%d",TCR);
			}break;
			case TCB:
			{
				sprintf((char *)buf,"热电偶类型:B-%d",TCB);
			}break;
			case PT100:
			{
				sprintf((char *)buf,"热电偶类型:PT100-%d",PT100);
			}break;
		}
		s = CH376ByteWrite( buf, strlen((const char *)buf), NULL );
		if(s != 0x14)
		{
			//fileflag = 0;
		}
		switch(UNIT)
		{
			case C:
			{
				sprintf((char *)buf,"\t单位:℃-%d\n",C);
			}break;
			case F:
			{
				sprintf((char *)buf,"\t单位:℉-%d\n",F);
			}break;
			case K:
			{
				sprintf((char *)buf,"\t单位:K-%d\n",K);
			}break;
		}
		s = CH376ByteWrite(buf, strlen((const char *)buf), NULL );
		if(s != 0x14)
		{
			//fileflag = 0;
		}
		CH376ByteLocate(0xFFFFFFFF);
		if(CHNUM == 16)
		{
			sprintf((char *)buf,"\t日期\t时间\t通道1\t通道2\t通道3\t通道4\t通道5\t通道6\t通道7\t通道8\t通道9\t通道10\t通道11\t通道12\t通道13\t通道14\t通道15\t通道16");
		}else if(CHNUM == 8){
			sprintf((char *)buf,"\t日期\t时间\t通道1\t通道2\t通道3\t通道4\t通道5\t通道6\t通道7\t通道8");
		}
		s = CH376ByteWrite(buf, strlen((const char *)buf), NULL );
		if(s != 0x14)
		{
			//fileflag = 0;
		}
		s = CH376FileClose(TRUE);
		if(s != 0x14)
		{
			//fileflag = 0;
		}
	}
	Delay(100);
//	s = CH376FileOpenPath("\\TEST.XLS");
	strcpy((char *)TarName,filename);
	s = CH376FileOpenPath(TarName);
	if(s != 0x14)
	{
		//fileflag = 0;
	}
	s = CH376ByteLocate(0xFFFFFFFF);
	if(s != 0x14)
	{
		//fileflag = 0;
	}
	//////// 写入
	udcount++;
	sprintf(buf, "\n%d\t20%02d-%02d-%02d\t%02d:%02d:%02d"
											,udcount
											,YEAR
											,MONTH
											,DATE
											,HOURS
											,MINUTES
											,SECONDS
											); /* 目标文件名 */
	s = CH376ByteWrite( buf, strlen((const char *)buf), NULL ); 
	if(s != 0x14)
	{
		//fileflag = 0;
	}
	for(i=0;i<CHNUM;i++)
	{

		if(ch_temp[i] > 1999)
		{
			strcpy(buf,"\tN/A");
		}else{
			sprintf(buf,"\t%.1f",ch_temp[i]);
		}
		s = CH376ByteWrite( buf, strlen((const char *)buf), NULL );
		if(s != 0x14)
		{
			//fileflag = 0;
		}
	}
//	strcpy((char *)buf, "Test ,Hello World!" );
	
	
	//printf("s=%02x \n",(unsigned short)s );
	s = CH376FileClose(TRUE);
	if(s != 0x14)
	{
		//fileflag = 0;
	}
}

//按键声音
void ButtonSound(void)
{
	BEEP_ON;
	Delay(0xffff);
	BEEP_OFF;
}

//出厂校准
void Fac_Cal(u8 bit,u8 ch,u8 type)
{
	u8 i,calbuf[10];
	
	calbuf[0] = 0x01;
	calbuf[1] = 0x66;
	calbuf[2] = bit;
	calbuf[3] = ch;
	calbuf[4] = type;
	calbuf[5] = 0x5a;
	calbuf[6] = 0xa5;
	
	for(i=0;i<7;i++)
	{
		Usart_SendByte(DEBUG_USART,calbuf[i]);
	}
	
}

/*********************************************END OF FILE**********************/
