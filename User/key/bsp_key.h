#ifndef __KEY_H
#define	__KEY_H

#include "stm32f4xx.h"

//引脚定义
/*******************************************************/
#define P1_PIN                  GPIO_Pin_9                 
#define P1_GPIO_PORT            GPIOH                      
#define P1_GPIO_CLK             RCC_AHB1Periph_GPIOH

#define P2_PIN                  GPIO_Pin_10                 
#define P2_GPIO_PORT            GPIOH                      
#define P2_GPIO_CLK             RCC_AHB1Periph_GPIOH

#define P3_PIN                  GPIO_Pin_5                 
#define P3_GPIO_PORT            GPIOC                      
#define P3_GPIO_CLK             RCC_AHB1Periph_GPIOC

#define P4_PIN                  GPIO_Pin_11                 
#define P4_GPIO_PORT            GPIOH                      
#define P4_GPIO_CLK             RCC_AHB1Periph_GPIOH

//#define P3_PIN                  GPIO_Pin_11                 
//#define P3_GPIO_PORT            GPIOH                      
//#define P3_GPIO_CLK             RCC_AHB1Periph_GPIOH

//#define P4_PIN                  GPIO_Pin_12                 
//#define P4_GPIO_PORT            GPIOH                      
//#define P4_GPIO_CLK             RCC_AHB1Periph_GPIOH

#define P5_PIN                  GPIO_Pin_1                 
#define P5_GPIO_PORT            GPIOI                      
#define P5_GPIO_CLK             RCC_AHB1Periph_GPIOI

#define P6_PIN                  GPIO_Pin_3                 
#define P6_GPIO_PORT            GPIOI                      
#define P6_GPIO_CLK             RCC_AHB1Periph_GPIOI
//#define P6_EXTI_PORTSOURCE      EXTI_PortSourceGPIOI
//#define P6_EXTI_PINSOURCE       EXTI_PinSource3
//#define P6_EXTI_LINE            EXTI_Line3
//#define P6_EXTI_IRQ             EXTI0_IRQn

//#define P6_IRQHandler           EXTI0_IRQHandler

/*******************************************************/

 /** 按键按下标置宏
	* 按键按下为高电平，设置 KEY_ON=1， KEY_OFF=0
	* 若按键按下为低电平，把宏设置成KEY_ON=0 ，KEY_OFF=1 即可
	*/
#define KEY_ON	1
#define KEY_OFF	0
/*******************************************************/

 /** 键值
	* 
	*/
#define KEY_1		0x16
#define KEY_2		0x0C
#define KEY_3		0x11
#define KEY_4		0x18
#define KEY_5		0x0D
#define KEY_6		0x14
#define KEY_7		0x19
#define KEY_8		0x0E
#define KEY_9		0x13
#define KEY_0		0x08
#define KEY_DOT		0x09
#define KEY_BACK	0x0A
#define KEY_FUNC1	0x15
#define KEY_FUNC2	0x0B
#define KEY_FUNC3	0x10
#define KEY_FUNC4	0x06
#define KEY_FUNC5	0x05
#define KEY_UP		0x0F
#define KEY_DOWN	0x07
#define KEY_LEFT	0x17
#define KEY_RIGHT	0x02
#define KEY_ESC		0x01
#define KEY_ENTER	0x03
#define KEY_ACC		0x12
/*******************************************************/	
void Key_GPIO_Config(void);
void Key_Scan(void);
void Key_Function(void);
void FUNC1_HANDLE(void);
void FUNC2_HANDLE(void);
void FUNC3_HANDLE(void);
void FUNC4_HANDLE(void);
void FUNC5_HANDLE(void);
void ENTER_HANDLE(void);
void ESC_HANDLE(void);
void UP_HANDLE(void);
void DOWN_HANDLE(void);
void LEFT_HANDLE(void);
void RIGHT_HANDLE(void);
void KEY1_HANDLE(void);
void KEY2_HANDLE(void);
void KEY3_HANDLE(void);
void KEY4_HANDLE(void);
void KEY5_HANDLE(void);
void KEY6_HANDLE(void);
void KEY7_HANDLE(void);
void KEY8_HANDLE(void);
void KEY9_HANDLE(void);
void KEY0_HANDLE(void);
void DOT_HANDLE(void);
void BACK_HANDLE(void);
void ACC_HANDLE(void);
void input_num(char* num);
void input_pass(char* num);
void clear_input(void);
void del_num(void);
void Utest(void);
void Input_start(u8 num);
void Input_end(u8 num);
//uint8_t Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin);

#endif /* __LED_H */

