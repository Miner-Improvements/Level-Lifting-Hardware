#include "IMUManager.h"
#include <HardwareSerial.h>

Adafruit_BNO08x bno08x;
sh2_SensorValue_t sensorValue;

/** NEEDS TO BE UPDATED. **/
bool imu_init()
{
    Serial.write("Setting up Adafruit BNO08x IMU over I2C... ");

    // Initialize connection to IMU through I2C
    // if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT)) {
    if (!bno08x.begin_I2C()) {
        Serial.println("Failed to find BNO08x chip.");
        return false;
    }
    Serial.write("BNO08x found!\n");
}

void setReports(void) {
  Serial.write("Setting desired reports\n");
  if (!bno08x.enableReport(SH2_ACCELEROMETER)) {
    Serial.write("Could not enable accelerometer\n");
  }
  if (!bno08x.enableReport(SH2_GYROSCOPE_CALIBRATED)) {
    Serial.write("Could not enable gyroscope\n");
  }
//   if (!bno08x.enableReport(SH2_MAGNETIC_FIELD_CALIBRATED)) {
//     Serial.println("Could not enable magnetic field calibrated");
//   }
//   if (!bno08x.enableReport(SH2_LINEAR_ACCELERATION)) {
//     Serial.println("Could not enable linear acceleration");
//   }
  if (!bno08x.enableReport(SH2_GRAVITY)) {
    Serial.write("Could not enable gravity vector\n");
  }
//   if (!bno08x.enableReport(SH2_ROTATION_VECTOR)) {
//     Serial.println("Could not enable rotation vector");
//   }
//   if (!bno08x.enableReport(SH2_GEOMAGNETIC_ROTATION_VECTOR)) {
//     Serial.println("Could not enable geomagnetic rotation vector");
//   }
//   if (!bno08x.enableReport(SH2_GAME_ROTATION_VECTOR)) {
//     Serial.println("Could not enable game rotation vector");
//   }
//   if (!bno08x.enableReport(SH2_STEP_COUNTER)) {
//     Serial.println("Could not enable step counter");
//   }
//   if (!bno08x.enableReport(SH2_STABILITY_CLASSIFIER)) {
//     Serial.println("Could not enable stability classifier");
//   }
//   if (!bno08x.enableReport(SH2_RAW_ACCELEROMETER)) {
//     Serial.println("Could not enable raw accelerometer");
//   }
//   if (!bno08x.enableReport(SH2_RAW_GYROSCOPE)) {
//     Serial.println("Could not enable raw gyroscope");
//   }
//   if (!bno08x.enableReport(SH2_RAW_MAGNETOMETER)) {
//     Serial.println("Could not enable raw magnetometer");
//   }
//   if (!bno08x.enableReport(SH2_SHAKE_DETECTOR)) {
//     Serial.println("Could not enable shake detector");
//   }
//   if (!bno08x.enableReport(SH2_PERSONAL_ACTIVITY_CLASSIFIER)) {
//     Serial.println("Could not enable personal activity classifier");
//   }
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