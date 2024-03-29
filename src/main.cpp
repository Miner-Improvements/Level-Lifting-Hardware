/*  Senior Design 2023 - Miner Improvements: Level Lifting
 *   Information to go here. BLE only right now
 *
 */

#include <esp32-hal.h>
#include <HardwareSerial.h>
#include "SerialManager.h"
#include "BluetoothManager.h"
#include "IMUManager.h"
#include "WorkoutManager.h"
#include "board.h"

bool interruptCounter = false; // Loop occurs after 100 Hz ISR
uint8_t txValue = 0;
hw_timer_t *timer = nullptr;                          // Timer for ISR
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED; // Used for Synchronizing ISR variable interruptCounter

// Polling ISR at 100 Hz to trigger loop
void IRAM_ATTR onTimer()
{
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter = true;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup()
{
  // Wait for serial monitor to open
  serial_init();

  // Initialize IMU
  if (DEBUG != 1)
  {
    imu_init();
  }

  // Initialize Bluetooth
  bluetooth_init();

  // ISR timer with divider of 80, making clock frequency = 1 MHz
  // Alarm occurs at 10000 cycles, making ISR frequency 1000000/10000 = 100 Hz
  // Start timer and enable alarm
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 10000, true);
  timerAlarmEnable(timer);
}

void loop()
{

  // only deal with BLE characteristics when device connected to listen
  if (interruptCounter)
  {
    if (deviceConnected)
    {
      handle_workout();  // handle workout data from IMU if workoutOngoing
      handle_commands(); // handle commands from app
    }

    interruptCounter = 0; // reset for timer ISR to trigger

    // incrementally set txValue for testing purposes, then notify
    // pTxCharacteristic->setValue(&txValue, 1);
    // pTxCharacteristic->notify();
    // txValue++;
    // delay(10); // bluetooth stack will go into congestion, if too many packets are sent
  }

  // disconnecting
  if (!deviceConnected && !advertising)
  {
    handle_disconnect();
  }

  // connecting
  if (deviceConnected && advertising)
  {
    handle_connect();
  }
}
