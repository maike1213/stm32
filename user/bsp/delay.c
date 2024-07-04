#include "delay.h"
#include "led.h"
#include "beep.h"
#include "key.h"
uint32_t mstick=0;

volatile uint32_t systime=0;
volatile uint32_t Dht11TaskTick[2] = {0,1000};
volatile uint32_t ledTask[2] ={0,100};
volatile uint32_t KeyTaskTick[2] ={0,10};
extern void FontTick(void);

//1ms中断一次
void SysTick_Handler(void)
{
	
	mstick++;
	systime++;
	KeyTaskTick[0]++;
	Dht11TaskTick[0]++;
	ledTask[0]++;
	FontTick();

	
}
void TIM6_IRQHandler()
{
		
#ifdef STD_LIB
	if(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update) ==SET)
	{
		//Led_Run(1);
		TIM_ClearFlag(TIM6,TIM_FLAG_Update);
	}
#else
	if(TIM6->SR & (1<<0) !=0)
	{
		Led_Run(1);
		TIM6->SR = ~(1<<0);
	}
#endif

	
	
}
void SysTick_Init(void)
{
	if(SysTick_Config(72000))
	{
		while(1);
	}
	
}
void TIM2_Init(uint16_t psc,uint16_t arr)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision =0;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = arr-1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc-1;
	//TIM_TimeBaseInitStructure.TIM_RepetitionCounter = ;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	TIM_ARRPreloadConfig(TIM2,ENABLE);
	TIM_SetClockDivision(TIM2,1);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM2,ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
}
void TIM6_Init(uint16_t psc,uint16_t arr)
{
	
	
#ifdef STD_LIB
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision =0;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = arr-1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc-1;
	//TIM_TimeBaseInitStructure.TIM_RepetitionCounter = ;
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStructure);
	TIM_ARRPreloadConfig(TIM6,ENABLE);
	TIM_SetClockDivision(TIM6,1);
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM6,ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
#else
	RCC->APB1ENR |= (1<<4);
	TIM6->CR1 = 0;
	TIM6->CR1 = (1<<7);
	TIM6->CR2 = 0;
	TIM6->DIER = 0;
	TIM6->DIER |= (1<<0);
	TIM6->SR &= ~(1<<0);
	
//	TIM6->EGR = 1;//事件
	TIM6->CNT = 0;
	TIM6->PSC = psc-1;
	TIM6->ARR = arr-1;
	TIM6->CR1 = (1<<0);
	NVIC_EnableIRQ(TIM6_IRQn);
	NVIC_SetPriority(TIM6_IRQn,14);
	
#endif

}



//void Delay_ms(uint32_t t)
//{
//	uint64_t T = t*1000;
//	while(T--) {
//		delay_1us();
//	}
//}

void Delay_ms(uint32_t t)
{
	uint32_t time = systime+t;
 	while(time >systime)
	{
	}
}
void Delay_us(uint32_t t)
{
	while(t--) {
		delay_1us();
	}
}











