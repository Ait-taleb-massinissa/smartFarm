#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include "driver/gpio.h"

void servo_task(void *pvParameters);
void servo_start_task(gpio_num_t gpio_num);
void servo_set_angle(ledc_channel_t channel, int angle);

#endif