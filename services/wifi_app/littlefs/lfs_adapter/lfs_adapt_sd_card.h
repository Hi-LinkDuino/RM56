#ifndef __LFS_ADAPT_SD_CARD_H__
#define __LFS_ADAPT_SD_CARD_H__
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef LFS_ON_SD_CARD
    int lfs_sd_card_open(void);
    int lfs_sd_card_close(void);
    int lfs_sd_card_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);
    int lfs_sd_card_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size);
    int lfs_sd_card_erase(const struct lfs_config *c, lfs_block_t block);
    int lfs_sd_card_sync(const struct lfs_config *c);
    lfs_size_t lfs_sd_card_get_read_size(void);
    lfs_size_t lfs_sd_card_get_prog_size(void);
    lfs_size_t lfs_sd_card_get_block_size(void);
    lfs_size_t lfs_sd_card_get_block_count(void);
    int lfs_sd_card_get_block_cycles(void);
    lfs_size_t lfs_sd_card_get_cache_size(void);
    lfs_size_t lfs_sd_card_get_lookahead_size(void);
#endif /* LFS_ON_SD_CARD */

#ifdef __cplusplus
}
#endif

#endif /* __LFS_ADAPT_SD_CARD_H__ */