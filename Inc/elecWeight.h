#ifndef SERIALDEVICES_H
#define SERIALDEVICES_H

#include "stm32f7xx_hal.h"

//------------------------------------电子秤操作函数------------------------------

//Entry for reading Electronic Weight
    //Return -100.00 means error
HAL_StatusTypeDef GetWeightValue();

float analyWeightValue(char rawData[20]);

void loopWeight();

HAL_StatusTypeDef SetWeightZero();

// SERIALDEVICES_H
#endif 
