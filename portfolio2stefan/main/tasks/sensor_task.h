#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

void temperature_sensor_publish_task(void *pvParameters);

void innitialise_semaphore();

bool is_semaphore_taken();