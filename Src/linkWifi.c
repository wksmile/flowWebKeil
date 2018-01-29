#include "main.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal.h"

// 设置wifi参数的函数库

UART_HandleTypeDef huart4;

#define WIFIRESET(X) HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,(GPIO_PinState)X)
#define WIFIUART huart4

void WIFI_UARTSend(char* data, uint16_t len)
{
        HAL_UART_Transmit(&WIFIUART,(uint8_t*)data, len,100);
}

void WIFI_UARTSendString(char* stringD)
{
        uint16_t counter = 0;
        while(1)
        {
                if(*(stringD + counter) == '\0' || counter >= 100) break;
                counter++;
        }

        WIFI_UARTSend(stringD, counter);
}


void WIFI_ResetWIFI()
{
        WIFIRESET(0);
        HAL_Delay(100);
        WIFIRESET(1);
}

void WIFI_setParameter(char* SSID, char* PassWord,char* ServerAddress)
{
        char CR = 0x0d;

        WIFI_UARTSendString("+++");
        HAL_Delay(500);
        WIFI_UARTSendString("a");
        HAL_Delay(500);

        WIFI_UARTSendString("AT+WMODE=STA");
        WIFI_UARTSend(&CR,1);
        HAL_Delay(500);

        WIFI_UARTSendString("AT+WSTA=");
        WIFI_UARTSendString(SSID);
        WIFI_UARTSendString(",");
        WIFI_UARTSendString(PassWord);
        WIFI_UARTSend(&CR,1);
        HAL_Delay(500);

        WIFI_UARTSendString("AT+SOCKA=TCPC,");
        WIFI_UARTSendString(ServerAddress);
        WIFI_UARTSendString(",8899");
        WIFI_UARTSend(&CR,1);
        HAL_Delay(500);

        WIFI_UARTSendString("AT+Z");
        WIFI_UARTSend(&CR,1);
        HAL_Delay(500);
}
// use------------
//HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_SET);
//  HAL_Delay(500);
//  WIFI_setParameter("TP-LINK_5D08", "00000000","192.168.1.105");    ÔÚÖ÷º¯ÊýÖÐµ÷ÓÃ