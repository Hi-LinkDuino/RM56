#ifndef __LFS_FILE_OPS_H__
#define __LFS_FILE_OPS_H__
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "lfs.h"

void lfs_file_ops_first_call(void);
void *lfs_file_ops_create_file(const char *name, uint32_t size);
void *lfs_file_ops_open_file(const char *name, uint32_t size);
int lfs_file_ops_write_file(void *fh, const uint8_t *buf, uint32_t len);
int lfs_file_ops_read_file(void *fh, uint8_t *buf, uint32_t len);
int lfs_file_ops_close_file(void *fh);
int lfs_file_ops_get_file_size(const char *name, uint32_t *p_size);

lfs_dir_t *lfs_file_ops_opendir(const char *name);
int lfs_file_ops_readdir (lfs_dir_t *dir, struct lfs_info *info);
int lfs_file_ops_closedir (lfs_dir_t *dir);
int lfs_file_ops_seekdir (lfs_dir_t *dir, long loc);
void lfs_file_ops_rewinddir (lfs_dir_t *dir);

#ifdef __cplusplus
}
#endif

#endif /* __LFS_FILE_OPS_H__ */