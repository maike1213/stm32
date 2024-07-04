#include "rtc.h"
#include "stdio.h"
#include "string.h"
#include "led.h"
uint8_t alarm_flag=0;
uint32_t Alarm_Value=1717151235;
struct tm *nowtime=NULL;
struct tm settime =     {10,27,10,31,5-1,2024-1900};
struct tm Alarm_Settime={10,27,10,31,5-1,2024-1900};
struct tm * RTC_GetTime(void)
{
	
	uint32_t s = RTC_GetCounter();
	return localtime(&s);
}

void RTC_SetTime(uint32_t s)
{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
		/* Allow access to BKP Domain */
		PWR_BackupAccessCmd(ENABLE);
		RTC_WaitForLastTask();
		RTC_SetCounter(s);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
}
void Alarm_Set()
{
	uint32_t s1= mktime(&Alarm_Settime);
		RTC_EnterConfigMode();
		RTC_SetAlarm(s1);
		RTC_ExitConfigMode();
		RTC_WaitForLastTask();

}
void RTC_IRQHandler(void)
{
  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
  {
    /* Clear the RTC Second interrupt */
    RTC_ClearITPendingBit(RTC_IT_SEC);
		//Led_Toggle(0);
		
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    
  }
	 if (RTC_GetITStatus(RTC_FLAG_ALR) != RESET)
  {
    /* Clear the RTC Second interrupt */
    RTC_ClearITPendingBit(RTC_FLAG_ALR);
		alarm_flag = 1;
		Su03T_Play(11,0);
		printf("--------------------\r\n");

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    
  }
}

void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}
  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
	
  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);
	
	RTC_ITConfig(RTC_IT_ALR,ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
	
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
	
}
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;


  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
void RTC_Config(void)
{
		
	  if (BKP_ReadBackupRegister(BKP_DR1) != 0x888)
  {
	
    printf("\r\n µÚÒ»´Î....\r\n");
   RTC_Configuration();
    printf("\r\n RTC configured....");
		RTC_SetCounter(Alarm_Value);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
		
    BKP_WriteBackupRegister(BKP_DR1, 0x888);

  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
      printf("\r\n\n Power On Reset occurred....");
    }
    /* Check if the Pin Reset flag is set */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      printf("\r\n\n External Reset occurred....");
    }

    printf("\r\n No need to configure RTC....");
    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();
		
    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC|RTC_IT_ALR, ENABLE);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
		
  }
	NVIC_Configuration();
	
	
}

