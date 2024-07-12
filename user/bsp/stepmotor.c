#include "stepmotor.h"

/*************************************************************************
 *
 *				�������
 *
*************************************************************************/
//														ϸ��ֵ
STEPMOTOR_TypeDef stepmotor = {1600,0,0,0,0};

#ifdef VERSION2

/**
  * @brief  ��ʼ���������������
	*					PB5		DIR			PB6		PWM TIM4_CH1
  * @retval None
  */
void StepMonter_Init (void)
{
	/* GPIOB clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	//PB5	DIR
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// PB6/TIM4_CH1		PUL
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//TIM4_CH1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	/* Time base configuration */
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	TIM_TimeBaseStructure.TIM_Period = 2000-1;//������װ��ֵ2ms��500hz
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;//��time4�ļ����ٶȷ�Ƶ��1M
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_SetCompare1(TIM4, 1000-1);//���ñȽ�ֵ
	
	StepMotor_SetSpeed(50);//��ʼ�ٶ�Ϊ60��/s
	StepMontor_Stop();
	
	//ʹ�ܸ����ж�
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	NVIC_EnableIRQ (TIM4_IRQn);
}

/**
  * @brief  �ò��������ת��ָ���Ƕ�
  * @param 	angle  �����õĽǶ�ֵ
  * @retval None
  */
void StepMontor_Move (float angle)
{
	stepmotor.targetangel = angle;//����Ŀ��ֵ
	float difvalue = stepmotor.angle - stepmotor.targetangel;
	if(difvalue < -stepmotor.pulseangle || difvalue > stepmotor.pulseangle)
	{
		if(angle > 0 && difvalue < 0)
			STEPDIR(DIR_RIGHT);
		if((angle > 0 || angle == 0) && difvalue > 0)
			STEPDIR(DIR_LEFT);
		if((angle < 0 || angle == 0) && difvalue < 0)
			STEPDIR(DIR_RIGHT);
		if(angle < 0 && difvalue > 0)
			STEPDIR(DIR_LEFT);
		TIM_Cmd(TIM4, ENABLE);
	}
	else
		TIM_Cmd(TIM4, DISABLE);
}

u8 Step_MoveCheck(void)
{
	float difvalue = stepmotor.angle - stepmotor.targetangel;	
	if(difvalue < -stepmotor.pulseangle || difvalue > stepmotor.pulseangle)
		return 0;//û���ƶ���λ
	else
		return 1;//�ƶ���λ
}

void StepMontor_Stop (void)
{
	TIM_Cmd(TIM4, DISABLE);
}


/********************************************
		���ò����������ת�ٶ�	
		���Σ�angle		angle��/s
*********************************************/
void StepMotor_SetSpeed(float angle)
{
	stepmotor.pulseangle = 360.0 / stepmotor.subdiv;
	stepmotor.speed = angle;
	
	//�����ٶ���ÿ���ӵ������� = ÿ���ӵĽǶ� / ������Ƕ�
	float pulsenum = angle / stepmotor.pulseangle + 0.5;
	if(pulsenum < 0.16)
	{
		//���ÿ����������С��0.16�����������ٶ�Ϊÿ��0.16������
		TIM4->PSC = 7200 - 1;
		TIM_SetAutoreload(TIM4, 65535);
		TIM_SetCompare1(TIM4, 32768);		
	}
	else if(pulsenum < 16)
	{
		//�����������ٶȱ� 65.536ms ��,��ô���ǿ��������޸ķ�Ƶֵ
		TIM4->PSC = 7200 - 1;
		TIM_SetAutoreload(TIM4, 10000.0f/pulsenum);
		TIM_SetCompare1(TIM4, 10000.0f/pulsenum/2);
	}
	else
	{
		//��1usһ�εļ��������£�1s�ڵ�����������1usһ�Σ�����65.536msһ��
		TIM4->PSC = 72 - 1;//72��Ƶ��1us����һ��
		TIM_SetAutoreload(TIM4, 1000000.0f/pulsenum);
		TIM_SetCompare1(TIM4, 1000000.0f/pulsenum/2);	
	}
	
}

//�жϷ�����
//ִ�в����������ת��ֹͣ
void TIM4_IRQHandler (void)
{
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	if (STEPDIRGET())
		stepmotor.angle += stepmotor.pulseangle;
	else
		stepmotor.angle -= stepmotor.pulseangle;
	
	//��⵱ǰ�Ƕ���Ŀ��ǶȲ�ֵ�������һ���������Ƕ�֮�����ʾ�Ƕ�һ�£��رն�ʱ��
	float difvalue = stepmotor.angle - stepmotor.targetangel;
	if(difvalue < -stepmotor.pulseangle || difvalue > stepmotor.pulseangle)
		TIM_Cmd(TIM4, ENABLE);
	else
		TIM_Cmd(TIM4, DISABLE);
}

