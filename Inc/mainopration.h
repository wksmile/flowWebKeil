#ifndef MAINOPRATION_H
#define MAINOPRATION_H

#include "tcpServer.h"
// #include "CurvePlotter.h"
#include "serialdevices.h"
#include "sampleThread.h"


// namespace Ui {
//     class MainOpration;
// }

// class MainOpration : public QWidget
// {
//     Q_OBJECT

// public:
    // explicit MainOpration(QWidget *parent = 0);
    ~MainOpration();

    // Ui::MainOpration *ui;

    // QTimer sampleTimer;
    // SerialDevices serialDevice;    定义serialDevices类型

    // sampleThread samplethread;   定义两个类的实类
    // sampleThread RS485Thread;
    // QThread sampleT;      定义两个线程
    // QThread RS485T;

// private:
    // CEthernetThread server;   定义以太网线程类的实类

    // CurvePlotter curvePlotter;   定义画图类的实类

    float totalFlow0;
    float totalFlowHM0;

    //Flag to control which quantities to be plotted on the curve
    int showFlag;


// signals:
    void SIG_SetInverter(float);
    void SIG_ZeroWeight();

    void SIG_ReadSensorsNon485();
    void SIG_ReadSensors485();


// private slots:
    void on_checkTotalFlowHM_clicked(bool checked);
    void on_checkFlowHM_clicked(bool checked);
    void on_sliderInverter_sliderReleased();
    void on_sliderInverter_sliderMoved(int position);
    void on_buttonQuit_clicked();
    void on_checkWeight_clicked(bool checked);
    void on_checkTotalFlow_clicked(bool checked);
    void on_checkFlow_clicked(bool checked);
    void on_checkTemp_clicked(bool checked);
    void on_checkDist_clicked(bool checked);
    void on_checkUltra_clicked(bool checked);
    void on_buttonZeroFlow_clicked();
    void on_buttonClearCurve_clicked();
    void on_buttonZeroWeight_clicked();


    void on_pushButto_Vale3_close_clicked();
    void on_pushButto_Vale3_open_clicked();
    void on_pushButto_Vale2_close_clicked();
    void on_pushButto_Vale2_open_clicked();
    void on_pushButto_Vale1_close_clicked();
    void on_pushButto_Vale1_open_clicked();

    void on_pushButton_StartPump_clicked();
    void on_pushButton_StopPump_clicked();

    void SLOT_Timer();

// public slots:

     //解析客户端命令
    void SLOT_TCPdecoder(QString);

    void SLOT_ProcessSensorData();

    // 根据输入的数返回该数的保留几位小数的结果
    float getReserveDecimal(float num,int decimal){
        float result;
        result *= pow(10,decimal);
        result = (int)result;
        result = result/(pow(10,decimal));
        return result;
    }
// };
// MAINOPRATION_H
#endif
