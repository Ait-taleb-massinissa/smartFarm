#ifndef QUEUES_H
#define QUEUES_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

extern QueueHandle_t TempQueue;
extern QueueHandle_t LightQueue;
extern QueueHandle_t MoistureQueue;
extern QueueHandle_t WaterQueue;
extern QueueHandle_t GasQueue;

void create_queues(void);
void start_queue_handlers(void);
void showdata(void);
#endif
