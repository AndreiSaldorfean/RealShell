#include "rs_ls.h"
#include <stddef.h>
#include "string.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "ff.h"

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
