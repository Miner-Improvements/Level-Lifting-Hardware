/*
#include <Arduino.h>
#include <BluetoothSerial.h>
#include <HardwareSerial.h>
#include <Adafruit_BNO08x.h> 
*/


/*
#define LED_RED 19
#define LED_GREEN 18
#define LED_BLUE 15
#define UART_TX 17
#define UART_RX 16


HardwareSerial SerialPort(2); //use UART2

void setup() {

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(UART_TX, OUTPUT);
  pinMode(UART_RX, INPUT);
  SerialPort.begin(9600, SERIAL_8N1, UART_RX, UART_TX);
  
}

void loop() {
if (SerialPort.available())
  {
    float position = SerialPort.read();
  }
}
*/

/*
void setup()
{
pinMode(22, OUTPUT); // Set GPIO22 as digital output pin
}

void loop() 
{
digitalWrite(22, HIGH); // Set GPIO22 active high
delay(1000);  // delay of one second
digitalWrite(22, LOW); // Set GPIO22 active low
delay(1000); // delay of one second
}
*/

/* Test sketch for Adafruit BNO08x sensor in UART-RVC mode */

#include "Adafruit_BNO08x_RVC.h"

Adafruit_BNO08x_RVC rvc = Adafruit_BNO08x_RVC();

void setup() {
  // Wait for serial monitor to open
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println("Adafruit BNO08x IMU - UART-RVC mode");

  Serial1.begin(115200); // This is the baud rate specified by the datasheet
  while (!Serial1)
    delay(10);

  if (!rvc.begin(&Serial1)) { // connect to the sensor over hardware serial
    Serial.println("Could not find BNO08x!");
    while (1)
      delay(10);
  }

  Serial.println("BNO08x found!");
}

void loop() {
  BNO08x_RVC_Data heading;

  if (!rvc.read(&heading)) {
    return;
  }

  Serial.println();
  Serial.println(F("---------------------------------------"));
  Serial.println(F("Principal Axes:"));
  Serial.println(F("---------------------------------------"));
  Serial.print(F("Yaw: "));
  Serial.print(heading.yaw);
  Serial.print(F("\tPitch: "));
  Serial.print(heading.pitch);
  Serial.print(F("\tRoll: "));
  Serial.println(heading.roll);
  Serial.println(F("---------------------------------------"));
  Serial.println(F("Acceleration"));
  Serial.println(F("---------------------------------------"));
  Serial.print(F("X: "));
  Serial.print(heading.x_accel);
  Serial.print(F("\tY: "));
  Serial.print(heading.y_accel);
  Serial.print(F("\tZ: "));
  Serial.println(heading.z_accel);
  Serial.println(F("---------------------------------------"));


  //  delay(200);
}
