/**
  ******************************************************************************
  * @file    bsp_basic_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   基本定时器定时范例
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./tim/bsp_basic_tim.h"
#include "./usart/bsp_debug_usart.h"
#include "./key/bsp_key.h" 
#include "./lcd/bsp_lcd.h"
#include "./ch376/ch376.h"
#include  "usbd_hid_core.h"
#include "./beep/bsp_beep.h"
#include  "usbd_usr.h"
#include  "usbd_desc.h"
#include "jk508.h"

void MODS_Poll(void);
void RecHandle(void);

extern u8 key_value;
extern u16 count;
extern u8 count_flag;
extern u8 uartflag;
u8 tempreq[9] = {0x01,0x03,0x00,0x00,0x00,0x10,0x44,0x06,0xff};
u8 reqcode;
u8 brightness;
u32 Tick_10ms=0;
u32 OldTick;
u8 udflag;
extern u8 ReCount;
extern u8 g_mods_timeout;
extern __ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;
 /**
  * @brief  基本定时器 TIMx,x[6,7]中断优先级配置
  * @param  无
  * @retval 无
  */
static void TIMx_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = BASIC_TIM_IRQn; 	
		// 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	  // 设置子优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         都有
 * TIM_CounterMode			 TIMx,x[6,7]没有，其他都有（基本定时器）
 * TIM_Period            都有
 * TIM_ClockDivision     TIMx,x[6,7]没有，其他都有(基本定时器)
 * TIM_RepetitionCounter TIMx,x[1,8]才有(高级定时器)
 *-----------------------------------------------------------------------------
 */
static void TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	// 开启TIMx_CLK,x[6,7] 
  RCC_APB1PeriphClockCmd(BASIC_TIM_CLK, ENABLE); 

  /* 累计 TIM_Period个后产生一个更新或者中断*/		
  //当定时器从0计数到4999，即为5000次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Period = 100-1;       
	
	//定时器时钟源TIMxCLK = 2 * PCLK1  
  //				PCLK1 = HCLK / 4 
  //				=> TIMxCLK=HCLK/2=SystemCoreClock/2=90MHz
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=10000Hz
  TIM_TimeBaseStructure.TIM_Prescaler = 9000-1;	
	
	// 初始化定时器TIMx, x[2,3,4,5]
	TIM_TimeBaseInit(BASIC_TIM, &TIM_TimeBaseStructure);
	
	
	// 清除定时器更新中断标志位
	TIM_ClearFlag(BASIC_TIM, TIM_FLAG_Update);
	
	// 开启定时器更新中断
	TIM_ITConfig(BASIC_TIM,TIM_IT_Update,ENABLE);
	
	// 使能定时器
	TIM_Cmd(BASIC_TIM, ENABLE);	
}

static void TIM_7_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	// 开启TIMx_CLK,x[6,7] 
	RCC_APB1PeriphClockCmd(BEEP_TIM_CLK, ENABLE); 

  /* 累计 TIM_Period个后产生一个更新或者中断*/		
  //当定时器从0计数到4999，即为5000次，为一个定时周期
	TIM_TimeBaseStructure.TIM_Period = 10-1;       
	
	//定时器时钟源TIMxCLK = 2 * PCLK1  
  //				PCLK1 = HCLK / 4 
  //				=> TIMxCLK=HCLK/2=SystemCoreClock/2=90MHz
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=10000Hz
	TIM_TimeBaseStructure.TIM_Prescaler = 9000-1;	
	
	// 初始化定时器TIMx, x[2,3,4,5]
	TIM_TimeBaseInit(BEEP_TIM, &TIM_TimeBaseStructure);
	
	
	// 清除定时器更新中断标志位
	TIM_ClearFlag(BEEP_TIM, TIM_FLAG_Update);
	
	// 开启定时器更新中断
	TIM_ITConfig(BEEP_TIM,TIM_IT_Update,ENABLE);
	
	// 使能定时器
	TIM_Cmd(BEEP_TIM, ENABLE);	
}


/**
  * @brief  配置TIM复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
static void TIMx_GPIO_Config(void) 
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启相关的GPIO外设时钟*/
	RCC_AHB1PeriphClockCmd (LTDC_BL_GPIO_CLK, ENABLE); 
  /* 定时器通道引脚复用 */
	GPIO_PinAFConfig(LTDC_BL_GPIO_PORT,GENERAL_OCPWM_PINSOURCE,GENERAL_OCPWM_AF); 
  
	/* 定时器通道引脚配置 */															   
	GPIO_InitStructure.GPIO_Pin = LTDC_BL_GPIO_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(LTDC_BL_GPIO_PORT, &GPIO_InitStructure);
}



