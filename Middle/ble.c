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
  * @brief ������
  * @param None
  * @retval None
  */
void Error_Handler(void)
{
	return ;
	//while(1);
}

/**
  * @brief 	BLE��ʼ��
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
	BleUartHandle.AdvancedInit.OverrunDisable=UART_ADVFEATURE_OVERRUN_DISABLE;//�ر�OverRun
	
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
//�������ƣ�void BLE_Check(void)
//�������ܣ��������ģ�飬��������ģ�飬��һֱ���
//������������
//��������ֵ����
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
//�������ƣ�void BLE_Reset(void)
//�������ܣ���������ģ�飬��������ģ�飬��һֱ�������� ����500ms������
//������������
//��������ֵ����
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
//�������ƣ�void BLE_BaudSet(uint32_t BaudRate)
//�������ܣ��趨����ģ�鲨����
//����������BaudRate��1200 2400 4800 9600	19200 38200 57600	115200 230400
//��������ֵ����
//ע������趨������ǰ����ʹ���������ʺ͵�Ƭ����ͬ��Ȼ���趨���趨��ϵ磬��ʹ���²�����
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
//�������ƣ�void BLE_PassSet(uint8_t* PassWord)
//�������ܣ��趨����ģ������
//����������PassWord�ַ�����6λ����
//��������ֵ����
//ע������趨������ǰ����ʹ���������ʺ͵�Ƭ����ͬ��Ȼ���趨���趨��ϵ磬��ʹ���²�����
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
//�����ܵ�����������  ����OK����ʾ�����趨���
//************************************************
//�������ƣ�void BLE_ReCheck(uint8_t BLEData)
//�������ܣ��������ģ�飬��������ģ�飬��һֱ���
//����������BLEData �жϽ��ܵ������ݽ���
//��������ֵ����
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
