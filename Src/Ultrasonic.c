#include <string.h>
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"
#include "ultrasonic.h"
#include "tcpServer.h"
#include "test.h"

extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart1;

uint8_t receive5[5];
// 打开5号串口接收中断
// 调试的时候Rx接串口的Tx，Tx接串口的Rx
// 这里5改为1了
void open5Receive(){
    HAL_UART_Receive_IT(&huart5,receive5,1);
}

// 返回9位精度数据，按DS18B20格式，范围为 -40~+125 摄氏度，探测耗时约83ms.
// 发送读取超声波命令的函数   超声波温度
// 0x01 0x80
void GetUltraTemperature()

{
    //KS103 instruction structure:
    //0xe8: Address code
    //0x02: Command register address
    //0xc9: Read temperature(9bit)
    uint8_t cmd[] = {0xe8, 0x02, 0xc9};
    HAL_UART_Transmit(&huart5,cmd,3,100);
}

// 解析并返回读取的超声波数据
float anaslyUltraTemperature(uint8_t Data[5]){
    int temp;
    //if(Data[0] == NULL) return -100.0f;   // 出错
    temp = Data[0]&0x07;
    temp = temp << 8;
    temp += Data[1];
    if(Data[0] & 0x80) temp = -temp;//Negative temperature
    // 重置接收中断
    return temp/16.0f;
}

// 超声波距离,单位毫米,范围0x0a~0x1450，探测耗时约83毫秒
// 返回格式 0x01 0x74
void GetUltraDistance()
{
    //KS103 instruction structure:
    //0xe8: Address code
    //0x02: Command register address
    //0xb4: Read distance(0-5m, with temperature compensation), requires max=87ms of time
    uint8_t cmd[] = {0xe8, 0x02, 0xb4};
    HAL_UART_Transmit(&huart5,cmd,sizeof(cmd),0xFF);
}

int anaslyUltraDistance(uint8_t rawData[5]){
    unsigned int distance;
    // const char cmd[] = {0xe8, 0x02, 0xb4};
    // int numOfBuffer = 5 - huart5.RxXferCount;
    // if(rawData[0] == NULL) return -1;   // 出错
    distance = rawData[0];
    distance<<=8;
    distance +=rawData[1];
    // 将结果毫米量程转化为0~50cm；
    distance = distance*255/(10*50);
    return distance;
}

// 超声波时间，单位us,范围0x44~0xfc7，探测时间最大5毫秒；
// 返回0x08 0x8A
// @return {HAL_ERROR} {HAL_TIMEOUT} {HAL_OK}
void GetUltraDurationTime()
{
    //KS103 instruction structure:
    //0xe8: Address code
    //0x02: Command register address
    //0x07: Read delay(us), distance_max=700mm
    uint8_t cmd[] = {0xe8, 0x02, 0x07};
    HAL_UART_Transmit(&huart5,cmd,3,100);
}

int anaslyUltraTime(uint8_t rawData[5]){
    int duration;
    //KS103 instruction structure:
    //0xe8: Address code
    //0x02: Command register address
    //0x07: Read delay(us), distance_max=700mm
    // int numOfBuffer = 5 - huart5.RxXferCount;
    // if(rawData[0]==NULL) return -1;   // 出错
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
    GetUltraTemperature();
    HAL_Delay(500);
    // 接收成功
    HAL_UART_Receive(&huart5,ultraTemperature,5,100);
        // char ret[5] = ultraTemperature;
        float receiveTemper = anaslyUltraTemperature(ultraTemperature);

        char uTemperChar[16];
        sprintf(uTemperChar,"%g",receiveTemper);
    	char temperature[25]="Temperature:";
        sendData(temperature,uTemperChar,100);
        // 发送到触摸屏
        sendDataTouchScreen(temperature,uTemperChar,100);

        // 发送获取超声波液位并发送到远程
        GetUltraDistance();
        HAL_Delay(500);
        // 接收成功
        HAL_UART_Receive(&huart5,ultraDistance,5,100);
            // char ret[5] = *ultraDistance;
            int receiveDistance = anaslyUltraDistance(ultraDistance);

            char uDistanceChar[16];
            sprintf(uDistanceChar,"%d",receiveDistance);
			char distance[25]="Distance:";
            sendData(distance,uDistanceChar,100);
            // 发送到触摸屏
            sendDataTouchScreen(distance,uDistanceChar,100);

    // 发送并获取超声波时间并发送到远程
    GetUltraDurationTime();
    HAL_Delay(500);
        // 接收成功
        HAL_UART_Receive(&huart5,ultraTime,5,100);
            // char ret[5] = *ultraTime;
            int receiveTime = anaslyUltraTime(ultraTime);

            char uTimeChar[16];
            sprintf(uTimeChar,"%d",receiveTime);
			char ultrasonic[25] = "Ultrasonic:";
            sendData(ultrasonic,uTimeChar,100);
            // 发送到触摸屏
            sendDataTouchScreen(ultrasonic,uTimeChar,100);
}

