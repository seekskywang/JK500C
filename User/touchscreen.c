/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   触摸屏驱动
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
#include "touchscreen.h"
#include "math.h"
#include "stdlib.h"
#include "./systick/bsp_SysTick.h"

__IO uint16_t ADC_Converted[RHEOSTAT_NOFCHANEL1]={0};
_m_tp_dev tp_dev=
{
	TP_Init,
	TP_Scan,
	TP_Adjust,
	0,
	0, 
	0,
	0,
	0,
	0,	  	 		
	0,
	0,	  	 		
};					
//默认为touchtype=0的数据.
u8 CMD_RDX=0;
u8 CMD_RDY=1;
u16 Xconvert;
u16 Yconvert;
u32 corpara[20];
u16 XCOOR;
u16 YCOOR;
u8 touchflag;
u8 press;
Touch_save_Typedef Touch_save;

 static void set_yz(void){	X_P0;	X_N0;	Y_P1;	Y_N1;}
static void set_yy(void){	X_P0;	X_N1;	Y_P1;	Y_N0;}
static void set_xx(void){	X_P1;	X_N0;	Y_P0;	Y_N1;}
static void set_mid(void){	X_P0;	X_N1;	Y_P1;	Y_N0;}



		 
void Touch_GPIO_Config(void) 
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*开启相关的GPIO外设时钟*/
	RCC_AHB1PeriphClockCmd (TOUCH_XPLUS_GPIO_CLK|TOUCH_XMINUS_GPIO_CLK|TOUCH_YPLUS_GPIO_CLK|
	TOUCH_YMINUS_GPIO_CLK,ENABLE); 

	GPIO_InitStructure.GPIO_Pin = TOUCH_YPLUS_GPIO_PIN|TOUCH_YMINUS_GPIO_PIN;/*选择引脚*/     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;/*设置引脚为输入模式*/   
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; /*设置引脚为上拉模式*/	  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;/*设置引脚速率为2MHz */    
	GPIO_Init(TOUCH_YPLUS_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = TOUCH_XMINUS_GPIO_PIN;/*选择引脚*/     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;/*设置引脚为输出模式*/   
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;/*设置引脚的输出类型为推挽输出*/    
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; /*设置引脚为上拉模式*/	  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;/*设置引脚速率为2MHz */    
	GPIO_Init(TOUCH_XMINUS_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = TOUCH_XPLUS_GPIO_PIN;/*选择引脚*/     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;/*设置引脚为输出模式*/   
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;/*设置引脚的输出类型为推挽输出*/    
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; /*设置引脚为上拉模式*/	  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;/*设置引脚速率为2MHz */    
	GPIO_Init(TOUCH_XPLUS_GPIO_PORT, &GPIO_InitStructure);
	
//	Touch_Up_flag=0;
//	GPIO_ResetBits(TOUCH_XMINUS_GPIO_PORT,TOUCH_XMINUS_GPIO_PIN);
}
static void AD_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	/*=====================通道1======================*/	
	// 使能 GPIO 时钟
	RCC_AHB1PeriphClockCmd(ADC_GPIO_CLK0,ENABLE);		
	// 配置 IO
    GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PIN0|ADC_GPIO_PIN1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    //不上拉不下拉	
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(ADC_GPIO_PORT0, &GPIO_InitStructure);

}
static void Rheostat_ADC_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
  // ------------------DMA Init 结构体参数 初始化--------------------------
  // ADC1使用DMA2，数据流0，通道0，这个是手册固定死的
  // 开启DMA时钟
  RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_DMA_CLK, ENABLE); 
	// 外设基址为：ADC 数据寄存器地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = RHEOSTAT_ADC_DR_ADDR;	
  // 存储器地址，实际上就是一个内部SRAM的变量	
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)ADC_Converted;  
  // 数据传输方向为外设到存储器	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	
	// 缓冲区大小为，指一次传输的数据量
	DMA_InitStructure.DMA_BufferSize = RHEOSTAT_NOFCHANEL1;	
	// 外设寄存器只有一个，地址不用递增
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  // 存储器地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
  // // 外设数据大小为半字，即两个字节 
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; 
  //	存储器数据大小也为半字，跟外设数据大小相同
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	
	// 循环传输模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  // DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  // 禁止DMA FIFO	，使用直连模式
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;  
  // FIFO 大小，FIFO模式禁止时，这个不用配置	
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
	// 选择 DMA 通道，通道存在于流中
  DMA_InitStructure.DMA_Channel = RHEOSTAT_ADC_DMA_CHANNEL; 
  //初始化DMA流，流相当于一个大的管道，管道里面有很多通道
	DMA_Init(RHEOSTAT_ADC_DMA_STREAM, &DMA_InitStructure);
	// 使能DMA流
  DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM, ENABLE);
	
	// 开启ADC时钟
	RCC_APB2PeriphClockCmd(RHEOSTAT_ADC_CLK , ENABLE);
  // -------------------ADC Common 结构体 参数 初始化------------------------
	// 独立ADC模式
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  // 时钟为fpclk x分频	
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
  // 禁止DMA直接访问模式	
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  // 采样时间间隔	
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;  
  ADC_CommonInit(&ADC_CommonInitStructure);
	
  // -------------------ADC Init 结构体 参数 初始化--------------------------
  // ADC 分辨率
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  // 扫描模式，多通道采集需要	
  ADC_InitStructure.ADC_ScanConvMode = ENABLE; 
  // 连续转换	
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
  //禁止外部边沿触发
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  //使用软件触发，外部触发不用配置，注释掉即可
  //ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  //数据右对齐	
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  //转换通道 1个
  ADC_InitStructure.ADC_NbrOfConversion = RHEOSTAT_NOFCHANEL1;                                    
  ADC_Init(RHEOSTAT_ADC, &ADC_InitStructure);
  //---------------------------------------------------------------------------
	
  // 配置 ADC 通道转换顺序和采样时间周期
  ADC_RegularChannelConfig(RHEOSTAT_ADC, ADC_CHANNEL0, 1, 
	                         ADC_SampleTime_144Cycles);
  ADC_RegularChannelConfig(RHEOSTAT_ADC, ADC_CHANNEL1, 2, 
	                         ADC_SampleTime_144Cycles); 

  // 使能DMA请求 after last transfer (Single-ADC mode)
  ADC_DMARequestAfterLastTransferCmd(RHEOSTAT_ADC, ENABLE);
  // 使能ADC DMA
  ADC_DMACmd(RHEOSTAT_ADC, ENABLE);
	
	// 使能ADC
  ADC_Cmd(RHEOSTAT_ADC, ENABLE);  
  //开始adc转换，软件触发
  ADC_SoftwareStartConv(RHEOSTAT_ADC);
}
u16 TP_Read_AD(u8 CMD)	  
{ 	 

	u16 Num=0; 
	if(CMD)
	{
		
		set_yy();
		delay_1us(40);
		Num = ADC_Converted[1];
		

	}else{
		
		set_xx();
		delay_1us(40);
		Num = ADC_Converted[0];
		
	
	}
	return(Num);   
}
//读取一个坐标值(x或者y)
//连续读取READ_TIMES次数据,对这些数据升序排列,
//然后去掉最低和最高LOST_VAL个数,取平均值 
//xy:指令（CMD_RDX/CMD_RDY）
//返回值:读到的数据
#define READ_TIMES 3 	//读取次数
#define LOST_VAL 1	  	//丢弃值
u16 TP_Read_XOY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)
		buf[i]=TP_Read_AD(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 
//读取x,y坐标
//最小值不能少于100.
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
u8 TP_Read_XY(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;	
	//set_mid();	
	xtemp=TP_Read_XOY(CMD_RDX);
	//set_mid();
	ytemp=TP_Read_XOY(CMD_RDY);	 
	//set_mid();	
	if(xtemp<100||ytemp<100)return 0;//读数失败
	*x=xtemp;
	*y=ytemp;
	return 1;//读数成功
}
//连续2次读取触摸屏IC,且这两次的偏差不能超过
//ERR_RANGE,满足条件,则认为读数正确,否则读数错误.	   
//该函数能大大提高准确度
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
#define ERR_RANGE 50 //误差范围 
u8 TP_Read_XY2(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=TP_Read_XY(&x1,&y1);   
    if(flag==0)return(0);
    flag=TP_Read_XY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
		
        return 1;
    }else return 0;	  
}  
//////////////////////////////////////////////////////////////////////////////////		  
//与LCD部分有关的函数  
//画一个触摸点
//用来校准用的
//x,y:坐标
//color:颜色
void TP_Drow_Touch_Point(u16 x,u16 y,u32 color)
{
//	POINT_COLOR=color;
	LCD_SetColors(color,color);
	LCD_DrawLine(x+12,y,25,0);//横线
	LCD_DrawLine(x,y+13,25,1);//竖线
	LCD_DrawUniLine(x,y+1,x,y+1);
	LCD_DrawUniLine(x-2,y+1,x-2,y+1);
	LCD_DrawUniLine(x,y-1,x,y-1);
	LCD_DrawUniLine(x-2,y-1,x-2,y-1);
	LCD_DrawCircle(x,y,6);//画中心圈
}	  
//画一个大点(2*2的点)		   
//x,y:坐标
//color:颜色
void TP_Draw_Big_Point(u16 x,u16 y,u32 color)
{	    
//	POINT_COLOR=color;
	LCD_SetColors(color,color);
	LCD_DrawUniLine(x,y,x,y);//中心点 
	LCD_DrawUniLine(x+1,y,x+1,y);
	LCD_DrawUniLine(x,y+1,x,y+1);
	LCD_DrawUniLine(x+1,y+1,x+1,y+1);	 	  	
}						  
//////////////////////////////////////////////////////////////////////////////////		  
//触摸按键扫描
//tp:0,屏幕坐标;1,物理坐标(校准等特殊场合用)
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
u8 TP_Scan(u8 tp)
{		
	u16 value;
	set_yz(); 
    delay_us(20);
value=ADC_Converted[0];	
	//if(PEN==0)//有按键按下
	if(value>100)
	touchflag=1;
	if(touchflag)//
	{
		touchflag=0;
		if(tp)TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]);//读取物理坐标
		else if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))//读取屏幕坐标
		{
	 		tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;//将结果转换为屏幕坐标
			tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff;  
	 	} 
		if((tp_dev.sta&TP_PRES_DOWN)==0)//之前没有被按下
		{		 
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//按键按下  
			tp_dev.x[4]=tp_dev.x[0];//记录第一次按下时的坐标
			tp_dev.y[4]=480-tp_dev.y[0];  	   			 
		}			   
	}
	else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//之前是被按下的
		{
			tp_dev.sta&=~(1<<7);//标记按键松开	
		}else//之前就没有被按下
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		}	    
	}
	return tp_dev.sta&TP_PRES_DOWN;//返回当前的触屏状态
}	  
//////////////////////////////////////////////////////////////////////////	 
//保存在EEPROM里面的地址区间基址,占用13个字节(RANGE:SAVE_ADDR_BASE~SAVE_ADDR_BASE+12)
#define SAVE_ADDR_BASE 40
//保存校准参数										    
void TP_Save_Adjdata(void)
{
	s32 temp;			 
	//保存校正结果!		   							  
	Touch_save.x_pos=tp_dev.xfac*100000000;//保存x校正因素      
      
	Touch_save.y_pos=tp_dev.yfac*100000000;//保存y校正因素    
    
	//保存x偏移量
	Touch_save.xoff=tp_dev.xoff;
    		    
	//保存y偏移量
	Touch_save.yoff=tp_dev.yoff;	
	//保存触屏类型
	Touch_save.save_flag=0x0a;
	Save_flag();
}
//得到保存在EEPROM里面的校准值
//返回值：1，成功获取数据
//        0，获取失败，要重新校准
u8 TP_Get_Adjdata(void)
{					  
	s32 tempfac;
	tempfac=Touch_save.save_flag;//读取标记字,看是否校准过！ 		 
	if(tempfac==0X0A)//触摸屏已经校准过了			   
	{    												 
		tempfac=Touch_save.x_pos;		   
		tp_dev.xfac=(float)tempfac/100000000;//得到x校准参数
		tempfac=Touch_save.y_pos;			          
		tp_dev.yfac=(float)tempfac/100000000;//得到y校准参数
	    //得到x偏移量
		tp_dev.xoff=Touch_save.xoff;			   	  
 	    //得到y偏移量
		tp_dev.yoff=Touch_save.yoff;				 	  
 		//tp_dev.touchtype=AT24CXX_ReadOneByte(SAVE_ADDR_BASE+12);//读取触屏类型标记
//		if(tp_dev.touchtype)//X,Y方向与屏幕相反
//		{
//			CMD_RDX=0X90;
//			CMD_RDY=0XD0;	 
//		}else				   //X,Y方向与屏幕相同
//		{
//			CMD_RDX=0XD0;
//			CMD_RDY=0X90;	 
//		}		 
		return 1;	 
	}
	return 0;
}	 
//提示字符串
u8* const TP_REMIND_MSG_TBL="Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";
 					  
