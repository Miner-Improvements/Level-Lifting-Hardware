#include "IMUManager.h"
#include <HardwareSerial.h>

Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

bool imu_init()
{
    Serial.write("Setting up Adafruit BNO08x IMU over I2C... ");

    // Initialize connection to IMU through I2C
    // if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT)) {
    if (!bno08x.begin_I2C()) {
        Serial.write("Failed to find BNO08x chip.\n");
        return false;
    }
    Serial.write("BNO08x found!\n");

    return true;
}

void setReports(void) {
  Serial.write("Setting desired reports\n");
  if (!bno08x.enableReport(SH2_ACCELEROMETER, IMU_REPORT_INTERVAL_US)) {
    Serial.write("Could not enable accelerometer report\n");
  }
  if (!bno08x.enableReport(SH2_GYROSCOPE_CALIBRATED, IMU_REPORT_INTERVAL_US)) {
    Serial.write("Could not enable gyroscope report\n");
  }
  if (!bno08x.enableReport(SH2_GRAVITY, IMU_REPORT_INTERVAL_US)) {
    Serial.write("Could not enable gravity report\n");
  }
}

void printProdIds() {
    for (int n = 0; n < bno08x.prodIds.numEntries; n++) {
        Serial.print("Part ");
        Serial.print(bno08x.prodIds.entry[n].swPartNumber);
        Serial.print(": Version :");
        Serial.print(bno08x.prodIds.entry[n].swVersionMajor);
        Serial.print(".");
        Serial.print(bno08x.prodIds.entry[n].swVersionMinor);
        Serial.print(".");
        Serial.print(bno08x.prodIds.entry[n].swVersionPatch);
        Serial.print(" Build ");
        Serial.println(bno08x.prodIds.entry[n].swBuildNumber);
  }
}