#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "../config.h"

static const char *TAG = "wifi_manager";

// This function handles different WiFi and IP related events
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();
                ESP_LOGI(TAG, "Wi-Fi STA started, attempting to connect...");
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                wifi_event_sta_disconnected_t* disc = (wifi_event_sta_disconnected_t*) event_data;
                ESP_LOGI(TAG, "Wi-Fi STA disconnected, reason: %d, attempting to reconnect...", disc->reason);
                esp_wifi_connect();
                break;
            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI(TAG, "Wi-Fi STA connected.");
                break;  // Optionally add more actions if needed
            default:
                ESP_LOGW(TAG, "Unhandled Wi-Fi event: %li", event_id);
                break;
        }
    } else if (event_base == IP_EVENT) {
        switch (event_id) {
            case IP_EVENT_STA_GOT_IP:
                ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
                ESP_LOGI(TAG, "Got IP Address: " IPSTR, IP2STR(&event->ip_info.ip));
                mqtt_app_start(); // Start MQTT connection here
                break;
            default:
                ESP_LOGE(TAG, "Unhandled IP event: %li", event_id);
                break;
        }
    } else {
        ESP_LOGE(TAG, "Unknown event base: %s", event_base);
    }
}


// Initialize and start the Wi-Fi as station
void wifi_init_sta(void) {
    ESP_LOGI(TAG, "Initializing Wi-Fi...");
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL));

    esp_wifi_connect(); // Attempt to connect to the configured AP
    ESP_LOGI(TAG, "Wi-Fi setup complete.");
}
