#include "sensor_task.h"
#include "cJSON.h"
#include "../managers/sensor_manager.h"
#include "../managers/mqtt_manager.h"
#include "mqtt_client.h"
#include "../config.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"


SemaphoreHandle_t temperature_task_semaphore = NULL;

void innitialise_semaphore()
{
    temperature_task_semaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(temperature_task_semaphore);
}

bool is_semaphore_taken()
{
    return xSemaphoreTake(temperature_task_semaphore, 0);
}

//for logging
static const char *TAG = "sensor_task";

//expecting parameters to be the number of times the loop should run and the delay between each loop
void temperature_sensor_publish_task(void *pvParameters) {
    if(!is_semaphore_taken())
    {
        ESP_LOGE(TAG, "Semaphore is taken, cannot run task");
        free(pvParameters);
        vTaskDelete(NULL);
        return;
    }

    
    ESP_LOGI(TAG, "Starting temperature sensor task");
    const char *topic = MQTT_RESPONSE_TOPIC;
    

    //use the struct in the config file to get the parameters
    TaskParameters *parameters = (TaskParameters *)pvParameters;

    int loop_count = parameters->num_responses;
    int delay = parameters->delay;

    while (loop_count > 0) {
        //delay is before the reading according to the reqs
        vTaskDelay(pdMS_TO_TICKS(delay));

        uint32_t current_time = esp_timer_get_time() / 1000;
        cJSON *root = cJSON_CreateObject();

        //add current loop number to the response JSON according to requirement
        cJSON_AddNumberToObject(root, "loops_left", loop_count - 1);

        cJSON_AddNumberToObject(root, "temperature", get_temperature_reading());

        //get current time
        uint32_t time_diff = current_time - ((int *)pvParameters)[2];
        //make string of time_diff
        char time_diff_c[10];
        sprintf(time_diff_c, "%lu", time_diff);

        cJSON_AddStringToObject(root, "time_dif", time_diff_c);

        char *message = cJSON_Print(root);
        esp_mqtt_client_publish(get_mqtt_client(), topic, message, strlen(message), 1, 0);
        cJSON_Delete(root);
        free(message);
        loop_count--;
    }

    
    // Clean up parameter struct and self-delete the task
    free(parameters);
    xSemaphoreGive(temperature_task_semaphore);
    vTaskDelete(NULL); // Pass NULL to delete the calling task
}
