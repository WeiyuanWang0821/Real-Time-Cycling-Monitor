/**************************************************************************************************
 * File: sfm3300.h
 * Description: This header file contains the class definition and function prototypes for 
 *              interfacing with the Sensirion SFM3300 flow sensor using the I2C protocol.
 *
 * Sensor: Sensirion SFM3300 Series
 * Features:
 *   - Measures gas flow with high accuracy
 *   - Pre-calibrated for air, O2
 *   - Digital I2C interface for easy integration
 *   - Fast response time
 *   - Low pressure drop
 *
 * This file defines:
 *   - I?C commands for interacting with the SFM3300 sensor
 *   - The `SFM3300` class for sensor initialization and data retrieval
 *   - Function prototypes for:
 *       - Initializing the sensor (`initSFM3300()`)
 *       - Resetting the sensor (`resetSFM3300()`)
 *       - Reading gas flow data (`readSFM3300()`)
 *
 **************************************************************************************************/
#ifndef SFM3300_H
#define SFM3300_H

#include <QObject>
#include <QThread>//Thread header file
#include <QDebug>//Debug header file
//wiringPi firmware header file
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <iostream>
#include <cstdint>  // Import uint16_t and int16_t


#define	SFM3300_OFFSET		32768	// Offset
#define	SFM3300_FLOWCOEFF	120		// Air flow coefficient
/* Device command */
#define SFM3300_FLOWREAD	0x1000	// Read flow: returns 2 bytes
#define SFM3300_IDREAD		0x31AE	// Read ID: returns 4 bytes
#define SFM3300_RESET		0x2000	// Reset command

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
