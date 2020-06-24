#ifndef __SD_SPI_H
#define __SD_SPI_H

#include "stm32f4xx.h"
#include <stdio.h>


/*SD卡SPI接口定义-开头****************************/
#define SD_SPI                           SPI3
#define SD_SPI_CLK                       RCC_APB2Periph_SPI3
#define SD_SPI_CLK_INIT                  RCC_APB2PeriphClockCmd

#define SD_SPI_SCK_PIN                   GPIO_Pin_10                  
#define SD_SPI_SCK_GPIO_PORT             GPIOC                       
#define SD_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOC
#define SD_SPI_SCK_PINSOURCE             GPIO_PinSource10
#define SD_SPI_SCK_AF                    GPIO_AF_SPI3

#define SD_SPI_MISO_PIN                  GPIO_Pin_11                
#define SD_SPI_MISO_GPIO_PORT            GPIOC                   
#define SD_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOC
#define SD_SPI_MISO_PINSOURCE            GPIO_PinSource11
#define SD_SPI_MISO_AF                   GPIO_AF_SPI3

#define SD_SPI_MOSI_PIN                  GPIO_Pin_12                
#define SD_SPI_MOSI_GPIO_PORT            GPIOC                      
#define SD_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOC
#define SD_SPI_MOSI_PINSOURCE            GPIO_PinSource12
#define SD_SPI_MOSI_AF                   GPIO_AF_SPI3

#define SD_CS_PIN                        GPIO_Pin_6               
#define SD_CS_GPIO_PORT                  GPIOF                     
#define SD_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOF


#endif