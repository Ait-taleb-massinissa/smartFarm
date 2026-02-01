#import "Relay.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define RELAY_GPIO GPIO_NUM_23

void relay_init()
{
    gpio_set_direction(RELAY_GPIO, GPIO_MODE_OUTPUT);
    relay_off();
}

void relay_on()
{
    gpio_set_level(RELAY_GPIO, 0);
}

void relay_off()
{
    gpio_set_level(RELAY_GPIO, 1);
}
