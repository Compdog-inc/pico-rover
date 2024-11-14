#ifndef _PORTS_H
#define _PORTS_H

// Standard headers
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <array>

// Hardware headers
#include <pico/stdlib.h>

#include <math/units.h>
#include <math/vectors.h>
#include "subsystems/moduleconfig.h"

namespace Config
{
    namespace GPIO
    {
        static constexpr uint SETUP_BTN = 7;

        // gpio_init(Config::GPIO::SETUP_BTN);
        // gpio_set_dir(Config::GPIO::SETUP_BTN, GPIO_IN);
        // gpio_pull_up(Config::GPIO::SETUP_BTN);
    }

    namespace Network
    {
        static constexpr uint32_t UPDATE_TIME_MS = 1000 / 40; // 40 Hz
        static constexpr int64_t UPDATE_TIME_US = 1000 * UPDATE_TIME_MS;
    }

    namespace Control
    {
        using namespace std::literals;
        static constexpr int DRIVERSTATION_PORT = 5002;
        static constexpr std::string_view DRIVERSTATION_PROTOCOL = "driverstation.pico.rover"sv;
        static constexpr uint32_t DRIVERSTATION_TIMEOUT_MS = 1000;

        static constexpr int XBOX_UDP_PORT = 5001;
    }

    namespace Drivetrain
    {
        static constexpr Units ROBOT_WHEEL_DISTANCE = Units<float>::inches(14.5);
        static constexpr Units WHEEL_DIAMETER = Units<float>::inches(4.75);

        static constexpr ModuleConfig LEFT_CONSTANTS = ModuleConfig(18U, 17U, 13U, 14U, 19U, 20U, WHEEL_DIAMETER);
        static constexpr ModuleConfig RIGHT_CONSTANTS = ModuleConfig(12U, 11U, 21U, 22U, 16U, 15U, WHEEL_DIAMETER);

        static constexpr Units ROBOT_MAX_SPEED = Units<float>::meters(1);
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

    namespace Battery
    {
        static constexpr uint PING_PIN = 8;
        static constexpr int32_t PING_TIMER_DELAY_MS = -1000;
    }

    struct RobotProperties
    {
        vec3<float> frameDimensions;
        Units<float> wheelDiameter;
        std::array<transform<float>, 6> wheels;
        std::array<transform<float>, 6> whiskers;

        template <class T>
        void pack(T &pack) const
        {
            pack(frameDimensions, wheelDiameter, wheels, whiskers);
        }
    };

    static constexpr RobotProperties ROBOT_PROPERTIES = {
        vec3<float>(Units<float>::inches(13), Units<float>::inches(12.5), Units<float>::inches(21)), // frameDimensions

        Drivetrain::WHEEL_DIAMETER, // wheelDiameter

        // wheels
        {
            transform<float>(
                vec3<float>(
                    Units<float>::inches(-7),
                    Units<float>::inches(2.375),
                    Units<float>::inches(6)),
                vec4<float>::fromEuler(
                    Units<float>::degrees(0),
                    Units<float>::degrees(270),
                    Units<float>::degrees(0))),
            transform<float>(
                vec3<float>(
                    Units<float>::inches(7),
                    Units<float>::inches(2.375),
                    Units<float>::inches(6)),
                vec4<float>::fromEuler(
                    Units<float>::degrees(0),
                    Units<float>::degrees(90),
                    Units<float>::degrees(0))),
            transform<float>(
                vec3<float>(
                    Units<float>::inches(7.1875),
                    Units<float>::inches(2.375),
                    Units<float>::inches(0.25)),
                vec4<float>::fromEuler(
                    Units<float>::degrees(0),
                    Units<float>::degrees(90),
                    Units<float>::degrees(0))),
            transform<float>(
                vec3<float>(
                    Units<float>::inches(-7.0625),
                    Units<float>::inches(2.375),
                    Units<float>::inches(0.5)),
                vec4<float>::fromEuler(
                    Units<float>::degrees(0),
                    Units<float>::degrees(270),
                    Units<float>::degrees(0))),
            transform<float>(
                vec3<float>(
                    Units<float>::inches(7.1875),
                    Units<float>::inches(2.375),
                    Units<float>::inches(-5)),
                vec4<float>::fromEuler(
                    Units<float>::degrees(0),
                    Units<float>::degrees(90),
                    Units<float>::degrees(0))),
            transform<float>(
                vec3<float>(
                    Units<float>::inches(-6.375),
                    Units<float>::inches(2.375),
                    Units<float>::inches(-5)),
                vec4<float>::fromEuler(
                    Units<float>::degrees(0),
                    Units<float>::degrees(270),
                    Units<float>::degrees(0))),
        },

        // whiskers
        {
            transform<float>(
                vec3<float>(
                    Units<float>::inches(-5.75),
                    Units<float>::inches(3),
                    Units<float>::inches(9.75)),
                vec4<float>::fromEuler(
                    Units<float>::degrees(0),
                    Units<float>::degrees(315),
                    Units<float>::degrees(0))),
            transform<float>(
                vec3<float>(
                    Units<float>::inches(0),
                    Units<float>::inches(3),
                    Units<float>::inches(10.4375)),
                vec4<float>::fromEuler(
                    Units<float>::degrees(0),
                    Units<float>::degrees(0),
                    Units<float>::degrees(0))),
            transform<float>(
                vec3<float>(
                    Units<float>::inches(5.75),
                    Units<float>::inches(3),
                    Units<float>::inches(9.75)),
                vec4<float>::fromEuler(
                    Units<float>::degrees(0),
                    Units<float>::degrees(45),
                    Units<float>::degrees(0))),
            transform<float>(
                vec3<float>(
                    Units<float>::inches(-5.75),
                    Units<float>::inches(3),
                    Units<float>::inches(-9.75)),
                vec4<float>::fromEuler(
                    Units<float>::degrees(0),
                    Units<float>::degrees(135),
                    Units<float>::degrees(0))),
            transform<float>(
                vec3<float>(
                    Units<float>::inches(0),
                    Units<float>::inches(3),
                    Units<float>::inches(-10.4375)),
                vec4<float>::fromEuler(
                    Units<float>::degrees(0),
                    Units<float>::degrees(180),
                    Units<float>::degrees(0))),
            transform<float>(
                vec3<float>(
                    Units<float>::inches(-5.75),
                    Units<float>::inches(3),
                    Units<float>::inches(-9.75)),
                vec4<float>::fromEuler(
                    Units<float>::degrees(0),
                    Units<float>::degrees(225),
                    Units<float>::degrees(0))),
        },
    };
}

#endif