/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         都有
 * TIM_CounterMode			 TIMx,x[6,7]没有，其他都有（基本定时器）
 * TIM_Period            都有
 * TIM_ClockDivision     TIMx,x[6,7]没有，其他都有(基本定时器)
 * TIM_RepetitionCounter TIMx,x[1,8]才有(高级定时器)
 *-----------------------------------------------------------------------------
 */
void TIM_PWMOUTPUT_Config(u8 duty)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	// 开启TIMx_CLK,x[2,3,4,5,12,13,14] 
  RCC_APB1PeriphClockCmd(GENERAL_TIM_CLK, ENABLE); 

  /* 累计 TIM_Period个后产生一个更新或者中断*/		
  //当定时器从0计数到8999，即为9000次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Period = 100-1;       
	
	// 高级控制定时器时钟源TIMxCLK = HCLK/2=90MHz 
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=100KHz
  TIM_TimeBaseStructure.TIM_Prescaler = 9000-1;	
  // 采样时钟分频
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  // 计数方式
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	// 初始化定时器TIMx, x[2,3,4,5,12,13,14] 
	TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);
	
	/*PWM模式配置*/
	/* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = duty-1;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  	  //当定时器计数值小于CCR1_Val时为高电平
  TIM_OC3Init(GENERAL_TIM, &TIM_OCInitStructure);	 //使能通道3
  
	/*使能通道1重载*/
	TIM_OC1PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
	
	// 使能定时器
	TIM_Cmd(GENERAL_TIM, ENABLE);	
}

/**
  * @brief  配置TIM复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
static void TIM8_GPIO_Config(void) 
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启相关的GPIO外设时钟*/
	RCC_AHB1PeriphClockCmd (BEEP_GPIO_CLK, ENABLE); 
  /* 定时器通道引脚复用 */
	GPIO_PinAFConfig(BEEP_PORT,BEEP_OCPWM_PINSOURCE,BEEP_OCPWM_AF); 
  
	/* 定时器通道引脚配置 */															   
	GPIO_InitStructure.GPIO_Pin = BEEP_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(BEEP_PORT, &GPIO_InitStructure);
}

/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         都有
 * TIM_CounterMode			 TIMx,x[6,7]没有，其他都有（基本定时器）
 * TIM_Period            都有
 * TIM_ClockDivision     TIMx,x[6,7]没有，其他都有(基本定时器)
 * TIM_RepetitionCounter TIMx,x[1,8]才有(高级定时器)
 *-----------------------------------------------------------------------------
 */
void TIM8_PWMOUTPUT_Config(u16 duty)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	
	// 开启TIMx_CLK,x[2,3,4,5,12,13,14] 
  RCC_APB2PeriphClockCmd(BEEP_TIM_CLK, ENABLE); 

 //当定时器从0计数到1023，即为1024次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Period = 63;
	// 高级控制定时器时钟源TIMxCLK = HCLK=180MHz 
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=100000Hz
  TIM_TimeBaseStructure.TIM_Prescaler = 1800-1;	
  // 采样时钟分频
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  // 计数方式
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	// 初始化定时器TIMx, x[2,3,4,5,12,13,14] 
	TIM_TimeBaseInit(BEEP_TIM, &TIM_TimeBaseStructure);
	
	/*PWM模式配置*/
	/* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = duty;
 TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
  
	/* 使能通道1重载 */
	TIM_OC1PreloadConfig(BEEP_TIM, TIM_OCPreload_Enable);
		
//	/* 自动输出使能，断路、死区时间和锁定配置 */
//  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
//  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
//  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
//  TIM_BDTRInitStructure.TIM_DeadTime = 11;
//  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
//  TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;
//  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
//  TIM_BDTRConfig(BEEP_TIM, &TIM_BDTRInitStructure);
  TIM_OC1Init(BEEP_TIM, &TIM_OCInitStructure);	 //使能通道1
  

	/*使能通道1重载*/
	TIM_OC1PreloadConfig(BEEP_TIM, TIM_OCPreload_Enable);
	
	// 使能定时器
	TIM_Cmd(BEEP_TIM, ENABLE);	
	
}
/**
  * @brief  初始化基本定时器定时，1ms产生一次中断
  * @param  无
  * @retval 无
  */
void TIMx_Configuration(void)
{
	TIMx_NVIC_Configuration();	
    TIM_Mode_Config();
	
	TIM8_GPIO_Config();
	TIM8_PWMOUTPUT_Config(32);
	
	TIMx_GPIO_Config();  
	TIM_PWMOUTPUT_Config(brightness);
}