char buf5[64];
char buf5T[64];
char buf5Time[64];
// 检查5号串口接收的数据
void checkUart5Receive()
{
    uint8_t numOfBuffer;
    if(huart5.RxXferCount < 5) {
        numOfBuffer = 5-huart5.RxXferCount;
        if(numOfBuffer==2)
        {
            int result = anaslyUltraDistance(receive5);
            if(result!=-1){
                sprintf(buf5,"add 13,1,%d",result);
                uint8_t counter = 0;
                while(1)
                {
                    if(buf5[counter] == '\0')
                    {
                        break;
                    }
                    counter ++;
                }
                HAL_UART_Transmit(&huart2,(uint8_t *)buf5,counter,0xffff);
                UART_Send_END();
            }
        }
        for(int i=0;i<5;i++)
        {
            receive5[i] = 0;
        }
        huart5.RxXferCount = 5;
        huart5.pRxBuffPtr = receive5;
        //HAL_UART_Transmit_IT(&huart7, &sendData7, sizeof(sendData7));
    }
}

char stage=0;
void ultraData()
{
    if(stage==0)
    {
        // 发送查询距离
        GetUltraDistance();
        HAL_Delay(100);
        stage=1;
    }
    if(stage==1)
    {
        if(receive5[0]!=0 || receive5[1]!=0)
        {
            int result = anaslyUltraDistance(receive5);
            if(result!=-1){
                sprintf(buf5,"add 13,1,%d",result);
                uint8_t counter = 0;
                while(1)
                {
                    if(buf5[counter] == '\0')
                    {
                        break;
                    }
                    counter ++;
                }
                HAL_UART_Transmit(&huart1,(uint8_t *)buf5,counter,0xffff);
                UART_Send_END();
            }
        }
        for(int i=0;i<5;i++)
        {
            receive5[i] = 0;
        }
        huart5.RxXferCount = 5;
        huart5.pRxBuffPtr = receive5;
        stage=2;
    }
    if(stage==2)
    {
        // 发送查询温度
        GetUltraTemperature();
        HAL_Delay(100);
        stage=3;
    }
    if(stage==3)
    {
        if(receive5[0]!=0 || receive5[1]!=0)
        {
            int resultTemp = (int)anaslyUltraTemperature(receive5);
            if(resultTemp!=-1){
                sprintf(buf5T,"add 13,2,%d",resultTemp);
                uint8_t counterT = 0;
                while(1)
                {
                    if(buf5T[counterT] == '\0')
                    {
                        break;
                    }
                    counterT ++;
                }
                HAL_UART_Transmit(&huart1,(uint8_t *)buf5T,counterT,0xffff);
                UART_Send_END();
            }
        }
        for(int i=0;i<5;i++)
        {
            receive5[i] = 0;
        }
        huart5.RxXferCount = 5;
        huart5.pRxBuffPtr = receive5;
        stage=4;
    }
    if(stage==4)
    {
        // 获取超声波时间
        GetUltraDurationTime();
        HAL_Delay(100);
        stage=5;
    }
    if(stage==5)
    {
        if(receive5[0]!=0 || receive5[1]!=0)
        {
            // 把返回的时间比如2186us量程除以10，是的在255之内。
            int resultTime = anaslyUltraTime(receive5)/20;
            if(resultTime!=-1){
                sprintf(buf5Time,"add 13,3,%d",resultTime);
                uint8_t counterTime = 0;
                while(1)
                {
                    if(buf5Time[counterTime] == '\0')
                    {
                        break;
                    }
                    counterTime ++;
                }
                HAL_UART_Transmit(&huart1,(uint8_t *)buf5Time,counterTime,0xffff);
                UART_Send_END();
            }
        }
        for(int i=0;i<5;i++)
        {
            receive5[i] = 0;
        }
        huart5.RxXferCount = 5;
        huart5.pRxBuffPtr = receive5;
        stage=0;
    }
}

