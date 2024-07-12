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


//触摸屏初始化  		    
//返回值:0,没有进行校准
//       1,进行过校准
u8 TP_Init(void)
{	
	if(lcddev.id == 0X1963)			//7寸电容触摸屏
	{
		GT911_InitHard();
		tp_dev.scan = GT911_Scan;		//扫描函数指向GT9147触摸屏扫描
		tp_dev.touchtype|=0X80;			//电容屏 
		tp_dev.touchtype|=lcddev.dir&0X01;//横屏还是竖屏 
		return 0;
	}
	return 1; 									 
}


