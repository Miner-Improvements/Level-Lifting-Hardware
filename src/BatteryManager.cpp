#include "BatteryManager.h"
#include <HardwareSerial.h>

int battery = 0;
int battery_sum = 0;                  // sum of samples taken
float battery_voltage = 0.0;           // calculated voltage
float battery_output = 0.0;            // output value

void adc_setup()
{
    adc2_config_channel_atten(ADC2_CHANNEL_0, ADC_ATTEN_0db); //set ref voltage internally
}

float read_battery()
{
    for (int i = 0; i < samples; i++)
    {
         adc2_get_raw(ADC2_CHANNEL_0, ADC_WIDTH_12Bit, &battery);
         battery_sum+=battery;
    }

    battery_voltage = battery_sum / samples;
    battery_voltage = (battery_voltage / adc_max) * volt_ref; // for internal 1.1v reference
    // use it with divider circuit 
    // voltage = voltage / (R2/(R1+R2));
    // round value by two DP
    //battery_voltage = roundf(battery_voltage * 100) / 100;
    Serial.print("voltage: ");
    Serial.println(battery_voltage, 2);
    battery_output = ((battery_voltage - battery_min) / (battery_max - battery_min)) * 100;
    if (battery_output < 100)
        return battery_output;
    else
        return 100.0f;
}