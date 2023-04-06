#ifndef BLUETOOTH_MANAGER_H
#define BLUETOOTH_MANAGER_H

#include <BLEServer.h>

#define bleServerName "LevelLifting Wearable Tracker"

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID_UART "ac9a41ba-9764-41a6-837f-fc08f2b29d28"
#define CHARACTERISTIC_UUID_TX "ac9a41ba-9764-41a6-837f-fc08f2b29d28"
#define CHARACTERISTIC_UUID_RX "ac9a41ba-9764-41a6-837f-fc08f2b29d28"
#define SERVICE_UUID_IMU "91da3684-137e-4473-8fd2-b112126b19e1"
#define CHARACTERISTIC_UUID_YAW "694b27b3-7c84-4a7e-a58c-baf08b34cc0c"
#define CHARACTERISTIC_UUID_PITCH "58499525-a4cf-4bb9-9b16-a6ec73065923"
#define CHARACTERISTIC_UUID_ROLL "7a5e5ca3-db92-41e5-807e-24f19751179f"
#define CHARACTERISTIC_UUID_X_ACCEL "f881bd9e-1a9e-447c-b0a7-298c1069489e"
#define CHARACTERISTIC_UUID_Y_ACCEL "c58d3d80-bb7b-4d22-9b1c-55a4c39e7308"
#define CHARACTERISTIC_UUID_Z_ACCEL "aa744b51-7483-4d48-b973-df3eb0a05ff7"
#define CHARACTERISTIC_UUID_TIMESTAMP "4a7c0369-4188-47f3-b1ee-82784ee99d2a"

// forward declarations
class BLEServer;
class BLEService;
class BLECharacteristic;
class BLEAdvertising;
typedef struct hw_timer_s hw_timer_t;
typedef struct BNO08xRVCData BNO08x_RVC_Data;

extern bool deviceConnected;
extern bool advertising;
extern hw_timer_t *Timer1;

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer);
  void onDisconnect(BLEServer *pServer);
};

class MyRXCharacteristicCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic);
};

void bluetooth_init();

void set_imu_characteristics(BNO08x_RVC_Data *heading);

void handle_disconnect();

void handle_connect();

#endif