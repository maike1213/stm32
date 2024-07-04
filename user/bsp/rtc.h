#ifndef __RTC_H
#define __RTC_H

#include "stm32f10x.h"
#include "time.h"
extern uint32_t Alarm_Value;
extern uint8_t alarm_flag;
extern	struct tm *nowtime;
extern struct tm settime;
extern struct tm Alarm_Settime;
struct tm * RTC_GetTime(void);
void Alarm_Set();
void RTC_SetTime(uint32_t s);
void RTC_Configuration(void);
void NVIC_Configuration(void);
void RTC_Config(void);
#endif

