/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   RTC驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./RTC/bsp_rtc.h"
#include "./usart/bsp_debug_usart.h"
#include "./lcd/bsp_lcd.h"
#include "jk508.h"

u8 time_buf[10][7];
u32 rec_num = 0;
u8 usbreadtime[7];
u8 trigflag;
RTC_TimeTypeDef RTC_TimeStructure;
RTC_DateTypeDef RTC_DateStructure;
/**
  * @brief  设置时间和日期
  * @param  无
  * @retval 无
  */
void RTC_TimeAndDate_Set(void)
{
//	RTC_TimeTypeDef RTC_TimeStructure;
//	RTC_DateTypeDef RTC_DateStructure;
	
	// 初始化时间
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = HOURS;        
	RTC_TimeStructure.RTC_Minutes = MINUTES;    
	RTC_TimeStructure.RTC_Seconds = SECONDS;      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
	
  // 初始化日期	
	RTC_DateStructure.RTC_WeekDay = WEEKDAY;       
	RTC_DateStructure.RTC_Date = DATE;         
	RTC_DateStructure.RTC_Month = MONTH;         
	RTC_DateStructure.RTC_Year = YEAR;        
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

/**
  * @brief  显示时间和日期
  * @param  无
  * @retval 无
  */
void RTC_TimeAndDate_Show(void)
{
	
	char LCDTemp[100];
	
	uint8_t Rtctmp;
	if(RTC_TimeStructure.RTC_Seconds != 0)
	{
		Rtctmp=0;
	}
	
	// 获取日历
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	
	//更新初始化时间
	YEAR = RTC_DateStructure.RTC_Year;
	MONTH =RTC_DateStructure.RTC_Month;
	DATE = RTC_DateStructure.RTC_Date;
	
	HOURS = RTC_TimeStructure.RTC_Hours;
	MINUTES = RTC_TimeStructure.RTC_Minutes;
	SECONDS = RTC_TimeStructure.RTC_Seconds;
	if(TIMETIRG == trig_off)
	{
		trigflag = 1;
	}else{
		if((HOURS*60 + MINUTES >= STARTH*60 + STARTM) && (HOURS*60 + MINUTES < ENDH*60 + ENDM)){
			trigflag = 1;
		}else{
			trigflag = 0;
		}
	}
	// 每秒打印一次
  	if(Rtctmp != RTC_TimeStructure.RTC_Seconds)
    {					
		if(page_flag != poweron && page_flag != touchcal && GPIO_ReadInputDataBit(GPIOI,GPIO_Pin_11) == 1)
		{
			//液晶显示时间
			sprintf(LCDTemp,"%0.2d:%0.2d:%0.2d", 
			RTC_TimeStructure.RTC_Hours,
			RTC_TimeStructure.RTC_Minutes,
			RTC_TimeStructure.RTC_Seconds);
			
			LCD_SetTextColor(LCD_COLOR_YELLOW);  		
			LCD_SetBackColor(LCD_COLOR_BACK);				
			LCD_DisplayStringLine(5,510,(uint8_t *)LCDTemp);
		}
		
		usbreadtime[0] = (u8)((2000 + RTC_DateStructure.RTC_Year) >> 8);//年高位
		usbreadtime[1] = (u8)(RTC_DateStructure.RTC_Year);//年低位
		usbreadtime[2] = (u8)(RTC_DateStructure.RTC_Month);//月
		usbreadtime[3] = (u8)(RTC_DateStructure.RTC_Date);//日
		usbreadtime[4] = (u8)(RTC_TimeStructure.RTC_Hours);//时
		usbreadtime[5] = (u8)(RTC_TimeStructure.RTC_Minutes);//分
		usbreadtime[6] = (u8)(RTC_TimeStructure.RTC_Seconds);//秒

		if(trigflag == 1)
		{

			if(count == 0)
			{
				time_buf[count/50][0] = 20;
				time_buf[count/50][1] = RTC_DateStructure.RTC_Year;
				time_buf[count/50][2] = RTC_DateStructure.RTC_Month;
				time_buf[count/50][3] = RTC_DateStructure.RTC_Date;
				time_buf[count/50][4] = RTC_TimeStructure.RTC_Hours;
				time_buf[count/50][5] = RTC_TimeStructure.RTC_Minutes;
				time_buf[count/50][6] = RTC_TimeStructure.RTC_Seconds;
			}else if(count == 50){
				time_buf[count/50][0] = 20;
				time_buf[count/50][1] = RTC_DateStructure.RTC_Year;
				time_buf[count/50][2] = RTC_DateStructure.RTC_Month;
				time_buf[count/50][3] = RTC_DateStructure.RTC_Date;
				time_buf[count/50][4] = RTC_TimeStructure.RTC_Hours;
				time_buf[count/50][5] = RTC_TimeStructure.RTC_Minutes;
				time_buf[count/50][6] = RTC_TimeStructure.RTC_Seconds;
			}else if(count == 100){
				time_buf[count/50][0] = 20;
				time_buf[count/50][1] = RTC_DateStructure.RTC_Year;
				time_buf[count/50][2] = RTC_DateStructure.RTC_Month;
				time_buf[count/50][3] = RTC_DateStructure.RTC_Date;
				time_buf[count/50][4] = RTC_TimeStructure.RTC_Hours;
				time_buf[count/50][5] = RTC_TimeStructure.RTC_Minutes;
				time_buf[count/50][6] = RTC_TimeStructure.RTC_Seconds;
			}else if(count == 150){
				time_buf[count/50][0] = 20;
				time_buf[count/50][1] = RTC_DateStructure.RTC_Year;
				time_buf[count/50][2] = RTC_DateStructure.RTC_Month;
				time_buf[count/50][3] = RTC_DateStructure.RTC_Date;
				time_buf[count/50][4] = RTC_TimeStructure.RTC_Hours;
				time_buf[count/50][5] = RTC_TimeStructure.RTC_Minutes;
				time_buf[count/50][6] = RTC_TimeStructure.RTC_Seconds;
			}else if(count == 200){
				time_buf[count/50][0] = 20;
				time_buf[count/50][1] = RTC_DateStructure.RTC_Year;
				time_buf[count/50][2] = RTC_DateStructure.RTC_Month;
				time_buf[count/50][3] = RTC_DateStructure.RTC_Date;
				time_buf[count/50][4] = RTC_TimeStructure.RTC_Hours;
				time_buf[count/50][5] = RTC_TimeStructure.RTC_Minutes;
				time_buf[count/50][6] = RTC_TimeStructure.RTC_Seconds;
			}else if(count == 250){
				time_buf[count/50][0] = 20;
				time_buf[count/50][1] = RTC_DateStructure.RTC_Year;
				time_buf[count/50][2] = RTC_DateStructure.RTC_Month;
				time_buf[count/50][3] = RTC_DateStructure.RTC_Date;
				time_buf[count/50][4] = RTC_TimeStructure.RTC_Hours;
				time_buf[count/50][5] = RTC_TimeStructure.RTC_Minutes;
				time_buf[count/50][6] = RTC_TimeStructure.RTC_Seconds;
			}else if(count == 300){
				time_buf[count/50][0] = 20;
				time_buf[count/50][1] = RTC_DateStructure.RTC_Year;
				time_buf[count/50][2] = RTC_DateStructure.RTC_Month;
				time_buf[count/50][3] = RTC_DateStructure.RTC_Date;
				time_buf[count/50][4] = RTC_TimeStructure.RTC_Hours;
				time_buf[count/50][5] = RTC_TimeStructure.RTC_Minutes;
				time_buf[count/50][6] = RTC_TimeStructure.RTC_Seconds;
			}else if(count == 350){
				time_buf[count/50][0] = 20;
				time_buf[count/50][1] = RTC_DateStructure.RTC_Year;
				time_buf[count/50][2] = RTC_DateStructure.RTC_Month;
				time_buf[count/50][3] = RTC_DateStructure.RTC_Date;
				time_buf[count/50][4] = RTC_TimeStructure.RTC_Hours;
				time_buf[count/50][5] = RTC_TimeStructure.RTC_Minutes;
				time_buf[count/50][6] = RTC_TimeStructure.RTC_Seconds;
			}else if(count == 400){
				time_buf[count/50][0] = 20;
				time_buf[count/50][1] = RTC_DateStructure.RTC_Year;
				time_buf[count/50][2] = RTC_DateStructure.RTC_Month;
				time_buf[count/50][3] = RTC_DateStructure.RTC_Date;
				time_buf[count/50][4] = RTC_TimeStructure.RTC_Hours;
				time_buf[count/50][5] = RTC_TimeStructure.RTC_Minutes;
				time_buf[count/50][6] = RTC_TimeStructure.RTC_Seconds;
			}else if(count == 450){
				time_buf[count/50][0] = 20;
				time_buf[count/50][1] = RTC_DateStructure.RTC_Year;
				time_buf[count/50][2] = RTC_DateStructure.RTC_Month;
				time_buf[count/50][3] = RTC_DateStructure.RTC_Date;
				time_buf[count/50][4] = RTC_TimeStructure.RTC_Hours;
				time_buf[count/50][5] = RTC_TimeStructure.RTC_Minutes;
				time_buf[count/50][6] = RTC_TimeStructure.RTC_Seconds;
				
			}
		}else{
			
		}
		
		if(page_flag == sysset)
		{
			//液晶显示日期
			//先把要显示的数据用sprintf函数转换为字符串，然后才能用液晶显示函数显示
			sprintf(LCDTemp,"20%0.2d-%0.2d-%0.2d",
			RTC_DateStructure.RTC_Year,
			RTC_DateStructure.RTC_Month,
			RTC_DateStructure.RTC_Date);
			if(op_flag == set_date)
			{
				LCD_SetTextColor(LCD_COLOR_BLACK);  		
				LCD_SetBackColor(LCD_COLOR_YELLOW);				
			}else{
				LCD_SetTextColor(LCD_COLOR_YELLOW);  		
				LCD_SetBackColor(LCD_COLOR_BACK);
			}
			LCD_DisplayStringLine(90,170,(uint8_t *)LCDTemp);


			//液晶显示时间
			sprintf(LCDTemp,"%0.2d:%0.2d:%0.2d",
			RTC_TimeStructure.RTC_Hours,
			RTC_TimeStructure.RTC_Minutes,
			RTC_TimeStructure.RTC_Seconds);
			
			if(op_flag == set_time)
			{
				LCD_SetTextColor(LCD_COLOR_BLACK);  		
				LCD_SetBackColor(LCD_COLOR_YELLOW);				
			}else{
				LCD_SetTextColor(LCD_COLOR_YELLOW);  		
				LCD_SetBackColor(LCD_COLOR_BACK);
			}
			LCD_DisplayStringLine(90,400,(uint8_t *)LCDTemp);
		}
		
		(void)RTC->DR;
    }
    Rtctmp = RTC_TimeStructure.RTC_Seconds;
}

/**
  * @brief  RTC配置：选择RTC时钟源，设置RTC_CLK的分频系数
  * @param  无
  * @retval 无
  */
void RTC_CLK_Config(void)
{  
	RTC_InitTypeDef RTC_InitStructure;
	
	/*使能 PWR 时钟*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  /* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
  PWR_BackupAccessCmd(ENABLE);

#if defined (RTC_CLOCK_SOURCE_LSI) 
  /* 使用LSI作为RTC时钟源会有误差 
	 * 默认选择LSE作为RTC的时钟源
	 */
  /* 使能LSI */ 
  RCC_LSICmd(ENABLE);
  /* 等待LSI稳定 */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }
  /* 选择LSI做为RTC的时钟源 */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

#elif defined (RTC_CLOCK_SOURCE_LSE)

  /* 使能LSE */ 
  RCC_LSEConfig(RCC_LSE_ON);
   /* 等待LSE稳定 */   
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }
  /* 选择LSE做为RTC的时钟源 */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);    

