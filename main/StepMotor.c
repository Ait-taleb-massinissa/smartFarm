#include "StepMotor.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp32/rom/ets_sys.h"

#define IN1 14
#define IN2 27
#define IN3 26
#define IN4 25

int pins[4] = {IN4, IN2, IN3, IN1};
// int pins[4] = {IN4, IN2, IN3, IN1};

int step_seq[8][4] = {

    {1, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 1, 1},
    {0, 0, 1, 0},
    {0, 1, 1, 0},
    {0, 1, 0, 0},
    {1, 1, 0, 0},
    {1, 0, 0, 0}};

void step_motor_init()
{
    for (int i = 0; i < 4; i++)
    {
        gpio_set_direction(pins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(pins[i], 0);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void step_motor_forward(int steps)
{
    for (int i = 0; i < steps; i++)
    {
        for (int s = 0; s < 8; s++)
        {
            for (int p = 0; p < 4; p++)
            {
                gpio_set_level(pins[p], step_seq[s][p]);
            }
            ets_delay_us(1500);
        }
    }
}
void step_motor_backward(int steps)
{
    int pins2[4] = {IN1, IN3, IN2, IN4};


    int step_seq2[8][4] = {
        {1, 0, 0, 0},
        {1, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 1},
        {0, 0, 0, 1},
        {1, 0, 0, 1},
    };
    for (int i = 0; i < steps; i++)
    {
        for (int s = 0; s < 8; s++)
        {
            for (int p = 0; p < 4; p++)
            {
                gpio_set_level(pins2[p], step_seq2[s][p]);
            }
            ets_delay_us(1500);
        }
    }
}

void step_motor_cleanup()
{
    for (int i = 0; i < 4; i++)
    {
        gpio_set_level(pins[i], 0);
    }
    ets_delay_us(1500);
}