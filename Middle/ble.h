#ifndef __BLE_H
#define __BLE_H
#include <stdint.h>
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"

/* Size of Trasmission buffer */
#define BLE_TXBUFFERSIZE                      256
/* Size of Reception buffer */
#define BLE_RXBUFFERSIZE                      256

void Init_BLE(void);
void BLE_Check(void);
void BLE_ReCheck(uint8_t BLEData);
void BLE_BaudSet(uint32_t BaudRate);
void BLE_PassSet(void* PassWord);
void BLE_Reset(void);
extern uint8_t BLE_CheckState;//蓝牙检验状态，任何命令受到OK即可确定正确
HAL_StatusTypeDef BLE_send(uint8_t * TxBuffer, uint32_t TxLen);
HAL_StatusTypeDef BLE_send_char(uint8_t TxBuffer);

HAL_StatusTypeDef BLE_receive(uint8_t * RxBuffer, uint32_t RxLen);

#endif
