#include "oled.h"
#include "font.h"

#include <stdio.h>

#define OLED_PAGES  8                   // OLED页数
#define OLED_ROW    8 * OLED_PAGES      // OLED行数 8页每页8行
#define OLED_COLUMN 128                 // OLED列数

uint8_t OLED_GRAM[OLED_PAGES][OLED_COLUMN];

struct OLED_Config g_oledCfg;

// 字体设置
const ASCIIFont *currFont = &afont16x8;

void OLED_ConfigDisplay(I2C_HandleTypeDef *handle, uint8_t i2cAddr)
{
    g_oledCfg.i2cHandle = handle;
    g_oledCfg.i2cAddr = i2cAddr;
}

static int OLED_WriteCmd(uint8_t cmd)
{
    uint8_t buf[2];
    buf[0] = 0x00;
    buf[1] = cmd;
    return HAL_I2C_Master_Transmit(g_oledCfg.i2cHandle, g_oledCfg.i2cAddr, buf, sizeof(buf), HAL_MAX_DELAY);
}

static int OLED_WriteData(uint8_t *data, size_t len)
{
    uint8_t buf[256] = {0};
    buf[0] = 0x40;
    if (len > 128) {
        len = 128;
    }
    memcpy(buf+1, data, len);

    // len+1包含了0x40作为数据开头
    return HAL_I2C_Master_Transmit(g_oledCfg.i2cHandle, g_oledCfg.i2cAddr, buf, len+1, HAL_MAX_DELAY);
}

int OLED_InitDisplay()
{
    // 等待OLED初始化
    HAL_Delay(20);

    if (g_oledCfg.i2cHandle->State != HAL_I2C_STATE_READY) {
        return -1;
    }

    /*写入一系列的命令，对OLED进行初始化配置*/
    OLED_WriteCmd(0xAE);	//设置显示开启/关闭，0xAE关闭，0xAF开启
        
    OLED_WriteCmd(0xD5);	//设置显示时钟分频比/振荡器频率
    OLED_WriteCmd(0x80);	//0x00~0xFF
        
    OLED_WriteCmd(0xA8);	//设置多路复用率
    OLED_WriteCmd(0x3F);	//0x0E~0x3F
        
    OLED_WriteCmd(0xD3);	//设置显示偏移
    OLED_WriteCmd(0x00);	//0x00~0x7F
        
    OLED_WriteCmd(0x40);	//设置显示开始行，0x40~0x7F
        
    OLED_WriteCmd(0xA1);	//设置左右方向，0xA1正常，0xA0左右反置
        
    OLED_WriteCmd(0xC8);	//设置上下方向，0xC8正常，0xC0上下反置
    OLED_WriteCmd(0xDA);	//设置COM引脚硬件配置
    OLED_WriteCmd(0x12);
        
    OLED_WriteCmd(0x81);	//设置对比度
    OLED_WriteCmd(0xCF);	//0x00~0xFF
    OLED_WriteCmd(0xD9);	//设置预充电周期
    OLED_WriteCmd(0xF1);
    OLED_WriteCmd(0xDB);	//设置VCOMH取消选择级别
    OLED_WriteCmd(0x30);
    OLED_WriteCmd(0xA4);	//设置整个显示打开/关闭
    OLED_WriteCmd(0xA6);	//设置正常/反色显示，0xA6正常，0xA7反色
    OLED_WriteCmd(0x8D);	//设置充电泵
    OLED_WriteCmd(0x14);
    OLED_WriteCmd(0xAF);	//开启显示

    return 0;
}

void OLED_SetPos(uint8_t X, uint8_t Y)
{
    if (X >= OLED_COLUMN || Y >= OLED_ROW) {
        return;
    }
    uint8_t pg = Y / OLED_PAGES;

    // b0~b7: 设置页地址
    OLED_WriteCmd(0xb0 + pg);

   // 设置列地址
   // 低4位
   OLED_WriteCmd((X & 0x0f));
   // 高4位
   OLED_WriteCmd(((X & 0xf0) >> 4) | 0x10);
}

void OLED_SetCursor(uint8_t page, uint8_t X)
{
#ifdef OLED_1INCH3
    X += 2;
#endif

    // b0~b7: 设置页地址
    OLED_WriteCmd(0xb0 + page);

   // 设置列地址
   // 低4位
   OLED_WriteCmd((X & 0x0f));
   // 高4位
   OLED_WriteCmd(((X & 0xf0) >> 4) | 0x10);
}

