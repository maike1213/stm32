#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"


#define Led1_Port			GPIOE
#define Led1_Pin			GPIO_Pin_2
#define Led1_Clock		RCC_APB2Periph_GPIOE

#define Led2_Port			GPIOE
#define Led2_Pin			GPIO_Pin_3
#define Led2_Clock		RCC_APB2Periph_GPIOE

#define Led3_Port			GPIOE
#define Led3_Pin			GPIO_Pin_4
#define Led3_Clock		RCC_APB2Periph_GPIOE

#define Led4_Port			GPIOE
#define Led4_Pin			GPIO_Pin_5
#define Led4_Clock		RCC_APB2Periph_GPIOE




typedef enum{
	LED1,
	LED2,
	LED3,
	LED4,
}LED_NUM;


void Led_Config(void);
void Led_On(LED_NUM num);
void Led_Off(LED_NUM num);
void Led_Toggle(LED_NUM num);
void Led_Run(uint8_t s);
#endif
