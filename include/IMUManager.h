#ifndef IMU_MANAGER_H
#define IMU_MANAGER_H

#include <Adafruit_BNO08x.h>

// relies on Serial being configured before these functions are called.

// For SPI mode, we also need a RESET
//#define BNO08X_RESET 5
// but not for I2C or UART
#define BNO08X_RESET -1

// I2C uses the following ESP32 GPIO pins by default, and thus we are using 
// them. Should these need to be changed in the future, there should be a 
// way to just configure which GPIO pins I2C uses. Not sure how to do it at
// this time.
//  SDA: 21
//  SCL: 22

extern Adafruit_BNO08x bno08x(BNO08X_RESET);
extern sh2_SensorValue_t sensorValue;

bool imu_init();

void setReports(void);

void printProdIds();

#endif