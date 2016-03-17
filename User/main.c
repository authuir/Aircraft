#include <stdio.h>
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "system.h"

extern __IO int mscnt;

int main()
{
	CPU_CACHE_Enable();
	
	//晶振没起振，可能有问题
	//SystemClock_Config();
	
	HAL_Init();
	Init_LED(true);
	
	while(1)
	{
		if (mscnt%1000>500)
			LED_00_ON;
		else
			LED_00_OFF;
		
		if (mscnt%500>250)
			LED_11_ON;
		else
			LED_11_OFF;
		
		if (mscnt%2000>1000)
			LED_22_ON;
		else
			LED_22_OFF;
	}
	
	return 0;
} 
