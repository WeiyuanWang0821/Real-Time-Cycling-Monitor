////////////////////////////////////////
// File: o2.h
// Description: Header for O2 sensor handler.
//              Declares serial communication interface,
//              Qt timer-based event callbacks, and data processing slots.
////////////////////////////////////////

#ifndef O2_H
#define O2_H

#include <QObject>
#include <QTimer>
#include <QThread>      // Thread support
#include <QDebug>       // Debug output
#include <QSerialPort>  // Serial port support
#include <QSerialPortInfo>

class O2 : public QObject
{
    Q_OBJECT
public:
    explicit O2(QObject *parent = nullptr);
    ~O2();


public slots:
    void runO2ThreadFun(bool isFlag);       // Thread control function
    bool openSerialPort();                  // Open and configure serial port
    void sendSerialPortData();              // Send request data to sensor
    char getCheckSum(char *packet);         // Compute checksum
    void readSerialPortData(float *O2C);    // Parse and process received data


signals:
    void signalsO2Data(float O2C);          // Emitted when valid O2 data is ready

private slots:
    void onO2Timeout();                     // Triggered by timer, sends request


private:
    QSerialPort *pSerialPort  = nullptr;    // Serial port instance
    QTimer *pTimer  = nullptr;              // Periodic timer
    float mO2C = 0;                         // Current O2 concentration
    float mO2CSum = 0;                      // Accumulated valid O2 values
    int mO2CCount = 0;                      // Number of valid readings
    int runCount = 0;                       // Timer loop counter
};

#endif // O2_H
