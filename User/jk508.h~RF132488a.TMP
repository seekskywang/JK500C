#include "stm32f4xx.h"
#include "stdlib.h"
#include <string.h>

#ifndef _jk508_h_
#define _jk508_h_

extern u8 op_flag;
extern u8 page_flag;
extern u8 key_value;
extern u8 ch_page;
extern u8 bit_flag;
extern u8 tc_type;
extern float ch1_temp;
extern float offset[40];
extern u16 count;
extern u8 time_buf[10][7];
extern u8 histime[10][7];
extern float range;
extern float enrate;
extern float G_Data[16][500];//温度曲线数据
extern float hisdata[16][500];
extern u8 Data_buf[16][16];
extern u32 rec_num;
extern uint8_t USB_Recive_Buffer[65];
extern uint16_t USB_Received_Count;
extern float TempHLimits[40];
extern float TempLLimits[40];
extern float YLIMIT[3];
extern float Correction[40];
extern float ch_temp[40];
extern u8 RecBuff[40];
extern u8 usbsendbuf[0x40];
extern u8 savedata[80];
extern u8 tempreq[8];
extern u8 usbreadtime[7];
extern u16 his_config[10];
extern u8 reqcode;
//extern u8 usbreadtime;
extern u8 eqmtstatus;
extern u8 cor_page;
extern u8 brightness;
extern u8 charge;
extern u8 battery;
extern u8 usbstatus;
extern __IO uint32_t FlashID;
extern u16 graphcount;
extern u8 reflag;
extern char UsbHidReceiveComplete;


void page_home(void);
void page_set(void);
void page_sys(void);
void page_sep(void);
void page_sysinfo(void);
void page_graph(void);
void page_his(void);
void page_gset(void);
void page_cal(void);
void power_on(void);
void Save_flag(void);
void Read_flag(void);
void Save_history(u16 rec);
void Read_history(u16 rec);
void Save_time(u16 rec);
void Read_time(u16 rec);
void Read_Flash_Init_Handle(void);

u8 udisk_scan(void);
void Delay(__IO u32 nCount);
void minfocus_on(u8 c,float lmt);
void minfocus_off(u8 c,float lmt);
void maxfocus_on(u8 c,float lmt);
void maxfocus_off(u8 c,float lmt);
void yhfocus_on(float lmt);
void yhfocus_off(float lmt);
void ylfocus_on(float lmt);
void ylfocus_off(float lmt);
void multifocus_on(int lmt);
void multifocus_off(int lmt);
void cal_disp(u8 c,float lmt);
void Draw_graph(void);
void InitBrt(void);
void TempDisplay(void);
void Sprint_temp(float temp);
void Check_limits(u8 chn);
void UsbDataHandle(void);
void Save_Sflag(void);
void Read_Sflag(void);
int hex_to_bcd(int data);
void DrawTime(void);
void hispage(u16 page);

extern u8 usbbuf[0x40];
uint16_t CRC16(uint8_t *puchMsg, uint8_t Len);
uint32_t Flash_EnableReadProtection(void);
uint32_t Flash_DisableReadProtection(void);

//历史数据存储配置
#define SECTOR_REC					his_config[0]
#define TIME_REC					his_config[1]
//串口请求命令
#define CODETEMP					0

//USB检测响应
#define NO_CONNECTION				0
#define UDISK_READY					1
#define UDISK_NOTREADY				2

