#ifndef IMU_MANAGER_H
#define IMU_MANAGER_H

#include <Adafruit_BNO08x.h>
#include <Adafruit_BNO08x_RVC.h>

extern Adafruit_BNO08x_RVC *rvc;
extern BNO08x_RVC_Data *heading;

void imu_init();

#endif