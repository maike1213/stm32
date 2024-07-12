#ifndef __STEPMOTOR_H
#define __STEPMOTOR_H

#include "stm32f10x.h"

typedef struct{
	u32		subdiv;					//ϸ��ֵ
	float speed;					//�ٶ�
	float angle;					//��ǰ�Ƕ�
	float targetangel;		//Ŀ��Ƕ�
	float pulseangle;			//������ĽǶ�
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
