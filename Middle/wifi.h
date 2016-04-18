#ifndef __WIFI_H
#define __WIFI_H
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"

#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

/* Size of Trasmission buffer */
#define TXBUFFERSIZE                      (COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
#define RXBUFFERSIZE                      TXBUFFERSIZE


#define USARTx_TX_DMA_STREAM              DMA1_Stream4
#define USARTx_RX_DMA_STREAM              DMA1_Stream2
#define USARTx_TX_DMA_CHANNEL             DMA_CHANNEL_4
#define USARTx_RX_DMA_CHANNEL             DMA_CHANNEL_4

/* Definition for USARTx's NVIC */
#define USARTx_DMA_TX_IRQn                DMA1_Stream4_IRQn
#define USARTx_DMA_RX_IRQn                DMA1_Stream2_IRQn
#define USARTx_DMA_TX_IRQHandler          DMA1_Stream4_IRQHandler
#define USARTx_DMA_RX_IRQHandler          DMA1_Stream2_IRQHandler

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      UART4_IRQn
#define USARTx_IRQHandler                UART4_IRQHandler


void Init_WIFI(void);
void Init_WIFI_test(void);
void WifiSendCom_Start(void);
void WifiSendCom_Check(uint8_t InputData);
void WifiSendCom_Reset();
HAL_StatusTypeDef Wifi_send(uint8_t * TxBuffer, uint32_t TxLen);
#endif
