#ifndef GAS_SENSOR_H
#define GAS_SENSOR_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_err.h"

// Configuration simple
#define GAS_SENSOR_ADC_CHANNEL ADC1_CHANNEL_6 // GPIO34
#define GAS_READ_INTERVAL_MS 1000
#define GAS_THRESHOLD_DANGER 2200 // Si > 2200 → ventilateur + fenêtres

// Données globales (simples)
extern uint16_t g_gas_value;
extern bool g_gas_alert;

// Fonctions simples
esp_err_t gas_sensor_init(void);
void gas_sensor_start(void);

#endif // GAS_SENSOR_H