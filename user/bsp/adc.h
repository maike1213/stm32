#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"
typedef struct{
	float adc;
	float v;
}ADC_VALUE_Def;
extern ADC_VALUE_Def adclight;
extern ADC_VALUE_Def adcMq2;

void ADC_Config(void);
uint16_t Get(void);
void Get_Adc();
#endif
