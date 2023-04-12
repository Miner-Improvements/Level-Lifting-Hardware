#include "BluetoothManager.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <HardwareSerial.h>
#include <Adafruit_BNO08x_RVC.h>

bool deviceConnected = false;
bool advertising = false;
hw_timer_t *Timer1 = nullptr;

BLEServer *pServer = nullptr;
BLEService *pService_UART = nullptr;
BLECharacteristic *pTxCharacteristic = nullptr;
BLECharacteristic *pRxCharacteristic = nullptr;

BLEService *pService_IMU = nullptr;
BLECharacteristic *pYawCharacteristic = nullptr;
BLECharacteristic *pPitchCharacteristic = nullptr;
BLECharacteristic *pRollCharacteristic = nullptr;
BLECharacteristic *pXAccelCharacteristic = nullptr;
BLECharacteristic *pYAccelCharacteristic = nullptr;
BLECharacteristic *pZAccelCharacteristic = nullptr;
BLECharacteristic *pTimestampCharacteristic = nullptr;

BLEAdvertising *pAdvertising = nullptr;

float DEFAULT_YAW = 0.0;
float DEFAULT_PITCH = 0.0;
float DEFAULT_ROLL = 0.0;
float DEFAULT_X_ACCEL = 0.05;
float DEFAULT_Y_ACCEL = 0.05;
float DEFAULT_Z_ACCEL = 9.801;

void MyServerCallbacks::onConnect(BLEServer *pServer)
{
    deviceConnected = true;
    Serial.write("Device connected\n");
};
void MyServerCallbacks::onDisconnect(BLEServer *pServer)
{
    deviceConnected = false;
    Serial.write("Device disconnected\n");
}

void MyRXCharacteristicCallbacks::onWrite(BLECharacteristic *pCharacteristic)
{
    std::string rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0)
    {
        Serial.write("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
            Serial.print(rxValue[i]);

        Serial.write("\n");
        Serial.write("*********");
    }
}

void bluetooth_init()
{
    // Create the BLE Device
    BLEDevice::init(bleServerName);
    Serial.write("BLE device initialized.\n");

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    Serial.write("BLE Server created, callbacks set.\n");

    // Create the BLE Service
    pService_UART = pServer->createService(SERVICE_UUID_UART);
    // NOTE: each characteristic within service needs 3 handles,
    //       so numHandles = 3 * num characteristics
    pService_IMU = pServer->createService(BLEUUID(SERVICE_UUID_IMU), 21);
    Serial.write("BLE Services created.\n");

    // Create characteristics and add descriptors
    pTxCharacteristic = pService_UART->createCharacteristic(
        CHARACTERISTIC_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_READ);
    pTxCharacteristic->addDescriptor(new BLE2902()); // not sure what this does exactly

    pRxCharacteristic = pService_UART->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ);
    pRxCharacteristic->addDescriptor(new BLE2902());
    pRxCharacteristic->setCallbacks(new MyRXCharacteristicCallbacks());

    pYawCharacteristic = pService_IMU->createCharacteristic(
        CHARACTERISTIC_UUID_YAW,
        BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_READ);
    pYawCharacteristic->addDescriptor(new BLE2902);

    pPitchCharacteristic = pService_IMU->createCharacteristic(
        CHARACTERISTIC_UUID_PITCH,
        BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_READ);
    pPitchCharacteristic->addDescriptor(new BLE2902);

    pRollCharacteristic = pService_IMU->createCharacteristic(
        CHARACTERISTIC_UUID_ROLL,
        BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_READ);
    pRollCharacteristic->addDescriptor(new BLE2902);

    pXAccelCharacteristic = pService_IMU->createCharacteristic(
        CHARACTERISTIC_UUID_X_ACCEL,
        BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_READ);
    pXAccelCharacteristic->addDescriptor(new BLE2902);

    pYAccelCharacteristic = pService_IMU->createCharacteristic(
        CHARACTERISTIC_UUID_Y_ACCEL,
        BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_READ);
    pYAccelCharacteristic->addDescriptor(new BLE2902);

    pZAccelCharacteristic = pService_IMU->createCharacteristic(
        CHARACTERISTIC_UUID_Z_ACCEL,
        BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_READ);
    pZAccelCharacteristic->addDescriptor(new BLE2902);

    pTimestampCharacteristic = pService_IMU->createCharacteristic(
        CHARACTERISTIC_UUID_TIMESTAMP,
        BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_READ);
    pTimestampCharacteristic->addDescriptor(new BLE2902);

    Serial.write("BLE characteristics, descriptors, and callbacks set.");

    // Start the service
    pService_UART->start();
    pService_IMU->start();
    Serial.write("BLE Services started.\n");

    // Start advertising
    pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID_UART);
    pAdvertising->addServiceUUID(SERVICE_UUID_IMU);
    pServer->getAdvertising()->start();
    advertising = true;
    Serial.write("BLE advertising started.\n");

    // log waiting status
    Serial.write("Waiting for connection...\n");

    // TODO: Make sure this is initializing the timer properly
    Timer1 = timerBegin(1, 2, true);
    Serial.write("Timer1 started.\n");
}

