#include "scd4x.h"

SCD4X::SCD4X(QObject *parent)
    : QObject{parent}
{

}


/***************************************************************************************************************
from sensirion_i2c.c sensirion_i2c_generate_crc
****************************************************************************************************************/
uint8_t SCD4X::SCD4X_Generate_CRC(uint8_t* data, uint16_t count)
{
    uint16_t current_byte;
    uint8_t crc = CRC8_INIT;
    uint8_t crc_bit;

    /* calculates 8-Bit checksum with given polynomial */
    for (current_byte = 0; current_byte < count; ++current_byte)
    {
        crc ^= (data[current_byte]);
        for (crc_bit = 8; crc_bit > 0; --crc_bit)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ CRC8_POLYNOMIAL;
            else
                crc = (crc << 1);
        }
    }
    return crc;
}

/***************************************************************************************************************
SCD4X Read Command
****************************************************************************************************************/
bool SCD4X::SCD4X_ReadCommand(uint8_t* reg_addr, uint16_t* rev_data, uint8_t length)
{
    uint8_t redata_len = length * 3;
    uint8_t* redata = (uint8_t*)malloc(length * 3 + 1);
    int ret = -1 ;
    QThread::msleep(5); // 等待5ms一次数据
    ret = wiringPiI2CRawWrite (SCD4XID, reg_addr , 3);
    if (ret == -1){
        qDebug() << "SCD4XID 命令写入失败";
        return false;
    }
    else{
        //qDebug() << "SCD4XID 命令写入成功";
    }
    QThread::msleep(5); // 等待5ms一次数据
    //获取值
    ret = wiringPiI2CRawRead(SCD4XID , redata ,redata_len);
    if (ret == -1){
        qDebug() << "获取值失败";
        return false;
    }
    else{
    }

    for (int i = 0; i < redata_len; i++)
    {
        if ((i + 1) % 3 == 0)
        {
            uint8_t tmp[2];
            tmp[0] = redata[i - 2];
            tmp[1] = redata[i - 1];
            if (SCD4X_Generate_CRC(tmp, 2) != redata[i])
            {
                free(redata);
                return false;
            }

            *rev_data = (redata[i - 2] << 8) | redata[i - 1];
            rev_data++;
        }
    }
    free(redata);
    return true;
}

/***************************************************************************************************************
SCD4X Write Command
****************************************************************************************************************/
void SCD4X::SCD4X_WriteCommand(uint8_t* send_data, uint8_t length)
{
    int ret = -1;

    ret = wiringPiI2CRawWrite (SCD4XID, send_data , length);
    if (ret == -1){
        qDebug() << "SCD4XID 命令写入失败";
        return;
    }
    else{
    // qDebug() << "SCD4XID 命令写入成功";
    }
}

/***************************************************************************************************************
SCD4X Get Device Identity
****************************************************************************************************************/
bool SCD4X::SCD4X_getSerialNumber(void)
{
    uint16_t uid[3] = { 0 };
    uint8_t cmd[3] = { SCD4x_COMMAND_GET_SERIAL_NUMBER >> 8, SCD4x_COMMAND_GET_SERIAL_NUMBER & 0xff, 0x00 };
    cmd[2] = SCD4X_Generate_CRC(cmd, 2);
    if (SCD4X_ReadCommand(cmd, uid, 3) != true)
        return false;
    printf("serial id:0x%x%x%x\r\n", uid[0], uid[1], uid[2]);
    return true;
}

/*************************************************************************************************************
stopPeriodicMeasurement() - Stop periodic measurement and return to idle mode for sensor configuration or to safe energy.
****************************************************************************************************************/
void SCD4X::SCD4X_StopPeriodicMeasurement(void)
{
    uint8_t cmd[3] = { SCD4x_COMMAND_STOP_PERIODIC_MEASUREMENT >> 8, SCD4x_COMMAND_STOP_PERIODIC_MEASUREMENT & 0xff,
                       0x00 };
    cmd[2] = SCD4X_Generate_CRC(cmd, 2);
    SCD4X_WriteCommand(cmd, 3);
    QThread::msleep(500);
}

/*************************************************************************************************************
scd4x_start_periodic_measurement() - start periodic measurement, signal update interval is 5 seconds.
****************************************************************************************************************/
void SCD4X::SCD4X_StartPeriodicMeasurement(void)
{
    uint8_t cmd[3] = { SCD4x_COMMAND_START_PERIODIC_MEASUREMENT >> 8, SCD4x_COMMAND_START_PERIODIC_MEASUREMENT & 0xff,
                       0x00 };
    cmd[2] = SCD4X_Generate_CRC(cmd, 2);
    SCD4X_WriteCommand(cmd, 3);
    for (int var = 0; var < 5; ++var) {
        QThread::msleep(1000);
        qDebug() << var <<"S";
    }

}

