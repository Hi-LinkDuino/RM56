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

#ifndef __FS_RETARGET_H__
#define __FS_RETARGET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "best_fs.h"

BEST_RESULT fat_mount(const BEST_CHAR * path, uint8_t opt);
BEST_RESULT fat_unmount(const BEST_CHAR * path);
BEST_RESULT fat_format(const BEST_CHAR * path);
int         fat_remove(const char *path);
int         fat_rename(const char *oldname, const char *newname);
int         fat_opendir(const char *path);
int         fat_closedir(int handle);
int         fat_mkdir(const char *path);
BEST_FILINFO *  fat_readdir(int handle);
BEST_RESULT     fat_rewinddir(int handle);
BEST_RESULT     fat_closefileinfo(BEST_FILINFO *fno);// do not forget to call close*** after using the related resources
int         fat_fopen(const char * path, char * mode);
int         fat_fclose(int handle);
int         fat_fread(BEST_CHAR *buffer, unsigned int  size, unsigned int  count, int handle);
BEST_CHAR * fat_fgets(BEST_CHAR * buffer, int size, int handle);
int         fat_fwrite(BEST_CHAR* buffer, unsigned int  size, unsigned int  count, int handle);
int         fat_fseek(int handle, BEST_FSIZE offset, int origin);
void        fat_rewind(int handle);
int         fat_feof(int handle);
uint32_t    fat_fsize(int handle);
uint8_t     fat_isdir(BEST_FILINFO * fno);
uint8_t     fat_isfile(BEST_FILINFO *  fno);
uint8_t     fat_isreadonly(BEST_FILINFO *  fno);
uint8_t     fat_ishidden(BEST_FILINFO *  fno);
uint8_t     fat_issystem(BEST_FILINFO *  fno);
uint8_t     fat_isatty(int handle);

#ifdef __cplusplus
}
#endif

#endif
