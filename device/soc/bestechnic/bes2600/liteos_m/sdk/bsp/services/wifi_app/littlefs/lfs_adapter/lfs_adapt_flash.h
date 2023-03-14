#ifndef __LFS_ADAPT_FLASH_H__
#define __LFS_ADAPT_FLASH_H__
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef LFS_ON_FLASH
    int lfs_flash_open(void);
    int lfs_flash_close(void);
    int lfs_flash_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);
    int lfs_flash_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size);
    int lfs_flash_erase(const struct lfs_config *c, lfs_block_t block);
    int lfs_flash_sync(const struct lfs_config *c);
    lfs_size_t lfs_flash_get_read_size(void);
    lfs_size_t lfs_flash_get_prog_size(void);
    lfs_size_t lfs_flash_get_block_size(void);
    lfs_size_t lfs_flash_get_block_count(void);
    int lfs_flash_get_block_cycles(void);
    lfs_size_t lfs_flash_get_cache_size(void);
    lfs_size_t lfs_flash_get_lookahead_size(void);

    int lfs_flash_test(void);
#endif /* LFS_ON_FLASH */

#ifdef __cplusplus
}
#endif

#endif /* __LFS_ADAPT_FLASH_H__ */