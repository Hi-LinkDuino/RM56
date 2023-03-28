
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "lfs.h"
#include "lfs_util.h"

#include "flash.h"
#include "cmsis_os.h"

osMutexDef(lfs_lock);
typedef void lfs_lock_t;

typedef struct {
    struct lfs_config *config;
    lfs_t             *lfs;
    lfs_lock_t        *lock;
} lfs_manager_t;

static const char *lfs_mount_path = "/data";
static lfs_manager_t g_lfs_manager;

static int32_t littlefs_block_read(const struct lfs_config *c, lfs_block_t block,
                             lfs_off_t off, void *dst, lfs_size_t size)
{
    uint32_t addr;

    addr = c->block_size * block + off;

    return hal_flash_read((hal_partition_t)HAL_PARTITION_LITTLEFS, &addr, dst, size);
}

static int32_t littlefs_block_write(const struct lfs_config *c, lfs_block_t block,
                              lfs_off_t off, const void *dst, lfs_size_t size)
{
    uint32_t addr;

    addr = c->block_size * block + off;

    return hal_flash_write((hal_partition_t)HAL_PARTITION_LITTLEFS, &addr, dst, size);
}

static int32_t littlefs_block_erase(const struct lfs_config *c, lfs_block_t block)
{
    uint32_t addr;

    addr = c->block_size * block;

    return hal_flash_erase((hal_partition_t)HAL_PARTITION_LITTLEFS, addr, c->block_size);
}

static int32_t littlefs_block_sync(const struct lfs_config *c)
{
    return 0;
}


/* Relative path convert */
static char *path_convert(const char *path)
{
    int32_t len, prefix;
    char *target_path, *p;

    if (path == NULL) {
        return NULL;
    }

    len = strlen(path);
    prefix = strlen(lfs_mount_path);
    if (strncmp(lfs_mount_path, path, prefix) != 0) {
        return NULL;
    }

    len = len - prefix;
    target_path =(char *)malloc(len + 1);
    if (target_path == NULL) {
        return NULL;
    }

    memset(target_path, 0, len + 1);
    if (len > 0) {
        p = (char *)(path + prefix + 1);
        memcpy(target_path, p, len - 1);
    }

    target_path[len] = '\0';
    return target_path;
}

static struct lfs_config default_cfg = {
    // block device operations
    .read  = littlefs_block_read,
    .prog  = littlefs_block_write,
    .erase = littlefs_block_erase,
    .sync  = littlefs_block_sync,

    // block device configuration
    .read_size = 256,
    .prog_size = 256,
    .block_size = 4096,
    .block_count = 888,
    .cache_size = 256,
    .lookahead_size = 16,
    .block_cycles = 1000,
};

static lfs_t native_lfs;

static char native_lfs_read_buffer[256];
static char native_lfs_prog_buffer[256];
static char native_lfs_lookahead_buffer[16];

static void lfs_lock(void)
{
    osMutexWait(g_lfs_manager.lock, osWaitForever);
}

static void lfs_unlock(void)
{
    osMutexRelease(g_lfs_manager.lock);
}

int lfs_vfs_mount(void)
{
    int ret;

    g_lfs_manager.config = &default_cfg;

    /* Create LFS Global Lock */
    g_lfs_manager.lock = osMutexCreate(osMutex(lfs_lock));
    if (g_lfs_manager.lock == NULL) {
        goto ERROR;
    }

    /* Create LFS struct */
    g_lfs_manager.lfs = &native_lfs;
    if (g_lfs_manager.lfs == NULL) {
        goto ERROR;
    }

    g_lfs_manager.config->read_buffer = native_lfs_read_buffer;
    g_lfs_manager.config->prog_buffer = native_lfs_prog_buffer;
    g_lfs_manager.config->lookahead_buffer = native_lfs_lookahead_buffer;


    lfs_lock();
    ret = lfs_mount(g_lfs_manager.lfs, g_lfs_manager.config);
    if (ret < 0) {
        printf("mount failed");
    }
    lfs_unlock();

    return ret;

ERROR:
    osMutexDelete(g_lfs_manager.lock);
    return -1;
}


int lfs_vfs_unmount(void)
{
    int ret = lfs_unmount(g_lfs_manager.lfs);
    osMutexDelete(g_lfs_manager.lock);
    return ret;
}

typedef struct {
    lfs_file_t file;
    lfs_dir_t dir;
} file_point_t;

void *lfs_vfs_open(const char *path, int flags)
{
    int ret;
    if (!strlen(path)) {
        return NULL;
    }

    file_point_t *fp = (file_point_t *)malloc(sizeof(file_point_t));

    lfs_lock();
    ret = lfs_file_open(g_lfs_manager.lfs, &(fp->file),  path_convert(path), flags);
    lfs_unlock();

    if (ret < 0) {
        free(fp);
        fp = NULL;
    }

    return (void *)fp;
}

int32_t lfs_vfs_close(void *fp)
{
    int ret;

    if (fp == NULL) {
        return -1;
    }
    file_point_t *file_p = (file_point_t *)(fp);

    lfs_lock();
    ret = lfs_file_close(g_lfs_manager.lfs, &(file_p->file));
    lfs_unlock();

    if (ret < 0) {
        return ret;
    }

    free(fp);
    return ret;
}

int32_t lfs_vfs_read(void *fp, char *buf, uint32_t len)
{
    int ret;

    file_point_t *file_p = (file_point_t *)(fp);

    lfs_lock();
    ret = lfs_file_read(g_lfs_manager.lfs, &(file_p->file), buf, len);
    lfs_unlock();

    return ret;
}

