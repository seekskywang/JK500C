/**
  ******************************************************************************
  * @file    fonts.h
  * @author  MCD Application Team
  * @version V5.0.2
  * @date    05-March-2012
  * @brief   Header for fonts.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FONTS_H
#define __FONTS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup Common
  * @{
  */

/** @addtogroup FONTS
  * @{
  */ 

/** @defgroup FONTS_Exported_Types
  * @{
  */ 
//extern const FNT_GB24 Chinese24x24_Table;


/*********************************************************************************************************
汉字字模的数据结构定义
*********************************************************************************************************/
typedef struct {                                                 /* 汉字字模数据结构             */
    unsigned char Index[2];                                             /* 汉字内码索引                 */
    unsigned char Msk[72];                                              /* 点阵码数据                   */
}FNT_GB24;

typedef struct {                                                 /* 汉字字模数据结构             */
    unsigned char Index[2];                                             /* 汉字内码索引                 */
    unsigned char Msk[81];                                              /* 点阵码数据                   */
}FNT_GB27;

typedef struct {                                                 /* 汉字字模数据结构             */
    unsigned char Index[2];                                             /* 汉字内码索引                 */
    unsigned char Msk[108];                                              /* 点阵码数据                   */
}FNT_GB32x27;

typedef struct {                                                 /* 汉字字模数据结构             */
    unsigned char Index[2];                                             /* 汉字内码索引                 */
    unsigned char Msk[124];                                              /* 点阵码数据                   */
}FNT_GB32x31;

typedef struct {                                                 /* 汉字字模数据结构             */
    unsigned char Index[2];                                             /* 汉字内码索引                 */
    unsigned char Msk[128];                                              /* 点阵码数据                   */
}FNT_GB32x32;

typedef struct {                                                 /* 汉字字模数据结构             */
    unsigned char Index[2];                                             /* 汉字内码索引                 */
    unsigned char Msk[57];                                              /* 点阵码数据                   */
}FNT_GB24x19;

typedef struct {                                                 /* 汉字字模数据结构             */
    unsigned char Index[2];                                             /* 汉字内码索引                 */
    unsigned char Msk[32];                                              /* 点阵码数据                   */
}FNT_GB16x16;

typedef struct {                                                 /* 汉字字模数据结构             */
    unsigned char Index[2];                                             /* 汉字内码索引                 */
    unsigned char Msk[36];                                              /* 点阵码数据                   */
}FNT_GB16x20;

extern const FNT_GB24 Chinese24x24_Table[];
extern const FNT_GB27 Chinese24x27_Table[];
extern const FNT_GB32x27 Chinese32x27_Table[];
extern const FNT_GB32x31 Chinese32x31_Table[];
extern const FNT_GB32x32 Chinese32x32_Table[];
extern const FNT_GB24x19 Chinese24x19_Table[];
extern const FNT_GB16x20 Chinese16x20_Table[];
extern const FNT_GB16x16 Chinese16x16_Table[];
extern const FNT_GB16x16 SONG_16x16_Table[];


typedef struct _tFont
{    
  const uint16_t *table;
  uint16_t Width;
  uint16_t Height;
} sFONT;

extern sFONT Font16x24;
extern sFONT Font16x31;
extern sFONT FontC_8x20;
extern sFONT FontC_8x20B;
extern sFONT FontC_16x24;
extern sFONT FontC_16x32;
extern sFONT FontC_8x14;
extern sFONT FontT_24x40;
extern sFONT FontT_24x34;
extern sFONT FontT_16x28;
extern sFONT FontT_16x24;
extern sFONT Font12x12;
extern sFONT Font8x12;
extern sFONT Font8x8;
extern sFONT Font24x24;
extern sFONT C_Font24x27;
//extern sFONT C_Font32x27;
//extern sFONT C_Font32x31;
//extern sFONT C_Font32x32;
extern sFONT C_Font24x19;
extern sFONT C_Font16x16;
extern sFONT C_Font16x20;
extern sFONT FontT_INS;
extern sFONT FontT_SYS;
extern sFONT SONG_Font16x16;
/**
  * @}
  */ 

/** @defgroup FONTS_Exported_Constants
  * @{
  */ 
#define LINE(x) x//((x) * (((sFONT *)LCD_GetFont())->Height))



/*******************中文********** 在显示屏上显示的字符大小 ***************************/
#define      macWIDTH_CH_CHAR		                24	    //中文字符宽度 
#define      macHEIGHT_CH_CHAR		              24		  //中文字符高度 

//0表示使用SD卡字模，非零表示FLASH字模,由于SD卡字模有文件系统，速度慢很多。

#define GBKCODE_FLASH 						1

#if GBKCODE_FLASH
/*使用FLASH字模*/
/*中文字库存储在FLASH的起始地址*/
/*FLASH*/
#define GBKCODE_START_ADDRESS   1360*4096


/*获取字库的函数*/
//定义获取中文字符字模数组的函数名，ucBuffer为存放字模数组名，usChar为中文字符（国标码）
#define      macGetGBKCode( ucBuffer, usChar )  GetGBKCode_from_EXFlash( ucBuffer, usChar )  
int GetGBKCode_from_EXFlash( uint8_t * pBuffer, uint16_t c);

#else
/*使用SD字模*/


/*SD卡字模路径*/
#define GBKCODE_FILE_NAME			"0:/Font/GB2312_H2424.FON"


/*获取字库的函数*/
//定义获取中文字符字模数组的函数名，ucBuffer为存放字模数组名，usChar为中文字符（国标码）

#define macGetGBKCode( ucBuffer, usChar )  GetGBKCode_from_sd( ucBuffer, usChar )
int GetGBKCode_from_sd ( uint8_t * pBuffer, uint16_t c);

#endif

/******************************************************************************/


/**
  * @}
  */ 

/** @defgroup FONTS_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup FONTS_Exported_Functions
  * @{
  */ 
/**
  * @}
  */

#ifdef __cplusplus
}
#endif
  
#endif /* __FONTS_H */
 
/**
  * @}
  */

/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */      

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
