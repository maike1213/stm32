#ifndef __USART1_H
#define __USART1_H

#include "stm32f10x.h"

#define USART1_RX_MAX	256

typedef struct{
	uint8_t buff[USART1_RX_MAX];
	uint16_t cnt;
	uint8_t overFlag;
}USART1_DATA_Def;



void Usart1_Config(uint32_t brr);
void Usart1_SendByte(uint8_t data);
void Usart1_SendStr(char str[]);
uint8_t Usart1_RecvByte(void);

void Usart1_DataAnalysis(void);
#endif
