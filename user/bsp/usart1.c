#include "usart1.h"
#include "stdio.h"
#include "string.h"
#include "led.h"
#include "time.h"
#include "rtc.h"
#include "spi.h"
uint8_t usart1RxData = 0;
uint16_t temp[2]={0};
USART1_DATA_Def usart1Data = {0};
#define FONT_SAVE_ADDR 0
uint32_t addrIndex = 0;
uint32_t tick = 0;
void FontTick(void)
{
	if(tick) tick++;
}
uint16_t buff_w25[100]={0};

void FontTickOver(void)
{
	if(tick>100){
		printf("---���---%d-�ֽ�\r\n",addrIndex);
	addrIndex = 0;
	tick = 0;
		W25Q_PageReadhald(0,buff_w25,100);
	for(int i=0;i<100;i++){
		printf("%0x ",buff_w25[i]);
	}
	
	}
}
uint16_t cnt = 0;
uint16_t temp_falg=0;
void USART1_IRQHandler(void)
{
	if((USART1->SR & (1<<5)) != 0)//����һ���ֽ�����
	{
		usart1RxData = USART1->DR;//��ȡ���ݣ��Զ����ж�
			W25Q_Pagewrite(addrIndex+FONT_SAVE_ADDR,&usart1RxData,1);
//	temp[cnt%2] = usart1RxData;
//	if((cnt%2) == 1) {
//		temp_falg =(temp[0]<<8) + (temp[1]);
//		W25Q_Pagewritehald(addrIndex+FONT_SAVE_ADDR,&temp_falg,1);

//	}
//	cnt++;
		addrIndex++;
		tick =1;
	}

}




//����1������
//���͡� ���ա������ʴ��ν���
//USART1  TX PA9		RX PA10
void Usart1_Config(uint32_t brr)
{

	/*1 ����ͨ�����ŵ�ģʽ PA9 PA10*/
	//��GPIOA��ʱ��
	RCC->APB2ENR |= (1<<2);
	//PA9 ����������������
	GPIOA->CRH &= ~(0xF<<4);//��0
	GPIOA->CRH |= (0xB<<4);//��1
	//PA10 ��������
	GPIOA->CRH &= ~(0xF<<8);//��0
	GPIOA->CRH |= (0x4<<8);//��1
	
	/*2 ���ô��ڵĹ���ģʽ��
			��ʼ��
			��ʼλ������λ��
			��żУ��λ��ֹͣλ ������ �������ͽ�������
			������жϣ������ж�*/
	RCC->APB2ENR |= (1<<14);
	
	float div = 72000000/16/brr+0.5;
	uint32_t Mantissa = div;
	uint32_t Fraction = (div-Mantissa)*16;
	USART1->BRR = (Mantissa<<4)|Fraction;
	
	USART1->CR1 = 0;
	USART1->CR1 &= ~(1<<12);//8bit���ݳ���
	USART1->CR1 &= ~(1<<10);//�ر�У��
	USART1->CR1 |= (1<<3);//�򿪷�����
	USART1->CR1 |= (1<<2);//�򿪽�����
	
	USART1->CR2 = 0;
	USART1->CR2 &= ~(0x3<<12);//һ��ֹͣλ
	
	/*3 ������жϣ�NVICҪ����*/
	USART1->CR1 |= (1<<5);//��RXNE�жϣ��յ�һ�����ݳ��ȣ��ͻ���һ���ж�
	//USART1->CR1 |= (1<<4);//�򿪿����ж�
	NVIC_SetPriority(USART1_IRQn, 3);
	NVIC_EnableIRQ(USART1_IRQn);
	
	/*4 �������ڣ�ʹ�ܴ���*/
	USART1->CR1 |= (1<<13);
}


void Usart1_SendByte(uint8_t data)
{
	//�����һ�ε������Ƿ������
	while((USART1->SR & (1<<6)) == 0)
	{}
	USART1->DR = data;//��������
}

//��������
uint8_t Usart1_RecvByte(void)
{
	//����Ƿ���յ�����
	while((USART1->SR & (1<<5)) == 0)
	{}
	return USART1->DR;//��������
}


void Usart1_SendStr(char str[])
{
	uint32_t i = 0;
	while(str[i] != '\0') {
		while((USART1->SR & (1<<6)) == 0)
		{}
		USART1->DR = str[i];//��������
		i++;
	}
}

void Usart1_SendBuff(uint8_t buff[], uint16_t len)
{
	uint32_t i = 0;
	for(i=0; i<len; i++)
	{
		Usart1_SendByte(buff[i]);
	}
}



/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{
	Usart1_SendByte(ch);
  return ch;
}


