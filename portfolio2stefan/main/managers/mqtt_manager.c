#include "mqtt_manager.h"
#include "esp_log.h"
#include "../config.h"
#include "tasks/sensor_task.h"
#include <cJSON.h>
#include "esp_timer.h"
#include <string.h>


static const char *TAG = "mqtt_manager";
static esp_mqtt_client_handle_t mqtt_client = NULL;

static void mqtt_event_handler(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    switch (event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT Connected");
            //subscribe to topics
            esp_mqtt_client_subscribe(event->client, CONFIG_MQTT_COMMAND_TOPIC, 0);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT Disconnected");
            //if disconnected, stop tasks
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "Message Published");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "Subscribed to topic");
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "Unsubscribed from topic");
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT event data");
            // Allocate memory for the topic string
            char *topicStr = malloc(event->topic_len + 1);
            if (topicStr) {
                //if sending from console using mosquitto_pub remember to escape the quotes (" -> \")
                memcpy(topicStr, event->topic, event->topic_len);
                topicStr[event->topic_len] = '\0';  // Null-terminate the topic
                ESP_LOGI(TAG, "Message received on topic: %s", topicStr);

                // Call the handle_mqtt_data function with null-terminated strings
                handle_mqtt_data(topicStr, event->data, event->data_len);

                free(topicStr);
            } else {
                ESP_LOGE(TAG, "Failed to allocate memory for topic string");
            }
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT event error");
            break;
        default:
            ESP_LOGI(TAG, "Other MQTT event occurred: %ld", event_id);
            break;
    }
}

void mqtt_app_start(void) {
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_MQTT_BROKER,
        .credentials.username = CONFIG_MQTT_USERNAME,
        .credentials.authentication.password = CONFIG_MQTT_PASSWORD,
    };
    ESP_LOGI(TAG, "MQTT credentials: %s, %s", mqtt_cfg.credentials.username, mqtt_cfg.credentials.authentication.password);

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

esp_mqtt_client_handle_t get_mqtt_client(void) {
    return mqtt_client;  // Return the handle to the MQTT client
}
//test with mosquitto_pub -h 10.126.50.109 -t "lock/command" -u "com.dl.mqtt" -P "avd85hfk" -m '{\"R\": 0, \"G\": 0, \"B\": 1}'
// Declare the handle_mqtt_data function
void handle_mqtt_data(const char* topic, const char* data, int data_len) {
    // Log the entire message with specified length
    ESP_LOGI(TAG, "Received data on %s: %.*s", topic, data_len, data);

    // If the topic is "device/commands"
    if(strcmp(topic, CONFIG_MQTT_COMMAND_TOPIC) == 0) {
        ESP_LOGI(TAG, "Parsing command");

        // Check if the data starts with "measure:"
        if (strncmp(data, "measure:", 8) == 0) {
            // Pointer to start of the numbers after "measure:"
            const char *numbers = data + 8;

            // Use sscanf to parse the numbers from the string
            int num_responses, delay;
            if (sscanf(numbers, "%d,%d", &num_responses, &delay) == 2) {
                // Successfully parsed both numbers
                ESP_LOGI(TAG, "Parsed command measure with num_responses: %d and delay: %d", num_responses, delay);

                // Call function to start the temperature sensor task
                startTemperatureSensorTask(num_responses, delay);
            } else {
                // Parsing error
                ESP_LOGE(TAG, "Failed to parse command parameters.");
            }
        } else {
            ESP_LOGE(TAG, "Unknown command format.");
        }
    }
}

void startTemperatureSensorTask(int num_responses, int delay)
{
    ESP_LOGI(TAG, "Starting temperature sensor task");

    TaskParameters *parameters = malloc(sizeof(TaskParameters));
    if (parameters)
    {
        parameters->num_responses = num_responses;
        parameters->delay = delay;
        parameters->current_time = esp_timer_get_time() / 1000;

        if (xTaskCreate(temperature_sensor_publish_task, "temperature_sensor_publish_task", 4096, parameters, 5, NULL) != pdPASS)
        {
            ESP_LOGE(TAG, "Failed to start task");
            free(parameters); // Free memory if task fails to start
        }
    }
    else
    {
        ESP_LOGE(TAG, "Failed to allocate memory for task parameters");
    }

}
