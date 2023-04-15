#include "WorkoutManager.h"
#include <HardwareSerial.h>
#include "BluetoothManager.h"
#include "board.h"
#include "IMUManager.h"

bool workoutOngoing = false;

void handle_commands()
{
    std::string rx_data = get_rx_data();
    if (rx_data.length() > 0)
    {
        Serial.write("Received command: ");
        Serial.write(rx_data.c_str());
        Serial.write("\n");
        if (rx_data == "start_workout")
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
            set_imu_characteristics();
        }
        else
        {
            if (!(rvc->read(heading)))
            {                               // read rvc data
                Serial.write("Not Read\n"); // indiciate if unsuccessful
            }
            else
            {
                set_imu_characteristics(heading); // set characteristics to be sent to client
            }
        }
    }
}