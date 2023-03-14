#ifndef __LFS_ADAPT_RAM_H__
#define __LFS_ADAPT_RAM_H__
#ifdef __cplusplus
extern "C"
{
#endif

#define LFS_RAM_DEBUG 0
#define LFS_RAM_TOTAL_SIZE (0x200000) //(0x1000 * 2)	// TODO
#define LFS_RAM_PAGE_SIZE_IN_BYTES (256)
#define LFS_RAM_SECTOR_SIZE_IN_BYTES (4096)


int lfs_ram_open(void);
int lfs_ram_close(void);
int lfs_ram_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);
int lfs_ram_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size);
int lfs_ram_erase(const struct lfs_config *c, lfs_block_t block);
int lfs_ram_sync(const struct lfs_config *c);
lfs_size_t lfs_ram_get_read_size(void);
lfs_size_t lfs_ram_get_prog_size(void);
lfs_size_t lfs_ram_get_block_size(void);
lfs_size_t lfs_ram_get_block_count(void);
int lfs_ram_get_block_cycles(void);
lfs_size_t lfs_ram_get_cache_size(void);
lfs_size_t lfs_ram_get_lookahead_size(void);

int lfs_ram_test(void);


#ifdef __cplusplus
}
#endif

#endif /* __LFS_ADAPT_RAM_H__ */