/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "PCA9685Task.hpp"

#include "PCA9685Helpers.hpp"
#include <i2clib/I2CBus.hpp>
#include <i2clib/PCA9685.hpp>

#include <base-logging/Logging.hpp>

using namespace i2clib;
using namespace raw_io;
using namespace std;
using namespace base;

using PWMRange = PCA9685Configuration::PWMRange;

PCA9685Task::PCA9685Task(std::string const& name)
    : PCA9685TaskBase(name)
{
}

PCA9685Task::~PCA9685Task()
{
}

/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See PCA9685Task.hpp for more detailed
// documentation about them.

static pair<vector<PWMRange>, PWMDutyDurations> convertAutoBehaviour(
    vector<PCA9685Configuration::PWMAutoBehaviour> const& behaviour)
{
    vector<PWMRange> ranges;
    PWMDutyDurations command;
    for (auto const& b : behaviour) {
        ranges.push_back(b);
        command.on_durations.push_back(b.on_duration);
    }
    return make_pair(ranges, command);
}

bool PCA9685Task::configureHook()
{
    if (!PCA9685TaskBase::configureHook())
        return false;

    auto conf = _configuration.get();
    pca9685helpers::validateRanges(conf.ranges);
    m_ranges = conf.ranges;

    tie(m_stop_ranges, m_stop_command) = convertAutoBehaviour(conf.stop_behaviour);
    tie(m_timeout_ranges, m_timeout_command) =
        convertAutoBehaviour(conf.timeout_behaviour);
    pca9685helpers::validateRanges(m_stop_ranges);
    pca9685helpers::validateRanges(m_timeout_ranges);

    auto i2c_conf = _i2c_configuration.get();
    auto i2c = make_unique<I2CBus>(i2c_conf.device_path);
    i2c->setTimeout(i2c_conf.timeout);

    auto device = make_unique<PCA9685>(*m_i2c, _i2c_address.get());
    device->writeSleepMode();
    device->writePrescale(conf.prescale);

    double clock_frequency = PCA9685::INTERNAL_OSCILLATOR_FREQUENCY;
    if (conf.external_oscillator_frequency) {
        clock_frequency = conf.external_oscillator_frequency;
        device->enableExternalClock();
    }
    m_pwm_period = PCA9685::prescaleToPeriod(conf.prescale, clock_frequency);

    m_i2c = move(i2c);
    m_device = move(device);
    m_timeout = conf.timeout;

    return true;
}
bool PCA9685Task::startHook()
{
    if (!PCA9685TaskBase::startHook())
        return false;

    m_device->writeNormalMode();
    writeCommand(m_timeout_command, m_timeout_ranges);
    m_deadline = Time();
    return true;
}

void PCA9685Task::updateHook()
{
    PCA9685TaskBase::updateHook();

    if (_cmd.read(m_cmd) == RTT::NewData) {
        writeCommand(m_cmd, m_ranges);
        m_deadline = Time::now() + m_timeout;
    }
    else if (Time::now() > m_deadline) {
        writeCommand(m_timeout_command, m_timeout_ranges);
    }
}

void PCA9685Task::writeCommand(PWMDutyDurations const& cmd, vector<PWMRange> const& ranges)
{
    auto mapped = pca9685helpers::mapCommand(ranges, cmd);
    for (auto const& m : mapped) {
        m_device->writeDutyTimes(m.pwm, m.durations, m_pwm_period);
    }

    _status.write(cmd);
}
void PCA9685Task::errorHook()
{
    PCA9685TaskBase::errorHook();
}
void PCA9685Task::stopHook()
{
    PCA9685TaskBase::stopHook();

    writeCommand(m_stop_command, m_stop_ranges);
    m_device->writeSleepMode();
}
void PCA9685Task::cleanupHook()
{
    PCA9685TaskBase::cleanupHook();
}
