#include "../tasks/PCA9685Helpers.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <stdexcept>

using namespace i2clib;
using namespace raw_io;
using namespace std;

using PWMRange = PCA9685Configuration::PWMRange;

struct PCA9685HelpersTest : public ::testing::Test {};

TEST_F(PCA9685HelpersTest, it_returns_an_expected_size_of_zero_for_an_empty_set_of_ranges)
{
    vector<PWMRange> ranges = {};
    ASSERT_EQ(0, pca9685helpers::expectedDurationsSize(ranges));
}
TEST_F(PCA9685HelpersTest, it_computes_the_sum_of_the_size_of_ranges)
{
    vector<PWMRange> ranges = {
        { .start = 0, .size = 5},
        { .start = 5, .size = 2},
        {.start = 10, .size = 2},
        { .start = 7, .size = 1}
    };
    ASSERT_EQ(10, pca9685helpers::expectedDurationsSize(ranges));
}
TEST_F(PCA9685HelpersTest, it_accepts_acceptable_ranges)
{
    vector<PWMRange> ranges = {
        { .start = 0, .size = 5},
        { .start = 5, .size = 2},
        {.start = 10, .size = 2},
        { .start = 7, .size = 1}
    };
    ASSERT_NO_THROW(pca9685helpers::validateRanges(ranges));
}
TEST_F(PCA9685HelpersTest, it_rejects_a_range_that_start_in_another_and_finished_after_it)
{
    vector<PWMRange> ranges = {
        {.start = 0, .size = 5},
        {.start = 5, .size = 2},
        {.start = 6, .size = 5}
    };
    ASSERT_THROW(pca9685helpers::validateRanges(ranges), std::invalid_argument);
}
TEST_F(PCA9685HelpersTest, it_rejects_ranges_whose_size_is_beyond_16)
{
    vector<PWMRange> ranges = {
        PWMRange{ .start = 0, .size = 5},
        PWMRange{.start = 10, .size = 7}
    };
    ASSERT_THROW(pca9685helpers::validateRanges(ranges), std::invalid_argument);
}
TEST_F(PCA9685HelpersTest, it_maps_command_to_set_of_duty_writes)
{
    vector<PWMRange> ranges = {
        PWMRange{ .start = 0, .size = 3},
        PWMRange{.start = 10, .size = 2},
        PWMRange{ .start = 5, .size = 1}
    };
    raw_io::PWMDutyDurations cmd = {
        .on_durations{1, 2, 3, 4, 5, 6}
    };

    vector<pca9685helpers::MappedCommand> expected{
        { .pwm = 0, .durations{1, 2, 3}},
        {.pwm = 10,    .durations{4, 5}},
        { .pwm = 5,       .durations{6}}
    };
    EXPECT_THAT(pca9685helpers::mapCommand(ranges, cmd), testing::ContainerEq(expected));
}
TEST_F(PCA9685HelpersTest, it_throws_if_the_input_command_is_too_small)
{
    vector<PWMRange> ranges = {
        PWMRange{ .start = 0, .size = 3},
        PWMRange{.start = 10, .size = 2},
        PWMRange{ .start = 5, .size = 1}
    };
    raw_io::PWMDutyDurations cmd = {
        .on_durations{1, 2, 3, 4, 5}
    };

    ASSERT_THROW(pca9685helpers::mapCommand(ranges, cmd), std::invalid_argument);
}
TEST_F(PCA9685HelpersTest, it_throws_if_the_input_command_is_too_big)
{
    vector<PWMRange> ranges = {
        PWMRange{ .start = 0, .size = 3},
        PWMRange{.start = 10, .size = 2},
        PWMRange{ .start = 5, .size = 1}
    };
    raw_io::PWMDutyDurations cmd = {
        .on_durations{1, 2, 3, 4, 5, 6, 7}
    };

    ASSERT_THROW(pca9685helpers::mapCommand(ranges, cmd), std::invalid_argument);
}