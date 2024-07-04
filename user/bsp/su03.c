#include "su03.h"
#include "stdio.h"
#include "string.h"
#include "usart5.h"
#include "dht11.h"
#include "led.h"
#include "adc.h"
#include "time.h"
#include "rtc.h"
uint8_t uart4RxData = 0;
UART4_DATA_Def uart4Data = {0};

void UART4_IRQHandler(void)
{
	if(USART_GetITStatus(UART4, USART_IT_RXNE) == SET)//接收一个字节数据
	{
		uart4RxData = USART_ReceiveData(UART4);//读取数据，自动清中断
		uart4Data.buff[uart4Data.cnt++] = uart4RxData;
		uart4Data.cnt %= UART4_RX_MAX;
	}
	if(USART_GetITStatus(UART4, USART_IT_IDLE) == SET)//空闲 
	{
																						//先读SR
		uart4RxData = USART_ReceiveData(UART4);	//再读DR，清中断
		uart4Data.overFlag = 1;	
	}
}

void Uart4_DataAnalysis(void)
{ 
	if(uart4Data.overFlag == 0) return;
	if(uart4Data.cnt == 0) {
		uart4Data.overFlag = 0;
		return;
	}
	
	uint16_t i=0;
	printf("UART4收到%d个字节数据\r\n", uart4Data.cnt);
	for(i=0; i<uart4Data.cnt; i++){
		printf("%02X ", uart4Data.buff[i]);
	}
	printf("\r\n");
	
	if(uart4Data.cnt<3) {
		printf("数据不完整\r\n");
		goto end;
	}
	
	if((uart4Data.buff[0]!=0xAA) || (uart4Data.buff[1]!=0x55)) {
		printf("帧头错误\r\n");
		goto end;
	}
	
	switch(uart4Data.buff[2])
	{
		case 0x01:
			printf("收到 播报空气质量|空气质量怎么样 指令\r\n");
			Su03T_Play(1, kqm6600.voc);
		break;
		case 0x02:printf("收到 播报甲醛|播报甲醛浓度 指令\r\n");
		Su03T_Play(2, kqm6600.ch2o);
		break;
		case 0x03:printf("收到 播报二氧化碳浓度|播报二氧化碳含量 指令\r\n");
		Su03T_Play(3, kqm6600.co2);
		break;
		case 0x04:printf("收到 播报当前湿度 指令\r\n");
		Su03T_Play(4, dht11.hum);
		break;
		case 0x05:printf("收到 播报当前温度 指令\r\n");
		Su03T_Play(5, dht11.tem);
		break;
		case 0x06:printf("收到 播报光照强度 指令\r\n");
		if(adclight.adc >4030)
		{
			Su03T_Play(6,adclight.adc);
		}else if(adclight.adc <4030 && adclight.adc >3000)
		{
			Su03T_Play(7,adclight.adc);
		}
		else if(adclight.adc <3000)
		{
			Su03T_Play(8,adclight.adc);
		}
		break;
		case 0x07:printf("收到 播报烟雾浓度 指令\r\n");
		Su03T_Play(9,adcMq2.v);
		break;
		case 0x10:printf("收到 播报当前时间 指令\r\n");
		time_play();
		break;
		case 0x11:printf("收到 设置闹钟 指令\r\n");
		//time_play();
		if(alarm_flag ==1){
			Su03T_Play(11,0);
		}
		break;
		case 0x22:printf("收到 播报开灯 指令\r\n");
		Led_On(1);
		break;
		case 0x23:printf("收到 播报关灯 指令\r\n");
		Led_Off(1);
		break;
	}

	
	end:
	memset(uart4Data.buff, 0, sizeof(uart4Data.buff));
	uart4Data.cnt = 0;
	uart4Data.overFlag = 0;
}




void Uart4_Config(void)
{
	//配置IO  PC10TX  PC11RX
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	//配置UART4
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(UART4, &USART_InitStructure);
	//配置中断
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);
	NVIC_SetPriority(UART4_IRQn, 5);
	NVIC_EnableIRQ(UART4_IRQn);
	//启动串口
	USART_Cmd(UART4,ENABLE);
}

void Su03T_Init(void)
{
	Uart4_Config();
}


void Uart4_SendByte(uint8_t data)
{
	//检查上一次的数据是否发送完成
	while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET)
	{}
	USART_SendData(UART4, data);
}


void Uart4_SendBuff(uint8_t buff[], uint16_t len)
{
	uint32_t i = 0;
	for(i=0; i<len; i++)
	{
		Uart4_SendByte(buff[i]);
	}
}


void Su03T_Play(uint8_t msgNum, double value)
{
	uint8_t buff[16] = {0};
	uint8_t cnt = 0;
	uint8_t i=0;
	uint8_t *p = (uint8_t*)&value;

	//帧头和消息号
	buff[cnt++] = 0xAA;
	buff[cnt++] = 0x55;
	buff[cnt++] = msgNum;
	
	//不是light指令，需要参数
	if(msgNum !=6 ||msgNum==11) {
		for(i=0; i<8; i++) {
			buff[cnt++] = p[i];
		}
	}
		
	//帧尾
	buff[cnt++] = 0x55;
	buff[cnt++] = 0xAA;	
	
	Uart4_SendBuff(buff, cnt);

}
void time_play(void)
{
	uint8_t buff[32] = {0};
	uint8_t cnt = 0;
	uint8_t i=0;
	uint8_t *ph = (uint8_t*)&(nowtime->tm_hour);
	uint8_t *pm = (uint8_t*)&(nowtime->tm_min);
	//帧头和消息号
	buff[cnt++] = 0xAA;
	buff[cnt++] = 0x55;
	buff[cnt++] = 10;
		for(i=0; i<4; i++) {
			buff[cnt++] = ph[i];
		}
		for(i=0; i<4; i++) {
			buff[cnt++] = pm[i];
		}
	//帧尾
	buff[cnt++] = 0x55;
	buff[cnt++] = 0xAA;	
	
	Uart4_SendBuff(buff, cnt);
}
