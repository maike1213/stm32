#ifndef __SD_H
#define __SD_H
#include "stm32f10x.h"
#include "sys.h"
/*SPI 
			cs   PC11
			SCK	 PC12
			MISO PC8
			MOSI PD2
*/


#define sSD_SPI_SCK_PIN               GPIO_Pin_12                  /* PA.05 */
#define sSD_SPI_SCK_GPIO_PORT         GPIOC                       /* GPIOA */

#define sSD_SPI_MISO_PIN              GPIO_Pin_8                  /* PA.06 */
#define sSD_SPI_MISO_GPIO_PORT        GPIOC                       /* GPIOA */

#define sSD_SPI_MOSI_PIN              GPIO_Pin_2                  /* PA.07 */
#define sSD_SPI_MOSI_GPIO_PORT        GPIOD                       /* GPIOA */

#define sSD_CS_PIN                    GPIO_Pin_11                  /* PE.06 */
#define sSD_CS_GPIO_PORT              GPIOC                       /* GPIOE */


//#define SD_CS PCout(11)
//#define SD_SCK PCout(12)
//#define SD_MOSI PDout(2)
//#define SD_MISO PCin(8)

#define sSD_CS_LOW()       GPIO_ResetBits(sSD_CS_GPIO_PORT, sSD_CS_PIN)
/**
  * @brief  Deselect sSD: Chip Select pin high
  */
#define sSD_CS_HIGH()      GPIO_SetBits(sSD_CS_GPIO_PORT, sSD_CS_PIN)   
/**
  * @}
  */ 
void SD_Config(void);
void SDWriteByte(u8 data);
u8 SDReadByte(void);


// SD��ָ���  	   
#define SDCard_CMD0    0       //����λ
#define SDCard_CMD8    8       //����8 ��SEND_IF_COND
#define SDCard_CMD9    9       //����9 ����CSD����
#define SDCard_CMD12   12      //����12��ֹͣ���ݴ���
//#define SDCard_CMD13   16      //����16������������С Ӧ����0x00
#define SDCard_CMD17   17      //����17��������
#define SDCard_CMD18   18      //����18�����������
#define SDCard_CMD23   23      //����23�����ö�����д��ǰԤ�Ȳ���N��block
#define SDCard_CMD24   24      //����24��д����
#define SDCard_CMD25   25      //����25��д�������
#define SDCard_CMD41   41      //����41��Ӧ����0x00
#define SDCard_CMD55   55      //����55��Ӧ����0x01
#define SDCard_CMD58   58      //����58����OCR��Ϣ



#endif

