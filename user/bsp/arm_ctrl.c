#include "arm_ctrl.h"
#include <math.h>
#include <stdio.h>
#include <string.h>



/**************************************************
	��е�� 		�����ʼ������
***************************************************/
void ArmCtrl_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	TIM_TimeBaseStructure.TIM_Period = 20000-1;//��װ��ֵ��20ms��50hz
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1Init(TIM5, &TIM_OCInitStructure);
	TIM_OC2Init(TIM5, &TIM_OCInitStructure);
	TIM_OC3Init(TIM5, &TIM_OCInitStructure);
	TIM_OC4Init(TIM5, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM5, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_ARRPreloadConfig(TIM5, ENABLE);

	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM5, ENABLE);
	
	//��ʼ������Ƕ�
	for(u8 i=0; i<6; i++)
	{
		Arm_SetAngle(i,Arm_InitAngle[i]);
		Arm_SetDJ_PWM(i,Arm_SetValue[i]);
	}	
	

}

//�����λ��ʽPID����							Pi	Ki	Di	Ek	Ek1	Ek2	LocSum
PID_LocTypeDef position_pid[6] = {{6,	0,	3,	0,	0,	0,	0},
																	{6,	0,	3,	0,	0,	0,	0},
																	{6,	0,	3,	0,	0,	0,	0},
																	{6,	0,	3,	0,	0,	0,	0},
																	{6,	0,	3,	0,	0,	0,	0},
																	{6,	0,	3,	0,	0,	0,	0}};


/*************************************************************************
�������� �� PID_Location
��    �� �� PIDλ��(Location)����
��    �� �� SetValue ------ ����ֵ(����ֵ)
            ActualValue --- ʵ��ֵ(����ֵ)
            PID ----------- PID���ݽṹ
�� �� ֵ �� PIDLoc -------- PIDλ��
*************************************************************************/
float PID_Location(float SetValue, float ActualValue, PID_LocTypeDef * PID)
{
	float PIDLoc;
	PID->Ek = SetValue - ActualValue;
	PID->LocSum += PID->Ek;
	PIDLoc = PID->Kp*PID->Ek/100 + PID->Ki*PID->LocSum/100 + PID->Kd*(PID->Ek1-PID->Ek)/100;
	PID->Ek1 = PID->Ek;
	return PIDLoc;
}


float Arm_Position[6] = {1500,1500,1500,1500,1500,1500};		//�����ǰλ��
float Arm_SetValue[6] = {1500,1500,1500,1500,1500,1500};		//���Ŀ��λ��

/************************************************************************
�������� ��Arm_SetDJ_PWM
��		�� �����ö��ռ�ձ�
��		�� ��djnum -------- �����		0			1			2			3			4			5
																		��		4��		3��		2��		1��		צ
					 pwm ----------	ռ�ձ�
�� �� ֵ ����

								2��-----3��-4��-צ
							 /
							/
						1��	
*********************************************************************/
void Arm_SetDJ_PWM(u8 djnum,float pwm)
{
	switch(djnum)
	{
		case 0:TIM_SetCompare1(TIM5, pwm);break;
		case 1:TIM_SetCompare2(TIM5, pwm);break;
		case 2:TIM_SetCompare3(TIM5, pwm);break;
		case 3:TIM_SetCompare4(TIM5, pwm);break;
		case 4:TIM_SetCompare1(TIM3, pwm);break;
		case 5:TIM_SetCompare2(TIM3, pwm);break;
	}
}

//�ж�ǰ�����Ƕ��Ƿ��ƶ���λ����
//��Χ����1
//num = 6
u8 Difference_Angle(u8 num)
{
	float dif = 0;
	for(u8 i=0; i<num; i++)
	{
		dif = Arm_Position[i] - (u16)Arm_SetValue[i];
		if(dif > 1 || dif < -1)
			return 0;
	}
	return 1;
}

/***********************************************************************************
�������� ��Arm_Update_Position
��		�� �����²����ö��λ�ã�ռ�ձȣ�		ִ��
��		�� ����
�� �� ֵ ����
***********************************************************************************/
void Arm_Update_Position(void)
{
	for(u8 i=0; i<6; i++)
	{
		Arm_Position[i] += PID_Location(Arm_SetValue[i], Arm_Position[i], &position_pid[i]);
		Arm_SetDJ_PWM(i, Arm_Position[i]);
	}
}

float Arm_InitAngle[6] = {90,0,-90,0,90,120};			//�����ʼ���Ƕ�
float Arm_TargetAngle[6] = {90,0,-90,0,90,120};		//���Ŀ��Ƕ�
/********************************************************************************************************
	�����		0			1			2			3			4			5
	���			��		4��		3��		2��		1��		צ

	LD20		500 -- 2500us  0�� -- 180��			���ֶ��
	LD1501	500 -- 2500us  0�� -- 180��			���ֶ��
	LD1501
	LD1501
	LD335		500 -- 2500us  0�� -- 180��			���ֶ��

	����Ƕ�����ʼ�����ϵ��
			1�Ŷ�������90��		-----			����90��			500 -- 180��			2500 - 0��			180  -- 0														
			2�Ŷ�������90��		-----			����0��			500 -- -90��			2500 - 90��		-90	 -- 90															
			3�Ŷ�������90��		-----			����-90��		500 -- -180��		2500 - 0��			-180 -- 0														
			4�Ŷ�������90��		-----			ĩ��ִ������ˮƽ��ƽ��	����Ƕ����е��������x��ļнǦ��ı�			
																		-90 - 90							���ڿ���צ��Ťת�Ƕ�
			5�Ŷ�������90��		-----			��Ӧצ�ӵļо� Ϊ�м�ֵ			500 -- ���о�180		2500 -- ��С�о�0												
*********************************************************************************************************/

void Arm_SetAngle(u8 djnum, float angle)
{
	Arm_TargetAngle[djnum] = angle;
	switch(djnum)
	{
		case 0:;break;//��
		case 1://4��	��Χ -90 -- 90			���ݲ���ʱ���������x���ƫ��angle	500����Ϊ	angle ֵΪ��		
			Arm_SetValue[djnum] = Arm_TargetAngle[djnum]/180.0*2000+1500;
		break;		
		case 2://3��	��Χ -180�� -- 0��		500 - 2500
			Arm_SetValue[djnum] = (180+Arm_TargetAngle[djnum])/180.0*2000+500;
		break;
		case 3://2��	��Χ -90 -- 90			500 - 2500
			Arm_SetValue[djnum] = Arm_TargetAngle[djnum]/180.0*2000+1500;
		break;
		case 4://1��	��Χ 180 -- 0				500 - 2500
			Arm_SetValue[djnum] = (180-Arm_TargetAngle[djnum])/180.0*2000+500;
		break;
		case 5://צ		�о� 0 -- 5.5cm			2500 - 500	180 - 0
			//צ���ڼ�ȡ������Ҫ�ȴ���������ƶ���ɲ���ץȡ
			Arm_SetValue[djnum] = (180 - Arm_TargetAngle[djnum])/180.0*2000+500;
			break;
	}
}

//����Ŀ��Ƕ�ֵ����
void Arm_UpdateAngle(float angle1, float angle2, float angle3, float angle4, float angle5)
{
	Arm_TargetAngle[DUOJI1] = angle1;
	Arm_TargetAngle[DUOJI2] = angle2;
	Arm_TargetAngle[DUOJI3] = angle3;
	Arm_TargetAngle[DUOJI4] = angle4;
	Arm_TargetAngle[PAW] = angle5;
	for(u8 i=0; i<6; i++)
		Arm_SetAngle(i, Arm_TargetAngle[i]);
}
