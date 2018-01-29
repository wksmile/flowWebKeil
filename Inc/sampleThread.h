#ifndef MYTHREAD_H
#define MYTHREAD_H

// #include <QThread>
#include "serialdevices.h"
// #include "./Sleep/mySleep.h"


// #include <QVector>
// #include <QByteArray>
// #include <QMetaType>


struct SensorValues
{
    float   temperature;
    int     waterLevel;
    int     ultrasonicTime;
    float   weight;
    float   flowRate;
    float   totalFlow;
    float   flowRateHM;
    float   totalFlowHM;
    float   temperatureHM;

    bool    valuesReady;
};
// Q_DECLARE_METATYPE(SensorValues);

extern SensorValues sensorValues;


// class sampleThread : public QObject
// {
//     Q_OBJECT
// public:    //  这里定义了多个SerialDevices类

//    explicit sampleThread(SerialDevices* _serialDevice);
    SerialDevices* serialDevice;

// protected:
    void run();

// private:

// signals:
    void SIG_SensorRead();

// public slots:
    void SLOT_ReadSensorsNon485();
    void SLOT_ReadSensors485();
    void SLOT_SetInverter(float freq);
    void SLOT_ZeroWeight();

// };

// MYTHREAD_H

#endif
