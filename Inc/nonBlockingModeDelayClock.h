#ifndef _NONBLOCKING_H_
#define _NONBLOCKING_H_

#include "stm32f4xx_hal.h"
#include "commonFunction.h"

//#define NBDC_DEBUG /*!If defined NBDC_DEBUG, Max number of in-use timer will be recorded*/

typedef struct
{
        uint32_t startTime;
        uint32_t endTime;
        uint8_t overARound;
        uint8_t isBusy;
        uint32_t uniqueTimeNum;
}NBDC_delayTimeStruct;

typedef enum
{
        NBDC_DELAY_FINISH,
        NBDC_DELAY_UNFINISH,
        NBDC_DELAY_UNUSED
}NBDC_timerState;

sysStatus NBDC_delayMS(unsigned int data);
sysStatus NBDC_startADelayClock(NBDC_delayTimeStruct *storeStruct, uint32_t delayLength);
sysStatus NBDC_stopADelayClock(NBDC_delayTimeStruct *storeStruct);
NBDC_timerState NBDC_queryADelayClock(NBDC_delayTimeStruct *storeStruct);
NBDC_timerState NBDC_queryADelayClockWithoutStop(NBDC_delayTimeStruct *storeStruct);

void NBDC_delayTimerInc(void);

#ifdef NBDC_DEBUG
extern unsigned char NBDC_MaxInUseTimer;
#endif

#endif
