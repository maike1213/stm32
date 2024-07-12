#include "gt911.h"
#include "delay.h"

#include "touch.h"

#define delay_us	Delay_us
#define delay_ms	Delay_ms

/* GT911�����������ò�����һ����д�� */ 

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
*	�� �� ��: GT911_INT_GPIO_Input_Init 
*	����˵��: ��ʼ��RST����Ϊ���������INT����Ϊ��©��� 
*	�� ��: �� 
*	�� �� ֵ: �� 
*********************************************************************************************************
*/ 
void GT911_RST_INT_GPIO_Init(void) 
{ 
  GPIO_InitTypeDef  GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);	 //ʹ��PF�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				 // PF11�˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF11�������
	GPIO_SetBits(GPIOF,GPIO_Pin_4);//����
		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 // PB0�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //��©���
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF10��������
	GPIO_SetBits(GPIOF,GPIO_Pin_0);//����	

} 
/*  
*********************************************************************************************************
*	�� �� ��: GT911_INT_GPIO_Input_Init 
*	����˵��: �趨INT����Ϊ�������� 
*	�� ��: �� 
*	�� �� ֵ: �� 
*********************************************************************************************************
*/ 
void GT911_INT_GPIO_Input_Init(void) 
{ 
  GPIO_InitTypeDef  GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);	 //ʹ��PF�˿�ʱ��

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 // PB2�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //��������
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF10��������
	GPIO_SetBits(GPIOF,GPIO_Pin_10);//����

} 


/* 
*********************************************************************************************************
*	�� �� ��: GT911_WriteReg 
*	����˵��: д1���������Ķ���Ĵ��� 
*	�� ��: _usRegAddr : �Ĵ�����ַ 
*	_pRegBuf : �Ĵ������ݻ����� 
*	_ucLen : ���ݳ��� 
*	�� �� ֵ: �� 
*********************************************************************************************************
*/ 
static void GT911_WriteReg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen) 
{ 
    uint8_t i; 

    CT_IIC_Start();	/* ���߿�ʼ�ź� */ 

    CT_IIC_Send_Byte(g_GT911.i2c_addr);	/* �����豸��ַ+д�ź� */ 
    CT_IIC_Wait_Ack(); 

    CT_IIC_Send_Byte(_usRegAddr >> 8);	/* ��ַ��8λ */ 
    CT_IIC_Wait_Ack(); 

    CT_IIC_Send_Byte(_usRegAddr);	/* ��ַ��8λ */ 
    CT_IIC_Wait_Ack(); 

    for (i = 0; i < _ucLen; i++) 
    { 
        CT_IIC_Send_Byte(_pRegBuf[i]);	/* �Ĵ������� */ 
        CT_IIC_Wait_Ack(); 
    } 

    CT_IIC_Stop(); /* ����ֹͣ�ź� */ 
} 

/* 
*********************************************************************************************************
*	�� �� ��: GT911_ReadReg 
*	����˵��: ��1���������Ķ���Ĵ��� 
*	�� ��: _usRegAddr : �Ĵ�����ַ 
*	_pRegBuf : �Ĵ������ݻ����� 
*	_ucLen : ���ݳ��� 
*	�� �� ֵ: �� 
*********************************************************************************************************
*/ 
static void GT911_ReadReg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen) 
{ 
    uint8_t i; 

    CT_IIC_Start();	/* ���߿�ʼ�ź� */ 

    CT_IIC_Send_Byte(g_GT911.i2c_addr);	/* �����豸��ַ+д�ź� */ 
    CT_IIC_Wait_Ack(); 

    CT_IIC_Send_Byte(_usRegAddr >> 8);	/* ��ַ��8λ */ 
    CT_IIC_Wait_Ack(); 

    CT_IIC_Send_Byte(_usRegAddr);	/* ��ַ��8λ */ 
    CT_IIC_Wait_Ack(); 
    CT_IIC_Stop();	
    CT_IIC_Start(); 
    CT_IIC_Send_Byte(g_GT911.i2c_addr + 0x01);	/* �����豸��ַ+���ź� */ 
    CT_IIC_Wait_Ack(); 

    for (i = 0; i < _ucLen - 1; i++) 
    { 
        _pRegBuf[i] = CT_IIC_Read_Byte(1);	/* ���Ĵ������� */ 
//        CT_IIC_Ack(); 
    } 

    /* ���һ������ */ 
    _pRegBuf[i] = CT_IIC_Read_Byte(0);	/* ���Ĵ������� */ 
//    i2c_NAck(); 

    CT_IIC_Stop();	/* ����ֹͣ�ź� */ 
} 



