#include "rs_init.h"
#include "rs_tasks.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

int main(void)
{
    rs_init_peripherals();

    createTasks();

    vTaskStartScheduler();

    while (1) {}

    return 0;
}
