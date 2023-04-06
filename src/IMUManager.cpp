#include "IMUManager.h"
#include <HardwareSerial.h>

Adafruit_BNO08x_RVC *rvc;
BNO08x_RVC_Data *heading = new BNO08x_RVC_Data();

void imu_init()
{
    Serial.println("Adafruit BNO08x IMU - UART-RVC mode");

    // Initialize connect to IMU through serial connection
    Serial1.begin(115200, SERIAL_8N1, 3, 17); // This is the baud rate specified by the datasheet

    while (!Serial1)
        delay(10);

    rvc = new Adafruit_BNO08x_RVC();

    if (!rvc->begin(&Serial1))
    { // connect to the sensor over hardware serial
        Serial.println("Could not find BNO08x!");
        while (1)
            delay(10);
    }
    Serial.println("BNO08x found!");
}