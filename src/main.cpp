#include <Arduino.h>
#include <BluetoothSerial.h>
#include <HardwareSerial.h>
#include <Adafruit_BNO08x.h> 



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