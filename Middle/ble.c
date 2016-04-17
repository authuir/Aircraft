#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "ble.h"

/* Buffer used for transmission */
uint8_t aaTxBuffer[BLE_TXBUFFERSIZE] = " ****UART_TwoBoards_ComIT****  ****UART_TwoBoards_ComIT****  ****UART_TwoBoards_ComIT**** ";

/* Buffer used for reception */
uint8_t aaRxBuffer[BLE_RXBUFFERSIZE];

UART_HandleTypeDef BleUartHandle;


/**
  * @brief UART MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - DMA configuration for transmission request by peripheral 
  *           - NVIC configuration for DMA interrupt request enable
  * @param huart: UART handle pointer
  * @retval None
  */
void BLE_UART_MspInit(UART_HandleTypeDef *huart)
{  
	GPIO_InitTypeDef  GPIO_InitStruct;
  
	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO TX/RX clock */
	__HAL_RCC_GPIOE_CLK_ENABLE();
	/* Enable USARTx clock */
	__UART7_CLK_ENABLE();

  
	/*##-2- Configure peripheral GPIO ##########################################*/  
	/* UART TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = GPIO_PIN_7|GPIO_PIN_8;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF8_UART7;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    
	/*##-3- Configure the NVIC for UART ########################################*/
	/* NVIC for USART */
	//HAL_NVIC_SetPriority(UART7_IRQn, 0, 1);
	//HAL_NVIC_EnableIRQ(UART7_IRQn);
}

/**
  * @brief UART MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - DMA configuration for transmission request by peripheral 
  *           - NVIC configuration for DMA interrupt request enable
  * @param huart: UART handle pointer
  * @retval None
  */
void BLE_UART_MspDeInit(UART_HandleTypeDef *huart)
{
	
}

/**
  * @brief 错误处理
  * @param None
  * @retval None
  */
void Error_Handler(void)
{
	return ;
	//while(1);
}

/**
  * @brief 	BLE初始化
  * @param 	None
  * @retval None
  */
void Init_BLE(void)
{
	BleUartHandle.Instance        = UART7;

	BleUartHandle.Init.BaudRate   = 115200;
	BleUartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	BleUartHandle.Init.StopBits   = UART_STOPBITS_1;
	BleUartHandle.Init.Parity     = UART_PARITY_NONE;
	BleUartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	BleUartHandle.Init.Mode       = UART_MODE_TX_RX;
	BleUartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	
	if(HAL_UART_DeInit(&BleUartHandle) != HAL_OK)
	{
		Error_Handler();
	}  
	if(HAL_UART_Init(&BleUartHandle) != HAL_OK)
	{
		Error_Handler();
	}	
/*	while (1)
	{
		if(HAL_UART_Transmit(&BleUartHandle, (uint8_t*)aaTxBuffer, BLE_TXBUFFERSIZE, 5000)!= HAL_OK)
		{
			Error_Handler();
		}
		if(HAL_UART_Receive(&BleUartHandle, (uint8_t *)aaRxBuffer, BLE_TXBUFFERSIZE, 5000) != HAL_OK)
		{
			Error_Handler();
		}
	}*/
}

HAL_StatusTypeDef BLE_send(uint8_t * TxBuffer, uint32_t TxLen)
{
	if(HAL_UART_Transmit(&BleUartHandle, (uint8_t*)TxBuffer, (uint32_t)TxLen, 5000)!= HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef BLE_receive(uint8_t * RxBuffer, uint32_t RxLen)
{
	if(HAL_UART_Receive(&BleUartHandle, (uint8_t*)RxBuffer, (uint32_t)RxLen, 5000)!= HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef BLE_send_char(uint8_t TxBuffer)
{
	if(HAL_UART_Transmit(&BleUartHandle, &TxBuffer, 1, 5000)!= HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}
