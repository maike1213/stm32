#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x.h"
#define sFLASH_SPI                       SPI2
#define sFLASH_SPI_CLK                   RCC_APB1Periph_SPI2
#define sFLASH_SPI_SCK_PIN               GPIO_Pin_13                  /* PB.13 */
#define sFLASH_SPI_SCK_GPIO_PORT         GPIOB                     
#define sFLASH_SPI_MISO_PIN              GPIO_Pin_14                  /* PB.14 */
#define sFLASH_SPI_MISO_GPIO_PORT        GPIOB                      
#define sFLASH_SPI_MOSI_PIN              GPIO_Pin_15                  /* PB.15 */
#define sFLASH_SPI_MOSI_GPIO_PORT        GPIOB                      
#define sFLASH_CS_PIN                    GPIO_Pin_12                  /* PB.12 */
#define sFLASH_CS_GPIO_PORT              GPIOB                      
#define sFLASH_CS_LOW()       GPIO_ResetBits(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN)
#define sFLASH_CS_HIGH()      GPIO_SetBits(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN)   

#define SD_CS PBout(12)
#define SD_SCK PBout(12)
#define SD_MOSI PBout(15)
#define SD_MISO PBin(14)

void SPI_Config(void);
uint8_t sFLASH_SendByte(uint8_t byte);
void W25Q_WriteEnable();
void W25Q_WriteDisable();
uint8_t W25Q_IsBusy();
void W25Q_S(uint32_t addr);
void W25Q_Del();
void W25Q_Pagewrite(uint32_t addr,uint8_t *buff,uint16_t len);
void W25Q_writebuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void W25Q_PageRead(uint32_t addr,uint8_t *buff,uint32_t len);
void W25Q_Pagewritehald(uint32_t addr,uint16_t *buff,uint32_t len);
void W25Q_PageReadhald(uint32_t addr,uint16_t *buff,uint32_t len);
uint32_t sFLASH_ReadID(void);
#endif