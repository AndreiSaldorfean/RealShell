/* ================================================ INCLUDES =============================================== */
#include "rs_init.h"
#include <hardware/gpio.h>
#include <hardware/clocks.h>
#include <stddef.h>
#include "string.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "pico/stdio.h"

/* ================================================= MACROS ================================================ */
/* ============================================ LOCAL VARIABLES ============================================ */
static void rs_gpio_init()
{
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
}
/* ============================================ GLOBAL VARIABLES =========================================== */
/* ======================================= LOCAL FUNCTION DECLARATIONS ===================================== */
/* ======================================== LOCAL FUNCTION DEFINITIONS ===================================== */
/* ================================================ MODULE API ============================================= */

void rs_init_peripherals(void)
{
    set_sys_clock_48mhz();
    stdio_init_all();
    rs_gpio_init();
}

