#include "my_spi.h"
#include "delay.h"

// CPOL: 0, CPHA: 0
void SPI_WriteByte(uint8_t byte)
{
    for (int i = 0; i < 8; i++) {
        if ((byte & 0x80)) {
            HAL_GPIO_WritePin(MAX7219_DIN_GPIO_Port, MAX7219_DIN_Pin, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(MAX7219_DIN_GPIO_Port, MAX7219_DIN_Pin, GPIO_PIN_RESET);
        }

        HAL_GPIO_WritePin(MAX7219_CLK_GPIO_Port, MAX7219_CLK_Pin, GPIO_PIN_SET);
        byte <<= 1;
        HAL_GPIO_WritePin(MAX7219_CLK_GPIO_Port, MAX7219_CLK_Pin, GPIO_PIN_RESET);
    }
}

// CPOL: 0, CPHA: 0
void SPI_WriteWord(uint16_t word)
{
    // 拉低CS
    HAL_GPIO_WritePin(MAX7219_CS_GPIO_Port, MAX7219_CS_Pin, GPIO_PIN_RESET);

    for (int i = 0; i < 16; i++) {
        if ((word & 0x8000)) {
            HAL_GPIO_WritePin(MAX7219_DIN_GPIO_Port, MAX7219_DIN_Pin, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(MAX7219_DIN_GPIO_Port, MAX7219_DIN_Pin, GPIO_PIN_RESET);
        }

        HAL_GPIO_WritePin(MAX7219_CLK_GPIO_Port, MAX7219_CLK_Pin, GPIO_PIN_SET);
        word <<= 1;
        HAL_GPIO_WritePin(MAX7219_CLK_GPIO_Port, MAX7219_CLK_Pin, GPIO_PIN_RESET);
    }

    // 拉高CS
    HAL_GPIO_WritePin(MAX7219_CS_GPIO_Port, MAX7219_CS_Pin, GPIO_PIN_SET);
}

void SPI_Write(uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        SPI_WriteByte(data[i]);
    }
}