#include "usart5.h"
#include "string.h"
#include "stdio.h"
#include "delay.h"

KQM6600_DATA_Def kqm6600={0};

uint8_t uart5RxData = 0;
UART5_DATA_Def uart5Data = {0};

void UART5_IRQHandler(void)
{
	if(USART_GetITStatus(UART5, USART_IT_RXNE) == SET)//接收一个字节数据
	{
		uart5RxData = USART_ReceiveData(UART5);//读取数据，自动清中断
		uart5Data.buff[uart5Data.cnt++] = uart5RxData;
		uart5Data.cnt %= UART5_RX_MAX;
	}
	if(USART_GetITStatus(UART5, USART_IT_IDLE) == SET)//空闲 
	{
																						//先读SR
		uart5RxData = USART_ReceiveData(UART5);	//再读DR，清中断
		uart5Data.overFlag = 1;	
	}
	
}

void Uart5_DataAnalysis(void)
{ 
	if(uart5Data.overFlag == 0) return;
	if(uart5Data.cnt == 0) {
		uart5Data.overFlag = 0;
		return;
	}
	
	uint16_t i=0;
	printf("uart5收到%d个字节数据\r\n", uart5Data.cnt);
	for(i=0; i<uart5Data.cnt; i++){
		printf("%02X ", uart5Data.buff[i]);
	}
	printf("\r\n");
	
	if(uart5Data.cnt<8) {
		printf("数据不完整\r\n");
		goto end;
	}
	
	if(uart5Data.buff[0] != 0x5f) {
		printf("帧头错误\r\n");
		goto end;
	}
	
	uint8_t check = 0;
	for(i=0; i<7; i++){//计算校验值
		check += uart5Data.buff[i];
	}
	if(uart5Data.buff[7] != check) {
		printf("校验失败\r\n");
		goto end;
	}
	if(uart5Data.buff[1] == 0xFF && uart5Data.buff[2] == 0xFF)
	{
		kqm6600.state = KQM_PreHeat;
	}
	else {
		kqm6600.state = KQM_Work;
		kqm6600.voc = ((uart5Data.buff[1]<<8)+(uart5Data.buff[2]))*0.1;
		kqm6600.ch2o = ((uart5Data.buff[3]<<8)+(uart5Data.buff[4]))*0.01;
		kqm6600.co2 = ((uart5Data.buff[5]<<8)+(uart5Data.buff[6]));
		
		printf("  %.2f   %.2f  %.2f\r\n", kqm6600.voc, kqm6600.ch2o, kqm6600.co2);
	}
	
	end:
	memset(uart5Data.buff, 0, sizeof(uart5Data.buff));
	uart5Data.cnt = 0;
	uart5Data.overFlag = 0;
}




void Uart5_Config(void)
{
	//配置IO  PD2 RX
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//配置UART5
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx;//只接收
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(UART5, &USART_InitStructure);
	//配置中断
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);
	NVIC_SetPriority(UART5_IRQn, 4);
	NVIC_EnableIRQ(UART5_IRQn);
	//启动串口
	USART_Cmd(UART5,ENABLE);
}

