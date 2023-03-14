/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#ifndef __LITEOS_M__
#include "string.h"
#include "disk.h"
#else
#include "ff_gen_drv.h"
#if defined ( __GNUC__ )
#ifndef __weak
#define __weak __attribute__((weak))
#endif
#endif
#endif

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

#ifndef __LITEOS_M__
#define CARD_UNPLUGED   1
extern int get_cardstatus(int pdrv);
#define GET_CARD_STATUS					\
	do {						\
		if (get_cardstatus(pdrv) == 0)		\
			return STA_NOINIT;		\
	} while (0)
extern  struct tm tm;
#endif

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
#ifndef __LITEOS_M__
	return 0;
#else
	DSTATUS stat;

	stat = g_diskDrv.drv[pdrv]->disk_status(g_diskDrv.lun[pdrv]);
	return stat;
#endif
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
#ifndef __LITEOS_M__
	return 0;
#else
	DSTATUS stat = RES_OK;

	if(g_diskDrv.initialized[pdrv] == 0)
	{
		stat = g_diskDrv.drv[pdrv]->disk_initialize(g_diskDrv.lun[pdrv]);
	}
	return stat;
#endif
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
#ifndef __LITEOS_M__
	int result;

	result = los_part_read((int)pdrv, (void *)buff, sector, (UINT32)count, TRUE);

	if (result == 0)
		return RES_OK;
	else
		return RES_ERROR;
#else
	DRESULT res;

	res = g_diskDrv.drv[pdrv]->disk_read(g_diskDrv.lun[pdrv], buff, sector, count);
	return res;
#endif
}

#ifndef __LITEOS_M__
DRESULT disk_read_readdir (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,		/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	int result;

	result = los_part_read((int)pdrv, (void *)buff, sector, (UINT32)count, FALSE);

	if (result == 0)
		return RES_OK;
	else
		return RES_ERROR;
}
#endif


#ifndef __LITEOS_M__
DRESULT disk_raw_read (int id, void *buff, LBA_t sector, UINT32 count)
{
	int result;

	result = los_disk_read(id, buff, sector, count, TRUE);

	if (result == 0)
		return RES_OK;
	else
		return RES_ERROR;
}
#endif



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
#ifndef __LITEOS_M__
	int result;

	result = los_part_write((int)pdrv, (void *)buff, sector, (UINT32)count);

	if (result == 0)
		return RES_OK;
	else
		return RES_ERROR;
#else
	DRESULT res;

	res = g_diskDrv.drv[pdrv]->disk_write(g_diskDrv.lun[pdrv], buff, sector, count);

	return res;
#endif
}

#ifndef __LITEOS_M__
DRESULT disk_raw_write(int id, void *buff, QWORD sector, UINT32 count){
	int result;
	void *uwBuff = buff;

	result = los_disk_write(id, (const void*)uwBuff, sector, count);

	if (result == 0)
		return RES_OK;
	else
		return RES_ERROR;
}
#endif

#endif /* FF_FS_READONLY == 0 */


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
#ifndef __LITEOS_M__
	int result;

	result = los_part_ioctl((int)pdrv, (int)cmd, buff);

	if (result == 0)
		return RES_OK;
	else
		return RES_ERROR;
#else
	DRESULT res;

	res = g_diskDrv.drv[pdrv]->disk_ioctl(g_diskDrv.lun[pdrv], cmd, buff);
	return res;
#endif
}

 /*
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */

#ifndef __LITEOS_M__
DWORD get_fattime (void)
{
	time_t seconds = 0;
	struct tm local_time = {0};

	seconds = time(NULL);

	if (localtime_r(&seconds, &local_time) == NULL)
		return 0;
	if ((local_time.tm_year + 1900) < 1980) {	/* year must start at 1980 */
		return 0;
	}

	/* get system time */
	return  ((DWORD)(local_time.tm_year - 80) << 25) |
			((DWORD)(local_time.tm_mon + 1) << 21) |
			((DWORD)local_time.tm_mday << 16) |
			((DWORD)local_time.tm_hour << 11) |
			((DWORD)local_time.tm_min << 5) |
			((DWORD)local_time.tm_sec >> 1);
}
#else
__weak DWORD get_fattime (void)
{
	return 0;
}
#endif