static int32_t lfs_vfs_write(void *fp, const char *buf, uint32_t len)
{
    int ret;

    file_point_t *file_p = (file_point_t *)(fp);

    lfs_lock();
    ret = lfs_file_write(g_lfs_manager.lfs, &(file_p->file), buf, len);
    lfs_unlock();

    return ret;
}

static uint32_t lfs_vfs_lseek(void *fp, int64_t off, int32_t whence)
{
    int32_t ret;

    file_point_t *file_p = (file_point_t *)(fp);

    lfs_lock();
    ret = lfs_file_seek(g_lfs_manager.lfs, &(file_p->file), off, whence);
    lfs_unlock();

    return ret;
}

static int32_t lfs_vfs_sync(void *fp)
{
    int ret;

    file_point_t *file_p = (file_point_t *)(fp);

    lfs_lock();
    ret = lfs_file_sync(g_lfs_manager.lfs, &(file_p->file));
    lfs_unlock();

    return ret;
}

static int32_t lfs_vfs_fstat(void *fp, struct lfs_info *fst)
{
    int32_t ret;
    file_point_t *file_p = (file_point_t *)(fp);

    lfs_lock();
    // ret = lfs_fstat(g_lfs_manager.lfs, &(file_p->file), fst);
    lfs_unlock();

    return ret;
}

static int32_t lfs_vfs_stat(const char *path,struct lfs_info *st)
{
    int ret;
    char *target_path = path_convert(path);
    if (target_path == NULL) {
        return -1;
    }

    lfs_lock();
    ret = lfs_stat(g_lfs_manager.lfs, target_path, st);
    lfs_unlock();

    return ret;
}

static int32_t lfs_vfs_access(const char *path, struct lfs_info *st)
{
    int ret = lfs_vfs_stat(path_convert(path), st);
    return ret;
}

static int32_t lfs_vfs_remove(const char *path)
{
    int ret;
    char *target_path = path_convert(path);
    if (target_path == NULL) {
        return -1;
    }

    lfs_lock();
    ret = lfs_remove(g_lfs_manager.lfs, target_path);
    lfs_unlock();

    return ret;
}

static int32_t lfs_vfs_rename(const char *oldpath, const char *newpath)
{
    int32_t ret;

    char *oldname = NULL;
    char *newname = NULL;

    oldname = oldpath;
    if (!oldname) {
        return -1;
    }

    newname = newpath;
    if (!newname) {
        return -1;
    }

    lfs_lock();
    ret = lfs_rename(g_lfs_manager.lfs, path_convert(oldname), path_convert(newname));
    lfs_unlock();

    return ret;
}

void *lfs_vfs_opendir(const char *path)
{
    int32_t ret;

    if (path == NULL) {
        return NULL;
    }
    file_point_t *file_p = (file_point_t *)malloc(sizeof(file_point_t));

    if (file_p == NULL) {
        return NULL;
    }

    memset(&(file_p->dir), 0, sizeof(lfs_dir_t));

    lfs_lock();
    ret = lfs_dir_open(g_lfs_manager.lfs, &(file_p->dir), path_convert(path));
    lfs_unlock();

    if (ret < 0) {
        free(file_p);
        file_p = NULL;
    }

    return (void *)file_p;
}

int32_t lfs_vfs_readdir(void *fp,struct lfs_info *info)
{
    int32_t ret;
    if (fp == NULL) {
        return -1;
    }
    file_point_t *file_p = (file_point_t *)fp;

    lfs_lock();
    ret = lfs_dir_read(g_lfs_manager.lfs, &(file_p->dir), info);
    lfs_unlock();

    return ret;
}

int32_t lfs_vfs_closedir(void *fp)
{
    int32_t ret;

    if (fp == NULL) {
        return -1;
    }
    file_point_t *file_p = (file_point_t *)fp;

    lfs_lock();
    ret = lfs_dir_close(g_lfs_manager.lfs, &(file_p->dir));
    lfs_unlock();

    return ret;
}

static int32_t lfs_vfs_mkdir(const char *path)
{
    int32_t ret;
    if (path == NULL) {
        return NULL;
    }

    lfs_lock();
    ret = lfs_mkdir(g_lfs_manager.lfs,path_convert(path));
    lfs_unlock();

    return ret;
}

static int32_t lfs_vfs_rmdir (const char *path)
{
    int32_t ret;
    if (path == NULL) {
        return -1;
    }

    lfs_lock();
    ret = lfs_remove(g_lfs_manager.lfs, path_convert(path));
    lfs_unlock();

    return ret;
}

static void lfs_vfs_rewinddir(void *fp)
{
    if (fp == NULL) {
        return -1;
    }
    file_point_t *file_p = (file_point_t *)fp;

    lfs_lock();
    lfs_dir_rewind(g_lfs_manager.lfs, &(file_p->dir));
    lfs_unlock();
}

static int32_t lfs_vfs_telldir(void *fp)
{
    if (fp == NULL) {
        return -1;
    }
    file_point_t *file_p = (file_point_t *)fp;

    int32_t ret;

    lfs_lock();
    ret = lfs_dir_tell(g_lfs_manager.lfs, &(file_p->dir));
    lfs_unlock();

    return ret;
}

static void lfs_vfs_seekdir(void *fp, int32_t loc)
{
    if (fp == NULL) {
        return -1;
    }
    file_point_t *file_p = (file_point_t *)fp;

    lfs_lock();
    lfs_dir_seek(g_lfs_manager.lfs, &(file_p->dir), (lfs_off_t)loc);
    lfs_unlock();
}
