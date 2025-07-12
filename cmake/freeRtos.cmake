set( FREERTOS_PORT "GCC_RP2040" CACHE STRING "" FORCE)
set( FREERTOS_HEAP "4" CACHE STRING "" FORCE)

add_library(freertos_config INTERFACE)

target_include_directories(freertos_config SYSTEM
INTERFACE
    ${CMAKE_SOURCE_DIR}/config
)

target_compile_definitions(freertos_config
  INTERFACE
    projCOVERAGE_TEST=0
)

add_subdirectory(${CMAKE_SOURCE_DIR}/libs/FreeRTOS-Kernel)

include_directories(
    PRIVATE
    ./libs/FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040/include/
    ./libs/FreeRTOS-Kernel/include/
)
