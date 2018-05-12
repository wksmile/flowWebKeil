#ifndef SERIALDEVICES_H
#define SERIALDEVICES_H

#include "stm32f7xx_hal.h"

//------------------------------------电子秤操作函数------------------------------

//Entry for reading Electronic Weight
    //Return -100.00 means error
void GetWeightValue(void);

float analyWeightValue(uint8_t rawData[20]);

void loopWeight(void);

HAL_StatusTypeDef SetWeightZero(void);

// SERIALDEVICES_H
#endif 
