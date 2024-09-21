#include "max7219.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "my_spi.h"
#include "delay.h"

extern SPI_HandleTypeDef hspi1;

static void MAX7219_Write(uint8_t opcode, uint8_t data)
{
    uint16_t w = ((uint16_t)opcode) << 8;
    w |= data;
    SPI_WriteWord(w);
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

void MAX7219_DisplayText(char *text, int mode)
{
    char trimStr[16] = {0};
    int  decimal[16] = {0};

    int len = strlen(text);
    if (len > 16)
        len = 16;
    
    int i = 0, j = 0;
    for (; i < len; i++) {
        if (text[i] == '.') {
            if (j > 1) {
                decimal[j-1] = 1;
            } else {
                decimal[0] = 1;
            }
        } else {
            trimStr[j] = text[i];
            j++;
        }
    }

    if (j > 8)
        j = 8;
    switch (mode) {
        case 0:
            for (i = 0; i < j; i++) {
                MAX7219_DisplayChar(j-i, trimStr[i], decimal[i]);
            }
            break;
        case 1:
            for (i = 0; i < j; i++) {
                MAX7219_DisplayChar(8-i, trimStr[i], decimal[i]);
            }
            break;
        default:
            break;
    }
    
}

int MAX7219_printf(const char *fmt, ...)
{
    va_list vp;
    char buf[16] = {0};
    int fmtLen = 0;

    va_start(vp, fmt);
    fmtLen = vsnprintf(buf, sizeof(buf), fmt, vp);
    va_end(vp);

    MAX7219_DisplayText(buf, 0);
    
    return fmtLen;
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