#ifndef RAM_DRIVER
#define RAM_DRIVER

#include "diskio.h"

DRESULT SD_disk_status();
DRESULT SD_disk_initialize();
DRESULT SD_disk_read(BYTE*, LBA_t, UINT);
DRESULT SD_disk_write(const BYTE*, LBA_t, UINT);
DRESULT SD_disk_ioctl(const BYTE*, const BYTE*);

#endif /* RAM_DRIVER */
