#ifndef BATTERY_MANAGER_H
#define BATTERY_MANAGER_H

#include <driver/adc.h>

//read battery voltage per %
extern int * battery_sum;                  // sum of samples taken
extern float battery_voltage;           // calculated voltage
extern float battery_output;            // output value
const float battery_max = 3.7; // maximum voltage of battery
const float battery_min = 3.0; // minimum voltage of battery before shutdown
const float R1 = 100000.0;     // resistance of R1 (100K)
const float R2 = 10000.0;      // resistance of R2 (10K)
const float samples = 500.0;       // number of samples taken
const float volt_ref = 1.1;    // 1.1 V internal ADC reference
const float adc_max = 4096.0;  // maximum 12-bit adc value

void adc_setup();
float read_battery();

#endif