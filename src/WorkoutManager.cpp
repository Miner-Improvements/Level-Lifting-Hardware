#include "WorkoutManager.h"
#include <HardwareSerial.h>
#include "BluetoothManager.h"
#include "board.h"
#include "IMUManager.h"

bool workoutOngoing = false;
sh2_SensorValue_t sensorEvent;
sh2_Accelerometer_t accelVector;
sh2_Accelerometer_t gravityVector;
sh2_Gyroscope_t gyroVector;

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
/** NEEDS TO BE UPDATED: NEW DATA FORMAT. **/
void handle_workout()
{
    if (workoutOngoing)
    {
        if (DEBUG == 1)
        {
            // set_imu_characteristics();
        }
        else
        {
            // if BNO08x gets reset somehow, reports will need to be set again
            if (bno08x.wasReset()) {
                Serial.write("Sensor was reset! Setting up reports again...\n");
                setReports();
            }
            // Trying this while loop out,might need to be changed.
            // Tead all the reports available each time the interrupt triggers this
            // that we should get reports (every 10ms at the time of writing).
            while(bno08x.getSensorEvent(&sensorValue)) {
                switch (sensorValue.sensorId) {
                    case SH2_ACCELEROMETER:
                        Serial.write("Accelerometer report received.\n");
                        set_accel_characteristic(sensorValue.un.accelerometer);
                        break;
                    case SH2_GYROSCOPE_CALIBRATED:
                        Serial.write("Gyroscope report received.\n");
                        set_gyro_characteristic(sensorValue.un.gyroscope);
                        break;
                    case SH2_GRAVITY:
                        // TDB
                        break;
                }
            }
        }
    }
}