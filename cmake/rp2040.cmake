set(PICO_SDK_PATH libs/pico-sdk)
set(TOOLCHAIN_PATH "/home/rudy/tools/arm-gnu-toolchain")
set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/bin/arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PATH}/bin/arm-none-eabi-gcc)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(COMMON_FLAGS
    "-mcpu=cortex-m0plus \
    -mthumb \
    -mfloat-abi=soft")

set(CMAKE_CXX_FLAGS
    "${COMMON_FLAGS} \
    -Wall \
    --specs=nano.specs \
    -Wextra \
    -g \
    -O0 \
    -Wa,-alh=output.lst")

set(CMAKE_C_FLAGS
    "${COMMON_FLAGS} \
    -Wall \
    --specs=nano.specs \
    -Wextra \
    -g \
    -O0 \
    -Wa,-alh=output.lst")

set(CMAKE_ASM_FLAGS ${COMMON_FLAGS} CACHE STRING "" FORCE)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_EXECUTABLE_SUFFIX ".elf")
# Prevent TinyUSB from re-importing the SDK incorrectly
set(PICO_SDK_PATH ${CMAKE_SOURCE_DIR}/libs/pico-sdk CACHE PATH "Path to the Pico SDK" FORCE)
set(PICO_TINYUSB_PATH ${PICO_SDK_PATH}/lib/tinyusb CACHE PATH "TinyUSB source path")
