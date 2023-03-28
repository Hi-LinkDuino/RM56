#ifndef HAVE_DIRENT_OP
#define _REENT_ONLY
#define __FILE_defined
#define __CUSTOM_FILE_IO__

#include "lfs.h"
typedef lfs_file_t __FILE;
typedef __FILE FILE;

#include "lfs_util.h"
#include "flash.h"

#include <sys/dirent.h>
#include "cmsis_os.h"
#include "sys/stat.h"

lfs_size_t lfs_flash_get_read_size(void);
lfs_size_t lfs_flash_get_prog_size(void);
lfs_size_t lfs_flash_get_block_size(void);
lfs_size_t lfs_flash_get_block_count(void);

typedef struct {
    struct lfs_config *config;
    lfs_t             *lfs;
    void              *lock;
} dir_manager_t;

osMutexDef(lfs_lock);
static dir_manager_t g_dir_manager;
static const char *lfs_mount_path = "/"; //"/data/"
static const char *lfs_prefix = "./";

static lfs_t native_lfs;
static char native_lfs_read_buffer[256];
static char native_lfs_prog_buffer[256];
static char native_lfs_lookahead_buffer[16];

static int32_t littlefs_block_read(const struct lfs_config *c, lfs_block_t block,
                             lfs_off_t off, void *dst, lfs_size_t size)
{
    uint32_t addr = c->block_size * block + off;
    return hal_flash_read((hal_partition_t)HAL_PARTITION_LITTLEFS, &addr, dst, size);
}

static int32_t littlefs_block_write(const struct lfs_config *c, lfs_block_t block,
                              lfs_off_t off, const void *dst, lfs_size_t size)
{
    uint32_t addr = c->block_size * block + off;
    return hal_flash_write((hal_partition_t)HAL_PARTITION_LITTLEFS, &addr, dst, size);
}

static int32_t littlefs_block_erase(const struct lfs_config *c, lfs_block_t block)
{
    uint32_t addr = c->block_size * block;
    return hal_flash_erase((hal_partition_t)HAL_PARTITION_LITTLEFS, addr, c->block_size);
}

static int32_t littlefs_block_sync(const struct lfs_config *c)
{
    return 0;
}


static void lfs_lock(void)
{
    osMutexWait(g_dir_manager.lock, osWaitForever);
}

