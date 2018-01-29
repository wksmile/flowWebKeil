#include "sampleThread.h"
// #include <QDebug>
// #include<QtCore>
// #include<QVector>
#include "IOController.h"

SensorValues sensorValues;


void SLOT_SetInverter(float freq)
{
   if(freq == 0)   //停止
   {
       serialDevice->StopInverter();
   }
   else if(freq <= 50)  //改变运行频率
    {
       serialDevice->SetInverterFreq(freq);
       msleep(20);
       serialDevice->StartInverter();
   }
}

void SLOT_ZeroWeight()
{
    serialDevice->SetWeightZero();
}

void SLOT_ReadSensorsNon485()
{
    sensorValues.weight = serialDevice->GetWeightValue();
    sensorValues.ultrasonicTime = serialDevice->GetUltraDurationTime();
    sensorValues.waterLevel = serialDevice->GetUltraDistance();
    sensorValues.temperature = serialDevice->GetUltraTemperature();
    //qDebug() << "Non 485 Sensors read completed...";

    if(sensorValues.valuesReady)
    {
        // emit SIG_SensorRead();
        return;
    }
    sensorValues.valuesReady = true;

}

void SLOT_ReadSensors485()
{
    HeatData heat = serialDevice->ReadHeatMeter();

    //No error
    if(heat.currentFlow > -0.1f)
    {
        sensorValues.flowRateHM = heat.currentFlow;
        sensorValues.totalFlowHM = heat.totalFlow;
        sensorValues.temperatureHM = heat.temperature;
    }

    serialDevice->GetVortexData();
    sensorValues.flowRate = serialDevice->vortexFlowrate();
    sensorValues.totalFlow = serialDevice->vortexTotalFlow();

    //qDebug() << "485 Sensors read completed...";

    if(sensorValues.valuesReady)
    {
        // emit SIG_SensorRead();
        return;
    }
    sensorValues.valuesReady = true;
}

sampleThread(SerialDevices* _serialDevice)
{
    serialDevice = _serialDevice;
    qRegisterMetaType<SensorValues>("SensorValues");
    // QObject::connect(this,SIGNAL(SIG_ReadHeatmeter()),this,SLOT(SLOT_ReadHeatmeter()));
}