void BEEP_TIM_IRQHandler(void)
{
	static u8 beepsw;
	if(TIM_GetITStatus( BEEP_TIM, TIM_IT_Update) != RESET )
	{
		if(beepsw == 0)
		{
			BEEP_ON;
		}else{
			BEEP_OFF;
		}
		beepsw = !beepsw;
		TIM_ClearITPendingBit(BEEP_TIM,TIM_IT_Update);
	}
}

void BASIC_TIM_IRQHandler (void)
{
	static u8 sendcount;
//	static u16 usavecount;
	static u32 dim_time;
	static u8 dimflag;
	
	u8 i;
	if(TIM_GetITStatus( BASIC_TIM, TIM_IT_Update) != RESET )
	{
		Key_Scan();//按键扫描
//		Touch_Scan();//触摸扫描
		
//		Tick_10ms++;
//		MODS_Poll();
		if(page_flag != poweron && trigflag == 1)
		{
			if(sendcount == 4*5 && GPIO_ReadInputDataBit(GPIOI,GPIO_Pin_11))
			{
				if(tcflag == 0)//没有通道设置命令，发送温度请求命令
				{
					GPIO_SetBits(GPIOA,GPIO_Pin_8);
		//			USART_ITConfig(DEBUG_USART, USART_IT_RXNE, DISABLE);
					for(i=0;i<9;i++)
					{
						Usart_SendByte(DEBUG_USART,tempreq[i]);//请求温度数据
					}
		//			USART_ITConfig(DEBUG_USART, USART_IT_RXNE, ENABLE);						
					sendcount = 0;
		//			GPIO_SetBits(GPIOA,GPIO_Pin_8);	
		//			Delay(1000);
					GPIO_ResetBits(GPIOA,GPIO_Pin_8);
				}else{//发送通道设置命令
					if(tcflag == 1)
					{
						SetSTctype();//单独通道设置
					}else{
						SetTctype(tcflag-1);//统一设置
					}
					tcflag = 0;
				}
			}
		}else if(page_flag == poweron){
			if(sendcount == 4*5 && GPIO_ReadInputDataBit(GPIOI,GPIO_Pin_11))
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_8);
	//			USART_ITConfig(DEBUG_USART, USART_IT_RXNE, DISABLE);
				for(i=0;i<9;i++)
				{
					Usart_SendByte(DEBUG_USART,tempreq[i]);//请求温度数据
				}
	//			USART_ITConfig(DEBUG_USART, USART_IT_RXNE, ENABLE);						
				sendcount = 0;
	//			GPIO_SetBits(GPIOA,GPIO_Pin_8);	
	//			Delay(1000);
				GPIO_ResetBits(GPIOA,GPIO_Pin_8);
			}
		}else{
			for(i=0;i<CHNUM;i++)
			{
				ch_temp[i] = 1999.9;
			}
		}
//		Delay(1000);
//		GPIO_ResetBits(GPIOA,GPIO_Pin_8);
//		if(usavecount == 100 && GPIO_ReadInputDataBit(GPIOI,GPIO_Pin_11))
//		{
//			if(page_flag != poweron)
//			{
//				udisk_scan();
//			}
//			usavecount = 0;
//		}
//		if(usave == 100)
//		{
////			if(usbstatus == CONNECTED)
////			{
////				Utest();
////			}
////			usave = 0;
//		}
		if(key_value == 0xFF && dimflag == 0)
		{			
			if(DIM == DOFF)
			{				
			}else if(DIM == D5){
				if(dim_time < 7500*4)
				{
					dim_time++;
				}else if(dim_time == 7500*4)
				{
					TIM_PWMOUTPUT_Config(10);
					dimflag = 1;
				}
			}else if(DIM == D10){
				if(dim_time < 15000*4)
				{
					dim_time++;
				}else if(dim_time == 15000*4)
				{
					TIM_PWMOUTPUT_Config(10);
					dimflag = 1;
				}
			}else if(DIM == D15){
				if(dim_time < 22500*4)
				{
					dim_time++;
				}else if(dim_time == 22500*4)
				{
					TIM_PWMOUTPUT_Config(10);
					dimflag = 1;
				}
			}else if(DIM == D30){
				if(dim_time < 45000*4)
				{
					dim_time++;
				}else if(dim_time == 45000*4)
				{
					TIM_PWMOUTPUT_Config(10);
					dimflag = 1;
				}
			}			
		}else if(key_value != 0xff){
			if(dim_time == (DIM * 7500*4))
			{
				TIM_PWMOUTPUT_Config(brightness);
				dimflag = 0;
			}
			dim_time = 0;
		}
		sendcount ++;
