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
	__GPIOD_CLK_ENABLE();//打开端口时钟
	/* Enable USARTx clock */
	__USART2_CLK_ENABLE();
  
	/*##-2- Configure peripheral GPIO ##########################################*/  
	/* UART TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = GPIO_PIN_5|GPIO_PIN_6;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    
	/*##-3- Configure the NVIC for UART ########################################*/
	/* NVIC for USART */
		HAL_NVIC_SetPriority(USART2_IRQn, 0, 2);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
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
#define nReload_ON		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
#define nReload_OFF		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);
#define RST_ON		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
#define RST_OFF		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);

#define nReady_Read HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_0)
#define nLink_Read	HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8)
uint32_t readpin;
void WifiGPIO_PortInit(void)
{

		GPIO_InitTypeDef  GPIO_InitStruct;
  
	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO TX/RX clock */
	__HAL_RCC_GPIOE_CLK_ENABLE();__HAL_RCC_GPIOB_CLK_ENABLE();
  
	/*##-2- Configure peripheral GPIO ##########################################*/
	GPIO_InitStruct.Pin       = GPIO_PIN_1;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);//nReload
	GPIO_InitStruct.Pin       = GPIO_PIN_0;
	GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);//nReady
	GPIO_InitStruct.Pin       = GPIO_PIN_9;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);//RST
	GPIO_InitStruct.Pin       = GPIO_PIN_8;
	GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);//nLink
	if(nReady_Read!=0)//表示此时准备已经完成
	{
		RST_OFF;HAL_Delay(300);RST_ON;
		while(nReady_Read!=0){}
	}
	
}

/**
  * @brief 错误处理
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
	__USART2_CLK_ENABLE();
	WifiUartHandle.Instance        = USART2;
	WifiUartHandle.Init.BaudRate   = 57600;
	WifiUartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	WifiUartHandle.Init.StopBits   = UART_STOPBITS_1;
	WifiUartHandle.Init.Parity     = UART_PARITY_NONE;
	WifiUartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	WifiUartHandle.Init.Mode       = UART_MODE_TX_RX;
	WifiUartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT; 
	WifiUartHandle.AdvancedInit.OverrunDisable=UART_ADVFEATURE_OVERRUN_DISABLE;//关闭OverRun
	
	if(HAL_UART_DeInit(&WifiUartHandle) != HAL_OK)
	{
		Errors_Handler();
	}  
	if(HAL_UART_Init(&WifiUartHandle) != HAL_OK)
	{
		Errors_Handler();
	}	
	  /*##-4- Put UART peripheral in reception process ###########################*/ 
    /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_ENABLE_IT(&WifiUartHandle, UART_IT_RXNE);
	WifiGPIO_PortInit();
}
HAL_StatusTypeDef Wifi_send(uint8_t * TxBuffer, uint32_t TxLen)
{
	if(HAL_UART_Transmit(&WifiUartHandle, (uint8_t*)TxBuffer, (uint32_t)TxLen, 5000)!= HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

#define A_State_True	250
#define A_State_Flase	254
uint8_t A_State_Flag=0;

#define OK_State_True 250
#define OK_State_Flase 254
uint8_t OK_State_Flag=0;
void WifiSendCom_Start(void)
{
	uint8_t wifiCom[]={'+','+','+'};
	uint8_t wifiCheck[1]={'a'};
	OK_State_Flag=OK_State_Flase;
	A_State_Flag=A_State_Flase;
	Wifi_send(wifiCom,3);
	while(A_State_Flag!=A_State_True)//没接受成功，一直发送
	{}
	OK_State_Flag=OK_State_Flase;
	Wifi_send(wifiCheck,1);
	while(OK_State_Flag!=OK_State_True){}
	OK_State_Flag=0;A_State_Flag=0;
}
void WifiSendCom_Check(uint8_t InputData)
{
	static uint8_t OK_Num=0;
	if(A_State_Flag==A_State_Flase)
	{
		if(InputData=='a') A_State_Flag=A_State_True;
	}
	if(OK_State_Flag==OK_State_Flase)
	{
		switch(OK_Num)
		{
			case 0:if(InputData=='o') OK_Num=1;else OK_Num=0;break;
			case 1:if(InputData=='k') {OK_Num=0;OK_State_Flag=OK_State_True;} else OK_Num=0;break;
		}
	}
}
void WifiSendCom_Reset()
{
	RST_OFF;HAL_Delay(300);RST_ON;
	while(nReady_Read!=0){}
}
