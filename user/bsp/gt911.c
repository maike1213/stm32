#include "gt911.h"
#include "delay.h"

#include "touch.h"

#define delay_us	Delay_us
#define delay_ms	Delay_ms

/* GT911单个触点配置参数，一次性写入 */ 

uint8_t s_GT911_CfgParams[]= 
{ 
	0x00,
	0x20,0x03,
	0xE0,0x01,
	0x0A,
	0x3D,
	0x00,
	0x01,
	0x08,
	0x28,
	0x05,
	0x5A,
	0x46,
	0x03,
	0x05,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x10,
	0x11,
	0x14,
	0x0F,
	0x8C,
	0x2E,
	0x0E,
	0x4B,
	0x50,
	0xB5,
	0x06,
	0x00,
	0x00,
	0x00,
	0x00,
	0x02,
	0x1D,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x03,
	0x64,
	0x32,
	0x00,
	0x00,
	0x00,
	0x3C,
	0x78,
	0x94,
	0xC5,
	0x02,
	0x00,
	0x00,
	0x00,
	0x00,
	0x92,
	0x40,
	0x00,
	0x81,
	0x4A,
	0x00,
	0x73,
	0x55,
	0x00,
	0x68,
	0x61,
	0x00,
	0x5E,
	0x70,
	0x00,
	0x5E,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,
	0x02,
	0x04,
	0x06,
	0x08,
	0x0A,
	0x0C,
	0x0E,
	0x10,
	0x12,
	0x14,
	0x16,
	0x18,
	0x1A,
	0x1C,
	0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x04,
	0x06,0x08,0x0A,0x0C,0x0F,0x10,0x12,0x13,0x14,
	0x16,0x18,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,
	0x24,0x26,0x28,0x29,0x2A,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0xA2,0x01
};

uint8_t s_GT911_ClearStatue[]= 
{ 
	0x00 
}; 


GT911_T g_GT911; 

/* 
*********************************************************************************************************
*	函 数 名: GT911_INT_GPIO_Input_Init 
*	功能说明: 初始化RST引脚为推挽输出，INT引脚为开漏输出 
*	形 参: 无 
*	返 回 值: 无 
*********************************************************************************************************
*/ 
void GT911_RST_INT_GPIO_Init(void) 
{ 
  GPIO_InitTypeDef  GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);	 //使能PF端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				 // PF11端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF11推挽输出
	GPIO_SetBits(GPIOF,GPIO_Pin_4);//上拉
		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 // PB0端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //开漏输出
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF10上拉输入
	GPIO_SetBits(GPIOF,GPIO_Pin_0);//上拉	

} 
/*  
*********************************************************************************************************
*	函 数 名: GT911_INT_GPIO_Input_Init 
*	功能说明: 设定INT引脚为输入悬空 
*	形 参: 无 
*	返 回 值: 无 
*********************************************************************************************************
*/ 
void GT911_INT_GPIO_Input_Init(void) 
{ 
  GPIO_InitTypeDef  GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);	 //使能PF端口时钟

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 // PB2端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //浮空输入
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF10上拉输入
	GPIO_SetBits(GPIOF,GPIO_Pin_10);//上拉

} 


/* 
*********************************************************************************************************
*	函 数 名: GT911_WriteReg 
*	功能说明: 写1个或连续的多个寄存器 
*	形 参: _usRegAddr : 寄存器地址 
*	_pRegBuf : 寄存器数据缓冲区 
*	_ucLen : 数据长度 
*	返 回 值: 无 
*********************************************************************************************************
*/ 
static void GT911_WriteReg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen) 
{ 
    uint8_t i; 

    CT_IIC_Start();	/* 总线开始信号 */ 

    CT_IIC_Send_Byte(g_GT911.i2c_addr);	/* 发送设备地址+写信号 */ 
    CT_IIC_Wait_Ack(); 

    CT_IIC_Send_Byte(_usRegAddr >> 8);	/* 地址高8位 */ 
    CT_IIC_Wait_Ack(); 

    CT_IIC_Send_Byte(_usRegAddr);	/* 地址低8位 */ 
    CT_IIC_Wait_Ack(); 

    for (i = 0; i < _ucLen; i++) 
    { 
        CT_IIC_Send_Byte(_pRegBuf[i]);	/* 寄存器数据 */ 
        CT_IIC_Wait_Ack(); 
    } 

    CT_IIC_Stop(); /* 总线停止信号 */ 
} 

