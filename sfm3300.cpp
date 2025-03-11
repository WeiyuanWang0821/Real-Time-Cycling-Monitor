/**************************************************************************************************
 * File: sfm3300.cpp
 * Description: This file implements the communication and data processing functions for the 
 *              Sensirion SFM3300 flow sensor using the I2C protocol.
 *
 * Sensor: Sensirion SFM3300 Series
 * Features:
 *   - Measures gas flow with high accuracy
 *   - Pre-calibrated for air, O2
 *   - Digital I2C interface for easy integration
 *   - Fast response time
 *   - Low pressure drop
 *
 * Functions implemented in this file:
 *   - Initialize the sensor (`initSFM3300()`)
 *   - Reset the sensor (`resetSFM3300()`)
 *   - Read gas flow data (`readSFM3300()`)
 *
 **************************************************************************************************/

#include "sfm3300.h"

SFM3300::SFM3300(QObject *parent)
    : QObject{parent}
{

}

bool SFM3300::resetSFM3300(void)//Reset flow sensor
{
    uint8_t data[2] = {0};
    data[0] = SFM3300_RESET >> 8;
    data[1] = SFM3300_RESET & 0XFF;

    if (wiringPiI2CRawWrite (SFM3300ID, data,2) == -1){
        qDebug() << "SFM3300RESET failure";
        return false;
    }
    else{
        qDebug() << "SFM3300RESET success";
    }
    return true;
}

bool SFM3300::initSFM3300(void)
{
    bool ret = false;

    if (wiringPiSetup() == -1){
        qDebug() << "wiringPiSetup initialization failure";
        return false;
    }
    else{
        qDebug() << "WiringPi initialization success";
    }

    SFM3300ID = wiringPiI2CSetup(0x40);
    if (SFM3300ID == -1){
        qDebug() << "wiringPiI2CSetup initialization failure";
        return false;
    }
    else{
        qDebug() << "wiringPiI2CSetup initialization success";
    }

    QThread::msleep(100);

    if (resetSFM3300() == false){// Reset
        qDebug() << "resetSFM3300 failure";
        return false;
    }
    else{
        qDebug() << "SFM3300RESET success";
    }

    QThread::msleep(100);
    float afmGetVal = 0;

    // the previously acquired data are inaccurate, remove
    for(uint8_t i = 0; i < 5; i++){
        readSFM3300(&afmGetVal);
    }
    return true;
}

bool SFM3300::readSFM3300(float *sfmGetVal)
{
    uint8_t sfmVal[3] = {0};	//2 bytes of data + 1 byte of CRC
    uint8_t data[2] = {0};
    float sfmGetValTemp = 0;
    data[0] = SFM3300_FLOWREAD >> 8;
    data[1] = SFM3300_FLOWREAD & 0XFF;

    bool ret = false;
    QThread::msleep(5); // Wait at least 5ms between each data read

    if (wiringPiI2CRawWrite (SFM3300ID, data , 2) == -1){
        qDebug() << "SFM3300_FLOWREAD Command write failure";
        return false;
    }
    else{
    // qDebug() << "SFM3300_FLOWREAD Command write success";
    }

    QThread::msleep(5); // Wait at least 5ms between each data read

    //Get gas flow rate
    if (wiringPiI2CRawRead(SFM3300ID , sfmVal ,3) == -1){
        //qDebug() << "Failed to obtain the gas flow";
        return false;
    }
    else{
    // qDebug() << "afmVal"<<afmVal[0];
    // qDebug() << "afmVal"<<afmVal[1];
    // qDebug() << "afmVal"<<afmVal[2];
    }

    //Calculate the flow value
    sfmGetValTemp = (sfmVal[0]<<8 | sfmVal[1]);
    sfmGetValTemp = (sfmGetValTemp - SFM3300_OFFSET) / 1.0 / SFM3300_FLOWCOEFF; // Calculate the data using the formula: ((measurement - offset) / flow coefficient)
    *sfmGetVal = static_cast<float>(sfmGetValTemp);
    //qDebug() << "gas flow=" << *sfmGetVal;
    return true;
}
