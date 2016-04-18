#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "pwm.h"

static TIM_HandleTypeDef hPwm;
static TIM_OC_InitTypeDef hPwmOC;

void PWM_Error_Handler()
{
	return;
}

void Init_PWM()
{
	GPIO_InitTypeDef  GPIO_Pwm;
	
	/* Config GPIO to PWM mode */
	__GPIOA_CLK_ENABLE();
	
	GPIO_Pwm.Pin       = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
	GPIO_Pwm.Mode      = GPIO_MODE_AF_PP;
	GPIO_Pwm.Pull      = GPIO_PULLUP;
	GPIO_Pwm.Speed     = GPIO_SPEED_HIGH;
	GPIO_Pwm.Alternate = GPIO_AF1_TIM1;
	HAL_GPIO_Init(GPIOA, &GPIO_Pwm);
	
	/* Config TIM1 to PWM mode */
	__TIM1_CLK_ENABLE();
	
	hPwm.Instance = TIM1;
	hPwm.Init.Period            = 10000 - 1;				
	hPwm.Init.Prescaler         = 40;					
	hPwm.Init.ClockDivision     = 0;					
	hPwm.Init.CounterMode       = TIM_COUNTERMODE_UP;	
	hPwm.Init.RepetitionCounter = 0;
	

	if (HAL_TIM_PWM_Init(&hPwm) != HAL_OK)
	{
		/* Initialization Error */
		PWM_Error_Handler();
	}
	
	hPwmOC.OCMode 		= TIM_OCMODE_PWM1;
	hPwmOC.Pulse		= 4000;
	hPwmOC.OCPolarity	= TIM_OCPOLARITY_HIGH;
	hPwmOC.OCFastMode	= TIM_OCFAST_DISABLE;
	hPwmOC.OCIdleState	= TIM_OCIDLESTATE_SET;
	
	hPwm.Channel = HAL_TIM_ACTIVE_CHANNEL_1;
	HAL_TIM_PWM_ConfigChannel(&hPwm, &hPwmOC, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&hPwm, TIM_CHANNEL_1);
	
	hPwm.Channel = HAL_TIM_ACTIVE_CHANNEL_2;
	HAL_TIM_PWM_ConfigChannel(&hPwm, &hPwmOC, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&hPwm, TIM_CHANNEL_2);
	
	hPwm.Channel = HAL_TIM_ACTIVE_CHANNEL_3;
	HAL_TIM_PWM_ConfigChannel(&hPwm, &hPwmOC, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&hPwm, TIM_CHANNEL_3);
	
	hPwm.Channel = HAL_TIM_ACTIVE_CHANNEL_4;
	HAL_TIM_PWM_ConfigChannel(&hPwm, &hPwmOC, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&hPwm, TIM_CHANNEL_4);
}

void PWM_CHG(int M,double Duty)
{
	hPwmOC.Pulse		= (uint32_t)9999*Duty;
	switch (M)
	{
		case M1:
			hPwm.Channel = HAL_TIM_ACTIVE_CHANNEL_1;
			HAL_TIM_PWM_ConfigChannel(&hPwm, &hPwmOC, TIM_CHANNEL_1);
			HAL_TIM_PWM_Start(&hPwm, TIM_CHANNEL_1);
			break;
		case M2:
			hPwm.Channel = HAL_TIM_ACTIVE_CHANNEL_2;
			HAL_TIM_PWM_ConfigChannel(&hPwm, &hPwmOC, TIM_CHANNEL_2);
			HAL_TIM_PWM_Start(&hPwm, TIM_CHANNEL_2);
			break;
		case M3:
			hPwm.Channel = HAL_TIM_ACTIVE_CHANNEL_3;
			HAL_TIM_PWM_ConfigChannel(&hPwm, &hPwmOC, TIM_CHANNEL_3);
			HAL_TIM_PWM_Start(&hPwm, TIM_CHANNEL_3);
			break;
		case M4:
			hPwm.Channel = HAL_TIM_ACTIVE_CHANNEL_4;
			HAL_TIM_PWM_ConfigChannel(&hPwm, &hPwmOC, TIM_CHANNEL_4);
			HAL_TIM_PWM_Start(&hPwm, TIM_CHANNEL_4);
			break;
	}
}
