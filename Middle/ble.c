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
	HAL_NVIC_SetPriority(UART7_IRQn, 0, 3);
	HAL_NVIC_EnableIRQ(UART7_IRQn);
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
	BleUartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT; 
	BleUartHandle.AdvancedInit.OverrunDisable=UART_ADVFEATURE_OVERRUN_DISABLE;//关闭OverRun
	
	if(HAL_UART_DeInit(&BleUartHandle) != HAL_OK)
	{
		Error_Handler();
	}  
	if(HAL_UART_Init(&BleUartHandle) != HAL_OK)
	{
		Error_Handler();
	}	
	__HAL_UART_ENABLE_IT(&BleUartHandle, UART_IT_RXNE);
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

uint8_t BLE_CheckState=0;
#define BLE_CheckTrue	250
#define BLE_CheckFalse 255
//************************************************
//函数名称：void BLE_Check(void)
//函数功能：检测蓝牙模块，若无蓝牙模块，会一直检测
//函数参数：无
//函数返回值：无
//************************************************
void BLE_Check(void)
{
		uint8_t AT[4]={'A','T'};
		BLE_CheckState=BLE_CheckFalse;
		while(BLE_CheckState==BLE_CheckFalse)
		{
			BLE_send(AT,2);HAL_Delay(20);
		}	
}
//************************************************
//函数名称：void BLE_Reset(void)
//函数功能：重启蓝牙模块，若无蓝牙模块，会一直发此命令 并在500ms后重启
//函数参数：无
//函数返回值：无
//***************************************************
void BLE_Reset(void)
{
		uint8_t AT[8]={'A','T','+','R','E','S','E','T'};
		BLE_CheckState=BLE_CheckFalse;
		while(BLE_CheckState==BLE_CheckFalse)
		{
			BLE_send(AT,8);HAL_Delay(20);
		}	
}
//************************************************
//函数名称：void BLE_BaudSet(uint32_t BaudRate)
//函数功能：设定蓝牙模块波特率
//函数参数：BaudRate：1200 2400 4800 9600	19200 38200 57600	115200 230400
//函数返回值：无
//注意事项：设定波特率前必须使蓝牙波特率和单片机相同，然后设定，设定后断电，再使用新波特率
//***************************************************
void BLE_BaudSet(uint32_t BaudRate)
{
		uint8_t AT[3]={'A','T','+'};
		BLE_CheckState=BLE_CheckFalse;
		while(BLE_CheckState==BLE_CheckFalse)
		{
			BLE_send(AT,3);
			switch(BaudRate)
			{
				case 1200:BLE_send((uint8_t *)("BAUD7"),5);break;
				case 2400:BLE_send((uint8_t *)"BAUD6",5);break;
				case 4800:BLE_send((uint8_t *)"BAUD5",5);break;
				case 9600:BLE_send((uint8_t *)"BAUD0",5);break;
				case 19200:BLE_send((uint8_t *)"BAUD1",5);break;
				case 38400:BLE_send((uint8_t *)"BAUD2",5);break;
				case 57600:BLE_send((uint8_t *)"BAUD3",5);break;
				case 115200:BLE_send((uint8_t *)"BAUD4",5);break;
				case 230400:BLE_send((uint8_t *)"BAUD8",5);break;
			}
			HAL_Delay(20);
		}	
}
//************************************************
//函数名称：void BLE_PassSet(uint8_t* PassWord)
//函数功能：设定蓝牙模块密码
//函数参数：PassWord字符串，6位密码
//函数返回值：无
//注意事项：设定波特率前必须使蓝牙波特率和单片机相同，然后设定，设定后断电，再使用新波特率
//***************************************************
void BLE_PassSet(void* PassWord)
{
		uint8_t AT[7]="AT+PASS";
		uint8_t* Get=(uint8_t*)PassWord;
		BLE_CheckState=BLE_CheckFalse;
		while(BLE_CheckState==BLE_CheckFalse)
		{
			BLE_send(AT,7);BLE_send(Get,6);
			HAL_Delay(20);
		}	
}
//解析受到的数据命令  返回OK即表示数据设定完成
//************************************************
//函数名称：void BLE_ReCheck(uint8_t BLEData)
//函数功能：检测蓝牙模块，若无蓝牙模块，会一直检测
//函数参数：BLEData 中断接受到的数据解析
//函数返回值：无
//***************************************************
void BLE_ReCheck(uint8_t BLEData)
{
	static uint8_t CommonNum=0;
	if(BLE_CheckState==BLE_CheckFalse)
	{
		switch(CommonNum)
		{
			case 0:if(BLEData=='O') CommonNum=1;else CommonNum=0;break;
			case 1:if(BLEData=='K') {CommonNum=0;BLE_CheckState=BLE_CheckTrue;}break;
		}
	}
		
}
