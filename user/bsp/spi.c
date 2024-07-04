#include "spi.h"
//#include "sd.h"
/*SPI 
			cs   PB12
			SCK	 PB13
			MISO PB14
			MOSI PB15
*/
void SPI_Config(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(sFLASH_SPI_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(sFLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
  /*!< Configure sFLASH_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_MOSI_PIN;
  GPIO_Init(sFLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);
  /*!< Configure sFLASH_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_Init(sFLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
  
  /*!< Configure sFLASH_CS_PIN pin: sFLASH Card CS pin */
  GPIO_InitStructure.GPIO_Pin = sFLASH_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(sFLASH_CS_GPIO_PORT, &GPIO_InitStructure);
	
	sFLASH_CS_HIGH();
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);

  /*!< Enable the sFLASH_SPI  */
  SPI_Cmd(SPI2, ENABLE);
}



uint8_t sFLASH_SendByte(uint8_t byte)
{
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
  /*!< Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI2, byte);
  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI2);
}

void W25Q_WriteEnable()
{
	sFLASH_CS_LOW();
	sFLASH_SendByte(0x06);
	sFLASH_CS_HIGH();
}
void W25Q_WriteDisable()
{
	sFLASH_CS_LOW();
	sFLASH_SendByte(0x04);
	sFLASH_CS_HIGH();
}
uint8_t W25Q_IsBusy()
{
	uint8_t s = 0;
	sFLASH_CS_LOW();
	sFLASH_SendByte(0x05);
	do{
		 s = sFLASH_SendByte(0x11);
	}while((s &(1<<0)) !=0);
	sFLASH_CS_HIGH();
}
void W25Q_S(uint32_t addr)
{	
	W25Q_WriteEnable();
	W25Q_IsBusy();
	sFLASH_CS_LOW();
  sFLASH_SendByte(0x20);
	sFLASH_SendByte(addr>>16);
	sFLASH_SendByte(addr>>8);
	sFLASH_SendByte(addr);
  sFLASH_CS_HIGH();
	W25Q_IsBusy();
}
void W25Q_Del()
{	
	W25Q_WriteEnable();
	W25Q_IsBusy();
	sFLASH_CS_LOW();
  sFLASH_SendByte(0xC7);

  sFLASH_CS_HIGH();
	W25Q_IsBusy();
}
void W25Q_Pagewrite(uint32_t addr,uint8_t *buff,uint16_t len)
{
	W25Q_WriteEnable();
	W25Q_IsBusy();
	sFLASH_CS_LOW();
	sFLASH_SendByte(0x02);
	sFLASH_SendByte(addr>>16);
	sFLASH_SendByte(addr>>8);
	sFLASH_SendByte(addr);
	for(int i=0;i<len;i++){
		sFLASH_SendByte(buff[i]);
	}
	sFLASH_CS_HIGH();
	W25Q_IsBusy();
}
uint16_t sFLASH_SendByte_hald(uint16_t byte)
{
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
  /*!< Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI2, byte);
  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI2);
}
void W25Q_Pagewritehald(uint32_t addr,uint16_t *buff,uint32_t len)
{
	uint8_t data =0;
	W25Q_WriteEnable();
	W25Q_IsBusy();
	sFLASH_CS_LOW();
	sFLASH_SendByte(0x02);
	sFLASH_SendByte(addr>>16);
	sFLASH_SendByte(addr>>8);
	sFLASH_SendByte(addr);
	for(int i=0;i<len;i++){
		data =buff[i];
		sFLASH_SendByte(data<<8);
		sFLASH_SendByte(buff[i]);
	}
	sFLASH_CS_HIGH();
	W25Q_IsBusy();
}
void W25Q_PageReadhald(uint32_t addr,uint16_t *buff,uint32_t len)
{
	uint16_t temp=0;
	uint8_t data=0;
	W25Q_WriteEnable();
	W25Q_IsBusy();
	sFLASH_CS_LOW();
  sFLASH_SendByte(0x03);
	sFLASH_SendByte(addr>>16);
	sFLASH_SendByte(addr>>8);
	sFLASH_SendByte(addr);
	for(uint32_t i=0;i<len;i++){
		
	 buff[i]=((sFLASH_SendByte(0x11)<<8)+sFLASH_SendByte(0x11));

	}
  sFLASH_CS_HIGH();
}
void W25Q_writebuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
	Addr =WriteAddr%256;
	count =256-Addr;
	NumOfPage = NumByteToWrite / 256;
	NumOfSingle = NumByteToWrite % 256;
	if(Addr ==0)
	{
		if(NumOfPage == 0)
		{
			W25Q_Pagewrite(WriteAddr,pBuffer,NumByteToWrite);
		}
		else
		{
			while(NumOfPage--)
			{
				W25Q_Pagewrite(WriteAddr,pBuffer,256);
				WriteAddr +=256;
				pBuffer +=256;
			}
			W25Q_Pagewrite(WriteAddr,pBuffer,NumOfSingle);
		}
	}
	else
	{
	
	if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > sFLASH_PAGESIZE */
      {
        temp = NumOfSingle - count;
				W25Q_Pagewrite(WriteAddr,pBuffer,count);
        
        WriteAddr +=  count;
        pBuffer += count;
				W25Q_Pagewrite(WriteAddr,pBuffer,temp);
       
      }
      else
      {
				W25Q_Pagewrite(WriteAddr,pBuffer,NumByteToWrite);
  
      }
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / 256;
      NumOfSingle = NumByteToWrite % 256;

      W25Q_Pagewrite(WriteAddr,pBuffer,  count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        W25Q_Pagewrite(WriteAddr, pBuffer, 256);
        WriteAddr +=  256;
        pBuffer += 256;
      }

      if (NumOfSingle != 0)
      {
        W25Q_Pagewrite(WriteAddr,pBuffer, NumOfSingle);
      }
    }
  }
}
void W25Q_PageRead(uint32_t addr,uint8_t *buff,uint32_t len)
{
	W25Q_WriteEnable();
	W25Q_IsBusy();
	sFLASH_CS_LOW();
  sFLASH_SendByte(0x03);
	sFLASH_SendByte(addr>>16);
	sFLASH_SendByte(addr>>8);
	sFLASH_SendByte(addr);
	for(uint32_t i=0;i<len;i++){
	buff[i] =sFLASH_SendByte(0x11);
	}
  sFLASH_CS_HIGH();
}
uint32_t sFLASH_ReadID(void)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();
  /*!< Send "RDID " instruction */
  sFLASH_SendByte(0x9F);
	//SDWriteByte(0x9F);
  /*!< Read a byte from the FLASH */
  Temp0 = sFLASH_SendByte(0x11);
  /*!< Read a byte from the FLASH */
  Temp1 = sFLASH_SendByte(0x11);
  /*!< Read a byte from the FLASH */
  Temp2 = sFLASH_SendByte(0x11);
  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();
  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
  return Temp;
}
