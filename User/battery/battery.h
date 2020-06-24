#ifndef _battery_H
#define _battery_H


//#define BATCAP_PIN4							GPIO_Pin_4
//#define BATCAP_PORT4          				GPIOB
//#define BATCAP_CLK4              			RCC_AHB1Periph_GPIOB
//#define BATCAP_PINSOURCE4            		GPIO_PinSource4

//#define BATCAP_PIN3							GPIO_Pin_11
//#define BATCAP_PORT3          				GPIOI
//#define BATCAP_CLK3              			RCC_AHB1Periph_GPIOI
//#define BATCAP_PINSOURCE3            		GPIO_PinSource11

//#define BATCAP_PIN2							GPIO_Pin_2
//#define BATCAP_PORT2          				GPIOE
//#define BATCAP_CLK2              			RCC_AHB1Periph_GPIOC
//#define BATCAP_PINSOURCE2            		GPIO_PinSource2

//#define BATCAP_PIN1							GPIO_Pin_14
//#define BATCAP_PORT1          				GPIOG
//#define BATCAP_CLK1              			RCC_AHB1Periph_GPIOG
//#define BATCAP_PINSOURCE1            		GPIO_PinSource14

#define BATCAP_PIN4							GPIO_Pin_2
#define BATCAP_PORT4          				GPIOG
#define BATCAP_CLK4              			RCC_AHB1Periph_GPIOG
#define BATCAP_PINSOURCE4            		GPIO_PinSource2

#define BATCAP_PIN3							GPIO_Pin_3
#define BATCAP_PORT3          				GPIOG
#define BATCAP_CLK3              			RCC_AHB1Periph_GPIOG
#define BATCAP_PINSOURCE3            		GPIO_PinSource3

#define BATCAP_PIN2							GPIO_Pin_6
#define BATCAP_PORT2          				GPIOC
#define BATCAP_CLK2              			RCC_AHB1Periph_GPIOC
#define BATCAP_PINSOURCE2            		GPIO_PinSource6

#define BATCAP_PIN1							GPIO_Pin_6
#define BATCAP_PORT1          				GPIOG
#define BATCAP_CLK1              			RCC_AHB1Periph_GPIOG
#define BATCAP_PINSOURCE1            		GPIO_PinSource6
void Battery_GPIO_Init(void);

#endif