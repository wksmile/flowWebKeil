//#include "./CCode/tcpclient.h"
#include <stdlib.h>
#include <string.h>
#include "mainopration.h"
#include "ui_mainopration.h"
#include "IOController.h"

#define OVERFLOW_WEIGHT 20//The maximum weight upper tank could hold. If reach this threshold, pump will sop and valveout will open.

// 开始的电机频率
int inverterFrequency = 30;
// 电机开始状态
bool inverterStatus = 0;
// 数据包的结构体变量
SensorValues sv;

//There are two threads which are both constructed with SampleThread class
//The first one, samplethread, controls electronic scale and ultrasonic meter（超声波测试距离） (sampling ultrasonic meter takes ~300ms)
//The second one, RS485Thread, controls all devices connected to the RS485 network, including inverter, vortex flow meter（渦街流量计） and heat meter（热能表）
//Because heat meter is also slow (~500ms to complete a read process), the RS485 thread must be separated with the sample thread
//Both of them uses the same SerialDevices class.

//the MainOpration constructor

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

}

//解析客户端命令
void SLOT_TCPdecoder(char mes[])
{
    // 将接收的字符串转化为小写
    strlwr(mes)
    char *messageSeparated[3]
    char split[] = '='; 

    messageSeparated[0] = strtok(mes,split);
    messageSeparated[1] = strtok(NULL,split);
    // 没有收到数据
    if(messageSeparated[0]==NULL) return; 

    if(messageSeparated[0]=="valvein"){setRelayState(4, messageSeparated[1]=="1");return;}
    if(messageSeparated[0]=="valveout"){setRelayState(3, messageSeparated[1]=="1");return;}
    if(messageSeparated[0]=="valveside"){setRelayState(1, messageSeparated[1]=="1");return;}
    // 启动电机信号
    if(messageSeparated[0]=="pump"){
        // emit SIG_SetInverter(messageSeparated[1].toFloat()); 
        // inverterStatus = (messageSeparated[1].toFloat()<0.01)?0:1; 
        return;
    }

    if(messageSeparated[0]=="reset")
    {
        setRelayState(4, 0);//顶阀关
        setRelayState(1, 0);//侧阀关
        setRelayState(3, 1);//出水阀开
        inverterStatus = 0;
        // 关闭电机
        // emit SIG_SetInverter(0);
        return;
    }
    if(messageSeparated[0]=="zeroweight" && sv.weight<5) {
        emit SIG_ZeroWeight();//清零电子称
    }

    if(messageSeparated[0]=="stopexperiment")
    {
        setRelayState(4, 0);//顶阀关
        setRelayState(1, 0);//侧阀关
        setRelayState(3, 0);//出水阀关
        inverterStatus = 0;
        // 关闭电机
        // emit SIG_SetInverter(0);
        return;
    }

    if(messageSeparated[0]=="startexperiment")//开始实验
    {
        setRelayState(4, 1);//顶阀开
        setRelayState(1, 0);//侧阀关
        setRelayState(3, 0);//出水阀关
        // 启动电机
        // emit SIG_SetInverter(messageSeparated[1].toFloat());
        // inverterStatus = (messageSeparated[1].toFloat()<0.01)?0:1;
        return;
    }
    return;
}

void SLOT_Timer()
{
    //qDebug() << "Start Reading Sensors...";

    sensorValues.valuesReady = false;
    // emit SIG_ReadSensors485();
    // emit SIG_ReadSensorsNon485();

}


// MainOpration::~MainOpration()
// {
//     delete ui;
// }

// ----------------------图形界面相关操作-------------------------------------------------------------------------------

//Start Pump
void on_pushButton_StartPump_clicked()
{
    // 图形界面开启水泵
    // inverterFrequency = ui->sliderInverter->value();
    // emit SIG_SetInverter(inverterFrequency);
    // inverterStatus = 1;
}

//Stop Pump
void on_pushButton_StopPump_clicked()
{
    // emit SIG_SetInverter(0);
    // inverterStatus = 0;
}



