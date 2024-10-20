// Standard headers
#include <stdlib.h>
#include <math.h>

// Kernel headers
#include <FreeRTOS.h>
#include <task.h>

// Hardware headers
#include <pico/stdlib.h>
#include <hardware/pwm.h>

#include "subsystems/lights.h"

#define LIGHT_LEFT 9
#define LIGHT_RIGHT 10

static TaskHandle_t animationTask;
static bool runAnimationTask = true;

static LightMode leftMode = LightMode::Off;
static LightMode rightMode = LightMode::Off;

#define PULSE_LENGTH 2000000
#define PULSE_HALF_LENGTH 1000000
#define PULSE_DIVISOR 16

#define BLINK_LENGTH 1000000
#define BLINK_ON_LENGTH 250000

void light_subsystem_set(uint gpio, uint16_t level)
{
    float flt = (float)level / (float)UINT16_MAX;
    flt *= flt;
    uint16_t out = (uint16_t)std::floor(flt * UINT16_MAX);
    pwm_set_gpio_level(gpio, out);
}

void light_subsystem_run(uint gpio, LightMode mode)
{
    switch (mode)
    {
    case LightMode::Off:
    {
        light_subsystem_set(gpio, 0);
    }
    break;
    case LightMode::On:
    {
        light_subsystem_set(gpio, 0xFFFF);
    }
    break;
    case LightMode::Pulse:
    {
        uint32_t cycleTime = time_us_32() % PULSE_LENGTH;
        if (cycleTime < PULSE_HALF_LENGTH)
            light_subsystem_set(gpio, cycleTime / PULSE_DIVISOR);
        else
            light_subsystem_set(gpio, (PULSE_LENGTH - cycleTime) / PULSE_DIVISOR);
    }
    break;
    case LightMode::Blink:
    {
        uint32_t cycleTime = time_us_32() % BLINK_LENGTH;
        if (cycleTime < BLINK_ON_LENGTH)
            light_subsystem_set(gpio, 0xFFFF);
        else
            light_subsystem_set(gpio, 0);
    }
    break;
    case LightMode::Alt1:
    {
        uint32_t cycleTime = time_us_32() % PULSE_LENGTH;
        if (cycleTime < PULSE_HALF_LENGTH)
            light_subsystem_set(gpio, cycleTime / PULSE_DIVISOR);
        else
            light_subsystem_set(gpio, (PULSE_LENGTH - cycleTime) / PULSE_DIVISOR);
    }
    break;
    case LightMode::Alt2:
    {
        uint32_t cycleTime = time_us_32() % PULSE_LENGTH;
        if (cycleTime < PULSE_HALF_LENGTH)
            light_subsystem_set(gpio, 62500 - cycleTime / PULSE_DIVISOR);
        else
            light_subsystem_set(gpio, 62500 - (PULSE_LENGTH - cycleTime) / PULSE_DIVISOR);
    }
    break;
    }
}

void animation_task(__unused void *params)
{
    while (runAnimationTask)
    {
        light_subsystem_run(LIGHT_LEFT, leftMode);
        light_subsystem_run(LIGHT_RIGHT, rightMode);
        vTaskDelay(pdMS_TO_TICKS(2));
    }

    vTaskDelete(NULL);
}

void light_subsystem_init()
{
    gpio_init(LIGHT_LEFT);
    gpio_init(LIGHT_RIGHT);

    gpio_set_dir(LIGHT_LEFT, true);
    gpio_set_dir(LIGHT_RIGHT, true);

    gpio_set_function(LIGHT_LEFT, GPIO_FUNC_PWM);
    gpio_set_function(LIGHT_RIGHT, GPIO_FUNC_PWM);

    uint slice_num_left = pwm_gpio_to_slice_num(LIGHT_LEFT);
    uint slice_num_right = pwm_gpio_to_slice_num(LIGHT_RIGHT);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.f);
    pwm_init(slice_num_left, &config, true);
    pwm_init(slice_num_right, &config, true);

    xTaskCreate(animation_task, "LightAnimationThread", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 2UL), &animationTask);
}

void light_subsystem_deinit()
{
    runAnimationTask = false;
}

void light_subsystem_set(Light light, LightMode mode)
{
    switch (light)
    {
    case Light::Left:
        leftMode = mode;
        break;
    case Light::Right:
        rightMode = mode;
        break;
    }
}