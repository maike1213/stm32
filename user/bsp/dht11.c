#include "dht11.h"
#include "delay.h"
DHT11_Data dht11;
//PG11 DHT11
void DHT11_Config(void)
{
	RCC_APB2PeriphClockCmd(DHT11_Clock,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = DHT11_Pin;
	GPIO_Init(DHT11_Port, &GPIO_InitStructure);

}
int DHT11_ReadData()
{

	uint8_t cnt = 0;
	uint8_t i=0, j=0;
	uint8_t buff[8]={0};
	DHT11_Out_L();
	Delay_ms(20);
	DHT11_Out_H();
	while(DHT11_In_State()==Bit_SET)
	{
		delay_1us();
		cnt++;
		if(cnt > 100)
		{
			return -1;//��Ӧ��
		}
	}
	cnt = 0;
	while(DHT11_In_State()==Bit_RESET)
	{
		delay_1us();
		cnt++;
		if(cnt > 100)
		{
			return -2;//Ӧ��ʱ
		}
	}
for(i=0; i<5; i++)
	{
		for(j=0; j<8; j++) 
		{
			//�ȴ���Ӧ  �ȴ��ߵ�ƽ�������͵�ƽ����
			cnt = 0;
			while(DHT11_In_State() == Bit_SET) {
				delay_1us();
				cnt++;
				if(cnt > 100) {
					return -3;//��Ӧ��
				}
			}
			//�ȴ���Ӧ���  �ȴ��͵�ƽ�������ߵ�ƽ����
			cnt = 0;
			while(DHT11_In_State() == Bit_RESET) {
				delay_1us();
				cnt++;
				if(cnt > 100) {
					return -4;//Ӧ��ʱ
				}
			}	
			Delay_us(30);
			if(DHT11_In_State() == Bit_SET) {//1
				buff[i] |= (1<<(7-j));
			}// buff��ʼ��Ϊ0�ˣ������յ�0ʱ����Ӧλ�Ѿ���0 	
		}		
	}
	if((buff[0]+buff[1]+buff[2]+buff[3]) == buff[4]) {//У��ɹ�
		dht11.hum = buff[0];
		dht11.tem = buff[2] + (buff[3]&0x7F)*0.1;
		if((buff[3]&(1<<7)) != 0) {
			dht11.tem = -dht11.tem;
		}
	}
	else {
		return -5;//Ӧ��ʱ
	}
	return 0;
	
	
}

