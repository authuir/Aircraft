#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <stdio.h>
#include <stdbool.h>
#include "stm32f7xx_hal.h"
#include "stm32f7xx.h"
#include "stm32f7xx_hal_gpio.h"

#define LED_ALL_ON HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2,GPIO_PIN_RESET)
#define LED_ALL_OFF HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2,GPIO_PIN_SET)

#define LED_00_ON HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET)
#define LED_00_OFF HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET)

#define LED_11_ON HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET)
#define LED_11_OFF HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET)

#define LED_22_ON HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET)
#define LED_22_OFF HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET)

//void MPU_Config(void);
void SystemClock_Config(void);
void CPU_CACHE_Enable(void);

void Init_LED();



#endif
