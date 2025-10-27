#ifndef i2clib_TYPES_HPP
#define i2clib_TYPES_HPP

#include <base/Float.hpp>
#include <base/Time.hpp>
#include <cstdint>

#include <i2clib/MS5837.hpp>

namespace i2clib {
    /** Common i2c access configuration */
    struct I2CConfiguration {
        /** Path to the i2c character device */
        std::string device_path;

        /** i2c transaction timeout */
        base::Time timeout = base::Time::fromMilliseconds(100);
    };

    /** Configuration of a PCA9685 chip */
    struct PCA9685Configuration {
        static constexpr int PWM_COUNT = 16;

        /** Change the duration of the PWM cycle
         *
         * Internally, the chip allows for the selection of a PWM period based
         * on a 'prescale' parameter that can be set from 3 to 255. The
         * available periods depend on the oscillator frequency (25MHz by
         * default, set \c external_oscillator_frequency to use an external oscillator
         * and provide its frequency).
         *
         * The chip's datasheet explains how to compute the prescale parameter that is
         * closest to a desired period (given the oscillator frequency). Note that this
         * might not be the prescale parameter you want. Depending on the devices you are
         * controlling with the PWM, you might need a period of "at least" some duration.
         * The i2c_pca9685_ctl tool has `period-to-prescale` and `prescale-to-period` to
         * help you play with the values and select the one that is appropriate.
         */
        std::uint8_t prescale = 3;

        /** Frequency of an external oscillator, in Hz
         *
         * If left unset, the component will assume that the internal oscillator
         * is meant to be used
         *
         * Note that while the frequency value is only used internally in the driver
         * (not used by the hardware itself), the fact that an external oscillator
         * should be used cannot be "unset" without a chip reset.
         *
         * In other words, once this is set to not-NaN, setting it back to NaN requires
         * resetting the hardware.
         */
        float external_oscillator_frequency = base::unset<float>();

        /** Range of continguous PWMs
         *
         * The default is the range representing all of the PWMs of the chip
         */
        struct PWMRange {
            int start = 0;
            int size = PWM_COUNT;
        };

        /** List of PWMs ranges to be controlled
         *
         * The command input will be interpreted as the list of duty durations
         * following the ordering set by this vector.
         *
         * It is *highly* recommended to group in as few ranges as possible. Not
         * doing so will have a very bad effect on the utilization of the i2c bus
         */
        std::vector<PWMRange> ranges{PWMRange()};

        /** Configuration of automated behaviours (init, stop and timeout) */
        struct PWMAutoCommand {
            enum Mode {
                MODE_KEEP = 0,
                MODE_SET = 1
            };

            Mode timeout_mode = MODE_KEEP;

            /** Value written on the PWM on timeout if timeout_mode is MODE_SET.
             * It is used as an initialization value on component start if the mode
             * is MODE_KEEP.
             */
            uint32_t timeout_on_duration = 0;

            Mode stop_mode = MODE_KEEP;

            /** Value written on the PWM on stop if stop_mode is MODE_SET.
             */
            uint32_t stop_on_duration = 0;
        };

        /** Duration after which the component will write the configured timeout
         * values
         */
        base::Time timeout{base::Time::fromMilliseconds(100)};

        /** Per-PWM parameter for init, timeout and stop behaviour. It has to have the
         * same size as the expected command vector
         */
        std::vector<PWMAutoCommand> auto_behaviours;
    };

    struct MS5837Configuration {
        MS5837::Models model = MS5837::MODEL_30BA;

        /** Oversampling factor for temperature acquisition
         *
         * The oversampling factor, between 0 and 5 that corresponds to
         * 256 samples up to 8192. Acquisition time grows exponentially with this
         * parameter, from 640us to 20.5ms
         */
        int temperature_osr = 0;

        /** Oversampling factor for pressure acquisition
         *
         * The oversampling factor, between 0 and 5 that corresponds to
         * 256 samples up to 8192. Acquisition time grows exponentially with this
         * parameter, from 640us to 20.5ms
         */
        int pressure_osr = 0;
    };
}

#endif
