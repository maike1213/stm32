#include "key.h"
#include "delay.h"


void Key_Config(void)
{
	//��GPIOA��ʱ��
	RCC->APB2ENR |= (1<<2);
	//PA0 KEY1 ��������ģʽ
	GPIOA->CRL &= ~(0xF<<0);//��0
	GPIOA->CRL |= (0x4<<0);//��1   0100
	
	//KEY234  PC456  ����Ϊ��   �ɿ�Ϊ��
	//��������ģʽ
	RCC->APB2ENR |= (1<<4);
	GPIOC->CRL &= ~(0xFFF<<16);	//��0
	GPIOC->CRL |= (0x444<<16);	//��1   0100
	
}


//����1�ļ�⺯�����Դ�����
//��⵽���·���1  ���򷵻�0
uint8_t Key1_Check(void)
{
	uint8_t ret = 0;
	static uint32_t cnt = 0;
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_SET)
	{
		cnt++;
	}
	else 
	{
		if(cnt > 100) {
			ret = 2;
		}
		else if(cnt > 1) {
			ret = 1;
		}
		cnt = 0;
	}
	return ret;
//	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_SET)
//	{
//		Delay_ms(10);
//		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_SET)
//		{
//			while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_SET)
//			{}
//			ret = 1;
//		}
//	}

}


uint8_t Key2_Check(void)
{
	uint8_t ret = 0;
	if((GPIOC->IDR&(1<<4)) == 0)
	{
		Delay_ms(10);
		if((GPIOC->IDR&(1<<4)) == 0)
		{
			while((GPIOC->IDR&(1<<4)) == 0)
			{}
			ret = 1;
		}
	}
	return ret;
}


uint8_t Key3_Check(void)
{
	uint8_t ret = 0;
	if((GPIOC->IDR&(1<<5)) == 0)
	{
		Delay_ms(10);
		if((GPIOC->IDR&(1<<5)) == 0)
		{
			while((GPIOC->IDR&(1<<5)) == 0)
			{}
			ret = 1;
		}
	}
	return ret;
}


uint8_t Key4_Check(void)
{
	uint8_t ret = 0;
	if((GPIOC->IDR&(1<<6)) == 0)
	{//���ܰ���
		Delay_ms(10);
		if((GPIOC->IDR&(1<<6)) == 0)
		{//һ������
			while((GPIOC->IDR&(1<<6)) == 0)
			{}
			//һ���ɿ�
			ret = 1;
		}
	}
	return ret;
}


