/* Blink Example

   This example code is a modified version of the example script provided by espressif

*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG = "example";

/*
    Define GPIO pin of the LED
    For the board at hand (ESP32-C3-DevKit-RUST), the LED GPIO pin is 7.
    For most other ESP-C3 devkits, it is at GPIO pin 8. Refer to the appropriate devkit manual to find out.
*/
#define BLINK_GPIO 10
static uint8_t s_led_state = 0;


// To configure the GPIO LED as opposed to the LED strip:
#ifndef CONFIG_BLINK_LED_GPIO
#error "configured wrong LED; run 'idf.py menuconfig' to change"
#endif

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

void app_main(void)
{
    configure_led();
    while (1) {
        ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        gpio_set_level(BLINK_GPIO, s_led_state);
        s_led_state = !s_led_state;
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}
