#include "stm32f4xx.h"
#include "./ch376/ch376.h"
#include "./systick/bsp_SysTick.h"
#include "jk508.h"

static __IO uint32_t  SPITimeout = SPIT_LONG_TIMEOUT;   

static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);



void CH376_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	
	RCC_AHB1PeriphClockCmd(CH376_INT_CLK|CH376_SCK_CLK|CH376_SCS_CLK|CH376_MOSI_CLK|CH376_MISO_CLK,ENABLE);	/*开启按键GPIO口的时钟*/
	CH376_SPI_CLK_INIT(CH376_SPI_CLK,ENABLE);

	
	 //设置引脚复用
	GPIO_PinAFConfig(CH376_SCK_PORT,CH376_SPI_SCK_PINSOURCE,CH376_SPI_SCK_AF); 
	GPIO_PinAFConfig(CH376_MISO_PORT,CH376_SPI_MISO_PINSOURCE,CH376_SPI_MISO_AF); 
	GPIO_PinAFConfig(CH376_MOSI_PORT,CH376_SPI_MOSI_PINSOURCE,CH376_SPI_MOSI_AF); 
	
	/*!< 配置 CH376 引脚: SCK */
	GPIO_InitStructure.GPIO_Pin = CH376_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
	  
	GPIO_Init(CH376_SCK_PORT, &GPIO_InitStructure);
	  
    /*!< 配置 CH376 引脚: MISO */
	GPIO_InitStructure.GPIO_Pin = CH376_MISO_PIN;
	GPIO_Init(CH376_MISO_PORT, &GPIO_InitStructure);
	
	/*!< 配置 CH376 引脚: MOSI */
	GPIO_InitStructure.GPIO_Pin = CH376_MOSI_PIN;
	GPIO_Init(CH376_MOSI_PORT, &GPIO_InitStructure);  	
	
	/*!< 配置 CH376 引脚: CS */
    GPIO_InitStructure.GPIO_Pin = CH376_SCS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(CH376_SCS_PORT, &GPIO_InitStructure);
	
	/*!< 配置 CH376 引脚: INT */
	GPIO_InitStructure.GPIO_Pin = CH376_INT_PIN;//CH376 INT
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;   //输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CH376_INT_PORT, &GPIO_InitStructure);
	
	
	
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//选择了串行时钟的稳态:时钟悬空高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//数据捕获于第二个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	
	SPI_Cmd(SPI1, ENABLE); //使能SPI外设
}
/*******************************************************************************
* 描      述      : 延时指定微秒时间,根据单片机主频调整,不精确.
*******************************************************************************/
void mDelayuS( u8 us )
{
	while ( us -- );
}

/*******************************************************************************
* 描      述      : 延时指定毫秒时间,根据单片机主频调整,不精确
*******************************************************************************/
void mDelaymS( u8 ms )
{
	while ( ms -- ) 
	{
		mDelayuS( 250 );
		mDelayuS( 250 );
		mDelayuS( 250 );
		mDelayuS( 250 );
	}
}

/*******************************************************************************
* 描      述      : 向CH376写  命令.
* 输      入      : UINT8 mCmd:要发送的命令.
*******************************************************************************/
void xWriteCH376Cmd(u8 mCmd)
{
//	SPI_CH376_SCS_HIGH();    /* 防止之前未通过xEndCH376Cmd禁止SPI片选 */
	GPIO_SetBits(CH376_SCS_PORT,CH376_SCS_PIN);
	Delay(10);
	Delay(10);
/* 对于双向I/O引脚模拟SPI接口,那么必须确保已经设置SPI_SCS,SPI_SCK,SPI_SDI为输出
*  方向,SPI_SDO为输入方向 */
	GPIO_ResetBits(CH376_SCS_PORT,CH376_SCS_PIN);/* SPI片选有效 */
//	SPI_CH376_SCS_LOW();      /* SPI片选有效 */
	SPIx_ReadWriteByte(mCmd);  /* 发出命令码 */
	Delay(10); 
	Delay(10); 
	Delay(10);   /* 延时1.5uS确保读写周期大于1.5uS,或者用上面一行的状态查询代替 */
	Delay(10); 
}


