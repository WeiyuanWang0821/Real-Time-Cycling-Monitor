#ifndef SERIALPORTTHREAD_H
#define SERIALPORTTHREAD_H

#include <QObject>
#include <QThread>//线程头文件
#include <QDebug>//调试头文件
//串口头文件
#include <QSerialPort>
#include <QSerialPortInfo>

class SerialPortThread : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortThread(QObject *parent = nullptr);

    //线程处理函数
    void runSerialPortThreadFun();
    void setSerialPortThreadFlag(bool flag = false);

    void openSerialPort();
    void sendSerialPortData();
    void readSerialPortData();
signals:
    void signalsSerialPortThread(int);

private:
    bool isSerialPortThreadFlag = false;
    QSerialPort *pSerialPort;  //串口对象
    int mInspiredVolume;
    int mExpiredVolume;



};
#endif // SERIALPORTTHREAD_H
