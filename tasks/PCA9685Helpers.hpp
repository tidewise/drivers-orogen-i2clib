#ifndef i2clib_orogen_PCA9685HELPERS_HPP
#define i2clib_orogen_PCA9685HELPERS_HPP

#include <i2clib/i2clibTypes.hpp>
#include <raw_io/PWMDutyDurations.hpp>

#include <cstdint>
#include <vector>

namespace i2clib {
    namespace pca9685helpers {
        struct MappedCommand {
            int pwm;
            std::vector<uint32_t> durations;

            bool operator==(MappedCommand const& other) const {
                return pwm == other.pwm && durations == other.durations;
            }
        };

        /** Validate the given set of ranges
         */
        void validateRanges(std::vector<PCA9685Configuration::PWMRange> const& ranges);

        /** Return the size of the flat command array that is compatible with the given
         * ranges
         *
         * Use this to later validate the input command size, before using mapCommand
         */
        size_t expectedDurationsSize(
            std::vector<PCA9685Configuration::PWMRange> const& ranges);

        /** Map a flat input pwm command into a set of commands ready to send to the
         * PCA9685
         *
         * The function expects that the size of \c durations has already been validated.
         * See \c expectedDurationsSize
         */
        std::vector<MappedCommand> mapCommand(
            std::vector<PCA9685Configuration::PWMRange> const& ranges,
            raw_io::PWMDutyDurations const& durations);
    };
}

#endif