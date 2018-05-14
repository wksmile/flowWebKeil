#include <string.h>
#include <ctype.h>
#include "RS485.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"
#include "stm32f7xx_hal_def.h"
#include "tcpServer.h"
#include "IOControl.h"
#include "ultrasonic.h"
#include "elecWeight.h"
#include "RS485.h"
#include "cJSON.h"

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;

extern uint8_t wifiRxBuffer[20];

#define OVERFLOW_WEIGHT 20//The maximum weight upper tank could hold. If reach this threshold, pump will sop and valveout will open.

// 开始的电机频率
int inverterFrequency = 30;
// 电机开始状态
uint8_t inverterStatus = 0;
// SensorValues sv;

// 解析接收数据，发送读取的数据
//  key 形式类似于"Temperature:"
// sendData("Temperature:","38");
// 中断发送

void sendDataIT(char key[],char value[]){
	//uint8_t TCPPack[25];
	strcat(key, value);
	HAL_UART_Transmit_IT(&huart4,(uint8_t *)key,strlen(key));
}

// 普通发送，timeout一般设为100ms
void sendData(char key[],char value[],uint8_t timeout){
	strcat(key, value);
	HAL_UART_Transmit(&huart4,(uint8_t *)key,strlen(key),timeout);
}

// 发送数据到触摸屏
void sendDataTouchScreen(char key[],char value[],uint8_t timeout){
    strcat(key, value);
    HAL_UART_Transmit(&huart2,(uint8_t *)key,strlen(key),timeout);
}

/*
void SLOT_ProcessSensorData()
{
    sv = sensorValues;
    sv.totalFlow-=totalFlow0;//Subtract the original total flow
    sv.totalFlowHM-=totalFlowHM0;//Subtract the original total flow
//  QString 是字符串？？？？？？？？？？？？？
    chat *ptr;  // 数字转换为字符串后参数buf指针所指的空间
    unsigned char TCPPack[250];

    char Temperature[] = "Temperature:";
    strcat( TCPPack, Temperature);
    gcvt(sv.temperature,3,ptr);
    strcat( TCPPack, *ptr);

    char Ultrasonic[] = "\r\nUltrasonic:";
    strcat( TCPPack, Ultrasonic);
    itoa(sv.ultrasonicTime,5,ptr);
    strcat( TCPPack, *ptr);

    char Distance[] = "\r\nDistance:";
    strcat( TCPPack, Distance);
    itoa(sv.waterLevel,5,ptr);
    strcat( TCPPack, *ptr);

    char FlowRate[] = "\r\nFlowRate:";
    strcat( TCPPack, FlowRate);
    gcvt(sv.flowRate,3,ptr);
    strcat( TCPPack, *ptr);

    char TotalFlow[] = "\r\nTotalFlow:";
    strcat( TCPPack, TotalFlow);
    gcvt(sv.totalFlow,3,ptr);
    strcat( TCPPack, *ptr);

    char Weight[] = "\r\nWeight:";
    strcat( TCPPack, Weight);
    gcvt(sv.weight,4,ptr);
    strcat( TCPPack, *ptr);

    char FlowRateHM[] = "\r\nFlowRateHM:";
    strcat( TCPPack, FlowRateHM);
    gcvt(sv.flowRateHM,4,ptr);
    strcat( TCPPack, *ptr);

    char TotalFlowHM[] = "\r\nTotalFlowHM:";
    strcat( TCPPack, TotalFlowHM);
    gcvt(sv.totalFlowHM,5,ptr);
    strcat( TCPPack, *ptr);

    char TemperatureHM[] = "\r\nTemperatureHM:";
    strcat( TCPPack, TemperatureHM);
    gcvt(sv.temperatureHM,4,ptr);
    strcat( TCPPack, *ptr);

    char ValveIn[] = "\r\nValveIn:";
    strcat( TCPPack, ValveIn);
    *ptr = getRelayState(4)?"on":"off";
    strcat( TCPPack, *ptr);

    char ValveOut[] = "\r\nValveOut:";
    strcat( TCPPack, ValveOut);
    *ptr = getRelayState(3)?"on":"off";
    strcat( TCPPack, *ptr);

    char ValveSide[] = "\r\nValveSide:";
    strcat( TCPPack, ValveSide);
    *ptr = getRelayState(1)?"on":"off";
    strcat( TCPPack, *ptr);

    char Inverter[] = "\r\nInverter:";
    strcat( TCPPack, Inverter);
    *ptr = inverterStatus?"on":"off";
    strcat( TCPPack, *ptr);

    char *ptr = "\r\n";
    strcat( TCPPack, *ptr);

   // 这里需要发送tcp的包
    // server.Send(TCPPack);
// print the tcp send data to debug
    printf("Weight: %f kg\n",sv.weight);  
    printf("Temperature: %f C\n",sv.temperature); 
    printf("Ultrasonic: %f\n us",sv.ultrasonicTime); 
    printf("Distance: %f\n mm",sv.waterLevel); 
    printf("FlowRate: %f\n m3/h",sv.flowRate); 
    printf("TotalFlow: %f\n m3",sv.totalFlow); 
    printf("FlowRateHM: %f\n m3/h",sv.flowRateHM); 
    printf("TotalFlowHM: %f\n m3",sv.totalFlowHM);
    printf("TempHM: %f\n C",sv.temperatureHM);  

    //print the curve on screen
    // curvePlotter.AddPoint(sv);
    // curvePlotter.Render(showFlag);

    // ui->labelCurve->setPixmap(*curvePlotter.pixmapCurve);

    //  if the weight is overflow then send error
    if(sv.weight>OVERFLOW_WEIGHT && inverterStatus)//Overflow
    {
        inverterStatus = 0;
    //    emit SIG_SetInverter(0);
        setRelayState(3, 1);
        TCPPack[] = "Error:Overflow\r\n";
        // 发送水位超重报警TCPPack数据包
        // server.Send(TCPPack);  改到这里
    }
}   */


