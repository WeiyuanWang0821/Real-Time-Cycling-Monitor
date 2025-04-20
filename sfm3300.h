////////////////////////////////////////
// File: sfm3300.h
// Description: Header for SFM3300 gas flow sensor driver
//              Defines I²C communication, sensor commands, data processing,
//              and Qt-based timer-driven acquisition framework.
////////////////////////////////////////

#ifndef SFM3300_H
#define SFM3300_H

#include <QObject>
#include <QTimer>
#include <QThread>      // Threading support
#include <QMutex>
#include <QDebug>       // Debug output
#include <iostream>
#include <cstdint>      // For uint16_t and int16_t
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>


// Flow conversion constants
#define SFM3300_OFFSET     32768    // Sensor offset
#define SFM3300_FLOWCOEFF  120      // Flow coefficient for air

// Device commands
#define SFM3300_FLOWREAD   0x1000   // Read flow (2 bytes)
#define SFM3300_IDREAD     0x31AE   // Read ID (4 bytes)
#define SFM3300_RESET      0x2000   // Reset command

#define SFM3300_ERROR      0
#define SFM3300_OK         0

class SFM3300 : public QObject
{
    Q_OBJECT
public:
    explicit SFM3300(QObject *parent = nullptr);
    ~SFM3300();

public slots:
    bool resetSFM3300();
    bool initSFM3300();
    bool readSFM3300(float *sfmGetVal);
    void runSFM3300ThreadFun(bool isFlag);

signals:
    void signalsSFM3300Data(float mGasFlow);

private slots:
    void onSFM3300Timeout();

private:
    QMutex mMutex;
    int SFM3300ID;
    QTimer *pTimer  = nullptr;
    float mGasFlow = 0;
    float mGasFlowSum = 0;
    int mGasFlowCount = 0;
    int runCount = 0;

};

#endif // SFM3300_H
