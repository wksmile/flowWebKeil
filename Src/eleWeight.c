#include <string.h>
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"
#include "elecWeight.h"
#include "RS485.h"
#include "IOControl.h"
#include "tcpServer.h"
#include "test.h"

extern UART_HandleTypeDef huart7;
extern UART_HandleTypeDef huart1;

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

extern char receive7[20];

float weight=0;

// 发送获取电子秤重量指令
// @return {HAL_ERROR} {HAL_TIMEOUT} {HAL_OK}
// HAL_StatusTypeDef 类型
// 数据格式 ww0000.00kg
// 量程为30kg
// 总耗时511ms
void GetWeightValue()
{                                                  
    //Conversion takes a long time, so start new conversion after reading.
    //Clear input buffer
    // tcflush(fdSerialWeight, TCIFLUSH);
    //Send read command
    uint8_t cmd[] = {'R'};
    HAL_UART_Transmit(&huart7,cmd,sizeof(cmd),0xff);
}

// 解析电子秤获取的重量
float analyWeightValue(int8_t *rawData){
    float weight = 0;
    if(rawData[0]!='w' || rawData[1]!='w')return -100;//Error...
    /*
    rawData[3]-='0';
    rawData[4]-='0';
    rawData[5]-='0';
    rawData[7]-='0';
    rawData[8]-='0';
    */
    weight = (rawData[3]-0x30)*100.0f + (rawData[4]-0x30)*10.0f + (rawData[5]-0x30)*1.0f + (rawData[7]-0x30)*0.10f + (rawData[8]-0x30)*0.01f;
    // 超过最大重量，关闭电机，打开下水阀
    if(weight > 20) {
        // 打开下水阀
        K3Close();
    }
    if(weight < 1) {
        K3Open();
    }
    return weight;
}

// 循环获取电子秤的重量,电子秤采样时间500ms
void loopWeight() {
    // GetWeightValue();
    // HAL_Delay(550);
    float innerWeight;
    //HAL_UART_Receive(&huart7,weightData,20,100);
    if(receive7[0] != 0){
        // innerWeight可能为-100；
        innerWeight=analyWeightValue((int8_t *)receive7);
        if(innerWeight!=-100) weight=innerWeight;
        //huart7.RxXferCount = 20;
        //huart7.pRxBuffPtr = receive7;
        // char ret[20] = *weightData;
        /*
        float receivWeight = analyWeightValue(receive7);

        char weightChar[10];
        sprintf(weightChar,"%g",receivWeight);
		char weight[15] = "Weight:";
        sendData(weight,weightChar,100);
        */
		/*
		for(int i=0;i<20;i++)
		{
			receive7[i] = 0;
		}*/
		// 这里需要加上这一句话
		open7Receive(); 
		huart7.RxXferCount = huart7.RxXferSize;
		huart7.pRxBuffPtr = receive7;
	}
}

// 电子秤清零
void SetWeightZero()
{
    uint8_t zmd[] = {'Z'};
    HAL_UART_Transmit(&huart7,zmd,2,100);
}

float getWeight()
{
    return weight;
}

char bufWeight[64];
// 画电子秤的曲线
void drawWeight()
{
    int dealRcvBuf;
    // 转化量程为0-250
    dealRcvBuf = (int)weight*5;
    sprintf(bufWeight,"add 13,0,%d",dealRcvBuf);
    uint8_t counter = 0;
    while(1)
    {
        if(bufWeight[counter] == '\0')
        {
            break;
        }
        counter ++;
    }
    HAL_UART_Transmit(&huart1,(uint8_t *)bufWeight,counter,0xffff);
    UART_Send_END();
}
