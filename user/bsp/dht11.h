#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f10x.h"
#define  DHT11_Port  GPIOG
#define  DHT11_Pin  GPIO_Pin_11
#define  DHT11_Clock  RCC_APB2Periph_GPIOG
#define DHT11_Out_H() (GPIO_SetBits(DHT11_Port,DHT11_Pin))
#define DHT11_Out_L() (GPIO_ResetBits(DHT11_Port,DHT11_Pin))
#define DHT11_In_State()	(GPIO_ReadInputDataBit(DHT11_Port,DHT11_Pin))
typedef struct{
	float hum;
	float tem;
}DHT11_Data;
extern DHT11_Data dht11;
void DHT11_Config(void);
int DHT11_ReadData();
#endif
