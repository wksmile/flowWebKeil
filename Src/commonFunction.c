#include "nonBlockingModeDelayClock.h"

// 师兄代码文件
uint8_t COMM_EnableFilker = 0;
uint8_t COMM_LEDState = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
        if (htim == &htim2) /*!Timer for nonBlocking Mode Delay Clock, trigged per 10ms*/
        {
                NBDC_delayTimerInc();
        }
        else if (htim == &htim1) /*!Fan Speed control function, trigger per 1s*/
        {
                FAN_PIDProcess();
                if (COMM_EnableFilker)
                {
                        BOARDLED0(COMM_LEDState);
                        COMM_LEDState = ~COMM_LEDState;
                }
        }
}