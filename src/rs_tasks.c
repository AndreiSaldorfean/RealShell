#include "rs_tasks.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "ff.h"
#include "diskio.h"
#include "ff_ram_driver.h"


FRESULT res;
FIL File[2];				/* File objects */
DIR Dir;					/* Directory object */
FILINFO Finfo;
UINT bw, br;
char write_buf[] = "FAT12 test working!\r\n";
char read_buf[32];

FATFS FatFs;

#define FN "mydir/File.txt"

extern BYTE RamDisk[FF_SECTOR_COUNT * FF_MIN_SS] __attribute__ ((aligned (4)));

static void createFs(void* param)
{
    (void)param;

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

static void HelloWorldTask(void* param)
{
    (void)param;

    printf("Hello World!\n");
}

void createTasks(void)
{
    TaskHandle_t gHelloWorld = NULL;
    TaskHandle_t createFile = NULL;
    uint32_t status;

    (void)status;

    status = xTaskCreate(
        HelloWorldTask,
        "Hello World Task",
        100,
        NULL,
        tskIDLE_PRIORITY,
        &gHelloWorld);

    status = xTaskCreate(
        createFs,
        "Create File Task",
        100,
        NULL,
        tskIDLE_PRIORITY,
        &createFile);
}
