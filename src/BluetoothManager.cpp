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
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
            Serial.print(rxValue[i]);

        Serial.println();
        Serial.println("*********");
    }
}

void bluetooth_init()
{
    // Create the BLE Device
    BLEDevice::init(bleServerName);
    Serial.println("BLE device initialized.");

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    Serial.println("BLE Server created, callbacks set.");

    // Create the BLE Service
    pService_UART = pServer->createService(SERVICE_UUID_UART);
    // NOTE: each characteristic within service needs 3 handles,
    //       so numHandles = 3 * num characteristics
    pService_IMU = pServer->createService(BLEUUID(SERVICE_UUID_IMU), 21);
    Serial.println("BLE Services created.");

    // Create characteristics and add descriptors
    pTxCharacteristic = pService_UART->createCharacteristic(
        CHARACTERISTIC_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_READ);
    pTxCharacteristic->addDescriptor(new BLE2902()); // not sure what this does exactly

    pRxCharacteristic = pService_UART->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE);
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

    Serial.println("BLE characteristics, descriptors, and callbacks set.");

    // Start the service
    pService_UART->start();
    pService_IMU->start();
    Serial.println("BLE Services started.");

    // Start advertising
    pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID_UART);
    pAdvertising->addServiceUUID(SERVICE_UUID_IMU);
    pServer->getAdvertising()->start();
    advertising = true;
    Serial.println("BLE advertising started.");

    // log waiting status
    Serial.println("Waiting for connection...");

    // TODO: Make sure this is initializing the timer properly
    Timer1 = timerBegin(1, 2, true);
    Serial.println("Timer1 started.");
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

void handle_disconnect()
{
    delay(500);                  // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("Advertising started...");
    advertising = true;
}

void handle_connect()
{
    // do stuff here on connecting
    advertising = false; // could also be put in onConnected callback
}