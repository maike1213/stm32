#include "touch.h"
#include "bsp_lcd.h"
#include "gt911.h"

_m_tp_dev tp_dev=
{
	TP_Init,
	GT911_Scan,
	0,
	0,
	0, 
	0,
	0,
	0,
	0,	  	 		
	0,
	0,	  	 		
};	


//��������ʼ��  		    
//����ֵ:0,û�н���У׼
//       1,���й�У׼
u8 TP_Init(void)
{	
	if(lcddev.id == 0X1963)			//7����ݴ�����
	{
		GT911_InitHard();
		tp_dev.scan = GT911_Scan;		//ɨ�躯��ָ��GT9147������ɨ��
		tp_dev.touchtype|=0X80;			//������ 
		tp_dev.touchtype|=lcddev.dir&0X01;//������������ 
		return 0;
	}
	return 1; 									 
}


