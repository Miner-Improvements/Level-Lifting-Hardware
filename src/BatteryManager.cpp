#include "BatteryManager.h"
#include <HardwareSerial.h>

int battery = 0;
int battery_sum = 0;                  // sum of samples taken
float battery_voltage = 0.0;           // calculated voltage
float battery_output = 0.0;            // output value
esp_adc_cal_characteristics_t *adc_chars = (esp_adc_cal_characteristics_t*)(calloc(1, sizeof(esp_adc_cal_characteristics_t)));

void adc_setup()
{
    //adc2_config_channel_atten(ADC2_CHANNEL_0, ADC_ATTEN_11db); //set ref voltage internally

 //Characterize ADC at particular atten

    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_12, 1100, adc_chars);
    //Check type of calibration value used to characterize ADC
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("eFuse Vref");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Two Point");
    } else {
        printf("Default");
    }

}

float read_battery()
{
    for (int i = 0; i < samples; i++)
    {
         adc2_get_raw(ADC2_CHANNEL_0, ADC_WIDTH_12Bit, &battery);
         battery_sum+=battery;
    }

    battery_voltage = battery_sum / samples;
    /*battery_voltage = (battery_voltage / adc_max) * volt_ref; // for internal 1.1v reference
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
        */

    battery_voltage = esp_adc_cal_raw_to_voltage(battery_voltage, adc_chars);
    Serial.print("voltage: ");
    Serial.println(battery_voltage, 2);
    return battery_output;
}