//提示校准结果(各个参数)
void TP_Adj_Info_Show(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 x3,u16 y3,u16 fac)
{	  
//	POINT_COLOR=RED;
//	LCD_ShowString(40,160,lcddev.width,lcddev.height,16,"x1:");
// 	LCD_ShowString(40+80,160,lcddev.width,lcddev.height,16,"y1:");
// 	LCD_ShowString(40,180,lcddev.width,lcddev.height,16,"x2:");
// 	LCD_ShowString(40+80,180,lcddev.width,lcddev.height,16,"y2:");
//	LCD_ShowString(40,200,lcddev.width,lcddev.height,16,"x3:");
// 	LCD_ShowString(40+80,200,lcddev.width,lcddev.height,16,"y3:");
//	LCD_ShowString(40,220,lcddev.width,lcddev.height,16,"x4:");
// 	LCD_ShowString(40+80,220,lcddev.width,lcddev.height,16,"y4:");  
// 	LCD_ShowString(40,240,lcddev.width,lcddev.height,16,"fac is:");     
//	LCD_ShowNum(40+24,160,x0,4,16);		//显示数值
//	LCD_ShowNum(40+24+80,160,y0,4,16);	//显示数值
//	LCD_ShowNum(40+24,180,x1,4,16);		//显示数值
//	LCD_ShowNum(40+24+80,180,y1,4,16);	//显示数值
//	LCD_ShowNum(40+24,200,x2,4,16);		//显示数值
//	LCD_ShowNum(40+24+80,200,y2,4,16);	//显示数值
//	LCD_ShowNum(40+24,220,x3,4,16);		//显示数值
//	LCD_ShowNum(40+24+80,220,y3,4,16);	//显示数值
// 	LCD_ShowNum(40+56,240,fac,3,16); 	//显示数值,该数值必须在95~105范围之内.

}
		 
//触摸屏校准代码
//得到四个校准参数
void TP_Adjust(void)
{								 
	u16 pos_temp[4][2];//坐标缓存值
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	double fac; 	
	u16 outtime=0;
 	cnt=0;				
//	POINT_COLOR=BLUE;
//	BACK_COLOR =WHITE;
	LCD_Clear(LCD_COLOR_WHITE);//清屏   
//	POINT_COLOR=RED;//红色 
//	LCD_Clear(WHITE);//清屏 	   
//	POINT_COLOR=BLACK;
//	LCD_ShowString(40,40,160,100,16,(u8*)TP_REMIND_MSG_TBL);//显示提示信息
	TP_Drow_Touch_Point(20,20,LCD_COLOR_RED);//画点1 
	tp_dev.sta=0;//消除触发信号 
	tp_dev.xfac=0;//xfac用来标记是否校准过,所以校准之前必须清掉!以免错误	 
	while(1)//如果连续10秒钟没有按下,则自动退出
	{
		tp_dev.scan(1);//扫描物理坐标
		if((tp_dev.sta&0xc0)==TP_CATH_PRES)//按键按下了一次(此时按键松开了.)
		{	
			outtime=0;		
			tp_dev.sta&=~(1<<6);//标记按键已经被处理过了.
						   			   
			pos_temp[cnt][0]=tp_dev.x[0];
			pos_temp[cnt][1]=tp_dev.y[0];
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					TP_Drow_Touch_Point(20,20,LCD_COLOR_WHITE );	 			//清除点1 
					TP_Drow_Touch_Point(LCD_PIXEL_WIDTH-20,20,LCD_COLOR_RED);	//画点2
					break;
				case 2:
 					TP_Drow_Touch_Point(LCD_PIXEL_WIDTH-20,20,LCD_COLOR_WHITE);	//清除点2
					TP_Drow_Touch_Point(20,LCD_PIXEL_HEIGHT-20,LCD_COLOR_RED);	//画点3
					break;
				case 3:
 					TP_Drow_Touch_Point(20,LCD_PIXEL_HEIGHT-20,LCD_COLOR_WHITE);			//清除点3
 					TP_Drow_Touch_Point(LCD_PIXEL_WIDTH-20,LCD_PIXEL_HEIGHT-20,LCD_COLOR_RED);	//画点4
					break;
				case 4:	 //全部四个点已经得到
	    		    //对边相等
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,2的距离
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到3,4的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)//不合格
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(LCD_PIXEL_WIDTH-20,LCD_PIXEL_HEIGHT-20,LCD_COLOR_WHITE);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,LCD_COLOR_RED);								//画点1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
 						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,3的距离
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,4的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
//						cnt=0;
// 				    	TP_Drow_Touch_Point(LCD_PIXEL_WIDTH-20,LCD_PIXEL_HEIGHT,LCD_COLOR_WHITE);	//清除点4
//   	 					TP_Drow_Touch_Point(20,20,LCD_COLOR_RED);								//画点1
// 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
//						continue;
					}//正确了
								   
					//对角线相等
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,4的距离
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,3的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;

						TP_Drow_Touch_Point(LCD_PIXEL_WIDTH-20,LCD_PIXEL_HEIGHT-20,LCD_COLOR_WHITE);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,LCD_COLOR_RED);	
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
						continue;
					}//正确了
					//计算结果
					tp_dev.xfac=(float)(LCD_PIXEL_WIDTH-40)/(pos_temp[1][0]-pos_temp[0][0]);//得到xfac		 
					tp_dev.xoff=(LCD_PIXEL_WIDTH-tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//得到xoff
						  
					tp_dev.yfac=(float)(LCD_PIXEL_HEIGHT-40)/(pos_temp[2][1]-pos_temp[0][1]);//得到yfac
					tp_dev.yoff=(LCD_PIXEL_HEIGHT-tp_dev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//得到yoff  
					if(fabs(tp_dev.xfac)>2||fabs(tp_dev.yfac)>2)//触屏和预设的相反了.
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(LCD_PIXEL_WIDTH-20,LCD_PIXEL_HEIGHT-20,LCD_COLOR_WHITE);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,LCD_COLOR_RED);								//画点1
						//LCD_ShowString(40,26,lcddev.width,lcddev.height,16,"TP Need readjust!");
//						tp_dev.touchtype=!tp_dev.touchtype;//修改触屏类型.
//						if(tp_dev.touchtype)//X,Y方向与屏幕相反
//						{
//							CMD_RDX=0;
//							CMD_RDY=1;	 
//						}else				   //X,Y方向与屏幕相同
//						{
//							CMD_RDX=1;
//							CMD_RDY=0;	 
//						}		
//						page_home();
						continue;
					}		
//					POINT_COLOR=BLUE;
//					LCD_Clear(WHITE);//清屏
//					LCD_ShowString(35,110,lcddev.width,lcddev.height,16,"Touch Screen Adjust OK!");//校正完成
//					delay_ms(1000);
					TP_Save_Adjdata();  
					
