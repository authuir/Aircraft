#include <stdio.h>
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "system.h"

extern void BLE_UART_MspInit(UART_HandleTypeDef *huart);
extern void BLE_UART_MspDeInit(UART_HandleTypeDef *huart);

extern void WIFI_UART_MspInit(UART_HandleTypeDef *huart);
extern void WIFI_UART_MspDeInit(UART_HandleTypeDef *huart);

/**
  * @brief UART MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - NVIC configuration for UART interrupt request enable
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	if (huart->Instance == UART7)
	{
		BLE_UART_MspInit(huart);
	} else if (huart->Instance == UART4)
	{
		WIFI_UART_MspInit(huart);
	}
}

/**
  * @brief UART MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
	if (huart->Instance == UART7)
	{
		BLE_UART_MspDeInit(huart);
	} else if (huart->Instance == UART4)
	{
		WIFI_UART_MspDeInit(huart);
	}
}
