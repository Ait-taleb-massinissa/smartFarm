#ifndef SERVO_H
#define SERVO_H

void servo_init(void);
void servo_open(void);
void servo_close(void);
void servo_task(void *pvParameters);

#endif // SERVO_H