//修正值
#define COR1					   Correction[0]
#define COR2					   Correction[1] 
#define COR3					   Correction[2] 
#define COR4					   Correction[3] 
#define COR5					   Correction[4] 
#define COR6					   Correction[5] 
#define COR7					   Correction[6] 
#define COR8					   Correction[7] 
#define COR9					   Correction[8] 
#define COR10					   Correction[9] 
#define COR11					   Correction[10]
#define COR12					   Correction[11]
#define COR13					   Correction[12] 
#define COR14					   Correction[13] 
#define COR15					   Correction[14] 
#define COR16					   Correction[15] 
#define COR17					   Correction[16] 
#define COR18					   Correction[17] 
#define COR19					   Correction[18] 
#define COR20					   Correction[19] 
#define COR21					   Correction[20]
#define COR22					   Correction[21] 
#define COR23					   Correction[22] 
#define COR24					   Correction[23] 
#define COR25					   Correction[24] 
#define COR26					   Correction[25] 
#define COR27					   Correction[26] 
#define COR28					   Correction[27] 
#define COR29					   Correction[28] 
#define COR30					   Correction[29] 
#define COR31					   Correction[30]
#define COR32					   Correction[31]
#define COR33					   Correction[32] 
#define COR34					   Correction[33] 
#define COR35					   Correction[34] 
#define COR36					   Correction[35] 
#define COR37					   Correction[36] 
#define COR38					   Correction[37] 
#define COR39					   Correction[38] 
#define COR40					   Correction[39] 

//显示温度值
#define CH1TEMP						ch_temp[0]
#define CH2TEMP						ch_temp[1]
#define CH3TEMP						ch_temp[2]
#define CH4TEMP						ch_temp[3]
#define CH5TEMP						ch_temp[4]
#define CH6TEMP						ch_temp[5]
#define CH7TEMP						ch_temp[6]
#define CH8TEMP						ch_temp[7]
#define CH9TEMP						ch_temp[8]
#define CH10TEMP					ch_temp[9]
#define CH11TEMP					ch_temp[10]
#define CH12TEMP					ch_temp[11]
#define CH13TEMP					ch_temp[12]
#define CH14TEMP					ch_temp[13]
#define CH15TEMP					ch_temp[14]
#define CH16TEMP					ch_temp[15]
#define CH17TEMP					ch_temp[16]
#define CH18TEMP					ch_temp[17]
#define CH19TEMP					ch_temp[18]
#define CH20TEMP					ch_temp[19]
#define CH21TEMP					ch_temp[20]
#define CH22TEMP					ch_temp[21]
#define CH23TEMP					ch_temp[22]
#define CH24TEMP					ch_temp[23]
#define CH25TEMP					ch_temp[24]
#define CH26TEMP					ch_temp[25]
#define CH27TEMP					ch_temp[26]
#define CH28TEMP					ch_temp[27]
#define CH29TEMP					ch_temp[28]
#define CH30TEMP					ch_temp[29]
#define CH31TEMP					ch_temp[30]
#define CH32TEMP					ch_temp[31]
#define CH33TEMP					ch_temp[32]
#define CH34TEMP					ch_temp[33]
#define CH35TEMP					ch_temp[34]
#define CH36TEMP					ch_temp[35]
#define CH37TEMP					ch_temp[36]
#define CH38TEMP					ch_temp[37]
#define CH39TEMP					ch_temp[38]
#define CH40TEMP					ch_temp[39]



