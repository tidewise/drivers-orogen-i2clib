/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "BMP280Task.hpp"
#include <i2clib/BMP280.hpp>
#include <i2clib/I2CBus.hpp>

using namespace i2clib;
using namespace std;
using namespace base;

BMP280Task::BMP280Task(std::string const& name)
    : BMP280TaskBase(name)
{
}

BMP280Task::~BMP280Task()
{
}

/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See BMP280Task.hpp for more detailed
// documentation about them.

bool BMP280Task::configureHook()
{
    if (!BMP280TaskBase::configureHook())
        return false;

    auto i2c_conf = _i2c_configuration.get();
    auto i2c = make_unique<I2CBus>(i2c_conf.device_path);
    i2c->setTimeout(i2c_conf.timeout);

    auto device = make_unique<BMP280>(*i2c, _i2c_address.get());
    device->sleepAndWriteConfiguration(_configuration.get());

    m_device = move(device);
    m_i2c = move(i2c);
    return true;
}
bool BMP280Task::startHook()
{
    if (!BMP280TaskBase::startHook())
        return false;

    m_device->writeMode(BMP280::MODE_NORMAL);
    return true;
}
void BMP280Task::updateHook()
{
    BMP280TaskBase::updateHook();

    auto sample = m_device->read();
    auto now = Time::now();
    _pressure.write(samples::Pressure(now, sample.pressure));
    _temperature.write(samples::Temperature(now, sample.temperature));
}
void BMP280Task::errorHook()
{
    BMP280TaskBase::errorHook();
}
void BMP280Task::stopHook()
{
    BMP280TaskBase::stopHook();
    m_device->writeMode(BMP280::MODE_SLEEP);
}
void BMP280Task::cleanupHook()
{
    BMP280TaskBase::cleanupHook();
}
