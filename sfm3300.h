#ifndef SFM3300_H
#define SFM3300_H

#include <QObject>
#include <QThread>//线程头文件
#include <QDebug>//调试头文件
//wiringPi固件头文件
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <iostream>
#include <cstdint>  // 引入uint16_t和int16_t


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

public slots:
    bool resetSFM3300();
    bool initSFM3300();
    bool readSFM3300(float *sfmGetVal);
signals:

private:
    int SFM3300ID;

};

#endif // SFM3300_H
