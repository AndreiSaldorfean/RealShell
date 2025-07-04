#include <stdint.h>
#define STM32F4
#include <stddef.h>
#include "string.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/gpio.h"
#include "libopencm3/stm32/usart.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "ff.h"
#include "diskio.h"
#include "ff_ram_driver.h"

int main();

volatile UINT Timer;	/* 1kHz increment timer */
volatile WORD rtcYear = 2017;
volatile BYTE rtcMon = 5, rtcMday = 14, rtcHour, rtcMin, rtcSec;

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
    0, 0, 0, 0,                       // Reserved
    vPortSVCHandler,
    DebugMon_Handler,                 // Debug monitor handler
    0,                               // Reserved
    xPortPendSVHandler,
    SysTick_Handler,                  // SysTick handler (used by FreeRTOS)
};

static void clock_setup(void)
{
    /* Enable GPIOD clock for LED & USARTs. */
    rcc_periph_clock_enable(RCC_GPIOD);
    rcc_periph_clock_enable(RCC_GPIOA);

    /* Enable clocks for USART2. */
    rcc_periph_clock_enable(RCC_USART2);
}

static void usart_setup(void)
{
    /* Setup USART2 parameters. */
    usart_set_baudrate(USART2, 115200);
    usart_set_databits(USART2, 8);
    usart_set_stopbits(USART2, USART_STOPBITS_1);
    usart_set_mode(USART2, USART_MODE_TX);
    usart_set_parity(USART2, USART_PARITY_NONE);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

    /* Finally enable the USART. */
    usart_enable(USART2);
}

static void gpio_setup(void)
{
    /* Setup GPIO pin GPIO12 on GPIO port D for LED. */
    gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);

    /* Setup GPIO pins for USART2 transmit. */
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);

    /* Setup USART2 TX pin as alternate function. */
    gpio_set_af(GPIOA, GPIO_AF7, GPIO2);
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

void HelloWorldTask(void* param)
{
    printf("Hello World!\n");
}

void createFileTask(void* param)
{
    // // Create and open a file
    // res = f_open(&file, "hello.txt", FA_WRITE | FA_CREATE_ALWAYS);
    // if (res != FR_OK) {
    //     printf("f_open failed: %d\n", res);
    //     return;
    // }

    // // Write to file
    // const char *msg = "Hello from FatFs!\n";
    // res = f_write(&file, msg, strlen(msg), &bw);
    // if (res != FR_OK || bw != strlen(msg)) {
    //     printf("f_write failed: %d\n", res);
    //     f_close(&file);
    //     return;
    // }

    // printf("Wrote to file: %s\n", msg);
    // f_close(&file);
}

extern BYTE RamDisk[FF_SECTOR_COUNT * FF_MIN_SS] __attribute__ ((aligned (4)));

void list_files()
{
    DIR dir;
    FILINFO fno;
    FRESULT res;

    res = f_opendir(&dir, "/");  // open root directory
    if (res != FR_OK) {
        printf("Failed to open root directory: %d\n", res);
        return;
    }

    printf("Listing files:\n");

    while (1) {
        res = f_readdir(&dir, &fno);  // read a directory item
        if (res != FR_OK || fno.fname[0] == 0) break;  // error or end of dir

        if (fno.fattrib & AM_DIR) {
            printf("[DIR]  %s\n", fno.fname);
        } else {
            printf("       %s  (%lu bytes)\n", fno.fname, fno.fsize);
        }
    }

    f_closedir(&dir);
}

void dump_ramdisk_sector(uint32_t sector, uint32_t count)
{
    uint32_t offset = sector * FF_MIN_SS;
    for (uint32_t i = 0; i < count * FF_MIN_SS; i++) {
        printf("%c ", RamDisk[offset + i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    printf("\n");
}

FRESULT res;
FIL File[2];				/* File objects */
DIR Dir;					/* Directory object */
FILINFO Finfo;
UINT bw, br;
char write_buf[] = "FAT12 test working!\r\n";
char read_buf[32];

FATFS FatFs;

#define FN "mydir/File.txt"
void createFs()
{
    MKFS_PARM opt = {
        .fmt = FM_FAT,
        .align = 0,
        .au_size = 0,
        .n_fat = 1,
        .n_root = 512
    };

    BYTE buffer[FF_MIN_SS];

    // Mount (mounting first is mandatory even for formatting)
    res = f_mkfs("", &opt, buffer, sizeof(buffer));
    if (res != FR_OK) {
        printf("f_mkfs failed: %d\n", res);
        return;
    }

    res = f_mount(&FatFs, "", 1);
    if (res != FR_OK) {
        printf("f_mount failed: %d\n", res);
        return;
    }

    FRESULT res;

    // Create directory "mydir"
    res = f_mkdir("mydir");
    if (res == FR_OK) {
        printf("Directory created successfully.\n");
    } else if (res == FR_EXIST) {
        printf("Directory already exists.\n");
    } else {
        printf("Failed to create directory: %d\n", res);
    }
    FILINFO fno;
    res = f_stat("mydir", &fno);
    if (res == FR_OK) {
        if (fno.fattrib & AM_DIR) {
            printf("Directory exists.\n");
        } else {
            printf("Found but not a directory.\n");
        }
    } else if (res == FR_NO_FILE) {
        printf("Directory does not exist.\n");
    } else {
        printf("Failed to get directory info: %d\n", res);
    }

    FIL f;

    res = f_open(&f, FN, FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) {
        printf("Failed to open file: %d\n", res);
        return;
    }

    {
        FILINFO fno;
        FRESULT fr;

        // Try to get file info (exists if FR_OK)
        fr = f_stat(FN, &fno);
        if (fr == FR_OK) {
            printf("File exists: %s, size: %lu bytes\n", "TEST.TXT", fno.fsize);
        } else {
            printf("File does not exist or error: %d\n", fr);
        }
    }


    res = f_write(&f, write_buf, sizeof(write_buf) - 1, &bw);
    if (res != FR_OK) {
        printf("Failed to write to file: %d\n", res);
        return;
    }
    res = f_close(&f);
    if (res != FR_OK) {
        printf("Failed to read file: %d\n", res);
    }

    res = f_open(&f, FN, FA_READ);

    res = f_read(&f, read_buf, sizeof(write_buf) - 1, &br);
    if (res != FR_OK) {
        printf("Failed to read file: %d\n", res);
    }
    printf("%s\n",read_buf);

    res = f_close(&f);


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

int main(void)
{
    char ceva[16];
    clock_setup();
    gpio_setup();
    usart_setup();
    createFs();
    setbuf(stdout, NULL); // disable buffering

    TaskHandle_t gHelloWorld = NULL;
    TaskHandle_t createFile = NULL;


    uint32_t status = xTaskCreate(
        HelloWorldTask,
        "Hello World Task",
        100,
        NULL,
        tskIDLE_PRIORITY,
        &gHelloWorld);

    uint32_t status1 = xTaskCreate(
        createFileTask,
        "Create File Task",
        100,
        NULL,
        tskIDLE_PRIORITY,
        &createFile);

    printf("Status: %X", status);
    if (status  != pdPASS) {
        printf("Task creation failed!\n");
        while(1);
    }

    vTaskStartScheduler();

    while (1) {}

    return 0;
}