/* 
*********************************************************************************************************
*	函 数 名: GT911_ReadReg 
*	功能说明: 读1个或连续的多个寄存器 
*	形 参: _usRegAddr : 寄存器地址 
*	_pRegBuf : 寄存器数据缓冲区 
*	_ucLen : 数据长度 
*	返 回 值: 无 
*********************************************************************************************************
*/ 
static void GT911_ReadReg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen) 
{ 
    uint8_t i; 

    CT_IIC_Start();	/* 总线开始信号 */ 

    CT_IIC_Send_Byte(g_GT911.i2c_addr);	/* 发送设备地址+写信号 */ 
    CT_IIC_Wait_Ack(); 

    CT_IIC_Send_Byte(_usRegAddr >> 8);	/* 地址高8位 */ 
    CT_IIC_Wait_Ack(); 

    CT_IIC_Send_Byte(_usRegAddr);	/* 地址低8位 */ 
    CT_IIC_Wait_Ack(); 
    CT_IIC_Stop();	
    CT_IIC_Start(); 
    CT_IIC_Send_Byte(g_GT911.i2c_addr + 0x01);	/* 发送设备地址+读信号 */ 
    CT_IIC_Wait_Ack(); 

    for (i = 0; i < _ucLen - 1; i++) 
    { 
        _pRegBuf[i] = CT_IIC_Read_Byte(1);	/* 读寄存器数据 */ 
//        CT_IIC_Ack(); 
    } 

    /* 最后一个数据 */ 
    _pRegBuf[i] = CT_IIC_Read_Byte(0);	/* 读寄存器数据 */ 
//    i2c_NAck(); 

    CT_IIC_Stop();	/* 总线停止信号 */ 
} 



/* 
*********************************************************************************************************
*	函 数 名: GT911_Reset_Sequence 
*	功能说明: G911硬复位操作,RST为低电平时，INT持续为低电平，1ms后RST置为高电平，10ms后INT设置为输入， 
*	使GT911地址设定为0xBA/0xBB。 
*	形 参: 无 
*	返 回 值: 无 
*********************************************************************************************************
*/ 
void GT911_Reset_Sequence(uint8_t ucAddr) 
{ 
	GT911_RST_INT_GPIO_Init(); 
	switch(ucAddr) 
	{ 
		case 0xBA: 
				GT911_RST_0(); //RST引脚低电平 
				GT911_INT_0(); //INT引脚低电平 
				delay_ms(30); //延时30ms，最短1 
				GT911_RST_1(); //RST引脚高电平 
				GT911_INT_0(); //INT引脚低电平 
				delay_ms(30); //延时30ms，最短20 
				GT911_INT_0(); 
				delay_ms(30); //延时30ms，最短20 
				GT911_INT_1(); 
				break; 
		case 0x28: 
				GT911_RST_0(); //RST引脚低电平 
				GT911_INT_1(); //INT引脚高电平 
				delay_ms(30); //延时30ms，最短1 
				GT911_RST_1(); //RST引脚高电平 
				GT911_INT_1(); //INT引脚高电平 
				delay_ms(30); //延时30ms，最短20 
				GT911_INT_0(); 
				delay_ms(30); //延时30ms，最短20 
				GT911_INT_1(); 
				break; 
		default: //缺省为0xBA 
				GT911_RST_0(); //RST引脚低电平 
				GT911_INT_0(); //INT引脚低电平 
				delay_ms(30); //延时30ms，最短1 
				GT911_RST_1(); //RST引脚高电平 
				GT911_INT_0(); //INT引脚低电平 
				delay_ms(30); //延时30ms，最短20 
				GT911_INT_0(); 
				delay_ms(30); //延时30ms，最短20 
				GT911_INT_1(); 
				break; 
	} 
} 

/* 
*********************************************************************************************************
*	函 数 名: GT911_Soft_Reset 
*	功能说明: G911软复位操作。 
*	形 参: 无 
*	返 回 值: 无 
*********************************************************************************************************
*/ 
void GT911_Soft_Reset(void) 
{ 
	uint8_t buf[1]; 
	buf[0] = 0x01; 
	GT911_WriteReg(GT911_COMMAND_REG, (uint8_t *)buf, 1); 
} 


/* 
*********************************************************************************************************
*	函 数 名: GT911_ReadStatue 
*	功能说明: G911读产品ID与配置参数版本号。 
*	形 参: 无 
*	返 回 值: 配置参数版本号 
*********************************************************************************************************
*/ 
uint8_t GT911_ReadStatue(void) 
{ 
	uint8_t buf[4]; 
	GT911_ReadReg(GT911_PRODUCT_ID_REG, (uint8_t *)&buf[0], 3); 
	GT911_ReadReg(GT911_CONFIG_VERSION_REG, (uint8_t *)&buf[3], 1); 
 // printf("TouchPad_ID:%c,%c,%c\r\nTouchPad_Config_Version:%2x\r\n",buf[0],buf[1],buf[2],buf[3]);
	return buf[3]; 
} 


/* 
*********************************************************************************************************
*	函 数 名: GT911_ReadFirmwareVersion 
*	功能说明: 获得GT911的芯片固件版本 
*	形 参: 无 
*	返 回 值: 16位版本号 
*********************************************************************************************************
*/ 
uint16_t GT911_ReadFirmwareVersion(void) 
{ 
	uint8_t buf[2]; 
	GT911_ReadReg(GT911_FIRMWARE_VERSION_REG, buf, 2); 
	return ((uint16_t)buf[1] << 8) + buf[0]; 
} 


