#include <gtest/gtest.h>
#include "../tasks/PCA9685Helpers.hpp"

#include <stdexcept>

using namespace i2clib;
using namespace std;

using PWMRange = PCA9685Configuration::PWMRange;

struct PCA9685HelpersTest : public ::testing::Test {
};

TEST_F(PCA9685HelpersTest, it_returns_an_expected_size_of_zero_for_an_empty_set_of_ranges) {
    vector<PWMRange> ranges = {};
    ASSERT_EQ(0, pca9685helpers::expectedDurationsSize(ranges));
}
TEST_F(PCA9685HelpersTest, it_computes_the_sum_of_the_size_of_ranges) {
    vector<PWMRange> ranges = {
        {.start = 0, .size = 5},
        {.start = 5, .size = 2},
        {.start = 10, .size = 2},
        {.start = 7, .size = 1}
    };
    ASSERT_EQ(10, pca9685helpers::expectedDurationsSize(ranges));
}
TEST_F(PCA9685HelpersTest, it_accepts_acceptable_ranges) {
    vector<PWMRange> ranges = {
        {.start = 0, .size = 5},
        {.start = 5, .size = 2},
        {.start = 10, .size = 2},
        {.start = 7, .size = 1}
    };
    ASSERT_NO_THROW(pca9685helpers::validateRanges(ranges));
}
TEST_F(PCA9685HelpersTest, it_rejects_a_range_that_start_in_another_and_finished_after_it) {
    vector<PWMRange> ranges = {
        {.start = 0, .size = 5},
        {.start = 5, .size = 2},
        {.start = 6, .size = 5}
    };
    ASSERT_THROW(pca9685helpers::validateRanges(ranges), std::invalid_argument);
}
TEST_F(PCA9685HelpersTest, it_rejects_ranges_whose_size_is_beyond_16) {
    vector<PWMRange> ranges = {
        PWMRange{.start = 0, .size = 5},
        PWMRange{.start = 10, .size = 7}
    };
    ASSERT_THROW(pca9685helpers::validateRanges(ranges), std::invalid_argument);
}