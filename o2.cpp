#include "o2.h"

O2::O2(QObject *parent)
    : QObject{parent}
{
    qDebug()<<"O2线程号"<<QThread::currentThreadId();
    pSerialPort = new QSerialPort(this);
    if(openSerialPort())
    {
        //连接接收信息槽
        connect(pSerialPort, &QSerialPort::readyRead, [=]() {
            readSerialPortData(&mO2C);
            if(mO2C > 10 && mO2C < 21)
            {
                mO2CCount++;
                mO2CSum += mO2C;
            }
            if(runCount > 10)//每秒统计一次数据
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
        // 设置定时器超时信号连接到槽函数
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


//O2的线程处理函数
void O2::runO2ThreadFun(bool isFlag)
{
    //qDebug()<<"runO2ThreadFun线程号"<<QThread::currentThreadId();
    if (pTimer)
    {
        if(isFlag) {
            pTimer->start(100);
            //qDebug()<<"启动O2定时器"<<QThread::currentThreadId();
        }
        else {
            pTimer->stop();
            //qDebug()<<"停止O2定时器"<<QThread::currentThreadId();
        }
    }
}
void O2::onO2Timeout()
{
    //qDebug()<<"onO2Timeout线程号"<<QThread::currentThreadId();
    runCount ++ ;
    sendSerialPortData();
}


//串口打开函数
bool O2::openSerialPort()
{
    if(pSerialPort->isOpen())//读取串口当前状态
    {
        qDebug()<<"串口状态已打开,";
        return true;
    }
    //对串口对象进行设置
    pSerialPort->setPortName("ttyAMA0");    //设置串口名
    pSerialPort->setBaudRate(9600);         //设置波特率
    pSerialPort->setDataBits(QSerialPort::Data8);//设置数据位
    pSerialPort->setParity(QSerialPort::NoParity);//设置校验位
    pSerialPort->setStopBits(QSerialPort::OneStop);//设置停止位
    bool ret = pSerialPort->open(QIODevice::ReadWrite);//打开串口
    if(ret == true)
    {
        qDebug()<<"串口打开成功";
        return ret;
    }
    else
    {
        qDebug()<<"串口打开失败"<<pSerialPort->errorString();
        return ret;
    }
}

//发送串口数据
void O2::sendSerialPortData(void)
{
    if(pSerialPort != nullptr)
    {
    //  qDebug()<<"sendSerialPortData:"<<QByteArray::fromHex(QString("FF018600000000007900").toUtf8());
        pSerialPort->write(QByteArray::fromHex(QString("FF018600000000007900").toUtf8()));//读取气体浓度值
        pSerialPort->flush();
    }
}


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
            qDebug()<<"O2浓度异常："<<ret1;
          }
          else
          {

            *O2C = ret1 / 10.0;
            //qDebug()<<"O2浓度="<< *O2C <<"%";
          }
        }
    }
    pSerialPort->clear();//清除接收的数据
}