//通道上下限设置
#define   CH1MIN                    TempLLimits[0]
#define   CH2MIN                    TempLLimits[1]
#define   CH3MIN                    TempLLimits[2]
#define   CH4MIN                    TempLLimits[3]
#define   CH5MIN                    TempLLimits[4]
#define   CH6MIN                    TempLLimits[5]
#define   CH7MIN                    TempLLimits[6]
#define   CH8MIN                    TempLLimits[7]
#define   CH9MIN                    TempLLimits[8]
#define   CH10MIN                   TempLLimits[9]
#define   CH11MIN                   TempLLimits[10]
#define   CH12MIN                   TempLLimits[11]
#define   CH13MIN                   TempLLimits[12]
#define   CH14MIN                   TempLLimits[13]
#define   CH15MIN                   TempLLimits[14]
#define   CH16MIN                   TempLLimits[15]
#define   CH17MIN                   TempLLimits[16]
#define   CH18MIN                   TempLLimits[17]
#define   CH19MIN                   TempLLimits[18]
#define   CH20MIN                   TempLLimits[19]
#define   CH21MIN                   TempLLimits[20]
#define   CH22MIN                   TempLLimits[21]
#define   CH23MIN                   TempLLimits[22]
#define   CH24MIN                   TempLLimits[23]
#define   CH25MIN                   TempLLimits[24]
#define   CH26MIN                   TempLLimits[25]
#define   CH27MIN                   TempLLimits[26]
#define   CH28MIN                   TempLLimits[27]
#define   CH29MIN                   TempLLimits[28]
#define   CH30MIN                   TempLLimits[29]
#define   CH31MIN                   TempLLimits[30]
#define   CH32MIN                   TempLLimits[31]
#define   CH33MIN                   TempLLimits[32]
#define   CH34MIN                   TempLLimits[33]
#define   CH35MIN                   TempLLimits[34]
#define   CH36MIN                   TempLLimits[35]
#define   CH37MIN                   TempLLimits[36]
#define   CH38MIN                   TempLLimits[37]
#define   CH39MIN                   TempLLimits[38]
#define   CH40MIN                   TempLLimits[39]
#define   CH1MAX                    TempHLimits[0]
#define   CH2MAX                    TempHLimits[1]
#define   CH3MAX                    TempHLimits[2]
#define   CH4MAX                    TempHLimits[3]
#define   CH5MAX                    TempHLimits[4]
#define   CH6MAX                    TempHLimits[5]
#define   CH7MAX                    TempHLimits[6]
#define   CH8MAX                    TempHLimits[7]
#define   CH9MAX                    TempHLimits[8]
#define   CH10MAX                   TempHLimits[9]
#define   CH11MAX                   TempHLimits[10]
#define   CH12MAX                   TempHLimits[11]
#define   CH13MAX                   TempHLimits[12]
#define   CH14MAX                   TempHLimits[13]
#define   CH15MAX                   TempHLimits[14]
#define   CH16MAX                   TempHLimits[15]
#define   CH17MAX                   TempHLimits[16]
#define   CH18MAX                   TempHLimits[17]
#define   CH19MAX                   TempHLimits[18]
#define   CH20MAX                   TempHLimits[19]
#define   CH21MAX                   TempHLimits[20]
#define   CH22MAX                   TempHLimits[21]
#define   CH23MAX                   TempHLimits[22]
#define   CH24MAX                   TempHLimits[23]
#define   CH25MAX                   TempHLimits[24]
#define   CH26MAX                   TempHLimits[25]
#define   CH27MAX                   TempHLimits[26]
#define   CH28MAX                   TempHLimits[27]
#define   CH29MAX                   TempHLimits[28]
#define   CH30MAX                   TempHLimits[29]
#define   CH31MAX                   TempHLimits[30]
#define   CH32MAX                   TempHLimits[31]
#define   CH33MAX                   TempHLimits[32]
#define   CH34MAX                   TempHLimits[33]
#define   CH35MAX                   TempHLimits[34]
#define   CH36MAX                   TempHLimits[35]
#define   CH37MAX                   TempHLimits[36]
#define   CH38MAX                   TempHLimits[37]
#define   CH39MAX                   TempHLimits[38]
#define   CH40MAX                   TempHLimits[39]
#define   YHLIMIT                   YLIMIT[0]
#define   YLLIMIT                   YLIMIT[1]
#define   MULTI						YLIMIT[2]


////需要储存的标志位
//#define   CH1_SW             		(FLAG1.BIT_FLAG.SW1)//通道1开关标志位 
//#define   CH2_SW             		(FLAG1.BIT_FLAG.SW2)//通道2开关标志位 
//#define   CH3_SW 					(FLAG1.BIT_FLAG.SW3)//通道3开关标志位 
//#define   CH4_SW 					(FLAG1.BIT_FLAG.SW4)//通道4开关标志位 
//#define   CH5_SW 					(FLAG1.BIT_FLAG.SW5)//通道5开关标志位 
//#define   CH6_SW 					(FLAG1.BIT_FLAG.SW6)//通道6开关标志位 
//#define   CH7_SW       				(FLAG1.BIT_FLAG.SW7)//通道7开关标志位 
//#define   CH8_SW       		    	(FLAG1.BIT_FLAG.SW8)//通道8开关标志位 

