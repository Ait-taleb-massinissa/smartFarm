#include "Lamp.h"
#include "esp_log.h"
#include "driver/gpio.h"

static const char *TAG = "LAMP";
static int lamp_state = 0; // 0 = éteinte, 1 = allumée

// Initialise le GPIO de la lampe en mode sortie
void lamp_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LAMP_GPIO_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};

    gpio_config(&io_conf);

    // Éteindre la lampe au démarrage
    gpio_set_level(LAMP_GPIO_PIN, 0);
    lamp_state = 0;

    ESP_LOGI(TAG, "Lampe initialisée sur GPIO %d", LAMP_GPIO_PIN);
}

// Allume la lampe (met le GPIO à 3.3V)
void lamp_turn_on(void)
{
    gpio_set_level(LAMP_GPIO_PIN, 1);
    lamp_state = 1;
    ESP_LOGI(TAG, "Lampe allumée");
}

// Éteint la lampe (met le GPIO à 0V)
void lamp_turn_off(void)
{
    gpio_set_level(LAMP_GPIO_PIN, 0);
    lamp_state = 0;
    ESP_LOGI(TAG, "Lampe éteinte");
}

// Inverse l'état de la lampe
void lamp_toggle(void)
{
    if (lamp_state == 0)
    {
        lamp_turn_on();
    }
    else
    {
        lamp_turn_off();
    }
}

// Retourne l'état actuel de la lampe
int lamp_get_state(void)
{
    return lamp_state;
}