#endif /* RTC_CLOCK_SOURCE_LSI */

  /* 使能RTC时钟 */
  RCC_RTCCLKCmd(ENABLE);

  /* 等待 RTC APB 寄存器同步 */
  RTC_WaitForSynchro();
   
/*=====================初始化同步/异步预分频器的值======================*/
	/* 驱动日历的时钟ck_spare = LSE/[(255+1)*(127+1)] = 1HZ */
	
	/* 设置异步预分频器的值 */
	RTC_InitStructure.RTC_AsynchPrediv = ASYNCHPREDIV;
	/* 设置同步预分频器的值 */
	RTC_InitStructure.RTC_SynchPrediv = SYNCHPREDIV;	
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24; 
	/* 用RTC_InitStructure的内容初始化RTC寄存器 */
	if (RTC_Init(&RTC_InitStructure) == ERROR)
	{
		printf("\n\r RTC 时钟初始化失败 \r\n");
	}	
}

/**
  * @brief  RTC配置：选择RTC时钟源，设置RTC_CLK的分频系数
  * @param  无
  * @retval 无
  */
#define LSE_STARTUP_TIMEOUT     ((uint16_t)0x05000)
void RTC_CLK_Config_Backup(void)
{  
  __IO uint16_t StartUpCounter = 0;
	FlagStatus LSEStatus = RESET;	
	RTC_InitTypeDef RTC_InitStructure;
	
	/* 使能 PWR 时钟 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  /* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
  PWR_BackupAccessCmd(ENABLE);
	
/*=========================选择RTC时钟源==============================*/
/* 默认使用LSE，如果LSE出故障则使用LSI */
  /* 使能LSE */
  RCC_LSEConfig(RCC_LSE_ON);	
	
	/* 等待LSE启动稳定，如果超时则退出 */
  do
  {
    LSEStatus = RCC_GetFlagStatus(RCC_FLAG_LSERDY);
    StartUpCounter++;
  }while((LSEStatus == RESET) && (StartUpCounter != LSE_STARTUP_TIMEOUT));
	
	
	if(LSEStatus == SET )
  {
		/* 选择LSE作为RTC的时钟源 */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  }
	else
	{
		
		/* 使能LSI */	
		RCC_LSICmd(ENABLE);
		/* 等待LSI稳定 */ 
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
		{			
		}
		
		printf("\n\r LSI 启动成功 \r\n");
		/* 选择LSI作为RTC的时钟源 */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	}
	
  /* 使能 RTC 时钟 */
  RCC_RTCCLKCmd(ENABLE);
  /* 等待 RTC APB 寄存器同步 */
  RTC_WaitForSynchro();

/*=====================初始化同步/异步预分频器的值======================*/
	/* 驱动日历的时钟ck_spare = LSE/[(255+1)*(127+1)] = 1HZ */
	
	/* 设置异步预分频器的值为127 */
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
	/* 设置同步预分频器的值为255 */
	RTC_InitStructure.RTC_SynchPrediv = 0xFF;	
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24; 
	/* 用RTC_InitStructure的内容初始化RTC寄存器 */
	if (RTC_Init(&RTC_InitStructure) == ERROR)
	{
		printf("\n\r RTC 时钟初始化失败 \r\n");
	}	
}

