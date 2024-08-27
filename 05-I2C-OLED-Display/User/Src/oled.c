#include "oled.h"

#include <stdio.h>

struct OLED_Config oledCfg;

void configOledDisplay(I2C_HandleTypeDef *h, uint8_t i2cAddr)
{
    oledCfg.i2cHandle = h;
    oledCfg.i2cAddr = i2cAddr;
}

static int oledWriteCmd(uint8_t cmd)
{
    uint8_t buf[2];
    buf[0] = 0x00;
    buf[1] = cmd;
    return HAL_I2C_Master_Transmit(oledCfg.i2cHandle, oledCfg.i2cAddr, buf, sizeof(buf), HAL_MAX_DELAY);
}

static int oledWriteData(uint8_t data)
{
    uint8_t buf[2];
    buf[0] = 0x40;
    buf[1] = data;
    return HAL_I2C_Master_Transmit(oledCfg.i2cHandle, oledCfg.i2cAddr, buf, sizeof(buf), HAL_MAX_DELAY);
}

int initOledDisplay()
{
    if (oledCfg.i2cHandle->State != HAL_I2C_STATE_READY) {
        return -1;
    }

    // wait some time for display init
    HAL_Delay(500);

    oledWriteCmd(0xAE); // 屏幕关
	oledWriteCmd(0x20); // 设置寻址模式
	oledWriteCmd(0x10); /* 
                        00,Horizontal Addressing Mode;
		    		    01,Vertical Addressing Mode;
		    		    10,Page    Addressing Mode (RESET);
		    		    11,Invalid 
                        */
	oledWriteCmd(0xb0); // Set Page Start Address for Page Addressing Mode,0-7
	oledWriteCmd(0xc8); // COM扫描反向
	oledWriteCmd(0x00); // 起始列地址低位
	oledWriteCmd(0x10); // 起始列地址高位
	oledWriteCmd(0x40); // set start line address
	oledWriteCmd(0x81); // 设置对比度
	oledWriteCmd(0xff); // 亮度调节 范围0x00~0xff
	oledWriteCmd(0xa1); // set segment re-map 0 to 127
	oledWriteCmd(0xa6); // 正常显示
	
	oledWriteCmd(0xa8); // 设置MUX数(1 to 64)
	oledWriteCmd(0x3F); // MUX=63 显示63行
	
	oledWriteCmd(0xa4); // 0xa4,输出 RAM 内容;0xa5,忽略 RAM 内容
	oledWriteCmd(0xd3); // 设置显示偏移
	oledWriteCmd(0x00); // 没有偏移
	
	oledWriteCmd(0xd5); // 设置显示时钟分频数、振荡器频率
	oledWriteCmd(0xf0); // set divide ratio
	oledWriteCmd(0xd9); // 设置预充电周期
	oledWriteCmd(0x22); //
	oledWriteCmd(0xda); // COM引脚配置
	oledWriteCmd(0x12); // 启用左右反置
	
	oledWriteCmd(0xdb); // 设置 vcomh 输出
	oledWriteCmd(0x20); // 0x20,0.77xVcc
	
	oledWriteCmd(0x8d); // set DC-DC enable
	oledWriteCmd(0x14); //
	oledWriteCmd(0xaf); // turn on oled panel

    return 0;
}