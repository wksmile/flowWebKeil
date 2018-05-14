#include "test.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"

/* USER CODE BEGIN Includes */
#include "stm32f7xx_hal_gpio.h"
#include "IOControl.h"
#include "RS485.h"
#include "tcpServer.h"
#include "elecWeight.h"
#include "ultrasonic.h"

extern UART_HandleTypeDef huart7;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart1;

extern uint8_t receive3[30];

char *strcat(char *dest, const char *src);

uint8_t aRxBuffer[20];
uint8_t sendData7 = 'R';
uint8_t receive[20];
// 7号串口接收数据
char receive7[20];

char receive1[20];

/******************************
void loop(){
  // å¾ªçŽ¯å‘é?è¯»å–ç”µå­ç§¤æ•°æ®åˆ°tcpæœåŠ¡å™?
	uint8_t numOfBuffer;
	if(huart7.RxXferCount < 20) {
		numOfBuffer = 20-huart7.RxXferCount;   // ÃŠÂµÂ¼ÃŠÂ½Ã“ÃŠÃ•ÂµÂ½ÂµÃ„Ã—Ã–Â·Ã»Ã—ÃœÃŠÃ½
		HAL_UART_Transmit(&huart4,aRxBuffer,sizeof(aRxBuffer),100);
		// Â½Â«aRxBufferÃ‡Ã¥Â¿Ã•
		for(int i=0;i<20;i++){
			aRxBuffer[i] = NULL;
		}
		huart7.RxXferCount = 20;
		huart7.pRxBuffPtr = aRxBuffer;
		//HAL_UART_Transmit_IT(&huart7, &sendData7, sizeof(sendData7));
	}
}  */

uint8_t wifiRxBuffer[20];
// 打开wifi模块接收中断
void wifiStartListening() {
	HAL_UART_Receive_IT(&huart4,wifiRxBuffer,1);
}

// 2号串口
void open2Receive(){
	HAL_UART_Receive_IT(&huart2,(uint8_t *)receive,1);
}

void open7Receive(){
	HAL_UART_Receive_IT(&huart7,(uint8_t *)receive7,20);
}

void open1Receive(){
	HAL_UART_Receive_IT(&huart1,(uint8_t *)receive1,1);
}

/*
void test(){
	uint8_t numOfBuffer;
	if(huart2.RxXferCount < 20) {
		numOfBuffer = 20-huart2.RxXferCount;
		strcat(receive," -receive");
		HAL_UART_Transmit(&huart2,(uint8_t *)receive,sizeof(receive),100);
		HAL_Delay(500);
		// Â½Â«aRxBufferÃ‡Ã¥Â¿Ã•
		for(int i=0;i<20;i++){
			receive[i] = NULL;
		}
		huart2.RxXferCount = 20;
		huart2.pRxBuffPtr = (uint8_t*)receive;
		// HAL_UART_Transmit(&huart2, &sendData7, sizeof(sendData7),100); 
	}
}   */

// 自己缓存的数据

/***********************************
char receive_test[20];
int j=0;

void testonces(){
	uint8_t numOfBuffer;
	if(huart2.RxXferCount < 20) {
		numOfBuffer = 20-huart2.RxXferCount;

		for(int i=0; i<numOfBuffer; i++){
			// 接收到结束符
			if(receive[i]=='}' || j>=20){
				strcat(receive_test," -receive");
				HAL_UART_Transmit(&huart2,(uint8_t *)receive_test,sizeof(receive_test),100);
				HAL_Delay(10);
				// 清空自己的缓存区
				for(int i=0;i<20;i++){
					receive_test[i] = NULL;
				}
				j=0;
			} else {
				receive_test[j]=receive[i];
				j++;
			}
		}
		// Â½Â«aRxBufferÃ‡Ã¥Â¿Ã•
		for(int i=0;i<numOfBuffer;i++){
			receive[i] = NULL;
		}
		huart2.RxXferCount = 20;
		huart2.pRxBuffPtr = (uint8_t*)receive;
		// HAL_UART_Transmit(&huart2, &sendData7, sizeof(sendData7),100); 
	}
}
*/

// 0xD1 close relay
// 0xD2 open  relay
// 0xD3 open  machinery
// 0xD4 close machinery

