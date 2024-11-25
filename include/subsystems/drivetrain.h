#ifndef _DRIVETRAIN_H
#define _DRIVETRAIN_H

#include <motor.h>
#include <math/units.h>
#include <kinematics/differentialdrive.h>
#include "moduleconfig.h"

class DifferentialModule
{
public:
    DifferentialModule(const ModuleConfig &config);
    ~DifferentialModule();

    void setDesiredState(Units<float> speed);
    void stop();

private:
    Motor *motorFront;
    Motor *motorCenter;
    Motor *motorBack;

    Units<float> wheelDiameter;
};

class Drivetrain
{
public:
    Drivetrain();
    ~Drivetrain();

    void drive(Units<float> speed, Units<float> rotation);
    void stop();

private:
    DifferentialDriveKinematics *kinematics;

    DifferentialModule *left;
    DifferentialModule *right;
};

#endif