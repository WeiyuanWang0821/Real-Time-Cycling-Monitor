#include "iicthread.h"

IICThread::IICThread(QObject *parent)
    : QObject{parent}
{
    qDebug()<<"IICThreadNumber"<<QThread::currentThreadId();
    isIICThreadFlag = false;

    bool ret= false;

    isSFM3300Flag = false;
    pSFM3300 = new SFM3300(this);

    isO2Flag = false;
    pO2 = new O2(this);

    isSCD4XFlag = false;
    pSCD4X = new SCD4X(this);

    ret = pO2->initO2();
    if( ret == false){
        qDebug()<<"pO2->initO2error";
        isO2Flag = false;
    }

    else{
        qDebug()<<"pO2->initO2 success";
        isO2Flag = true;
    }

   ret = pSFM3300->initSFM3300();
   if( ret == false)// SFM3300Initialization failed, sensor thread not executed
   {
       qDebug()<<"SFM3300->initSFM3300 error";
        isSFM3300Flag = false;
   }
   else
   {
       qDebug()<<"SFM3300->initSFM3300 success";
       isSFM3300Flag = true;
   }

   ret = pSCD4X->SCD4X_Init();
   if( ret == false){
       qDebug()<<"pSCD4X->SCD4X_Init error";
       isSCD4XFlag = false;

   }

   else{
       qDebug()<<"pSCD4X->SCD4X_Init success";
       isSCD4XFlag = true;
   }


}

// Sets the thread switch flag bit
void IICThread::setIICThreadFlag(bool flag)
{
    isIICThreadFlag = flag;
}

//Get parameter per second
void IICThread::getData(float *o2c ,float *co2c ,float *gasflow)
{
    QReadWriteLock lock;
    // Write operation: Add a write lock, other threads can not read or write
    lock.lockForWrite();
    if(mO2CCount > 0)
    {
        *o2c = mO2CSum / mO2CCount;
        mO2CSum = 0;
        mO2CCount = 0;
    }
    else
       *o2c = 0;

    if(mCO2CCount > 0)
    {
        *co2c = mCO2CSum / mCO2CCount / 100.0;
        mCO2CSum = 0;
        mCO2CCount = 0;
    }
    else
       *co2c = 0;

    if(mGasFlowCount > 0)
    {
        *gasflow = mGasFlowSum * 1000 / mGasFlowCount / 60.0;
        mGasFlowSum = 0;
        mGasFlowCount = 0;
    }
    else
       *gasflow = 0;

    qDebug() << *o2c << " --" << *co2c  << " --" << *gasflow;

    lock.unlock();
}


void IICThread::runIICThreadFun()// Thread function
{
    qDebug()<<"runIICThreadFunNumber"<<QThread::currentThreadId();
    pSCD4X->SCD4X_StartPeriodicMeasurement();
//    bool ret= false;

//    isSFM3300Flag = false;
//    pSFM3300 = new SFM3300(this);

//    isO2Flag = false;
//    pO2 = new O2(this);

//    isSCD4XFlag = false;
//    pSCD4X = new SCD4X(this);

//    ret = pO2->initO2();
//    if( ret == false){
//        qDebug()<<"pO2->initO2error";
//        isO2Flag = false;
//    }

//    else{
//        qDebug()<<"pO2->initO2success";
//        isO2Flag = true;
//    }

//   ret = pSFM3300->initSFM3300();
//   if( ret == false)// SFM3300Initialization failed, sensor thread not executed
//   {
//       qDebug()<<"SFM3300->initSFM3300error";
//        isSFM3300Flag = false;
//   }
//   else
//   {
//       qDebug()<<"SFM3300->initSFM3300success";
//       isSFM3300Flag = true;
//   }

//   ret = pSCD4X->SCD4X_Init();
//   if( ret == false){
//       qDebug()<<"pSCD4X->SCD4X_Initerror";
//       isSCD4XFlag = false;

//   }

//   else{
//       qDebug()<<"pSCD4X->SCD4X_Initsuccess";
//       isSCD4XFlag = true;
//   }

    while(isIICThreadFlag)
    {
        QThread::msleep(100);//
        //emit signalsIICThread();

        pO2->readSerialPortData(&mO2C);
        if(mO2C > 10 && mO2C < 21)
        {
            mO2CCount++;
            mO2CSum +=mO2C;
        }
        pSFM3300->readSFM3300(&mGasFlow);
        if(mGasFlow >=0 && mGasFlow < 250)
        {
            mGasFlowCount++;
            mGasFlowSum +=mGasFlow;
        }

        pSCD4X->SCD4X_ReadMeasurement(&mCO2C, &mTemperature, &mHumidity);
        if(mCO2C > 100 && mCO2C < 4000)
        {
            mCO2CCount++;
            mCO2CSum +=mCO2C;
        }

        if(!isIICThreadFlag)
        {
            break;
        }
    }
}
