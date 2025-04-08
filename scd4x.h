#ifndef SCD4X_H
#define SCD4X_H

#include <QObject>

#include <QThread>//线程头文件
#include <QDebug>//调试头文件
//wiringPi固件头文件
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <iostream>
#include <cstdint>  // 引入uint16_t和int16_t
#include <math.h>
#include <cstdlib> // 用于 malloc 和 free


#define SCD4X_I2CADDR 0x62
#define CRC8_POLYNOMIAL 0x31
#define CRC8_INIT 0xFF
#define CRC8_LEN 1

//Basic Commands
#define SCD4x_COMMAND_START_PERIODIC_MEASUREMENT              0x21b1
#define SCD4x_COMMAND_READ_MEASUREMENT                        0xec05 // execution time: 1ms
#define SCD4x_COMMAND_STOP_PERIODIC_MEASUREMENT               0x3f86 // execution time: 500ms

//On-chip output signal compensation
#define SCD4x_COMMAND_SET_TEMPERATURE_OFFSET                  0x241d // execution time: 1ms
#define SCD4x_COMMAND_GET_TEMPERATURE_OFFSET                  0x2318 // execution time: 1ms
#define SCD4x_COMMAND_SET_SENSOR_ALTITUDE                     0x2427 // execution time: 1ms
#define SCD4x_COMMAND_GET_SENSOR_ALTITUDE                     0x2322 // execution time: 1ms
#define SCD4x_COMMAND_SET_AMBIENT_PRESSURE                    0xe000 // execution time: 1ms

//Field calibration
#define SCD4x_COMMAND_PERFORM_FORCED_CALIBRATION              0x362f // execution time: 400ms
#define SCD4x_COMMAND_SET_AUTOMATIC_SELF_CALIBRATION_ENABLED  0x2416 // execution time: 1ms
#define SCD4x_COMMAND_GET_AUTOMATIC_SELF_CALIBRATION_ENABLED  0x2313 // execution time: 1ms

//Low power
#define SCD4x_COMMAND_START_LOW_POWER_PERIODIC_MEASUREMENT    0x21ac
#define SCD4x_COMMAND_GET_DATA_READY_STATUS                   0xe4b8 // execution time: 1ms

//Advanced features
#define SCD4x_COMMAND_PERSIST_SETTINGS                        0x3615 // execution time: 800ms
#define SCD4x_COMMAND_GET_SERIAL_NUMBER                       0x3682 // execution time: 1ms
#define SCD4x_COMMAND_PERFORM_SELF_TEST                       0x3639 // execution time: 10000ms
#define SCD4x_COMMAND_PERFORM_FACTORY_RESET                   0x3632 // execution time: 1200ms
#define SCD4x_COMMAND_REINIT                                  0x3646 // execution time: 20ms

//Low power single shot - SCD41 only
#define SCD4x_COMMAND_MEASURE_SINGLE_SHOT                     0x219d // execution time: 5000ms
#define SCD4x_COMMAND_MEASURE_SINGLE_SHOT_RHT_ONLY            0x2196 // execution time: 50ms

class SCD4X : public QObject
{
    Q_OBJECT
public:
    explicit SCD4X(QObject *parent = nullptr);

public slots:
    uint8_t SCD4X_Generate_CRC(uint8_t *data, uint16_t count);
    bool SCD4X_ReadCommand(uint8_t *reg_addr, uint16_t *rev_data, uint8_t length);
    void SCD4X_WriteCommand(uint8_t *send_data, uint8_t length);
    bool SCD4X_getSerialNumber();
    void SCD4X_StopPeriodicMeasurement();
    void SCD4X_StartPeriodicMeasurement();
    bool SCD4X_ReadMeasurement(uint16_t *co2, int32_t *temperature, int32_t *humidity);
    bool SCD4X_GetAutomaticSelfCalibrationEnabled(uint8_t enable);
    bool SCD4X_SetAutomaticSelfCalibrationEnabled(bool enabled);
    bool SCD4X_GetDataReadyStatus();
    bool SCD4X_Init();
signals:

private:
    int SCD4XID = 0;

};

#endif // SCD4X_H
