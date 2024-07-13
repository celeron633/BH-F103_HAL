#include "dht11.h"
#include "delay.h"

#include <stdio.h>
#include <string.h>

extern TIM_HandleTypeDef htim6;

int retryCount1 = 0;
int retryCount2 = 0;
int retryCount3 = 0;

static inline void dht11DataIn()
{
    GPIOA->CRL &= 0xFF0FFFFF;
    GPIOA->CRL |= (0x04 << 20);
}

static inline void dht11DataOut()
{
    GPIOA->CRL &= 0xFF0FFFFF;
    GPIOA->CRL |= (0x07 << 20);
}

void dht11Init()
{
    // 启动定时器
    __HAL_TIM_ENABLE(&htim6);

    // 初始化GPIO
    GPIO_InitTypeDef dht11GPIO;
    dht11GPIO.Mode  = GPIO_MODE_OUTPUT_OD;
    dht11GPIO.Pin   = DHT11_DATA_PORT;
    dht11GPIO.Pull  = GPIO_NOPULL;
    dht11GPIO.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(DHT11_DATA_GPIO, &dht11GPIO);

}

int dht11Check()
{
    // 切换到输入模式
    dht11DataIn();
    // 先会被DHT11拉低80us左右, 再会被DHT11拉高80us
    int retryCnt = 0;
    // 等待被拉低
    while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO, DHT11_DATA_PORT) == GPIO_PIN_SET) {
        retryCnt++;
        tim_delay_us(10);
        // 超时了
        if (retryCnt >= 5) {
            return -1;
        }
    }
    retryCount1 = retryCnt;
    retryCnt = 0;
    // 等待被拉高
    while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO, DHT11_DATA_PORT) == GPIO_PIN_RESET) {
        retryCnt++;
        tim_delay_us(10);
        // 超时了
        if (retryCnt >= 12) {
            return -1;
        }
    }
    retryCount2 = retryCnt;
    retryCnt = 0;

    // 等待再次被拉低
    while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO, DHT11_DATA_PORT) == GPIO_PIN_SET) {
        retryCnt++;
        tim_delay_us(10);
        if (retryCnt >= 12) {
            return -1;
        }
    }
    retryCount3 = retryCnt;
    // 后面DHT11开始返回温度数据了
}

void dht11Reset()
{
    // 切换到输出模式
    dht11DataOut();
    // 先拉低18毫秒
    HAL_GPIO_WritePin(DHT11_DATA_GPIO, DHT11_DATA_PORT, GPIO_PIN_RESET);
    HAL_Delay(18);
    // 再拉高30us, 等待DHT11响应
    HAL_GPIO_WritePin(DHT11_DATA_GPIO, DHT11_DATA_PORT, GPIO_PIN_SET);
    tim_delay_us(30);
}

uint8_t dht11ReadBit(void)
{
    uint8_t bit = 0;
    // 等待拉高
    while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO, DHT11_DATA_PORT) == GPIO_PIN_RESET) {

    }
    tim_delay_us(40);
    // 拉高后40us检查
    if (HAL_GPIO_ReadPin(DHT11_DATA_GPIO, DHT11_DATA_PORT) == GPIO_PIN_RESET) {
        bit = 1;
    } else {
        bit = 0;
    }
    // 再次等待到高电平结束, 为读取下个bit做好准备
    while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO, DHT11_DATA_PORT) == GPIO_PIN_SET) {

    }

    return bit;
}

uint8_t dht11ReadByte(void)
{
    uint8_t data = 0;
    uint8_t tmp = 0;

    for (uint8_t i = 0; i < 8; i++) {
        tmp = dht11ReadBit();
        data = data << 1;
        data = data | tmp;
        // DHT11先传输高位, 再传输地位从MSB到LSB传输
    }
    
    return data;
}

int dht11ReadData(uint8_t *temp, uint8_t *humi)
{
    // 复位后, dht11会返回5个字节40个bit的数据
    static uint8_t data[5] = {0x00};
    // 先复位
    dht11Reset();
    // 检查复位情况
    if (dht11Check() < 0) {
        printf("reset sensor failed!");
        return -1;
    }

    // 读取5个字节, 40个bit
    for (int i = 0; i < 5; i++) {
        data[i] = dht11ReadByte();
    }


    printf("==data begin==\r\n");
    for (int i = 0; i < 5; i++) {
        printf("0x%x\r\n", data[i]);
    }
    printf("==data end==\r\n");

    // 计算校验和
    uint8_t chkSum = 0;
    for (int i = 0; i < 4; i++) {
        chkSum += data[i];
    }
    printf("chkSum: %x\r\n", chkSum);

    // 处理温度, 湿度数据

    return 0;
}