#include "ds18b20.h"
#include "stm32f1xx_hal_tim.h"

#include "delay.h"

#include <stdio.h>

extern TIM_HandleTypeDef htim6;

static void writeDQ(GPIO_PinState state)
{
    HAL_GPIO_WritePin(DS18B20_GPIO, DS18B20_DQ_PIN, state);
    delay_us(2);
}

#define DQ_HIGH()                   writeDQ(GPIO_PIN_SET)
#define DQ_LOW()                    writeDQ(GPIO_PIN_RESET)
#define DQ_READ()                   HAL_GPIO_ReadPin(DS18B20_GPIO, DS18B20_DQ_PIN)

void DS18B20_Init(void)
{
    DS18B20_CLK_ENABLE();

    GPIO_InitTypeDef ds18b20Gpio;
    // 使用开漏模式, 高电平PIN进入高阻态
    ds18b20Gpio.Mode    = GPIO_MODE_OUTPUT_OD;
    ds18b20Gpio.Pin     = DS18B20_DQ_PIN;
    ds18b20Gpio.Pull    = GPIO_NOPULL;
    ds18b20Gpio.Speed   = GPIO_SPEED_HIGH;

    HAL_GPIO_Init(DS18B20_GPIO, &ds18b20Gpio);
    HAL_GPIO_WritePin(DS18B20_GPIO, DS18B20_DQ_PIN, GPIO_PIN_SET); 
}

/*
发送初始化时序——主机发送复位脉冲且从机正确回复应答脉冲的过程
请求: 主机通过拉低单线480us以上, 产生复位脉冲, 然后释放该线, 进入Rx接收模式. 主机释放总线时, 会产生一个上升沿脉冲.
DQ : 1 -> 0(480us+) -> 1
响应: DS18B20检测到该上升沿后, 延时15~60us, 通过拉低总线60~240us来产生应答脉冲.
DQ: 1(15~60us) -> 0(60~240us)
接收响应: 主机接收到从机的应答脉冲后, 说明有单线器件在线. 至此, 初始化完成.
DQ: 0
https://blog.csdn.net/qq_43279384/article/details/104945464
*/
int DS18B20_Reset(void)
{
    DQ_LOW();
    delay_us(480);
    DQ_HIGH();

    // 等待被DS18B20拉低
    __HAL_TIM_SET_COUNTER(&htim6, 0);
    while (DQ_READ() == GPIO_PIN_SET)
    {
        if (__HAL_TIM_GET_COUNTER(&htim6) > 100) {
            printf("waiting for ds18b20 pull down timeout!\r\n");
            return -1;
        }
    }

    // 等待DS18B20释放总线
    __HAL_TIM_SET_COUNTER(&htim6, 0);
    while (DQ_READ() == GPIO_PIN_RESET)
    {
        // 拉低总线60~240us来产生应答脉冲.
        if (__HAL_TIM_GET_COUNTER(&htim6) > 300) {
            printf("waiting for ds18b20 release timeout!\r\n");
            return -1;
        }
    }

    return 0;
}

void DS18B20_WriteByte(uint8_t data)
{   
    // !注意! 不论高低, 每个bit要保持60us, 否则协议不识别
    // 先发LSB
    for (int i = 0; i < 8; i++) {                            
        if (data & 0x01) {      // H
            DQ_LOW();
            delay_us(15);
            DQ_HIGH();
            delay_us(45);
        } else {                // L
            DQ_LOW();
            delay_us(55);
            DQ_HIGH();
        }
        // 要发送的数据右移动一位
        data >>= 1;
    }
}

uint8_t DS18B20_ReadByte()
{
    uint8_t result = 0;
    uint8_t tmp = 0;

    for (int i = 0; i < 8; i++) {
        // 每个周期固定60us
        DQ_LOW();
        delay_us(5);
        DQ_HIGH();
        delay_us(5);
        tmp = DQ_READ();
        delay_us(50);

        // 置位1
        if (tmp) {
            result |= (tmp << i);
        }
    }

    return result;
}

int DS18B20_ReadTemperature(double *temperature)
{
    // 复位
    if (DS18B20_Reset() < 0) {
        printf("reset ds18b20 failed\r\n");
        return -1;
    } else {
        printf("reset ds18b20 success\r\n");
    }
    HAL_Delay(10);
    // 忽略序列号
    DS18B20_WriteByte(0xCC);
    // 启动温度转换
    DS18B20_WriteByte(0x44);
    HAL_Delay(1);

    // 等待温度转换完成
    // 返回0表示还在进行转换
    // 返回0xFF表示转换已经完成
    while(DS18B20_ReadByte() != 0xFF) {
        printf("polling for convertion done...\r\n");
        HAL_Delay(50);
    }

    // 复位
    if (DS18B20_Reset() < 0) {
        printf("reset ds18b20 failed\r\n");
        return -1;
    } else {
        printf("reset ds18b20 success\r\n");
    }
    // 忽略序列号
    DS18B20_WriteByte(0xCC);
    // 读温度寄存器
    DS18B20_WriteByte(0xBE);
    HAL_Delay(1);

    uint8_t tempL = DS18B20_ReadByte();
    uint8_t tempH = DS18B20_ReadByte();

    uint16_t temp = ((uint16_t)tempH << 8) | ((uint16_t)tempL);
    *temperature = (double)temp / 16;
    printf("tempL: [%d], tempH: [%d] temp: [%.2f]\r\n", tempL, tempH, *temperature);

    return 0;
}