static void lfs_unlock(void)
{
    osMutexRelease(g_dir_manager.lock);
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


/* Relative path convert */
static char *path_convert(const char *path)
{
    int32_t len, prefix;
    char *target_path, *p;

    if (path == NULL || strcmp("/", lfs_mount_path) == 0 || *lfs_mount_path == 0) {
        return (char*)path;
    }

    len = strlen(path);
    prefix = strlen(lfs_mount_path);
    int prefix2 = strlen(lfs_prefix);

    char *point_root_path = NULL;
    if (strncmp(lfs_prefix, path, prefix2) == 0) {
        point_root_path = (char *)malloc(prefix+len-prefix2+1);
        memcpy(point_root_path,lfs_mount_path,prefix);
        memcpy(point_root_path+prefix,path+prefix2,len-prefix2);
        point_root_path[prefix+len-prefix2] = '\0';
        path = point_root_path;
    }
    len = strlen(path);
    if (strncmp(lfs_mount_path, path, prefix) == 0) {
        len = len - prefix;
        target_path =(char *)malloc(len + 1);
    } else {
        target_path = NULL;
    }
    if (target_path == NULL) {
        return NULL;
    }
    memset(target_path, 0, len + 1);
    if (len > 0) {
        p = (char *)(path + prefix);
        memcpy(target_path, p, len);
    }

    target_path[len] = '\0';
    free(point_root_path);
    return target_path;
}

DIR *opendir(const char *name)
{
    if (name == NULL) {
        return NULL;
    }
    lfs_dir_t *dir = (lfs_dir_t *)malloc(sizeof(lfs_dir_t));
    char *real_path = path_convert(name);
    lfs_lock();
    int err = lfs_dir_open(g_dir_manager.lfs, dir, real_path);
    lfs_unlock();
    if(real_path != name)free(real_path);
    if (err < 0) {
        free(dir);
        return NULL;
    }
    return (void *)dir;
}


/*
 * get next entry in a directory.
 */
struct dirent *readdir (DIR *dirp)
{
    if (dirp == NULL) {
        return NULL;
    }
    lfs_dir_t *dir = (lfs_dir_t *)dirp;
    struct lfs_info info;

    lfs_lock();
    int err = lfs_dir_read(g_dir_manager.lfs, dir, &info);
    lfs_unlock();

    if (err < 0) {
        printf("%s failed\n",__FUNCTION__);
        return NULL;
    }
    if (err == 0) {
        // ("%s EOF\n",__FUNCTION__);
        return NULL;
    }

    struct dirent *direntp = (struct dirent *)malloc(sizeof(struct dirent));
    if (direntp == NULL) {
        return NULL;
    }
    direntp->d_ino = 0;
    direntp->d_type = (info.type==LFS_TYPE_REG?DT_REG:info.type==LFS_TYPE_DIR?DT_DIR:DT_UNKNOWN);
    memset(direntp->d_name,0,sizeof(direntp->d_name));
    strncpy(direntp->d_name, info.name, sizeof(info.name));
    direntp->d_size = info.size;
    return direntp;
}


/*
 * close a directory.
 */
int closedir (DIR *dirp)
{
    if (dirp == NULL)
        return -1;

    lfs_dir_t *dir = (lfs_dir_t *)dirp;
    lfs_lock();
    int rc = lfs_dir_close(g_dir_manager.lfs, dir);
    lfs_unlock();
    free((void *)dir);
    dir = NULL;
    return rc;
}

/*
 * Seek to an entry in a directory.
 */
void seekdir (DIR *dirp, long loc)
{
    if (dirp == NULL) {
        return;
    }
    lfs_dir_t *dir = (lfs_dir_t *)dirp;
    lfs_lock();
    lfs_dir_seek(g_dir_manager.lfs, dir, (lfs_off_t)loc);
    lfs_unlock();
}


void rewinddir (DIR *dirp)
{
    if (dirp == NULL) {
        return;
    }
    lfs_dir_t *dir = (lfs_dir_t *)dirp;
    lfs_lock();
    lfs_dir_rewind(g_dir_manager.lfs, dir);
    lfs_unlock();
}


uint32_t telldir (DIR *dirp)
{
    if (dirp == NULL) {
        return 0;
    }
    lfs_dir_t *dir = (lfs_dir_t *)dirp;
    lfs_lock();
    uint32_t ret = lfs_dir_tell(g_dir_manager.lfs, dir);
    lfs_unlock();

    return ret;
}

int mount_flash(void)
{
    g_dir_manager.config = &default_cfg;
    g_dir_manager.config->read_size = lfs_flash_get_read_size(),
    g_dir_manager.config->prog_size = lfs_flash_get_prog_size(),
    g_dir_manager.config->block_size = lfs_flash_get_block_size(),
    g_dir_manager.config->block_count = lfs_flash_get_block_count(),
    /* Create LFS Global Lock */
    g_dir_manager.lock = osMutexCreate(osMutex(lfs_lock));
    if (g_dir_manager.lock == NULL) {
        goto ERROR;
    }

    /* Create LFS struct */
    g_dir_manager.lfs = &native_lfs;
    if (g_dir_manager.lfs == NULL) {
        goto ERROR;
    }

    g_dir_manager.config->read_buffer = native_lfs_read_buffer;
    g_dir_manager.config->prog_buffer = native_lfs_prog_buffer;
    g_dir_manager.config->lookahead_buffer = native_lfs_lookahead_buffer;

    lfs_lock();
    int err = lfs_mount(g_dir_manager.lfs, g_dir_manager.config);
    lfs_unlock();
    if (err < 0) {
        printf("%s mount failed\n",__FUNCTION__);
    }
    printf("%s mount success\n",__FUNCTION__);
    return err;
ERROR:
    osMutexDelete(g_dir_manager.lock);
    return -1;
}

int unmount_flash(void)
{
    int err = lfs_unmount(g_dir_manager.lfs);
    osMutexDelete(g_dir_manager.lock);
    return err;
}



int mode_flags (const char *mode)
{
    int ret;

    switch (mode[0])
    {
        case 'r':           /* open for reading */
            ret = LFS_O_RDONLY;
            break;

        case 'w':           /* open for writing */
            ret = LFS_O_WRONLY | LFS_O_TRUNC;
            break;

        case 'a':           /* open for appending */
            ret = LFS_O_CREAT | LFS_O_APPEND;
            break;

        default:            /* illegal mode */
            return (0);
    }
    while (*++mode)
    {
        switch (*mode)
        {
            case '+':
                ret |= LFS_O_RDWR | LFS_O_CREAT ;
                break;

            case 'b':
                // ret |= O_BINARY;
                break;

            case 't':
                // ret |= O_BO_TEXTINARY;
                break;

            case 'x':              /* test file is exist ? */
                ret |= LFS_O_EXCL;
                break;
            default:
                break;
        }
    }
    return ret;
}

FILE *fopen (const char *file,const char *mode)
{
    if (!strlen(file)) {
        return NULL;
    }

    FILE *fp = (FILE *)malloc(sizeof(FILE)+4+sizeof(int));
    char *real_path = path_convert(file);
    lfs_lock();
    int err = lfs_file_open(g_dir_manager.lfs, fp,  real_path, mode_flags(mode));
    lfs_unlock();
    if(real_path != file)free(real_path);
    // TRACE(0, "fopen: %s--->%s: %d", file, real_path, err);
    if (err < 0) {
        free(fp);
        fp = NULL;
    }

    return (FILE *)fp;
}


int fclose (FILE * fp)
{
    if (fp == NULL) {
        return -1;
    }

    lfs_lock();
    int err = lfs_file_close(g_dir_manager.lfs, fp);
    lfs_unlock();

    if (err < 0)
        return err;

    free(fp);
    return err;
}

int __wrap_fseek (FILE *fp, long offset,int whence)
{
    if (fp == NULL) {
        return -1;
    }
    lfs_lock();
    int err = lfs_file_seek(g_dir_manager.lfs, fp, offset, whence);
    lfs_unlock();
    // TRACE(0, "%s: offset=%ld, whence=%d, returns %d", __func__, offset, whence, err);
    if (err != LFS_ERR_OK)
        return err;

    return err;
}
#define ALIGN_DIFF(x,y)          (x%y)
#define ALIGN_BLOCK(x,y)         x-ALIGN_DIFF(x,y)+y
size_t fread (void * buf,size_t size,size_t count,FILE * fp)
{
    if (fp == NULL) {
        return -1;
    }
    lfs_lock();
    size_t err = lfs_file_read(g_dir_manager.lfs, fp, buf, size*count);
    lfs_unlock();
    if (err < 0)
        return err;
    return err;
}

size_t fwrite (const void * buf,size_t size,size_t count,FILE * fp)
{
    if (fp == NULL) {
        return -1;
    }
    lfs_lock();
    size_t err = lfs_file_write(g_dir_manager.lfs, fp, buf, size*count);
    lfs_unlock();
    if (err != LFS_ERR_OK)
        return err;

    return err;
}

long ftell (FILE * fp)
{
    if (fp == NULL) {
        return -1;
    }
    return fp->pos;
}

int stat(const char *path, struct stat *st)
{
    struct lfs_info s;
    char *real_path = path_convert(path);
    lfs_lock();
    int err = lfs_stat(g_dir_manager.lfs, real_path, &s);
    lfs_unlock();
    if(real_path != path)free(real_path);
    if (err == 0) {
        st->st_size = s.size;
        st->st_mode = S_IRWXU | S_IRWXG | S_IRWXO |
                      ((s.type == LFS_TYPE_DIR ? S_IFDIR : S_IFREG));
    }

    return err;
}


// int fstat(const char *path, struct stat *st)
// {
//     return stat(path,st);
// }

int access(const char *path)
{
    return stat(path,NULL);
}

int remove(const char *path)
{
    char *real_path = path_convert(path);
    lfs_lock();
    int ret = lfs_remove(g_dir_manager.lfs, real_path);
    lfs_unlock();
    if(real_path != path)free(real_path);
    return ret;
}

int rename(const char *oldpath, const char *newpath)
{
    char *oldname = NULL;
    char *newname = NULL;

    oldname = (char*)oldpath;
    if (!oldname) {
        return -1;
    }

    newname = (char*)newpath;
    if (!newname) {
        return -1;
    }
    char *real_path_old = path_convert(oldname);
    char *real_path_new = path_convert(newname);
    lfs_lock();
    int ret = lfs_rename(g_dir_manager.lfs, real_path_old, real_path_new);
    lfs_unlock();
    if(real_path_old != oldname)free(real_path_old);
    if(real_path_new != newname)free(real_path_new);
    return ret;
}

int mkdir(const char *path,mode_t mode)
{
    if (path == NULL)
        return -1;
    char *real_path = path_convert(path);
    lfs_lock();
    int ret = lfs_mkdir(g_dir_manager.lfs,real_path);
    lfs_unlock();
    if(real_path != path)free(real_path);
    return ret;
}

int lfs_vfs_rmdir (const char *path)
{
    int32_t ret = -1;
    return ret;
}

int feof(FILE *stream)
{
    // TRACE_IMM(0,"stream->pos:%d  lfs_file_size:%d\n",stream->pos,lfs_file_size(g_dir_manager.lfs, stream));
    return stream->pos >= lfs_file_size(g_dir_manager.lfs, stream);
}

int __wrap_fgetc (FILE * fp)
{
    if (fp == NULL)      /* sanity check */
        return EOF;
    unsigned char buf[1] = {0};
    size_t err = lfs_file_read(g_dir_manager.lfs, fp, buf, 1);
    // TRACE(0, "%s: get %x, returns %d", __func__, buf[0], err);
    if (err <= 0)
        return EOF;
    int ch = (int)buf[0];
    return ch;
}

char *  fgets (char *buf, int n, FILE * fp)
{
    if (fp == NULL)      /* sanity check */
        return NULL;
    if (feof(fp))      /* sanity check */
        return NULL;
    int c = EOF;
    char *cs = buf;
    while( (--n > 0) && ((c = fgetc(fp)) != EOF) )
    {
        *cs++ = c;
        if ((c == 10) || (c == 13))
            break;
    }
    *cs ='\0';

    return (c == EOF && cs == buf) ? NULL : buf;
}

int	__wrap_fflush (FILE *fp)
{
    return 1;
}

#endif /* ! HAVE_DIRENT_OP */
