#include "ff_ram_driver.h"
#include "string.h"
#include "ffconf.h"

BYTE RamDisk[FF_MIN_SS * FF_SECTOR_COUNT] __attribute__ ((aligned (4)));
#define N_SEC_TRACK 63			/* Sectors per track for determination of drive CHS */

DRESULT RAM_disk_status()
{
    // Always ready
    return RES_OK;
}

DRESULT RAM_disk_initialize()
{
    // Nothing to initialize for RAM disk
    return RES_OK;
}

DRESULT RAM_disk_read(BYTE* buff, LBA_t sector, UINT count)
{
    // Calculate byte offset: (sector + offset) * sector size
    memcpy(buff, &RamDisk[sector  * FF_MIN_SS], count * FF_MIN_SS);
    return RES_OK;
}


DRESULT RAM_disk_write(const BYTE* buff, LBA_t sector, UINT count)
{

    memcpy(&RamDisk[sector * FF_MIN_SS], buff, count * FF_MIN_SS);
    return RES_OK;
}

DRESULT RAM_disk_ioctl(const BYTE* cmd, const BYTE* buff)
{
    // For example, support some common IOCTL commands:
    switch (*cmd) {
        case CTRL_SYNC:
            // RAM disk is always synced
            return RES_OK;
        case GET_SECTOR_COUNT:
            *(DWORD*)buff = FF_SECTOR_COUNT;
            return RES_OK;
        case GET_SECTOR_SIZE:
            *(WORD*)buff = FF_MIN_SS;
            return RES_OK;
        case GET_BLOCK_SIZE:
            *(DWORD*)buff = 1; // erase block size in sectors
            return RES_OK;
        default:
            return RES_PARERR;
    }
}
