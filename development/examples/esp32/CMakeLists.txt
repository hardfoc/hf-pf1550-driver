# =============================================================================
# PF1550 Driver — ESP-IDF examples (ESP32-C6)
# =============================================================================
cmake_minimum_required(VERSION 3.16)

set(DEFAULT_APP_TYPE "pf1550_esp32c6_probe" CACHE STRING "Default app type")
set(DEFAULT_BUILD_TYPE "Debug" CACHE STRING "Default build type")
set(DEFAULT_SOURCE_FILE "pf1550_esp32c6_probe.cpp" CACHE STRING "Default source file")

if(NOT DEFINED APP_TYPE)
    message(FATAL_ERROR
        "APP_TYPE not defined. Use build_app.sh from examples/esp32/scripts (git submodule).\n"
        "Example: ./scripts/build_app.sh ${DEFAULT_APP_TYPE} ${DEFAULT_BUILD_TYPE}"
    )
endif()

if(NOT DEFINED BUILD_TYPE)
    message(FATAL_ERROR "BUILD_TYPE not defined. Use build_app.sh (see README.md).")
endif()

message(STATUS "Building app type: ${APP_TYPE}")
message(STATUS "Build type: ${BUILD_TYPE}")

set(APP_TYPE "${APP_TYPE}" CACHE STRING "App type to build" FORCE)
set(BUILD_TYPE "${BUILD_TYPE}" CACHE STRING "Build type" FORCE)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(esp32_pf1550_${APP_TYPE}_app)
