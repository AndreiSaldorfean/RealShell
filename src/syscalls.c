#define STM32F4
#include <stdio.h>
#include <unistd.h>
#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/gpio.h"
#include "libopencm3/stm32/usart.h"
#include "stddef.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "errno.h"
#include "ff.h"

volatile UINT Timer;	/* 1kHz increment timer */
volatile WORD rtcYear = 2017;
volatile BYTE rtcMon = 5, rtcMday = 14, rtcHour, rtcMin, rtcSec;

// int __errno_var = 0;
extern int* __errno(void);
void _exit(int status)
{
    (void)status;
    for(;;);
}

int _close(int file) {
    return -1;
}

int _lseek(int file, int ptr, int dir) {
    return 0;
}

int _fstat(int file, struct stat *st) {
    return 0;
}

int _isatty(int file) {
    return 1;
}
int _kill() {}
int _getpid() {}

extern uint32_t __heap_end__;   // heap starts here
extern uint32_t _heap_start;
extern uint32_t __stack_start__; // stack starts here (stack grows down)

static uint32_t *heap = NULL;

void* _sbrk(ptrdiff_t incr) {
    if (heap == NULL)
        heap = &_heap_start;

    uint32_t *prev_heap = heap;
    uint32_t *next_heap = heap + incr;

    // Prevent heap-stack collision
    if (next_heap > (uint8_t*)&__heap_end__) {
        errno = ENOMEM;
        return (void*) -1;
    }

    heap = next_heap;
    return (void*) prev_heap;
}

int _write(int file, char *ptr, int len)
{
    int i;
    if (file == STDOUT_FILENO || file == STDERR_FILENO) {
        for (i = 0; i < len; i++) {
            if (ptr[i] == '\n') {
                usart_send_blocking(USART2, '\r');
            }
            usart_send_blocking(USART2, ptr[i]);
        }
        return i;
    }
    errno = EIO;
    return -1;
}

int _read(int file, char *ptr, int len) {
    if (file != STDIN_FILENO) {
            errno = EBADF;
        return -1;
    }

    int i = 0;
    while (i < len) {
        ptr[i++] = usart_recv_blocking(USART2);
    }
    return i;
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;
    // You can print a message or halt here
    printf("Stack overflow in task %s\n", pcTaskName);
    taskDISABLE_INTERRUPTS();
    for(;;);
}

DWORD get_fattime (void)
{
	DWORD tmr;


	/* Pack date and time into a DWORD variable */
	tmr =	(((DWORD)rtcYear - 1980) << 25)
			| ((DWORD)rtcMon << 21)
			| ((DWORD)rtcMday << 16)
			| (WORD)(rtcHour << 11)
			| (WORD)(rtcMin << 5)
			| (WORD)(rtcSec >> 1);

	return tmr;
}
