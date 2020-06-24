#ifndef __BASIC_TIM_H
#define	__BASIC_TIM_H

#include "stm32f4xx.h"

#define BASIC_TIM           			TIM6
#define BASIC_TIM_CLK       			RCC_APB1Periph_TIM6

#define BASIC_TIM_IRQn					TIM6_DAC_IRQn
#define BASIC_TIM_IRQHandler    		TIM6_DAC_IRQHandler

#define GENERAL_TIM           		    TIM5
#define GENERAL_TIM_CLK       		    RCC_APB1Periph_TIM5

#define GENERAL_TIM_IRQn				TIM5_IRQn
#define GENERAL_TIM_IRQHandler       	TIM5_IRQHandler

#define BEEP_TIM           				TIM8
#define BEEP_TIM_CLK       				RCC_APB2Periph_TIM8 

#define BEEP_TIM_IRQn					TIM8_IRQn
#define BEEP_TIM_IRQHandler    			TIM8_IRQHandler

void TIMx_Configuration(void);
void TIM_PWMOUTPUT_Config(u8 duty);
#endif /* __BASIC_TIM_H */


