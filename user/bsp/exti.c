#include "exti.h"
#include "beep.h"
#include "led.h"

#include "rtc.h"
void EXTI0_IRQHandler(void)
{
#ifdef STD_LIB
	if(EXTI_GetITStatus(EXTI_Line0)==SET)
	{
		//Alarm_Value--;
		Alarm_Settime.tm_min--;
		Alarm_Set();
			Led_Toggle(0);//Beep_On();
		EXTI_ClearITPendingBit(EXTI_Line0);
	
	}
	
#else
	if(EXTI->PR & (1<<0) != 0)
	{
		Beep_On();
		EXTI->PR |=(1<<0);
	}
#endif
}
void EXTI4_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line4)==SET)
	{
		Alarm_Settime.tm_min++;
		Alarm_Value++;
		Alarm_Set();
			Led_Toggle(1);
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}
void EXTI9_5_IRQHandler(void){
		if(EXTI_GetITStatus(EXTI_Line5)==SET)
	{
		Alarm_Settime.tm_hour--;
		Alarm_Set();
			Led_Toggle(2);
		EXTI_ClearITPendingBit(EXTI_Line5);
	}
	if(EXTI_GetITStatus(EXTI_Line6)==SET)
	{
		Alarm_Settime.tm_hour++;
		Alarm_Set();
			Led_Toggle(3);
		EXTI_ClearITPendingBit(EXTI_Line6);
	}
}
void EXTI0_Init(void)
{
#ifdef STD_LIB
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	

	
	
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode =GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin =GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource4);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource5);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource6);
	EXTI_InitStructure.EXTI_Line =EXTI_Line4|EXTI_Line5|EXTI_Line6;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_Init(&NVIC_InitStructure);
#else
	RCC->APB2ENR |= (1<<2);
	GPIOA->CRL &= ~(0xF<<0);
	GPIOA->CRL |=(0x4<<0);
	RCC->APB2ENR |= (1<<0);
	AFIO->EXTICR[0] &= ~(0xF<<0);
	EXTI->IMR |=(1<<0);
	EXTI->RTSR |=(1<<0);
	EXTI->PR |=(1<<0);//Ð´Èë1 
	NVIC_SetPriority(EXTI0_IRQn,6);
	NVIC_EnableIRQ(EXTI0_IRQn);
		
#endif
	
	
}

