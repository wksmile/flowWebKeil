#ifndef KKRS485_H
#define KKRS485_H

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"
#include "stm32f7xx_hal_def.h"
#include "stdio.h"
#include "RS485.h"
#include "tcpServer.h"

//RS485 addresses for different modules
#define ADDR_INVERTER   0x01
#define ADDR_FLOWMETER  0x02
#define ADDR_THERMO     0x03

struct HeatData
{
    float totalFlow;
    float currentFlow;
    float temperature;
};

#define BCD(data) (10*(((unsigned char)data)>>4) + (((unsigned char)data)&0x0f))

HAL_StatusTypeDef StartInverter(void);
HAL_StatusTypeDef StopInverter(void);
HAL_StatusTypeDef SetInverterFreq(float freq);

void open6Receive(void);
void open3Receive(void);
// 获取涡街流量计总的流量
float getVortexTotalFlow(void);
// 获取涡街流量计总的速率
float getVortexFlowrate(void);

// 涡街流量计读取数据
void loopVortex(void);

// 画涡街流量计的数据
void drawVortex(void);

// 热能表读取数据
void loopHeatMeter(void);

// 获取热能表数据,返回结构体
struct HeatData getHeatData(void);

void readHeatMeter(void);

void drawHeatMeter(void);

// 返回电机频率
float getFrequency(void);

void setFrequency(float req);

// SERIALDEVICES_H
#endif
