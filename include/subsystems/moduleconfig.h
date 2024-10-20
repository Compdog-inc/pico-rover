#ifndef _MODULE_CONFIG_H
#define _MODULE_CONFIG_H

#include <stdint.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include <math/units.h>

struct ModuleConfig
{
    uint frontPinCW;
    uint frontPinCCW;
    uint centerPinCW;
    uint centerPinCCW;
    uint backPinCW;
    uint backPinCCW;

    Units<float> wheelDiameter;

    constexpr ModuleConfig(uint frontPinCW,
                           uint frontPinCCW,
                           uint centerPinCW,
                           uint centerPinCCW,
                           uint backPinCW,
                           uint backPinCCW,
                           Units<float> wheelDiameter) : frontPinCW(frontPinCW),
                                                         frontPinCCW(frontPinCCW),
                                                         centerPinCW(centerPinCW),
                                                         centerPinCCW(centerPinCCW),
                                                         backPinCW(backPinCW),
                                                         backPinCCW(backPinCCW),
                                                         wheelDiameter(wheelDiameter)
    {
    }
};

#endif