// 1 value open
void on_pushButto_Vale1_open_clicked()
{
    K1Open();
}

// 1 value close
void on_pushButto_Vale1_close_clicked()
{
    K1Close();
}

// 2 value open
void on_pushButto_Vale2_open_clicked()
{
    K3Open();
}

// 2 value close
void on_pushButto_Vale2_close_clicked()
{
    K3Close();
}

// 3 value open
void on_pushButto_Vale3_open_clicked()
{
     K4Open();
}

// 3 value close
void on_pushButto_Vale3_close_clicked()
{
    K4Close();
}

// 清零
void on_buttonZeroWeight_clicked()
{
    // emit SIG_ZeroWeight();
}

// 清平
void on_buttonClearCurve_clicked()
{
    // curvePlotter.Clear();
}

void on_buttonZeroFlow_clicked()
{
    totalFlow0 = sensorValues.totalFlow;
    totalFlowHM0 = sensorValues.totalFlowHM;
}


//////////////////////Check boxes to control SHOW FLAG//////////////////////////////
void on_checkUltra_clicked(bool checked)
{
    if(checked)
        showFlag |= SHOW_ULTRA;
    else
        showFlag &= ~SHOW_ULTRA;

    // curvePlotter.Render(showFlag);
    // ui->labelCurve->setPixmap(*curvePlotter.pixmapCurve);
}

void on_checkDist_clicked(bool checked)
{
    if(checked)
        showFlag |= SHOW_DIST;
    else
        showFlag &= ~SHOW_DIST;

    // curvePlotter.Render(showFlag);
    // ui->labelCurve->setPixmap(*curvePlotter.pixmapCurve);
}

void on_checkTemp_clicked(bool checked)
{
    if(checked)
        showFlag |= SHOW_TEMP;
    else
        showFlag &= ~SHOW_TEMP;

    // curvePlotter.Render(showFlag);
    // ui->labelCurve->setPixmap(*curvePlotter.pixmapCurve);
}

void on_checkFlow_clicked(bool checked)
{
    if(checked)
        showFlag |= SHOW_FLOW;
    else
        showFlag &= ~SHOW_FLOW;

    // curvePlotter.Render(showFlag);
    // ui->labelCurve->setPixmap(*curvePlotter.pixmapCurve);
}

void on_checkTotalFlow_clicked(bool checked)
{
    if(checked)
        showFlag |= SHOW_TOTAL;
    else
        showFlag &= ~SHOW_TOTAL;

    // curvePlotter.Render(showFlag);
    // ui->labelCurve->setPixmap(*curvePlotter.pixmapCurve);
}

void on_checkWeight_clicked(bool checked)
{
    if(checked)
        showFlag |= SHOW_WEIGHT;
    else
        showFlag &= ~SHOW_WEIGHT;

    // curvePlotter.Render(showFlag);
    // ui->labelCurve->setPixmap(*curvePlotter.pixmapCurve);
}

void on_checkFlowHM_clicked(bool checked)
{
    if(checked)
        showFlag |= SHOW_FLOW_HM;
    else
        showFlag &= ~SHOW_FLOW_HM;

    // curvePlotter.Render(showFlag);
    // ui->labelCurve->setPixmap(*curvePlotter.pixmapCurve);
}

void on_checkTotalFlowHM_clicked(bool checked)
{
    if(checked)
        showFlag |= SHOW_TOTAL_HM;
    else
        showFlag &= ~SHOW_TOTAL_HM;

    // curvePlotter.Render(showFlag);
    // ui->labelCurve->setPixmap(*curvePlotter.pixmapCurve);
}

void on_buttonQuit_clicked()
{
    K2Close();
    K3Close();
    K4Close();
    // emit SIG_SetInverter(0);
    // this->close();
}

void on_sliderInverter_sliderMoved(int position)
{
    // ui->labelFrequency->setText(QString::number(position) + " Hz");
}

void on_sliderInverter_sliderReleased()
{
    // inverterFrequency = ui->sliderInverter->value();
    // if(inverterStatus)emit SIG_SetInverter(inverterFrequency);
}