// 测试电子秤发送数据
uint8_t cmd[] = {'R'};
char weightData[20];
uint8_t numOfBuffer;
void testweight(){
	if(huart2.RxXferCount < 20) 
	{
		numOfBuffer = 20-huart2.RxXferCount;
		// 接收到读的命令
		if(receive[0]==0xD2)
		{
    		setRelayState(4,1);
		} 
		else if(receive[0]==0xD1)
		{
			setRelayState(4,0);
		}
		for(int j=0;j<20;j++)
		{
			receive[j] = 0;
		}
		//HAL_UART_Transmit(&huart2, &sendData7, sizeof(sendData7),100); 
	//	__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);/*!Pause the receive interrupt*/
	//	HAL_UART_Receive_IT(&huart2,receive,20);
		
		huart2.pRxBuffPtr = (uint8_t *)receive;// -= huart2.RxXferSize-huart2.RxXferCount;
		huart2.RxXferCount = huart2.RxXferSize;
	//	__HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE); /*!Enable the receive interrupt*/
	}
}


/**************************/
char halusart7[20];
// 测试电子秤接收数据
void testUsart7receive(){
	uint8_t numOfBuffer;
	if(huart7.RxXferCount < 20) {
		numOfBuffer = 20-huart7.RxXferCount;
		// 接收到读的命令
		HAL_UART_Transmit(&huart2,(uint8_t*)receive7,sizeof(receive7),100);
		huart7.RxXferCount = 20;
		huart7.RxXferSize = 20;
	}
}



// Solenoid valve test
void valveTest(){
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);
	HAL_Delay(3000);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET);
	HAL_Delay(3000);
}

/*  test main --------------------
    loopWeight();
    HAL_Delay(500);
    loopUltra();
    HAL_Delay(500);
    loopVortex();
    HAL_Delay(500);  
------------*/

//HAL_UART_Transmit(&huart4,sendData9,sizeof(sendData9),100);
//HAL_UART_Transmit(&huart7,&sendData7,sizeof(sendData7),100);

/*
if(huart2.RxXferCount < 20) {
    //unsigned char numOfBuffer;
	//numOfBuffer = 250-huart4.RxXferCount;   // 实际接收到的字符总数
	// 这里应该解析到完整指令然后才开始进入中断 判断是不是{...}这种格式
	// 这里发送到单片机去解析数据
	if(aRxBuffer[0]==0x01){
		recievecahr = aRxBuffer[2];
		setRelayState(3, recievecahr);
		huart2.RxXferCount = 20;
		huart2.pRxBuffPtr = aRxBuffer;
	}
}  */
 //HAL_Delay(500);


/* loop to check if wifi has reveived data,to send data to wifi by TCP server
  wifiDataReceived();
  // test send string 'send'
HAL_UART_Transmit(&huart4,sendData9,sizeof(sendData9),100);
HAL_Delay(900);
*/

// 原始代码
/*
static uint16_t RcvBufLen = 0;
uint8_t RcvBuf[20];
static uint8_t flag_uartbegin;
void handleRxCpltCallback(UART_HandleTypeDef *huart)
{
	// UNUSED(huart);
	if (huart == &huart2)
	{
		if (RcvBufLen == 0 &&  receive[0]=='{')
			flag_uartbegin = 1;
		
		if (flag_uartbegin == 1)
		{
			if (RcvBufLen < 20)
				RcvBuf[RcvBufLen++] = receive[0];
		}
		if (RcvBufLen == 20 || receive[0]=='}')
		{
			flag_uartbegin = 0;
			
			HAL_UART_Transmit(huart,(uint8_t *)RcvBuf,RcvBufLen,0xffff);
			
			RcvBufLen = 0;
		}
		HAL_UART_Receive_IT(huart,receive,1);	
	}
}  */
uint8_t endChar = 0xff;
// 发送结束符
void UART_Send_END(void)
{
	HAL_UART_Transmit(&huart1,&endChar,sizeof(endChar),0xffff);
	HAL_UART_Transmit(&huart1,&endChar,sizeof(endChar),0xffff);
	HAL_UART_Transmit(&huart1,&endChar,sizeof(endChar),0xffff);
}

// 确保HMI串口正常
void UART_Send_start(void)
{
	HAL_Delay(10);
	UART_Send_END();
	HAL_Delay(10);
}

uint8_t changePage[] = {"page main"};
void UART_Change_Page()
{
	HAL_UART_Transmit(&huart2,changePage,sizeof(changePage),0xffff);
	UART_Send_END();
}

// 去掉数组前面的0字符，例如"000005070"返回"5070"
uint32_t ridOfZero(uint8_t str[])
{
	// unsigned char recv[] = "000005070";
	uint8_t *p=str;
	while(*(p++)=='0');
    // printf("%s \r\n",p-1);
    if(*(p-1) == '\0') return 0;
	uint32_t bufint;
	sscanf(p-1,"%d",&bufint);
	bufint = bufint*255/30;
	return bufint;
}

// 中断处理函数
static uint16_t RcvBufLen = 0;
uint8_t RcvBuf[20];
static uint8_t flag_uartbegin;

// wifi定义变量
static uint16_t WifiBufLen = 0;
uint8_t WifiBuf[20];
static uint8_t flag_wifibegin;

