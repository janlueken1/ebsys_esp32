/*
 See default example in ~/.espressif/v6.1/esp-idf/examples/system/temp_sensor
 and/or documentation in
 https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/peripherals/temp_sensor.html
 */

#include <stdio.h>
#include "esp_log.h"
#include "driver/temperature_sensor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "temp";

void app_main(void)
{
    temperature_sensor_handle_t sensor = NULL;

    temperature_sensor_config_t config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
    temperature_sensor_install(&config, &sensor);
    temperature_sensor_enable(sensor);

    float temp;
    while (1) {
        temperature_sensor_get_celsius(sensor, &temp);
        ESP_LOGI(TAG, "Temperature: %.2f °C", temp);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}