// 提取'{  }'中的字符
/*
#include "stdio.h"
int main()
{
    char a[] ="ghf12hj4) i love chi)na.(123 i love china.[12] i love china.";
    int x,i,k;
    char buf[52];
    sscanf(a, "%*[^(](%[^)]s", buf);
    printf("%s\n",buf);
    return 0;
}  */


// 解析客户端命令,注意命令大小写
void SLOT_TCPdecoder(uint8_t receiveData[]){
	char s[2] = "=";
	char *key = strtok((char *)receiveData,s);
	char *value = strtok(NULL,s);
	// 没有收到数据
	if(strlen(key)<=1) return;
	// 入水阀门命令
	char *valvein = "valvein";
	char *valveout = "valveout";
	char *valveside = "valveside";
	char *pump = "pump";
	char *reset = "reset";
	if(*key == *valvein) {
		int valueint=atoi(value);
		setRelayState(4, valueint);
		return;
	}
	// 出水阀门命令
	if(*key==*valveout) {
		int valueint=atoi(value);
		setRelayState(3, valueint);
		return;
	}
	// 侧阀门控制命令
	if(*key==*valveside) {
		int valueint = atoi(value);
		setRelayState(1, valueint);
		return;
	}
	// 设置电机频率
	if(*key==*pump){
		SetInverterFreq(atof(value));
		inverterStatus = 1;
		return;
	}
	// 重置实验
	if(*key==*reset){
	    setRelayState(4, 0);  //顶阀关
        setRelayState(1, 0);  //侧阀关
        setRelayState(3, 1);  //出水阀开
        // 电机开始状态
        inverterStatus = 0;
        // 关闭电机
        StopInverter();
        return;
	}
	//清零电子称 sv中保存有电子秤的重量
	 //if(key=="zeroweight" && sv.weight<5){
	//	SetWeightZero();
	//}
}

