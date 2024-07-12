#include "stepmotor.h"

/*************************************************************************
 *
 *				步进电机
 *
*************************************************************************/
//														细分值
STEPMOTOR_TypeDef stepmotor = {1600,0,0,0,0};

#ifdef VERSION2

/**
  * @brief  初始化基座：步进电机
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

	TIM_TimeBaseStructure.TIM_Period = 2000-1;//设置重装载值2ms，500hz
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;//将time4的计数速度分频至1M
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
	TIM_SetCompare1(TIM4, 1000-1);//设置比较值
	
	StepMotor_SetSpeed(50);//初始速度为60°/s
	StepMontor_Stop();
	
	//使能更新中断
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	NVIC_EnableIRQ (TIM4_IRQn);
}

/**
  * @brief  让步进电机旋转到指定角度
  * @param 	angle  被设置的角度值
  * @retval None
  */
void StepMontor_Move (float angle)
{
	stepmotor.targetangel = angle;//更新目标值
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
		return 0;//没有移动到位
	else
		return 1;//移动到位
}

void StepMontor_Stop (void)
{
	TIM_Cmd(TIM4, DISABLE);
}


/********************************************
		设置步进电机的旋转速度	
		传参：angle		angle°/s
*********************************************/
void StepMotor_SetSpeed(float angle)
{
	stepmotor.pulseangle = 360.0 / stepmotor.subdiv;
	stepmotor.speed = angle;
	
	//根据速度求每秒钟的脉冲数 = 每秒钟的角度 / 单脉冲角度
	float pulsenum = angle / stepmotor.pulseangle + 0.5;
	if(pulsenum < 0.16)
	{
		//如果每秒钟脉冲数小于0.16个，则设置速度为每秒0.16个脉冲
		TIM4->PSC = 7200 - 1;
		TIM_SetAutoreload(TIM4, 65535);
		TIM_SetCompare1(TIM4, 32768);		
	}
	else if(pulsenum < 16)
	{
		//如果脉冲给的速度比 65.536ms 慢,那么我们可以重新修改分频值
		TIM4->PSC = 7200 - 1;
		TIM_SetAutoreload(TIM4, 10000.0f/pulsenum);
		TIM_SetCompare1(TIM4, 10000.0f/pulsenum/2);
	}
	else
	{
		//在1us一次的计数速率下，1s内的脉冲个数最快1us一次，最慢65.536ms一次
		TIM4->PSC = 72 - 1;//72分频，1us计数一下
		TIM_SetAutoreload(TIM4, 1000000.0f/pulsenum);
		TIM_SetCompare1(TIM4, 1000000.0f/pulsenum/2);	
	}
	
}

//中断服务函数
//执行步进电机的旋转和停止
void TIM4_IRQHandler (void)
{
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	if (STEPDIRGET())
		stepmotor.angle += stepmotor.pulseangle;
	else
		stepmotor.angle -= stepmotor.pulseangle;
	
	//检测当前角度与目标角度差值，误差在一个±步进角度之间则表示角度一致，关闭定时器
	float difvalue = stepmotor.angle - stepmotor.targetangel;
	if(difvalue < -stepmotor.pulseangle || difvalue > stepmotor.pulseangle)
		TIM_Cmd(TIM4, ENABLE);
	else
		TIM_Cmd(TIM4, DISABLE);
}

#else


/**
  * @brief  初始化基座：步进电机
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

	TIM_TimeBaseStructure.TIM_Period = 2000-1;//设置重装载值2ms，500hz
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;//将time4的计数速度分频至1M
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
	TIM_SetCompare4(TIM4, 1000-1);//设置比较值
	
	StepMotor_SetSpeed(50);//初始速度为50°/s
	StepMontor_Stop();
	
	//使能更新中断
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	NVIC_EnableIRQ (TIM4_IRQn);
}

/**
  * @brief  让步进电机旋转到指定角度
  * @param 	angle  被设置的角度值
  * @retval None
  */
void StepMontor_Move (float angle)
{
	stepmotor.targetangel = angle;//更新目标值
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
		return 0;//没有移动到位
	else
		return 1;//移动到位
}

void StepMontor_Stop (void)
{
	TIM_Cmd(TIM4, DISABLE);
}


/********************************************
		设置步进电机的旋转速度	
		传参：angle		angle°/s
*********************************************/
void StepMotor_SetSpeed(float angle)
{
	//单脉冲的旋转角度
	stepmotor.pulseangle = 360.0 / stepmotor.subdiv;
	stepmotor.speed = angle;
	
	//根据速度求每秒钟的脉冲数 = 每秒钟的角度 / 单脉冲角度
	float pulsenum = angle / stepmotor.pulseangle + 0.5;
	if(pulsenum < 0.16)
	{
		//如果每秒钟脉冲数小于0.16个，则设置速度为每秒0.16个脉冲
		TIM4->PSC = 7200 - 1;
		TIM_SetAutoreload(TIM4, 65535);
		TIM_SetCompare4(TIM4, 32768);		
	}
	else if(pulsenum < 16)
	{
		//如果脉冲给的速度比 65.536ms 慢,那么我们可以重新修改分频值
		TIM4->PSC = 7200 - 1;
		TIM_SetAutoreload(TIM4, 10000.0f/pulsenum);
		TIM_SetCompare4(TIM4, 10000.0f/pulsenum/2);
	}
	else
	{
		//在1us一次的计数速率下，1s内的脉冲个数最快1us一次，最慢65.536ms一次
		TIM4->PSC = 72 - 1;//72分频，1us计数一下
		TIM_SetAutoreload(TIM4, 1000000.0f/pulsenum);
		TIM_SetCompare4(TIM4, 1000000.0f/pulsenum/2);	
	}
	
}

//中断服务函数
//执行步进电机的旋转和停止
void TIM4_IRQHandler (void)
{
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	if (STEPDIRGET())
		stepmotor.angle += stepmotor.pulseangle;
	else
		stepmotor.angle -= stepmotor.pulseangle;
	
	//检测当前角度与目标角度差值，误差在一个±步进角度之间则表示角度一致，关闭定时器
	float difvalue = stepmotor.angle - stepmotor.targetangel;
	if(difvalue < -stepmotor.pulseangle || difvalue > stepmotor.pulseangle)
		TIM_Cmd(TIM4, ENABLE);
	else
		TIM_Cmd(TIM4, DISABLE);
}

#endif



