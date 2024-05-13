#include "sensor_manager.h"
#include "esp_log.h"
#include "driver/adc.h"
#include <stdbool.h>
#include "../tasks/sensor_task.h"
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// Using analog LMT86 temperature sensor

// For logging
static const char *TAG = "sensor_manager";

void initialise_temperature_sensor() {
    // Initialize the temperature sensor ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_12);  // using pin 34 for requirement
    ESP_LOGI(TAG, "Temperature sensor initialized");
    innitialise_semaphore();
}

float get_temperature_reading() {
    // Read the ADC value and convert it to temperature

    uint32_t raw_adc = 0;
    for (int i = 0; i < 6; i++) {
        raw_adc += adc1_get_raw(ADC1_CHANNEL_6);
    }
    raw_adc /= 6;


    float voltage = raw_adc * (3.3f / 4095.0f);  // Convert ADC value to voltage in volts
    ESP_LOGI(TAG, "Voltage reading: %.2f V", voltage);  //for debugging

    float V_temp_mV = voltage * 1000;  // Convert voltage to millivolts for formula compatibility

    // linear approximation for temperature between 0°C and 30°C
    
    //voltage drop per degree C; V_drop = (V_at_0 - V_at_30) / (0 - 30) = 10.77 mV/°C
    float V_drop = 10.77; 
    int V_at_30 = 1777; //mV
    int max_temp = 30; //degrees
    float temperature = (V_temp_mV - V_at_30) / -V_drop + max_temp;

    ESP_LOGI(TAG, "Temperature reading: %.2f degrees", temperature);
    return temperature;
}
