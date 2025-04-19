#include "sfm3300.h"

SFM3300::SFM3300(QObject *parent)
    : QObject{parent}
{
   if(initSFM3300()) {
       qDebug()<<"initSFM3300成功";

       pTimer = new QTimer(this);
       // 设置定时器超时信号连接到槽函数
       connect(pTimer, &QTimer::timeout, this, &SFM3300::onSFM3300Timeout);
   }
   else {
       qDebug()<<"SFM3300->initSFM3300错误";
   }
}

SFM3300::~SFM3300()
{
    if (pTimer) {
        pTimer->stop();
        delete pTimer;
    }
}


//线程处理函数
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
void SFM3300::onSFM3300Timeout()
{
    runCount ++ ;
    readSFM3300(&mGasFlow);
    if(mGasFlow >=0 && mGasFlow < 250)
    {
        mGasFlowCount++;
        mGasFlowSum +=mGasFlow;
    }
    if(runCount > 10)//每秒统计一次数据
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


bool SFM3300::resetSFM3300(void)//复位流量传感器
{
    uint8_t data[2] = {0};
    data[0] = SFM3300_RESET >> 8;
    data[1] = SFM3300_RESET & 0XFF;

    if (write (SFM3300ID, data,2) == -1){
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
    //QMutexLocker locker(&mMutex); // 自动锁定和解锁
    SFM3300ID = open("/dev/i2c-1", O_RDWR);
    if (SFM3300ID < 0) {
        qDebug() << "无法打开I2C总线:" << SFM3300ID;
        return false;
    }
    qDebug() << "SFM3300ID:" << SFM3300ID;

    if (ioctl(SFM3300ID, I2C_SLAVE, 0x40) < 0) {
        qDebug() << "无法设置I2C设备地址:" << static_cast<int>(0x40);
        close(SFM3300ID);
        SFM3300ID = -1;
        return false;
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

    QThread::msleep(5); // 至少等5ms一次数据
    QMutexLocker locker(&mMutex); // 自动锁定和解锁
    if (write (SFM3300ID, data , 2) == -1){
        qDebug() << "SFM3300_FLOWREAD 命令写入失败";
        return false;
    }
    else{
    // qDebug() << "SFM3300_FLOWREAD 命令写入成功";
    }

    QThread::msleep(5); // 至少等待5ms一次数据

    //获取气体流量
    if (read(SFM3300ID , sfmVal ,3) == -1){
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
