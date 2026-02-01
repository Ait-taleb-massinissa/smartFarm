#ifndef STEPMOTOR_H
#define STEPMOTOR_H
void step_motor_init();
void step_motor_forward(int steps);
void step_motor_backward(int steps);
void step_motor_cleanup();
#endif