#ifndef LAMP_H
#define LAMP_H

#include "driver/gpio.h"

// Configuration du GPIO pour la lampe
#define LAMP_GPIO_PIN GPIO_NUM_16  // Modifier selon votre branchement

// Initialise le GPIO de la lampe
void lamp_init(void);

// Allume la lampe (3.3V)
void lamp_turn_on(void);

// Éteint la lampe (0V)
void lamp_turn_off(void);

// Inverse l'état de la lampe
void lamp_toggle(void);

// Retourne l'état actuel de la lampe (1 = allumée, 0 = éteinte)
int lamp_get_state(void);

#endif // LAMP_H
