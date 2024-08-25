#include "my_dma.h"

DMA_HandleTypeDef uart1RxDMAHandle;
DMA_HandleTypeDef uart1TxDMAHandle;

HAL_StatusTypeDef InitUART1DMA(void)
{
    HAL_StatusTypeDef rxRet = InitUART1RxDMA();
    HAL_StatusTypeDef txRet = InitUART1TxDMA();

    if (rxRet == HAL_ERROR || txRet == HAL_ERROR) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef InitUART1RxDMA(void)
{
    DMA_InitTypeDef rxDMAInitType;
    rxDMAInitType.Direction = DMA_PERIPH_TO_MEMORY;
    rxDMAInitType.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    rxDMAInitType.MemInc = DMA_MINC_ENABLE;
    rxDMAInitType.Mode = DMA_NORMAL;
    rxDMAInitType.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    rxDMAInitType.PeriphInc = DMA_PINC_DISABLE;
    rxDMAInitType.Priority = DMA_PRIORITY_LOW;

    uart1RxDMAHandle.ChannelIndex = 5;
    uart1RxDMAHandle.DmaBaseAddress = DMA1;
    uart1RxDMAHandle.Init = rxDMAInitType;
    uart1RxDMAHandle.Instance = DMA1_Channel5;
    return HAL_DMA_Init(&uart1RxDMAHandle);
}

HAL_StatusTypeDef InitUART1TxDMA(void)
{
    DMA_InitTypeDef txDMAInitType;
    txDMAInitType.Direction = DMA_MEMORY_TO_PERIPH;
    txDMAInitType.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    txDMAInitType.MemInc = DMA_MINC_ENABLE;
    txDMAInitType.Mode = DMA_NORMAL;
    txDMAInitType.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    txDMAInitType.PeriphInc = DMA_PINC_DISABLE;
    txDMAInitType.Priority = DMA_PRIORITY_LOW;

    uart1TxDMAHandle.ChannelIndex = 4;
    uart1TxDMAHandle.DmaBaseAddress = DMA1;
    uart1TxDMAHandle.Init = txDMAInitType;
    uart1TxDMAHandle.Instance = DMA1_Channel5;
    return HAL_DMA_Init(&uart1TxDMAHandle);
}