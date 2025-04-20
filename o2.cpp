////////////////////////////////////////
// File: o2.cpp
// Description: Handles O₂ concentration acquisition via UART.
//              Uses Qt event-driven callbacks and timers to 
//              receive, filter, and average data before emitting.
////////////////////////////////////////

#include "o2.h"

O2::O2(QObject *parent)
    : QObject{parent}
{
    qDebug()<<"O2 thread number"<<QThread::currentThreadId();
    pSerialPort = new QSerialPort(this);
    if(openSerialPort())
    {
        //Connection to Receive Message Slot
        connect(pSerialPort, &QSerialPort::readyRead, [=]() {
            readSerialPortData(&mO2C);
            if(mO2C > 10 && mO2C < 21)
            {
                mO2CCount++;
                mO2CSum += mO2C;
            }
            if(runCount > 10)// Aggregate once per second
            {
                runCount = 0;
                if(mO2CCount > 0)
                {
                    mO2C = mO2CSum / mO2CCount;
                    mO2CSum = 0;
                    mO2CCount = 0;
                }
                else
                {
                    mO2C = 0;
                }
                emit signalsO2Data(mO2C);
            }
        });

        pTimer = new QTimer(this);
        // Connect timeout signal to handler
        connect(pTimer, &QTimer::timeout, this, &O2::onO2Timeout);
    }
}
O2::~O2()
{
    if (pTimer) {
        pTimer->stop();
        delete pTimer;
    }
    if(pSerialPort)
    {
        pSerialPort->close();
        delete pSerialPort;
    }
}


// Thread function to control timer start/stop
void O2::runO2ThreadFun(bool isFlag)
{
    //qDebug()<<"runO2ThreadFun线程号"<<QThread::currentThreadId();
    if (pTimer)
    {
        if(isFlag) {
            pTimer->start(100);
            //qDebug()<<"Start O2 timer"<<QThread::currentThreadId();
        }
        else {
            pTimer->stop();
            //qDebug()<<"Stop O2 Timer"<<QThread::currentThreadId();
        }
    }
}
// Called every timer interval
void O2::onO2Timeout()
{
    //qDebug()<<"onO2Timeout thread number"<<QThread::currentThreadId();
    runCount ++ ;
    sendSerialPortData();
}


// Open and configure serial port
bool O2::openSerialPort()
{
    if(pSerialPort->isOpen()) // Check if already open
    {
        qDebug()<<"Serial port status is on,";
        return true;
    }
    //Setting up the serial port object
    pSerialPort->setPortName("ttyAMA0");              // Port name
    pSerialPort->setBaudRate(9600);                   // Baud rate
    pSerialPort->setDataBits(QSerialPort::Data8);     // Data bits
    pSerialPort->setParity(QSerialPort::NoParity);    // Parity
    pSerialPort->setStopBits(QSerialPort::OneStop);   // Stop bits
    bool ret = pSerialPort->open(QIODevice::ReadWrite); // Open port
    if(ret == true)
    {
        qDebug()<<"Serial port opened successfully";
        return ret;
    }
    else
    {
        qDebug()<<"Failed to open serial port"<<pSerialPort->errorString();
        return ret;
    }
}

// Send data request via serial
void O2::sendSerialPortData(void)
{
    if(pSerialPort != nullptr)
    {
    //  qDebug()<<"sendSerialPortData:"<<QByteArray::fromHex(QString("FF018600000000007900").toUtf8());
        pSerialPort->write(QByteArray::fromHex(QString("FF018600000000007900").toUtf8()));//读取气体浓度值
        pSerialPort->flush();
    }
}

// Calculate checksum for validation
char O2::getCheckSum(char *packet)
{
    char i,checksum;
    for(i = 1 ; i < 8 ; i++)
    {
        checksum += packet[i];
    }
    checksum = 0xff - checksum;
    checksum += 1;
    return checksum;
}
// Read and decode serial data
void O2::readSerialPortData(float *O2C)
{
    QByteArray read_msg = pSerialPort->readAll();
    if(read_msg.count() == 9)
    {
        if ( getCheckSum(read_msg.data()) == read_msg.at(8))
        {
          int ret1 = (read_msg.at(3)*256+read_msg.at(4));
          if(ret1 > 250 || ret1 < 100)
          {
            qDebug()<<"Abnormal O2 concentration："<<ret1;
          }
          else
          {

            *O2C = ret1 / 10.0;
            //qDebug()<<"O2 concentration ="<< *O2C <<"%";
          }
        }
    }
    pSerialPort->clear();// Clear buffer
}

