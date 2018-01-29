	#include "mySleep.h"
	#include "tim.h"

	#define  NBDC_MAXIUMDELAYCOUNTERVAL 4294967295 /*!2^32 - 1*/
	#define  NBDC_INTERRUPTIONPERIOD  10     /*! in ms, interruption period*/

	#ifdef NBDC_DEBUG
	unsigned char NBDC_MaxInUseTimer = 0;
	#endif

	uint32_t NBDC_delayCounter;

	uint32_t NBDC_inUsingDelayTimer = 0x00000000;/*<Timer In using will be set to 1*/
	uint32_t NBDC_overRoundTimer    = 0x00000000;/*<Timer counter is full for a round*/

	NBDC_delayTimeStruct NBDC_delayTimeForBlockedStruct;
	/**
	* @brief  Delay for n ms
	* @param  data : delay time in ms
	* @retval None
	*/
	sysStatus NBDC_delayMS(unsigned int data)
	{
		sysStatus applyState = NBDC_startADelayClock(&NBDC_delayTimeForBlockedStruct, data);
		if(applyState == sysOK)
		{
				while(1)
				{
						if(NBDC_queryADelayClock(&NBDC_delayTimeForBlockedStruct) == NBDC_DELAY_FINISH)
						{
								break;
						}
				}
				return sysOK;
		}
		else return sysFAIL;
	}

	/**
	* @brief  Initialize a non-blocking delay clcok timer
	* @note   Max delay time is @ref NBDC_MAXIUMDELAYCOUNTERVAL
	* @param  storeStruct:Structure to store timer information
	* @param  delayLength:set time (ms)
	* @retval sysStatus:Initialization state
	*/
	// 初始化定时器
	sysStatus NBDC_startADelayClock(NBDC_delayTimeStruct *storeStruct, uint32_t delayLength)
	{
		uint32_t temp;
		uint32_t temp2;
		delayLength /= NBDC_INTERRUPTIONPERIOD;
		if(delayLength == 0) delayLength = 1;
		if(storeStruct->isBusy == 1)/*!����ʱ���Ѿ���ʼ�����һ���ʹ����ʱ���ܽ��г�ʼ��*/
		{
				return sysBUSY;
		}
	else
	{
		storeStruct->startTime = NBDC_delayCounter;
		temp = storeStruct->startTime + delayLength;
		if(temp > NBDC_MAXIUMDELAYCOUNTERVAL)/*!��������ʱ��ʱ�䳬����׼��ʱ��������ֵʱ*/
		{
				storeStruct->overARound = 1;
				storeStruct->endTime = temp - NBDC_MAXIUMDELAYCOUNTERVAL;
		}
		else
		{
				storeStruct->overARound = 0;
				storeStruct->endTime = (uint32_t) temp;
		}
		storeStruct->isBusy = 1;

		temp2 = 1;
		for(temp = 0; temp < 32; temp++) /*!��¼��ʹ�ö�ʱ�������ɶ�ʱ����ID*/
		{
				if((NBDC_inUsingDelayTimer & temp2) == 0)
				{
						break;
				}
				else
				{
						temp2 = temp2 << 1;
				}
		}

		if(temp < 32)
		{
				NBDC_inUsingDelayTimer |= temp2;
				storeStruct->uniqueTimeNum = temp2;
				return sysOK;
		}
		else
		{
			 return sysFAIL;/*!Can't allocate more timer*/
		}
	}
	}

	/**
	* @brief  ǿ��ֹͣ/��ʼ��һ���������Ե���ʱ��ʱ��
	* @note   ��ʱ��ʱ��������ֵ������ʼ��
	* @param  storeStruct:���ڴ������ض�ʱ�����������ݽṹ��ָ��
	* @retval sysStatus:�Ƿ����óɹ�
	*/
	sysStatus NBDC_stopADelayClock(NBDC_delayTimeStruct *storeStruct)
	{
		storeStruct->isBusy = 0;
		storeStruct->startTime = 0;
		storeStruct->endTime   = 0;
		storeStruct->overARound = 0;
		NBDC_inUsingDelayTimer &= ~(storeStruct->uniqueTimeNum);
		NBDC_overRoundTimer    &= ~(storeStruct->uniqueTimeNum);
		return sysOK;
	}

	/**
	* @brief  ��ѯ��ʱ�Ƿ�����
	* @param  storeStruct:���ڴ������ض�ʱ�����������ݽṹ��ָ��
	* @retval NBDC_timerState: ���ض�ʱ״̬
	*/
	NBDC_timerState NBDC_queryADelayClock(NBDC_delayTimeStruct *storeStruct)
	{
	uint32_t timerID;
	if(storeStruct->isBusy == 0) /*!������ʱ����δ����ʼ��*/
	{
			return NBDC_DELAY_UNUSED;
	}
	else
	{
			timerID = storeStruct->uniqueTimeNum;
			if(storeStruct->overARound == 0)
			{
					if((NBDC_overRoundTimer & timerID) == 0)/*!�˶�ʱ������ֵ����������Ȧ�����⣬�Ҵ�ʱ�ڶ�ʱ�������ڼ䲻�����ż���������������*/
					{
							if(NBDC_delayCounter < (storeStruct->endTime))
							{
									return NBDC_DELAY_UNFINISH;
							}
							else
							{
									NBDC_stopADelayClock(storeStruct);
									return NBDC_DELAY_FINISH;
							}
					}
					else/*!�˶�ʱ������ֵ����������Ȧ�����⣬�����ڶ�ʱ�������ڼ������ż���������������*/
					{
							NBDC_stopADelayClock(storeStruct);
							return NBDC_DELAY_FINISH;/*!��Ϊ�������Ѿ����������Ƕ�ʱ��������ֵû����Ȧ�����������Դ�ʱ��ʱֵ�Ѿ����������õļ���ֵ*/
					}
			}
			else
			{
					if((NBDC_overRoundTimer & timerID) == 0)/*!��ʱ��ʱ��������ֵ������Ȧ�����⣬�����ڶ�ʱ�������ڼ䲻�����ż���������������*/
					{
							return NBDC_DELAY_UNFINISH;/*!��ʱ��������ֵ�Ѿ���Ȧ�ˣ����Ǽ�������ֵδ���������Դ�ʱ��ʱ��������ֵ��δ��*/
					}
					else/*!��ʱ��ʱ�������ô�������Ȧ�����⣬���Ҽ�������ֵ�Ѿ�������*/
					{
							if(NBDC_delayCounter < (storeStruct->endTime))
							{
									return NBDC_DELAY_UNFINISH;
						  }
							else
							{
									NBDC_stopADelayClock(storeStruct);
									return NBDC_DELAY_FINISH;
							}
					}
			}
	}
	}

