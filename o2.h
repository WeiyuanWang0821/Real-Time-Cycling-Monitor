#ifndef O2_H
#define O2_H

#include <QObject>
#include <QTimer>
#include <QThread>//线程头文件
#include <QDebug>//调试头文件
//串口头文件
#include <QSerialPort>
#include <QSerialPortInfo>

class O2 : public QObject
{
    Q_OBJECT
public:
    explicit O2(QObject *parent = nullptr);
    ~O2();


public slots:
    //线程处理函数
    void runO2ThreadFun(bool isFlag);
    bool openSerialPort();
    void sendSerialPortData();
    char getCheckSum(char *packet);
    void readSerialPortData(float *O2C);

signals:
    void signalsO2Data(float O2C);

private slots:
    void onO2Timeout();

private:
    QSerialPort *pSerialPort  = nullptr;  //串口对象
    QTimer *pTimer  = nullptr;
    float mO2C = 0;
    float mO2CSum = 0;
    int mO2CCount = 0;
    int runCount = 0;
};

#endif // O2_H
