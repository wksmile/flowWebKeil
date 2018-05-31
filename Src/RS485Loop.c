#include <string.h>
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"
#include "stdio.h"
#include "RS485.h"
#include "tcpServer.h"
#include "ultrasonic.h"
// #include "mySleep.h"

// 注意除了热能表的需要改变频率为2400外，其他的485位9600
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;


// 3号串口接收中断
uint8_t receive3[30];
void open3Receive(){
    HAL_UART_Receive_IT(&huart3,receive3,30);
}

// 3号串口接收中断
uint8_t receive6[100];
void open6Receive(){
    HAL_UART_Receive_IT(&huart6,receive6,100);
}

// 改变波特率为2400,奇校验，8位数据位，1位停止位
static void uart3_change_baudrate_2400(void)
{
  huart3.Init.BaudRate = 2400;
  huart3.Init.Parity = UART_PARITY_EVEN;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

// 改变波特率为9600，无校验位，8位数据位，1位停止位
static void uart3_change_baudrate_9600(void)
{
  huart3.Init.BaudRate = 9600;
  huart3.Init.Parity = UART_PARITY_NONE;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

// CRC检验
unsigned short CRCCodeTest(unsigned char *nData, int wLength)
{
    static unsigned short wCRCTable[] = {
    0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
    0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
    0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
    0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
    0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
    0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
    0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
    0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
    0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
    0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
    0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
    0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
    0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
    0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
    0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
    0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
    0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
    0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
    0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
    0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
    0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
    0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
    0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
    0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
    0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
    0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
    0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
    0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
    0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
    0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
    0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
    0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040 };

    unsigned char nTemp;
    unsigned short wCRCWord = 0xFFFF;

    while (wLength--)
    {
      nTemp = *nData++ ^ wCRCWord;
      wCRCWord >>= 8;
      wCRCWord ^= wCRCTable[nTemp];
    }
    return wCRCWord&0xffff;
}
// 开启电泵
// 波特率为9600，无校验位，8位数据位，1位停止位
// @return {HAL_ERROR} {HAL_TIMEOUT} {HAL_OK}
// 0x01 0x06 0x20 0x00 0x00 0x05 0x42 0x09
HAL_StatusTypeDef StartInverter()
{
    //See EDS1000 manual P121 for detailed instruction format
    unsigned char cmd[8] = {ADDR_INVERTER, 0x06, 0x20, 0x00, 0x00, 0x05, 0x00, 0x00};
    unsigned short CRCCode = CRCCodeTest(cmd, 6);    //  这个
    cmd[6] = CRCCode&0xff;
    cmd[7] = (CRCCode>>8)&0xff;
    return HAL_UART_Transmit(&huart3,cmd,8,100);
}

// 0x20001A0C "h ??add 13,0,30"

// 关闭电机
// @return {HAL_ERROR} {HAL_TIMEOUT} {HAL_OK}
// 0x01, 0x06, 0x20, 0x00, 0x00, 0x06, 0x02, 0x08
HAL_StatusTypeDef StopInverter()
{
    //See EDS1000 manual P121 for detailed instruction format
    unsigned char cmd[] = {ADDR_INVERTER, 0x06, 0x20, 0x00, 0x00, 0x06, 0x00, 0x00};
    unsigned short CRCCode = CRCCodeTest(cmd, 6);
    cmd[6] = CRCCode&0xff;
    cmd[7] = (CRCCode>>8)&0xff;
    return HAL_UART_Transmit(&huart3,cmd,8,100);
}

float frequency=30;

float getFrequency()
{
    return frequency;
}

void setFrequency(float req)
{
    frequency=req;
}

// 设置电机频率
// @return {HAL_ERROR} {HAL_TIMEOUT} {HAL_OK}
// 30Hz指令   01 06 20 01 0B B8 D4 88
HAL_StatusTypeDef SetInverterFreq(float freq)
{
    //See EDS1000 manual P121 for detailed instruction format
    unsigned char cmd[] = {ADDR_INVERTER, 0x06, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00};
    unsigned short freq_int = (unsigned short)(freq*100);
    cmd[4] = (freq_int>>8)&0xff;
    cmd[5] = freq_int&0xff;
    unsigned short CRCCode = CRCCodeTest(cmd, 6);
    cmd[6] = CRCCode&0xff;
    cmd[7] = (CRCCode>>8)&0xff;
    HAL_StatusTypeDef state=HAL_UART_Transmit(&huart3,cmd,8,100);
    if(state==HAL_OK) frequency=freq;
    return state;
}

/*****************************************************************************/
float _vortexTotalFlow = 0;
float _vortexFlowrate = 0;

// 初始状态 0  已发送读取数据 1 读取到了数据 2  发送数据 3
int vortexState = 0;
// vortex接收中断的数据
char vortexRxBuffer[30]; 
char vortexData[30];

// 获取涡街流量计的数据 Triggers a read operation, refresh the flowrate and totalFlowrate，主函数中一直循环每隔一段时间读取涡街流量计的数据
// @return {HAL_ERROR} {HAL_TIMEOUT} {HAL_OK}
// #define ADDR_FLOWMETER  0x02
// CRC检验后的16进制字符02 04 00 00 00 04 f1 fa
// 返回的数据：02 04 08 00 00 25 00 01 F5 74 41 DA 82
void GetVortexData()
{
    unsigned char cmd[8] = {0x02, 0x04, 0x00, 0x00, 0x00, 0x04, 0xf1, 0xfa};
    //unsigned short CRCCode = CRCCodeTest(cmd, 6);
    //cmd[6] = CRCCode&0xff;
    //cmd[7] = (CRCCode>>8)&0xff;
    // 发送读取涡街流量计的数据
    HAL_UART_Transmit(&huart3,cmd,8,100);
}

// 解析涡街流量计读取的数据，ret为返回的数据
int AnalyVortexData(uint8_t ret[30]){
        int flowRate = 0;
        int totalFlow = 0;
        int i = 0;
        //Find the frame start
        for(i=0; i<30; i++)
        {
            if(ret[i]==ADDR_FLOWMETER && ret[i+1]==0x04)break;
        }
        if(i>=20)return -1;//Can't find the framestart，error

        //Convert hexadecimal data to decimal and adjust mantissa
        //See manual for detail
        // 提取帧数据
        flowRate = ret[i+3];
        flowRate<<=8;
        flowRate += ret[i+4];
        switch(ret[i+5]&0xf0)
        {
            case 0x00:_vortexFlowrate = (float)flowRate;break;
            case 0x10:_vortexFlowrate = flowRate*0.10f;break;
            case 0x20:_vortexFlowrate = flowRate*0.01f;break;
            default:  _vortexFlowrate = flowRate*0.001f;break;
        }

        totalFlow =  ret[i+6];
        totalFlow<<=8;
        totalFlow += ret[i+7];
        totalFlow<<=8;
        totalFlow += ret[i+8];
        totalFlow<<=8;
        totalFlow += ret[i+9];
        switch(ret[i+10]&0xf0)
        {
            case 0x00:_vortexTotalFlow = (float)totalFlow*0.10f;break;
            case 0x10:_vortexTotalFlow = totalFlow*0.010f;break;
            case 0x20:_vortexTotalFlow = totalFlow*0.001f;break;
            default:  _vortexTotalFlow = totalFlow*0.0001f;break;
        }
        return 0;
}

// 获取涡街流量计总的流量
float getVortexTotalFlow(){
    return _vortexTotalFlow;
}

// 获取涡街流量计总的速率
float getVortexFlowrate(){
    return _vortexFlowrate;
}

// 这个函数应该放在主循环中
// 涡街流量计采集数据
// 涡街流量计最大21ms
void loopVortex(){
    // uint8_t vortesData[30]={0};
    
	// 发送获取数据成功
	//huart3.RxXferCount = 8;
	//huart3.pRxBuffPtr = vortesData;
    GetVortexData();
    HAL_Delay(50);
        // 数据接收成功,数据类型如下
    // 02 04 08 25 01 F5 51 41 3F 12 02 04
    if(receive3[0]==0x02 && receive3[1]==0x04)
    {
        // 接收的数据不为空
		// char recData[30] = vortesData;
        // 解析数据,返回0说明解析数据成功
        AnalyVortexData(receive3);
    }
    // 复原串口3
    for(int i=0;i<30;i++)
    {
        receive3[i] = 0;
    }
    huart3.RxXferCount = 30;
    huart3.pRxBuffPtr = receive3;
    open3Receive();
}

// 画涡街流量计的数据
void drawVortex()
{
    int flowrate3=0;
    int totalRate3=0;
    char buf3rate[64];
    char buf3total[64];
    // 转化为整数
    flowrate3 = _vortexFlowrate/1;
    // 转化为整数
    totalRate3 = _vortexTotalFlow/1;

    // 发送涡街流量计速率
    sprintf(buf3rate,"add 13,0,%d",flowrate3);
    uint8_t counterR = 0;
    while(1)
    {
        if(buf3rate[counterR] == '\0')
        {
            break;
        }
        counterR ++;
    }
    HAL_UART_Transmit(&huart1,(uint8_t *)buf3rate,counterR,0xff);
    UART_Send_END();

    // 发送涡街流量计总流量
    sprintf(buf3total,"add 13,1,%d",totalRate3);
    uint8_t counterTotal = 0;
    while(1)
    {
        if(buf3total[counterTotal] == '\0')
        {
            break;
        }
        counterTotal ++;
    }
    HAL_UART_Transmit(&huart1,(uint8_t *)buf3total,counterTotal,0xff);
    UART_Send_END();

    /* 发送到wifi模块
    // 如果解析后有_vortexTotalFlow数据
    char totalFlowChar[16];
    sprintf(totalFlowChar,"%g",_vortexTotalFlow);
    char totalFlow[25] = "TotalFlow:";
    sendData(totalFlow,totalFlowChar,100);
    // 发送获取的数据
    char flowRateChar[16];
    sprintf(flowRateChar,"%g",_vortexFlowrate);
    char flowRate[25] = "FlowRate:";
    sendData(flowRate,flowRateChar,100);
    */
}

// 这个函数应该放在主循环中
/*  中断接收
void loopVortex(){
    if(vortexState==0){
        // 去获取数据
        GetVortexData();
        vortexState = 1;
    } else if(vortexState==1){
        // 查询是否获取到了数据
        unsigned char numOfBuffer;
        // 接收到数据则执行
        if(huart3.RxXferCount < 30) {
            numOfBuffer = 30-huart3.RxXferCount;   // 实际接收到的字符总数
            vortexData = vortexRxBuffer;
            // 这里发送到单片机去解析数据
            huart3.RxXferCount = 30;
            huart3.pRxBuffPtr = vortexRxBuffer;
             操作缓冲数组，暂停接受中断
            __HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE);
             vortexState = 2;
        }
    } else if(vortexState == 2){   // 读取到了数据
        if(strlen(vortexData)<=0){
            // 数据为空，重新发送获取数据
            vortexState = 0;
            return;
        } else {
            // 解析数据
            AnalyVortexData(vortexData);
            if(_vortexTotalFlow){
                char totalFlowChar;
                sprintf(totalFlowChar,"%g",_vortexTotalFlow)
                sendData("TotalFlow",totalFlowChar);
                _vortexTotalFlow = 0;
            }
            if(_vortexFlowrate){
                char flowRateChar;
                sprintf(flowRateChar,"%g",_vortexFlowrate)
                sendData("TotalFlow",flowRateChar);
                _vortexFlowrate=0;
            }
            vortexState = 0;
        }
    }
}  */

// extern static void MX_USART3_UART_Init_2400(void);
// extern static void MX_USART3_UART_Init(void);
// int ReadHeatMeter_state = 0;   // 初始状态
// NBDC_delayTimeStruct ReadHeatMeter_450;
char heatMeterRxBuffer[100];
// 读取热能表的数据,采集一次耗时300ms
// @return {HAL_ERROR} {HAL_TIMEOUT} {HAL_OK}
// 计算完成后cmd的指令为0x68,  0x20,  0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,  0x01,  0x03,  0x1f, 0x90, 0x23,  0x04,  0x16
void readHeatMeter()
{
    //int i = 0;
    //   See CJ/T 188-2004 for M-Bus standatds
    //  |START| Heatmeter |----------Broadcast ADDRESS--------|CTR_0|Length|Read measurement|Parity|END|
    unsigned char tecmd[] = {0x68, 0x20, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x01, 0x03, 0x1f, 0x90, 0x23, 0x04, 0x16};

    // 0x68, 0x20, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x3F, 0x01, 0x03, 0x1f, 0x3F, 0x23, 0x04, 0x16, 0x03, 0xF8

    //Calculate parity
    // for(i=0; i<14; i++)
    // {
    //     cmd[14]+=cmd[i];
    // }

    // Set baudrate to 2400, EVEN parity, for M-Bus设置波特率2400,奇校验
    // uart3_change_baudrate_2400();
    HAL_UART_Transmit(&huart6,(uint8_t *)tecmd,sizeof(tecmd),0xffff);
}

// 解析热能表的数据   ret热能表读取的数据结果
struct HeatData AnanlyReadHeatMeterData(uint8_t ret[100]){
        int i = 0;
        int frameStart = 0;
        char parity = 0;
        struct HeatData result;
        result.currentFlow = -1;
        result.temperature = -1;
        result.totalFlow = -1;

        //Finding the frame start
        for(i=0; i<64; i++)
        {
            if(ret[i]==0x68)break;
        }
        if(i>20) return result;

        frameStart = i;
        parity = 0;
        for(i=0; i<57; i++)
        {
            parity+=ret[frameStart+i];
        }

        result.currentFlow = BCD(ret[frameStart+32])*100.0f + BCD(ret[frameStart+31])*1.0f + BCD(ret[frameStart+30])*0.01f + BCD(ret[frameStart+29])*0.0001f;
        result.totalFlow = BCD(ret[frameStart+37])*10000.0f + BCD(ret[frameStart+36])*100.0f + BCD(ret[frameStart+35])*1.0f + BCD(ret[frameStart+34])*0.01f;
        result.temperature = BCD(ret[frameStart+41])*100.0f + BCD(ret[frameStart+40])*1.0f + BCD(ret[frameStart+39])*0.01f;
        return result;
}

// 初始状态 0  已发送读取数据 1 读取到了数据 2  发送数据 3
int heatMetweState=0;
// vortex接收中断的数据
char heatMeterData[100];

struct HeatData HeatMeterData;
void loopHeatMeter(){
    //uint8_t meterData[100];
    readHeatMeter();
    HAL_Delay(300);
        // 数据接收成功,数据类型如下
    // 02 04 08 25 01 F5 51 41 3F 12 02 04
    if(receive6[0]!=0)
    {
        HeatMeterData=AnanlyReadHeatMeterData(receive6);
    }
    // 复原串口3
    for(int i=0;i<100;i++)
    {
        receive6[i] = 0;
    }
    huart6.RxXferCount = 100;
    huart6.pRxBuffPtr = receive6;
    open6Receive();
    // uint8_t ret[100] = heatMeterData;
    // 解析热能表数据

    // 发送数据
    /*
    if(HeatMeterData.currentFlow>=0){
        char flowChar[16]={0};
        sprintf(flowChar,"%g",HeatMeterData.currentFlow);
        sendData("FlowRateHM:",flowChar,100);
    }
    if(HeatMeterData.temperature>=0){
        char flowTemperature[20]={0};
        sprintf(flowTemperature,"%g",HeatMeterData.temperature);
        sendData("TemperatureHM:",flowTemperature,100);
    }
    if(HeatMeterData.totalFlow>=0){
        char flowTotal[20]={0};
        sprintf(flowTotal,"%g",HeatMeterData.totalFlow);
        sendData("TotalFlowHM:",flowTotal,100);
    }
    */
}

// 获取热能表数据
struct HeatData getHeatData()
{
    return HeatMeterData;
}


void drawHeatCurrentFlow()
{
    char bufcurrent[64];
    int currentFlow=(int)HeatMeterData.currentFlow;
    if(currentFlow!=-1){
        sprintf(bufcurrent,"add 13,0,%d",currentFlow);
        uint8_t counterTime = 0;
        while(1)
        {
            if(bufcurrent[counterTime] == '\0')
            {
                break;
            }
            counterTime ++;
        }
        HAL_UART_Transmit(&huart1,(uint8_t *)bufcurrent,counterTime,0xffff);
        UART_Send_END();
    }
}

void drawHeattotalFlow()
{
    char buftotal[64];
    int totalFlow=(int)HeatMeterData.totalFlow;
    if(totalFlow!=-1){
        sprintf(buftotal,"add 13,1,%d",totalFlow);
        uint8_t counterTime = 0;
        while(1)
        {
            if(buftotal[counterTime] == '\0')
            {
                break;
            }
            counterTime ++;
        }
        HAL_UART_Transmit(&huart1,(uint8_t *)buftotal,counterTime,0xffff);
        UART_Send_END();
    }
}

void drawHeatTemperature()
{
    char buftemperature[64];
    int temperature=(int)HeatMeterData.temperature;
    if(temperature!=-1){
        sprintf(buftemperature,"add 13,2,%d",temperature);
        uint8_t counterTime = 0;
        while(1)
        {
            if(buftemperature[counterTime] == '\0')
            {
                break;
            }
            counterTime ++;
        }
        HAL_UART_Transmit(&huart1,(uint8_t *)buftemperature,counterTime,0xffff);
        UART_Send_END();
    }
}

void drawHeatMeter()
{
    drawHeatCurrentFlow();
    drawHeattotalFlow();
    drawHeatTemperature();
}

// 这个函数应该放在主循环中,查询热能表数据
/*
void loopHeatMeter(){
    if(heatMetweState==0){
        // 去获取数据
        ReadHeatMeter();
        heatMetweState = 1;
    } else if(heatMetweState==1){
        // 查询是否获取到了数据
        unsigned char numOfBuffer;
        // 接收到数据则执行
        if(huart3.RxXferCount < 100) {
            numOfBuffer = 100-huart3.RxXferCount;   // 实际接收到的字符总数
            heatMeterData = heatMeterRxBuffer;
            // 这里发送到单片机去解析数据
            huart3.RxXferCount = 100;
            huart3.pRxBuffPtr = heatMeterRxBuffer;
            !操作缓冲数组，暂停接受中断
            __HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE);
             heatMetweState = 2;
        }
    } else if(heatMetweState == 2){   // 读取到了数据
        if(strlen(heatMeterData)<=0){
            // 数据为空，重新发送获取数据
            heatMetweState = 0;
            return;
        } else {
            // 解析数据
            struct HeatData HeatMeterData = AnanlyReadHeatMeterData(heatMeterData);
            if(HeatMeterData.currentFlow>=0){
                char flowChar;
                sprintf(flowChar,"%g",HeatMeterData.currentFlow)
                sendData("TotalFlowHM",flowChar);
            }
            if(HeatMeterData.temperature>=0){
                char flowTemperature;
                sprintf(flowTemperature,"%g",HeatMeterData.temperature)
                sendData("TemperatureHM",flowTemperature);
            }
            if(HeatMeterData.totalFlow>=0){
                char flowTotal;
                sprintf(flowTotal,"%g",HeatMeterData.totalFlow)
                sendData("TotalFlowHM",flowTotal);
            }
            heatMetweState = 0;
        }
    }
}  */
