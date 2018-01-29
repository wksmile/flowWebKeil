#ifndef CETHERNETTHREAD_H
#define CETHERNETTHREAD_H

// 中断发送数据
void sendDataIT(char key[],char value[]);

// 普通发送数据，timeout一般设为100ms
void sendData(char key[],char value[], unsigned int timeout);

// 解析客户端命令,注意命令大小写
void SLOT_TCPdecoder(char receiveData[]);

// 打开wifi模块接收中断
void wifiStartListening();

// 循环查询wifi模块是否接收到数据
void wifiDataReceived();

// CETHERNETTHREAD_H
#endif
