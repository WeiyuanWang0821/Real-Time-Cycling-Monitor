////////////////////////////////////////
// File: sfm3300.cpp
// Description: SFM3300 gas flow sensor handler via I²C
//              Implements initialization, reset, periodic data acquisition,
//              and Qt timer-based event-driven flow data reporting.
////////////////////////////////////////

#include "sfm3300.h"

SFM3300::SFM3300(QObject *parent)
    : QObject{parent}
{
   if(initSFM3300()) {
       qDebug()<<"initSFM3300 success";

       pTimer = new QTimer(this);
    // Connect timer signal to slot
       connect(pTimer, &QTimer::timeout, this, &SFM3300::onSFM3300Timeout);
   }
   else {
       qDebug()<<"SFM3300->initSFM3300 failed";
   }
}

SFM3300::~SFM3300()
{
    if (pTimer) {
        pTimer->stop();
        delete pTimer;
    }
}


// Thread control function
void SFM3300::runSFM3300ThreadFun(bool isFlag)
{
    if (pTimer)
    {
        if(isFlag) {
            pTimer->start(100);
        }
        else {
            pTimer->stop();
        }
    }
}
// Timer callback for periodic flow averaging
void SFM3300::onSFM3300Timeout()
{
    runCount ++ ;
    readSFM3300(&mGasFlow);
    if(mGasFlow >=0 && mGasFlow < 250)
    {
        mGasFlowCount++;
        mGasFlowSum +=mGasFlow;
    }
    if(runCount > 10)// Compute average every second
    {
        runCount = 0;
        if(mGasFlowCount > 0)
        {
            mGasFlow = mGasFlowSum / mGasFlowCount;
            mGasFlowSum = 0;
            mGasFlowCount = 0;
        }
        else
        {
            mGasFlow = 0;
        }
        emit signalsSFM3300Data(mGasFlow);
    }

}

// Send reset command to sensor
bool SFM3300::resetSFM3300(void)
{
    uint8_t data[2] = {0};
    data[0] = SFM3300_RESET >> 8;
    data[1] = SFM3300_RESET & 0XFF;

    if (write (SFM3300ID, data,2) == -1){
        qDebug() << "SFM3300RESET failed";
        return false;
    }
    else{
        qDebug() << "SFM3300RESET succeeded";
    }
    return true;
}
// Initialize SFM3300 sensor
bool SFM3300::initSFM3300(void)
{
    //QMutexLocker locker(&mMutex); 
    SFM3300ID = open("/dev/i2c-1", O_RDWR);
    if (SFM3300ID < 0) {
        qDebug() << "Failed to open I2C bus:" << SFM3300ID;
        return false;
    }
    qDebug() << "SFM3300ID:" << SFM3300ID;

    if (ioctl(SFM3300ID, I2C_SLAVE, 0x40) < 0) {
        qDebug() << "Failed to set I2C address:" << static_cast<int>(0x40);
        close(SFM3300ID);
        SFM3300ID = -1;
        return false;
    }

    QThread::msleep(100);

    if (resetSFM3300() == false){// reset
        qDebug() << "resetSFM3300 failed";
        return false;
    }
    else{
        qDebug() << "SFM3300RESET successed";
    }

    QThread::msleep(100);
    float afmGetVal = 0;

    // Discard first few reads to stabilize
    for(uint8_t i = 0; i < 5; i++){
        readSFM3300(&afmGetVal);
    }
    return true;
}
// Read flow data from SFM3300 sensor
bool SFM3300::readSFM3300(float *sfmGetVal)
{
    uint8_t sfmVal[3] = {0};	// 2 bytes data + 1 byte CRC
    uint8_t data[2] = {0};
    float sfmGetValTemp = 0;
    data[0] = SFM3300_FLOWREAD >> 8;
    data[1] = SFM3300_FLOWREAD & 0XFF;

    QThread::msleep(5); // Wait at least 5ms before read
    QMutexLocker locker(&mMutex); // Thread-safe I²C access
    if (write (SFM3300ID, data , 2) == -1){
        qDebug() << "SFM3300_FLOWREAD write failed";
        return false;
    }
    else{
    // qDebug() << "SFM3300_FLOWREAD write success";
    }

    QThread::msleep(5); // Wait before reading

    // Read flow data from sensor
    if (read(SFM3300ID , sfmVal ,3) == -1){
        //qDebug() << "Failed to get gas flow";
        return false;
    }
    else{
    // qDebug() << "afmVal"<<afmVal[0];
    // qDebug() << "afmVal"<<afmVal[1];
    // qDebug() << "afmVal"<<afmVal[2];
    }

    // Convert raw data to flow value
    sfmGetValTemp = (sfmVal[0]<<8 | sfmVal[1]);
    sfmGetValTemp = (sfmGetValTemp - SFM3300_OFFSET) / 1.0 / SFM3300_FLOWCOEFF; // Calculated data，According to the formula （（test volume - shift）/ flow coefficient）//// (raw - offset) / coefficient
    *sfmGetVal = static_cast<float>(sfmGetValTemp);
    //qDebug() << "gas flow=" << *sfmGetVal;
    return true;
}
