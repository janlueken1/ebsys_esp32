// refer to the default example in ~/.espressif/v6.1/esp-idf/examples/system/esp_timer


#include <stdio.h>
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "timer_example";

static void timer_callback(void *arg)
{
    ESP_LOGI(TAG, "tick");
}

void app_main(void)
{
    const esp_timer_create_args_t timer_args = {
        .callback = timer_callback,
        .name     = "periodic"
    };

    esp_timer_handle_t timer;
    esp_timer_create(&timer_args, &timer);
    esp_timer_start_periodic(timer, 500000); // 500 000 us = 0.5 s

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}