///***************************************************************************************************************
//!!!!!!!!!!!!!!only scd41!!!!!!!!!!!!!!!!!!!!!!!!
//On-demand measurement of CO2 concentration, relative humidity and temperature. The sensor output is read
//using the read_measurement command (chapter 3.5.2).
//****************************************************************************************************************/
//void SCD4X_MeasureSingleShot(void)
//{
//    uint8_t cmd[3] = { SCD4x_COMMAND_MEASURE_SINGLE_SHOT >> 8, SCD4x_COMMAND_MEASURE_SINGLE_SHOT & 0xff,
//                       0x00 };
//    cmd[2] = SCD4X_Generate_CRC(cmd, 2);
//    SCD4X_WriteCommand(cmd, 3);
//    QThread::msleep(5000);
//}

///***************************************************************************************************************
//!!!!!!!!!!!!!!only scd41!!!!!!!!!!!!!!!!!!!!!!!!
//On-demand measurement of relative humidity and temperature only. The sensor output is read using the
//read_measurement command (chapter 3.5.2). CO2 output is returned as 0 ppm.
//****************************************************************************************************************/
//void SCD4X_MeasureSingleShotRHTOnly(void)
//{
//    uint8_t cmd[3] = { SCD4x_COMMAND_MEASURE_SINGLE_SHOT_RHT_ONLY >> 8,
//                       SCD4x_COMMAND_MEASURE_SINGLE_SHOT_RHT_ONLY & 0xff,
//                       0x00 };
//    cmd[2] = SCD4X_Generate_CRC(cmd, 2);
//    SCD4X_WriteCommand(cmd, 3);
//    QThread::msleep(50);
//}

///*************************************************************************************************************
//start low power periodic measurement, signal update interval is approximately 30 seconds.
//****************************************************************************************************************/
//void SCD4X_StartLowPowerPeriodicMeasurement(void)
//{
//    uint8_t cmd[3] = { SCD4x_COMMAND_START_LOW_POWER_PERIODIC_MEASUREMENT >> 8,
//                       SCD4x_COMMAND_START_LOW_POWER_PERIODIC_MEASUREMENT & 0xff,
//                       0x00 };
//    cmd[2] = SCD4X_Generate_CRC(cmd, 2);
//    SCD4X_WriteCommand(cmd, 3);
//}

///*************************************************************************************************************
//read data ready status
//****************************************************************************************************************/
bool SCD4X::SCD4X_GetDataReadyStatus(void)
{
    uint16_t status = 0x0000;
    uint8_t cmd[3] = { SCD4x_COMMAND_GET_DATA_READY_STATUS >> 8,
                       SCD4x_COMMAND_GET_DATA_READY_STATUS & 0xff,
                       0x00 };
    cmd[2] = SCD4X_Generate_CRC(cmd, 2);
    if (SCD4X_ReadCommand(cmd, &status, 1) != true)
        return false;
    if ((status & 0x07ff) == 0x0000)
        return (false);
    return true;
}

/*************************************************************************************************************
read sensor output and convert.
****************************************************************************************************************/
bool SCD4X::SCD4X_ReadMeasurement(uint16_t* co2, int32_t* temperature, int32_t* humidity)
{
    if (!SCD4X_GetDataReadyStatus())
        return false;
    uint16_t buf[3] = { 0 };
    uint8_t cmd[3] = { SCD4x_COMMAND_READ_MEASUREMENT >> 8, SCD4x_COMMAND_READ_MEASUREMENT & 0xff,
                       0x00 };
    cmd[2] = SCD4X_Generate_CRC(cmd, 2);

    if (SCD4X_ReadCommand(cmd, buf, 3) != true)
        return false;

    *co2 = buf[0];
    //qDebug() << "co2浓度 = "<<*co2;
    *temperature = ((21875 * (int32_t)buf[1]) >> 13) - 45000;
    *humidity = ((12500 * (int32_t)buf[2]) >> 13);
    return true;
}