void set_imu_characteristics(BNO08x_RVC_Data *heading)
{
    // write IMU data to characteristics and notify for each
    // TODO: dtmn if notifications for each characterisitc are necessary
    //       could just notify timer and let that be the indicator
    pYawCharacteristic->setValue(heading->yaw);
    pYawCharacteristic->notify();
    // Serial.write("Yaw updated.\n");
    pPitchCharacteristic->setValue(heading->pitch);
    pPitchCharacteristic->notify();
    // Serial.write("Pitch updated.\n");
    pRollCharacteristic->setValue(heading->roll);
    pRollCharacteristic->notify();
    // Serial.write("Roll updated.\n");
    pXAccelCharacteristic->setValue(heading->x_accel);
    pXAccelCharacteristic->notify();
    // Serial.write("X accel updated.\n");
    pYAccelCharacteristic->setValue(heading->y_accel);
    pYAccelCharacteristic->notify();
    // Serial.write("Y accel updated.\n");
    pZAccelCharacteristic->setValue(heading->z_accel);
    pZAccelCharacteristic->notify();
    // Serial.write("Z accel updated.\n");
    uint64_t temp = timerRead(Timer1);
    pTimestampCharacteristic->setValue((uint8_t *)&temp, 8);
    pTimestampCharacteristic->notify();
    // Serial.write("Timer updated.\n");

    Serial.write("Read successfully!\n");
}

void set_imu_characteristics()
{

    float imu_vals[3] = {DEFAULT_X_ACCEL, DEFAULT_Y_ACCEL, DEFAULT_Z_ACCEL};
    uint8_t *data = new uint8_t[3 * sizeof(float) + sizeof(uint64_t)];
    // convert imu_vals to byte array
    memcpy(data, imu_vals, 3 * sizeof(float));
    // convert timer to byte array
    uint64_t temp = timerRead(Timer1);
    memcpy(data + 3 * sizeof(float), &temp, sizeof(uint64_t));
    // pYawCharacteristic->setValue(DEFAULT_YAW);
    // // Serial.write("Yaw updated.\n");
    // pPitchCharacteristic->setValue(DEFAULT_PITCH);
    // // Serial.write("Pitch updated.\n");
    // pRollCharacteristic->setValue(DEFAULT_ROLL);
    // // Serial.write("Roll updated.\n");
    // pXAccelCharacteristic->setValue(DEFAULT_X_ACCEL);
    // // Serial.write("X accel updated.\n");
    // pYAccelCharacteristic->setValue(DEFAULT_Y_ACCEL);
    // Serial.write("Y accel updated.\n");
    pZAccelCharacteristic->setValue(data, 3 * sizeof(float) + sizeof(uint64_t));
    pZAccelCharacteristic->notify();
    // Serial.write("Z accel updated.\n");
    delete[] data;

    // pTimestampCharacteristic->setValue((uint8_t *)&temp, 8);
    // Serial.write("Timer updated.\n");

    Serial.write("Read successfully!\n");
}

void set_tx_characteristic(std::string data)
{
    pTxCharacteristic->setValue(data);
    pTxCharacteristic->notify();
}

void clear_rx_characteristic()
{
    pRxCharacteristic->setValue("");
}

void handle_disconnect()
{
    delay(500);                  // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.write("Advertising started...\n");
    advertising = true;
}

void handle_connect()
{
    // do stuff here on connecting
    advertising = false; // could also be put in onConnected callback
}

std::string get_rx_data()
{
    std::string rx_data = pRxCharacteristic->getValue();
    return rx_data;
}
