cmake_minimum_required(VERSION 3.14)

include(./libs/pico-sdk/pico_sdk_init.cmake)
include(./cmake/rp2040.cmake)

project(RealShell LANGUAGES C ASM CXX)

file(GLOB SOURCES
    "src/*.c"
    "${CMAKE_SOURCE_DIR}./libs/FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040/*.c"
    "./libs/fatfs/source/*.c"
)

add_executable(${PROJECT_NAME} ${SOURCES})

include(./cmake/pico-sdk.cmake)
include(./cmake/freeRtos.cmake)
include(./cmake/fatfs.cmake)

include_directories(
    PRIVATE
    ./inc/
    ./config/
    /home/rudy/tools/arm-gnu-toolchain/lib/gcc/arm-none-eabi/14.2.1/
    /home/rudy/tools/arm-gnu-toolchain/lib/gcc/
    /home/rudy/tools/arm-gnu-toolchain/arm-none-eabi/lib/
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    freertos_config
    freertos_kernel
    pico_stdlib
    pico_stdio_usb
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND utils/add_path_to_comp_com
)
