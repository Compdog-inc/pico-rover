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

        // static constexpr radio_characteristics_packet robot_characteristics = {
        //     vec3(Units::fromInches(13), Units::fromInches(12.5), Units::fromInches(21)), // frameSize

        //     Units::fromInches(4.75), // wheelDiameter

        //     vec3(Units::fromInches(-7), Units::fromInches(2.375), Units::fromInches(6)),        // wheel0Location
        //     vec3(Units::fromInches(7), Units::fromInches(2.375), Units::fromInches(6)),         // wheel1Location
        //     vec3(Units::fromInches(7.1875), Units::fromInches(2.375), Units::fromInches(0.25)), // wheel2Location
        //     vec3(Units::fromInches(-7.0625), Units::fromInches(2.375), Units::fromInches(0.5)), // wheel3Location
        //     vec3(Units::fromInches(7.1875), Units::fromInches(2.375), Units::fromInches(-5)),   // wheel4Location
        //     vec3(Units::fromInches(-6.375), Units::fromInches(2.375), Units::fromInches(-5)),   // wheel5Location

        //     vec4(Units::fromInches(-5.75), Units::fromInches(3), Units::fromInches(9.75), Units::fromDegrees(315)),  // whisker0
        //     vec4(Units::fromInches(0), Units::fromInches(3), Units::fromInches(10.4375), Units::fromDegrees(0)),     // whisker1
        //     vec4(Units::fromInches(5.75), Units::fromInches(3), Units::fromInches(9.75), Units::fromDegrees(45)),    // whisker2
        //     vec4(Units::fromInches(-5.75), Units::fromInches(3), Units::fromInches(-9.75), Units::fromDegrees(135)), // whisker3
        //     vec4(Units::fromInches(0), Units::fromInches(3), Units::fromInches(-10.4375), Units::fromDegrees(180)),  // whisker4
        //     vec4(Units::fromInches(-5.75), Units::fromInches(3), Units::fromInches(-9.75), Units::fromDegrees(225))  // whisker5
        // };
    }

    namespace Lights
    {
        static constexpr uint RING_INDICATOR_LEFT_PIN = 9;
        static constexpr uint RING_INDICATOR_RIGHT_PIN = 10;

        static constexpr uint PULSE_LENGTH = 2000000;
        static constexpr uint PULSE_HALF_LENGTH = 1000000;
        static constexpr uint PULSE_DIVISOR = 16;

        static constexpr uint BLINK_LENGTH = 1000000;
        static constexpr uint BLINK_ON_LENGTH = 250000;
    }
}

#endif