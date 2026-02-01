#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "TempSensor.h"
#include "LightSensor.h"
#include "WaterSensor.h"
#include "MoistureSensor.h"
#include "Relay.h"
#include "StepMotor.h"
#include "nvs_flash.h"
#include "Server.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "wifi.h"
#include "Queues.h"
#include "Servo.h"
#include "gas_sensor.h"
void app_main(void)
{
    // create_queues();

    // xTaskCreate(&get_temperature, "get temperature", 2048, NULL, 5, NULL);
    // xTaskCreate(&get_light_intensity, "get light intensity", 2048, NULL, 5, NULL);
    // xTaskCreate(&get_moisture_level, "get moisture level", 2048, NULL, 5, NULL);
    // xTaskCreate(&get_water_level, "get water level", 2048, NULL, 5, NULL);
    // ESP_LOGI("MAIN", "Smart Farm System Running...");

    // esp_err_t ret = nvs_flash_init();
    // if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    // {
    //     nvs_flash_erase();
    //     nvs_flash_init();
    // }

    // ESP_LOGI("MAIN", "Starting WiFi...");
    // wifi_init();

    // vTaskDelay(pdMS_TO_TICKS(5000)); // wait for IP
    // start_webserver();

    // ESP_LOGI("MAIN", "Web server started");
    xTaskCreate(&servo_task, "gas", 3072, NULL, 5, NULL);
}
