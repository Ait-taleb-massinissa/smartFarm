#include "servo.h"
#include "driver/ledc.h"
#include "esp_log.h"

static const char *TAG = "SERVO";

// Convertir un angle en duty cycle PWM
static void servo_set_angle(ledc_channel_t channel, int angle)
{
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    uint32_t pulse = 500 + ((2500 - 500) * angle / 180); // 0.5ms -> 2.5ms
    uint32_t duty = (pulse * 16384) / 20000; // timer 14-bit, 20ms period

    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel);
}

// Obtenir le canal LEDC selon le GPIO
static ledc_channel_t get_servo_channel(gpio_num_t gpio)
{
    switch (gpio)
    {
        case GPIO_NUM_18: return LEDC_CHANNEL_0;
        case GPIO_NUM_19: return LEDC_CHANNEL_1;
        case GPIO_NUM_21: return LEDC_CHANNEL_2;
        default: return LEDC_CHANNEL_1;
    }
}

// Initialiser le servo sur un GPIO
void servo_init(gpio_num_t gpio)
{
    static bool timer_init = false;

    if (!timer_init)
    {
        ledc_timer_config_t timer_conf = {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .duty_resolution = LEDC_TIMER_14_BIT,
            .timer_num = LEDC_TIMER_1,
            .freq_hz = 50,
            .clk_cfg = LEDC_AUTO_CLK
        };
        ledc_timer_config(&timer_conf);
        timer_init = true;
    }

    ledc_channel_t channel = get_servo_channel(gpio);

    ledc_channel_config_t channel_conf = {
        .gpio_num = gpio,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = channel,
        .timer_sel = LEDC_TIMER_1,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channel_conf);

    // Position initiale: fermée
    servo_set_angle(channel, 0);

    ESP_LOGI(TAG, "Servo initialisé sur GPIO%d", gpio);
}

// Ouvrir le servo (90°)
void servo_ouvrir(gpio_num_t gpio)
{
    ledc_channel_t channel = get_servo_channel(gpio);
    ESP_LOGI(TAG, "Ouverture servo sur GPIO%d", gpio);
    servo_set_angle(channel, 90);
}

// Fermer le servo (0°)
void servo_fermer(gpio_num_t gpio)
{
    ledc_channel_t channel = get_servo_channel(gpio);
    ESP_LOGI(TAG, "Fermeture servo sur GPIO%d", gpio);
    servo_set_angle(channel, 0);
}

// Définir un angle personnalisé
void servo_set_position(gpio_num_t gpio, int angle)
{
    ledc_channel_t channel = get_servo_channel(gpio);
    ESP_LOGI(TAG, "Servo GPIO%d -> %d degrés", gpio, angle);
    servo_set_angle(channel, angle);
}


