#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"
#include "stm32f7xx_hal_def.h"

//------------------------------------超声波液位计函数------------------------------

HAL_StatusTypeDef GetUltraTemperature();
// 解析并返回读取的超声波数据
float anaslyUltraTemperature(char Data[5]);

// 超声波距离
HAL_StatusTypeDef GetUltraDistance();
int anaslyUltraDistance(char rawData[5]);

HAL_StatusTypeDef GetUltraDurationTime();
int anaslyUltraTime(char rawData[5]);

// 放在主函数中循环运行的函数
void loopUltra();


#endif