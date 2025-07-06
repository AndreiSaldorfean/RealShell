pico_sdk_init()

pico_enable_stdio_usb(${PROJECT_NAME}   0)
pico_enable_stdio_uart(${PROJECT_NAME}  1)
pico_add_extra_outputs(${PROJECT_NAME})

include_directories(PRIVATE
    ./libs/pico-sdk/src/common/pico_stdlib_headers/include/
    ./libs/pico-sdk/src/common/pico_base_headers/include/
)
