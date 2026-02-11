#include "EventGroup.h"
#include "freertos/event_groups.h"
#include "freertos/FreeRTOS.h"
#include "Relay.h"
#include "Servo.h"
#include "esp_log.h"

#define BIT_GAZ_ALERT (1 << 0)
#define BIT_GAZ_NORMAL (1 << 1)
#define BIT_TEMP_ALERT (1 << 2)
#define BIT_TEMP_NORMAL (1 << 3)
#define BIT_LIGHT_ALERT (1 << 4)
#define BIT_LIGHT_NORMAL (1 << 5)
#define BIT_MOISTURE_ALERT (1 << 6)
#define BIT_MOISTURE_NORMAL (1 << 7)
#define BIT_WATER_ALERT (1 << 8)
#define BIT_WATER_NORMAL (1 << 9)

EventGroupHandle_t event_group_handle;

void temp_event_group_handler(void *pvParameters)
{
    while (1)
    {
        EventBits_t bits = xEventGroupWaitBits(
            event_group_handle,
            BIT_TEMP_ALERT | BIT_TEMP_NORMAL,
            pdTRUE, // clear ONLY the bit that occurred
            pdFALSE,
            portMAX_DELAY);

        if (bits & BIT_TEMP_ALERT)
        {
            ESP_LOGW("EVENT_GROUP", "🚨 DANGER TEMP DETECTED!");
            relay_on(GPIO_NUM_23);     // Activer le relais pour la ventilation
            servo_ouvrir(GPIO_NUM_18); // Ouvrir la fenetre
            servo_ouvrir(GPIO_NUM_19); // Ouvrir la fenetre
            // servo_ouvrir(GPIO_NUM_21); // Ouvrir la porte
        }

        if (bits & BIT_TEMP_NORMAL)
        {
            ESP_LOGI("EVENT_GROUP", "✅ Temp normal.");
            relay_off(GPIO_NUM_23);
            servo_fermer(GPIO_NUM_18); // Fermer la fenetre
            servo_fermer(GPIO_NUM_19); // Fermer la fenetre
            // servo_fermer(GPIO_NUM_21); // Fermer la porte
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }
}

void gaz_event_group_handler(void *pvParameters)
{
    while (1)
    {
        EventBits_t bits = xEventGroupWaitBits(
            event_group_handle,
            BIT_GAZ_ALERT | BIT_GAZ_NORMAL,
            pdTRUE, // clears ONLY the triggered bit
            pdFALSE,
            portMAX_DELAY);

        if (bits & BIT_GAZ_ALERT)
        {
            ESP_LOGW("EVENT_GROUP", "🚨 DANGER GAZ DETECTED!");
            relay_on(GPIO_NUM_23);
            servo_ouvrir(GPIO_NUM_18); // Ouvrir la fenetre
            servo_ouvrir(GPIO_NUM_19); // Ouvrir la fenetre
            // servo_ouvrir(GPIO_NUM_21); // Ouvrir la porte
        }

        if (bits & BIT_GAZ_NORMAL)
        {
            ESP_LOGI("EVENT_GROUP", "✅ Gaz normal.");
            relay_off(GPIO_NUM_23);
            servo_fermer(GPIO_NUM_18); // Fermer la fenetre
            servo_fermer(GPIO_NUM_19); // Fermer la fenetre
            // servo_fermer(GPIO_NUM_21); // Fermer la porte
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }
}

void light_event_group_handler(void *pvParameters)
{
    while (1)
    {
        EventBits_t bits = xEventGroupWaitBits(
            event_group_handle,
            BIT_LIGHT_ALERT | BIT_LIGHT_NORMAL,
            pdTRUE,
            pdFALSE,
            portMAX_DELAY);

        if (bits & BIT_LIGHT_ALERT)
        {
            ESP_LOGW("EVENT_GROUP", "🚨 LOW LIGHT DETECTED!");
            relay_on(GPIO_NUM_25); // Activer le relais pour la lumière
        }

        if (bits & BIT_LIGHT_NORMAL)
        {
            ESP_LOGI("EVENT_GROUP", "✅ Light normal.");
            relay_off(GPIO_NUM_25); // Désactiver le relais pour la lumière
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }
}

void moisture_event_group_handler(void *pvParameters)
{
    while (1)
    {
        EventBits_t bits = xEventGroupWaitBits(
            event_group_handle,
            BIT_MOISTURE_ALERT | BIT_MOISTURE_NORMAL,
            pdTRUE,
            pdFALSE,
            portMAX_DELAY);

        if (bits & BIT_MOISTURE_ALERT)
        {
            ESP_LOGW("EVENT_GROUP", "🚨 DRY SOIL DETECTED!");
            relay_on(GPIO_NUM_22);
        }

        if (bits & BIT_MOISTURE_NORMAL)
        {
            ESP_LOGI("EVENT_GROUP", "✅ Soil normal.");
            relay_off(GPIO_NUM_22);
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }
}

void water_event_group_handler(void *pvParameters)
{
    while (1)
    {
        EventBits_t bits = xEventGroupWaitBits(
            event_group_handle,
            BIT_WATER_ALERT | BIT_WATER_NORMAL,
            pdTRUE,
            pdFALSE,
            portMAX_DELAY);

        if (bits & BIT_WATER_ALERT)
        {
            ESP_LOGW("EVENT_GROUP", "🚨 WATER NEEDED DETECTED!");
            relay_on(GPIO_NUM_22);
        }

        if (bits & BIT_WATER_NORMAL)
        {
            ESP_LOGI("EVENT_GROUP", "✅ Water normal.");
            relay_off(GPIO_NUM_22);
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }
}

void start_event_group_handler()
{
    event_group_handle = xEventGroupCreate();
    xTaskCreate((TaskFunction_t)temp_event_group_handler, "TempEventGroupHandler", 4096, NULL, 5, NULL);
    xTaskCreate((TaskFunction_t)light_event_group_handler, "LightEventGroupHandler", 4096, NULL, 5, NULL);
    xTaskCreate((TaskFunction_t)moisture_event_group_handler, "MoistureEventGroupHandler", 4096, NULL, 5, NULL);
    xTaskCreate((TaskFunction_t)water_event_group_handler, "WaterEventGroupHandler", 4096, NULL, 5, NULL);
    xTaskCreate((TaskFunction_t)gaz_event_group_handler, "GazEventGroupHandler", 4096, NULL, 5, NULL);
}