#include "WaterSensor.h"
#include <stdio.h>
#include <math.h>
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Queues.h"

#define WATER_ADC ADC1_CHANNEL_4 // GPIO32

void water_sensor_init(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(WATER_ADC, ADC_ATTEN_DB_11);
}

int is_water_present(void)
{
    int adc_value = adc1_get_raw(WATER_ADC);

    // seuil à ajuster après tests
    if (adc_value > 1200)
        return 1; // eau présente
    else
        return 0; // réservoir vide
}

void water_monitor_task(void *pvParameters)
{
    int last_state = -1;

    while (1)
    {
        int water_present = is_water_present();

        if (water_present != last_state)
        {
            if (!water_present)
            {
                printf("Réservoir VIDE → ouverture électrovanne\n");
            }
            else
            {
                printf("Eau détectée → fermeture électrovanne\n");
            }

            last_state = water_present;
        } 

        printf("ADC: %d | Eau: %s\n",
               adc1_get_raw(WATER_ADC),
               water_present ? "OUI" : "NON");

        xQueueSend(WaterQueue, &water_present, portMAX_DELAY);

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
