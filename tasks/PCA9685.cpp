/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "PCA9685.hpp"

using namespace i2clib;

PCA9685::PCA9685(std::string const& name)
    : PCA9685Base(name)
{
}

PCA9685::~PCA9685()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See PCA9685.hpp for more detailed
// documentation about them.

bool PCA9685::configureHook()
{
    if (! PCA9685Base::configureHook())
        return false;
    return true;
}
bool PCA9685::startHook()
{
    if (! PCA9685Base::startHook())
        return false;
    return true;
}
void PCA9685::updateHook()
{
    PCA9685Base::updateHook();
}
void PCA9685::errorHook()
{
    PCA9685Base::errorHook();
}
void PCA9685::stopHook()
{
    PCA9685Base::stopHook();
}
void PCA9685::cleanupHook()
{
    PCA9685Base::cleanupHook();
}
