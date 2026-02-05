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

void relay_task(void *pvParameters);

#endif