#ifndef O2_H
#define O2_H

#include <QObject>

#include <QThread>
#include <QDebug>

#include <QSerialPort>
#include <QSerialPortInfo>

class O2 : public QObject
{
    Q_OBJECT
public:
    explicit O2(QObject *parent = nullptr);

public slots:
    bool initO2();
    bool openSerialPort();
    void sendSerialPortData();
    char getCheckSum(char *packet);
    void readSerialPortData(float *O2C);
signals:

private:
    bool isSerialPortThreadFlag = false;
    QSerialPort *pSerialPort;  
    int mInspiredVolume;
    int mExpiredVolume;

};

#endif // O2_H
