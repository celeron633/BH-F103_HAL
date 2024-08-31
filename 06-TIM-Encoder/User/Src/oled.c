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

    OLED_WriteCmd(0xAE); // 屏幕关
	OLED_WriteCmd(0x20); // 设置寻址模式
	OLED_WriteCmd(0x10); /* 
                        00,Horizontal Addressing Mode;
		    		    01,Vertical Addressing Mode;
		    		    10,Page    Addressing Mode (RESET);
		    		    11,Invalid 
                        */
	OLED_WriteCmd(0xb0); // Set Page Start Address for Page Addressing Mode,0-7
	OLED_WriteCmd(0xc8); // COM扫描反向
	OLED_WriteCmd(0x00); // 起始列地址低位
	OLED_WriteCmd(0x10); // 起始列地址高位
	OLED_WriteCmd(0x40); // set start line address
	OLED_WriteCmd(0x81); // 设置对比度
	OLED_WriteCmd(0xff); // 亮度调节 范围0x00~0xff
	OLED_WriteCmd(0xa1); // set segment re-map 0 to 127
	OLED_WriteCmd(0xa6); // 正常显示
	
	OLED_WriteCmd(0xa8); // 设置MUX数(1 to 64)
	OLED_WriteCmd(0x3F); // MUX=63 显示63行
	
	OLED_WriteCmd(0xa4); // 0xa4,输出 RAM 内容;0xa5,忽略 RAM 内容
	OLED_WriteCmd(0xd3); // 设置显示偏移
	OLED_WriteCmd(0x00); // 没有偏移
	
	OLED_WriteCmd(0xd5); // 设置显示时钟分频数、振荡器频率
	OLED_WriteCmd(0xf0); // set divide ratio
	OLED_WriteCmd(0xd9); // 设置预充电周期
	OLED_WriteCmd(0x22); //
	OLED_WriteCmd(0xda); // COM引脚配置
	OLED_WriteCmd(0x12); // 启用左右反置
	
	OLED_WriteCmd(0xdb); // 设置 vcomh 输出
	OLED_WriteCmd(0x20); // 0x20,0.77xVcc
	
	OLED_WriteCmd(0x8d); // set DC-DC enable
	OLED_WriteCmd(0x14); //
	OLED_WriteCmd(0xaf); // turn on oled panel

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

void OLED_Test()
{
    OLED_NewFrame();
    OLED_ShowFrame();
    
    int i = 0;
    for (;;) {
        if (i >= 50) {
            i = 0;
        }
        OLED_NewFrame();
        OLED_ShowString(0, i, "Hello World");
        // OLED_ShowChar(0, 0, 'A');
        OLED_ShowFrame();
        HAL_Delay(100);
        i+=3;
    }

    
}