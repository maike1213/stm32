#ifndef __SU03_H
#define __SU03_H
#include "stm32f10x.h"
extern uint16_t adc_value;
extern uint16_t SomkeDensity;
#define UART4_RX_MAX	16

typedef struct{
	uint8_t buff[UART4_RX_MAX];
	uint8_t cnt;
	uint8_t overFlag;
}UART4_DATA_Def;


void Su03T_Init(void);
void Uart4_DataAnalysis(void);
void Uart4_SendBuff(uint8_t buff[], uint16_t len);
void Su03T_Play(uint8_t msgNum, double value);
void time_play(void);
#endif

