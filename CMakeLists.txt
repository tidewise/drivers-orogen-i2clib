cmake_minimum_required(VERSION 3.1)
project(i2clib VERSION 0.0)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/.orogen/config")
include(i2clibBase)

if (ROCK_TEST_ENABLED)
    enable_testing()
    find_package(Rock REQUIRED)
    add_subdirectory(test)
endif()

# FIND_PACKAGE(KDL)
# FIND_PACKAGE(OCL)

