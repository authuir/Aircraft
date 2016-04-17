#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "wifi.h"


/* Buffer used for transmission */
uint8_t aTxBuffer[] = "AT";

/* Buffer used for reception */
uint8_t aRxBuffer[RXBUFFERSIZE];

UART_HandleTypeDef WifiUartHandle;

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
void WIFI_UART_MspInit(UART_HandleTypeDef *huart)
{  
	GPIO_InitTypeDef  GPIO_InitStruct;
  
	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO TX/RX clock */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	/* Enable USARTx clock */
	__UART4_CLK_ENABLE();

  
	/*##-2- Configure peripheral GPIO ##########################################*/  
	/* UART TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = GPIO_PIN_5|GPIO_PIN_6;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    
	/*##-3- Configure the NVIC for UART ########################################*/
	/* NVIC for USART */
	//HAL_NVIC_SetPriority(USARTx_IRQn, 0, 1);
	//HAL_NVIC_EnableIRQ(USARTx_IRQn);
}

void WIFI_UART_MspDeInit(UART_HandleTypeDef *huart)
{

}

void GPIO_TEST()
{
	GPIO_InitTypeDef  GPIO_InitStruct;
  
	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO TX/RX clock */
	__HAL_RCC_GPIOD_CLK_ENABLE();
  
	/*##-2- Configure peripheral GPIO ##########################################*/  
	/* UART TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = GPIO_PIN_5|GPIO_PIN_6;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
//	GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
	
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_SET);
}

/**
  * @brief ¥ÌŒÛ¥¶¿Ì
  * @param None
  * @retval None
  */
void Errors_Handler(void)
{
	return ;
	//while(1);
}

void Init_WIFI(void)
{
	UART_HandleTypeDef WifiUartHandle;
	WifiUartHandle.Instance        = UART4;

	WifiUartHandle.Init.BaudRate   = 9600;
	WifiUartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	WifiUartHandle.Init.StopBits   = UART_STOPBITS_1;
	WifiUartHandle.Init.Parity     = UART_PARITY_NONE;
	WifiUartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	WifiUartHandle.Init.Mode       = UART_MODE_TX_RX;
	WifiUartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT; 
	
	if(HAL_UART_DeInit(&WifiUartHandle) != HAL_OK)
	{
		Errors_Handler();
	}  
	if(HAL_UART_Init(&WifiUartHandle) != HAL_OK)
	{
		Errors_Handler();
	}	
	/*while (1)
	{
		if(HAL_UART_Transmit(&WifiUartHandle, (uint8_t*)aTxBuffer, TXBUFFERSIZE, 5000)!= HAL_OK)
		{
			Errors_Handler();
		}
		if(HAL_UART_Receive(&WifiUartHandle, (uint8_t *)aRxBuffer, RXBUFFERSIZE, 5000) != HAL_OK)
		{
			Errors_Handler();  
		} else if(HAL_UART_Receive(&WifiUartHandle, (uint8_t *)aRxBuffer, RXBUFFERSIZE, 5000) == HAL_OK){
			Errors_Handler();  
		}
	}*/
}