/*******************************************************************************
* 描      述      : 向CH376写   数据.
* 输      入      : UINT8 mData:
*                   要发送的数据.
*******************************************************************************/
void xWriteCH376Data(u8 mData)
{
	SPIx_ReadWriteByte(mData);
	Delay(10);  /* 确保读写周期大于0.6uS */
}

/*******************************************************************************
* 函  数  名      : xReadCH376Data
* 描      述      : 从CH376读数据.
*******************************************************************************/
u8 xReadCH376Data(void)
{
	u8 i;
	Delay(10);
	i = SPIx_ReadWriteByte( 0xFF );
	return( i );
}

/*******************************************************************************
* 函  数  名      : xEndCH376Cmd   结束命令.
*******************************************************************************/

void xEndCH376Cmd(void)
{
	SPI_CH376_SCS_HIGH();/* 禁止SPI片选 */
}

//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPIx_ReadWriteByte(u8 TxData)
{		
		SPITimeout = SPIT_FLAG_TIMEOUT;

	  /* 等待发送缓冲区为空，TXE事件 */
	  while (SPI_I2S_GetFlagStatus(CH376_SPI, SPI_I2S_FLAG_TXE) == RESET)
	   {
		if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
	   }

	  /* 写入数据寄存器，把要写入的数据写入发送缓冲区 */
	  SPI_I2S_SendData(CH376_SPI,TxData);

	  SPITimeout = SPIT_FLAG_TIMEOUT;

	  /* 等待接收缓冲区非空，RXNE事件 */
	  while (SPI_I2S_GetFlagStatus(CH376_SPI, SPI_I2S_FLAG_RXNE) == RESET)
	   {
		if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(1);
	   }

	  /* 读取数据寄存器，获取接收缓冲区数据 */
	  return SPI_I2S_ReceiveData(CH376_SPI);			    
}

/*******************************************************************************
* 描      述      : 查询CH376中断(INT#低电平).
* 返      回      : FALSE:无中断.       TRUE:有中断.
*******************************************************************************/
u8 Query376Interrupt(void)
{
	u8 i;
	i = (GPIO_ReadInputDataBit(CH376_INT_PORT,CH376_INT_PIN) ? FALSE : TRUE ); 	/* 如果连接了CH376的中断引脚则直接查询中断引脚 */
	return( i ); 
}


/*******************************************************************************
* 描      述      : 初始化CH376.
* 返      回      : FALSE:无中断.  TRUE:有中断.
*******************************************************************************/
u8 Init_CH376(void)
{
	u8 res;
	Delay(200);
	Delay(200);
	Delay(200);
	
//	SPI_CH376_SCS_HIGH();/* 禁止SPI片选 */
	GPIO_SetBits(CH376_SCS_PORT,CH376_SCS_PIN);/* 禁止SPI片选 */
	GPIO_SetBits(CH376_INT_PORT,CH376_INT_PIN);
	
	xWriteCH376Cmd( CMD11_CHECK_EXIST );
	xWriteCH376Data(0x55);
	res = xReadCH376Data();
	xEndCH376Cmd();
	if (res != 0xAA) 		
	{
		return(ERR_USB_UNKNOWN);  /* 通讯接口不正常,可能原因有:接口连接异常,其它设备影响(片选不唯一),串口波特率,一直在复位,晶振不工作 */
	}
//	else{
//		return(CH376OK);
//	}
	xWriteCH376Cmd( CMD11_SET_USB_MODE ); /* 设备USB工作模式 */
	xWriteCH376Data( 0x06 );
	Delay(20);
	res = xReadCH376Data();
	xEndCH376Cmd();
	
	if ( res == CMD_RET_SUCCESS )  //RES=51  命令操作成功
	{
	    return( USB_INT_SUCCESS ); //USB事务或者传输操作成功 
	}
	else 
	{
	    return( ERR_USB_UNKNOWN );/* 设置模式错误 */
	}
}

/**
  * @brief  等待超时回调函数
  * @param  None.
  * @retval None.
  */
static  uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* 等待超时后的处理,输出错误信息 */
	FLASH_ERROR("SPI 等待超时!errorCode = %d",errorCode);
  return 0;
}

