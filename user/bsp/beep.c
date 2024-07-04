#include "beep.h"
#include "delay.h"
void Beep_Config(void)
{
	RCC->APB2ENR |= (1<<4);
	GPIOC->CRL &= ~(0xF<<0);
	GPIOC->CRL |= (0x3<<0);
	GPIOC->ODR &= ~(0x1<<0);
}


void Beep_On(void)
{
	GPIOC->ODR |= (0x1<<0);
}

void Beep_Off(void)
{
	GPIOC->ODR &= ~(0x1<<0);
}


void Beep_Toggle()
{
	GPIOC->ODR ^= (0x1<<0);
}