// 					LCD_Clear(WHITE);//清屏   
					return;//校正完成				 
			}
		}
		delay_ms(10);
		outtime++;
		if(outtime>1000)
		{
			TP_Get_Adjdata();
			break;
	 	} 
 	}
}	
//触摸屏初始化  		    
//返回值:0,没有进行校准
//       1,进行过校准
u8 TP_Init(void)
{
//  GPIO_InitTypeDef  GPIO_InitStructure;	
	AD_GPIO_Config();
	Rheostat_ADC_Mode_Config();
	

		
   
		TP_Read_XY(&tp_dev.x[0],&tp_dev.y[0]);//第一次读取初始化	 

//		if(TP_Get_Adjdata())return 0;//已经校准
//		else			   //未校准?
//		{ 										    //
////			LCD_Clear(WHITE);//清屏
//			TP_Adjust();  	//屏幕校准 
//			TP_Save_Adjdata();	 
//		}			
		TP_Get_Adjdata();	
//	}
	return 1; 									 
}
void TouchHandle(u16 x,u16 y)
{

//	if(touchflag == 1 && press == 0)
//	{
		
		switch(page_flag)
		{
			case display:
			{
				if(x >= FONTX1 && x <= FONTX2 && y >= FONTY1 && y <= FONTY2)
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
					op_sw = op_on;
					press = 1;
					ButtonSound();//按键音
				}
				if(x >= TCTYPEX1 && x <= TCTYPEX2 && y >= TCTYPEY1 && y <= TCTYPEY2)
				{
					DrawMenu();//重绘菜单栏
					LCD_SetTextColor(LCD_COLOR_YELLOW);
					LCD_SetBackColor(LCD_COLOR_BLACK);
					LCD_DisplayStringLine(450,35,"TC-T");
					LCD_DisplayStringLine(450,157,"TC-K");
					LCD_DisplayStringLine(450,295,"TC-J");
					LCD_DisplayStringLine(450,420,"TC-N");
					if(LANG == chs)
					{
						LCD_DisplayStringLine(450,550,"更多");
					}else{
						LCD_DisplayStringLine(450,550,"MORE");
					}
					if(LANG == chs)
					{
						DrawInstruction("热电偶型号选择");
					}else{
						DrawInstruction("Thermocouple model");
					}
					op_flag = type_1;
					press = 1;
					ButtonSound();//按键音
				}
				
				if(x >= MENU1X1 && x <= MENU1X2 && y >= MENUY1 && y <= MENUY2)
				{
					switch(op_flag)
					{
						case type_1:
						{
							DrawMenu();//重绘菜单栏
							Drawhomemenu();
							LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BLUE);
							LCD_DrawFullRect(96,47,100,31);
							LCD_SetTextColor(LCD_COLOR_YELLOW);
							LCD_SetBackColor(LCD_COLOR_BACK);					
							LCD_DisplayStringLine(47,100,"TC-T");
							op_flag = home_type;
							TCTYPE = TCT;
							Save_flag();
							}break;
						case type_2:
						{
							DrawMenu();//重绘菜单栏
							Drawhomemenu();
							LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BLUE);
							LCD_DrawFullRect(96,47,100,31);
							LCD_SetTextColor(LCD_COLOR_YELLOW);
							LCD_SetBackColor(LCD_COLOR_BACK);
							LCD_DisplayStringLine(47,100,"TC-E");
							op_flag = home_type;
							TCTYPE = TCE;
							Save_flag();					
						}break;
						case type_3:
						{
							DrawMenu();//重绘菜单栏
							Drawhomemenu();
							LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BLUE);
							LCD_DrawFullRect(96,47,100,31);
							LCD_SetTextColor(LCD_COLOR_YELLOW);
							LCD_SetBackColor(LCD_COLOR_BACK);
							LCD_DisplayStringLine(47,100,"PT100");
							op_flag = home_type;
							TCTYPE = PT100;
							Save_flag();
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
						default:
						{
							page_home();
						}break;
					}
					press = 1;
					ButtonSound();//按键音
				}
				if(x >= MENU2X1 && x <= MENU2X2 && y >= MENUY1 && y <= MENUY2)
				{
					switch(op_flag)
					{
						case type_1:
						{
							DrawMenu();//重绘菜单栏
							Drawhomemenu();
							LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BLUE);
							LCD_DrawFullRect(96,47,100,31);
							LCD_SetTextColor(LCD_COLOR_YELLOW);
							LCD_SetBackColor(LCD_COLOR_BACK);
							LCD_DisplayStringLine(47,100,"TC-K");
							op_flag = home_type;
							TCTYPE = TCK;
							Save_flag();
						}break;
						case type_2:
						{
							DrawMenu();//重绘菜单栏
							Drawhomemenu();
							LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BLUE);
							LCD_DrawFullRect(96,47,100,31);
							LCD_SetTextColor(LCD_COLOR_YELLOW);
							LCD_SetBackColor(LCD_COLOR_BACK);
							LCD_DisplayStringLine(47,100,"TC-S");
							op_flag = home_type;
							TCTYPE = TCS;
							Save_flag();
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
						default:
						{
							page_graph();
						}break;
					}
					press = 1;
					ButtonSound();//按键音
				}
				if(x >= MENU3X1 && x <= MENU3X2 && y >= MENUY1 && y <= MENUY2)
				{
					switch(op_flag)
					{
						case type_1:
						{
							DrawMenu();//重绘菜单栏
							Drawhomemenu();
							LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BLUE);
							LCD_DrawFullRect(96,47,100,31);
							LCD_SetTextColor(LCD_COLOR_YELLOW);
							LCD_SetBackColor(LCD_COLOR_BACK);
							LCD_DisplayStringLine(47,100,"TC-J");
							op_flag = home_type;
							TCTYPE = TCJ;
							Save_flag();
						}break;
						case type_2:
						{
							DrawMenu();//重绘菜单栏
							Drawhomemenu();
							LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BLUE);
							LCD_DrawFullRect(96,47,100,31);
							LCD_SetTextColor(LCD_COLOR_YELLOW);
							LCD_SetBackColor(LCD_COLOR_BACK);
							LCD_DisplayStringLine(47,100,"TC-R");
							op_flag = home_type;
							TCTYPE = TCR;
							Save_flag();
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
						default:
						{
							page_sys();
						}break;
					}
					press = 1;
					ButtonSound();//按键音
				}
				if(x >= MENU4X1 && x <= MENU4X2 && y >= MENUY1 && y <= MENUY2)
				{
					switch(op_flag)
					{
						case type_1:
						{
							DrawMenu();//重绘菜单栏
							Drawhomemenu();
							LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BLUE);
							LCD_DrawFullRect(96,47,100,31);
							LCD_SetTextColor(LCD_COLOR_YELLOW);
							LCD_SetBackColor(LCD_COLOR_BACK);
							LCD_DisplayStringLine(47,100,"TC-N");
							op_flag = home_type;
							TCTYPE = TCN;
							Save_flag();
						}break;
						case type_2:
						{
							DrawMenu();//重绘菜单栏
							Drawhomemenu();
							LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BLUE);
							LCD_DrawFullRect(96,47,100,31);
							LCD_SetTextColor(LCD_COLOR_YELLOW);
							LCD_SetBackColor(LCD_COLOR_BACK);
							LCD_DisplayStringLine(47,100,"TC-B");
							op_flag = home_type;
							TCTYPE = TCB;
							Save_flag();
						}break;
						default:
						{
							page_set();
						}break;
					}
					press = 1;
					ButtonSound();//按键音
				}
				if(x >= MENU5X1 && x <= MENU5X2 && y >= MENUY1 && y <= MENUY2)
				{
					switch(op_flag)
					{
						case type_1:
						{
							DrawMenu();//重绘菜单栏
							LCD_SetTextColor(LCD_COLOR_YELLOW);
							LCD_SetBackColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine(450,35,"TC-E");
							LCD_DisplayStringLine(450,157,"TC-S");
							LCD_DisplayStringLine(450,295,"TC-R");
							LCD_DisplayStringLine(450,420,"TC-B");
							if(LANG == chs)
							{
								LCD_DisplayStringLine(450,550,"更多");
							}else{
								LCD_DisplayStringLine(450,550,"MORE");
							}
							op_flag = type_2;						
						}break;
						case type_2:
						{
//							DrawMenu();//重绘菜单栏
//							LCD_SetTextColor(LCD_COLOR_YELLOW);
//							LCD_SetBackColor(LCD_COLOR_BLACK);
//							LCD_DisplayStringLine(450,35,"PT100");
//							if(LANG == chs)
//							{
//								LCD_DisplayStringLine(450,550,"更多");
//							}else{
//								LCD_DisplayStringLine(450,550,"MORE");
//							}
//							op_flag = type_3;
							DrawMenu();//重绘菜单栏
							LCD_SetTextColor(LCD_COLOR_YELLOW);
							LCD_SetBackColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine(450,35,"TC-T");
							LCD_DisplayStringLine(450,157,"TC-K");
							LCD_DisplayStringLine(450,295,"TC-J");
							LCD_DisplayStringLine(450,420,"TC-N");
							if(LANG == chs)
							{
								LCD_DisplayStringLine(450,550,"更多");
							}else{
								LCD_DisplayStringLine(450,550,"MORE");
							}
							op_flag = type_1;							
						}break;
//						case type_3:
//						{
//							DrawMenu();//重绘菜单栏
//							LCD_SetTextColor(LCD_COLOR_YELLOW);
//							LCD_SetBackColor(LCD_COLOR_BLACK);
//							LCD_DisplayStringLine(450,35,"TC-T");
//							LCD_DisplayStringLine(450,157,"TC-K");
//							LCD_DisplayStringLine(450,295,"TC-J");
//							LCD_DisplayStringLine(450,420,"TC-N");
//							if(LANG == chs)
//							{
//								LCD_DisplayStringLine(450,550,"更多");
//							}else{
//								LCD_DisplayStringLine(450,550,"MORE");
//							}
//							op_flag = type_1;						
//						}break;						
					}
					press = 1;
					ButtonSound();//按键音
				}
				if(FONT == big)
				{
					if(x >= BIGCHLX1 && x <= BIGCHLX2 && y >= BIGCH1Y1 && y <= BIGCH1Y2)
					{
						
						if(ch_page == page1)
						{
							if(CH1_SW == ch_on)
							{
								CH1_SW = ch_off;
							}else{
								CH1_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH9_SW == ch_on)
							{
								CH9_SW = ch_off;
							}else{
								CH9_SW = ch_on;
							}
						}else if(ch_page == page3){
							if(CH17_SW == ch_on)
							{
								CH17_SW = ch_off;
							}else{
								CH17_SW = ch_on;
							}
						}else if(ch_page == page4){
							if(CH25_SW == ch_on)
							{
								CH25_SW = ch_off;
							}else{
								CH25_SW = ch_on;
							}
						}else if(ch_page == page5){
							if(CH33_SW == ch_on)
							{
								CH33_SW = ch_off;
							}else{
								CH33_SW = ch_on;
							}
						}
						ChannelSwtichBig();
					}else if(x >= BIGCHLX1 && x <= BIGCHLX2 && y >= BIGCH2Y1 && y <= BIGCH2Y2){
						if(ch_page == page1)
						{
							if(CH2_SW == ch_on)
							{
								CH2_SW = ch_off;
							}else{
								CH2_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH10_SW == ch_on)
							{
								CH10_SW = ch_off;
							}else{
								CH10_SW = ch_on;
							}
						}else if(ch_page == page3){
							if(CH18_SW == ch_on)
							{
								CH18_SW = ch_off;
							}else{
								CH18_SW = ch_on;
							}
						}else if(ch_page == page4){
							if(CH26_SW == ch_on)
							{
								CH26_SW = ch_off;
							}else{
								CH26_SW = ch_on;
							}
						}else if(ch_page == page5){
							if(CH34_SW == ch_on)
							{
								CH34_SW = ch_off;
							}else{
								CH34_SW = ch_on;
							}
						}
						ChannelSwtichBig();
					}else if(x >= BIGCHLX1 && x <= BIGCHLX2 && y >= BIGCH3Y1 && y <= BIGCH3Y2){
						if(ch_page == page1)
						{
							if(CH3_SW == ch_on)
							{
								CH3_SW = ch_off;
							}else{
								CH3_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH11_SW == ch_on)
							{
								CH11_SW = ch_off;
							}else{
								CH11_SW = ch_on;
							}
						}else if(ch_page == page3){
							if(CH19_SW == ch_on)
							{
								CH19_SW = ch_off;
							}else{
								CH19_SW = ch_on;
							}
						}else if(ch_page == page4){
							if(CH27_SW == ch_on)
							{
								CH27_SW = ch_off;
							}else{
								CH27_SW = ch_on;
							}
						}else if(ch_page == page5){
							if(CH35_SW == ch_on)
							{
								CH35_SW = ch_off;
							}else{
								CH35_SW = ch_on;
							}
						}
						ChannelSwtichBig();
					}else if(x >= BIGCHLX1 && x <= BIGCHLX2 && y >= BIGCH4Y1 && y <= BIGCH4Y2){
						if(ch_page == page1)
						{
							if(CH4_SW == ch_on)
							{
								CH4_SW = ch_off;
							}else{
								CH4_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH12_SW == ch_on)
							{
								CH12_SW = ch_off;
							}else{
								CH12_SW = ch_on;
							}
						}else if(ch_page == page3){
							if(CH20_SW == ch_on)
							{
								CH20_SW = ch_off;
							}else{
								CH20_SW = ch_on;
							}
						}else if(ch_page == page4){
							if(CH28_SW == ch_on)
							{
								CH28_SW = ch_off;
							}else{
								CH28_SW = ch_on;
							}
						}else if(ch_page == page5){
							if(CH36_SW == ch_on)
							{
								CH36_SW = ch_off;
							}else{
								CH36_SW = ch_on;
							}
						}
						ChannelSwtichBig();
					}else if(x >= BIGCHRX1 && x <= BIGCHRX2 && y >= BIGCH1Y1 && y <= BIGCH1Y2)
					{
						if(ch_page == page1)
						{
							if(CH5_SW == ch_on)
							{
								CH5_SW = ch_off;
							}else{
								CH5_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH13_SW == ch_on)
							{
								CH13_SW = ch_off;
							}else{
								CH13_SW = ch_on;
							}
						}else if(ch_page == page3){
							if(CH21_SW == ch_on)
							{
								CH21_SW = ch_off;
							}else{
								CH21_SW = ch_on;
							}
						}else if(ch_page == page4){
							if(CH29_SW == ch_on)
							{
								CH29_SW = ch_off;
							}else{
								CH29_SW = ch_on;
							}
						}else if(ch_page == page5){
							if(CH37_SW == ch_on)
							{
								CH37_SW = ch_off;
							}else{
								CH37_SW = ch_on;
							}
						}
						ChannelSwtichBig();
					}else if(x >= BIGCHRX1 && x <= BIGCHRX2 && y >= BIGCH2Y1 && y <= BIGCH2Y2){
						if(ch_page == page1)
						{
							if(CH6_SW == ch_on)
							{
								CH6_SW = ch_off;
							}else{
								CH6_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH14_SW == ch_on)
							{
								CH14_SW = ch_off;
							}else{
								CH14_SW = ch_on;
							}
						}else if(ch_page == page3){
							if(CH22_SW == ch_on)
							{
								CH22_SW = ch_off;
							}else{
								CH22_SW = ch_on;
							}
						}else if(ch_page == page4){
							if(CH30_SW == ch_on)
							{
								CH30_SW = ch_off;
							}else{
								CH30_SW = ch_on;
							}
						}else if(ch_page == page5){
							if(CH38_SW == ch_on)
							{
								CH38_SW = ch_off;
							}else{
								CH38_SW = ch_on;
							}
						}
						ChannelSwtichBig();
					}else if(x >= BIGCHRX1 && x <= BIGCHRX2 && y >= BIGCH3Y1 && y <= BIGCH3Y2){
						if(ch_page == page1)
						{
							if(CH7_SW == ch_on)
							{
								CH7_SW = ch_off;
							}else{
								CH7_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH15_SW == ch_on)
							{
								CH15_SW = ch_off;
							}else{
								CH15_SW = ch_on;
							}
						}else if(ch_page == page3){
							if(CH23_SW == ch_on)
							{
								CH23_SW = ch_off;
							}else{
								CH23_SW = ch_on;
							}
						}else if(ch_page == page4){
							if(CH31_SW == ch_on)
							{
								CH31_SW = ch_off;
							}else{
								CH31_SW = ch_on;
							}
						}else if(ch_page == page5){
							if(CH39_SW == ch_on)
							{
								CH39_SW = ch_off;
							}else{
								CH39_SW = ch_on;
							}
						}
						ChannelSwtichBig();
					}else if(x >= BIGCHRX1 && x <= BIGCHRX2 && y >= BIGCH4Y1 && y <= BIGCH4Y2){
						if(ch_page == page1)
						{
							if(CH8_SW == ch_on)
							{
								CH8_SW = ch_off;
							}else{
								CH8_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH16_SW == ch_on)
							{
								CH16_SW = ch_off;
							}else{
								CH16_SW = ch_on;
							}
						}else if(ch_page == page3){
							if(CH24_SW == ch_on)
							{
								CH24_SW = ch_off;
							}else{
								CH24_SW = ch_on;
							}
						}else if(ch_page == page4){
							if(CH32_SW == ch_on)
							{
								CH32_SW = ch_off;
							}else{
								CH32_SW = ch_on;
							}
						}else if(ch_page == page5){
							if(CH40_SW == ch_on)
							{
								CH40_SW = ch_off;
							}else{
								CH40_SW = ch_on;
							}
						}
						ChannelSwtichBig();
					}
				}else if(FONT == middle){
					if(x >= MIDCHLX1 && x <= MIDCHLX2 && y >= MIDCH1Y1 && y <= MIDCH1Y2)
					{
						
						if(ch_page == page1)
						{
							if(CH1_SW == ch_on)
							{
								CH1_SW = ch_off;
							}else{
								CH1_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH25_SW == ch_on)
							{
								CH25_SW = ch_off;
							}else{
								CH25_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHLX1 && x <= MIDCHLX2 && y >= MIDCH2Y1 && y <= MIDCH2Y2){
						if(ch_page == page1)
						{
							if(CH2_SW == ch_on)
							{
								CH2_SW = ch_off;
							}else{
								CH2_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH26_SW == ch_on)
							{
								CH26_SW = ch_off;
							}else{
								CH26_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHLX1 && x <= MIDCHLX2 && y >= MIDCH3Y1 && y <= MIDCH3Y2){
						if(ch_page == page1)
						{
							if(CH3_SW == ch_on)
							{
								CH3_SW = ch_off;
							}else{
								CH3_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH27_SW == ch_on)
							{
								CH27_SW = ch_off;
							}else{
								CH27_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHLX1 && x <= MIDCHLX2 && y >= MIDCH4Y1 && y <= MIDCH4Y2){
						if(ch_page == page1)
						{
							if(CH4_SW == ch_on)
							{
								CH4_SW = ch_off;
							}else{
								CH4_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH28_SW == ch_on)
							{
								CH28_SW = ch_off;
							}else{
								CH28_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHLX1 && x <= MIDCHLX2 && y >= MIDCH5Y1 && y <= MIDCH5Y2)
					{
						if(ch_page == page1)
						{
							if(CH5_SW == ch_on)
							{
								CH5_SW = ch_off;
							}else{
								CH5_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH29_SW == ch_on)
							{
								CH29_SW = ch_off;
							}else{
								CH29_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHLX1 && x <= MIDCHLX2 && y >= MIDCH6Y1 && y <= MIDCH6Y2){
						if(ch_page == page1)
						{
							if(CH6_SW == ch_on)
							{
								CH6_SW = ch_off;
							}else{
								CH6_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH30_SW == ch_on)
							{
								CH30_SW = ch_off;
							}else{
								CH30_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHLX1 && x <= MIDCHLX2 && y >= MIDCH7Y1 && y <= MIDCH7Y2){
						if(ch_page == page1)
						{
							if(CH7_SW == ch_on)
							{
								CH7_SW = ch_off;
							}else{
								CH7_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH31_SW == ch_on)
							{
								CH31_SW = ch_off;
							}else{
								CH31_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHLX1 && x <= MIDCHLX2 && y >= MIDCH8Y1 && y <= MIDCH8Y2){
						if(ch_page == page1)
						{
							if(CH8_SW == ch_on)
							{
								CH8_SW = ch_off;
							}else{
								CH8_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH32_SW == ch_on)
							{
								CH32_SW = ch_off;
							}else{
								CH32_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHMX1 && x <= MIDCHMX2 && y >= MIDCH1Y1 && y <= MIDCH1Y2 && CHNUM > 8)
					{
						
						if(ch_page == page1)
						{
							if(CH9_SW == ch_on)
							{
								CH9_SW = ch_off;
							}else{
								CH9_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH33_SW == ch_on)
							{
								CH33_SW = ch_off;
							}else{
								CH33_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHMX1 && x <= MIDCHMX2 && y >= MIDCH2Y1 && y <= MIDCH2Y2 && CHNUM > 8){
						if(ch_page == page1)
						{
							if(CH10_SW == ch_on)
							{
								CH10_SW = ch_off;
							}else{
								CH10_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH34_SW == ch_on)
							{
								CH34_SW = ch_off;
							}else{
								CH34_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHMX1 && x <= MIDCHMX2 && y >= MIDCH3Y1 && y <= MIDCH3Y2 && CHNUM > 8){
						if(ch_page == page1)
						{
							if(CH11_SW == ch_on)
							{
								CH11_SW = ch_off;
							}else{
								CH11_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH35_SW == ch_on)
							{
								CH35_SW = ch_off;
							}else{
								CH35_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHMX1 && x <= MIDCHMX2 && y >= MIDCH4Y1 && y <= MIDCH4Y2 && CHNUM > 8){
						if(ch_page == page1)
						{
							if(CH12_SW == ch_on)
							{
								CH12_SW = ch_off;
							}else{
								CH12_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH36_SW == ch_on)
							{
								CH36_SW = ch_off;
							}else{
								CH36_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHMX1 && x <= MIDCHMX2 && y >= MIDCH5Y1 && y <= MIDCH5Y2 && CHNUM > 8)
					{
						if(ch_page == page1)
						{
							if(CH13_SW == ch_on)
							{
								CH13_SW = ch_off;
							}else{
								CH13_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH37_SW == ch_on)
							{
								CH37_SW = ch_off;
							}else{
								CH37_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHMX1 && x <= MIDCHMX2 && y >= MIDCH6Y1 && y <= MIDCH6Y2 && CHNUM > 8){
						if(ch_page == page1)
						{
							if(CH14_SW == ch_on)
							{
								CH14_SW = ch_off;
							}else{
								CH14_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH38_SW == ch_on)
							{
								CH38_SW = ch_off;
							}else{
								CH38_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHMX1 && x <= MIDCHMX2 && y >= MIDCH7Y1 && y <= MIDCH7Y2 && CHNUM > 8){
						if(ch_page == page1)
						{
							if(CH15_SW == ch_on)
							{
								CH15_SW = ch_off;
							}else{
								CH15_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH39_SW == ch_on)
							{
								CH39_SW = ch_off;
							}else{
								CH39_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHMX1 && x <= MIDCHMX2 && y >= MIDCH8Y1 && y <= MIDCH8Y2 && CHNUM > 8){
						if(ch_page == page1)
						{
							if(CH16_SW == ch_on)
							{
								CH16_SW = ch_off;
							}else{
								CH16_SW = ch_on;
							}
						}else if(ch_page == page2){
							if(CH40_SW == ch_on)
							{
								CH40_SW = ch_off;
							}else{
								CH40_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHRX1 && x <= MIDCHRX2 && y >= MIDCH1Y1 && y <= MIDCH1Y2 && CHNUM > 16)
					{
						
						if(ch_page == page1)
						{
							if(CH17_SW == ch_on)
							{
								CH17_SW = ch_off;
							}else{
								CH17_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHRX1 && x <= MIDCHRX2 && y >= MIDCH2Y1 && y <= MIDCH2Y2 && CHNUM > 16){
						if(ch_page == page1)
						{
							if(CH18_SW == ch_on)
							{
								CH18_SW = ch_off;
							}else{
								CH18_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHRX1 && x <= MIDCHRX2 && y >= MIDCH3Y1 && y <= MIDCH3Y2 && CHNUM > 16){
						if(ch_page == page1)
						{
							if(CH19_SW == ch_on)
							{
								CH19_SW = ch_off;
							}else{
								CH19_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHRX1 && x <= MIDCHRX2 && y >= MIDCH4Y1 && y <= MIDCH4Y2 && CHNUM > 16){
						if(ch_page == page1)
						{
							if(CH20_SW == ch_on)
							{
								CH20_SW = ch_off;
							}else{
								CH20_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHRX1 && x <= MIDCHRX2 && y >= MIDCH5Y1 && y <= MIDCH5Y2 && CHNUM > 16)
					{
						if(ch_page == page1)
						{
							if(CH21_SW == ch_on)
							{
								CH21_SW = ch_off;
							}else{
								CH21_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHRX1 && x <= MIDCHRX2 && y >= MIDCH6Y1 && y <= MIDCH6Y2 && CHNUM > 16){
						if(ch_page == page1)
						{
							if(CH22_SW == ch_on)
							{
								CH22_SW = ch_off;
							}else{
								CH22_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHRX1 && x <= MIDCHRX2 && y >= MIDCH7Y1 && y <= MIDCH7Y2 && CHNUM > 16){
						if(ch_page == page1)
						{
							if(CH23_SW == ch_on)
							{
								CH23_SW = ch_off;
							}else{
								CH23_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}else if(x >= MIDCHRX1 && x <= MIDCHRX2 && y >= MIDCH8Y1 && y <= MIDCH8Y2 && CHNUM > 16){
						if(ch_page == page1)
						{
							if(CH24_SW == ch_on)
							{
								CH24_SW = ch_off;
							}else{
								CH24_SW = ch_on;
							}
						}
						ChannelSwtichMid();
					}
				}else if(FONT == small){
					if(x >= SMLCHLX1 && x <= SMLCHLX2 && y >= SMLCH1Y1 && y <= SMLCH1Y2)
					{
						
						if(CH1_SW == ch_on)
						{
							CH1_SW = ch_off;
						}else{
							CH1_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHLX1 && x <= SMLCHLX2 && y >= SMLCH2Y1 && y <= SMLCH2Y2){
						if(CH2_SW == ch_on)
						{
							CH2_SW = ch_off;
						}else{
							CH2_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHLX1 && x <= SMLCHLX2 && y >= SMLCH3Y1 && y <= SMLCH3Y2){
						if(CH3_SW == ch_on)
						{
							CH3_SW = ch_off;
						}else{
							CH3_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHLX1 && x <= SMLCHLX2 && y >= SMLCH4Y1 && y <= SMLCH4Y2){
						if(CH4_SW == ch_on)
						{
							CH4_SW = ch_off;
						}else{
							CH4_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHLX1 && x <= SMLCHLX2 && y >= SMLCH5Y1 && y <= SMLCH5Y2)
					{
						if(CH5_SW == ch_on)
						{
							CH5_SW = ch_off;
						}else{
							CH5_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHLX1 && x <= SMLCHLX2 && y >= SMLCH6Y1 && y <= SMLCH6Y2){
						if(CH6_SW == ch_on)
						{
							CH6_SW = ch_off;
						}else{
							CH6_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHLX1 && x <= SMLCHLX2 && y >= SMLCH7Y1 && y <= SMLCH7Y2){
						if(CH7_SW == ch_on)
						{
							CH7_SW = ch_off;
						}else{
							CH7_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHLX2 && x <= SMLCH7Y1 && y >= SMLCH8Y1 && y <= SMLCH8Y2){
						if(CH8_SW == ch_on)
						{
							CH8_SW = ch_off;
						}else{
							CH8_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM1X1 && x <= SMLCHM1X2 && y >= SMLCH1Y1 && y <= SMLCH1Y2 && CHNUM > 8)
					{
						
						if(CH9_SW == ch_on)
						{
							CH9_SW = ch_off;
						}else{
							CH9_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM1X1 && x <= SMLCHM1X2 && y >= SMLCH2Y1 && y <= SMLCH2Y2 && CHNUM > 8){
						if(CH10_SW == ch_on)
						{
							CH10_SW = ch_off;
						}else{
							CH10_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM1X1 && x <= SMLCHM1X2 && y >= SMLCH3Y1 && y <= SMLCH3Y2 && CHNUM > 8){
						if(CH11_SW == ch_on)
						{
							CH11_SW = ch_off;
						}else{
							CH11_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM1X1 && x <= SMLCHM1X2 && y >= SMLCH4Y1 && y <= SMLCH4Y2 && CHNUM > 8){
						if(CH12_SW == ch_on)
						{
							CH12_SW = ch_off;
						}else{
							CH12_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM1X1 && x <= SMLCHM1X2 && y >= SMLCH5Y1 && y <= SMLCH5Y2 && CHNUM > 8)
					{
						if(CH13_SW == ch_on)
						{
							CH13_SW = ch_off;
						}else{
							CH13_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM1X1 && x <= SMLCHM1X2 && y >= SMLCH6Y1 && y <= SMLCH6Y2 && CHNUM > 8){
						if(CH14_SW == ch_on)
						{
							CH14_SW = ch_off;
						}else{
							CH14_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM1X1 && x <= SMLCHM1X2 && y >= SMLCH7Y1 && y <= SMLCH7Y2 && CHNUM > 8){
						if(CH15_SW == ch_on)
						{
							CH15_SW = ch_off;
						}else{
							CH15_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM1X1 && x <= SMLCHM1X2 && y >= SMLCH8Y1 && y <= SMLCH8Y2 && CHNUM > 8){
						if(CH16_SW == ch_on)
						{
							CH16_SW = ch_off;
						}else{
							CH16_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM2X1 && x <= SMLCHM2X2 && y >= SMLCH1Y1 && y <= SMLCH1Y2 && CHNUM > 16)
					{
						
						if(CH17_SW == ch_on)
						{
							CH17_SW = ch_off;
						}else{
							CH17_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM2X1 && x <= SMLCHM2X2 && y >= SMLCH2Y1 && y <= SMLCH2Y2 && CHNUM > 16){
						if(CH18_SW == ch_on)
						{
							CH18_SW = ch_off;
						}else{
							CH18_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM2X1 && x <= SMLCHM2X2 && y >= SMLCH3Y1 && y <= SMLCH3Y2 && CHNUM > 16){
						if(CH19_SW == ch_on)
						{
							CH19_SW = ch_off;
						}else{
							CH19_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM2X1 && x <= SMLCHM2X2 && y >= SMLCH4Y1 && y <= SMLCH4Y2 && CHNUM > 16){
						if(CH20_SW == ch_on)
						{
							CH20_SW = ch_off;
						}else{
							CH20_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM2X1 && x <= SMLCHM2X2 && y >= SMLCH5Y1 && y <= SMLCH5Y2 && CHNUM > 16)
					{
						if(CH21_SW == ch_on)
						{
							CH21_SW = ch_off;
						}else{
							CH21_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM2X1 && x <= SMLCHM2X2 && y >= SMLCH6Y1 && y <= SMLCH6Y2 && CHNUM > 16){
						if(CH22_SW == ch_on)
						{
							CH22_SW = ch_off;
						}else{
							CH22_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM2X1 && x <= SMLCHM2X2 && y >= SMLCH7Y1 && y <= SMLCH7Y2 && CHNUM > 16){
						if(CH23_SW == ch_on)
						{
							CH23_SW = ch_off;
						}else{
							CH23_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM2X1 && x <= SMLCHM2X2 && y >= SMLCH8Y1 && y <= SMLCH8Y2 && CHNUM > 16){
						if(CH24_SW == ch_on)
						{
							CH24_SW = ch_off;
						}else{
							CH24_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM3X1 && x <= SMLCHM3X2 && y >= SMLCH1Y1 && y <= SMLCH1Y2 && CHNUM > 16)
					{
						
						if(CH25_SW == ch_on)
						{
							CH25_SW = ch_off;
						}else{
							CH25_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM3X1 && x <= SMLCHM3X2 && y >= SMLCH2Y1 && y <= SMLCH2Y2 && CHNUM > 16){
						if(CH26_SW == ch_on)
						{
							CH26_SW = ch_off;
						}else{
							CH26_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM3X1 && x <= SMLCHM3X2 && y >= SMLCH3Y1 && y <= SMLCH3Y2 && CHNUM > 16){
						if(CH27_SW == ch_on)
						{
							CH27_SW = ch_off;
						}else{
							CH27_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM3X1 && x <= SMLCHM3X2 && y >= SMLCH4Y1 && y <= SMLCH4Y2 && CHNUM > 16){
						if(CH28_SW == ch_on)
						{
							CH28_SW = ch_off;
						}else{
							CH28_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM3X1 && x <= SMLCHM3X2 && y >= SMLCH5Y1 && y <= SMLCH5Y2 && CHNUM > 16)
					{
						if(CH29_SW == ch_on)
						{
							CH29_SW = ch_off;
						}else{
							CH29_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM3X1 && x <= SMLCHM3X2 && y >= SMLCH6Y1 && y <= SMLCH6Y2 && CHNUM > 16){
						if(CH30_SW == ch_on)
						{
							CH30_SW = ch_off;
						}else{
							CH30_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM3X1 && x <= SMLCHM3X2 && y >= SMLCH7Y1 && y <= SMLCH7Y2 && CHNUM > 16){
						if(CH31_SW == ch_on)
						{
							CH31_SW = ch_off;
						}else{
							CH31_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHM3X1 && x <= SMLCHM3X2 && y >= SMLCH8Y1 && y <= SMLCH8Y2 && CHNUM > 16){
						if(CH32_SW == ch_on)
						{
							CH32_SW = ch_off;
						}else{
							CH32_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHRX1 && x <= SMLCHRX2 && y >= SMLCH1Y1 && y <= SMLCH1Y2 && CHNUM > 16)
					{
						
						if(CH33_SW == ch_on)
						{
							CH33_SW = ch_off;
						}else{
							CH33_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHRX1 && x <= SMLCHRX2 && y >= SMLCH2Y1 && y <= SMLCH2Y2 && CHNUM > 16){
						if(CH34_SW == ch_on)
						{
							CH34_SW = ch_off;
						}else{
							CH34_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHRX1 && x <= SMLCHRX2 && y >= SMLCH3Y1 && y <= SMLCH3Y2 && CHNUM > 16){
						if(CH35_SW == ch_on)
						{
							CH35_SW = ch_off;
						}else{
							CH35_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHRX1 && x <= SMLCHRX2 && y >= SMLCH4Y1 && y <= SMLCH4Y2 && CHNUM > 16){
						if(CH36_SW == ch_on)
						{
							CH36_SW = ch_off;
						}else{
							CH36_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHRX1 && x <= SMLCHRX2 && y >= SMLCH5Y1 && y <= SMLCH5Y2 && CHNUM > 16)
					{
						if(CH37_SW == ch_on)
						{
							CH37_SW = ch_off;
						}else{
							CH37_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHRX1 && x <= SMLCHRX2 && y >= SMLCH6Y1 && y <= SMLCH6Y2 && CHNUM > 16){
						if(CH38_SW == ch_on)
						{
							CH38_SW = ch_off;
						}else{
							CH38_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHRX1 && x <= SMLCHRX2 && y >= SMLCH7Y1 && y <= SMLCH7Y2 && CHNUM > 16){
						if(CH39_SW == ch_on)
						{
							CH39_SW = ch_off;
						}else{
							CH39_SW = ch_on;
						}
						ChannelSwtichSml();
					}else if(x >= SMLCHRX1 && x <= SMLCHRX2 && y >= SMLCH8Y1 && y <= SMLCH8Y2 && CHNUM > 16){
						if(CH40_SW == ch_on)
						{
							CH40_SW = ch_off;
						}else{
							CH40_SW = ch_on;
						}
						ChannelSwtichSml();
					}
				}
				
			}break;
			case graph:
			{
				if(x >= MENU1X1 && x <= MENU1X2 && y >= MENUY1 && y <= MENUY2)
				{
					page_home();
					press = 1;
					ButtonSound();//按键音
				}
				if(x >= MENU2X1 && x <= MENU2X2 && y >= MENUY1 && y <= MENUY2)
				{
					page_his();
					press = 1;
					ButtonSound();//按键音
				}
				if(x >= MENU3X1 && x <= MENU3X2 && y >= MENUY1 && y <= MENUY2)
				{
					page_sys();
					press = 1;
					ButtonSound();//按键音
				}
				if(x >= MENU4X1 && x <= MENU4X2 && y >= MENUY1 && y <= MENUY2)
				{
					
					page_set();
					press = 1;
					ButtonSound();//按键音
				}
				if(x >= MENU5X1 && x <= MENU5X2 && y >= MENUY1 && y <= MENUY2)
				{
					page_gset();
					press = 1;
					ButtonSound();//按键音
				}
			}break;
			case sysset:
			{
				if(x >= LANGX1 && x <= LANGX2 && y >= LANGY1 && y <= LANGY2)
				{
					DrawMenu();
					Drawlang();
					op_sw = op_on;
					op_flag = set_lang;
					press = 1;
					ButtonSound();//按键音
				}else
				if(x >= DATEX1 && x <= DATEX2 && y >= DATEY1 && y <= DATEY2)
				{
					DrawMenu();
					Drawdate();
					op_sw = op_on;
					op_flag = set_date;
					press = 1;
					ButtonSound();//按键音
				}else
				if(x >= TIMEX1 && x <= TIMEX2 && y >= TIMEY1 && y <= TIMEY2)
				{
					DrawMenu();
					Drawtime();
					op_sw = op_on;
					op_flag = set_time;
					press = 1;
					ButtonSound();//按键音
				}else
				if(x >= BACKX1 && x <= BACKX2 && y >= BACKY1 && y <= BACKY2)
				{
					DrawMenu();
					DrawBrt();
					op_sw = op_on;
					op_flag = set_brt;
					press = 1;
					ButtonSound();//按键音
				}else
				if(x >= DIMX1 && x <= DIMX2 && y >= DIMY1 && y <= DIMY2)
				{
					DrawMenu();
					DrawDim();
					op_sw = op_on;
					op_flag = set_dim;
					press = 1;
					ButtonSound();//按键音
				}else
				if(x >= TOUCHX1 && x <= TOUCHX2 && y >= TOUCHY1 && y <= TOUCHY2)
				{
					DrawMenu();
					DrawTouch();
					op_sw = op_on;
					op_flag = set_touch;
					press = 1;
					ButtonSound();//按键音
				}else
				if(x >= MENU1X1 && x <= MENU1X2 && y >= MENUY1 && y <= MENUY2)
				{
					if(op_sw == op_on)
					{
						switch(op_flag)
						{
							case set_lang:
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_YELLOW);
								LCD_DisplayStringLine(50,170,"CHINESE");
								LANG = chs;
								page_sys();
								op_sw =op_off;
								Save_flag();
								press = 1;
								ButtonSound();//按键音
							}break;
							case set_date:
							{
								if(date_page == page1)
								{
									plus_year();
								}else{
									plus_day();
								}
								press = 1;
								ButtonSound();//按键音
							}break;
							case set_time:
							{
								if(time_page == page1)
								{
									plus_hour();							
								}else{
									plus_sec();
								}
								press = 1;
								ButtonSound();//按键音
							}break;
							case set_brt:
							{
								press = 1;
								brt_set(1);
								press = 1;
								ButtonSound();//按键音
							}break;
							case set_dim:
							{
								press = 1;
								dim_set(1);
								press = 1;
								ButtonSound();//按键音
							}break;
							case set_touch:
							{
								press = 1;
								touch_set(1);
								press = 1;
								ButtonSound();//按键音
							}break;
						}
					}else{
						page_home();
						press = 1;
						ButtonSound();//按键音
					}
					
				}else
				if(x >= MENU2X1 && x <= MENU2X2 && y >= MENUY1 && y <= MENUY2)
				{
					if(op_sw == op_on)
					{
						switch(op_flag)
						{
							case set_lang:
							{
								LCD_SetBackColor(LCD_COLOR_BACK);
								LCD_SetTextColor(LCD_COLOR_YELLOW);
								LCD_DisplayStringLine(50,170,"ENGLISH");
								LANG = eng;
								page_sys();
								op_sw = op_off;
								Save_flag();
								press = 1;
								ButtonSound();//按键音
							}break;
							case set_date:
							{
								if(date_page == page1)
								{
									minus_year();
								}else{
									minus_day();
								}
								press = 1;
								ButtonSound();//按键音
							}break;
							case set_time:
							{
								if(time_page == page1)
								{
									minus_hour();
								}else{
									minus_sec();
								}
								press = 1;
								ButtonSound();//按键音
							}break;
							case set_brt:
							{
								press = 1;
								brt_set(2);
								press = 1;
								ButtonSound();//按键音
							}break;
							case set_dim:
							{
								press = 1;
								dim_set(2);
								press = 1;
								ButtonSound();//按键音
							}break;
							case set_touch:
							{
								press = 1;
								touch_set(2);
								press = 1;
								ButtonSound();//按键音
							}break;
						
						}
					}else{
						page_graph();
						press = 1;
						ButtonSound();//按键音
					}
				}else
				if(x >= MENU3X1 && x <= MENU3X2 && y >= MENUY1 && y <= MENUY2)
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
								press = 1;
								ButtonSound();//按键音
							}break;
							case set_time:
							{
								if(time_page == page1)
								{
									plus_min();
								}
								press = 1;
								ButtonSound();//按键音
							}break;				
							case set_brt:
							{
								press = 1;
								brt_set(3);
								press = 1;
								ButtonSound();//按键音
							}break;
							case set_dim:
							{
								press = 1;
								dim_set(3);
								press = 1;
								ButtonSound();//按键音
							}break;
						}
					}else{
						page_sys();
						press = 1;
						ButtonSound();//按键音
					}
				}else
				if(x >= MENU4X1 && x <= MENU4X2 && y >= MENUY1 && y <= MENUY2)
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
								press = 1;
								ButtonSound();//按键音
							}break;
							case set_time:
							{
								if(time_page == page1)
								{
									minus_min();
								}
								press = 1;
								ButtonSound();//按键音
							}break;				
							case set_brt:
							{
								press = 1;
								brt_set(4);
								press = 1;
								ButtonSound();//按键音
							}break;
							case set_dim:
							{
								press = 1;
								dim_set(4);
								press = 1;
								ButtonSound();//按键音
							}break;
						}
					}else{
						page_set();
						press = 1;
						ButtonSound();//按键音
					}
				}else
				if(x >= MENU5X1 && x <= MENU5X2 && y >= MENUY1 && y <= MENUY2)
				{
					if(op_sw == op_on)
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
								press = 1;
								ButtonSound();//按键音
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
								press = 1;
								ButtonSound();//按键音
							}break;
							case set_brt:
							{
								press = 1;
								if(op_sw == op_on)
								{
									brt_set(5);
								}else{
									page_sysinfo();
								}
								press = 1;
								ButtonSound();//按键音
							}break;
							case set_dim:
							{
								press = 1;
								if(op_sw == op_on)
								{
									dim_set(5);
								}else{
									page_sysinfo();
								}
								press = 1;
								ButtonSound();//按键音
							}break;
						}
					}else{
						page_sysinfo();
						press = 1;
						ButtonSound();//按键音
					}
				}else if(x >= SYSBLANKX1 && x <= SYSBLANKX2 && y >= SYSBLANKY1 && y <= SYSBLANKY2){
					DrawMenu();
					Drawsysmenu();
					op_sw = op_off;
					press = 1;
					ButtonSound();//按键音
				}	

			}break;
			case settings:
			{
				if(x >= COMPX1 && x <= COMPX2 && y >= COMPY1 && y <= COMPY2)
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
					press = 1;
					ButtonSound();//按键音
					op_flag = set_filter;
				}else if(x >= SPEEDX1 && x <= SPEEDX2 && y >= SPEEDY1 && y <= SPEEDY2){
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
					press = 1;
					ButtonSound();//按键音
					op_flag = set_spd;
				}else if(x >= BEEPX1 && x <= BEEPX2 && y >= BEEPY1 && y <= BEEPY2){
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
					press = 1;
					ButtonSound();//按键音
					op_flag = set_beep;
				}else if(x >= BAUDX1 && x <= BAUDX2 && y >= BEEPY1 && y <= BEEPY2){
					DrawMenu();//重绘菜单栏
					LCD_SetTextColor(LCD_COLOR_YELLOW);
					LCD_SetBackColor(LCD_COLOR_BLACK);
					LCD_DisplayStringLine(445,30,"9600");
					LCD_DisplayStringLine(445,150,"19200");
					LCD_DisplayStringLine(445,280,"38400");
					LCD_DisplayStringLine(445,405,"57600");
					LCD_DisplayStringLine(445,525,"115200");
					op_sw = op_on;
					press = 1;
					ButtonSound();//按键音
					op_flag = set_baud;
				}else if(x >= UNITX1 && x <= UNITX2 && y >= UNITY1 && y <= UNITY2){
					DrawMenu();//重绘菜单栏
					LCD_SetTextColor(LCD_COLOR_YELLOW);
					LCD_SetBackColor(LCD_COLOR_BLACK);
					DISP_CNL_S(445,48,"o");
					LCD_DisplayStringLine(445,53,"C");
					LCD_DisplayStringLine(445,173,"K");
					DISP_CNL_S(445,298,"o");
					LCD_DisplayStringLine(445,303,"F");
					op_sw = op_on;
					press = 1;
					ButtonSound();//按键音
					op_flag = set_unit;
				}else
				if(x >= MENU1X1 && x <= MENU1X2 && y >= MENUY1 && y <= MENUY2)
				{
					press = 1;
					ButtonSound();//按键音
					if(op_sw == op_on)
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
									focus_off1();
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
									focus_off1();
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
							case set_unit:
							{
								if(op_sw == op_on)
								{
									DrawMenu();//重绘菜单栏
									Drawsetmenu();
//									LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
//									LCD_DrawFullRect(150,127,24,31);
									LCD_SetBackColor(LCD_COLOR_BACK);
									LCD_SetTextColor(LCD_COLOR_YELLOW);
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
									focus_off1();
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
									focus_off1();
									LCD_DisplayStringLine(87,500,"9600");
									BAUD = b9600;
									op_sw = op_off;
									Save_flag();
								}else{
									page_home();
								}
							}break;
						}
					}else{
						page_home();
						press = 1;
						ButtonSound();//按键音
					}
					
				}else
				if(x >= MENU2X1 && x <= MENU2X2 && y >= MENUY1 && y <= MENUY2)
				{
					press = 1;
					ButtonSound();//按键音
					if(op_sw == op_on)
					{
						switch(op_flag)
						{
							case set_filter:
							{
								if(op_sw == op_on)
								{
									DrawMenu();//重绘菜单栏
									Drawsetmenu();
									focus_off1();
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
									focus_off1();
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
							case set_unit:
							{
								if(op_sw == op_on)
								{
									DrawMenu();//重绘菜单栏
									Drawsetmenu();
//									LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
//									LCD_DrawFullRect(150,127,24,31);
									LCD_SetBackColor(LCD_COLOR_BACK);
									LCD_SetTextColor(LCD_COLOR_YELLOW);					
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
									focus_off1();
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
									focus_off1();
									LCD_DisplayStringLine(87,500,"19200");
									BAUD = b19200;
									op_sw = op_off;
									Save_flag();
								}else{
									page_graph();
								}
							}break;
						
						}
					}else{
						page_graph();
						press = 1;
					}
				}else
				if(x >= MENU3X1 && x <= MENU3X2 && y >= MENUY1 && y <= MENUY2)
				{
					press = 1;
					ButtonSound();//按键音
					if(op_sw == op_on)
					{
						switch(op_flag)
						{
							case set_unit:
							{
								if(op_sw == op_on)
								{
									DrawMenu();//重绘菜单栏
									Drawsetmenu();
//									LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
//									LCD_DrawFullRect(150,127,24,31);
									LCD_SetBackColor(LCD_COLOR_BACK);
									LCD_SetTextColor(LCD_COLOR_YELLOW);					
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
									focus_off1();
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
									focus_off1();
									LCD_DisplayStringLine(87,500,"38400");
									BAUD = b38400;
									op_sw = op_off;
									Save_flag();
								}else{
									page_sys();
								}
							}break;
						}
					}else{
						page_sys();
						press = 1;
					}
				}else
				if(x >= MENU4X1 && x <= MENU4X2 && y >= MENUY1 && y <= MENUY2)
				{
					press = 1;
					ButtonSound();//按键音
					if(op_sw == op_on)
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
									focus_off1();
									LCD_DisplayStringLine(87,500,"57600");
									BAUD = b57600;
									op_sw = op_off;
									Save_flag();
								}else{
									page_set();
								}
							}break;
						}
					}else{
						page_cal();
						press = 1;
						ButtonSound();//按键音
					}
				}else
				if(x >= MENU5X1 && x <= MENU5X2 && y >= MENUY1 && y <= MENUY2)
				{
					press = 1;
					ButtonSound();//按键音
					if(op_sw == op_on)
					{
						switch(op_flag)
						{
							case set_baud:
							{
								if(op_sw == op_on)
								{
									DrawMenu();//重绘菜单栏
									Drawsetmenu();
									focus_off1();
									LCD_DisplayStringLine(87,500,"115200");
									BAUD = b115200;
									op_sw = op_off;
									Save_flag();
								}else{
									page_sep();
								}
							}break;
						}
					}else{
						page_sep();
						press = 1;
						ButtonSound();//按键音
					}
				}else if(x >= SETBLANKX1 && x <= SETBLANKX2 && y >= SETBLANKY1 && y <= SETBLANKY2){
					DrawMenu();
					Drawsetmenu();
					op_sw = op_off;
					press = 1;
					ButtonSound();//按键音
				}
			}break;
			case calibrate:
			{
				
				if(x >= MENU1X1 && x <= MENU1X2 && y >= MENUY1 && y <= MENUY2)
				{
					ButtonSound();//按键音
					page_home();
					press = 1;
					
				}else
				if(x >= MENU2X1 && x <= MENU2X2 && y >= MENUY1 && y <= MENUY2)
				{		
					ButtonSound();//按键音
					page_graph();
					press = 1;
				}else
				if(x >= MENU3X1 && x <= MENU3X2 && y >= MENUY1 && y <= MENUY2)
				{
					ButtonSound();//按键音
					press = 1;
					page_sys();
					press = 1;
				}else
				if(x >= MENU4X1 && x <= MENU4X2 && y >= MENUY1 && y <= MENUY2)
				{
					ButtonSound();//按键音
					page_set();
					press = 1;
				}
//				if(x >= MENU5X1 && x <= MENU5X2 && y >= MENUY1 && y <= MENUY2)
//				{
//					if(CHNUM > 8)
//					{
//						switch(cor_page)
//						{
//							case page1:
//							{
//								LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
//								LCD_SetBackColor(LCD_COLOR_BACK);
//								LCD_DisplayStringLine(80,10,"009");
//								LCD_DisplayStringLine(120,10,"010");
//								LCD_DisplayStringLine(160,10,"011");
//								LCD_DisplayStringLine(200,10,"012");
//								LCD_DisplayStringLine(240,10,"013");
//								LCD_DisplayStringLine(280,10,"014");
//								LCD_DisplayStringLine(320,10,"015");
//								LCD_DisplayStringLine(360,10,"016");
//								minfocus_off(1,CH9TEMP);
//								minfocus_off(2,CH10TEMP);
//								minfocus_off(3,CH11TEMP);
//								minfocus_off(4,CH12TEMP);
//								minfocus_off(5,CH13TEMP);
//								minfocus_off(6,CH14TEMP);
//								minfocus_off(7,CH15TEMP);
//								minfocus_off(8,CH16TEMP);
//								
//								maxfocus_on(1,COR9);
//								maxfocus_off(2,COR10);
//								maxfocus_off(3,COR11);
//								maxfocus_off(4,COR12);
//								maxfocus_off(5,COR13);
//								maxfocus_off(6,COR14);
//								maxfocus_off(7,COR15);
//								maxfocus_off(8,COR16);
//								
//								op_flag = set_c1;
//								cor_page = page2;
//							}break;
//							case page2:
//							{
//								LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
//								LCD_SetBackColor(LCD_COLOR_BACK);
//								LCD_DisplayStringLine(80,10,"017");
//								LCD_DisplayStringLine(120,10,"018");
//								LCD_DisplayStringLine(160,10,"019");
//								LCD_DisplayStringLine(200,10,"020");
//								LCD_DisplayStringLine(240,10,"021");
//								LCD_DisplayStringLine(280,10,"022");
//								LCD_DisplayStringLine(320,10,"023");
//								LCD_DisplayStringLine(360,10,"024");
//								minfocus_off(1,CH17TEMP);
//								minfocus_off(2,CH18TEMP);
//								minfocus_off(3,CH19TEMP);
//								minfocus_off(4,CH20TEMP);
//								minfocus_off(5,CH21TEMP);
//								minfocus_off(6,CH22TEMP);
//								minfocus_off(7,CH23TEMP);
//								minfocus_off(8,CH24TEMP);
//								
//								maxfocus_on(1,COR17);
//								maxfocus_off(2,COR18);
//								maxfocus_off(3,COR19);
//								maxfocus_off(4,COR20);
//								maxfocus_off(5,COR21);
//								maxfocus_off(6,COR22);
//								maxfocus_off(7,COR23);
//								maxfocus_off(8,COR24);
//								
//								op_flag = set_c1;
//								cor_page = page3;
//							}break;
//							case page3:
//							{
//								LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
//								LCD_SetBackColor(LCD_COLOR_BACK);
//								LCD_DisplayStringLine(80,10,"025");
//								LCD_DisplayStringLine(120,10,"026");
//								LCD_DisplayStringLine(160,10,"027");
//								LCD_DisplayStringLine(200,10,"028");
//								LCD_DisplayStringLine(240,10,"029");
//								LCD_DisplayStringLine(280,10,"030");
//								LCD_DisplayStringLine(320,10,"031");
//								LCD_DisplayStringLine(360,10,"032");
//								minfocus_off(1,CH25TEMP);
//								minfocus_off(2,CH26TEMP);
//								minfocus_off(3,CH27TEMP);
//								minfocus_off(4,CH28TEMP);
//								minfocus_off(5,CH29TEMP);
//								minfocus_off(6,CH30TEMP);
//								minfocus_off(7,CH31TEMP);
//								minfocus_off(8,CH32TEMP);
//								
//								maxfocus_on(1,COR25);
//								maxfocus_off(2,COR26);
//								maxfocus_off(3,COR27);
//								maxfocus_off(4,COR28);
//								maxfocus_off(5,COR29);
//								maxfocus_off(6,COR30);
//								maxfocus_off(7,COR31);
//								maxfocus_off(8,COR32);
//								
//								op_flag = set_c1;
//								cor_page = page4;
//							}break;
//							case page4:
//							{
//								LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
//								LCD_SetBackColor(LCD_COLOR_BACK);
//								LCD_DisplayStringLine(80,10,"033");
//								LCD_DisplayStringLine(120,10,"034");
//								LCD_DisplayStringLine(160,10,"035");
//								LCD_DisplayStringLine(200,10,"036");
//								LCD_DisplayStringLine(240,10,"037");
//								LCD_DisplayStringLine(280,10,"038");
//								LCD_DisplayStringLine(320,10,"039");
//								LCD_DisplayStringLine(360,10,"040");
//								minfocus_off(1,CH33TEMP);
//								minfocus_off(2,CH34TEMP);
//								minfocus_off(3,CH35TEMP);
//								minfocus_off(4,CH36TEMP);
//								minfocus_off(5,CH37TEMP);
//								minfocus_off(6,CH38TEMP);
//								minfocus_off(7,CH39TEMP);
//								minfocus_off(8,CH40TEMP);
//								
//								maxfocus_on(1,COR33);
//								maxfocus_off(2,COR34);
//								maxfocus_off(3,COR35);
//								maxfocus_off(4,COR36);
//								maxfocus_off(5,COR37);
//								maxfocus_off(6,COR38);
//								maxfocus_off(7,COR39);
//								maxfocus_off(8,COR40);
//								
//								op_flag = set_c1;
//								cor_page = page5;
//							}break;
//							case page5:
//							{
//								LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
//								LCD_SetBackColor(LCD_COLOR_BACK);
//								LCD_DisplayStringLine(80,10,"001");
//								LCD_DisplayStringLine(120,10,"002");
//								LCD_DisplayStringLine(160,10,"003");
//								LCD_DisplayStringLine(200,10,"004");
//								LCD_DisplayStringLine(240,10,"005");
//								LCD_DisplayStringLine(280,10,"006");
//								LCD_DisplayStringLine(320,10,"007");
//								LCD_DisplayStringLine(360,10,"008");
//								minfocus_off(1,CH1TEMP);
//								minfocus_off(2,CH2TEMP);
//								minfocus_off(3,CH3TEMP);
//								minfocus_off(4,CH4TEMP);
//								minfocus_off(5,CH5TEMP);
//								minfocus_off(6,CH6TEMP);
//								minfocus_off(7,CH7TEMP);
//								minfocus_off(8,CH8TEMP);
//								
//								maxfocus_on(1,COR1);
//								maxfocus_off(2,COR2);
//								maxfocus_off(3,COR3);
//								maxfocus_off(4,COR4);
//								maxfocus_off(5,COR5);
//								maxfocus_off(6,COR6);
//								maxfocus_off(7,COR7);
//								maxfocus_off(8,COR8);
//								
//								op_flag = set_min1;
//								cor_page = page1;
//							}break;
//						}
//					}
//					press = 1;
//				}
			}break;
			case separation:
			{
//				ButtonSound();//按键音
				if(x >= MENU1X1 && x <= MENU1X2 && y >= MENUY1 && y <= MENUY2)
				{					
					page_home();
					press = 1;
					ButtonSound();//按键音
					
				}else
				if(x >= MENU2X1 && x <= MENU2X2 && y >= MENUY1 && y <= MENUY2)
				{					
					page_graph();
					press = 1;
					ButtonSound();//按键音
				}else
				if(x >= MENU3X1 && x <= MENU3X2 && y >= MENUY1 && y <= MENUY2)
				{
					press = 1;
					page_sys();
					press = 1;
					ButtonSound();//按键音
				}else
				if(x >= MENU4X1 && x <= MENU4X2 && y >= MENUY1 && y <= MENUY2)
				{
					page_set();
					press = 1;
					ButtonSound();//按键音
				}else
				if(x >= MENU5X1 && x <= MENU5X2 && y >= MENUY1 && y <= MENUY2)
				{
					ButtonSound();//按键音
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
					press = 1;
				}
			}break;
			case sysinfo:
			{
				
				if(x >= MENU1X1 && x <= MENU1X2 && y >= MENUY1 && y <= MENUY2)
				{					
					page_home();
					press = 1;
					ButtonSound();//按键音
					
				}else
				if(x >= MENU2X1 && x <= MENU2X2 && y >= MENUY1 && y <= MENUY2)
				{					
					page_graph();
					press = 1;
					ButtonSound();//按键音
				}else
				if(x >= MENU3X1 && x <= MENU3X2 && y >= MENUY1 && y <= MENUY2)
				{
					press = 1;
					page_sys();
					press = 1;
					ButtonSound();//按键音
				}else
				if(x >= MENU4X1 && x <= MENU4X2 && y >= MENUY1 && y <= MENUY2)
				{
					page_set();
					press = 1;
					ButtonSound();//按键音
				}else
				if(x >= MENU5X1 && x <= MENU5X2 && y >= MENUY1 && y <= MENUY2)
				{
					page_sysinfo();
					press = 1;
					ButtonSound();//按键音
				}
			}break;
		}
//	}
}

void DrawTC(void)
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
	}else if(TCTYPE == AM2305){
		LCD_DisplayStringLine(47,100,"1~5V");
	}	
}


void ChannelSwtichBig(void)
{
	if(ch_page == page1)
	{
		if(CH1_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			LCD_DisplayStringLine_48(100,10,"001");
		}else if(CH1_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			LCD_DisplayStringLine_48(100,10,"001");
		}
		
		if(CH2_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			LCD_DisplayStringLine_48(170+10,10,"002");
		}else if(CH2_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			LCD_DisplayStringLine_48(170+10,10,"002");
		}
		
		if(CH3_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			LCD_DisplayStringLine_48(240+20,10,"003");
		}else if(CH3_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			LCD_DisplayStringLine_48(240+20,10,"003");
		}
		
		if(CH4_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			LCD_DisplayStringLine_48(310+30,10,"004");
		}else if(CH4_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			LCD_DisplayStringLine_48(310+30,10,"004");
		}
		
		if(CH5_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			LCD_DisplayStringLine_48(100,330,"005");
		}else if(CH5_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			LCD_DisplayStringLine_48(100,330,"005");
		}
		
		if(CH6_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			LCD_DisplayStringLine_48(170+10,330,"006");
		}else if(CH6_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			LCD_DisplayStringLine_48(170+10,330,"006");
		}
		
		if(CH7_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			LCD_DisplayStringLine_48(240+20,330,"007");
		}else if(CH7_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			LCD_DisplayStringLine_48(240+20,330,"007");
		}
		
		if(CH8_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			LCD_DisplayStringLine_48(310+30,330,"008");
		}else if(CH8_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			LCD_DisplayStringLine_48(310+30,330,"008");
		}
	}else if(ch_page == page2){
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
	}else if(ch_page == page3){
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
	}else if(ch_page == page4){
		if(CH25_SW == ch_off)
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
	}else if(ch_page == page5){
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
	}
	press = 1;
	ButtonSound();//按键音
	if(LANG == chs)
	{
		DrawInstruction("通道设置");
	}else{
		DrawInstruction("Channel set");
	}
	Save_flag();
}

void ChannelSwtichMid(void)
{
	if(ch_page == page1)
	{
		if(CH1_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_M(100,10,"001");
		}else if(CH1_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_M(100,10,"001");
		}
		
		if(CH2_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_M(140,10,"002");
		}else if(CH2_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_M(140,10,"002");
		}
		
		if(CH3_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_M(180,10,"003");
		}else if(CH3_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_M(180,10,"003");
		}
		
		if(CH4_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_M(220,10,"004");
		}else if(CH4_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_M(220,10,"004");
		}
		
		if(CH5_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_M(260,10,"005");
		}else if(CH5_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_M(260,10,"005");
		}
		
		if(CH6_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_M(300,10,"006");
		}else if(CH6_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_M(300,10,"006");
		}
		
		if(CH7_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_M(340,10,"007");
		}else if(CH7_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_M(340,10,"007");
		}
		
		if(CH8_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_M(380,10,"008");
		}else if(CH8_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_M(380,10,"008");
		}
		if(CHNUM > 8)
		{
			if(CH9_SW == ch_off)
			{
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
				DISP_CNL_M(100,210,"009");
			}else if(CH9_SW == ch_on){
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_HLT);
				DISP_CNL_M(100,210,"009");
			}
			
			if(CH10_SW == ch_off)
			{
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
				DISP_CNL_M(140,210,"010");
			}else if(CH10_SW == ch_on){
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_HLT);
				DISP_CNL_M(140,210,"010");
			}
			
			if(CH11_SW == ch_off)
			{
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
				DISP_CNL_M(180,210,"011");
			}else if(CH11_SW == ch_on){
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_HLT);
				DISP_CNL_M(180,210,"011");
			}
			
			if(CH12_SW == ch_off)
			{
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
				DISP_CNL_M(220,210,"012");
			}else if(CH12_SW == ch_on){
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_HLT);
				DISP_CNL_M(220,210,"012");
			}
			
			if(CH13_SW == ch_off)
			{
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
				DISP_CNL_M(260,210,"013");
			}else if(CH13_SW == ch_on){
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_HLT);
				DISP_CNL_M(260,210,"013");
			}
			
			if(CH14_SW == ch_off)
			{
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
				DISP_CNL_M(300,210,"014");
			}else if(CH14_SW == ch_on){
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_HLT);
				DISP_CNL_M(300,210,"014");
			}
			
			if(CH15_SW == ch_off)
			{
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
				DISP_CNL_M(340,210,"015");
			}else if(CH15_SW == ch_on){
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_HLT);
				DISP_CNL_M(340,210,"015");
			}
			
			if(CH16_SW == ch_off)
			{
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
				DISP_CNL_M(380,210,"016");
			}else if(CH16_SW == ch_on){
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_HLT);
				DISP_CNL_M(380,210,"016");
			}
		}
		if(CHNUM > 16)
		{
			if(CH17_SW == ch_off)
			{
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
				DISP_CNL_M(100,410,"017");
			}else if(CH17_SW == ch_on){
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_HLT);
				DISP_CNL_M(100,410,"017");
			}
			
			if(CH18_SW == ch_off)
			{
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
				DISP_CNL_M(140,410,"018");
			}else if(CH18_SW == ch_on){
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_HLT);
				DISP_CNL_M(140,410,"018");
			}
			
			if(CH19_SW == ch_off)
			{
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
				DISP_CNL_M(180,410,"019");
			}else if(CH19_SW == ch_on){
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_HLT);
				DISP_CNL_M(180,410,"019");
			}
			
			if(CH20_SW == ch_off)
			{
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
				DISP_CNL_M(220,410,"020");
			}else if(CH20_SW == ch_on){
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_HLT);
				DISP_CNL_M(220,410,"020");
			}
			
			if(CH21_SW == ch_off)
			{
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
				DISP_CNL_M(260,410,"021");
			}else if(CH21_SW == ch_on){
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_HLT);
				DISP_CNL_M(260,410,"021");
			}
			
			if(CH22_SW == ch_off)
			{
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
				DISP_CNL_M(300,410,"022");
			}else if(CH22_SW == ch_on){
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_HLT);
				DISP_CNL_M(300,410,"022");
			}
			
			if(CH23_SW == ch_off)
			{
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
				DISP_CNL_M(340,410,"023");
			}else if(CH23_SW == ch_on){
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_HLT);
				DISP_CNL_M(340,410,"023");
			}
			
			if(CH24_SW == ch_off)
			{
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
				DISP_CNL_M(380,410,"024");
			}else if(CH24_SW == ch_on){
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_HLT);
				DISP_CNL_M(380,410,"024");
			}
		}
	}else if(ch_page == page2){
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
	}
	
	press = 1;
	ButtonSound();//按键音
	if(LANG == chs)
	{
		DrawInstruction("通道设置");
	}else{
		DrawInstruction("Channel set");
	}
	Save_flag();
}

void ChannelSwtichSml(void)
{
	if(CH1_SW == ch_off)
	{
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
		DISP_CNL_S(100,10,"001");
	}else if(CH1_SW == ch_on){
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_SetTextColor(LCD_COLOR_HLT);
		DISP_CNL_S(100,10,"001");
	}
	
	if(CH2_SW == ch_off)
	{
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
		DISP_CNL_S(140,10,"002");
	}else if(CH2_SW == ch_on){
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_SetTextColor(LCD_COLOR_HLT);
		DISP_CNL_S(140,10,"002");
	}
	
	if(CH3_SW == ch_off)
	{
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
		DISP_CNL_S(180,10,"003");
	}else if(CH3_SW == ch_on){
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_SetTextColor(LCD_COLOR_HLT);
		DISP_CNL_S(180,10,"003");
	}
	
	if(CH4_SW == ch_off)
	{
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
		DISP_CNL_S(220,10,"004");
	}else if(CH4_SW == ch_on){
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_SetTextColor(LCD_COLOR_HLT);
		DISP_CNL_S(220,10,"004");
	}
	
	if(CH5_SW == ch_off)
	{
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
		DISP_CNL_S(260,10,"005");
	}else if(CH5_SW == ch_on){
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_SetTextColor(LCD_COLOR_HLT);
		DISP_CNL_S(260,10,"005");
	}
	
	if(CH6_SW == ch_off)
	{
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
		DISP_CNL_S(300,10,"006");
	}else if(CH6_SW == ch_on){
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_SetTextColor(LCD_COLOR_HLT);
		DISP_CNL_S(300,10,"006");
	}
	
	if(CH7_SW == ch_off)
	{
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
		DISP_CNL_S(340,10,"007");
	}else if(CH7_SW == ch_on){
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_SetTextColor(LCD_COLOR_HLT);
		DISP_CNL_S(340,10,"007");
	}
	
	if(CH8_SW == ch_off)
	{
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
		DISP_CNL_S(380,10,"008");
	}else if(CH8_SW == ch_on){
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_SetTextColor(LCD_COLOR_HLT);
		DISP_CNL_S(380,10,"008");
	}
	if(CHNUM > 8)
	{
		if(CH9_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(100,134,"009");
		}else if(CH9_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(100,134,"009");
		}
		
		if(CH10_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(140,134,"010");
		}else if(CH10_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(140,134,"010");
		}
		
		if(CH11_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(180,134,"011");
		}else if(CH11_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(180,134,"011");
		}
		
		if(CH12_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(220,134,"012");
		}else if(CH12_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(220,134,"012");
		}
		
		if(CH13_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(260,134,"013");
		}else if(CH13_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(260,134,"013");
		}
		
		if(CH14_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(300,134,"014");
		}else if(CH14_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(300,134,"014");
		}
		
		if(CH15_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(340,134,"015");
		}else if(CH15_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(340,134,"015");
		}
		
		if(CH16_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(380,134,"016");
		}else if(CH16_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(380,134,"016");
		}
	}
	if(CHNUM > 16)
	{
		if(CH17_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(100,258,"017");
		}else if(CH17_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(100,258,"017");
		}
		
		if(CH18_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(140,258,"018");
		}else if(CH18_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(140,258,"018");
		}
		
		if(CH19_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(180,258,"019");
		}else if(CH19_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(180,258,"019");
		}
		
		if(CH20_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(220,258,"020");
		}else if(CH20_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(220,258,"020");
		}
		
		if(CH21_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(260,258,"021");
		}else if(CH21_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(260,258,"021");
		}
		
		if(CH22_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(300,258,"022");
		}else if(CH22_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(300,258,"022");
		}
		
		if(CH23_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(340,258,"023");
		}else if(CH23_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(340,258,"023");
		}
		
		if(CH24_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(380,258,"024");
		}else if(CH24_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(380,258,"024");
		}
		
		if(CH25_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(100,382,"025");
		}else if(CH25_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(100,382,"025");
		}
		
		if(CH26_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(140,382,"026");
		}else if(CH26_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(140,382,"026");
		}
		
		if(CH27_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(180,382,"027");
		}else if(CH27_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(180,382,"027");
		}
		
		if(CH28_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(220,382,"028");
		}else if(CH28_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(220,382,"028");
		}
		
		if(CH29_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(260,382,"029");
		}else if(CH29_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(260,382,"029");
		}
		
		if(CH30_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(300,382,"030");
		}else if(CH30_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(300,382,"030");
		}
		
		if(CH31_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(340,382,"031");
		}else if(CH31_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(340,382,"031");
		}
		
		if(CH32_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(380,382,"032");
		}else if(CH32_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(380,382,"032");
		}
		
		if(CH33_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(100,506,"033");
		}else if(CH33_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(100,506,"033");
		}
		
		if(CH34_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(140,506,"034");
		}else if(CH34_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(140,506,"034");
		}
		
		if(CH35_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(180,506,"035");
		}else if(CH35_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(180,506,"035");
		}
		
		if(CH36_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(220,506,"036");
		}else if(CH36_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(220,506,"036");
		}
		
		if(CH37_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(260,506,"037");
		}else if(CH37_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(260,506,"037");
		}
		
		if(CH38_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(300,506,"038");
		}else if(CH38_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(300,506,"038");
		}
		
		if(CH39_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(340,506,"039");
		}else if(CH39_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(340,506,"039");
		}
		
		if(CH40_SW == ch_off)
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			DISP_CNL_S(380,506,"040");
		}else if(CH40_SW == ch_on){
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_HLT);
			DISP_CNL_S(380,506,"040");
		}
	}
	press = 1;
	ButtonSound();//按键音
	if(LANG == chs)
	{
		DrawInstruction("通道设置");
	}else{
		DrawInstruction("Channel set");
	}
	Save_flag();
}

void XYCAL(u8 step)
{
	static vu32 Modify_A_READ;
	static vu32 Modify_C_READ;
	static vu32 Modify_A_ACT;
		
	static vu32 Modify_B_READ;
	static vu32 Modify_D_READ;
	static vu32 Modify_B_ACT;
	
    if(step == 1)
    {
        Modify_A_READ += Xconvert;//测量电压值		
    }else if(step == 2){
		Modify_A_READ += Xconvert;
		Modify_A_READ = Modify_A_READ/2;
		Modify_A_ACT = 0x0A;//读取低段
	}else if(step == 3){
		Modify_B_READ += Xconvert;
	}else if(step == 4){
        vu16 var16;
        vu32 var32a;
        vu32 var32b;
        
        vu16 var16a;
        vu32 var32c;
        vu32 var32d;
        Modify_B_READ +=Xconvert;//测量电压值
		Modify_B_READ = Modify_B_READ/2;

        Modify_B_ACT = 0x0276;//读取高段
//        if(flag_OverV==1)//只有当有数据写入时才能将校准数据写入FLASH
//        {
            var32a = Modify_B_ACT;
            var32a = var32a - Modify_A_ACT;
            var32a = var32a << 12;
            var16 = Modify_B_READ - Modify_A_READ;
            var32a = var32a / var16;
            XCOR = var32a;
            var32a=0;
            var32a = Modify_B_ACT;
            var32a = var32a << 12;
            var32b = Modify_B_READ;
            var32b = var32b * XCOR;
            if (var32a < var32b)
            {
                var32b = var32b - var32a;
                XOffset = var32b;
                Polar1 |= 0x01;
            }
            else 
            {
                var32a = var32a - var32b;
                XOffset = var32a;
                Polar1 &= ~0x01;
            }
//        }
		Save_flag();
		Modify_A_READ = 0;
		Modify_C_READ = 0;
		Modify_A_ACT = 0;
		Modify_B_READ = 0;
		Modify_D_READ = 0;
		Modify_B_ACT = 0;
		
    }
	
	if(step == 5)
    {
        Modify_A_READ += Yconvert;//测量电压值		
    }else if(step == 6){
		Modify_A_READ += Yconvert;
		Modify_A_READ = Modify_A_READ/2;
		Modify_A_ACT = 0x0A;//读取低段
	}else if(step == 7){
		Modify_B_READ += Yconvert;
	}else if(step == 8){
        vu16 var16;
        vu32 var32a;
        vu32 var32b;
        
        vu16 var16a;
        vu32 var32c;
        vu32 var32d;
        Modify_B_READ +=Yconvert;//测量电压值
		Modify_B_READ = Modify_B_READ/2;

        Modify_B_ACT = 0x01D6;//读取高段
//        if(flag_OverV==1)//只有当有数据写入时才能将校准数据写入FLASH
//        {
            var32a = Modify_B_ACT;
            var32a = var32a - Modify_A_ACT;
            var32a = var32a << 12;
            var16 = Modify_B_READ - Modify_A_READ;
            var32a = var32a / var16;
            YCOR = var32a;
            var32a=0;
            var32a = Modify_B_ACT;
            var32a = var32a << 12;
            var32b = Modify_B_READ;
            var32b = var32b * YCOR;
            if (var32a < var32b)
            {
                var32b = var32b - var32a;
                YOffset = var32b;
                Polar2 |= 0x01;
            }
            else 
            {
                var32a = var32a - var32b;
                YOffset = var32a;
                Polar2 &= ~0x01;
            }
//        }
		Save_flag();
		Modify_A_READ = 0;
		Modify_C_READ = 0;
		Modify_A_ACT = 0;
		Modify_B_READ = 0;
		Modify_D_READ = 0;
		Modify_B_ACT = 0;
		
    }
}