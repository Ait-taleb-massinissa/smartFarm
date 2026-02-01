#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_log.h"

#define TAG "SERVO"

// -------- CONFIG SERVO --------
#define SERVO_GPIO       18
#define SERVO_FREQ       50      // 50 Hz
#define SERVO_TIMER      LEDC_TIMER_0
#define SERVO_CHANNEL    LEDC_CHANNEL_0

// ⚠️ À AJUSTER SELON TON SERVO
#define SERVO_OPEN_DUTY   6000   // fenêtre ouverte
#define SERVO_CLOSE_DUTY  3000   // fenêtre fermée
// ------------------------------

void servo_init(void)
{
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = SERVO_TIMER,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz = SERVO_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t channel = {
        .gpio_num = SERVO_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = SERVO_CHANNEL,
        .timer_sel = SERVO_TIMER,
        .duty = SERVO_CLOSE_DUTY,
        .hpoint = 0
    };
    ledc_channel_config(&channel);

    ESP_LOGI(TAG, "Servo initialisé (fenêtre fermée)");
}

// Ouvrir la fenêtre
void servo_open(void)
{
    ESP_LOGI(TAG, "Fenetre OUVERTE");
    ledc_set_duty(LEDC_LOW_SPEED_MODE, SERVO_CHANNEL, SERVO_OPEN_DUTY);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, SERVO_CHANNEL);
}

// Fermer la fenêtre
void servo_close(void)
{
    ESP_LOGI(TAG, "Fenetre FERMEE");
    ledc_set_duty(LEDC_LOW_SPEED_MODE, SERVO_CHANNEL, SERVO_CLOSE_DUTY);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, SERVO_CHANNEL);
}

// -------- TÂCHE SERVO --------
void servo_task(void *pvParameters)
{
    servo_init();
    while (1)
    {
        servo_open();
        vTaskDelay(pdMS_TO_TICKS(3000)); // temps pour ouvrir

        servo_close();
        vTaskDelay(pdMS_TO_TICKS(3000)); // temps pour fermer
    }
}

