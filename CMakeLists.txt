cmake_minimum_required(VERSION 3.14)

include(./libs/pico-sdk/pico_sdk_init.cmake)
include(./cmake/rp2040.cmake)

project(RealShell LANGUAGES C ASM CXX)

file(GLOB SOURCES
    "src/*.c"
    "${CMAKE_SOURCE_DIR}./libs/FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040/*.c"
    "./libs/fatfs/source/*.c"
    "./libs/ulibSD/*.c"
)

add_executable(${PROJECT_NAME} ${SOURCES})

include(./cmake/pico-sdk.cmake)
include(./cmake/freeRtos.cmake)
include(./cmake/fatfs.cmake)

file(GLOB V_GLOB LIST_DIRECTORIES true "./libs/pico-sdk/src/rp2040/*")
foreach(item ${V_GLOB})
	if(IS_DIRECTORY ${item})
        include_directories(PRIVATE ${item}/include/)
	endif()
endforeach()

file(GLOB V_GLOB LIST_DIRECTORIES true "./libs/pico-sdk/src/rp2_common/*")
foreach(item ${V_GLOB})
	if(IS_DIRECTORY ${item})
        include_directories(PRIVATE ${item}/include/)
	endif()
endforeach()

include_directories(
    PRIVATE
    ./inc/
    ./config/
    /home/rudy/tools/arm-gnu-toolchain/lib/gcc/arm-none-eabi/14.2.1/
    /home/rudy/tools/arm-gnu-toolchain/lib/gcc/
    /home/rudy/tools/arm-gnu-toolchain/arm-none-eabi/lib/
    ./libs/pico-sdk/src/rp2040/pico_platform/include/
    ./libs/ulibSD/
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    freertos_config
    freertos_kernel
    pico_stdlib
    pico_stdio_usb
    hardware_spi
    hardware_timer
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND utils/add_path_to_comp_com
)
