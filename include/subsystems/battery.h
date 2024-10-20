#ifndef _BATTERY_H
#define _BATTERY_H

#include <stdlib.h>
#include <pico/stdlib.h>
#include <pico/time.h>

class Battery
{
public:
    Battery();
    ~Battery();

    void startPingTimer();
    void stopPingTimer();

    void ping();

    uint getPingPin()
    {
        return pingPin;
    }

private:
    repeating_timer_t pingTimer;
    bool pingTimerRunning;

    uint pingPin;
};

#endif