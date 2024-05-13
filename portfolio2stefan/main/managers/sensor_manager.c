#include "sensor_manager.h"
#include "esp_log.h"
#include "driver/adc.h"
#include <stdbool.h>

#include <math.h>

// Using analog LMT86 temperature sensor

// For logging
static const char *TAG = "sensor_manager";

void initialise_temperature_sensor() {
    // Initialize the temperature sensor ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_0);  // assuming GPIO 34 is used
    ESP_LOGI(TAG, "Temperature sensor initialized");
}

float get_temperature_reading() {
    // Read the ADC value and convert it to temperature
    int raw_adc = adc1_get_raw(ADC1_CHANNEL_6);
    float voltage = raw_adc * (3.3f / 4095.0f);  // Convert ADC value to voltage in volts
    ESP_LOGI(TAG, "Voltage reading: %.2f V", voltage);
    float V_temp_mV = voltage * 1000;  // Convert voltage to millivolts for formula compatibility

    // Simplified linear approximation to calculate temperature
    float temperature = (V_temp_mV - 1788) / -8.6 + 20;

    ESP_LOGI(TAG, "Temperature reading: %.2f degrees", temperature);
    return temperature;
}