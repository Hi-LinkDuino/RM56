
/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hal_trace.h"
#include "cmsis_os.h"
#define EN_ALIOS 0
#include "lfs.h"
#include "lfs_adapt.h"

#if 0
static char *name_to_aos_mount_point(const char *name)
{
    // aos mount point is  " /data "
    return NULL;
}
#endif

static struct lfs_config *my_lfs_cfg = NULL;
static lfs_t my_lfs;
int lfs_file_ops_first_call(void)
{
    int ret = 0;
    lfs_device_open();
    my_lfs_cfg = lfs_get_config();
    ret = lfs_mount(&my_lfs, my_lfs_cfg);
    if (ret < 0)
    {
        TRACE(0, "lfs first mount error = %d, to format...", ret);
        ret = lfs_format(&my_lfs, my_lfs_cfg);
        if (ret < 0)
        {
            TRACE(0, "lfs_format error = %d", ret);
            return ret;
        }
        TRACE(0, "lfs foramt succ.");
        ret = lfs_mount(&my_lfs, my_lfs_cfg);
        if (ret < 0)
        {
            TRACE(0, "lfs mount after format error = %d", ret);
            return ret;
        }
    }
    TRACE(0, "lfs mount succ");
    return 0;
}

void *lfs_file_ops_create_file(const char *name, uint32_t size)
{
    int ret = 0;
    lfs_file_t *my_lfs_file = (lfs_file_t *)malloc(sizeof(lfs_file_t));
    if (!my_lfs_file)
    {
        return NULL;
    }
    memset(my_lfs_file, 0, sizeof(lfs_file_t));
    ret = lfs_file_open(&my_lfs, my_lfs_file, name, LFS_O_CREAT);
    if (ret < 0)
    {
        TRACE(0, "create file error");
        free(my_lfs_file);
        return NULL;
    }
    return my_lfs_file;
}

void *lfs_file_ops_open_file(const char *name, uint32_t size)
{
    int ret = 0;
    lfs_file_t *my_lfs_file = (lfs_file_t *)malloc(sizeof(lfs_file_t));
    if (!my_lfs_file)
    {
        return NULL;
    }
    memset(my_lfs_file, 0, sizeof(lfs_file_t));
    ret = lfs_file_open(&my_lfs, my_lfs_file, name, LFS_O_RDWR);
    if (ret < 0)
    {
        TRACE(0, "open file error");
        free(my_lfs_file);
        return NULL;
    }
    return my_lfs_file;
}

int lfs_file_ops_write_file(void *fh, const uint8_t *buf, uint32_t len)
{
    printf("%s: Write %u bytes: %02X %02X %02X %02X", __func__, len, buf[0], buf[1], buf[2], buf[3]);
    int ret = 0;
    lfs_file_t *my_lfs_file = (lfs_file_t *)fh;
    lfs_file_rewind(&my_lfs, my_lfs_file);
    ret = lfs_file_write(&my_lfs, my_lfs_file, buf, len);
    return ret;
}

int lfs_file_ops_read_file(void *fh, uint8_t *buf, uint32_t len)
{
    int ret = 0;
    lfs_file_t *my_lfs_file = (lfs_file_t *)fh;
    lfs_file_rewind(&my_lfs, my_lfs_file);
    ret = lfs_file_read(&my_lfs, my_lfs_file, buf, len);
    printf("%s: Read %u bytes: %02X %02X %02X %02X", __func__, len, buf[0], buf[1], buf[2], buf[3]);
    return ret;
}

int lfs_file_ops_close_file(void *fh)
{
    int ret = 0;
    lfs_file_t *my_lfs_file = (lfs_file_t *)fh;
    ret = lfs_file_close(&my_lfs, my_lfs_file);
    free(my_lfs_file);
    return ret;
}

int lfs_file_ops_get_file_size(const char *name, uint32_t *p_size)
{
    // TODO
    int ret = 0;
    lfs_file_t *my_lfs_file = lfs_file_ops_open_file(name, 0);
    ret = lfs_file_size(&my_lfs, my_lfs_file);
    if (ret < 0)
    {
        *p_size = 0;
        ret = -1;
    }
    else
    {
        *p_size = ret;
        ret = 0;
    }
    lfs_file_ops_close_file((void *)my_lfs_file);
    return ret;
}

lfs_dir_t *lfs_file_ops_opendir(const char *name)
{
    if (name == NULL) {
        return NULL;
    }
    lfs_dir_t *dir = (lfs_dir_t *)malloc(sizeof(lfs_dir_t));
    // lfs_lock();
    int err = lfs_dir_open(&my_lfs, dir, name);
    // lfs_unlock();
    if (err < 0) {
        free(dir);
        return NULL;
    }
    return dir;
}


/*
 * get next entry in a directory.
 */
int lfs_file_ops_readdir (lfs_dir_t *dir, struct lfs_info *info)
{
    if (dir == NULL) {
        return -1;
    }
    // lfs_lock();
    int err = lfs_dir_read(&my_lfs, dir, info);
    // lfs_unlock();

    if (err < 0) {
        printf("%s failed\n",__FUNCTION__);
        return NULL;
    }
    return err;
}


/*
 * close a directory.
 */
int lfs_file_ops_closedir (lfs_dir_t *dir)
{
    if (dir == NULL)
        return -1;

    // lfs_lock();
    int rc = lfs_dir_close(&my_lfs, dir);
    // lfs_unlock();
    free(dir);
    return rc;
}

/*
 * Seek to an entry in a directory.
 */
int lfs_file_ops_seekdir (lfs_dir_t *dir, long loc)
{
    if (dir == NULL) {
        return -1;
    }
    // lfs_lock();
    int ret = lfs_dir_seek(&my_lfs, dir, (lfs_off_t)loc);
    // lfs_unlock();
    return ret;
}


void lfs_file_ops_rewinddir (lfs_dir_t *dir)
{
    if (dir == NULL) {
        return;
    }
    // lfs_lock();
    lfs_dir_rewind(&my_lfs, dir);
    // lfs_unlock();
}

