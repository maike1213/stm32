#include "led.h"


void Led_Config(void)
{
	RCC_APB2PeriphClockCmd(Led1_Clock|Led2_Clock|Led3_Clock|Led4_Clock, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = Led1_Pin;
	GPIO_Init(Led1_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = Led2_Pin;
	GPIO_Init(Led2_Port, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = Led3_Pin;
	GPIO_Init(Led3_Port, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = Led4_Pin;
	GPIO_Init(Led4_Port, &GPIO_InitStructure);
	Led_Off(0);Led_Off(1);Led_Off(2);Led_Off(3);
//	//打开GPIOE的时钟
//	RCC->APB2ENR |= (1<<6);
//	//将PE2345 设置成通用推挽输出最高速
//	GPIOE->CRL &= ~(0xFFFF<<8);//清0
//	GPIOE->CRL |= (0x3333<<8);	//置1  0011
//	//PE2345输出高电平  灭
//	GPIOE->ODR |= (0xF<<2);
}

void Led_On(LED_NUM num)
{
//	switch(num)
//	{
//		case LED1: GPIOE->ODR &= ~(1<<2); break;
//		case LED2: GPIOE->ODR &= ~(1<<3); break;
//		case LED3: GPIOE->ODR &= ~(1<<4); break;
//		case LED4: GPIOE->ODR &= ~(1<<5); break;
//	}
	switch(num)
	{
		case LED1: GPIO_ResetBits(Led1_Port, Led1_Pin); break;
		case LED2: GPIO_ResetBits(Led2_Port, Led2_Pin); break;
		case LED3: GPIO_ResetBits(Led3_Port, Led3_Pin); break;
		case LED4: GPIO_ResetBits(Led4_Port, Led4_Pin); break;
	}
}

void Led_Off(LED_NUM num)
{
//	switch(num)
//	{
//		case LED1: GPIOE->ODR |= (1<<2); break;
//		case LED2: GPIOE->ODR |= (1<<3); break;
//		case LED3: GPIOE->ODR |= (1<<4); break;
//		case LED4: GPIOE->ODR |= (1<<5); break;
//	}
	switch(num)
	{
		case LED1: GPIO_SetBits(Led1_Port, Led1_Pin); break;
		case LED2: GPIO_SetBits(Led2_Port, Led2_Pin); break;
		case LED3: GPIO_SetBits(Led3_Port, Led3_Pin); break;
		case LED4: GPIO_SetBits(Led4_Port, Led4_Pin); break;
	}
}



//GPIOE  首地址   0x40011800
//GPIOE_CRL		(*(uint32_t *)(0x40011800+0x000))
//GPIOE_CRH		(*(uint32_t *)(0x40011800+0x004))
//GPIOE_IDR		(*(uint32_t *)(0x40011800+0x008))
//GPIOE_ODR		(*(uint32_t *)(0x40011800+0x00C))
//#define GPIOE_ODR		(*(uint32_t *)(0x40011800+0x00C))


void Led_Toggle(LED_NUM num)
{
	switch(num)
	{
		case LED1: GPIOE->ODR ^= (1<<2); break;
		case LED2: GPIOE->ODR ^= (1<<3); break;
		case LED3: GPIOE->ODR ^= (1<<4); break;
		case LED4: GPIOE->ODR ^= (1<<5); break;
	}
}



void Led_Run(uint8_t s)
{
	static uint8_t cnt = 0;
	if(s == 1) {
		switch(cnt)
		{
			case 0:Led_On(LED1);Led_Off(LED4);break;
			case 1:Led_On(LED2);Led_Off(LED1);break;
			case 2:Led_On(LED3);Led_Off(LED2);break;
			case 3:Led_On(LED4);Led_Off(LED3);break;
		}
		cnt++;
		cnt %= 4;
	}
	else {
		cnt=0;
		Led_Off(LED4); Led_Off(LED1); Led_Off(LED2); Led_Off(LED3);		
	}
}



