#ifndef _PORTS_H
#define _PORTS_H

// Standard headers
#include <stdlib.h>
#include <stdint.h>

// Hardware headers
#include <pico/stdlib.h>

#include "subsystems/moduleconfig.h"

namespace Config
{
    namespace GPIO
    {
        static constexpr uint SETUP_BTN = 7;
        static constexpr uint BATTERY_PING_PIN = 8;
    }

    namespace Network
    {
        static constexpr uint32_t UPDATE_TIME_MS = 1000 / 40; // 40 Hz
        static constexpr int64_t UPDATE_TIME_US = 1000 * UPDATE_TIME_MS;
    }

    namespace Drivetrain
    {
        static constexpr Units ROBOT_WHEEL_DISTANCE = Units<float>::inches(14.5);
        static constexpr Units WHEEL_DIAMETER = Units<float>::inches(4.75);

        static constexpr ModuleConfig LEFT_CONSTANTS = ModuleConfig(18U, 17U, 13U, 14U, 19U, 20U, WHEEL_DIAMETER);
        static constexpr ModuleConfig RIGHT_CONSTANTS = ModuleConfig(12U, 11U, 21U, 22U, 16U, 15U, WHEEL_DIAMETER);

        static constexpr Units ROBOT_MAX_SPEED = Units<float>::meters(1);
    }
}

#endif