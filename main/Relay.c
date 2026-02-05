/*
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
*/
#include "Relay.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// Tâche de contrôle du relais
void relay_task(void *pvParameters)
{
    // Récupérer le numéro de pin depuis pvParameters
    gpio_num_t relay_gpio = (gpio_num_t)(uintptr_t)pvParamete0rs;
    
    ESP_LOGI("RELAY", "Tache relais demarree sur GPIO%d", relay_gpio);
    
    // Initialiser le GPIO
    gpio_set_direction(relay_gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(relay_gpio, 1);  // OFF initial
    
    while (1)
    {
        // Allumer le relais
        ESP_LOGI("RELAY", "Relais ON (GPIO%d)", relay_gpio);
        gpio_set_level(relay_gpio, 0);
        vTaskDelay(pdMS_TO_TICKS(2000));
        
        // Éteindre le relais
        ESP_LOGI("RELAY", "Relais OFF (GPIO%d)", relay_gpio);
        gpio_set_level(relay_gpio, 1);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void relay_start_task(gpio_num_t gpio_num)
{
    xTaskCreate(relay_task,"relay_task",2048,(void *)gpio_num,3,NULL);
}