void DrawTime(void)
{
	char LCDTemp[100];
	
	sprintf(LCDTemp,"%0.2d:%0.2d:%0.2d",
			usbreadtime[4],
			usbreadtime[5],
			usbreadtime[6]);
			
	if(count == 0)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		DISP_CNL_S(402,80-80,(uint8_t *)LCDTemp);
	}else if(count == 50){
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		DISP_CNL_S(420,130-80,(uint8_t *)LCDTemp);
	}else if(count == 100){
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		DISP_CNL_S(402,180-80,(uint8_t *)LCDTemp);
	}else if(count == 150){
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		DISP_CNL_S(420,230-80,(uint8_t *)LCDTemp);
	}else if(count == 200){
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		DISP_CNL_S(402,280-80,(uint8_t *)LCDTemp);
	}else if(count == 250){
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);				
		DISP_CNL_S(420,330-80,(uint8_t *)LCDTemp);
	}else if(count == 300){
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		DISP_CNL_S(402,380-80,(uint8_t *)LCDTemp);
	}else if(count == 350){
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		DISP_CNL_S(420,430-80,(uint8_t *)LCDTemp);
	}else if(count == 400){
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		DISP_CNL_S(402,480-80,(uint8_t *)LCDTemp);
	}else if(count == 450){
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_SetBackColor(LCD_COLOR_BACK);
		DISP_CNL_S(420,530-80,(uint8_t *)LCDTemp);
	}else if(count == 495){
		rec_num ++;
//				Save_flag();
		LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
		LCD_DrawFullRect(0,402,600,34);
		DrawGridLine();
	}
}

/**********************************END OF FILE*************************************/
