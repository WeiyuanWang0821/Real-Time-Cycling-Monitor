#include "serialportthread.h"

SerialPortThread::SerialPortThread(QObject *parent)
    : QObject{parent}
{
    qDebug()<<"SerialPortThread线程号"<<QThread::currentThreadId();
    isSerialPortThreadFlag = false;

    pSerialPort = new QSerialPort(this);


}

// 设置线程开关标志位
void SerialPortThread::setSerialPortThreadFlag(bool flag)
{
    isSerialPortThreadFlag = flag;
}

// 线程函数
void SerialPortThread::runSerialPortThreadFun()
{
    qDebug()<<"runSerialPortThreadFun线程号"<<QThread::currentThreadId();
    //连接接收信息槽，参数1 串口 参数2 读取信号  参数三 Lam表达式
    connect(pSerialPort,&QSerialPort::readyRead,this,&SerialPortThread::readSerialPortData);
    openSerialPort();//打开串口
    while(isSerialPortThreadFlag)// 死循环
    {
        QThread::msleep(1000);// 采样间隔

        sendSerialPortData();//发送读取传感器数据命令

        if(!isSerialPortThreadFlag)
        {
            break;
        }
    }
    pSerialPort->close();

}

//串口打开函数
void SerialPortThread::openSerialPort()
{
    if(pSerialPort->isOpen())//读取串口当前状态
    {
        qDebug()<<"串口状态已打开,";
        return;
    }
    //对串口对象进行设置
    pSerialPort->setPortName("ttyAMA0");//设置串口名
    pSerialPort->setBaudRate(9600);//设置波特率
    pSerialPort->setDataBits(QSerialPort::Data8);//设置数据位
    pSerialPort->setParity(QSerialPort::NoParity);//设置校验位
    pSerialPort->setStopBits(QSerialPort::OneStop);//设置停止位
    bool ret = pSerialPort->open(QIODevice::ReadWrite);//打开串口
    if(ret)
    {
        qDebug()<<"串口打开成功";
    }
    else
    {
        qDebug()<<"串口打开失败"<<pSerialPort->errorString();
    }
}

//发送串口数据
void SerialPortThread::sendSerialPortData()
{
    //qDebug()<<"sendSerialPortData:"<<QByteArray::fromHex(QString("FF018600000000007900").toUtf8());
    pSerialPort->write(QByteArray::fromHex(QString("FF018600000000007900").toUtf8()));//读取气体浓度值
    pSerialPort->waitForBytesWritten(0);
}


char getCheckSum(char *packet)
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

void SerialPortThread::readSerialPortData()
{
    QByteArray read_msg = pSerialPort->readAll();
    //qDebug()<<"readSerialPortData"<<read_msg;
    if ( getCheckSum(read_msg.data()) == read_msg.at(8))
    {
      int ret = (read_msg.at(3)*256+read_msg.at(4));
      if(ret > 300 || ret < 100)
      {
        qDebug()<<"浓度异常："<<ret;
      }
      else
      {
        qDebug()<<"浓度："<<ret;
        //emit signalsSerialPortThread(ret);
      }
    }
    pSerialPort->clear();//清除接收的数据
}



