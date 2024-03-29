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
BLECharacteristic *pAngularCharacteristic = nullptr;
BLECharacteristic *pAccelCharacteristic = nullptr;

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

    pAngularCharacteristic = pService_IMU->createCharacteristic(
        CHARACTERISTIC_UUID_ANGULAR,
        BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_READ);
    pAngularCharacteristic->addDescriptor(new BLE2902);

    pAccelCharacteristic = pService_IMU->createCharacteristic(
        CHARACTERISTIC_UUID_ACCEL,
        BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_READ);
    pAccelCharacteristic->addDescriptor(new BLE2902);

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
    // get values from IMU and store in arrays
    float accel_vals[3] = {heading->x_accel, heading->y_accel, heading->z_accel};
    float angular_vals[3] = {heading->yaw, heading->pitch, heading->roll};

    // create byte arrays to store data
    uint8_t *accel_data = new uint8_t[3 * sizeof(float) + sizeof(uint64_t)];
    uint8_t *angular_data = new uint8_t[3 * sizeof(float) + sizeof(uint64_t)];

    // convert vals to byte arrays
    memcpy(accel_data, accel_vals, 3 * sizeof(float));
    memcpy(angular_data, angular_vals, 3 * sizeof(float));

    // get timer value
    uint64_t temp = timerRead(Timer1);

    // add timer value to end of byte arrays
    memcpy(accel_data + 3 * sizeof(float), &temp, sizeof(uint64_t));
    memcpy(angular_data + 3 * sizeof(float), &temp, sizeof(uint64_t));
    // update Accel characteristic
    pAccelCharacteristic->setValue(accel_data, 3 * sizeof(float) + sizeof(uint64_t));
    pAccelCharacteristic->notify();
    // update Angular characteristic
    pAngularCharacteristic->setValue(angular_data, 3 * sizeof(float) + sizeof(uint64_t));
    pAngularCharacteristic->notify();

    delete[] accel_data;
    delete[] angular_data;

    Serial.write("Read successfully!\n");
}

void set_imu_characteristics()
{

    // get values from IMU and store in arrays
    float accel_vals[3] = {DEFAULT_X_ACCEL, DEFAULT_Y_ACCEL, DEFAULT_Z_ACCEL};
    float angular_vals[3] = {DEFAULT_YAW, DEFAULT_PITCH, DEFAULT_ROLL};

    // create byte arrays to store data
    uint8_t *accel_data = new uint8_t[3 * sizeof(float) + sizeof(uint64_t)];
    uint8_t *angular_data = new uint8_t[3 * sizeof(float) + sizeof(uint64_t)];

    // convert vals to byte arrays
    memcpy(accel_data, accel_vals, 3 * sizeof(float));
    memcpy(angular_data, angular_vals, 3 * sizeof(float));

    // get timer value
    uint64_t temp = timerRead(Timer1);

    // add timer value to end of byte arrays
    memcpy(accel_data + 3 * sizeof(float), &temp, sizeof(uint64_t));
    memcpy(angular_data + 3 * sizeof(float), &temp, sizeof(uint64_t));
    // update Accel characteristic
    pAccelCharacteristic->setValue(accel_data, 3 * sizeof(float) + sizeof(uint64_t));
    pAccelCharacteristic->notify();
    // update Angular characteristic
    pAngularCharacteristic->setValue(angular_data, 3 * sizeof(float) + sizeof(uint64_t));
    pAngularCharacteristic->notify();

    delete[] accel_data;
    delete[] angular_data;

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
