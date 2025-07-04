#define STM32F4
#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/gpio.h"
#include "libopencm3/stm32/usart.h"

extern int main(void);
void NMI_Handler(void)           __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)      __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void)    __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)           __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void)      __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)        __attribute__((weak, alias("Default_Handler")));
extern void xPortSysTickHandler(void);

// SysTick handler required by FreeRTOS
void SysTick_Handler(void)
{
    gpio_toggle(GPIOD, GPIO12);
    xPortSysTickHandler();
}

// Default empty handler
void Default_Handler(void)
{
    while (1) {}
}

extern uint32_t _etext, _sdata, _edata, __bss_start__, __bss_end__;
void Reset_Handler(void)
{
    uint32_t *src = &_etext;
    uint32_t *dst = &_sdata;
    while (dst < &_edata)
        *dst++ = *src++;

    // Zero .bss
    dst = &__bss_start__;
    while (dst < &__bss_end__)
        *dst++ = 0;
    // Call main
    main();

    // If main returns, loop forever
    while (1);
}

extern void vPortSVCHandler(void);
extern void xPortPendSVHandler( void );
__attribute__((section(".isr_vector")))
void (* const vector_table[])(void) = {
    (void (*)(void))(0x20000000 + 128 * 1024), // Initial stack pointer
    Reset_Handler,                              // Reset handler
    NMI_Handler,                       // NMI handler
    HardFault_Handler,                 // Hard fault handler
    MemManage_Handler,                 // MPU fault handler
    BusFault_Handler,                  // Bus fault handler
    UsageFault_Handler,                // Usage fault handler
    0, 0, 0, 0,                        // Reserved
    vPortSVCHandler,
    DebugMon_Handler,                  // Debug monitor handler
    0,                                 // Reserved
    xPortPendSVHandler,
    SysTick_Handler,                   // SysTick handler (used by FreeRTOS)
};
