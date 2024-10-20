#ifndef _LIGHTS_H
#define _LIGHTS_H

enum class Light
{
    Left,
    Right
};

enum class LightMode
{
    Off,
    On,
    Pulse,
    Blink,
    Alt1,
    Alt2,
};

void light_subsystem_init();
void light_subsystem_deinit();
void light_subsystem_set(Light light, LightMode mode);

#endif