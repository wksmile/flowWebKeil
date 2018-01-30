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

#define BCD(data) (10*(((unsigned char)data)>>4) + (((unsigned char)data)&0x0f))

HAL_StatusTypeDef StartInverter(void);
HAL_StatusTypeDef StopInverter(void);
HAL_StatusTypeDef SetInverterFreq(float freq);

// 获取涡街流量计总的流量
float getVortexTotalFlow(void);
// 获取涡街流量计总的速率
float getVortexFlowrate(void);

// 涡街流量计读取数据
void loopVortex(void);
// 热能表读取数据
void loopHeatMeter(void);

// SERIALDEVICES_H
#endif
