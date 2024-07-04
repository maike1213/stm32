#ifndef __USART5_H
#define __USART5_H

#include "stm32f10x.h"

#define UART5_RX_MAX	16

typedef struct{
	uint8_t buff[UART5_RX_MAX];
	uint8_t cnt;
	uint8_t overFlag;
}UART5_DATA_Def;

typedef enum {
	KQM_PreHeat,
	KQM_Work,
}KQM6600_STATE;

typedef struct{
	float voc;
	float ch2o;
	float co2;
	uint8_t state;
}KQM6600_DATA_Def;
extern KQM6600_DATA_Def kqm6600;



void Uart5_Config(void);
void Kqm6600_Init(void);
void Uart5_DataAnalysis(void);
#endif