/*
uint8_t wifiRxBuffer[250];
// 打开wifi模块接收中断
void wifiStartListening() {
	HAL_UART_Receive_IT(&huart4,wifiRxBuffer,1);
}*/


// 检查dataArr字符串是否包含'{ }'这种格式，包含返回1，不包含返回0；
uint8_t *checkIsMatch(uint8_t dataArr[]){
    int isMatchLeft = 0;
    int isMatchRight = 0;
    int i=0;   // 数组索引
    uint8_t result[10];
    int flag=0;
    while(dataArr[i]){
        if(dataArr[i]=='{'){
            flag=1;
        } else if(dataArr[i]=='}'){
            return result;
        } else if(flag == 1) {
            result[i]=dataArr[i];
        }
    }
    return 0;
}

// 循环查询wifi模块是否接收到数据
void wifiDataReceived() {
	if(huart4.RxXferCount < 250) {
        //unsigned char numOfBuffer;
		//numOfBuffer = 250-huart4.RxXferCount;   // 实际接收到的字符总数
		// 这里应该解析到完整指令然后才开始进入中断 判断是不是{...}这种格式
		// 这里发送到单片机去解析数据
        if(checkIsMatch(wifiRxBuffer)){
            SLOT_TCPdecoder(wifiRxBuffer);
            huart4.RxXferCount = 250;
            huart4.pRxBuffPtr = wifiRxBuffer;
        }
	}
}

// produce cJSON
void sendInstrumentData()
{
    cJSON * rootCJSON =  cJSON_CreateObject();
    cJSON_AddItemToObject(rootCJSON, "type", cJSON_CreateString("data"));
    cJSON_AddItemToObject(rootCJSON, "ID", cJSON_CreateNumber(1));
    // 电子秤
    cJSON_AddItemToObject(rootCJSON, "W1", cJSON_CreateNumber( getWeight() ));
    // 超声波液位计温度
    cJSON_AddItemToObject(rootCJSON, "T1", cJSON_CreateNumber( getUlTemperature() ));
    // 超声波液位计渡越时间
    cJSON_AddItemToObject(rootCJSON, "U1", cJSON_CreateNumber( getUlTime() ));
    // 超声波液位计测量距离
    cJSON_AddItemToObject(rootCJSON, "D1", cJSON_CreateNumber( getUlDistance() ));
    // 涡街流量计瞬时流量
    cJSON_AddItemToObject(rootCJSON, "F1", cJSON_CreateNumber( getVortexFlowrate() ));
    // 涡街流量计累积流量
    cJSON_AddItemToObject(rootCJSON, "F2", cJSON_CreateNumber( getVortexTotalFlow() ));
	/*
    struct HeatData heatData;
    heatData.currentFlow=1;
    heatData.totalFlow=2;
    heatData.temperature=3;
    热能表瞬时流量
    cJSON_AddItemToObject(rootCJSON, "F3", cJSON_CreateNumber(heatData.currentFlow));
    cJSON_AddItemToObject(rootCJSON, "F4", cJSON_CreateNumber(heatData.totalFlow));
    cJSON_AddItemToObject(rootCJSON, "T2", cJSON_CreateNumber(heatData.temperature));
	*/
    cJSON_AddItemToObject(rootCJSON, "V1", cJSON_CreateNumber(getRelayState(1)));
    cJSON_AddItemToObject(rootCJSON, "V2", cJSON_CreateNumber(getRelayState(2)));
    cJSON_AddItemToObject(rootCJSON, "V3", cJSON_CreateNumber(getRelayState(3)));
    cJSON_AddItemToObject(rootCJSON, "I1", cJSON_CreateNumber(50.0));

    char * cJsonTest = cJSON_Print(rootCJSON);

    HAL_UART_Transmit(&huart4,(uint8_t*)cJsonTest,strlen(cJsonTest),0xff);
}