void OLED_Fill(uint8_t data)
{   
    memset(OLED_GRAM, data, sizeof(OLED_GRAM));
}

void OLED_NewFrame()
{
    OLED_Fill(0x00);
}

// 逻辑参考SetPixel
void OLED_ClearArea(int16_t X, int16_t Y, uint8_t width, uint8_t height)
{
   // 这个是像素级别操作, 和DrawImage不同
   for (int16_t i = Y; i < height; i++) {
        for (int16_t j = X; j < width; j++) {
            if (i >= 0 && i < OLED_ROW && j >= 0 && j < OLED_COLUMN) {
                // 这里只会清零XY对应的那个像素
                OLED_GRAM[i / 8][j] &= ~(0x01 << (i % 8));
            }
        }
   }
}

void OLED_SetPixel(int X, int Y)
{
    // 越界
    if (X >= OLED_COLUMN || Y >= OLED_ROW) {
        return;
    }

    // 第一维是所在页数, 第二维为列数
    OLED_GRAM[Y/8][X] |= 0x01 << (Y % 8);
}

// 刷新显存数据到屏幕
void OLED_ShowFrame()
{
    for (int i = 0; i < OLED_PAGES; i++) {
        OLED_SetCursor(i, 0);
        OLED_WriteData(OLED_GRAM[i], OLED_COLUMN);
    }
}

// image需要按列行式进行存储
void OLED_ShowImage(int16_t X, int16_t Y, uint8_t width, uint8_t height, const uint8_t *image)
{
    // 先清空那片区域
    OLED_ClearArea(X, Y, width, height);

    // 画的内容占多少页面（向上取整）
    uint8_t pages = ((height - 1) / 8) + 1;
    // 开始在哪页画
    uint8_t startPage = Y / 8;

    uint8_t shift = Y % 8;

    // 处理每一页
    for (uint8_t i = 0; i < pages; i++) {
        // 处理每一列
        for (uint8_t j = 0; j < width; j++) {
            // 只画范围内
            if (X + j >= 0 && X + j < OLED_COLUMN) {
                if (startPage + i >= 0 && startPage + i < OLED_ROW) {
                    // 低位往高位移动
                    OLED_GRAM[startPage + i][X + j] |= (image[i * width + j] << shift);
                }
                // 当前页有跨到下一页的处理
                if (startPage + i + 1 >= 0 && startPage + i + 1 < OLED_ROW) {
                    // 高位往低位移动
                    OLED_GRAM[startPage + i + 1][X + j] |= (image[i * width + j] >> (8 - shift));
                }
            }
        }
    }
}

void OLED_ShowChar(uint8_t X, uint8_t Y, char c)
{
    uint8_t chrIndex = c - ' ';
    uint32_t arrayIndex = (((currFont->h) + 7) / 8) * currFont->w;

    // printf("chrIndex: %u, arrayIndex: %lu\r\n", chrIndex, arrayIndex);
    OLED_ShowImage(X, Y, currFont->w, currFont->h, currFont->chars + chrIndex * arrayIndex);
}

void OLED_ShowString(uint8_t X, uint8_t Y, const char *str)
{
    while (*str) {
        OLED_ShowChar(X, Y, *str);
        X += currFont->w;
        str++;
    }
}

void OLED_DrawRectangle(int16_t X, int16_t Y, uint8_t width, uint8_t height)
{
    for (uint8_t i = 0; i < height; i++) {
        for (uint8_t j = 0; j < width; j++) {
            OLED_SetPixel(X+j, Y+i);
        }
    }
}

void OLED_Test()
{
    OLED_NewFrame();
    OLED_ShowFrame();
    
    /*
    int i = 0;
    for (;;) {
        if (i > 64-currFont->h) {
            i = 0;
        }
        OLED_NewFrame();
        OLED_ShowString(0, i, "Hello World");
        // OLED_ShowChar(0, 0, 'A');
        OLED_ShowFrame();
        HAL_Delay(100);
        i+=3;
    }
    */
    // OLED_ShowChar(0, 0, 'A');
    // OLED_ShowChar(0, 0, 'B');
    int i = 0;
    for (;;) {
        if (i >= 127) {
            i = 0;
        }
        i+=3;
        OLED_NewFrame();
        OLED_DrawRectangle(0, 0, i, 16);
        OLED_ShowFrame();
        HAL_Delay(100);
    }
    

}