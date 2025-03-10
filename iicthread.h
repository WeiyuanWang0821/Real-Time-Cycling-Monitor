#ifndef IICTHREAD_H
#define IICTHREAD_H

#include <QObject>
#include <QThread>//线程头文件
#include <QDebug>//调试头文件
#include <QReadWriteLock>

#include "sfm3300.h"
#include "scd4x.h"
#include "o2.h"

class IICThread : public QObject
{
    Q_OBJECT
public:
    explicit IICThread(QObject *parent = nullptr);

    //线程处理函数
    void runIICThreadFun();
    void setIICThreadFlag(bool flag = false);

public slots:

    void getData(float *o2c, float *co2c, float *gasflow);
signals:
    void signalsIICThread();

private:
    bool isIICThreadFlag = false;
    SFM3300 *pSFM3300 = nullptr;
    bool isSFM3300Flag = false;
    float mGasFlow = 0;
    float mGasFlowSum = 0;
    int mGasFlowCount = 0;


    SCD4X *pSCD4X = nullptr;
    bool isSCD4XFlag = false;
    uint16_t mCO2C = 0;
    uint mCO2CSum = 0;
    int32_t mTemperature = 0;
    int32_t mHumidity = 0;
    int mCO2CCount = 0;

    bool isO2Flag = false;
    O2 *pO2 = nullptr;
    float mO2C = 0;
    float mO2CSum = 0;
    int mO2CCount = 0;




};

#endif // IICTHREAD_H
