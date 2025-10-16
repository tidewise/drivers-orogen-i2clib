/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "MS5837Task.hpp"
#include <i2clib/I2CBus.hpp>
#include <i2clib/MS5837.hpp>

using namespace i2clib;
using namespace std;

MS5837Task::MS5837Task(std::string const& name)
    : MS5837TaskBase(name)
{
}

MS5837Task::~MS5837Task()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See MS5837Task.hpp for more detailed
// documentation about them.

bool MS5837Task::configureHook()
{
    if (! MS5837TaskBase::configureHook())
        return false;

    m_conf = _configuration.get();

    auto i2c_conf = _i2c_configuration.get();
    auto i2c = make_unique<I2CBus>(i2c_conf.device_path);
    i2c->setTimeout(i2c_conf.timeout);

    auto device = make_unique<MS5837>(m_conf.model, *i2c, _i2c_address.get());
    // Datasheet recommends resetting the chip
    device->reset();

    m_device = move(device);
    m_i2c = move(i2c);
    return true;
}
bool MS5837Task::startHook()
{
    if (! MS5837TaskBase::startHook())
        return false;
    return true;
}
void MS5837Task::updateHook()
{
    MS5837TaskBase::updateHook();

    auto meas = m_device->read(m_conf.temperature_osr, m_conf.pressure_osr);

    base::samples::Temperature temperature = { meas.time, meas.temperature };
    _temperature.write(temperature);
    base::samples::Pressure pressure = { meas.time, meas.pressure };
    _pressure.write(pressure);
}
void MS5837Task::errorHook()
{
    MS5837TaskBase::errorHook();
}
void MS5837Task::stopHook()
{
    MS5837TaskBase::stopHook();
}
void MS5837Task::cleanupHook()
{
    MS5837TaskBase::cleanupHook();
}