// 查询定时器
	NBDC_timerState NBDC_queryADelayClockWithoutStop(NBDC_delayTimeStruct *storeStruct)
	{
			uint32_t timerID;
			if(storeStruct->isBusy == 0) /*!������ʱ����δ����ʼ��*/
	{
			return NBDC_DELAY_UNUSED;
	}
	else
	{
			timerID = storeStruct->uniqueTimeNum;
			if(storeStruct->overARound == 0)
			{
					if((NBDC_overRoundTimer & timerID) == 0)/*!�˶�ʱ������ֵ����������Ȧ�����⣬�Ҵ�ʱ�ڶ�ʱ�������ڼ䲻�����ż���������������*/
					{
							if(NBDC_delayCounter < (storeStruct->endTime))
							{
									return NBDC_DELAY_UNFINISH;
							}
							else
							{
									//NBDC_stopADelayClock(storeStruct);
									return NBDC_DELAY_FINISH;
							}
					}
					else/*!�˶�ʱ������ֵ����������Ȧ�����⣬�����ڶ�ʱ�������ڼ������ż���������������*/
					{
							//NBDC_stopADelayClock(storeStruct);
							return NBDC_DELAY_FINISH;/*!��Ϊ�������Ѿ����������Ƕ�ʱ��������ֵû����Ȧ�����������Դ�ʱ��ʱֵ�Ѿ����������õļ���ֵ*/
					}
			}
			else
			{
					if((NBDC_overRoundTimer & timerID) == 0)/*!��ʱ��ʱ��������ֵ������Ȧ�����⣬�����ڶ�ʱ�������ڼ䲻�����ż���������������*/
					{
							return NBDC_DELAY_UNFINISH;/*!��ʱ��������ֵ�Ѿ���Ȧ�ˣ����Ǽ�������ֵδ���������Դ�ʱ��ʱ��������ֵ��δ��*/
					}
					else/*!��ʱ��ʱ�������ô�������Ȧ�����⣬���Ҽ�������ֵ�Ѿ�������*/
					{
							if(NBDC_delayCounter < (storeStruct->endTime))
							{
									return NBDC_DELAY_UNFINISH;
							}
							else
							{
									//NBDC_stopADelayClock(storeStruct);
									return NBDC_DELAY_FINISH;
							}
					}
			}
	}
	}
	/**
	* @brief  ��׼��ʱ����ֵ
	* @param  None
	* @retval None
	*/
	void NBDC_delayTimerInc()
	{
	if(NBDC_delayCounter == NBDC_MAXIUMDELAYCOUNTERVAL)/*!����׼��ʱ��������ˢ������ʹ���еĶ�ʱ���ı�־λ*/
	{
			NBDC_overRoundTimer = NBDC_inUsingDelayTimer;
	}

	NBDC_delayCounter++;

	#ifdef NBDC_DEBUG
	unsigned char counter;
	unsigned char timerNumberCounter = 0;
	unsigned int bitRead = 1;
	for(counter = 0; counter < 32; counter++)
	{
			if(NBDC_inUsingDelayTimer & bitRead) timerNumberCounter++;
			bitRead = bitRead << 1;
	}
	if(timerNumberCounter > NBDC_MaxInUseTimer) NBDC_MaxInUseTimer = timerNumberCounter;
	
	#endif
	}
