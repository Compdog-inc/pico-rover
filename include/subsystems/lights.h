#ifndef _LIGHTS_H
#define _LIGHTS_H

#include <stdlib.h>
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <task.h>

enum class Pattern
{
    Off,
    On,
    Pulse,
    Blink,
    Alt1,
    Alt2,
};

class Lights
{
public:
    Lights();
    ~Lights();

    void setRingIndicatorPattern(Pattern left, Pattern right);

    Pattern getLeftRingIndicatorPattern()
    {
        return leftRingIndicatorPattern;
    }
    Pattern getRightRingIndicatorPattern()
    {
        return rightRingIndicatorPattern;
    }

    bool isAnimationRunning()
    {
        return animationRunning;
    }

    uint getLeftRingIndicatorPin()
    {
        return leftRingIndicatorPin;
    }
    uint getRightRingIndicatorPin()
    {
        return rightRingIndicatorPin;
    }

private:
    TaskHandle_t animationTask;
    bool animationRunning;

    uint leftRingIndicatorPin;
    uint rightRingIndicatorPin;

    Pattern leftRingIndicatorPattern;
    Pattern rightRingIndicatorPattern;
};

#endif