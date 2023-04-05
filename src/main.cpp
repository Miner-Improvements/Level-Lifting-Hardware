/*  Senior Design 2023 - Miner Improvements: Level Lifting
*   Information to go here. BLE only right now
* 
*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <esp32-hal.h>
#include <HardwareSerial.h>
#include "Adafruit_BNO08x.h"
#include "Adafruit_BNO08x_RVC.h"
#include "HardwareSerial.h"


#define bleServerName "LevelLifting Wearable Tracker"

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID_UART               "ac9a41ba-9764-41a6-837f-fc08f2b29d28"
#define CHARACTERISTIC_UUID_TX          "ac9a41ba-9764-41a6-837f-fc08f2b29d28"
#define CHARACTERISTIC_UUID_RX          "ac9a41ba-9764-41a6-837f-fc08f2b29d28"
#define SERVICE_UUID_IMU                "91da3684-137e-4473-8fd2-b112126b19e1"
#define CHARACTERISTIC_UUID_YAW         "694b27b3-7c84-4a7e-a58c-baf08b34cc0c"
#define CHARACTERISTIC_UUID_PITCH       "58499525-a4cf-4bb9-9b16-a6ec73065923"
#define CHARACTERISTIC_UUID_ROLL        "7a5e5ca3-db92-41e5-807e-24f19751179f"
#define CHARACTERISTIC_UUID_X_ACCEL     "f881bd9e-1a9e-447c-b0a7-298c1069489e"
#define CHARACTERISTIC_UUID_Y_ACCEL     "c58d3d80-bb7b-4d22-9b1c-55a4c39e7308"
#define CHARACTERISTIC_UUID_Z_ACCEL     "aa744b51-7483-4d48-b973-df3eb0a05ff7"
#define CHARACTERISTIC_UUID_TIMESTAMP   "4a7c0369-4188-47f3-b1ee-82784ee99d2a"

// setup variable needed to run this program
#pragma region Variables

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

bool interruptCounter = false; // Loop occurs after 100 Hz ISR 
bool deviceConnected = false;
bool advertising = false; 
uint8_t txValue = 0;
hw_timer_t *timer = nullptr; // Timer for ISR
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED; // Used for Synchronizing ISR variable interruptCounter
hw_timer_t *Timer1 = nullptr;

Adafruit_BNO08x_RVC rvc = Adafruit_BNO08x_RVC();
BNO08x_RVC_Data heading;

#pragma endregion Variables

// ***** Start Setup Callbacks *****
#pragma region Callbacks

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.write("Device connected\n");
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.write("Device disconnected\n");
  }
};

class MyRXCharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
          Serial.print(rxValue[i]);

        Serial.println();
        Serial.println("*********");
      }
    }
};

#pragma endregion Callbacks
// ***** End Setup Callbacks *****

//Polling ISR at 100 Hz to trigger loop
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter = true;
  portEXIT_CRITICAL_ISR(&timerMux);
}


void setup() {
  // Wait for serial monitor to open
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println("Adafruit BNO08x IMU - UART-RVC mode");
  
  // Initialize connect to IMU through serial connection
  Serial1.begin(115200, SERIAL_8N1, 3, 17); // This is the baud rate specified by the datasheet
  while (!Serial1)
    delay(10);

  if (!rvc.begin(&Serial1)) { // connect to the sensor over hardware serial
    Serial.println("Could not find BNO08x!");
    while (1)
      delay(10);
  }
  Serial.println("BNO08x found!");

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
  #pragma region Characteristics
  pTxCharacteristic = pService_UART->createCharacteristic(
                        CHARACTERISTIC_UUID_TX,
                        BLECharacteristic::PROPERTY_NOTIFY |
                        BLECharacteristic::PROPERTY_READ
                      );
  pTxCharacteristic->addDescriptor(new BLE2902()); // not sure what this does exactly

  pRxCharacteristic = pService_UART->createCharacteristic(
                        CHARACTERISTIC_UUID_RX,
                        BLECharacteristic::PROPERTY_WRITE
                      );
  pRxCharacteristic->addDescriptor(new BLE2902());
  pRxCharacteristic->setCallbacks(new MyRXCharacteristicCallbacks());

  pYawCharacteristic = pService_IMU->createCharacteristic(
                         CHARACTERISTIC_UUID_YAW,
                         BLECharacteristic::PROPERTY_NOTIFY |
                         BLECharacteristic::PROPERTY_READ
  );
  pYawCharacteristic->addDescriptor(new BLE2902); 

  pPitchCharacteristic = pService_IMU->createCharacteristic(
                           CHARACTERISTIC_UUID_PITCH,
                           BLECharacteristic::PROPERTY_NOTIFY |
                           BLECharacteristic::PROPERTY_READ
  );
  pPitchCharacteristic->addDescriptor(new BLE2902); 

  pRollCharacteristic = pService_IMU->createCharacteristic(
                          CHARACTERISTIC_UUID_ROLL,
                          BLECharacteristic::PROPERTY_NOTIFY |
                          BLECharacteristic::PROPERTY_READ
  );
  pRollCharacteristic->addDescriptor(new BLE2902); 

  pXAccelCharacteristic = pService_IMU->createCharacteristic(
                            CHARACTERISTIC_UUID_X_ACCEL,
                            BLECharacteristic::PROPERTY_NOTIFY |
                            BLECharacteristic::PROPERTY_READ
  );
  pXAccelCharacteristic->addDescriptor(new BLE2902);

  pYAccelCharacteristic = pService_IMU->createCharacteristic(
                            CHARACTERISTIC_UUID_Y_ACCEL,
                            BLECharacteristic::PROPERTY_NOTIFY |
                            BLECharacteristic::PROPERTY_READ
  );
  pYAccelCharacteristic->addDescriptor(new BLE2902); 

  pZAccelCharacteristic = pService_IMU->createCharacteristic(
                            CHARACTERISTIC_UUID_Z_ACCEL,
                            BLECharacteristic::PROPERTY_NOTIFY |
                            BLECharacteristic::PROPERTY_READ
  );
  pZAccelCharacteristic->addDescriptor(new BLE2902);

  pTimestampCharacteristic = pService_IMU->createCharacteristic(
                               CHARACTERISTIC_UUID_TIMESTAMP,
                               BLECharacteristic::PROPERTY_NOTIFY |
                               BLECharacteristic::PROPERTY_READ
  );
  pTimestampCharacteristic->addDescriptor(new BLE2902);

  Serial.println("BLE characteristics, descriptors, and callbacks set.");
  
  #pragma endregion Characteristics

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
  Timer1 = timerBegin(1,2,true);
  Serial.println("Timer1 started.");

  // ISR timer with divider of 80, making clock frequency = 1 MHz
  // Alarm occurs at 10000 cycles, making ISR frequency 1000000/10000 = 100 Hz
  // Start timer and enable alarm
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 10000, true);
  timerAlarmEnable(timer);
}

void loop() {
if(interruptCounter)
{
  if (!rvc.read(&heading)) {
    Serial.write("Not Read\n");
    // return;
  }
  else {
    if (deviceConnected) {
      // write IMU data to characteristics and notify for each
      pYawCharacteristic->setValue(heading.yaw);
      pYawCharacteristic->notify();
      // Serial.write("Yaw updated.\n");
      pPitchCharacteristic->setValue(heading.pitch);
      pPitchCharacteristic->notify();
      // Serial.write("Pitch updated.\n");
      pRollCharacteristic->setValue(heading.roll);
      pRollCharacteristic->notify();
      // Serial.write("Roll updated.\n");
      pXAccelCharacteristic->setValue(heading.x_accel);
      pXAccelCharacteristic->notify();
      // Serial.write("X accel updated.\n");
      pYAccelCharacteristic->setValue(heading.y_accel);
      pYAccelCharacteristic->notify();
      // Serial.write("Y accel updated.\n");
      pZAccelCharacteristic->setValue(heading.z_accel);
      pZAccelCharacteristic->notify();
      // Serial.write("Z accel updated.\n");
      uint64_t temp = timerRead(Timer1);
      pTimestampCharacteristic->setValue((uint8_t*)&temp, 8);
      pTimestampCharacteristic->notify();
      // Serial.write("Timer updated.\n");
    }

    Serial.write("Read successfully!\n");

    // write IMU to serial
/*    Serial.println();
    Serial.println(F("---------------------------------------"));
    Serial.println(F("Principal Axes:"));
    Serial.println(F("---------------------------------------"));
    Serial.print(F("Yaw: "));
    Serial.print(heading.yaw);
    Serial.print(F("   Pitch: "));
    Serial.print(heading.pitch);
    Serial.print(F("   Roll: "));
    Serial.println(heading.roll);
    Serial.println(F("---------------------------------------"));
    Serial.println(F("Acceleration:"));
    Serial.println(F("---------------------------------------"));
    Serial.print(F("X: "));
    Serial.print(heading.x_accel);
    Serial.print(F("        Y: "));
    Serial.print(heading.y_accel);
    Serial.print(F("        Z: "));
    Serial.println(heading.z_accel);
    Serial.println(F("---------------------------------------"));
*/
  }

  if (deviceConnected) {
    // incrementally set txValue for testing purposes, then notify
    pTxCharacteristic->setValue(&txValue, 1);
    pTxCharacteristic->notify();
    txValue++;
    //delay(10); // bluetooth stack will go into congestion, if too many packets are sent
	}

  // disconnecting
  if (!deviceConnected && !advertising) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("Advertising started...");
    advertising = true;
  }
  
  // connecting
  if (deviceConnected && advertising) {
    // do stuff here on connecting
    advertising = false; // could also be put in onConnected callback
  }

  //delay(10); // delay to match IMU data rate of 100Hz
  interruptCounter = 0;
}
}