/* 
*********************************************************************************************************
*	函 数 名: GT911_ReadProductID 
*	功能说明: 识别显示模块类别。读取GT911 ProductID。 
*	形 参: 无 
*	返 回 值: 显示模块类别 
*********************************************************************************************************
*/ 
uint32_t GT911_ReadProductID(void) 
{ 
    uint8_t buf[4]; 
    uint32_t value = 0; 
    /* Product_ID*/ 
    GT911_ReadReg(GT911_PRODUCT_ID_REG, buf, 4); 
    value = ((uint32_t)buf[3]<<24)+((uint32_t)buf[2]<<16)+((uint32_t)buf[1]<<8)+buf[0]; 
    return value; 
} 


/* 
*********************************************************************************************************
*	函 数 名: GT911_InitHard 
*	功能说明: 配置触摸芯片. 
*	形 参: 无 
*	返 回 值: 无 
*********************************************************************************************************
*/ 

void GT911_InitHard(void) 
{ 
	uint8_t config_Checksum = 0,i; 

	g_GT911.i2c_addr = GT911_I2C_ADDR; 
	CT_IIC_Init(); 
	GT911_Reset_Sequence(g_GT911.i2c_addr); /*复位GT911，设定设备地址为0xBA/0xBB*/ 
	/* I2C总线初始化在 bsp.c 中执行 */ 
	GT911_Soft_Reset(); /*软复位*/ 
	/*读取配置文件版本，计算校验和*/ 
	s_GT911_CfgParams[0] = GT911_ReadStatue(); 
	for(i=0;i<sizeof(s_GT911_CfgParams)-2;i++) 
	{ 
		config_Checksum += s_GT911_CfgParams[i]; 
	} 
	s_GT911_CfgParams[184] = (~config_Checksum)+1; 
	/* 发送配置信息参数 */ 
	GT911_WriteReg(GT911_CONFIG_REG, (uint8_t *)s_GT911_CfgParams, sizeof(s_GT911_CfgParams)); 
	GT911_INT_GPIO_Input_Init(); //设定INT引脚为输入悬空 
	GT911_Soft_Reset(); /*软复位*/ 
	/*初始化校准，等待200ms*/ 
	delay_ms(200); 
	g_GT911.Enable = 1; 
}

/* 
*********************************************************************************************************
*	函 数 名: GT911_Scan 
*	功能说明: 读取GT911触摸数据。读取全部的数据。 
*	形 参: 无 
*	返 回 值: 无 
*********************************************************************************************************
*/ 
u8 GT911_Scan(u8 mode) 
{ 
	uint8_t buf[40]; 
	uint8_t Clearbuf = 0; 
	u8 res = 0;
	u8 temp;
	static u8 Mode = 0;
	static u8 t = 0;				//控制查询间隔，从而降低CPU占用率

	t++;
	if((t%2) == 0)		//空闲时,每进入5次CTP_Scan函数才检测1次,从而节省CPU使用率
	{
		GT911_ReadReg(GT911_READ_XY_REG, &Mode, 1);
		if ((Mode & 0x0F) == 0) 
		{ 
			GT911_WriteReg(GT911_CLEARBUF_REG, (uint8_t *)&Clearbuf, 1);
			t = 0;
			return 0; 
		}
		if(((Mode&0x0f))&&((Mode&0x0f)<6))
		{
			temp = 0xff<<(mode&0x0f);
			tp_dev.sta = (~temp)|TP_PRES_DOWN|TP_CATH_PRES;
			GT911_ReadReg(GT911_READ_XY_REG+1, &buf[1], 39); 
			GT911_WriteReg(GT911_CLEARBUF_REG, (uint8_t *)&Clearbuf, 1);  

			g_GT911.TouchpointFlag = buf[0]; 

			g_GT911.Touchkey1trackid = buf[1]; 
			tp_dev.x[0] = ((uint16_t)buf[3] << 8) + buf[2]; 
			tp_dev.y[0] = ((uint16_t)buf[5] << 8) + buf[4];
			
			g_GT911.X1 = tp_dev.x[0];
			g_GT911.Y1 = tp_dev.y[0];
			g_GT911.S1 = ((uint16_t)buf[7] << 8) + buf[6]; 

			res = 1;
			if(tp_dev.x[0]==0 && tp_dev.y[0]==0)Mode=0;	//读到的数据都是0,则忽略此次数据
			t = 0;
		}
	}
	
	if((Mode&0X1F)==0)//无触摸点按下
	{ 
		if(tp_dev.sta&TP_PRES_DOWN)	//之前是被按下的
		{
			tp_dev.sta&=~(1<<7);	//标记按键松开
		}
		else						//之前就没有被按下
		{ 
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
			tp_dev.sta&=0XE0;	//清除点有效标记	
		}	 
	} 	
	if(t>240)	t=2;//重新从5开始计数
	return res;
	
} 





