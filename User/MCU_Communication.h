#ifndef __MCU_COMMUNICATION_H__
#define __MCU_COMMUNICATION_H__

void MCUPortInit(void);
void MCU_UART_MspInit(UART_HandleTypeDef *huart);
void MCU_CalibrationSend();
HAL_StatusTypeDef MCU_send(uint8_t * TxBuffer, uint32_t TxLen);
void MCU_AccordSend(float X,float Y,float Z,float T);

void MCU_Open(void);
void MCU_Close(void);

#endif
