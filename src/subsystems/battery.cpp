#include "subsystems/battery.h"

#include "config/options.h"

Battery::Battery() : pingTimerRunning(false), pingPin(Config::Battery::PING_PIN)
{
    gpio_init(pingPin);
    gpio_set_dir(pingPin, true);
    gpio_put(pingPin, true);
}

Battery::~Battery()
{
    stopPingTimer();
    gpio_deinit(pingPin);
}

void Battery::startPingTimer()
{
    if (!pingTimerRunning)
    {
        pingTimerRunning = true;
        add_repeating_timer_ms(Config::Battery::PING_TIMER_DELAY_MS, [](repeating_timer_t *rt) -> bool
                               {
                                 Battery *battery = (Battery *)rt->user_data;
                                 battery->ping();
                                 return true; }, this, &pingTimer);
    }
}

void Battery::stopPingTimer()
{
    if (pingTimerRunning)
    {
        pingTimerRunning = false;
        cancel_repeating_timer(&pingTimer);
    }
}

int64_t ping_end(alarm_id_t id, void *pv_battery)
{
    Battery *battery = (Battery *)pv_battery;
    gpio_put(battery->getPingPin(), true);
    return 0;
}

void Battery::ping()
{
    gpio_put(pingPin, false);
    add_alarm_in_ms(100, ping_end, this, true);
}
