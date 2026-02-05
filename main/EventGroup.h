#ifndef EVENT_GROUP_H
#define EVENT_GROUP_H
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

extern EventGroupHandle_t event_group_handle;

void start_event_group_handler(void);
#endif // EVENT_GROUP_H