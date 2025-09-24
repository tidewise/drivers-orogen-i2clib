#include "PCA9685Helpers.hpp"

#include <algorithm>
#include <numeric>

using namespace i2clib;
using namespace i2clib::pca9685helpers;
using namespace std;
using PWMRange = PCA9685Configuration::PWMRange;

void i2clib::pca9685helpers::validateRanges(std::vector<PWMRange> const& ranges)
{
    vector<bool> matched(ranges.size(), false);
    for (auto const& r : ranges) {
        auto start = r.start;
        auto end = r.start + r.size;
        if (end > 16) {
            throw std::invalid_argument("found range that goes beyond 15 (last PWM)");
        }

        auto range_begin = matched.begin() + start;
        auto range_end = matched.begin() + end;
        if (find(range_begin, range_end, true) != range_end) {
            throw std::invalid_argument("found overlapping ranges");
        }

        fill(range_begin, range_end, true);
    }
}

size_t i2clib::pca9685helpers::expectedDurationsSize(std::vector<PWMRange> const& ranges)
{
    return accumulate(ranges.begin(), ranges.end(), 0, [](int v, auto const& r) -> int {
        return v + r.size;
    });
}

std::vector<MappedCommand> i2clib::pca9685helpers::mapCommand(
    std::vector<PWMRange> const& ranges,
    raw_io::PWMDutyDurations const& durations)
{
    vector<MappedCommand> result;

    auto src_begin = durations.on_durations.begin();
    for (auto const& r : ranges) {
        auto src_end = src_begin + r.size;
        if (src_end > durations.on_durations.end()) {
            throw std::invalid_argument("PWM command vector too small");
        }

        MappedCommand cmd{.pwm = r.start};
        cmd.durations.resize(r.size);
        copy(src_begin, src_end, cmd.durations.begin());
        result.emplace_back(move(cmd));

        src_begin = src_end;
    }

    if (src_begin != durations.on_durations.end()) {
        throw std::invalid_argument("PWM command vector too big");
    }
    return result;
}

pair<vector<PWMRange>, raw_io::PWMDutyDurations> i2clib::pca9685helpers::
    convertAutoBehaviour(vector<PCA9685Configuration::PWMAutoBehaviour> const& behaviour)
{
    vector<PWMRange> ranges;
    raw_io::PWMDutyDurations command;
    for (auto const& b : behaviour) {
        ranges.push_back(b);
        command.on_durations.resize(command.on_durations.size() + b.size, b.on_duration);
    }
    return make_pair(ranges, command);
}
