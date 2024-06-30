#include "ili9341.h"

#include <stdio.h>

// 英文字体
static sFONT    *LCD_Currentfonts   = &Font8x16;
static uint16_t CurrentTextColor    = RED; // 前景色
static uint16_t CurrentBackColor    = BLACK; // 背景色

// 调用ILI9341_GramScan函数设置方向时会自动更改
uint16_t LCD_X_LENGTH = ILI9341_LESS_PIXEL;
uint16_t LCD_Y_LENGTH = ILI9341_MORE_PIXEL;

// 液晶屏扫描模式，本变量主要用于方便选择触摸屏的计算参数
// 参数可选值为0-7
// 调用ILI9341_GramScan函数设置方向时会自动更改
// LCD刚初始化完成时会使用本默认值
uint8_t LCD_SCAN_MODE = 6;

// static函数声明begin
// 写命令
static void     ili9341WriteCommand(uint16_t cmd);
// 写数据
static void     ili9341WriteData(uint16_t d);
// 读数据
static uint16_t ili9341ReadData(void);
// 屏幕寄存器初始化
static void ili9341RegConfig(void);
// 设置扫描方式
static void ili9341GramScan(uint8_t option);
// 颜色充填
static void ili9341FillColor(uint32_t pointsCnt, uint16_t color);

// static函数声明end


// 命令与数据相关
void ili9341WriteCommand(uint16_t cmd)
{
    *(__IO uint16_t*)FSMC_LCD_COMMAND_ADDR = cmd;
}

void ili9341WriteData(uint16_t d)
{
    *(__IO uint16_t*)FSMC_LCD_DATA_ADDR = d;
}

uint16_t ili9341ReadData(void)
{
    return *(__IO uint16_t*)FSMC_LCD_DATA_ADDR;
}

