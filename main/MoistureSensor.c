#include "MoistureSensor.h"
#include <stdio.h>
#include <math.h>
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Queues.h"

#define MOISTURE_ADC ADC1_CHANNEL_5 // GPIO33
#define DRY_THRESHOLD 1000
#define WET_THRESHOLD 1500

void get_moisture_level(void *pvParameters)
{
    while(1){
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(MOISTURE_ADC, ADC_ATTEN_DB_11); // GPIO33

    // Read the ADC value
    int adc_value = adc1_get_raw(MOISTURE_ADC);

    const char *soil_state;
    if (adc_value < DRY_THRESHOLD )
    {
        soil_state = "Dry";
    }
    else if (adc_value >= WET_THRESHOLD)
    {
        soil_state = "Wet";
    }
    else
    {
        soil_state = "Moderate";
    }

    xQueueSend(MoistureQueue, &soil_state, portMAX_DELAY);
    printf("Soil ADC: %d | Soil: %s\n", adc_value, soil_state);
    vTaskDelay(2000 / portTICK_PERIOD_MS); // Delay for 2 seconds
    }
}

void show_moisture_level_task(void *pvParameters)
{
    while (1)
    {
        get_moisture_level(NULL);
        vTaskDelay(2000 / portTICK_PERIOD_MS); // Delay for 2 seconds
    }
}