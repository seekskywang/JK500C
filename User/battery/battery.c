#include "stm32f4xx.h"
#include "battery.h"

void Battery_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOG,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=BATCAP_PIN4;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(BATCAP_PORT4,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=BATCAP_PIN3;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(BATCAP_PORT3,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=BATCAP_PIN2;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(BATCAP_PORT2,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=BATCAP_PIN1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(BATCAP_PORT1,&GPIO_InitStructure);
}