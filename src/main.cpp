/*********
  Rui Santos
  Complete instructions at https://RandomNerdTutorials.com/esp32-ble-server-client/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <esp32-hal.h>
#include <HardwareSerial.h>

//BLE server name
#define bleServerName "HelloWorldServer"

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

bool deviceConnected = false;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID "ac9a41ba-9764-41a6-837f-fc08f2b29d28"

// set up hello world characteristic and descriptor to be added later
BLECharacteristic helloWorldCharacteristic("ca73b3ba-39f6-4ab3-91ae-186dc9577d99", BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor helloWorldDescriptor(BLEUUID((uint16_t)0x2903));

//Setup callbacks onConnect and onDisconnect
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

void setup() {
  // Start serial communication 
  Serial.begin(9600);

 

  // Create the BLE Device
  BLEDevice::init(bleServerName);
  Serial.println("BLE Device initialized.");

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  Serial.println("BLE Server created.");

  // Create the BLE Service
  BLEService *helloWorldService = pServer->createService(SERVICE_UUID);
  Serial.println("BLE Service created.");

  // Add BLE Characteristic and Add a BLE Descriptor
  helloWorldService->addCharacteristic(&helloWorldCharacteristic);
  helloWorldDescriptor.setValue("Hello World message");
  helloWorldCharacteristic.addDescriptor(&helloWorldDescriptor);
  Serial.println("BLE characteristic and descriptor set.");
  
  // Start the service
  helloWorldService->start();
  Serial.println("BLE Service started.");

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();
  Serial.println("BLE advertising started.");
  
  // log waiting status
  Serial.println("Waiting for connection to notify...");
}

void loop() {
  if (deviceConnected) {
    if ((millis() - lastTime) > timerDelay) {
      
      // append random long to msg string
      char msg_charArray[100];
      sprintf(msg_charArray, "Hello World!  ID: %ld", random());
      Serial.print(msg_charArray);

      // set characteristic value. needs to be std string
      std::string msg_stdString(msg_charArray);
      helloWorldCharacteristic.setValue(msg_stdString);

      // notify clients the value was changed
      helloWorldCharacteristic.notify();
      
      lastTime = millis();
    }
  } 
}
