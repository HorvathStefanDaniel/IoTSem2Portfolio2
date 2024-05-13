#pragma once

// WiFi credentials
#define WIFI_SSID "StefanIOT"
#define WIFI_PASSWORD "stefaniot"

// MQTT server configuration
#define MQTT_BROKER "mqtt://10.126.50.109"
#define MQTT_PORT 1883
#define MQTT_USERNAME "com.dl.mqtt"  // replace this with secret
#define MQTT_PASSWORD "avd85hfk"     // replace this with secret or set from a secure source oops

// MQTT topics
#define MQTT_RESPONSE_TOPIC "temperature/publish"
#define MQTT_COMMAND_TOPIC "device/commands"

//structs
typedef struct {
    int num_responses;
    int delay;
    uint32_t current_time;  // Correct type to match the value being stored
} TaskParameters;