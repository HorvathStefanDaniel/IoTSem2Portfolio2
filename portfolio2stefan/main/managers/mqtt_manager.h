#pragma once

#include "mqtt_client.h"  // Include the MQTT Client header for the handle type

void mqtt_app_start(void);
esp_mqtt_client_handle_t get_mqtt_client(void);  // Function to access the MQTT client handle
void handle_mqtt_data(const char* topic, const char* data, int data_len);