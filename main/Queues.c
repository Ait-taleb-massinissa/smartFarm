#include "Queues.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

QueueHandle_t TempQueue;
QueueHandle_t LightQueue;
QueueHandle_t MoistureQueue;
QueueHandle_t WaterQueue;
QueueHandle_t GasQueue;

void create_queues(void) {
    TempQueue = xQueueCreate(10, sizeof(float));
    LightQueue = xQueueCreate(10, sizeof(float));
    MoistureQueue = xQueueCreate(10, sizeof(float));
    WaterQueue = xQueueCreate(10, sizeof(float));
    GasQueue = xQueueCreate(10, sizeof(uint16_t));
}

void temp_queue_handler(){
    float temp_value;
    if (xQueueReceive(TempQueue, &temp_value, portMAX_DELAY)) {
        if (temp_value > 30.0) {
            // Handle high temperature alert
            
        } else {
            // Normal temperature processing
        }
    }
}

void light_queue_handler(){
    float light_value;
    if (xQueueReceive(LightQueue, &light_value, portMAX_DELAY)) {
        if (light_value < 50.0) {
            // Handle low light alert
        } else {
            // Normal light processing
        }
    }
}

void moisture_queue_handler(){
    char* moisture_status;
    if (xQueueReceive(MoistureQueue, &moisture_status, portMAX_DELAY)) {
        if (strcmp(moisture_status, "Dry") == 0) {
            // Handle low moisture alert
        } else {
            // Normal moisture processing
        }
    }
}

void water_queue_handler(){
    char* water_status;
    if (xQueueReceive(WaterQueue, &water_status, portMAX_DELAY)) {
        if (strcmp(water_status, "WATER DETECTED") == 0) {
            // Handle water needed alert
        } else {
            // Normal water processing
        }
    }
}

void gaz_queue_handler(){
    float gaz_value;
    if (xQueueReceive(GazQueue, &gaz_value, portMAX_DELAY)) {
        if (gaz_value > 2200) {
            // Handle high gaz alert
        } else {
            // Normal gaz processing
        }
    }
}

