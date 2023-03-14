/***************************************************************************
 *
 * Copyright 2015-2022 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __BES_FILE_H__
#define __BES_FILE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef NULL
#define NULL 0L
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

typedef int	BEST_RESULT;
typedef char BEST_CHAR;
typedef uint32_t BEST_FSIZE;

typedef struct {
    BEST_FSIZE	fsize;			/* File size */
    uint16_t	fdate;			/* Modified date */
    uint16_t	ftime;			/* Modified time */
    uint8_t		fattrib;		/* File attribute */
    BEST_CHAR	altname[12 + 1];/* Altenative file name */
    BEST_CHAR	fname[255 + 1];	/* Primary file name */
    uint8_t		isatty;			/* is a tty */
} BEST_FILINFO;

typedef struct {
    BEST_RESULT (*mount)(const BEST_CHAR *, uint8_t);
    BEST_RESULT (*unmount)(const BEST_CHAR * path);
    BEST_RESULT (*format)(const BEST_CHAR * path);
    int (*fopen)(const BEST_CHAR * path, const char * mode);
    BEST_RESULT (*fclose)(int handle);
    BEST_RESULT (*fstat)(const BEST_CHAR * path, BEST_FILINFO * fno);

    int (*eof)(int handle);

    BEST_CHAR * (*fgets)(BEST_CHAR * buffer, int size, int handle);
    unsigned int (*fread)( BEST_CHAR *buffer, unsigned int  size, unsigned int  count, int handle);

    //write related
    unsigned int (*fwrite)(const void* buffer, unsigned int  size, unsigned int  count, int handle);
    BEST_RESULT (*rename)(const BEST_CHAR * old, const BEST_CHAR * new_name);
    BEST_RESULT (*remove)(const BEST_CHAR * path);
    BEST_RESULT (*fsync)(int handle);

    //find related
    void 	(*rewind)(int handle);
    long 	(*ftell)(int handle);
    int 	(*fseek)(int handle, BEST_FSIZE offset, int origin);
    int 	(*ferror)(int handle);

    //dir related
    BEST_RESULT 	(*mkdir)(const BEST_CHAR *pathname);
    int 			(*access)(const BEST_CHAR *pathname, int mode);
    int 			(*opendir)(const BEST_CHAR *name);
    BEST_RESULT 	(*closedir)(int handle);
    BEST_RESULT 	(*rewinddir)(int handle);
    BEST_FILINFO *	(*readdir)(int handle);
    BEST_FILINFO * 	(*firstdir)(int handle, const BEST_CHAR * path, const BEST_CHAR * pattern);
    BEST_FILINFO * 	(*nextdir)(int handle);
    BEST_RESULT 	(*closefileinfo)(BEST_FILINFO * fno);

    uint32_t (*fsize)(int handle);
    uint8_t (*isdir)(BEST_FILINFO * fno);
    uint8_t (*isfile)(BEST_FILINFO * fno);
    uint8_t (*ishidden)(BEST_FILINFO * fno);
    uint8_t (*isreadonly)(BEST_FILINFO * fno);
    uint8_t (*issystem)(BEST_FILINFO * fno);

} BEST_FATFS;

BEST_FATFS * getBestFs();

#ifdef __cplusplus
}
#endif

#endif
