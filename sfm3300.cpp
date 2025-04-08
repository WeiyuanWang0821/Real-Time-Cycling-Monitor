#include "sfm3300.h"

SFM3300::SFM3300(QObject *parent)
    : QObject{parent}
{

}

bool SFM3300::resetSFM3300(void)//复位流量传感器
{
    uint8_t data[2] = {0};
    data[0] = SFM3300_RESET >> 8;
    data[1] = SFM3300_RESET & 0XFF;

    if (wiringPiI2CRawWrite (SFM3300ID, data,2) == -1){
        qDebug() << "SFM3300RESET失败";
        return false;
    }
    else{
        qDebug() << "SFM3300RESET成功";
    }
    return true;
}

bool SFM3300::initSFM3300(void)
{
    bool ret = false;

    if (wiringPiSetup() == -1){
        qDebug() << "wiringPiSetup初始化失败";
        return false;
    }
    else{
        qDebug() << "WiringPi 初始化成功";
    }

    SFM3300ID = wiringPiI2CSetup(0x40);
    if (SFM3300ID == -1){
        qDebug() << "wiringPiI2CSetup 初始化失败";
        return false;
    }
    else{
        qDebug() << "wiringPiI2CSetup 初始化成功";
    }

    QThread::msleep(100);

    if (resetSFM3300() == false){// 复位
        qDebug() << "resetSFM3300失败";
        return false;
    }
    else{
        qDebug() << "SFM3300RESET成功";
    }

    QThread::msleep(100);
    float afmGetVal = 0;

    // 前面几次获取的数据不准确，去掉
    for(uint8_t i = 0; i < 5; i++){
        readSFM3300(&afmGetVal);
    }
    return true;
}

bool SFM3300::readSFM3300(float *sfmGetVal)
{
    uint8_t sfmVal[3] = {0};	// 2字节的数据 + 1字节的CRC
    uint8_t data[2] = {0};
    float sfmGetValTemp = 0;
    data[0] = SFM3300_FLOWREAD >> 8;
    data[1] = SFM3300_FLOWREAD & 0XFF;

    bool ret = false;
    QThread::msleep(5); // 至少等5ms一次数据

    if (wiringPiI2CRawWrite (SFM3300ID, data , 2) == -1){
        qDebug() << "SFM3300_FLOWREAD 命令写入失败";
        return false;
    }
    else{
    // qDebug() << "SFM3300_FLOWREAD 命令写入成功";
    }

    QThread::msleep(5); // 至少等待5ms一次数据

    //获取气体流量
    if (wiringPiI2CRawRead(SFM3300ID , sfmVal ,3) == -1){
        //qDebug() << "获取气体流量失败";
        return false;
    }
    else{
    // qDebug() << "afmVal"<<afmVal[0];
    // qDebug() << "afmVal"<<afmVal[1];
    // qDebug() << "afmVal"<<afmVal[2];
    }

    //计算流量值
    sfmGetValTemp = (sfmVal[0]<<8 | sfmVal[1]);
    sfmGetValTemp = (sfmGetValTemp - SFM3300_OFFSET) / 1.0 / SFM3300_FLOWCOEFF; // 计算数据，根据公式 （（测试量 - 偏移量）/ 流量系数）
    *sfmGetVal = static_cast<float>(sfmGetValTemp);
    //qDebug() << "气体流量=" << *sfmGetVal;
    return true;
}
