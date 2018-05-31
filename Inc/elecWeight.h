#ifndef SERIALDEVICES_H
#define SERIALDEVICES_H

#include "stm32f7xx_hal.h"

//------------------------------------电子秤操作函数------------------------------
void loopWeight(void);

void SetWeightZero(void);

// 获取电子秤重量kg
float getWeight(void);

// 画电子秤的曲线
void drawWeight();
// SERIALDEVICES_H

//获取电子秤数据
void GetWeightValue(void);
#endif 