//#define   CH9_SW             		(FLAG2.BIT_FLAG.SW9)//通道9开关标志位 
//#define   CH10_SW             		(FLAG2.BIT_FLAG.SW10)//通道10开关标志位 
//#define   CH11_SW 					(FLAG2.BIT_FLAG.SW11)//通道11开关标志位 
//#define   CH12_SW 					(FLAG2.BIT_FLAG.SW12)//通道12开关标志位 
//#define   CH13_SW 					(FLAG2.BIT_FLAG.SW13)//通道13开关标志位 
//#define   CH14_SW 					(FLAG2.BIT_FLAG.SW14)//通道14开关标志位 
//#define   CH15_SW       			(FLAG2.BIT_FLAG.SW15)//通道15开关标志位 
//#define   CH16_SW       		    (FLAG2.BIT_FLAG.SW16)//通道16开关标志位 

//#define   CH17_SW             		(FLAG3.BIT_FLAG.SW17)//通道17开关标志位 
//#define   CH18_SW             		(FLAG3.BIT_FLAG.SW18)//通道18开关标志位 
//#define   CH19_SW 					(FLAG3.BIT_FLAG.SW19)//通道19开关标志位 
//#define   CH20_SW 					(FLAG3.BIT_FLAG.SW20)//通道20开关标志位 
//#define   CH21_SW 					(FLAG3.BIT_FLAG.SW21)//通道21开关标志位 
//#define   CH22_SW 					(FLAG3.BIT_FLAG.SW22)//通道22开关标志位 
//#define   CH23_SW       			(FLAG3.BIT_FLAG.SW23)//通道23开关标志位 
//#define   CH24_SW       		    (FLAG3.BIT_FLAG.SW24)//通道24开关标志位 

//#define   CH25_SW             		(FLAG4.BIT_FLAG.SW25)//通道25开关标志位 
//#define   CH26_SW             		(FLAG4.BIT_FLAG.SW26)//通道26开关标志位 
//#define   CH27_SW 					(FLAG4.BIT_FLAG.SW27)//通道27开关标志位 
//#define   CH28_SW 					(FLAG4.BIT_FLAG.SW28)//通道28开关标志位 
//#define   CH29_SW 					(FLAG4.BIT_FLAG.SW29)//通道29开关标志位 
//#define   CH30_SW 					(FLAG4.BIT_FLAG.SW30)//通道30开关标志位 
//#define   CH31_SW       			(FLAG4.BIT_FLAG.SW31)//通道31开关标志位 
//#define   CH32_SW       		    (FLAG4.BIT_FLAG.SW32)//通道32开关标志位

//#define   CH33_SW             		(FLAG5.BIT_FLAG.SW33)//通道33开关标志位 
//#define   CH34_SW             		(FLAG5.BIT_FLAG.SW34)//通道34开关标志位 
//#define   CH35_SW 					(FLAG5.BIT_FLAG.SW35)//通道35开关标志位 
//#define   CH36_SW 					(FLAG5.BIT_FLAG.SW36)//通道36开关标志位 
//#define   CH37_SW 					(FLAG5.BIT_FLAG.SW37)//通道37开关标志位 
//#define   CH38_SW 					(FLAG5.BIT_FLAG.SW38)//通道38开关标志位 
//#define   CH39_SW       			(FLAG5.BIT_FLAG.SW39)//通道39开关标志位 
//#define   CH40_SW       		    (FLAG5.BIT_FLAG.SW40)//通道40开关标志位

//#define   TCTYPE       		    	(FLAG6.BIT_FLAG.TC)  //热电偶类型标志位
//#define   BEEP       		    	(FLAG6.BIT_FLAG.BP)  //讯响开关标志位
//#define   UNIT       		    	(FLAG6.BIT_FLAG.UT)  //单位标志位
//#define   FILTER       		    	(FLAG6.BIT_FLAG.FT)  //比较标志位

//#define   FONT       		    	(FLAG7.BIT_FLAG.FN)  //字体标志位
//#define   SPEED       		    	(FLAG7.BIT_FLAG.SP)  //速度标志位
//#define   BAUD       		    	(FLAG7.BIT_FLAG.BD)  //波特标志位
//#define   LANG       		    	(FLAG7.BIT_FLAG.LG)  //语言标志位
//#define   BRTS						(FLAG8.BIT_FLAG.BR)  //亮度标志位
//#define   DIM						(FLAG8.BIT_FLAG.DM)  //降低亮度标志位

