#include "LightSensor.h"
#include <stdio.h>
#include <math.h>
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Queues.h"

#define ADC_CHANNEL ADC1_CHANNEL_7 // GPIO35
#define ADC_MAX 4095
#define MAX_LUX 350.0f

void get_light_intensity(void *pvParameters)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11);

    while (1)
    {
        int adc_value = adc1_get_raw(ADC_CHANNEL);

        // Clamp ADC value
        if (adc_value < 1)
            adc_value = 1;
        if (adc_value > ADC_MAX - 1)
            adc_value = ADC_MAX - 1;

        // Linear mapping ADC → Lux
        float light_intensity = ((float)adc_value / ADC_MAX) * MAX_LUX;

        xQueueSend(LightQueue, &light_intensity, portMAX_DELAY);

        printf("Light: %.2f lux | ADC: %d\n", light_intensity, adc_value);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void show_light_intensity_task(void *pvParameters)
{
    while (1)
    {
        get_light_intensity(NULL);
        vTaskDelay(2000 / portTICK_PERIOD_MS); // Delay for 2 seconds
    }
}