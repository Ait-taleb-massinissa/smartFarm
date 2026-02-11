/*
#ifndef RELAY_H
#define RELAY_H
void relay_init();
void relay_on();
void relay_off();
#endif
*/
#ifndef RELAY_H
#define RELAY_H

#include <stdint.h>
#include "driver/gpio.h"

void relay_init(gpio_num_t gpio_num);
void relay_on(gpio_num_t gpio_num);
void relay_off(gpio_num_t gpio_num);

#endif