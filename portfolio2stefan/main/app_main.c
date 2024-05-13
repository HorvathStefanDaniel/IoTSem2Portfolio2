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
#include "config.h"
#include "esp_timer.h"

static const char *TAG = "app_main";

void app_main(void) {
    ESP_LOGI(TAG, "Application starting...");
    initialise_temperature_sensor();
    //might need to make a setup function that just initializes everything, but this is fine for now
    wifi_init_sta();    //after wifi connects, mqtt will be started and tasks will be created
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay for 2 seconds to allow connection to be established
    

    ESP_LOGI(TAG, "Application started");

    //fast testing
    for(int i = 0; i < 10; i++) {
        TaskParameters *params = malloc(sizeof(TaskParameters));
        params->num_responses = 5;
        params->delay = 1000;
        params->current_time = esp_timer_get_time() / 1000;
        xTaskCreate(temperature_sensor_publish_task, "temperature_sensor_publish_task", 4096, params, 1, NULL);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
    }


}