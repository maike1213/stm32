#ifndef __GT911_H
#define __GT911_H

#include "stm32f10x.h"
#include "ctiic.h"


#define GT911_READ_XY_REG 					0x814E	/* 坐标寄存器 */ 
#define GT911_CLEARBUF_REG 					0x814E	/* 清除坐标寄存器 */ 
#define GT911_CONFIG_REG 						0x8047	/* 配置参数寄存器 */ 
#define GT911_COMMAND_REG 					0x8040 /* 实时命令 */ 
#define GT911_PRODUCT_ID_REG 				0x8140 /*productid*/ 
#define GT911_VENDOR_ID_REG 				0x814A /* 当前模组选项信息 */ 
#define GT911_CONFIG_VERSION_REG 		0x8047 /* 配置文件版本号 */ 
#define GT911_CONFIG_CHECKSUM_REG 	0x80FF /* 配置文件校验码 */ 
#define GT911_FIRMWARE_VERSION_REG 	0x8144 /* 固件版本号 */ 

/* 定义GT911复位与中断引脚连接的GPIO端口 */ 
#define GPIO_PORT_GT911_RST	GPIOC	/* GPIO端口 */ 
#define GPIO_PORT_GT911_INT	GPIOF	/* GPIO端口 */ 
#define RCC_GT911_RST_INT_PORT RCC_AHB1Periph_GPIOF	/* GPIO端口时钟 */ 
#define GT911_RST_PIN	GPIO_Pin_5	/* 连接到RST时钟线的GPIO */ 
#define GT911_INT_PIN	GPIO_Pin_11	/* 连接到INT数据线的GPIO */ 
#define EXTI_GT911_INT_PORT EXTI_PortSourceGPIOF /*GT911 INT EXTI PORT*/ 
#define EXTI_GT911_INT_PIN EXTI_PinSource11 /*GT911 INT EXTI PIN*/ 
#define EXTI_GT911_INT_LINE EXTI_Line11 /*GT911 INT EXTI LINE*/ 

/* 定义写RST和INT的宏 */ 
#define GT911_RST_1() {PFout(11) = 1;}	/* RST = 1 */ 
#define GT911_RST_0() {PFout(11) = 0;}	/* RST = 0 */ 

#define GT911_INT_1() {PFout(10) = 1;}	/* INT = 1 */ 
#define GT911_INT_0() {PFout(10) = 0;}	/* INT = 0 */ 


/* 
GT911 的从设备地址有两组可选，两组地址分别为：0xBA/0xBB和0x28/0x29 
*/ 
#define GT911_I2C_ADDR	0xBA 

typedef struct 
{ 
    uint8_t Enable; 
    uint8_t i2c_addr; 

    uint8_t TouchpointFlag; 

    uint8_t Touchkey1trackid; 
    uint16_t X1; 
    uint16_t Y1; 
    uint16_t S1; 

    uint8_t Touchkey2trackid; 
    uint16_t X2; 
    uint16_t Y2; 
    uint16_t S2; 

    uint8_t Touchkey3trackid; 
    uint16_t X3; 
    uint16_t Y3; 
    uint16_t S3; 

    uint8_t Touchkey4trackid; 
    uint16_t X4; 
    uint16_t Y4; 
    uint16_t S4; 

    uint8_t Touchkey5trackid; 
    uint16_t X5; 
    uint16_t Y5; 
    uint16_t S5; 
}GT911_T; 


static void GT911_WriteReg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen); 
static void GT911_ReadReg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen); 

u8 GT911_Scan(u8 mode);
uint16_t GT911_ReadFirmwareVersion(void);
void GT911_InitHard(void);
uint8_t GT911_ReadStatue(void);
void GT911_Soft_Reset(void);
void GT911_Reset_Sequence(uint8_t ucAddr);




#endif