static uint16_t RcvBufLen3 = 0;
uint8_t RcvBuf3[20];

char buf[64];

// uint8_t ffchar = 0xff;

// 曲线显示状态的函数
// 0 电子秤；
// 1 超声波液位计
// 2 涡街流量计
// 3 热能表
char state=0;
// 电子称的额重量（kg）
int weightTest=0;
HAL_StatusTypeDef maStatus;
uint8_t start_error[] = "start error";
uint8_t stop_error[] = "stop error";
void handleRxCpltCallback(UART_HandleTypeDef *huart){
	// 触摸屏接收的命令
	if (huart == &huart1)
	{
		switch(receive1[0]) {
			// 关闭电磁阀
			case 0xD1:
				setRelayState(4,0);
				break;
				// 打开电磁阀
			case 0xD2:
				setRelayState(4,1);
				break;
				// 开启电机
			case 0xD3:
				maStatus = StartInverter();
				if(maStatus == HAL_OK)  HAL_UART_Transmit(huart,start_error,sizeof(start_error),0xffff);
				break;
				// 关闭电机
			case 0xD4:
				maStatus = StopInverter();
				if(maStatus == HAL_OK)  HAL_UART_Transmit(huart,stop_error,sizeof(stop_error),0xffff);
				break;
			case 0xD5:
				state=0;
				break;
			case 0xD6:
				state=1;
				break;
			// 涡街流量计
			case 0xD7:
				state=2;
				break;
			case 0xD8:
				state=3;
				break;
		}
		HAL_UART_Receive_IT(huart,receive1,1);
	}

    if (huart == &huart3)
	{
		if (receive3[0])
		{
			RcvBuf3[RcvBufLen3++]=receive3[0];
		}
		if(RcvBufLen3>=20) RcvBufLen3=0;
		HAL_UART_Receive_IT(&huart3,receive3,1);
	}
	// 电子秤接收到数据,这里还要改回来，改为&huart7
	if (huart == &huart7)
	{
		if (RcvBufLen == 0 &&  receive7[0]=='w')
		{
			// 开始接收状态
			flag_uartbegin = 1;
			HAL_UART_Receive_IT(huart,(uint8_t *)receive7,1);
			return;
		}
		// 接收到为数字
		if (flag_uartbegin == 1 && receive7[0]>='0' && receive7[0]<='9')
		{
			if (RcvBufLen < 20) 
			{
				/*
				if(receive7[0]=='0' && RcvBufLen==0)
				{
					HAL_UART_Receive_IT(huart,(uint8_t *)receive7,1);
					return;
				}  */
				RcvBuf[RcvBufLen++] = receive7[0];
				
			}
		}
		if (RcvBufLen == 20 || receive7[0]=='.' || receive7[0]=='k')
		{
			// 重置为开始状态
			flag_uartbegin = 0;
			weightTest=(int)RcvBuf;
			uint32_t dealRcvBuf = ridOfZero(RcvBuf);
			sprintf(buf,"add 13,0,%d",dealRcvBuf);
			uint8_t counter = 0;
			while(1)
			{
				if(buf[counter] == '\0')
				{
					break;
				}
				counter ++;
			}
			HAL_UART_Transmit(&huart1,(uint8_t *)buf,counter,0xffff);
			UART_Send_END();
			RcvBufLen = 0;
		}
		HAL_UART_Receive_IT(huart,(uint8_t *)receive7,1);
	}  
	
	if (huart == &huart4)
	{
		if (WifiBufLen == 0 &&  wifiRxBuffer[0]=='{')
		{
			// 开始接收状态
			flag_wifibegin = 1;
			wifiStartListening();
			return;
		}
		// 接收到为数字
		if (flag_uartbegin == 1 && wifiRxBuffer[0])
		{
			if (WifiBufLen < 20) 
			{
				/*
				if(receive7[0]=='0' && RcvBufLen==0)
				{
					HAL_UART_Receive_IT(huart,(uint8_t *)receive7,1);
					return;
				}  */
				WifiBuf[WifiBufLen++] = wifiRxBuffer[0];
				
			}
		}
		if (RcvBufLen == 20 || wifiRxBuffer[0]=='}')
		{
			// 重置为开始状态
			flag_wifibegin = 0;
			RcvBufLen = 0;

			// 这里开始解析CJSON
		}
		wifiStartListening();
	}
}

void dataCurve()
{
	// 电子秤曲线
	if(state==0)
	{
		HAL_UART_Transmit(&huart7,cmd,sizeof(cmd),0xFF);
		HAL_Delay(100);
	}
	// 超声波液位计
	else if(state==1)
	{
		ultraData();
	}
	// 涡街流量计
	else if(state==2)
	{
		loopVortex();
	}
}

