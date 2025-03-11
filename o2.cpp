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

bool O2::openSerialPort()
{
    if(pSerialPort->isOpen())
    {
        qDebug()<<"The serial port is enabled,";
        return true;
    }
    pSerialPort->setPortName("ttyAMA0");
    pSerialPort->setBaudRate(9600);
    pSerialPort->setDataBits(QSerialPort::Data8);
    pSerialPort->setParity(QSerialPort::NoParity);
    pSerialPort->setStopBits(QSerialPort::OneStop);
    bool ret = pSerialPort->open(QIODevice::ReadWrite);
    if(ret == true)
    {
        qDebug()<<"Serial port successfully opened";
        return ret;
    }
    else
    {
        qDebug()<<"Failed to open the serial port"<<pSerialPort->errorString();
        return ret;
    }
}

void O2::sendSerialPortData(void)
{
    //qDebug()<<"sendSerialPortData:"<<QByteArray::fromHex(QString("FF018600000000007900").toUtf8());
    pSerialPort->write(QByteArray::fromHex(QString("FF018600000000007900").toUtf8()));
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
    pSerialPort->write(QByteArray::fromHex(QString("FF018600000000007900").toUtf8()));
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
            qDebug()<<"O2Concentration anomaly："<<ret1;
          }
          else
          {

            *O2C = ret1 / 10.0;
            //qDebug()<<"O2Concentration="<< *O2C <<"%";
          }
        }
    }
    pSerialPort->clear();
}

