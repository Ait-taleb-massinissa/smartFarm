#ifndef QUEUES_H
#define QUEUES_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

extern QueueHandle_t TempQueue;
extern QueueHandle_t LightQueue;
extern QueueHandle_t MoistureQueue;
extern QueueHandle_t WaterQueue;

void create_queues(void);

#endif