//需要储存的数据
#define   CH1_SW             		savedata[0]//通道1开关标志位 
#define   CH2_SW             		savedata[1]//通道2开关标志位 
#define   CH3_SW 					savedata[2]//通道3开关标志位 
#define   CH4_SW 					savedata[3]//通道4开关标志位 
#define   CH5_SW 					savedata[4]//通道5开关标志位 
#define   CH6_SW 					savedata[5]//通道6开关标志位 
#define   CH7_SW       				savedata[6]//通道7开关标志位 
#define   CH8_SW       		    	savedata[7]//通道8开关标志位 

#define   CH9_SW             		savedata[8]//通道9开关标志位 
#define   CH10_SW             		savedata[9]//通道10开关标志位 
#define   CH11_SW 					savedata[10]//通道11开关标志位 
#define   CH12_SW 					savedata[11]//通道12开关标志位 
#define   CH13_SW 					savedata[12]//通道13开关标志位 
#define   CH14_SW 					savedata[13]//通道14开关标志位 
#define   CH15_SW       			savedata[14]//通道15开关标志位 
#define   CH16_SW       		    savedata[15]//通道16开关标志位 

#define   CH17_SW             		savedata[16]//通道17开关标志位 
#define   CH18_SW             		savedata[17]//通道18开关标志位 
#define   CH19_SW 					savedata[18]//通道19开关标志位 
#define   CH20_SW 					savedata[19]//通道20开关标志位 
#define   CH21_SW 					savedata[20]//通道21开关标志位 
#define   CH22_SW 					savedata[21]//通道22开关标志位 
#define   CH23_SW       			savedata[22]//通道23开关标志位 
#define   CH24_SW       		    savedata[23]//通道24开关标志位 

#define   CH25_SW             		savedata[24]//通道25开关标志位 
#define   CH26_SW             		savedata[25]//通道26开关标志位 
#define   CH27_SW 					savedata[26]//通道27开关标志位 
#define   CH28_SW 					savedata[27]//通道28开关标志位 
#define   CH29_SW 					savedata[28]//通道29开关标志位 
#define   CH30_SW 					savedata[29]//通道30开关标志位 
#define   CH31_SW       			savedata[30]//通道31开关标志位 
#define   CH32_SW       		    savedata[31]//通道32开关标志位

#define   CH33_SW             		savedata[32]//通道33开关标志位 
#define   CH34_SW             		savedata[33]//通道34开关标志位 
#define   CH35_SW 					savedata[34]//通道35开关标志位 
#define   CH36_SW 					savedata[35]//通道36开关标志位 
#define   CH37_SW 					savedata[36]//通道37开关标志位 
#define   CH38_SW 					savedata[37]//通道38开关标志位 
#define   CH39_SW       			savedata[38]//通道39开关标志位 
#define   CH40_SW       		    savedata[39]//通道40开关标志位

#define   TCTYPE       		    	savedata[41]  //热电偶类型标志位
#define   BEEP       		    	savedata[42]  //讯响开关标志位
#define   UNIT       		    	savedata[43]  //单位标志位
#define   FILTER       		    	savedata[44]  //比较标志位

#define   FONT       		    	savedata[45]  //字体标志位
#define   SPEED       		    	savedata[46]  //速度标志位
#define   BAUD       		    	savedata[47]  //波特标志位
#define   LANG       		    	savedata[48]  //语言标志位
#define   BRTS						savedata[49]  //亮度标志位
#define   DIM						savedata[50]  //降低亮度标志位
#define   CHNUM						savedata[51]//设备通道数

/*USB状态*/
#define UNKNOWN		     2
#define UNCONNECTED		 0
#define CONNECTED		 1
/*亮度标志位*/
#define L0               0
#define L1        	     1
#define L2    	         2
#define L3    	         3
#define L4    	         4

/*降低亮度标志位*/
#define D5               1
#define D10        	     2
#define D15    	         3
#define D30    	         6
#define DOFF    	     0

