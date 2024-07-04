#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
#include "time.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "beep.h"
#include "sys.h"
#include "exti.h"
#include "usart1.h"
#include "usart5.h"
#include "su03.h"
#include "dht11.h"
#include "pwm.h"
#include "adc.h"
#include "rtc.h"

#include "spi.h"
//#include "sd.h"
typedef struct{
	char wifiname[32];
	char passwd[32];
}WIFI_def;
char wifibuf[64]={0};
extern void FontTickOver(void);

int main(void)
{
	WIFI_def wifi={0};
	//取消保护、时钟
	strcpy(wifi.wifiname,"kxw_WIFI");
	strcpy(wifi.passwd,"12345678_kxw");
	sprintf(wifibuf,"%s %s",wifi.wifiname,wifi.passwd);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SysTick_Init();
	NVIC_SetPriority(TIM2_IRQn, 10);
	NVIC_EnableIRQ(TIM2_IRQn);
//	TIM6_Init(7200,10000);
	Usart1_Config(115200);
	Led_Config();
//	Key_Config();
//	Beep_Config();
//	EXTI0_Init();
//	Su03T_Init();
//	DHT11_Config();
//	Uart5_Config();
//	PWM_Config();
//	ADC_Config();
	RTC_Config();
	printf("hello word\r\n");
	uint32_t s= mktime(&settime);
	printf("%u\r\n",s);
	RTC_SetTime(s);
//	lcd_init();
	SPI_Config();
	//SD_Config();
	//W25Q_Del();
	printf("擦除完成\r\n");
	//W25Q_S(0);
	
	//W25Q_Pagewrite(0,(uint8_t* )wifibuf,sizeof(wifibuf));
	uint16_t buff[2]={0x1234,0x5678};
	//W25Q_Pagewritehald(2047*4096,buff,2);
	//WIFI_def buff={0};
	char buff2[8]={0};
	//W25Q_PageRead(0,(uint8_t *)buff2,7);
//	W25Q_writebuffer((uint8_t* )&wifi,(2047*4096),sizeof(wifi));
//	W25Q_PageRead(2047*4096,(uint8_t *)&buff,sizeof(wifi));
//	printf("%s %s\r\n",buff.wifiname,buff.passwd);
	printf("%X\r\n",sFLASH_ReadID());
	// W25Q_PageReadhald(2047*4096,buff_w25,100);
	uint8_t buff_w2[100]={0};
	W25Q_PageRead(2047*4096,buff_w2,100);
	for(int i=0;i<100;i++){
		printf("%0x ",buff_w2[i]);
	}
  while (1)
  {	
		
	FontTickOver();
	Led_Run(1);
		Delay_ms(200);
	 
	}
}
