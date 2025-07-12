/* ================================================ INCLUDES =============================================== */
#include "rs_init.h"
#include "rs_tasks.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"
#include <pico/time.h>

/* ================================================= MACROS ================================================ */
/* ============================================ LOCAL VARIABLES ============================================ */
/* ============================================ GLOBAL VARIABLES =========================================== */
/* ======================================= LOCAL FUNCTION DECLARATIONS ===================================== */
/* ======================================== LOCAL FUNCTION DEFINITIONS ===================================== */
/* ================================================ MODULE API ============================================= */

int main(void)
{
    rs_init_peripherals();
    sleep_ms(1000); /* wait for uart and usb to initialize */

    createTasks();

    vTaskStartScheduler();

    while (1) {}
    return 0;
}

