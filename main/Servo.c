/*
#include "servo.h"
#include "gas_sensor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "Relay.h"

static const char *TAG = "SERVO_FENETRE";

// Etat de la fenetre
static bool fenetre_ouverte = false;

// Configuration PWM servo
static void servo_set_angle(int angle)
{
    uint32_t pulse = 500 + ((2500 - 500) * angle / 180);
    uint32_t duty = (pulse * 16384) / 20000;

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
}

void ouvrir_fenetre(void)
{
    if (fenetre_ouverte)
        return;

    ESP_LOGW(TAG, "DANGER GAZ - Ouverture fenetre...");

    for (int angle = SERVO_ANGLE_FERME; angle <= SERVO_ANGLE_OUVERT; angle += 5)
    {
        servo_set_angle(angle);
        vTaskDelay(pdMS_TO_TICKS(30));
    }

    fenetre_ouverte = true;
    ESP_LOGI(TAG, "Fenetre OUVERTE");
}

void fermer_fenetre(void)
{
    if (!fenetre_ouverte)
        return;

    ESP_LOGI(TAG, "Air propre - Fermeture fenetre...");

    for (int angle = SERVO_ANGLE_OUVERT; angle >= SERVO_ANGLE_FERME; angle -= 5)
    {
        servo_set_angle(angle);
        vTaskDelay(pdMS_TO_TICKS(30));
    }

    fenetre_ouverte = false;
    ESP_LOGI(TAG, "Fenetre FERMEE");
}

// Tache de controle de la fenetre
void servo_fenetre_task(void *pvParameters)
{
    // Attendre que le capteur de gaz soit pret (prechauffage)
    // vTaskDelay(pdMS_TO_TICKS(32000));

    ESP_LOGI(TAG, "Controle fenetre actif");

    while (1)
    {
        // Utilise directement g_gas_value de ton code gaz
        if (g_gas_value > GAS_THRESHOLD_DANGER)
        {
            ouvrir_fenetre();
            relay_on(GPIO_NUM_23);
        }
        else
        {
            fermer_fenetre();
            relay_off(GPIO_NUM_23);
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

esp_err_t servo_fenetre_init(void)
{
    ESP_LOGI(TAG, "Init servo fenetre (GPIO%d)", SERVO_GPIO);

    // Config timer PWM
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_14_BIT,
        .timer_num = LEDC_TIMER_1,
        .freq_hz = 50,
        .clk_cfg = LEDC_AUTO_CLK};
    ledc_timer_config(&timer_conf);

    // Config channel PWM
    ledc_channel_config_t channel_conf = {
        .gpio_num = SERVO_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .timer_sel = LEDC_TIMER_1,
        .duty = 0,
        .hpoint = 0};
    ledc_channel_config(&channel_conf);

    // Position initiale: fermee
    servo_set_angle(SERVO_ANGLE_FERME);

    return ESP_OK;
}

void servo_fenetre_start(void)
{
    xTaskCreate(servo_fenetre_task, "servo_fenetre", 2048, NULL, 4, NULL);
}
    */
   #include "servo_control.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_log.h"

static const char *TAG = "SERVO";

// Convertir angle en duty cycle
static void servo_set_angle(ledc_channel_t channel, int angle)
{
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;
    
    uint32_t pulse = 500 + ((2500 - 500) * angle / 180);
    uint32_t duty = (pulse * 16384) / 20000;
    
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel);
}

// Obtenir le canal LEDC selon le GPIO
static ledc_channel_t get_servo_channel(gpio_num_t gpio)
{
    if (gpio == GPIO_NUM_18) {
        return LEDC_CHANNEL_0;
    } else if (gpio == GPIO_NUM_19) {
        return LEDC_CHANNEL_1;
    } else if (gpio == GPIO_NUM_21) {
        return LEDC_CHANNEL_2;
    }
    return LEDC_CHANNEL_1;
}

// Fonction pour OUVRIR une fenêtre/porte (angle 90°)
void servo_ouvrir(gpio_num_t gpio)
{
    ledc_channel_t channel = get_servo_channel(gpio);
    ESP_LOGI(TAG, "Ouverture servo sur GPIO%d", gpio);
    servo_set_angle(channel, 90);  // 90° = position ouverte
}

// Fonction pour FERMER une fenêtre/porte (angle 0°)
void servo_fermer(gpio_num_t gpio)
{
    ledc_channel_t channel = get_servo_channel(gpio);
    ESP_LOGI(TAG, "Fermeture servo sur GPIO%d", gpio);
    servo_set_angle(channel, 0);  // 0° = position fermée
}

// Fonction pour définir un angle personnalisé
void servo_set_position(gpio_num_t gpio, int angle)
{
    ledc_channel_t channel = get_servo_channel(gpio);
    ESP_LOGI(TAG, "Servo GPIO%d -> %d degres", gpio, angle);
    servo_set_angle(channel, angle);
}

void servo_task(void *pvParameters)
{
    gpio_num_t servo_gpio = (gpio_num_t)(uintptr_t)pvParameters;
    
    ESP_LOGI(TAG, "Tache servo demarree sur GPIO%d", servo_gpio);
    
    // Initialiser le timer PWM (une seule fois)
    static bool timer_init = false;
    if (!timer_init) {
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
    
    ledc_channel_t servo_channel = get_servo_channel(servo_gpio);
    
    // Config channel PWM
    ledc_channel_config_t channel_conf = {
        .gpio_num = servo_gpio,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = servo_channel,
        .timer_sel = LEDC_TIMER_1,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channel_conf);
    
    // Position initiale: fermée
    servo_set_angle(servo_channel, 0);
    
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void servo_start_task(gpio_num_t gpio_num)
{
    xTaskCreate(
        servo_task, "servo_task", 2048, (void *)gpio_num, 3, NULL
    );
}