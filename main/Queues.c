#include "Queues.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "EventGroup.h"
#include <string.h>
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

QueueHandle_t TempQueue;
QueueHandle_t LightQueue;
QueueHandle_t MoistureQueue;
QueueHandle_t WaterQueue;
QueueHandle_t GasQueue;

void create_queues(void)
{
    TempQueue = xQueueCreate(10, sizeof(float));
    LightQueue = xQueueCreate(10, sizeof(float));
    MoistureQueue = xQueueCreate(10, sizeof(float));
    WaterQueue = xQueueCreate(10, sizeof(float));
    GasQueue = xQueueCreate(10, sizeof(uint16_t));
}

void temp_queue_handler()
{
    bool temp_alert = false;
    float temp_value;
    if (xQueueReceive(TempQueue, &temp_value, portMAX_DELAY))
    {
        if (temp_value > 30.0 && !temp_alert)
        {
            // Handle high temperature alert
            ESP_LOGI("TEMP_QUEUE", "🚨 DANGER TEMP! Valeur: %.2f \n", temp_value);
            xEventGroupSetBits(event_group_handle, BIT_TEMP_ALERT);
            temp_alert = true;
        }
        else if (temp_value <= 30.0 && temp_alert)
        {
            // Handle normal temperature processing
            ESP_LOGI("TEMP_QUEUE", "✅ Temp normal. Valeur: %.2f \n", temp_value);
            temp_alert = false;
            xEventGroupSetBits(event_group_handle, BIT_TEMP_NORMAL);
        }
    }
}

void light_queue_handler()
{
    bool light_alert = false;
    float light_value;
    if (xQueueReceive(LightQueue, &light_value, portMAX_DELAY))
    {
        if (light_value < 50.0 && !light_alert)
        {
            ESP_LOGI("LIGHT_QUEUE", "🚨 LOW LIGHT! Valeur: %.2f \n", light_value);
            light_alert = true;
            xEventGroupSetBits(event_group_handle, BIT_LIGHT_ALERT);
        }
        else if (light_value >= 50.0 && light_alert)
        {
            ESP_LOGI("LIGHT_QUEUE", "✅ Light normal. Valeur: %.2f \n", light_value);
            light_alert = false;
            xEventGroupSetBits(event_group_handle, BIT_LIGHT_NORMAL);
        }
    }
}

void moisture_queue_handler()
{
    bool moisture_alert = false;
    char *moisture_status;
    if (xQueueReceive(MoistureQueue, &moisture_status, portMAX_DELAY))
    {
        if (strcmp(moisture_status, "Dry") == 0)
        {
            ESP_LOGI("MOISTURE_QUEUE", "🚨 DRY SOIL! Status: %s \n", moisture_status);
            moisture_alert = true;
            xEventGroupSetBits(event_group_handle, BIT_MOISTURE_ALERT);
        }
        else if (strcmp(moisture_status, "Wet") == 0 && moisture_alert)
        {
            ESP_LOGI("MOISTURE_QUEUE", "✅ Soil normal. Status: %s \n", moisture_status);
            moisture_alert = false;
            xEventGroupSetBits(event_group_handle, BIT_MOISTURE_NORMAL);
        }
    }
}

void water_queue_handler()
{
    bool water_alert = false;
    char *water_status;
    if (xQueueReceive(WaterQueue, &water_status, portMAX_DELAY))
    {
        if (strcmp(water_status, "WATER DETECTED") == 0)
        {
            ESP_LOGI("WATER_QUEUE", "🚨 WATER NEEDED! Status: %s \n", water_status);
            water_alert = true;
            xEventGroupSetBits(event_group_handle, BIT_WATER_ALERT);
        }
        else if (strcmp(water_status, "WATER OK") == 0 && water_alert)
        {
            ESP_LOGI("WATER_QUEUE", "✅ Water normal. Status: %s \n", water_status);
            water_alert = false;
            xEventGroupSetBits(event_group_handle, BIT_WATER_NORMAL);
        }
    }
}
void gaz_queue_handler(void *pvParameters)
{
    uint16_t gaz_value;
    bool gaz_alert = false;
    while (1)
    {
        if (xQueueReceive(GasQueue, &gaz_value, portMAX_DELAY))
        {
            if (gaz_value > 2200 && !gaz_alert)
            {
                // Handle high gaz alert
                ESP_LOGI("GAZ_QUEUE", "🚨 DANGER GAZ! Valeur: %d \n", gaz_value);
                xEventGroupSetBits(event_group_handle, BIT_GAZ_ALERT);
                gaz_alert = true;
            }
            else if (gaz_value <= 2200 && gaz_alert)
            {
                // Handle normal gaz processing
                ESP_LOGI("GAZ_QUEUE", "✅ Gaz normal. Valeur: %d \n", gaz_value);
                gaz_alert = false;
                xEventGroupSetBits(event_group_handle, BIT_GAZ_NORMAL);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void start_queue_handlers()
{
    xTaskCreate(temp_queue_handler, "TempQueueHandler", 2048, NULL, 5, NULL);
    xTaskCreate(light_queue_handler, "LightQueueHandler", 2048, NULL, 5, NULL);
    xTaskCreate(moisture_queue_handler, "MoistureQueueHandler", 2048, NULL, 5, NULL);
    xTaskCreate(water_queue_handler, "WaterQueueHandler", 2048, NULL, 5, NULL);
    xTaskCreate(&gaz_queue_handler, "GazQueueHandler", 2048, NULL, 5, NULL);
}
