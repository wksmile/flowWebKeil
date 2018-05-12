#ifndef TEST_H
#define TEST_H

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"

void loop(void);

void test(void);

// test refer command
void testonces(void);

// 测试命令结束符接收才发送
void open2Receive(void);

void open7Receive(void);
void open3Receive(void);
void open1Receive(void);

// 测试电子秤
void testweight(void);

void testUsart7receive(void);

// valve test
void valveTest(void);

void handleRxCpltCallback(UART_HandleTypeDef *huart);

void UART_Send_start(void);

void UART_Change_Page(void);
// SERIALDEVICES_H

// send end char
void UART_Send_END(void);

// 曲线切换函数
void dataCurve(void);
#endif
