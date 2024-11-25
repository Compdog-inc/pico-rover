// Standard headers
#include <stdlib.h>

// Libraries
#include <motor.h>
#include <math/units.h>
#include <kinematics/differentialdrive.h>

// Config headers
#include "config/options.h"

#include "subsystems/drivetrain.h"

DifferentialModule::DifferentialModule(const ModuleConfig &config) : motorFront(new Motor(config.frontPinCW, config.frontPinCCW)),
                                                                     motorCenter(new Motor(config.centerPinCW, config.centerPinCCW)),
                                                                     motorBack(new Motor(config.backPinCW, config.backPinCCW)),
                                                                     wheelDiameter(config.wheelDiameter)
{
    stop();
}

DifferentialModule::~DifferentialModule()
{
    stop();
    delete motorFront;
    delete motorCenter;
    delete motorBack;
}

void DifferentialModule::setDesiredState(Units<float> speed)
{
    motorFront->set(speed.meters());
    motorCenter->set(speed.meters());
    motorBack->set(speed.meters());
}

void DifferentialModule::stop()
{
    motorFront->set(0);
    motorCenter->set(0);
    motorBack->set(0);
}

Drivetrain::Drivetrain() : kinematics(new DifferentialDriveKinematics(Config::Drivetrain::ROBOT_WHEEL_DISTANCE)),
                           left(new DifferentialModule(Config::Drivetrain::LEFT_CONSTANTS)),
                           right(new DifferentialModule(Config::Drivetrain::RIGHT_CONSTANTS))
{
    stop();
}

Drivetrain::~Drivetrain()
{
    stop();
    delete kinematics;
    delete left;
    delete right;
}

void Drivetrain::drive(Units<float> speed, Units<float> rotation)
{
    DifferentialDriveWheelSpeeds wheelSpeeds = kinematics->toWheelSpeeds(ChassisSpeeds<float>(speed, Units<float>::meters(0), rotation));
    wheelSpeeds.normalize(Config::Drivetrain::ROBOT_MAX_SPEED);

    left->setDesiredState(wheelSpeeds.left);
    right->setDesiredState(wheelSpeeds.right);
}

void Drivetrain::stop()
{
    drive(Units<float>::meters(0), Units<float>::radians(0));
    left->stop();
    right->stop();
}