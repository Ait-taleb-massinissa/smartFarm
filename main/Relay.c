#import "Relay.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define RELAY_GPIO GPIO_NUM_23

void relay_init()
{
    gpio_set_direction(RELAY_GPIO, GPIO_MODE_OUTPUT);
    relay_off();
}

void relay_on()
{
    ESP_LOGI("RELAY", "Relay ON");
    gpio_set_level(RELAY_GPIO, 0);
}

void relay_off()
{
    ESP_LOGI("RELAY", "Relay OFF");
    gpio_set_level(RELAY_GPIO, 1);
}