//		usavecount ++;
//		usave ++;
		TIM_ClearITPendingBit(BASIC_TIM,TIM_IT_Update);
	}
}

void MODS_Poll(void)
{
	static u32 testi;
	testi = ReCount;
	//判断通讯接收是否超时
	if(OldTick!=Tick_10ms)
  	{  
	  OldTick=Tick_10ms;
	   if(g_mods_timeout>0)
      { 
	    g_mods_timeout--;
      }
	  if(g_mods_timeout==0 && ReCount>0)   //有数但超时了
      { 
		// goto err_ret;
	
      }
      else if(g_mods_timeout==0 && ReCount==0) //没数超时了
         return;
      else //没超时了，继续收
         return;
	}
	else   //没有到10ms，不进入解析
		return;
	//g_mods_timeout = 0;	 					/* 清标志 */

	if (ReCount < 30)				/* 接收到的数据小于4个字节就认为错误 */
	{
		goto err_ret;
	}
	RecHandle();
	err_ret:
	#if 0										/* 姝ら儴鍒嗕负浜嗕覆鍙ｆ墦鍗扮粨鏋?瀹為檯杩愮敤涓彲涓嶈 */
//		g_tPrint.Rxlen = g_tModS.RxCount;
//		memcpy(g_tPrint.RxBuf, g_tModS.RxBuf, g_tModS.RxCount);

	#endif
	
//	ReCount = 0;	
}

void RecHandle(void)
{
	static uint8_t ucTemp;
	static u8 Total_Len = 0;
	static u8 uinitflag = 0;
	static u8 multicount = 0;
//	static float graphbuf[16];
	static u8 graphbuf[16][2];
	static u8 hisbuf[16][2];
	static u16 hisconv;
	static u16 corconv;
	u8 i;
	char buf[10];
	static int16_t tempbuf;
	
	if(ReCount != 39)
	{
		return;
	}else{
			charge = RecBuff[3];
			battery = RecBuff[4];
//					if(bcount == 3)
//					{
//						battery = btbuff/3;
//						DrawBattery(battery);
//						btbuff = 0;
//						bcount = 0;
//					}else{
//						btbuff += RecBuff[4];
//						bcount++;
//					}
			for(i=0;i<16;i++)
			{
				tempbuf = RecBuff[2*(i+1)+3]<<8;
				tempbuf = tempbuf + RecBuff[2*(i+1)+4];
				if(tempbuf < 0)
				{
					ch_temp[i] = (float)tempbuf/10;
				}else{
					ch_temp[i] = (float)tempbuf/10;
				}
//						ch_temp[i] = (RecBuff[2*(i+1)+3] * 256 + RecBuff[2*(i+1)+4])/10.0;
				if(count == 0 && page_flag == poweron)
				{
					InitBrt();//开机亮度
					LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_BLACK);
					DISP_INS(5+i*20,5,"Initializing Channel");
					sprintf(buf,"%03d",i+1);
					DISP_INS(5+i*20,336,(uint8_t*)buf);
					DISP_INS(5+i*20,384,"...");
					Delay(0x3fffff);
				}
				
			}
			
			
			if(multicount == 1 && page_flag == poweron)
			{
				LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_BLACK);
				DISP_INS(325,5,"Done!");
				Delay(0xffffff);
				page_home();
			}
			
			if(multicount%(int)MULTI == 0 && multicount != 0)
			{
				for(i=0;i<16;i++)
				{
					G_Data[i][count] = (graphbuf[i][0]/MULTI * 256 + graphbuf[i][1]/MULTI)/10.0 - Correction[i];							
				}
				if(page_flag == graph)
				{
					Draw_graph();
					DrawTime();
				}
//						if(page_flag != history)
//						{
				for(i=0;i<16;i++)
				{
//								savebuf = hex_to_bcd((int)(graphbuf[i]/MULTI * 10));
					hisconv = (u16)(hisbuf[i][0]/MULTI)<<8;
					hisconv = hisconv + hisbuf[i][1]/MULTI;
					corconv = (u16)(Correction[i]*10);
//								Data_buf[i][count%8 * 2] = hisbuf[i][0]/MULTI;
//								Data_buf[i][count%8 * 2 + 1] = hisbuf[i][1]/MULTI;
					Data_buf[i][count%8 * 2] = (u8)((hisconv - corconv)>>8);
					Data_buf[i][count%8 * 2 + 1] = (u8)(hisconv - corconv);
				}
//							Save_history(1);
				if(count > 0 && (count + 1) % 8 == 0)
				{
//								recflag = 1;
//					if(SECTOR_REC < 62000)
//					{
//						SECTOR_REC ++;
//						Save_history(SECTOR_REC);								
//						Save_Sflag();									
//					}else{
//						SECTOR_REC = 0;
//					}
					
				}
//						}
				if(count == 450)
				{
//					if(TIME_REC < 1000)
//					{
//						TIME_REC++;
//						Save_time(TIME_REC);
//						Save_Sflag();
//					}else{
//						TIME_REC = 0;
//					}
				}
				if(count > 494)
				{
					count = 0;
//							memcpy(hisdata,G_Data,sizeof(G_Data));
//							memcpy(histime,time_buf,sizeof(time_buf));
				}else{
					count++;
				}
				multicount=0;
				
				for(i = 0;i<16;i++)
				{
					hisbuf[i][0] = 0;
					hisbuf[i][1] = 0;
//							graphbuf[i] = 0;
					graphbuf[i][0] = 0;
					graphbuf[i][1] = 0;
				}

				for(i=0;i<16;i++)
				{
					hisbuf[i][0] += RecBuff[2*(i+1)+3];
					hisbuf[i][1] += RecBuff[2*(i+1)+4];
//							graphbuf[i] += ch_temp[i];
					graphbuf[i][0] += RecBuff[2*(i+1)+3];
					graphbuf[i][1] += RecBuff[2*(i+1)+4];
				}
				multicount++;
			}else{
				for(i=0;i<16;i++)
				{
					hisbuf[i][0] += RecBuff[2*(i+1)+3];
					hisbuf[i][1] += RecBuff[2*(i+1)+4];
//							graphbuf[i] += ch_temp[i];
					graphbuf[i][0] += RecBuff[2*(i+1)+3];
					graphbuf[i][1] += RecBuff[2*(i+1)+4];
				}
				multicount++;
			}
			ReCount = 0;
		}
}

