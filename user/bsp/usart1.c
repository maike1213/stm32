#include "usart1.h"
#include "stdio.h"
#include "string.h"
#include "led.h"
#include "time.h"
#include "rtc.h"
#include "spi.h"
uint8_t usart1RxData = 0;
uint16_t temp[2]={0};
USART1_DATA_Def usart1Data = {0};
#define FONT_SAVE_ADDR 0
uint32_t addrIndex = 0;
uint32_t tick = 0;
void FontTick(void)
{
	if(tick) tick++;
}
uint16_t buff_w25[100]={0};

void FontTickOver(void)
{
	if(tick>100){
		printf("---完成---%d-字节\r\n",addrIndex);
	addrIndex = 0;
	tick = 0;
		W25Q_PageReadhald(0,buff_w25,100);
	for(int i=0;i<100;i++){
		printf("%0x ",buff_w25[i]);
	}
	
	}
}
uint16_t cnt = 0;
uint16_t temp_falg=0;
void USART1_IRQHandler(void)
{
	if((USART1->SR & (1<<5)) != 0)//接收一个字节数据
	{
		usart1RxData = USART1->DR;//读取数据，自动清中断
			W25Q_Pagewrite(addrIndex+FONT_SAVE_ADDR,&usart1RxData,1);
//	temp[cnt%2] = usart1RxData;
//	if((cnt%2) == 1) {
//		temp_falg =(temp[0]<<8) + (temp[1]);
//		W25Q_Pagewritehald(addrIndex+FONT_SAVE_ADDR,&temp_falg,1);

//	}
//	cnt++;
		addrIndex++;
		tick =1;
	}

}




//串口1的配置
//发送、 接收、波特率传参进来
//USART1  TX PA9		RX PA10
void Usart1_Config(uint32_t brr)
{

	/*1 配置通信引脚的模式 PA9 PA10*/
	//打开GPIOA的时钟
	RCC->APB2ENR |= (1<<2);
	//PA9 复用推挽输出最高速
	GPIOA->CRH &= ~(0xF<<4);//清0
	GPIOA->CRH |= (0xB<<4);//置1
	//PA10 浮空输入
	GPIOA->CRH &= ~(0xF<<8);//清0
	GPIOA->CRH |= (0x4<<8);//置1
	
	/*2 配置串口的工作模式：
			开始钟
			开始位、数据位、
			奇偶校验位、停止位 波特率 发送器和接收器打开
			如果开中断，配置中断*/
	RCC->APB2ENR |= (1<<14);
	
	float div = 72000000/16/brr+0.5;
	uint32_t Mantissa = div;
	uint32_t Fraction = (div-Mantissa)*16;
	USART1->BRR = (Mantissa<<4)|Fraction;
	
	USART1->CR1 = 0;
	USART1->CR1 &= ~(1<<12);//8bit数据长度
	USART1->CR1 &= ~(1<<10);//关闭校验
	USART1->CR1 |= (1<<3);//打开发送器
	USART1->CR1 |= (1<<2);//打开接收器
	
	USART1->CR2 = 0;
	USART1->CR2 &= ~(0x3<<12);//一个停止位
	
	/*3 如果开中断，NVIC要配置*/
	USART1->CR1 |= (1<<5);//打开RXNE中断，收到一个数据长度，就会来一次中断
	//USART1->CR1 |= (1<<4);//打开空闲中断
	NVIC_SetPriority(USART1_IRQn, 3);
	NVIC_EnableIRQ(USART1_IRQn);
	
	/*4 启动串口，使能串口*/
	USART1->CR1 |= (1<<13);
}


void Usart1_SendByte(uint8_t data)
{
	//检查上一次的数据是否发送完成
	while((USART1->SR & (1<<6)) == 0)
	{}
	USART1->DR = data;//发送数据
}

//阻塞接收
uint8_t Usart1_RecvByte(void)
{
	//检查是否接收到数据
	while((USART1->SR & (1<<5)) == 0)
	{}
	return USART1->DR;//发送数据
}


void Usart1_SendStr(char str[])
{
	uint32_t i = 0;
	while(str[i] != '\0') {
		while((USART1->SR & (1<<6)) == 0)
		{}
		USART1->DR = str[i];//发送数据
		i++;
	}
}

void Usart1_SendBuff(uint8_t buff[], uint16_t len)
{
	uint32_t i = 0;
	for(i=0; i<len; i++)
	{
		Usart1_SendByte(buff[i]);
	}
}



/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{
	Usart1_SendByte(ch);
  return ch;
}


