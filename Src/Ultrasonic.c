#include <string.h>
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"
#include "ultrasonic.h"
#include "tcpServer.h"

extern UART_HandleTypeDef huart5;

// 发送读取超声波命令的函数   超声波温度
HAL_StatusTypeDef GetUltraTemperature()
{
    //KS103 instruction structure:
    //0xe8: Address code
    //0x02: Command register address
    //0xc9: Read temperature(9bit)
    uint8_t cmd[] = {0xe8, 0x02, 0xc9};
    return HAL_UART_Transmit(&huart5,cmd,3,100);
}

// 解析并返回读取的超声波数据
float anaslyUltraTemperature(uint8_t Data[5]){
    int temp;
    // int numOfBuffer = 5 - huart5.RxXferCount;
    if(Data[0] == NULL) return -100.0f;   // 出错
    // if(read(fdSerialUltraSonic, rawData, 5)<2)return -100.0f;
    temp = Data[0]&0x07;
    temp = temp << 8;
    temp += Data[1];
    if(Data[0] & 0x80) temp = -temp;//Negative temperature

    // 重置接收中断
    return (float)temp/16.0f;
}

// 超声波距离
HAL_StatusTypeDef GetUltraDistance()
{
    //KS103 instruction structure:
    //0xe8: Address code
    //0x02: Command register address
    //0xb4: Read distance(0-5m, with temperature compensation), requires max=87ms of time
    uint8_t cmd[] = {0xe8, 0x02, 0xb4};
    return HAL_UART_Transmit(&huart5,cmd,3,100);
}

int anaslyUltraDistance(uint8_t rawData[5]){
    int distance;
    // const char cmd[] = {0xe8, 0x02, 0xb4};
    // int numOfBuffer = 5 - huart5.RxXferCount;
    if(rawData[0] == NULL) return -1;   // 出错
    distance = rawData[0];
    distance<<=8;
    distance +=rawData[1];
    return distance;
}

// 超声波时间
// @return {HAL_ERROR} {HAL_TIMEOUT} {HAL_OK}
HAL_StatusTypeDef GetUltraDurationTime()
{
    //KS103 instruction structure:
    //0xe8: Address code
    //0x02: Command register address
    //0x07: Read delay(us), distance_max=700mm
    uint8_t cmd[] = {0xe8, 0x02, 0x07};
    return HAL_UART_Transmit(&huart5,cmd,3,100);
}

int anaslyUltraTime(uint8_t rawData[5]){
    int duration;
    //KS103 instruction structure:
    //0xe8: Address code
    //0x02: Command register address
    //0x07: Read delay(us), distance_max=700mm
    // int numOfBuffer = 5 - huart5.RxXferCount;
    if(rawData[0]==NULL) return -1;   // 出错
    duration = rawData[0];
    duration<<=8;
    duration +=rawData[1];
    return duration;
}

// 放在主函数中循环获取并发送超声波的温度，距离和时间
void loopUltra(){
    uint8_t ultraTemperature[5];
    uint8_t ultraDistance[5],ultraTime[5];
    // 发送获取超声波其温度并发送到远程
    if(GetUltraTemperature() == HAL_OK){
        // 接收成功
        if(HAL_UART_Receive(&huart5,ultraTemperature,5,100) == HAL_OK){
            // char ret[5] = ultraTemperature;
            float receiveTemper = anaslyUltraTemperature(ultraTemperature);

            char uTemperChar[16];
            sprintf(uTemperChar,"%g",receiveTemper);
			char temperature[25]="Temperature:";
            sendData(temperature,uTemperChar,100);
        }
    }
    // 发送获取超声波液位并发送到远程
    if(GetUltraDistance() == HAL_OK){
        // 接收成功
        if(HAL_UART_Receive(&huart5,ultraDistance,5,100) == HAL_OK){
            // char ret[5] = *ultraDistance;
            int receiveDistance = anaslyUltraDistance(ultraDistance);

            char uDistanceChar[16];
            sprintf(uDistanceChar,"%d",receiveDistance);
			char distance[25]="Distance:";
            sendData(distance,uDistanceChar,100);
        }
    }
    // 发送并获取超声波时间并发送到远程
    if(GetUltraDurationTime() == HAL_OK){
        // 接收成功
        if(HAL_UART_Receive(&huart5,ultraTime,5,100) == HAL_OK){
            // char ret[5] = *ultraTime;
            int receiveTime = anaslyUltraTime(ultraTime);

            char uTimeChar[16];
            sprintf(uTimeChar,"%d",receiveTime);
			char ultrasonic[25] = "Ultrasonic:";
            sendData(ultrasonic,uTimeChar,100);
        }
    }
}
