#ifndef BLUETOOTH_MANAGER_H
#define BLUETOOTH_MANAGER_H

#include <BLEServer.h>
#include <Adafruit_BNO08x.h>

#define bleServerName "LevelLifting Wearable Tracker"

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID_UART "bf854c21-e070-446c-9849-305c7188a693"
#define CHARACTERISTIC_UUID_TX "78628d2b-0008-4d46-b2c4-b755bf2c8c01"
#define CHARACTERISTIC_UUID_RX "d95dc802-137e-4d13-a3f1-6f384193b7d7"
#define SERVICE_UUID_IMU "91da3684-137e-4473-8fd2-b112126b19e1"
#define CHARACTERISTIC_UUID_ANGULAR "694b27b3-7c84-4a7e-a58c-baf08b34cc0c"
#define CHARACTERISTIC_UUID_ACCEL "aa744b51-7483-4d48-b973-df3eb0a05ff7"

// forward declarations
class BLEServer;
class BLEService;
class BLECharacteristic;
class BLEAdvertising;
typedef struct hw_timer_s hw_timer_t;

extern bool deviceConnected;
extern bool advertising;
extern hw_timer_t *Timer1;
extern sh2_Accelerometer_t currentGravityVector;

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

void set_imu_characteristic();

void set_imu_characteristics_DEBUG();

void handle_disconnect();

void handle_connect();

void set_tx_characteristic(std::string data);

std::string get_rx_data();

void clear_rx_characteristic();

#endif