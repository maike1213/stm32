#include "sd.h"
#include "spi.h"
#include "delay.h"
/*SPI 
			cs   PC11
			SCK	 PC12
			MISO PC8
			MOSI PD2
*/

u16 spi_speed = 100; //the spi speed(0-255),0 is fastest
//�����д�ٶȻؼ��Ӱ�쵽оƬ�ܲ��Ҷ����ļ������ֵ����ᵼ�²�ѯʱ������������Ź���λ�������Ϊͨ������ȷ����ֵ�Ĵ�С
u8 sd_state = 0;
//delay 1us?��actually not??it maybe is several us??I don't test it??
void usleep(u16 i)
{
    while (i > 0)
    {
        i--;
    };
}
void SD_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);
  GPIO_InitStructure.GPIO_Pin = sSD_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(sSD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
  /*!< Configure sSD_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = sSD_SPI_MOSI_PIN;
  GPIO_Init(sSD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);
  /*!< Configure sSD_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = sSD_SPI_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_Init(sSD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
  
  /*!< Configure sSD_CS_PIN pin: sSD Card CS pin */
  GPIO_InitStructure.GPIO_Pin = sSD_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(sSD_CS_GPIO_PORT, &GPIO_InitStructure);
	
	sSD_CS_HIGH();


  /*!< Enable the sSD_SPI  */
  SPI_Cmd(SPI2, ENABLE);
}

//write a byte
void SDWriteByte(u8 data)
{
    u8 i;
		SD_SCK = 1;
    //write 8 bits(MSB)
    for (i = 0; i < 8; i++)
    {
        SD_SCK = 0;
        usleep(spi_speed);
        if (data & 0x80)
				{   
					SD_MISO =1;
				}
        else
				{
					SD_MISO =1;
				}
        data <<= 1;
        SD_SCK  =1;;
         usleep(spi_speed);
    }
 
    //when DI is free,it should be set high
    SD_MISO =1;
}
 
#define  SPI_MI  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)
//read a byte
u8 SDReadByte(void)
{
    u8 data = 0x00, i;
		SD_SCK = 1;
    //read 8 bit(MSB)
    for (i = 0; i < 8; i++)
    {
        SD_SCK =0;
         usleep(spi_speed);
        SD_SCK =1;
        data <<= 1;
        if (SPI_MI)
            data |= 0x01;
        usleep(spi_speed);
    }
 
    return data;
}

//ȡ��ѡ��,�ͷ�SPI����
void SD_DisSelect(void)
{
    SD_CS =1;
    SDWriteByte(0xff);//�ṩ�����8��ʱ��
}
 
 
//�ȴ���׼����
//����ֵ:0,׼������;����,�������
u8 SD_WaitReady(void)
{
    u32 t=0;
    do
    {
        if(SDReadByte()==0XFF)return 0;//OK
        t++;            
    }while(t<0XFFFFFF);//�ȴ� 
    return 1;
}
 
//ѡ��sd��,���ҵȴ���׼��OK
//����ֵ:0,�ɹ�;1,ʧ��;
u8 SD_Select(void)
{
    SD_CS =0;
    if(SD_WaitReady()==0)return 0;//�ȴ��ɹ�
    SD_DisSelect();
    return 1;//�ȴ�ʧ��
}
//

//send a command and send back the response
u8 SDSendCmd(u8 cmd, u32 arg, u8 crc)
{
    u8 r1;  
    u8 Retry=0; 
    SD_DisSelect();//ȡ���ϴ�Ƭѡ
    if(SD_Select())return 0XFF;//ƬѡʧЧ 
    //����
    SDWriteByte(cmd | 0x40);//�ֱ�д������
    SDWriteByte(arg >> 24);
    SDWriteByte(arg >> 16);
    SDWriteByte(arg >> 8);
    SDWriteByte(arg);     
    SDWriteByte(crc); 
    if(cmd==12)SDWriteByte(0xff);//Skip a stuff byte when stop reading
    //�ȴ���Ӧ����ʱ�˳�
    Retry=0X1F;
    do
    {
        r1=SDReadByte();
    }while((r1&0X80) && Retry--);    
    //����״ֵ̬
    return r1;
}
