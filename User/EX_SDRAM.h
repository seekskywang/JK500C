
#ifndef __EX_SDRAM_h
#define __EX_SDRAM_h

#include "LPC177x_8x.h"
#include "lpc_types.h"
#include "system_LPC177x_8x.h" 

#define SDRAM_CONFIG_32BIT	// 外接2颗16位SDRAM

#ifdef SDRAM_CONFIG_16BIT 
#define SDRAM_SIZE               0x2000000	// 256Mbit
/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Blue           0xF800
#define Red            0x001F
#define Magenta        0xF81F
#define Green          0x07E0
#define Yellow         0x07FF

#elif defined SDRAM_CONFIG_32BIT	 //
/* LCD color */
#define White          0xFFFFFF
#define Black          0x000000
#define Red            0x0000FF
#define Green          0x00FF00
#define Blue           0xFF0000
#define Yellow         0x00FFFF
/*
  * @brief  LCD color  
  */ 

#define LCD_COLOR_WHITE          0xFFFFFF	//白色
#define LCD_COLOR_BLACK          0x000000	//黑色
#define LCD_COLOR_GREY           0xC0C0C0	//灰色
#define LCD_COLOR_BLUE           0x0000FF	//蓝色
#define LCD_COLOR_BLUE2          0xA00000
#define LCD_COLOR_RED            0xFF0000
#define LCD_COLOR_MAGENTA        0xFF00FF	 //洋红色
#define LCD_COLOR_GREEN          0x00FF00
#define LCD_COLOR_CYAN           0x00FFFF	//蓝绿色
#define LCD_COLOR_YELLOW         0xFFFF00
#define SDRAM_SIZE               0x4000000	// 512Mbit
#define	LCD_COLOR_TEST_MID  	 0x653434
#define	LCD_COLOR_TEST_BACK 	 0X500050//0xb4007d//
#define	LCD_COLOR_TEST_BUTON     0X310038		//按钮颜色
#define	LCD_COLOR_TEST_LINE      0X694368		//线条颜色
#define	LCD_COLOR_TEST_BAR       0x352F3
#define	LCD_COLOR_SELECT       	LCD_COLOR_BLUE  //被选中的颜色

#define LCD_COLOR_TURQUOISE		 0x3f4828	//蓝绿色     //0x9b9907 (蓝色)0x5f6738
#define LCD_COLOR_BABYGREY		 0xebe6e8	//浅灰色
#define LCD_COLOR_BABYBLACK		 0x1d1d1d	//浅黑色
#define LCD_COLOR_BABYBLUE		 0xe1e723
#define LCD_COLOR_ORANGE		 0x0000FF	//橘黄色
#define LCD_COLOR_DARKGREY	     0x8d898a 	//深灰色

#else
	error Wrong SDRAM config, check ex_sdram.h
#endif

#define SDRAM_BASE               0xA0000000	   /* CS0 */

void SDRAM_Init (void);

#endif
