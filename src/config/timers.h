#ifndef _TIMERS_H
#define _TIMERS_H

// Standard headers
#include <stdlib.h>
#include <string>
#include <cstdarg>
#include <cstring>
#include <cinttypes>

// Hardware headers
#include <pico/stdlib.h>
#include <pico/time.h>

volatile unsigned portLONG ulHighFrequencyTimerTicks = 0UL;

namespace Config
{
    namespace
    {
        bool task_timer_callback(repeating_timer_t *rt)
        {
#pragma GCC diagnostic ignored "-Wvolatile"
            ulHighFrequencyTimerTicks++; // tick timer
#pragma GCC diagnostic warning "-Wvolatile"
            return true; // keep repeating
        }

        void init_task_timer(repeating_timer_t *timer)
        {
            if (!add_repeating_timer_us(-1000000 / 20, task_timer_callback, NULL, timer))
            {
                printf("[INIT] Failed to create task tick timer\n");
                return;
            }

            printf("[INIT] Created task tick timer [%i] @%" PRId64 "\n", timer->alarm_id, timer->delay_us);
        }
    }

    static repeating_timer_t timer;

    void init_timers()
    {
        init_task_timer(&timer);
    }

    void deinit_timers()
    {
        cancel_repeating_timer(&timer);
    }
}

#endif