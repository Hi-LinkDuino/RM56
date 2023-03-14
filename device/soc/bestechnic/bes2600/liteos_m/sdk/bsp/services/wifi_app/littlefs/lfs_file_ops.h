#ifndef __LFS_FILE_OPS_H__
#define __LFS_FILE_OPS_H__
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

void lfs_file_ops_first_call(void);
void *lfs_file_ops_create_file(const char *name, uint32_t size);
void *lfs_file_ops_open_file(const char *name, uint32_t size);
int lfs_file_ops_write_file(void *fh, const uint8_t *buf, uint32_t len);
int lfs_file_ops_read_file(void *fh, uint8_t *buf, uint32_t len);
int lfs_file_ops_close_file(void *fh);
int lfs_file_ops_get_file_size(const char *name, uint32_t *p_size);

#ifdef __cplusplus
}
#endif

#endif /* __LFS_FILE_OPS_H__ */