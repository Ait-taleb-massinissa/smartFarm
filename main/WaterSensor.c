#include "WaterSensor.h"
#include <stdio.h>
#include <math.h>
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Queues.h"
#include "esp_log.h"

#define WATER_ADC ADC1_CHANNEL_4 // GPIO32

void water_monitor_task(void *pvParameters)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(WATER_ADC, ADC_ATTEN_DB_11);
    while (1)
    {
        int adc_value = adc1_get_raw(WATER_ADC);
        xQueueSend(WaterQueue, &adc_value, portMAX_DELAY);
        ESP_LOGI("water","✅ Water level: %d ADC\n", adc_value);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
