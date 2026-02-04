#include "gas_sensor.h"
#include "esp_adc_cal.h"
#include "esp_log.h"

static const char *TAG = "GAS_SENSOR";

// Variables globales simples
uint16_t g_gas_value = 0;
bool g_gas_alert = false;

// Tâche simple du capteur de gaz
static void gas_task(void *pvParameters)
{
    // PRÉCHAUFFAGE: Les capteurs MQ ont besoin de 20-30 secondes
    // printf("⏳ Préchauffage capteur MQ (30s)...\n");
    // vTaskDelay(pdMS_TO_TICKS(30000)); // Attendre 30 secondes
    printf("✅ Capteur MQ prêt!\n");

    while (1)
    {

        g_gas_value = adc1_get_raw(GAS_SENSOR_ADC_CHANNEL);

        // LOGIQUE INVERSÉE: MQ donne valeur BASSE quand gaz détecté
        // Air propre = 3000-4000, Gaz présent = 0-2000
        if (g_gas_value > GAS_THRESHOLD_DANGER)
        {
            g_gas_alert = true;
            printf("🚨 DANGER GAZ! Valeur: %d (bas = gaz!)\n", g_gas_value);
        }
        else
        {
            g_gas_alert = false;
            printf("✅ Air propre: %d ADC\n", g_gas_value);
        }

        vTaskDelay(pdMS_TO_TICKS(GAS_READ_INTERVAL_MS));
    }
}

// Init simple
esp_err_t gas_sensor_init(void)
{
    ESP_LOGI(TAG, "Init capteur gaz (GPIO34)");

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(GAS_SENSOR_ADC_CHANNEL, ADC_ATTEN_DB_11);

    return ESP_OK;
}

// Démarrer
void gas_sensor_start(void)
{
    xTaskCreate(gas_task, "gas", 3072, NULL, 5, NULL);
}