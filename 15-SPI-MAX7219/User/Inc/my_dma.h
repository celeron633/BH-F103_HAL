#ifndef __MY_DMA_H__
#define __MY_DMA_H__

#include "main.h"
#include "stm32f1xx_hal_uart.h"
#include "stm32f1xx_hal_dma.h"

HAL_StatusTypeDef InitUART1DMA(void);
HAL_StatusTypeDef InitUART1RxDMA(void);
HAL_StatusTypeDef InitUART1TxDMA(void);

#endif