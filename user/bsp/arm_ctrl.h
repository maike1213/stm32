#ifndef __ARM_CTRL_H
#define __ARM_CTRL_H

#include "stm32f10x.h"

#define PI	3.1415926535897

#define DUOJI1		4
#define DUOJI2		3
#define	DUOJI3		2
#define	DUOJI4		1
#define PAW				5

typedef struct
{
  float Kp;                       //比例系数Proportional
  float Ki;                       //积分系数Integral
  float Kd;                       //微分系数Derivative
 
  float Ek;                       //当前误差
  float Ek1;                      //前一次误差 e(k-1)
  float Ek2;                      //再前一次误差 e(k-2)
  float LocSum;                   //累计积分位置
}PID_LocTypeDef;									//位置式PID





extern PID_LocTypeDef position_pid[6];
extern float Arm_Position[6];//当前位置
extern float Arm_SetValue[6];//目标位置

extern float Arm_TargetAngle[6];//目标角度
extern float Arm_InitAngle[6];//舵机初始化角度


void ArmCtrl_Init(void);
float PID_Location(float SetValue, float ActualValue, PID_LocTypeDef * PID);
void Arm_SetDJ_PWM(u8 djnum,float pwm);
void Arm_Update_Position(void);
void Arm_SetAngle(u8 djnum, float angle);
void Arm_UpdateAngle(float angle1, float angle2, float angle3, float angle4, float angle5);
u8 Difference_Angle(u8 num);

#endif
