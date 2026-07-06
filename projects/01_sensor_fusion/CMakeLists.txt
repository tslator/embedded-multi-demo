# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(sensor_fusion C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(sensor_fusion main.c)

target_sources(sensor_fusion PRIVATE
    app/app.c
    app/heartbeat_svc.c
    app/setpoint_svc.c
    app/telemetry_svc.c
    drivers/adc/adc.c
    drivers/drivers.c
    drivers/led/led.c
    drivers/encoder/encoder.c
    drivers/imu/imu.c
    platform/rp2040/core.c
    platform/rp2040/delay.c
    platform/rp2040/led.c
    platform/rp2040/serial.c
    platform/rp2040/adc.c
    platform/rp2040/time.c
    system/system.c
    system/scheduler.c
)

target_include_directories(sensor_fusion PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/app
    ${CMAKE_CURRENT_LIST_DIR}/drivers
    ${CMAKE_CURRENT_LIST_DIR}/drivers/led
    ${CMAKE_CURRENT_LIST_DIR}/platform
    ${CMAKE_CURRENT_LIST_DIR}/system
)

pico_set_program_name(sensor_fusion "sensor fusion")
pico_set_program_version(sensor_fusion "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(sensor_fusion 0)
pico_enable_stdio_usb(sensor_fusion 1)

# Add the standard library to the build
target_link_libraries(sensor_fusion
        pico_stdlib
        pico_multicore)

# Add the standard include files to the build
target_include_directories(sensor_fusion PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/platform
        ${CMAKE_CURRENT_LIST_DIR}/drivers
        ${CMAKE_CURRENT_LIST_DIR}/system
)

pico_add_extra_outputs(sensor_fusion)