void backlightControl(FunctionalState state)
{
    if(state == ENABLE) {
        HAL_GPIO_WritePin(GPIOG, GPIO_LCD_BL_Pin, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(GPIOG, GPIO_LCD_BL_Pin, GPIO_PIN_SET);
    }
}

void ili9341Reset()
{
    HAL_GPIO_WritePin(GPIOG, GPIO_LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOG, GPIO_LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(120);
}

void ili9341RegConfig(void)
{
    // 屏幕主控为ST7789V

    /*  Power control B(CFh)  */
    HAL_Delay(1);
    ili9341WriteCommand(0xCF);
    ili9341WriteData(0x00);
    ili9341WriteData(0xC1);
    ili9341WriteData(0x30);
    
    /*  Power on sequence control(EDh) */
    HAL_Delay(1);
    ili9341WriteCommand(0xED);
    ili9341WriteData(0x64);
    ili9341WriteData(0x03);
    ili9341WriteData(0x12);
    ili9341WriteData(0x81);
    
    /*  Driver timing control A(E8h) */
    HAL_Delay(1);
    ili9341WriteCommand(0xE8);
    ili9341WriteData(0x85);
    ili9341WriteData(0x10);
    ili9341WriteData(0x78);
    
    /*  Power control A(CBh) */
    HAL_Delay(1);
    ili9341WriteCommand(0xCB);
    ili9341WriteData(0x39);
    ili9341WriteData(0x2C);
    ili9341WriteData(0x00);
    ili9341WriteData(0x34);
    ili9341WriteData(0x02);
    
    /* Pump ratio control(F7h) */
    HAL_Delay(1);
    ili9341WriteCommand(0xF7);
    ili9341WriteData(0x20);
    
    /* Driver timing control B */
    HAL_Delay(1);
    ili9341WriteCommand(0xEA);
    ili9341WriteData(0x00);
    ili9341WriteData(0x00);
    
    /* Power Control 1(C0h) */
    HAL_Delay(1);
    ili9341WriteCommand(0xC0);   //Power control
    ili9341WriteData(0x21);  //VRH[5:0]
    
    /* Power Control 2(C1h) */
    HAL_Delay(1);
    ili9341WriteCommand(0xC1);   //Power control
    ili9341WriteData(0x11);  //SAP[2:0];BT[3:0]
    
    /* VCOM Control 1(C5h) */
    ili9341WriteCommand(0xC5);
    ili9341WriteData(0x2D);
    ili9341WriteData(0x33);
    
    /*  VCOM Control 2(C7h)  */
    //	ili9341WriteCommand(0xC7);
    //	ili9341WriteData(0XC0);
    
    /* memory access control set */
    HAL_Delay(1);
    ili9341WriteCommand(0x36);   //Memory Access Control
    ili9341WriteData(0x00);  /*竖屏  左上角到(起点)到右下角(终点)扫描方式*/
    HAL_Delay(1);
    
    ili9341WriteCommand(0x3A);   
    ili9341WriteData(0x55); 
    
    /* Frame Rate Control(In Normal Mode/Full Colors)(B1h) */
    HAL_Delay(1);
    ili9341WriteCommand(0xB1);
    ili9341WriteData(0x00);
    ili9341WriteData(0x17);
    
    /*  Display Function Control(B6h) */
    HAL_Delay(1);
    ili9341WriteCommand(0xB6);
    ili9341WriteData(0x0A);
    ili9341WriteData(0xA2);
    
    ili9341WriteCommand(0xF6);    			
    ili9341WriteData(0x01); 
    ili9341WriteData(0x30); 
    
    /* Enable 3G(F2h) */
    ili9341WriteCommand(0xF2);
    ili9341WriteData(0x00);
    
    /* Gamma Set(26h) */
    ili9341WriteCommand(0x26);
    ili9341WriteData(0x01);
    HAL_Delay(1);
    
    /* Positive Gamma Correction */
    ili9341WriteCommand(0xe0); //Positive gamma
    ili9341WriteData(0xd0);         
    ili9341WriteData(0x00); 
    ili9341WriteData(0x02); 
    ili9341WriteData(0x07); 
    ili9341WriteData(0x0b); 
    ili9341WriteData(0x1a); 
    ili9341WriteData(0x31); 
    ili9341WriteData(0x54); 
    ili9341WriteData(0x40); 
    ili9341WriteData(0x29); 
    ili9341WriteData(0x12); 
    ili9341WriteData(0x12); 
    ili9341WriteData(0x12); 
    ili9341WriteData(0x17);

    /* Negative Gamma Correction(E1h) */
    ili9341WriteCommand(0xe1); //Negative gamma
    ili9341WriteData(0xd0); 
    ili9341WriteData(0x00); 
    ili9341WriteData(0x02); 
    ili9341WriteData(0x07); 
    ili9341WriteData(0x05); 
    ili9341WriteData(0x25); 
    ili9341WriteData(0x2d); 
    ili9341WriteData(0x44); 
    ili9341WriteData(0x45); 
    ili9341WriteData(0x1c); 
    ili9341WriteData(0x18); 
    ili9341WriteData(0x16); 
    ili9341WriteData(0x1c); 
    ili9341WriteData(0x1d); 
  
	
    //	/* column address control set */
    //	ili9341WriteCommand(CMD_SetCoordinateX); 
    //	ili9341WriteData(0x00);
    //	ili9341WriteData(0x00);
    //	ili9341WriteData(0x00);
    //	ili9341WriteData(0xEF);
    //	
    //	/* page address control set */
    //	HAL_Delay(1);
    //	ili9341WriteCommand(CMD_SetCoordinateY); 
    //	ili9341WriteData(0x00);
    //	ili9341WriteData(0x00);
    //	ili9341WriteData(0x01);
    //	ili9341WriteData(0x3F);
	
	
    /* Sleep Out(11h)  */
    ili9341WriteCommand(0x11);	  //Exit Sleep
    // ILI9341_Delay(0xAFFf<<2);
    HAL_Delay(120);
    HAL_Delay(1);
    
    /* Display ON(29h) */
    ili9341WriteCommand(0x29);   //Display on
    
    ili9341WriteCommand(0x2c);
}

void ili9341OpenWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    ili9341WriteCommand(CMD_SetCoordinateX);
    ili9341WriteData((x>>8) & 0xff);                // x起始高8位
    ili9341WriteData(x      & 0xff);                // x起始低8位
    ili9341WriteData(((x+width-1)>>8) & 0xff);      // x结束高8位
    ili9341WriteData((x+width-1)      & 0xff);      // x结束低8位

    ili9341WriteCommand(CMD_SetCoordinateY);
    ili9341WriteData((y>>8) & 0xff);                // y起始高8位
    ili9341WriteData(y      & 0xff);                // y起始低8位
    ili9341WriteData(((y+height-1)>>8) & 0xff);     // y结束高8位
    ili9341WriteData((y+height-1)      & 0xff);     // y结束低8位
}

void ili9341FillColor(uint32_t pointsCnt, uint16_t color)
{
    ili9341WriteCommand(CMD_SetPixel);
    for (uint32_t i = 0; i < pointsCnt; i++) {
        ili9341WriteData(color);
    }
}

void ili9341GramScan(uint8_t option)
{
    if (option > 7)
        return;

    if (option%2 == 0) {
        // 0 2 4 6模式下X方向像素宽度为240，Y方向为320
        LCD_X_LENGTH = ILI9341_LESS_PIXEL;
		LCD_Y_LENGTH =	ILI9341_MORE_PIXEL;
    } else {
        // 1 3 5 7模式下X方向像素宽度为320，Y方向为240
		LCD_X_LENGTH = ILI9341_MORE_PIXEL;
		LCD_Y_LENGTH =	ILI9341_LESS_PIXEL; 
    }
    // 0x36命令参数的高3位可用于设置GRAM扫描方向	
    ili9341WriteCommand(0x36);
    // ST7789V使用0x00
    ili9341WriteData(0x00 | (option<<5)); // 根据Option的值设置LCD参数，共0-7种模式

    ili9341WriteCommand(CMD_SetCoordinateX);
    ili9341WriteData(0x00); /* x 起始坐标高8位 */
    ili9341WriteData(0x00); /* x 起始坐标低8位 */
    ili9341WriteData((LCD_X_LENGTH-1)>>8 & 0xff);   /* x 结束坐标高8位 */
    ili9341WriteData((LCD_X_LENGTH-1)    & 0xff);   /* x 结束坐标高8位 */

    ili9341WriteCommand(CMD_SetCoordinateY);
    ili9341WriteData(0x00); /* y 起始坐标高8位 */
    ili9341WriteData(0x00); /* y 起始坐标低8位 */
    ili9341WriteData((LCD_Y_LENGTH-1)>>8 & 0xff);   /* y 结束坐标高8位 */
    ili9341WriteData((LCD_Y_LENGTH-1)    & 0xff);   /* y 结束坐标高8位 */

    /* write gram start */
	ili9341WriteCommand(CMD_SetPixel);	
}

uint16_t ili9341ReadID()
{
    uint16_t id = 0;

    // 我的这款实际上是ST7789V了
    // 0x04: 获取ID
    ili9341WriteCommand(0x04);
    ili9341ReadData();
    ili9341ReadData();
    id = ili9341ReadData();
    id <<= 8;
    id |= ili9341ReadData();

    printf("lcd id is [0x%x]\n", id);

    return id;
}

void ili9341Init()
{
    // 开启背光
    backlightControl(ENABLE);
    // 复位ILI9341
    ili9341Reset();
    // 初始化屏幕寄存器
    ili9341RegConfig();
    // 设置扫描模式
    ili9341GramScan(LCD_SCAN_MODE);
    // 设置为纯色
    ili9341OpenWindow(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
    ili9341FillColor(LCD_X_LENGTH*LCD_Y_LENGTH, BLACK);
}

void ili9341ShowChar(uint16_t x, uint16_t y, char c)
{
    // 当前字符相对于空格的偏移
    uint16_t chrOffset = c - ' ';
    // 每个字符多少个bytes
    uint8_t fontLength = (LCD_Currentfonts->Width * LCD_Currentfonts->Height) / 8;

    // 防止越界
    // if ((chrOffset * fontLength) > (sizeof(LCD_Currentfonts->table)/sizeof(LCD_Currentfonts->table[0])) {
    //     return;
    // }

    // 得到目标字符的偏移
    const uint8_t *pFont = &(LCD_Currentfonts->table[chrOffset * fontLength]);

    // 每个字符打开一个窗口
    ili9341OpenWindow(x, y, LCD_Currentfonts->Width, LCD_Currentfonts->Height);
    ili9341WriteCommand(CMD_SetPixel);

    for (uint8_t byteCount = 0; byteCount < fontLength; byteCount++) {
        for (uint8_t bitCount = 0; bitCount < 8; bitCount++) {
            if (pFont[byteCount] & (0x80 >> bitCount)) {
                ili9341WriteData(CurrentTextColor);
            } else {
                ili9341WriteData(CurrentBackColor);
            }
        }
    }
}

void ili9341ShowString(uint16_t x, uint16_t y, const char* str, size_t len)
{
    (void)len;

    while (*str) {
        if ((x - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH) {
                // 换行
                y += LCD_Currentfonts->Height;
                x = ILI9341_DispWindow_X_Star;
            }

            if ((y - ILI9341_DispWindow_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH) {
                // 横向写满了, 重新回到第一行第一列
                y = ILI9341_DispWindow_Y_Star;
                x = ILI9341_DispWindow_X_Star;
            }
            ili9341ShowChar(x, y, *str);
            x += LCD_Currentfonts->Width;
        str++;
    }

}

