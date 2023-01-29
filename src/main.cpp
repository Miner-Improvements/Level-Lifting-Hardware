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
  Serial.begin(115200);

  // Log status
  Serial.print("Setting up device...");

  // Create the BLE Device
  BLEDevice::init(bleServerName);

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *helloWorldService = pServer->createService(SERVICE_UUID);

  // Add BLE Characteristic and Add a BLE Descriptor
    helloWorldService->addCharacteristic(&helloWorldCharacteristic);
    helloWorldDescriptor.setValue("Hello World message");
    helloWorldCharacteristic.addDescriptor(&helloWorldDescriptor);
  
  // Start the service
  helloWorldService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  if (deviceConnected) {
    if ((millis() - lastTime) > timerDelay) {
      
      // create random num to append to msg
      unsigned long num = random();
      
      // Create message string and append num
      std::string msg = "Hello World! ";
      msg += (std:string)num;

      helloWorldCharacteristic.setValue(msg);
      helloWorldCharacteristic.notify();
      Serial.print(msg.c_str());
      
      lastTime = millis();
    }
  }
}
