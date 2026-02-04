#ifndef SERVO_H
#define SERVO_H

#include "esp_err.h"
#include <stdbool.h>

// Configuration du servo
#define SERVO_GPIO GPIO_NUM_18
#define SERVO_ANGLE_FERME 0
#define SERVO_ANGLE_OUVERT 90

// Fonctions
esp_err_t servo_fenetre_init(void);
void servo_fenetre_start(void);
void ouvrir_fenetre(void);
void fermer_fenetre(void);
void servo_fenetre_task(void *pvParameters);

#endif