#ifndef SERVO_H
#define SERVO_H

#include "driver/gpio.h"

// Initialisation du servo sur un GPIO
void servo_init(gpio_num_t gpio);

// Ouvrir le servo (angle 90°)
void servo_ouvrir(gpio_num_t gpio);

// Fermer le servo (angle 0°)
void servo_fermer(gpio_num_t gpio);

// Définir un angle personnalisé
void servo_set_position(gpio_num_t gpio, int angle);

#endif // SERVO_H
