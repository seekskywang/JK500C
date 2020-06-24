#include "./sd_spi/sd_spi.h"

/*******************************************************************************
* Function Name  : SPI_FLASH_Init
* Description    : Initializes the peripherals used by the SPI FLASH driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_SD_Init(void)
{
 
  GPIO_InitTypeDef GPIO_InitStructure;
 
  /* 使能SPI对应引脚的时钟  使能SPI1的时钟 */
  RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_APB1Periph_SPI3, ENABLE);
 
  /*配置SPI的时钟线SCK和SPI的MOSI线和SPI的MISO线 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用功能的推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /*配置SPI的片选线：CSN */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /*  拉高CSN引脚，停止使能SD*/
  GPIO_SetBits(GPIOA,GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_8);
  GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);

  // 配置SPI,使它适合SD的特性
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //双线双向全双工
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;  //主器件
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//8位数据长度
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;  //时钟悬空时为高
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; //数据捕获于第2个时钟沿
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;   //NSS信号由外部管脚管理
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//波特率预分频值为4
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; //数据传输的第一个字节为MSB
  SPI_InitStructure.SPI_CRCPolynomial = 7;  //CRC的多项式
  SPI_Init(SPI1, &SPI_InitStructure);
  /* 使能SPI1  */
  SPI_Cmd(SPI1, ENABLE);
}
