// Standard headers
#include <stdlib.h>
#include <math.h>

// Kernel headers
#include <FreeRTOS.h>
#include <task.h>

// Hardware headers
#include <pico/stdlib.h>
#include <hardware/pwm.h>

// Libraries
#include <board/led.h>

// Config headers
#include "config/options.h"

#include "subsystems/lights.h"

enum SPECIAL_GPIO
{
    STATUS_LED_GPIO = 255
};

void pwm_set_squared(uint gpio, uint16_t level)
{
    uint16_t squared = ((level >> 8) + 1) * ((level >> 8) + 1) - 1;
    if (gpio != STATUS_LED_GPIO)
    {
        pwm_set_gpio_level(gpio, squared);
    }
    else
    {
        BoardLed::set(level);
    }
}

void apply_pattern(uint gpio, Pattern pattern)
{
    switch (pattern)
    {
    case Pattern::Off:
    {
        pwm_set_squared(gpio, 0);
    }
    break;
    case Pattern::On:
    {
        pwm_set_squared(gpio, 0xFFFF);
    }
    break;
    case Pattern::Pulse:
    {
        uint32_t cycleTime = time_us_32() % Config::Lights::PULSE_LENGTH;
        if (cycleTime < Config::Lights::PULSE_HALF_LENGTH)
            pwm_set_squared(gpio, cycleTime / Config::Lights::PULSE_DIVISOR);
        else
            pwm_set_squared(gpio, (Config::Lights::PULSE_LENGTH - cycleTime) / Config::Lights::PULSE_DIVISOR);
    }
    break;
    case Pattern::Blink:
    {
        uint32_t cycleTime = time_us_32() % Config::Lights::BLINK_LENGTH;
        if (cycleTime < Config::Lights::BLINK_ON_LENGTH)
            pwm_set_squared(gpio, 0xFFFF);
        else
            pwm_set_squared(gpio, 0);
    }
    break;
    case Pattern::Alt1:
    {
        uint32_t cycleTime = time_us_32() % Config::Lights::PULSE_LENGTH;
        if (cycleTime < Config::Lights::PULSE_HALF_LENGTH)
            pwm_set_squared(gpio, cycleTime / Config::Lights::PULSE_DIVISOR);
        else
            pwm_set_squared(gpio, (Config::Lights::PULSE_LENGTH - cycleTime) / Config::Lights::PULSE_DIVISOR);
    }
    break;
    case Pattern::Alt2:
    {
        uint32_t cycleTime = time_us_32() % Config::Lights::PULSE_LENGTH;
        if (cycleTime < Config::Lights::PULSE_HALF_LENGTH)
            pwm_set_squared(gpio, 62500 - cycleTime / Config::Lights::PULSE_DIVISOR);
        else
            pwm_set_squared(gpio, 62500 - (Config::Lights::PULSE_LENGTH - cycleTime) / Config::Lights::PULSE_DIVISOR);
    }
    break;
    }
}

void animation_task(void *pv_lights)
{
    Lights *lights = (Lights *)pv_lights;

    while (lights->isAnimationRunning())
    {
        apply_pattern(lights->getLeftRingIndicatorPin(), lights->getLeftRingIndicatorPattern());
        apply_pattern(lights->getRightRingIndicatorPin(), lights->getRightRingIndicatorPattern());
        apply_pattern(STATUS_LED_GPIO, lights->getStatusLedPattern());
        vTaskDelay(pdMS_TO_TICKS(2));
    }

    vTaskDelete(NULL);
}

Lights::Lights() : animationRunning(true), leftRingIndicatorPin(Config::Lights::RING_INDICATOR_LEFT_PIN), rightRingIndicatorPin(Config::Lights::RING_INDICATOR_RIGHT_PIN)
{
    gpio_init(leftRingIndicatorPin);
    gpio_init(rightRingIndicatorPin);

    gpio_set_dir(leftRingIndicatorPin, true);
    gpio_set_dir(rightRingIndicatorPin, true);

    gpio_set_function(leftRingIndicatorPin, GPIO_FUNC_PWM);
    gpio_set_function(rightRingIndicatorPin, GPIO_FUNC_PWM);

    uint slice_num_left = pwm_gpio_to_slice_num(leftRingIndicatorPin);
    uint slice_num_right = pwm_gpio_to_slice_num(rightRingIndicatorPin);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.f);
    pwm_init(slice_num_left, &config, true);
    pwm_init(slice_num_right, &config, true);

    BoardLed::init();

    xTaskCreate(animation_task, "LightAnimationThread", configMINIMAL_STACK_SIZE, this, (tskIDLE_PRIORITY + 2UL), &animationTask);
}

Lights::~Lights()
{
    animationRunning = false;
}

void Lights::setRingIndicatorPattern(Pattern left, Pattern right)
{
    leftRingIndicatorPattern = left;
    rightRingIndicatorPattern = right;
}

void Lights::setStatusLedPattern(Pattern pattern)
{
    statusLedPattern = pattern;
}
