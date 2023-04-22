#include "WorkoutManager.h"
#include <HardwareSerial.h>
#include "BluetoothManager.h"
#include "board.h"
#include "IMUManager.h"
#include <Adafruit_BNO08x.h>

extern Adafruit_BNO08x bno08x;
extern sh2_SensorValue_t sensorValue;
bool workoutOngoing = false;

void handle_commands()
{
    std::string rx_data = get_rx_data();
    if (rx_data.length() > 0)
    {
        Serial.write("Received command:\n");
        Serial.write(rx_data.c_str());
        if (rx_data == "start_workout" || rx_data == "start_workout\n")
        {
            Serial.write("Starting workout...\n");
            set_tx_characteristic("starting_workout");
            workoutOngoing = true;
        }
        else if (rx_data == "stop_workout")
        {
            Serial.write("Stopping workout...\n");
            set_tx_characteristic("stopping_workout");
            workoutOngoing = false;
        }
        clear_rx_characteristic();
    }
}

void handle_workout()
{
    if (workoutOngoing)
    {
        if (DEBUG == 1)
        {
            set_imu_characteristics_DEBUG();
        }
        else
        {
            // if BNO08x gets reset somehow, reports will need to be set again
            if (bno08x.wasReset()) {
                Serial.write("Sensor was reset! Setting up reports again...\n");
                setReports();
            }
            // Read one report each time the interrupt triggers this
            // We should get reports (every 10ms at the time of writing).
            if(!bno08x.getSensorEvent(&sensorValue)) {
                Serial.write("Couldn't get sensor report!\n");
            }
            else {
                set_imu_characteristic();
            }
        }
    }
}