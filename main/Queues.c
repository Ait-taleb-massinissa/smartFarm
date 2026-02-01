#include "Queues.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

QueueHandle_t TempQueue;
QueueHandle_t LightQueue;
QueueHandle_t MoistureQueue;
QueueHandle_t WaterQueue;

void create_queues(void) {
    TempQueue = xQueueCreate(10, sizeof(float));
    LightQueue = xQueueCreate(10, sizeof(float));
    MoistureQueue = xQueueCreate(10, sizeof(float));
    WaterQueue = xQueueCreate(10, sizeof(float));
}
