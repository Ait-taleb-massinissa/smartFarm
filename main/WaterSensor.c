#include "WaterSensor.h"
#include <stdio.h>
#include <math.h>
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Queues.h"

#define WATER_ADC ADC1_CHANNEL_4 // GPIO32

void get_water_level(void *pvParameters)
{
    while (1)
    {
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(WATER_ADC, ADC_ATTEN_DB_11); // GPIO32

        // Read the ADC value
        int adc_value = adc1_get_raw(WATER_ADC);

        int water_detected = (adc_value > 1200);
        char *status = water_detected ? "WATER DETECTED" : "NO WATER";
        xQueueSend(WaterQueue, &status, portMAX_DELAY);
        printf("Water ADC: %d | Water detected: %s\n",
               adc_value,
               water_detected ? "YES" : "NO");
        vTaskDelay(2000 / portTICK_PERIOD_MS); // Delay for 2 seconds
    }
}

void show_water_level_task(void *pvParameters)
{
    while (1)
    {
        get_water_level(NULL);
        vTaskDelay(2000 / portTICK_PERIOD_MS); // Delay for 2 seconds
    }
}