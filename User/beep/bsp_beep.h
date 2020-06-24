#ifndef __BEEP_H_
#define	__BEEP_H_

#include "stm32f4xx.h"


/******************** BEEP 引脚配置参数定义 **************************/
#define BEEP_GPIO_CLK      RCC_AHB1Periph_GPIOI
#define BEEP_PORT          GPIOI
#define BEEP_PIN           GPIO_Pin_5
#define BEEP_OCPWM_PINSOURCE	  GPIO_PinSource5
#define BEEP_OCPWM_AF		  GPIO_AF_TIM8


/******************** BEEP 函数宏定义 **************************/
#define BEEP_ON            TIM_CtrlPWMOutputs(BEEP_TIM, ENABLE);
#define BEEP_OFF           TIM_CtrlPWMOutputs(BEEP_TIM, DISABLE);


/************************** BEEP 函数声明********************************/
void Beep_GPIO_Config(void);



#endif /* __BEEP_H_ */
