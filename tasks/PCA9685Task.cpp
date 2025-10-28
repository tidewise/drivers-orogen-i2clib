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

bool PCA9685Task::configureHook()
{
    if (!PCA9685TaskBase::configureHook())
        return false;

    auto conf = _configuration.get();
    pca9685helpers::validateRanges(conf.ranges);
    m_ranges = conf.ranges;
    m_expected_command_size = pca9685helpers::expectedDurationsSize(m_ranges);

    m_auto_behaviours = conf.auto_behaviours;
    if (m_auto_behaviours.size() != m_expected_command_size) {
        throw std::invalid_argument(
            "invalid size for the auto_behaviours array of the configuration property, "
            "got " +
            to_string(m_auto_behaviours.size()) + " but was expecting " +
            to_string(m_expected_command_size));
    }

    auto i2c_conf = _i2c_configuration.get();
    auto i2c = make_unique<I2CBus>(i2c_conf.device_path);
    i2c->setTimeout(i2c_conf.timeout);

    auto device = make_unique<PCA9685>(*i2c, _i2c_address.get());
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

    m_cmd.on_durations.clear();
    for (auto const& cmd : m_auto_behaviours) {
        m_cmd.on_durations.push_back(cmd.timeout_on_duration);
    }
    writeCommand(m_cmd, m_ranges);
    m_deadline = Time();
    return true;
}

void PCA9685Task::updateHook()
{
    PCA9685TaskBase::updateHook();

    PWMDutyDurations cmd;
    if (_cmd.read(cmd) == RTT::NewData) {
        if (cmd.on_durations.size() != m_expected_command_size) {
            exception(INVALID_INPUT_SIZE);
            return;
        }

        m_deadline = Time::now() + m_timeout;
    }
    else if (Time::now() > m_deadline) {
        cmd = applyAutoValues(m_cmd,
            m_auto_behaviours,
            &PWMAutoCommand::timeout_mode,
            &PWMAutoCommand::timeout_on_duration);
    }
    writeCommand(cmd, m_ranges);
    m_cmd = cmd;
}

void PCA9685Task::writeCommand(PWMDutyDurations const& cmd,
    vector<PWMRange> const& ranges)
{
    auto mapped = pca9685helpers::mapCommand(ranges, cmd);
    for (auto const& m : mapped) {
        m_device->writeDutyTimes(m.pwm, m.durations, m_pwm_period);
    }

    auto status = cmd;
    status.time = base::Time::now();
    _status.write(status);
}
void PCA9685Task::errorHook()
{
    PCA9685TaskBase::errorHook();
}
void PCA9685Task::stopHook()
{
    PCA9685TaskBase::stopHook();

    auto stop_cmd = applyAutoValues(m_cmd,
        m_auto_behaviours,
        &PWMAutoCommand::stop_mode,
        &PWMAutoCommand::stop_on_duration);
    writeCommand(stop_cmd, m_ranges);
    m_device->writeSleepMode();
}
void PCA9685Task::cleanupHook()
{
    PCA9685TaskBase::cleanupHook();
}

PWMDutyDurations PCA9685Task::applyAutoValues(PWMDutyDurations const& current,
    vector<PWMAutoCommand> const& auto_cmd,
    PWMAutoCommand::Mode PWMAutoCommand::*mode_field,
    uint32_t PWMAutoCommand::*duration_field)
{
    if (current.on_durations.size() != auto_cmd.size()) {
        throw std::invalid_argument(
            "duty cycle and auto command size mismatch in applyTimeoutValue");
    }

    auto result = current;
    for (size_t i = 0; i < auto_cmd.size(); ++i) {
        auto pwm = auto_cmd[i];
        auto mode = pwm.*mode_field;
        auto on_duration = pwm.*duration_field;
        if (mode == PWMAutoCommand::MODE_SET) {
            result.on_durations[i] = on_duration;
        }
    }
    return result;
}
