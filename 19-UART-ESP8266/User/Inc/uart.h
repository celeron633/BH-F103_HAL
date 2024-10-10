#ifndef __UART_H__
#define __UART_H__

#if 0

#include "main.h"
#include "stm32f1xx_hal_uart.h"

HAL_StatusTypeDef InitBoardUART(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);

#endif

#endif