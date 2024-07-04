#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"

#define delay_1us()	{\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();\
}
extern volatile uint16_t cnt;
extern volatile uint32_t Dht11TaskTick[2];
extern volatile uint32_t ledTask[2];
extern volatile uint32_t KeyTaskTick[2];
void Delay_us(uint32_t t);
void Delay_ms(uint32_t t);
void Systick_Config(void);
void SysTick_Init(void);
void TIM2_Init(uint16_t psc,uint16_t arr);
void TIM6_Init(uint16_t psc,uint16_t arr);
#endif