/* 
*********************************************************************************************************
*	�� �� ��: GT911_Reset_Sequence 
*	����˵��: G911Ӳ��λ����,RSTΪ�͵�ƽʱ��INT����Ϊ�͵�ƽ��1ms��RST��Ϊ�ߵ�ƽ��10ms��INT����Ϊ���룬 
*	ʹGT911��ַ�趨Ϊ0xBA/0xBB�� 
*	�� ��: �� 
*	�� �� ֵ: �� 
*********************************************************************************************************
*/ 
void GT911_Reset_Sequence(uint8_t ucAddr) 
{ 
	GT911_RST_INT_GPIO_Init(); 
	switch(ucAddr) 
	{ 
		case 0xBA: 
				GT911_RST_0(); //RST���ŵ͵�ƽ 
				GT911_INT_0(); //INT���ŵ͵�ƽ 
				delay_ms(30); //��ʱ30ms�����1 
				GT911_RST_1(); //RST���Ÿߵ�ƽ 
				GT911_INT_0(); //INT���ŵ͵�ƽ 
				delay_ms(30); //��ʱ30ms�����20 
				GT911_INT_0(); 
				delay_ms(30); //��ʱ30ms�����20 
				GT911_INT_1(); 
				break; 
		case 0x28: 
				GT911_RST_0(); //RST���ŵ͵�ƽ 
				GT911_INT_1(); //INT���Ÿߵ�ƽ 
				delay_ms(30); //��ʱ30ms�����1 
				GT911_RST_1(); //RST���Ÿߵ�ƽ 
				GT911_INT_1(); //INT���Ÿߵ�ƽ 
				delay_ms(30); //��ʱ30ms�����20 
				GT911_INT_0(); 
				delay_ms(30); //��ʱ30ms�����20 
				GT911_INT_1(); 
				break; 
		default: //ȱʡΪ0xBA 
				GT911_RST_0(); //RST���ŵ͵�ƽ 
				GT911_INT_0(); //INT���ŵ͵�ƽ 
				delay_ms(30); //��ʱ30ms�����1 
				GT911_RST_1(); //RST���Ÿߵ�ƽ 
				GT911_INT_0(); //INT���ŵ͵�ƽ 
				delay_ms(30); //��ʱ30ms�����20 
				GT911_INT_0(); 
				delay_ms(30); //��ʱ30ms�����20 
				GT911_INT_1(); 
				break; 
	} 
} 

/* 
*********************************************************************************************************
*	�� �� ��: GT911_Soft_Reset 
*	����˵��: G911��λ������ 
*	�� ��: �� 
*	�� �� ֵ: �� 
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
*	�� �� ��: GT911_ReadStatue 
*	����˵��: G911����ƷID�����ò����汾�š� 
*	�� ��: �� 
*	�� �� ֵ: ���ò����汾�� 
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
*	�� �� ��: GT911_ReadFirmwareVersion 
*	����˵��: ���GT911��оƬ�̼��汾 
*	�� ��: �� 
*	�� �� ֵ: 16λ�汾�� 
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
*	�� �� ��: GT911_ReadProductID 
*	����˵��: ʶ����ʾģ����𡣶�ȡGT911 ProductID�� 
*	�� ��: �� 
*	�� �� ֵ: ��ʾģ����� 
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
*	�� �� ��: GT911_InitHard 
*	����˵��: ���ô���оƬ. 
*	�� ��: �� 
*	�� �� ֵ: �� 
*********************************************************************************************************
*/ 

void GT911_InitHard(void) 
{ 
	uint8_t config_Checksum = 0,i; 

	g_GT911.i2c_addr = GT911_I2C_ADDR; 
	CT_IIC_Init(); 
	GT911_Reset_Sequence(g_GT911.i2c_addr); /*��λGT911���趨�豸��ַΪ0xBA/0xBB*/ 
	/* I2C���߳�ʼ���� bsp.c ��ִ�� */ 
	GT911_Soft_Reset(); /*��λ*/ 
	/*��ȡ�����ļ��汾������У���*/ 
	s_GT911_CfgParams[0] = GT911_ReadStatue(); 
	for(i=0;i<sizeof(s_GT911_CfgParams)-2;i++) 
	{ 
		config_Checksum += s_GT911_CfgParams[i]; 
	} 
	s_GT911_CfgParams[184] = (~config_Checksum)+1; 
	/* ����������Ϣ���� */ 
	GT911_WriteReg(GT911_CONFIG_REG, (uint8_t *)s_GT911_CfgParams, sizeof(s_GT911_CfgParams)); 
	GT911_INT_GPIO_Input_Init(); //�趨INT����Ϊ�������� 
	GT911_Soft_Reset(); /*��λ*/ 
	/*��ʼ��У׼���ȴ�200ms*/ 
	delay_ms(200); 
	g_GT911.Enable = 1; 
}

/* 
*********************************************************************************************************
*	�� �� ��: GT911_Scan 
*	����˵��: ��ȡGT911�������ݡ���ȡȫ�������ݡ� 
*	�� ��: �� 
*	�� �� ֵ: �� 
*********************************************************************************************************
*/ 
u8 GT911_Scan(u8 mode) 
{ 
	uint8_t buf[40]; 
	uint8_t Clearbuf = 0; 
	u8 res = 0;
	u8 temp;
	static u8 Mode = 0;
	static u8 t = 0;				//���Ʋ�ѯ������Ӷ�����CPUռ����

	t++;
	if((t%2) == 0)		//����ʱ,ÿ����5��CTP_Scan�����ż��1��,�Ӷ���ʡCPUʹ����
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
			if(tp_dev.x[0]==0 && tp_dev.y[0]==0)Mode=0;	//���������ݶ���0,����Դ˴�����
			t = 0;
		}
	}
	
	if((Mode&0X1F)==0)//�޴����㰴��
	{ 
		if(tp_dev.sta&TP_PRES_DOWN)	//֮ǰ�Ǳ����µ�
		{
			tp_dev.sta&=~(1<<7);	//��ǰ����ɿ�
		}
		else						//֮ǰ��û�б�����
		{ 
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
			tp_dev.sta&=0XE0;	//�������Ч���	
		}	 
	} 	
	if(t>240)	t=2;//���´�5��ʼ����
	return res;
	
} 





