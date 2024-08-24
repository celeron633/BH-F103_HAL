#ifndef __UART_H__
#define __UART_H__

#include "main.h"
#include "stm32f1xx_hal_uart.h"

HAL_StatusTypeDef InitBoardUART(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif