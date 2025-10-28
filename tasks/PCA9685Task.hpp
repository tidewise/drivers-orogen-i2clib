/* Generated from orogen/lib/orogen/templates/tasks/Task.hpp */

#ifndef I2CLIB_PCA9685TASK_TASK_HPP
#define I2CLIB_PCA9685TASK_TASK_HPP

#include "i2clib/PCA9685TaskBase.hpp"

namespace i2clib {
    class I2CBus;
    class PCA9685;

    class PCA9685Task : public PCA9685TaskBase {
        friend class PCA9685TaskBase;

        using Conf = PCA9685Configuration;
        using PWMRange = Conf::PWMRange;
        using PWMDutyDurations = raw_io::PWMDutyDurations;
        using PWMAutoCommand = Conf::PWMAutoCommand;

    protected:
        std::unique_ptr<I2CBus> m_i2c;
        std::unique_ptr<PCA9685> m_device;
        uint32_t m_pwm_period = 0;

        raw_io::PWMDutyDurations m_cmd;
        std::vector<PWMRange> m_ranges;
        size_t m_expected_command_size = 0;

        std::vector<PWMAutoCommand> m_auto_behaviours;
        PWMDutyDurations applyAutoValues(PWMDutyDurations const& current,
            std::vector<PWMAutoCommand> const& auto_cmd,
            PWMAutoCommand::Mode PWMAutoCommand::*mode_field,
            uint32_t PWMAutoCommand::*duration_field);

        base::Time m_timeout;
        base::Time m_deadline;

        void writeCommand(raw_io::PWMDutyDurations const& cmd,
            std::vector<PWMRange> const& ranges);

    public:
        /** TaskContext constructor for PCA9685Task
         * \param name Name of the task. This name needs to be unique to make it
         * identifiable via nameservices. \param initial_state The initial TaskState
         * of the TaskContext. Default is Stopped state.
         */
        PCA9685Task(std::string const& name = "i2clib::PCA9685Task");

        /** Default deconstructor of PCA9685Task
         */
        ~PCA9685Task();

        /** This hook is called by Orocos when the state machine transitions
         * from PreOperational to Stopped. If it returns false, then the
         * component will stay in PreOperational. Otherwise, it goes into
         * Stopped.
         *
         * It is meaningful only if the #needs_configuration has been specified
         * in the task context definition with (for example):
         \verbatim
         task_context "TaskName" do
           needs_configuration
           ...
         end
         \endverbatim
         */
        bool configureHook();

        /** This hook is called by Orocos when the state machine transitions
         * from Stopped to Running. If it returns false, then the component will
         * stay in Stopped. Otherwise, it goes into Running and updateHook()
         * will be called.
         */
        bool startHook();

        /** This hook is called by Orocos when the component is in the Running
         * state, at each activity step. Here, the activity gives the "ticks"
         * when the hook should be called.
         *
         * The error(), exception() and fatal() calls, when called in this hook,
         * allow to get into the associated RunTimeError, Exception and
         * FatalError states.
         *
         * In the first case, updateHook() is still called, and recover() allows
         * you to go back into the Running state.  In the second case, the
         * errorHook() will be called instead of updateHook(). In Exception, the
         * component is stopped and recover() needs to be called before starting
         * it again. Finally, FatalError cannot be recovered.
         */
        void updateHook();

        /** This hook is called by Orocos when the component is in the
         * RunTimeError state, at each activity step. See the discussion in
         * updateHook() about triggering options.
         *
         * Call recover() to go back in the Runtime state.
         */
        void errorHook();

        /** This hook is called by Orocos when the state machine transitions
         * from Running to Stopped after stop() has been called.
         */
        void stopHook();

        /** This hook is called by Orocos when the state machine transitions
         * from Stopped to PreOperational, requiring the call to configureHook()
         * before calling start() again.
         */
        void cleanupHook();
    };
}

#endif
