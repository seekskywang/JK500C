/**
  ******************************************************************************
  * @file    FMC_SDRAM/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   Main Interrupt Service Routines.
  *         This file provides template for all exceptions handler and
  *         peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h"
#include "./usart/bsp_debug_usart.h"
#include "jk508.h"
#include "usb_core.h"
#include "usbd_core.h"
//#include "usbd_hid_core.h"
#include "./lcd/bsp_lcd.h"
#include "usb_conf.h"
#include "./flash/bsp_spi_flash.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define CURSOR_STEP     10
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t remote_wakeup =0;
extern __IO uint32_t FlashID;
/* Private function prototypes -----------------------------------------------*/
extern USB_OTG_CORE_HANDLE           USB_OTG_dev;
//static uint8_t *USBD_HID_GetPos (void);
extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED 
extern uint32_t USBD_OTG_EP1IN_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
extern uint32_t USBD_OTG_EP1OUT_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
#endif

u8 RecBuff[39];

u8 charge;
u8 battery;
u8 reflag;
u8 ReCount=0;
u8 g_mods_timeout = 0;
//static u8 watchuart[39];
//static u8 watchcount;
extern u8 sendflag;
u8 uartflag;
/** @addtogroup STM32F429I_DISCOVERY_Examples
  * @{
  */

/** @addtogroup FMC_SDRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
		LED_RED;
	}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/

/**
  * @}
  */ 
//void P6_IRQHandler(void)
//{
//  //确保是否产生了EXTI Line中断
//	if(EXTI_GetITStatus(P6_EXTI_LINE) != RESET) 
//	{
//			
//		key_value = Key_Scan();
//    //清除中断标志位
//		EXTI_ClearITPendingBit(P6_EXTI_LINE);     
	
//	}
//}	

void DEBUG_USART_IRQHandler(void)
{
	static uint8_t ucTemp;
	static u8 Total_Len = 0;
	static u8 uinitflag = 0;
//	static u8 multicount = 0;
//	static float graphbuf[16];
//	static u8 graphbuf[16][2];
//	static u8 hisbuf[16][2];
//	static u16 hisconv;
//	static u16 corconv;
//	u8 i;
//	char buf[10];
//	static int16_t tempbuf;
//	static u8 usave;
////	u16 ureadcrc;
////	u8 *ucrc;
//	u8 ucrclen;
	if(uinitflag == 0)
	{
		LCD_SetColors(LCD_COLOR_BLACK,LCD_COLOR_BLACK);
		LCD_DrawFullRect(0,0,640,480); 
		uinitflag = 1;
	}
	if(USART_GetITStatus(DEBUG_USART,USART_IT_RXNE)!=RESET)
	{
		USART_ClearITPendingBit(DEBUG_USART, USART_IT_RXNE);
//		g_mods_timeout = 2;		
//		if(ReCount < 39)
//		{
//			RecBuff[ReCount++] = USART_ReceiveData( DEBUG_USART );
//		}
		RecBuff[ReCount] = USART_ReceiveData( DEBUG_USART );
		if(RecBuff[0] == 0x01 && ReCount == 0)
		{
			ReCount++;
		}else if(RecBuff[1]== 0x03 && ReCount == 1){
			ReCount ++;
			Total_Len = 39;
		}else if(ReCount > 1 && ReCount < 39){
			ReCount ++;
		}
		if(ReCount == Total_Len)
		{
			uartflag = 1;
			ReCount = 0;
		}
//		if(ReCount == 0)
//		{
//			if(ucTemp == 0x01)//	0x01
//			{
//				ReCount=1;
//				RecBuff[0]=ucTemp;
//			}else{
//				ReCount = 0;
//			}
//		}else if(ReCount == 1){
//			if(ucTemp == 0x03)
//			{
//				RecBuff[1] = ucTemp;
////				Total_Len = 23;
//				ReCount ++;
//			}else{
//				ReCount = 0;
//			}
//		}else if(ReCount == 2){
//			if(ucTemp == 0x22)
//			{
//				RecBuff[2] = ucTemp;
//				Total_Len = 39;
//				
//				ReCount ++;
//			}else{
//				ReCount = 0;
//			}
//		}else if(ReCount > 2){
//			RecBuff[ReCount] = ucTemp;
//			ReCount ++;
//			if(ReCount == Total_Len)
//			{
//				uartflag = 1;
//				ReCount = 0;
//			}
//		}
		
	}
}

/**
  * @brief  This function handles OTG_HS Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_HS
void OTG_HS_IRQHandler(void)
{
  USBD_OTG_ISR_Handler (&USB_OTG_dev);
}
#endif

#ifdef USE_USB_OTG_FS  
void OTG_FS_IRQHandler(void)
{
  USBD_OTG_ISR_Handler (&USB_OTG_dev);
}
#endif
#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED 
/**
  * @brief  This function handles EP1_IN Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_IN_IRQHandler(void)
{
  USBD_OTG_EP1IN_ISR_Handler (&USB_OTG_dev);
}

/**
  * @brief  This function handles EP1_OUT Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_OUT_IRQHandler(void)
{
  USBD_OTG_EP1OUT_ISR_Handler (&USB_OTG_dev);
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
