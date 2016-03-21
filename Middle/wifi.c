#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "wifi.h"

#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

/* Size of Trasmission buffer */
#define TXBUFFERSIZE                      (COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
#define RXBUFFERSIZE                      TXBUFFERSIZE

/* Buffer used for transmission */
uint8_t aTxBuffer[] = "AT";

/* Buffer used for reception */
uint8_t aRxBuffer[RXBUFFERSIZE];

void Init_WIFI()
{
	uint32_t timekeeper;
	extern int mscnt;
	UART_HandleTypeDef UartHandle;
	GPIO_InitTypeDef WIFI_GPIO;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	/* Configure the WiFi RST pin */
	WIFI_GPIO.Pin = GPIO_PIN_9;
	WIFI_GPIO.Mode = GPIO_MODE_OUTPUT_PP;
	WIFI_GPIO.Pull = GPIO_PULLUP;
	WIFI_GPIO.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &WIFI_GPIO);

	/* 复位WIFI模块，保持500ms */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
	/*timekeeper = mscnt+500;
	while (mscnt<timekeeper)
		timekeeper = timekeeper;*/
	
	timekeeper = 0;
	while (timekeeper<1000000)
		timekeeper ++;
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
	
	/* Configure the WiFi  pin */
	WIFI_GPIO.Pin = GPIO_PIN_0;
	WIFI_GPIO.Mode = GPIO_MODE_IT_FALLING;
	WIFI_GPIO.Pull = GPIO_NOPULL;
	WIFI_GPIO.Speed = GPIO_SPEED_FAST;
	
	HAL_GPIO_Init(GPIOE, &WIFI_GPIO);
	
	/* Enable and set Button EXTI Interrupt to the lowest priority */
    HAL_NVIC_SetPriority((IRQn_Type)(EXTI15_10_IRQn), 0x0F, 0x00);
    HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI15_10_IRQn));
	
	UartHandle.Instance        = UART4;

	UartHandle.Init.BaudRate   = 9600;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits   = UART_STOPBITS_1;
	UartHandle.Init.Parity     = UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode       = UART_MODE_TX_RX;
	
	if(HAL_UART_DeInit(&UartHandle) != HAL_OK)
	{
		//Error_Handler();
	}  
	if(HAL_UART_Init(&UartHandle) != HAL_OK)
	{
		//Error_Handler();
	}
	
	if(HAL_UART_Transmit(&UartHandle, (uint8_t*)aTxBuffer, TXBUFFERSIZE, 5000)!= HAL_OK)
	{
		//Error_Handler();   
	}

	/*##-3- Put UART peripheral in reception process ###########################*/  
	if(HAL_UART_Receive(&UartHandle, (uint8_t *)aRxBuffer, RXBUFFERSIZE, 5000) != HAL_OK)
	{
		//Error_Handler();  
	}
	
	return;
}
