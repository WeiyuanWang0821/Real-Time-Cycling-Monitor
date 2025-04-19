#ifndef SFM3300_H
#define SFM3300_H

#include <QObject>
#include <QTimer>
#include <QThread>//线程头文件
#include <QMutex>
#include <QDebug>//调试头文件
#include <iostream>
#include <cstdint>  // 引入uint16_t和int16_t
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>


#define	SFM3300_OFFSET		32768	// 偏移量
#define	SFM3300_FLOWCOEFF	120		// 空气的流量系数
/* 设备命令 */
#define SFM3300_FLOWREAD	0x1000	// 读取流量：返回2个字节
#define SFM3300_IDREAD		0x31AE	// 读取ID号：返回4个字节
#define SFM3300_RESET		0x2000	// 复位指令

#define SFM3300_ERROR       0
#define SFM3300_OK          0

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
