#include "o2.h"

O2::O2(QObject *parent)
    : QObject{parent}
{

}

bool O2::initO2(void)
{
    pSerialPort = new QSerialPort(this);
    return O2::openSerialPort();
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
    pSerialPort->setPortName("ttyAMA0");//设置串口名
    pSerialPort->setBaudRate(9600);//设置波特率
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
    //qDebug()<<"sendSerialPortData:"<<QByteArray::fromHex(QString("FF018600000000007900").toUtf8());
    pSerialPort->write(QByteArray::fromHex(QString("FF018600000000007900").toUtf8()));//读取气体浓度值
    pSerialPort->waitForBytesWritten(0);
    pSerialPort->waitForReadyRead(5);
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
    pSerialPort->write(QByteArray::fromHex(QString("FF018600000000007900").toUtf8()));//读取气体浓度值
    pSerialPort->waitForBytesWritten(0);
    bool ret = pSerialPort->waitForReadyRead();
    if(ret == true)
    {
        QByteArray read_msg = pSerialPort->readAll();
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

