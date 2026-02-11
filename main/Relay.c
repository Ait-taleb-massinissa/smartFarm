#include "Relay.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "RELAY";

// Initialise le relais sur le GPIO donné
void relay_init(gpio_num_t gpio_num)
{
    gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
    gpio_set_level(gpio_num, 1); // OFF initial (LOW active)
    ESP_LOGI(TAG, "Relais initialisé sur GPIO%d", gpio_num);
}

// Allume le relais (LOW = ON)
void relay_on(gpio_num_t gpio_num)
{
    gpio_set_level(gpio_num, 0); // ON
    ESP_LOGI(TAG, "Relais ON sur GPIO%d", gpio_num);
}

// Éteint le relais (HIGH = OFF)
void relay_off(gpio_num_t gpio_num)
{
    gpio_set_level(gpio_num, 1); // OFF
    ESP_LOGI(TAG, "Relais OFF sur GPIO%d", gpio_num);
}
