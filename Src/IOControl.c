#include <stdio.h>
#include <stdlib.h>
#include "stm32f7xx_hal.h"

//All switch outputs are on GPF
//Return non-zero value means error
// 设置开关控制电磁阀
// n 哪一个电磁阀   从下往上1~5
// state 0 关闭    1 打开
void setRelayState(int n, int state)
{
    GPIO_PinState pin_state;
    // true为开
    if(state) pin_state = GPIO_PIN_SET;
    else pin_state = GPIO_PIN_RESET;
    // int rState = -1;
    // 开关顺序从1~5为K1,K2,K3,K5,K4
    switch(n)
    {
        case 0:
        break;
        case 1:
             HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,pin_state);
        break;
        case 2:
             HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,pin_state);
        break;
        case 3:
             HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,pin_state);
        break;
        case 4:
             HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,pin_state);
        break;
        case 5:
             HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,pin_state);
        break;
        default:break;
    }
    // if(rState!=-1)relayState[n] = state;
    // return rState;
}

// 读取引脚状态，读取开关状态  n 引脚的编号
GPIO_PinState getRelayState(int n){
    switch(n)
    {
        case 1:
             return HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4);
        case 2:
             return HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5);
        case 3:
             return HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_6);
        case 4:
             return HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8);
        case 5:
             return HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7);
    }
}

