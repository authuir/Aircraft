/**
  ******************************************************************************
  * @file    UART/UART_TwoBoards_ComDMA/Src/stm32f7xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    25-June-2015
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_it.h"
#include "stm32f7xx_hal.h"
#include "px4.h"
#include  "GPS.h"
#include "ble.h"
#include "wifi.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* UART handler declared in "main.c" file */
extern UART_HandleTypeDef BleUartHandle;
extern UART_HandleTypeDef WifiUartHandle;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M7 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	extern int mscnt;
	HAL_IncTick();
	mscnt++;
}


/**
  * @brief  This function handles DMA interrupt request.  
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA  
  *         used for USART data transmission     
  */
void DMA1_Stream3_IRQHandler(void)
{
	HAL_DMA_IRQHandler(BleUartHandle.hdmarx);
}


void DMA2_Stream2_IRQHandler(void)
{
	//HAL_DMA_IRQHandler(PX4_UartHandle.hdmarx);
}


//光流驱动中断
void USART1_IRQHandler(void)
{
	uint8_t ReceiveData=0;
	ReceiveData=(uint8_t)((&PX4_UartHandle)->Instance->RDR);//自配,直接取地址  //数据接受
	HAL_UART_IRQHandler(&PX4_UartHandle);
	Flow_Mavlink(ReceiveData);
}
//GPS驱动中断
void UART4_IRQHandler(void)
{
	uint8_t ReceiveData=0;
	ReceiveData=(uint8_t)((&GPS_UartHandle)->Instance->RDR);//自配,直接取地址 //数据接受
	HAL_UART_IRQHandler(&GPS_UartHandle);
	//GPS_InputDate(&ReceiveData,1);
	GPS_Resolver(ReceiveData);
}
//BLE蓝牙中断
void UART7_IRQHandler(void)
{
	uint8_t ReceiveData=0;
	ReceiveData=(uint8_t)((&BleUartHandle)->Instance->RDR);//自配,直接取地址  //数据接受
	HAL_UART_IRQHandler(&BleUartHandle);
	BLE_ReCheck(ReceiveData);
}
//WIFI中断
uint8_t read[100],read_num=0;
void USART2_IRQHandler(void)
{
	uint8_t ReceiveData=0;
	ReceiveData=(uint8_t)((&WifiUartHandle)->Instance->RDR);//自配,直接取地址  //数据接受
	HAL_UART_IRQHandler(&WifiUartHandle);
	WifiSendCom_Check(ReceiveData);
	read[read_num++]=ReceiveData;
}
/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA  
  *         used for USART data reception    
  */
void DMA1_Stream1_IRQHandler(void)
{
//	HAL_DMA_IRQHandler(BleUartHandle.hdmatx);
}

/**
  * @brief  This function handles external lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  //HAL_GPIO_EXTI_IRQHandler(KEY_BUTTON_PIN);
	return ;
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
