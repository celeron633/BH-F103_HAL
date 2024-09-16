#include "max7219.h"

extern SPI_HandleTypeDef hspi1;

static void MAX7219_Write(uint8_t opcode, uint8_t data)
{
    // 拉低CS
    HAL_GPIO_WritePin(MAX7219_CS_GPIO_Port, MAX7219_CS_Pin, GPIO_PIN_RESET);

    // 写地址
    HAL_SPI_Transmit(&hspi1, &opcode, 1, HAL_MAX_DELAY);
    // 写数据
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);

    // 拉高CS
    HAL_GPIO_WritePin(MAX7219_CS_GPIO_Port, MAX7219_CS_Pin, GPIO_PIN_SET);
}

static unsigned char MAX7219_LookupCode(char character, unsigned int dp)
{
    int dot = 0;
    if (dp)
        dot = 1;
    
    if (character >= 35 && character <= 44) {
        character += 13;
        dot = 1;
    }

    for (int i = 0; MAX7219_Font[i].ascii; i++) {
        if (character == MAX7219_Font[i].ascii) {
            char tmpSeg = MAX7219_Font[i].segs;
            if (dot) {
                // 小数点
                tmpSeg |= (1 << 7);
            }
            return tmpSeg;
        }
    }
    return 0;
}

void MAX7219_ShutdownStop(void)
{
    MAX7219_Write(REG_SHUTDOWN, 1);
}

void MAX7219_DisplayTestStart(void)
{
    MAX7219_Write(REG_DISPLAY_TEST, 1);
}

void MAX7219_DisplayTestStop(void)
{
  MAX7219_Write(REG_DISPLAY_TEST, 0);                 
}

void MAX7219_Clear(void)
{
    for(int i = 0; i < 8; i++) {
        MAX7219_Write(i+1,0x00);
    }
}

void MAX7219_SetBrightness(uint8_t brightness)
{
    brightness &= 0x0f;
    MAX7219_Write(REG_INTENSITY, brightness);
}

void MAX7219_DisplayChar(int digit, char value, uint8_t dp)
{
    MAX7219_Write(digit, MAX7219_LookupCode(value, dp));
}

void MAX7219_Begin(void)
{
    MAX7219_Write(REG_SCAN_LIMIT, 7);
    MAX7219_Write(REG_DECODE, 0x00);
    MAX7219_ShutdownStop();
    MAX7219_DisplayTestStop();
    MAX7219_Clear();
    MAX7219_SetBrightness(INTENSITY_MAX);
}