/***************************************************************************************************************
Get Automatic Self Calibration Enabled
****************************************************************************************************************/
bool SCD4X::SCD4X_GetAutomaticSelfCalibrationEnabled(uint8_t enable)
{
    uint16_t read_enable = 0x00;
    uint8_t cmd[3] = { SCD4x_COMMAND_GET_AUTOMATIC_SELF_CALIBRATION_ENABLED >> 8,
                       SCD4x_COMMAND_GET_AUTOMATIC_SELF_CALIBRATION_ENABLED & 0xff,
                       0x00 };
    cmd[2] = SCD4X_Generate_CRC(cmd, 2);
    if (SCD4X_ReadCommand(cmd, &read_enable, 1) != true)
        return false;
    else
    {
        if (enable != read_enable)
            return false;
    }
    return true;
}

/***************************************************************************************************************
Set Automatic Self Calibration Enabled
****************************************************************************************************************/
bool SCD4X::SCD4X_SetAutomaticSelfCalibrationEnabled(bool enabled)
{
    uint8_t send_enable[2] = { 0x00, static_cast<uint8_t>(enabled == true ? 0x01 : 0x00) };
    uint8_t cmd[5] = { SCD4x_COMMAND_SET_AUTOMATIC_SELF_CALIBRATION_ENABLED >> 8,
                       SCD4x_COMMAND_SET_AUTOMATIC_SELF_CALIBRATION_ENABLED & 0xff,
                       0x00, static_cast<uint8_t>(enabled == true ? 0x01 : 0x00), 0x00 };
    cmd[4] = SCD4X_Generate_CRC(send_enable, 2);
    SCD4X_WriteCommand(cmd, 5);
    QThread::msleep(10);
    return SCD4X_GetAutomaticSelfCalibrationEnabled(send_enable[1]);
}

/***************************************************************************************************************
To successfully conduct an accurate forced recalibration, the following steps need to be carried out:
1. Operate the SCD4x in the operation mode later used in normal sensor operation (periodic measurement, low power
periodic measurement or single shot) for > 3 minutes in an environment with homogenous and constant CO2
concentration.
2. Issue stop_periodic_measurement. Wait 500 ms for the stop command to complete.
3. Subsequently issue the perform_forced_recalibration command and optionally read out the FRC correction (i.e. the
magnitude of the correction) after waiting for 400 ms for the command to complete.
%?A return value of 0xffff indicates that the forced recalibration has failed.
Note that the sensor will fail to perform a forced recalibration if it was not operated before sending the command. Please make
sure that the sensor is operated at the voltage desired for the application when applying the forced recalibration sequence.
****************************************************************************************************************/
//bool SCD4X_PerformForcedRecalibration(uint16_t target_co2_concentration, uint16_t* frc_correction)
//{
//    // stop potentially previously started measurement
//    SCD4X_StopPeriodicMeasurement();

//    uint8_t crc_data[2] = { target_co2_concentration >> 8, target_co2_concentration & 0xff };
//    uint8_t rev_data[3] = { 0 };
//    uint8_t cmd[5] = { SCD4x_COMMAND_PERFORM_FORCED_CALIBRATION >> 8,
//                       SCD4x_COMMAND_PERFORM_FORCED_CALIBRATION & 0xff,
//                       0x00, 0x00, 0x00 };
//    cmd[2] = crc_data[0];
//    cmd[3] = crc_data[1];
//    cmd[4] = SCD4X_Generate_CRC(crc_data, 2);
//    HAL_I2C_Master_Transmit(&hi2c1, SCD4X_I2CADDR << 0x01, cmd, 5, 100);
//    QThread::msleep(400);
//    HAL_I2C_Master_Receive(&hi2c1, SCD4X_I2CADDR << 0x01, rev_data, 3, 100);

//    if (SCD4X_Generate_CRC(rev_data, 2) != rev_data[2])
//        return false;

//    *frc_correction = ((uint16_t)(rev_data[0] << 8) | rev_data[1]) - 32768;
//    if (rev_data[0] == 0xff && rev_data[1] == 0xff)
//        return false;
//    return true;
//}

/***************************************************************************************************************
per frome self test,need 10 sec,if get 0x0000,it was pass.
****************************************************************************************************************/
//bool SCD4x_PerFormSelfTest(void)
//{
//    uint8_t send_data[3] = { SCD4x_COMMAND_PERFORM_SELF_TEST >> 8,
//                             SCD4x_COMMAND_PERFORM_SELF_TEST & 0xff,
//                             0x00 };
//    uint8_t rev_data[3] = { 0 };
//    send_data[2] = SCD4X_Generate_CRC(send_data, 2);
//    HAL_I2C_Master_Transmit(&hi2c1, SCD4X_I2CADDR << 0x01, send_data, 3, 100);
//    QThread::msleep(10000);
//    HAL_I2C_Master_Receive(&hi2c1, SCD4X_I2CADDR << 0x01, rev_data, 3, 100);
//    if (rev_data[0] != 0 && rev_data[1] != 0)
//    {
//        printf("error code:0x%x%x\r\n", rev_data[0], rev_data[1]);
//        return false;
//    }
//    return true;
//}

