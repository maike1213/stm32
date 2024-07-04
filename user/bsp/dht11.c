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
			return -1;//无应答
		}
	}
	cnt = 0;
	while(DHT11_In_State()==Bit_RESET)
	{
		delay_1us();
		cnt++;
		if(cnt > 100)
		{
			return -2;//应答超时
		}
	}
for(i=0; i<5; i++)
	{
		for(j=0; j<8; j++) 
		{
			//等待响应  等待高电平结束，低电平到来
			cnt = 0;
			while(DHT11_In_State() == Bit_SET) {
				delay_1us();
				cnt++;
				if(cnt > 100) {
					return -3;//无应答
				}
			}
			//等待响应完成  等待低电平结束，高电平到来
			cnt = 0;
			while(DHT11_In_State() == Bit_RESET) {
				delay_1us();
				cnt++;
				if(cnt > 100) {
					return -4;//应答超时
				}
			}	
			Delay_us(30);
			if(DHT11_In_State() == Bit_SET) {//1
				buff[i] |= (1<<(7-j));
			}// buff初始化为0了，所以收到0时，对应位已经是0 	
		}		
	}
	if((buff[0]+buff[1]+buff[2]+buff[3]) == buff[4]) {//校验成功
		dht11.hum = buff[0];
		dht11.tem = buff[2] + (buff[3]&0x7F)*0.1;
		if((buff[3]&(1<<7)) != 0) {
			dht11.tem = -dht11.tem;
		}
	}
	else {
		return -5;//应答超时
	}
	return 0;
	
	
}

