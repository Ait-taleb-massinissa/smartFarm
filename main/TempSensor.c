#include "TempSensor.h"
#include <stdio.h>
#include <math.h>
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Queues.h"

#define ADC_CHANNEL ADC1_CHANNEL_6 // GPIO34
#define R0 10000.0                 // 10k ohm
#define BETA 3975.0
#define VREF 3.3
#define ADC_MAX 4095.0

void get_temperature(void *pvParameters)
{
    while (1)
    {
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11);

        // Read the ADC value
        int adc_value = adc1_get_raw(ADC_CHANNEL);

        // Convert the ADC value to voltage
        float voltage = (adc_value / ADC_MAX) * VREF;

        // Convert the voltage to resistance
        float resistance = R0 * ((VREF / voltage) - 1);

        // Convert the resistance to temperature
        float temperature = 1 / (log(resistance / R0) / BETA + 1 / 298.15) - 273.15;

        xQueueSend(TempQueue, &temperature, portMAX_DELAY);

        printf("Temperature: %.2f°C\n", temperature);

        vTaskDelay(2000 / portTICK_PERIOD_MS); // Delay for 2 seconds
    }
}

void show_temperature_task(void *pvParameters)
{
    while (1)
    {
        get_temperature(NULL);
        vTaskDelay(2000 / portTICK_PERIOD_MS); // Delay for 2 seconds
    }
}