/***************************************************************************************************************
The perform_factory_reset command resets all configuration settings stored in the EEPROM and erases the
FRC and ASC algorithm history.
****************************************************************************************************************/
//void SCD4X_PerFormFactoryReset(void)
//{
//    uint8_t cmd[3] = { SCD4x_COMMAND_PERFORM_FACTORY_RESET >> 8,
//                       SCD4x_COMMAND_PERFORM_FACTORY_RESET & 0xff,
//                       0x00 };
//    cmd[2] = SCD4X_Generate_CRC(cmd, 2);
//    SCD4X_WriteCommand(cmd, 3);
//    QThread::msleep(1200);
//}

/***************************************************************************************************************
Configuration settings such as the temperature offset, sensor altitude and the ASC enabled/disabled parameter
are by default stored in the volatile memory (RAM) only and will be lost after a power-cycle. The persist_settings command
stores the current configuration in the EEPROM of the SCD4x, making them persistent across power-cycling. To avoid
unnecessary wear of the EEPROM, the persist_settings command should only be sent when persistence is required and if actual
changes to the configuration have been made. The EEPROM is guaranteed to endure at least 2000 write cycles before failure.
Note that field calibration history (i.e. FRC and ASC, see chapter 3.7) is automatically stored in a separate EEPROM
dimensioned for the specified sensor lifetime.
****************************************************************************************************************/
//void SCD4X_PersistSettings(void)
//{
//    uint8_t cmd[3] = { SCD4x_COMMAND_PERSIST_SETTINGS >> 8,
//                       SCD4x_COMMAND_PERSIST_SETTINGS & 0xff,
//                       0x00 };
//    cmd[2] = SCD4X_Generate_CRC(cmd, 2);
//    SCD4X_WriteCommand(cmd, 3);
//    QThread::msleep(1200);
//}

/***************************************************************************************************************
The reinit command reinitializes the sensor by reloading user settings from EEPROM. Before sending the reinit
command, the stop measurement command must be issued. If the reinit command does not trigger the desired re-initialization,
a power-cycle should be applied to the SCD4x.
****************************************************************************************************************/
//bool SCD4X_reInit(void)
//{
//    uint8_t cmd[3] = { SCD4x_COMMAND_REINIT >> 8,
//                       SCD4x_COMMAND_REINIT & 0xff,
//                       0x00 };
//    cmd[2] = SCD4X_Generate_CRC(cmd, 2);
//    SCD4X_WriteCommand(cmd, 3);
//    QThread::msleep(20);
//}

/***************************************************************************************************************
get_temperature_offset I2C sequence description
****************************************************************************************************************/
//bool SCD4X_GetTemperatureOffset(float* offset)
//{
//    uint16_t rev_data = 0x0000;
//    uint8_t cmd[3] = { SCD4x_COMMAND_GET_TEMPERATURE_OFFSET >> 8,
//                       SCD4x_COMMAND_GET_TEMPERATURE_OFFSET & 0xff,
//                       0x00 };
//    cmd[2] = SCD4X_Generate_CRC(cmd, 2);
//    if (SCD4X_ReadCommand(cmd, &rev_data, 1) != true)
//        return false;
//    *offset = ((float)rev_data) * 175.0 / 65535.0;
//    return true;
//}

/***************************************************************************************************************
The temperature offset has no influence on the SCD4x CO2 accuracy. Setting the temperature offset of the SCD4x
inside the customer device correctly allows the user to leverage the RH and T output signal. Note that the temperature offset
can depend on various factors such as the SCD4x measurement mode, self-heating of close components, the ambient
temperature and air flow. Thus, the SCD4x temperature offset should be determined inside the customer device under its typical
operation conditions (including the operation mode to be used in the application) and in thermal equilibrium. Per default, the
temperature offset is set to 4� C. To save the setting to the EEPROM, the persist setting (see chapter 3.9.1) command must be
issued. Equation (1) shows how the characteristic temperature offset can be obtained.

****************************************************************************************************************/
//bool SCD4X_SetTemperatureOffset(float offset)
//{
//    if (offset < 0.0)
//        return false;
//    uint16_t t_offset = (uint16_t)(offset * 65536 / 175);
//    uint8_t crc_data[2] = { t_offset >> 8, t_offset & 0xff };
//    uint8_t cmd[5] = { SCD4x_COMMAND_SET_TEMPERATURE_OFFSET >> 8,
//                       SCD4x_COMMAND_SET_TEMPERATURE_OFFSET & 0xff,
//                       0x00, 0x00, 0x00 };
//    cmd[2] = crc_data[0];
//    cmd[3] = crc_data[1];
//    cmd[4] = SCD4X_Generate_CRC(crc_data, 2);
//    SCD4X_WriteCommand(cmd, 5);
//    QThread::msleep(1);

