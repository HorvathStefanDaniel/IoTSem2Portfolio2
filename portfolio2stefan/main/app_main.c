#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"

//managers and tasks
#include "managers/wifi_manager.h"
#include "managers/sensor_manager.h"
#include "managers/mqtt_manager.h"
#include "tasks/sensor_task.h"

static const char *TAG = "app_main";

void app_main(void) {
    ESP_LOGI(TAG, "Application starting...");
    initialise_temperature_sensor();
    //might need to make a setup function that just initializes everything, but this is fine for now
    wifi_init_sta();    //after wifi connects, mqtt will be started and tasks will be created
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay for 2 seconds to allow connection to be established
    
    ESP_LOGI(TAG, "Application started");

}