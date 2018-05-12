#include <string.h>
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"
#include "elecWeight.h"
#include "RS485.h"
#include "IOControl.h"
#include "tcpServer.h"

extern UART_HandleTypeDef huart7;

// 电子秤的超重重量
#define OVERFLOW_WEIGHT 20
// char weightRxBuffer[20] = {0};
// char ultraRxBuffer[5] = {0};

// 放在while轮询查看手否收到数据
/*
void weightAndUltraDataReceived() {
    unsigned char numOfBuffer;
    // 接收电子秤数据中断
    if(huart7.RxXferCount < 20) {
        numOfBuffer = 20-huart7.RxXferCount;   // 实际接收到的字符总数
        // 这里处理数据
        huart7.RxXferCount = 200;
        huart7.pRxBuffPtr = weightRxBuffer;
    }
    if(huart5.RxXferCount < 5) {
        numOfBuffer = 5-huart5.RxXferCount;   // 实际接收到的字符总数
        // 这里处理数据
        huart5.RxXferCount = 5;
        huart5.pRxBuffPtr = ultraRxBuffer;
    }
}   */

// 发送获取电子秤重量指令
// @return {HAL_ERROR} {HAL_TIMEOUT} {HAL_OK}
// HAL_StatusTypeDef 类型
// 数据格式 ww0000.00kg
// 量程为30kg
void GetWeightValue()
{                                                  
    //Conversion takes a long time, so start new conversion after reading.
    //Clear input buffer
    // tcflush(fdSerialWeight, TCIFLUSH);
    //Send read command
    uint8_t cmd[] = {'R'};
    HAL_UART_Transmit(&huart7,cmd,2,100);
}

// 解析电子秤获取的重量
float analyWeightValue(uint8_t rawData[20]){
    float weight = 0;
    rawData[3]-='0';
    rawData[4]-='0';
    rawData[5]-='0';
    rawData[7]-='0';
    rawData[8]-='0';
    weight = rawData[3]*100.0f + rawData[4]*10.0f + rawData[5]*1.0f + rawData[7]*0.10f + rawData[8]*0.01f;
    if(rawData[2]=='-')weight = -weight;

    if(rawData[0]!='w' || rawData[1]!='w')return -100.0f;//Error...
    // 超过最大重量，关闭电机，打开下水阀
    if(weight > 20) {
        // 关闭电机
        StopInverter();
        // 打开下水阀
        K3Open();
    }
    return weight;
}

// 循环获取电子秤的重量
void loopWeight() {
    uint8_t weightData[20];
	GetWeightValue();
	HAL_Delay(500);
    HAL_UART_Receive(&huart7,weightData,20,100);
    if(weightData[0] != NULL){
        // char ret[20] = *weightData;
        float receivWeight = analyWeightValue(weightData);

        char weightChar[10];
        sprintf(weightChar,"%g",receivWeight);
		char weight[15] = "Weight:";
        sendData(weight,weightChar,100);
    }
}

// 电子秤清零
HAL_StatusTypeDef SetWeightZero()
{
    uint8_t cmd[] = {'Z'};
    return HAL_UART_Transmit(&huart7,cmd,2,100);
}

