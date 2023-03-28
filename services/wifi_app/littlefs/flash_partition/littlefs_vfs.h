#ifndef LITTLEFS_VFS_H
#define LITTLEFS_VFS_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lfs.h"
#include <stdint.h>

int lfs_vfs_mount(void);

int lfs_vfs_unmount(void);

void *lfs_vfs_open(const char *path, int flags);
int32_t lfs_vfs_close(void *fp);
int32_t lfs_vfs_read(void *fp, char *buf, uint32_t len);
int32_t lfs_vfs_readdir(void *fp,struct lfs_info *info);
void *lfs_vfs_opendir(const char *path);
int32_t lfs_vfs_readdir(void *fp,struct lfs_info *info);
int32_t lfs_vfs_closedir(void *fp);


#ifdef __cplusplus
}
#endif

#endif /* HAL_FLASH_H */