void SetTctype(u8 type)
{
	u8 tcbuf[28];
	u8 i;
	static u16 tccrc;
	u8 tcrc[25];
	
	tcbuf[0]=0X01;
	tcbuf[1]=0X10;
	tcbuf[2]=0X00;
	tcbuf[3]=0X00;
	tcbuf[4]=0X00;
	tcbuf[5]=0X09;
	tcbuf[6]=0X12;
	for(i=0;i<16;i++)
	{
		tcbuf[i+7]=type;
		savedata[60+i] = type;
	}
	tcbuf[23] = UNIT;
	tcbuf[24] = CHNUM;
	for(i=0;i<25;i++)
	{
		tcrc[i] = tcbuf[i];
	}
	tccrc = CRC16(tcrc,25);
	tcbuf[26] = (u8)(tccrc >> 8);
	tcbuf[25] = (u8)(tccrc);
	tcbuf[27] = 0xff;
	trigflag = 0;
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
	for(i=0;i<28;i++)
	{
		Usart_SendByte(DEBUG_USART,tcbuf[i]);
	}
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
	trigflag = 1;
	Disp_Type();
}

void SetSTctype(void)
{
	u8 tcbuf[28];
	u8 i;
	static u16 tccrc;
	u8 tcrc[25];
	
	tcbuf[0]=0X01;
	tcbuf[1]=0X10;
	tcbuf[2]=0X00;
	tcbuf[3]=0X00;
	tcbuf[4]=0X00;
	tcbuf[5]=0X09;
	tcbuf[6]=0X12;
	for(i=0;i<16;i++)
	{
		tcbuf[i+7]=savedata[60+i];
	}
	tcbuf[23] = UNIT;
	tcbuf[24] = CHNUM;
	for(i=0;i<25;i++)
	{
		tcrc[i] = tcbuf[i];
	}
	tccrc = CRC16(tcrc,25);
	tcbuf[26] = (u8)(tccrc >> 8);
	tcbuf[25] = (u8)(tccrc);
	tcbuf[27] = 0xff;
	trigflag = 0;
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
	for(i=0;i<28;i++)
	{
		Usart_SendByte(DEBUG_USART,tcbuf[i]);
	}
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
	trigflag = 1;
	Disp_Type();
}
///**
//  * @brief  初始化高级控制定时器定时，1ms产生一次中断
//  * @param  无
//  * @retval 无
//  */
//void TIMx_Configuration(void)
//{
//	TIMx_GPIO_Config();
//  
//  TIM_PWMOUTPUT_Config();
//}

/*********************************************END OF FILE**********************/