#else


/**
  * @brief  ��ʼ���������������
	*					PA10		DIR			PB9		PWM TIM4_CH4
  * @retval None
  */
void StepMonter_Init (void)
{
	/* GPIOB clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	//PA10	DIR
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// PB9/TIM4_CH4		PUL
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//TIM4_CH4
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	/* Time base configuration */
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	TIM_TimeBaseStructure.TIM_Period = 2000-1;//������װ��ֵ2ms��500hz
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;//��time4�ļ����ٶȷ�Ƶ��1M
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_SetCompare4(TIM4, 1000-1);//���ñȽ�ֵ
	
	StepMotor_SetSpeed(50);//��ʼ�ٶ�Ϊ50��/s
	StepMontor_Stop();
	
	//ʹ�ܸ����ж�
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	NVIC_EnableIRQ (TIM4_IRQn);
}

/**
  * @brief  �ò��������ת��ָ���Ƕ�
  * @param 	angle  �����õĽǶ�ֵ
  * @retval None
  */
void StepMontor_Move (float angle)
{
	stepmotor.targetangel = angle;//����Ŀ��ֵ
	float difvalue = stepmotor.angle - stepmotor.targetangel;
	if(difvalue < -stepmotor.pulseangle || difvalue > stepmotor.pulseangle)
	{
		if(angle > 0 && difvalue < 0)
			STEPDIR(DIR_RIGHT);
		if((angle > 0 || angle == 0) && difvalue > 0)
			STEPDIR(DIR_LEFT);
		if((angle < 0 || angle == 0) && difvalue < 0)
			STEPDIR(DIR_RIGHT);
		if(angle < 0 && difvalue > 0)
			STEPDIR(DIR_LEFT);
		TIM_Cmd(TIM4, ENABLE);
	}
	else
		TIM_Cmd(TIM4, DISABLE);
}

u8 Step_MoveCheck(void)
{
	float difvalue = stepmotor.angle - stepmotor.targetangel;	
	if(difvalue < -stepmotor.pulseangle || difvalue > stepmotor.pulseangle)
		return 0;//û���ƶ���λ
	else
		return 1;//�ƶ���λ
}

void StepMontor_Stop (void)
{
	TIM_Cmd(TIM4, DISABLE);
}


/********************************************
		���ò����������ת�ٶ�	
		���Σ�angle		angle��/s
*********************************************/
void StepMotor_SetSpeed(float angle)
{
	//���������ת�Ƕ�
	stepmotor.pulseangle = 360.0 / stepmotor.subdiv;
	stepmotor.speed = angle;
	
	//�����ٶ���ÿ���ӵ������� = ÿ���ӵĽǶ� / ������Ƕ�
	float pulsenum = angle / stepmotor.pulseangle + 0.5;
	if(pulsenum < 0.16)
	{
		//���ÿ����������С��0.16�����������ٶ�Ϊÿ��0.16������
		TIM4->PSC = 7200 - 1;
		TIM_SetAutoreload(TIM4, 65535);
		TIM_SetCompare4(TIM4, 32768);		
	}
	else if(pulsenum < 16)
	{
		//�����������ٶȱ� 65.536ms ��,��ô���ǿ��������޸ķ�Ƶֵ
		TIM4->PSC = 7200 - 1;
		TIM_SetAutoreload(TIM4, 10000.0f/pulsenum);
		TIM_SetCompare4(TIM4, 10000.0f/pulsenum/2);
	}
	else
	{
		//��1usһ�εļ��������£�1s�ڵ�����������1usһ�Σ�����65.536msһ��
		TIM4->PSC = 72 - 1;//72��Ƶ��1us����һ��
		TIM_SetAutoreload(TIM4, 1000000.0f/pulsenum);
		TIM_SetCompare4(TIM4, 1000000.0f/pulsenum/2);	
	}
	
}

//�жϷ�����
//ִ�в����������ת��ֹͣ
void TIM4_IRQHandler (void)
{
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	if (STEPDIRGET())
		stepmotor.angle += stepmotor.pulseangle;
	else
		stepmotor.angle -= stepmotor.pulseangle;
	
	//��⵱ǰ�Ƕ���Ŀ��ǶȲ�ֵ�������һ���������Ƕ�֮�����ʾ�Ƕ�һ�£��رն�ʱ��
	float difvalue = stepmotor.angle - stepmotor.targetangel;
	if(difvalue < -stepmotor.pulseangle || difvalue > stepmotor.pulseangle)
		TIM_Cmd(TIM4, ENABLE);
	else
		TIM_Cmd(TIM4, DISABLE);
}

#endif



