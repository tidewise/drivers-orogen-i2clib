rock_gtest(
    cxxtests
    SOURCES suite.cpp test_PCA9685Helpers.cpp ../tasks/PCA9685Helpers.cpp
    DEPS_PKGCONFIG i2clib
)