/* 页面标志位 */
#define display          0
#define sysset           1
#define sysinfo          2
#define graph            3
#define settings         4
#define separation       5
#define graphset         6
#define history			 7
#define calibrate		 8
#define poweron 		 9

/* 选项标志位 */
#define home_type        0
#define home_ch1         1
#define home_ch2         2
#define home_ch3         3
#define home_ch4         4
#define home_ch5         5
#define home_ch6         6
#define home_ch7         7
#define home_ch8         8
#define home_ch9         9
#define home_ch10        10
#define home_ch11        11
#define home_ch12        12
#define home_ch13        13
#define home_ch14        14
#define home_ch15        15
#define home_ch16        16
#define home_ch17        17
#define home_ch18        18
#define home_ch19        19
#define home_ch20        20
#define home_ch21        21
#define home_ch22        22
#define home_ch23        23
#define home_ch24        24
#define home_ch25        25
#define home_ch26        26
#define home_ch27        27
#define home_ch28        28
#define home_ch29        29
#define home_ch30        30
#define home_ch31        31
#define home_ch32        32
#define home_ch33        33
#define home_ch34        34
#define home_ch35        35
#define home_ch36        36
#define home_ch37        37
#define home_ch38        38
#define home_ch39        39
#define home_ch40        40
#define type_1           41
#define type_2           42
#define type_3           43
#define set_filter       44
#define set_beep         45
#define set_unit         46
#define set_chnum        47
#define set_file         48
#define set_spd          49
#define set_baud         50
#define set_timer        51
#define set_font         52
#define set_lang         53
#define set_date         54
#define set_time         55
#define set_min1		 56
#define set_min2		 57
#define set_min3		 58
#define set_min4		 59
#define set_min5		 60
#define set_min6		 61
#define set_min7		 62
#define set_min8		 63
#define set_max1		 64
#define set_max2		 65
#define set_max3		 66
#define set_max4		 67
#define set_max5		 68
#define set_max6		 69
#define set_max7		 70
#define set_max8		 71
#define set_yhigh		 72
#define set_ylow		 73
#define set_multi		 74
#define set_g1		 	 75
#define set_g2		 	 76
#define set_g3		 	 77
#define set_g4		 	 78
#define set_g5		 	 79
#define set_g6		 	 80
#define set_g7		 	 81
#define set_g8		 	 82
#define set_c1		 	 83
#define set_c2		 	 84
#define set_c3		 	 85
#define set_c4		 	 86
#define set_c5		 	 87
#define set_c6		 	 88
#define set_c7		 	 89
#define set_c8		 	 90
#define set_brt			 91//亮度
#define set_dim		     92//降低亮度

/* 字体标志位 */
#define big	             0
#define middle           1
#define small            2

/* 语言标志位 */
#define chs	             0
#define eng              1


/* 传感器标志位 */
#define TCT          	1//T型热电偶
#define TCK          	2//K型热电偶
#define TCJ          	3//J型热电偶
#define TCN          	4//N型热电偶
#define TCE          	5//E型热电偶
#define TCS          	6//S型热电偶
#define TCR          	7//R型热电偶
#define TCB          	8//B型热电偶
#define PT100        	9//PT100型热电偶
#define TCV       	 	10 //电压
#define TCI       	 	11//电流
#define T18B20			16//18B20
#define AM2305			17//温度湿度传感器

/* 页数标志位 */
#define page1          0
#define page2          1
#define page3          2
#define page4          3
#define page5          4
/* 通道开关定义 */
#define ch_on           0
#define ch_off          1
/* 比较开关标志位 */
#define ft_off          0
#define ft_on           1 
/* 讯响开关标志位 */
#define beep_off        0
#define beep_on         1
/* 单位标志位 */
#define C               5
#define F               4
#define K               1
/* 波特标志位 */
#define b9600           0
#define b19200          1
#define b38400          2
#define b57600          3
#define b115200         4
/* 速度标志位 */
#define fast            0
#define moderate        1
#define slow            2
/* 设置选项选中标志位 */
#define op_on           1
#define op_off          0


#endif
