/* ================================================ INCLUDES =============================================== */
#include "rs_tasks.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "ff.h"
#include "diskio.h"
#include <pico/time.h>
#include <hardware/gpio.h>
#include "pico/stdio.h"
#include "stdio.h"
#include "sd_io.h"
#include "string.h"

/* ================================================= MACROS ================================================ */
#define FN "mydir/File.txt"

/* ============================================ LOCAL VARIABLES ============================================ */
static FRESULT res;
static FIL File[2];				/* File objects */
static DIR Dir;					/* Directory object */
static FILINFO Finfo;
static UINT bw, br;
static char write_buf[] = "FAT12 test working!\r\n";
static char read_buf[32];
static FATFS FatFs;

/* ============================================ GLOBAL VARIABLES =========================================== */
extern BYTE RamDisk[FF_SECTOR_COUNT * FF_MIN_SS] __attribute__ ((aligned (4)));
extern SD_DEV dev[1];

/* ======================================= LOCAL FUNCTION DECLARATIONS ===================================== */
static void createFs(void* param);
static void BlinkyTask(void* param);
static void HelloWorldTask(void* param);
/* ======================================== LOCAL FUNCTION DEFINITIONS ===================================== */
static void createFs(void* param)
{
    (void)param;

    MKFS_PARM opt = {
        .fmt = FM_FAT32,
        .align = 0,
        .au_size = 0,
        .n_fat = 1,
        .n_root = 0
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

    while(1){}
}

static void BlinkyTask(void* param)
{
    (void)param;

    while(1){
        gpio_put(25,1);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_put(25,0);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void HelloWorldTask(void* param)
{
    (void)param;

    while(1)
    {
        printf("Hello World!\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void sdTester(void* param)
{
    uint8_t buffer[512] = "Hello World!";    // Example of your buffer data
    (void)param;

    SDRESULTS res;
    // Part of your initialization code here

    if(SD_Init(dev)!=SD_OK)
    {
        printf("SD init failed!\n");
    }
    // You can read the sd card. For example you can read from the second
    // sector the set of bytes between [04..20]:
    // - Second sector is 1
    // - Offset is 4
    // - Bytes to count is 16 (20 minus 4)
    printf("%s\n",buffer);
    res = SD_Write(dev, (void*)buffer, 1);
    if(res!=SD_OK)
    {
        printf("failed!\n");
    }
    memset(buffer,0,sizeof(buffer));
    printf("%s\n",buffer);
    res = SD_Read(dev, (void*)buffer, 1, 0, 512);
    printf("%s\n",buffer);
    if(res!=SD_OK)
    {
        printf("failed!\n");
    }
    while(1)
    {
    }
}
/* ================================================ MODULE API ============================================= */

void createTasks(void)
{
    TaskHandle_t tskHelloWorld = NULL;
    TaskHandle_t tskCreateFile = NULL;
    TaskHandle_t tskSdTester = NULL;
    TaskHandle_t tskBlinky = NULL;
    uint32_t status;

    (void)status;

    status = xTaskCreate(
        HelloWorldTask,
        "Hello World Task",
        128,
        NULL,
        1,
        &tskHelloWorld);

    status = xTaskCreate(
        BlinkyTask,
        "Blinky Task",
        128,
        NULL,
        1,
        &tskBlinky);

    status = xTaskCreate(
        sdTester,
        "SD tester task",
        512,
        NULL,
        1,
        &tskSdTester);

    status = xTaskCreate(
        createFs,
        "Create File Task",
        512,
        NULL,
        1,
        &tskCreateFile);
    // printf("status= %d\n", status);
}

