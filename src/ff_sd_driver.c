/* ================================================ INCLUDES =============================================== */
#include "ff_sd_driver.h"
#include "diskio.h"
#include "ffconf.h"
#include "stdio.h"
#include "sd_io.h"

/* ================================================= MACROS ================================================ */
/* ============================================ LOCAL VARIABLES ============================================ */
/* ============================================ GLOBAL VARIABLES =========================================== */
SD_DEV dev[1];
/* ======================================= LOCAL FUNCTION DECLARATIONS ===================================== */
/* ======================================== LOCAL FUNCTION DEFINITIONS ===================================== */
/* ================================================ MODULE API ============================================= */
DRESULT SD_disk_status()
{
    return RES_OK;
}

DRESULT SD_disk_initialize()
{
    if(SD_Init(dev)==SD_OK)
    {
        return RES_OK;
        printf("Sd init passed!\n");
    }
    printf("Sd init failed!\n");
    return RES_ERROR;
}

DRESULT SD_disk_read(BYTE* buff, LBA_t sector, UINT count)
{
    SDRESULTS res = SD_ERROR;
	res = SD_Read(dev, buff, sector, 0, count * FF_MIN_SS);
    printf("%s\n",buff);
    if ( res != SD_OK )
    {
        return RES_OK;
    }
    return RES_OK;
}


DRESULT SD_disk_write(const BYTE* buff, LBA_t sector, UINT count)
{
    SDRESULTS res = SD_ERROR;
    for (UINT i=0; i < count; i++)
    {
        res = SD_Write(dev, (uint8_t*)buff, i + sector);
        if(SD_OK != res)
        {
            return RES_ERROR;
        }
    }
    return RES_OK;
}

DRESULT SD_disk_ioctl(const BYTE* cmd, const BYTE* buff)
{
    // For example, support some common IOCTL commands:
    switch (*cmd) {
        case CTRL_SYNC:
            // SD disk is always synced
            return RES_OK;
        case GET_SECTOR_COUNT:
            *(DWORD*)buff = FF_SECTOR_COUNT;
            return RES_OK;
        case GET_BLOCK_SIZE:
            *(DWORD*)buff = 1; // erase block size in sectors
            return RES_OK;
        default:
            return RES_PARERR;
    }
}