//    return true;
//}

/***************************************************************************************************************
Get Sensor Altitude
****************************************************************************************************************/
//bool SCD4X_GetSensorAltitude(uint16_t* altitude)
//{
//    uint8_t cmd[3] = { SCD4x_COMMAND_GET_SENSOR_ALTITUDE >> 8,
//                       SCD4x_COMMAND_GET_SENSOR_ALTITUDE & 0xff,
//                       0x00 };
//    cmd[2] = SCD4X_Generate_CRC(cmd, 2);
//    if (SCD4X_ReadCommand(cmd, altitude, 1) != true)
//        return false;
//    return true;
//}

/***************************************************************************************************************
Reading and writing of the sensor altitude must be done while the SCD4x is in idle mode. Typically, the sensor
altitude is set once after device installation. To save the setting to the EEPROM, the persist setting (see chapter 3.9.1) command
must be issued. Per default, the sensor altitude is set to 0 meter above sea-level.
exp 1950 m:
 SCD4X_SetSensorAltitude(1950);
****************************************************************************************************************/
//void SCD4X_SetSensorAltitude(uint16_t altitude)
//{
//    uint8_t crc_data[2] = { altitude >> 8, altitude & 0xff };
//    uint8_t cmd[5] = { SCD4x_COMMAND_SET_SENSOR_ALTITUDE >> 8,
//                       SCD4x_COMMAND_SET_SENSOR_ALTITUDE & 0xff,
//                       0x00, 0x00, 0x00 };
//    cmd[2] = crc_data[0];
//    cmd[3] = crc_data[1];
//    cmd[4] = SCD4X_Generate_CRC(crc_data, 2);
//    SCD4X_WriteCommand(cmd, 5);
//    QThread::msleep(1);
//}

/***************************************************************************************************************
The set_ambient_pressure command can be sent during periodic measurements to enable continuous pressure
compensation. Note that setting an ambient pressure using set_ambient_pressure overrides any pressure compensation based
on a previously set sensor altitude. Use of this command is highly recommended for applications experiencing significant ambient
pressure changes to ensure sensor accuracy.
exp 98700 Pa:
 SCD4X_SetAmbientPressure(98700);
****************************************************************************************************************/
//void SCD4X_SetAmbientPressure(float pressure)
//{
//    uint8_t crc_data[2] = { ((uint16_t)(pressure / 100) >> 8), ((uint16_t)(pressure / 100) & 0xff) };
//    uint8_t cmd[5] = { SCD4x_COMMAND_SET_AMBIENT_PRESSURE >> 8,
//                       SCD4x_COMMAND_SET_AMBIENT_PRESSURE & 0xff,
//                       0x00, 0x00, 0x00 };
//    cmd[2] = crc_data[0];
//    cmd[3] = crc_data[1];
//    cmd[4] = SCD4X_Generate_CRC(crc_data, 2);
//    SCD4X_WriteCommand(cmd, 5);
//    QThread::msleep(1);
//}

/***************************************************************************************************************
SCD4X Initialization
****************************************************************************************************************/
bool SCD4X::SCD4X_Init(void)
{
    int ret = 0;
    ret = wiringPiSetup();
    if (ret == -1){
        qDebug() << "WiringPi 初始化失败";
        return false;
    }
    else{
        qDebug() << "WiringPi 初始化成功";
    }

    SCD4XID = ret = wiringPiI2CSetup(0x62);
    if (ret == -1){
        qDebug() << "wiringPiI2CSetup 初始化失败";
        return false;
    }
    else{
        qDebug() << "wiringPiI2CSetup 初始化成功";
    }

    // stop potentially previously started measurement
    SCD4X_StopPeriodicMeasurement();

    // Get SCD4X ID
    if (SCD4X_getSerialNumber() != true)
        return false;

    //per frome self test
    //SCD4x_PerFormSelfTest();

    // Enable automatic Self Calibration
    if (SCD4X_SetAutomaticSelfCalibrationEnabled(true) != true)
        return false;
//    SCD4X_StartPeriodicMeasurement();
    return true;
}
