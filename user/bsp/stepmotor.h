#ifndef __STEPMOTOR_H
#define __STEPMOTOR_H

#include "stm32f10x.h"

typedef struct{
	u32		subdiv;					//细分值
	float speed;					//速度
	float angle;					//当前角度
	float targetangel;		//目标角度
	float pulseangle;			//单脉冲的角度
}STEPMOTOR_TypeDef;


typedef enum{
	DIR_LEFT=0,
	DIR_RIGHT,
}STEPDIR_Def;

#define VERSION2

#ifdef VERSION2
//PB5
#define STEPDIRGET()	(GPIOB->ODR & (1<<5))
#define STEPDIR(x)		x?(GPIOB->ODR |= (1<<5)):(GPIOB->ODR &=~ (1<<5))
#else
//PA10
#define STEPDIRGET()	(GPIOA->ODR & (1<<10))
#define STEPDIR(x)		(x)?(GPIOA->ODR |= (1<<10)):(GPIOA->ODR &=~ (1<<10))
#endif

void StepMonter_Init (void);
void StepMontor_Move (float angle);
void StepMontor_Stop (void);
void StepMotor_SetSpeed(float angle);
u8 Step_MoveCheck(void);

#endif
