#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"
#include "stm32f7xx_hal_def.h"

//------------------------------------超声波液位计函数------------------------------
void open5Receive(void);

// 放在主函数中循环运行的函数
void loopUltra(void);

// 循环获取串口5的中断数据
void checkUart5Receive(void);

// 超声波液位计获取数据的函数
void ultraData(void);

//获得超声波液位计测量距离
int getUlDistance(void);

// 获取超声波液位计的温度
float getUlTemperature(void);

// 外部获取超声波液位计时间
int getUlTime(void);

// 在触摸屏中画超声波液位计测量的距离曲线
void drawUltraDistance(void);

// 在触摸屏中画超声波液位计测量的温度曲线
void drawUltraTemperature(void);

// 在触摸屏中画超声波液位计测量的超声波时间曲线
void drawUltraTime(void);

// 超声波液位计曲线绘制
void drawUltra(void);
#endif
