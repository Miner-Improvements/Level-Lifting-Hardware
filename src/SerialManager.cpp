#include "SerialManager.h"
#include <HardwareSerial.h>

void serial_init()
{
    Serial.begin(115200);
    while (!Serial)
        delay(10);
}