#ifndef RAM_DRIVER
#define RAM_DRIVER

#include "diskio.h"

// /* Results of Disk Functions */
// typedef enum {
// 	RES_OK = 0,		/* 0: Successful */
// 	RES_ERROR,		/* 1: R/W Error */
// 	RES_WRPRT,		/* 2: Write Protected */
// 	RES_NOTRDY,		/* 3: Not Ready */
// 	RES_PARERR		/* 4: Invalid Parameter */
// } DRESULT;

DRESULT RAM_disk_status();
DRESULT RAM_disk_initialize();
DRESULT RAM_disk_read(BYTE*, LBA_t, UINT);
DRESULT RAM_disk_write(const BYTE*, LBA_t, UINT);
DRESULT RAM_disk_ioctl(const BYTE*, const BYTE*);

#endif /* RAM_DRIVER */