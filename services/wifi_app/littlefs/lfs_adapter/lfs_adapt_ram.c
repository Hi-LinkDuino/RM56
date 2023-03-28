#ifdef LFS_ON_RAM
#include "cmsis_os.h"
#include "cmsis.h"
//#include "net_memory.h"
//#include "pmu.h"
#include "lfs.h"
#include "lfs_adapt.h"
#include "hal_trace.h"
//#include "app_utils.h"
//#include "watchdog/watchdog.h"
#include <string.h>

int lfs_ram_open(void)
{
    return 0;
}

int lfs_ram_close(void)
{
    return 0;
}

int lfs_ram_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    if (c == NULL || c->context == NULL || buffer == NULL) {
        TRACE(0, "%s paramers is NULL\r\n", __func__);
        return -1;
    }
#if LFS_RAM_DEBUG
    TRACE(0, "[lfs] blk_read, block = %d, off = %d, size = %d\n", block, off, size);
#endif
    uint32_t index = block * c->block_size + off;
	memcpy(buffer, (uint8_t *)(c->context) + index, size);
	return 0;
}

int lfs_ram_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    if (c == NULL || c->context == NULL || buffer == NULL) {
        TRACE(0, "%s paramers is NULL\r\n", __func__);
        return -1;
    }

#if LFS_RAM_DEBUG
    TRACE(0, "[lfs] blk_prog, block = %d, off = %d, size = %d\n", block, off, size);
#endif
	uint32_t index = block * c->block_size + off;
	memcpy((uint8_t *)(c->context) + index, buffer, size);
	return 0;
}

int lfs_ram_erase(const struct lfs_config *c, lfs_block_t block)
{
    if (c == NULL || c->context == NULL) {
        TRACE(0, "%s paramers is NULL\r\n", __func__);
        return -1;
    }
#if LFS_RAM_DEBUG
    TRACE(0, "[lfs] blk_erase, block = %d\n", block);
#endif
	uint32_t index = block * c->block_size;
	memset((uint8_t *)(c->context) + index, 0xFF, c->block_size);
	return 0;
}

int lfs_ram_sync(const struct lfs_config *c)
{
    (void)c;
#if LFS_RAM_DEBUG
    TRACE(0, "[lfs] ram_sync\n");
#endif
    return 0;
}

lfs_size_t lfs_ram_get_read_size(void)
{
    // Minimum size of a block read. All read operations will be a
    // multiple of this value.
    return LFS_RAM_PAGE_SIZE_IN_BYTES;
}

lfs_size_t lfs_ram_get_prog_size(void)
{
    // Minimum size of a block program. All program operations will be a
    // multiple of this value.
    return LFS_RAM_PAGE_SIZE_IN_BYTES;
}

lfs_size_t lfs_ram_get_block_size(void)
{
    // Size of an erasable block. This does not impact ram consumption and
    // may be larger than the physical erase size. However, non-inlined files
    // take up at minimum one block. Must be a multiple of the read
    // and program sizes.
    return LFS_RAM_SECTOR_SIZE_IN_BYTES;
}

lfs_size_t lfs_ram_get_block_count(void)
{
    // Number of erasable blocks on the device.
    //uint32_t ram_total_size = hal_norram_get_ram_total_size(HAL_NORFLASH_ID_0);
    //uint32_t useable_ram_size = ram_total_size - LFS_FLASH_BASE_ADDR;
    uint32_t useable_ram_size = LFS_RAM_TOTAL_SIZE;
    uint32_t sector_num = useable_ram_size / LFS_RAM_SECTOR_SIZE_IN_BYTES;
    TRACE(0, "useable_ram_size = %d(0x%0x), useable sector_num = %d\n", useable_ram_size, useable_ram_size, sector_num);
    return sector_num;
}

int lfs_ram_get_block_cycles(void)
{
    // Number of erase cycles before littlefs evicts metadata logs and moves
    // the metadata to another block. Suggested values are in the
    // range 100-1000, with large values having better performance at the cost
    // of less consistent wear distribution.
    //
    // Set to -1 to disable block-level wear-leveling.
    return 100;
}

lfs_size_t lfs_ram_get_cache_size(void)
{
    // Size of block caches. Each cache buffers a portion of a block in RAM.
    // The littlefs needs a read cache, a program cache, and one additional
    // cache per file. Larger caches can improve performance by storing more
    // data and reducing the number of disk accesses. Must be a multiple of
    // the read and program sizes, and a factor of the block size.
    return LFS_RAM_PAGE_SIZE_IN_BYTES;
}

lfs_size_t lfs_ram_get_lookahead_size(void)
{
    // Size of the lookahead buffer in bytes. A larger lookahead buffer
    // increases the number of blocks found during an allocation pass. The
    // lookahead buffer is stored as a compact bitmap, so each byte of RAM
    // can track 8 blocks. Must be a multiple of 8.
    return 16;
}

static void dump_buf(uint8_t *buf, uint32_t size)
{
    uint32_t remain_len = size;
    uint32_t offset = 0;
    uint32_t dump_len = 16;

    while (remain_len) {
        dump_len = 16;
        dump_len = dump_len > remain_len ? remain_len : dump_len;
        hal_trace_dump("0x%02x ", 1, dump_len, buf + offset);
        offset += dump_len;
        remain_len -= dump_len;
        osDelay(1);
    }
}

int lfs_ram_test(void)
{
    int r = 0;
    uint8_t read_buf[LFS_RAM_SECTOR_SIZE_IN_BYTES] = {0};
    uint8_t write_buf[LFS_RAM_SECTOR_SIZE_IN_BYTES] = {0};
    lfs_ram_get_block_size();
    r = lfs_ram_read(NULL, 0, 0, read_buf, LFS_RAM_SECTOR_SIZE_IN_BYTES);
    TRACE(0, "1. read ret = %d\n", r);
    dump_buf(read_buf, 16);

#if 1
    TRACE(0, "2. earse and read\n");
    lfs_ram_erase(NULL, 0);
    lfs_ram_read(NULL, 0, 0, read_buf, LFS_RAM_SECTOR_SIZE_IN_BYTES);
    dump_buf(read_buf, 16);
#endif

#if 1
    write_buf[0] = 0x0A;
    write_buf[1] = 0x0B;
    write_buf[2] = 0x0C;
    write_buf[3] = 0x0D;
    write_buf[4] = 0x0E;
    lfs_ram_prog(NULL, 0, 0, write_buf, LFS_RAM_SECTOR_SIZE_IN_BYTES);
    lfs_ram_read(NULL, 0, 0, read_buf, LFS_RAM_SECTOR_SIZE_IN_BYTES);
    TRACE(0, "3. write and read\n");
    dump_buf(read_buf, 16);
#endif

#if 1
    lfs_ram_erase(NULL, 0);
    TRACE(0, "3. earse and read\n");
    lfs_ram_read(NULL, 0, 0, read_buf, LFS_RAM_SECTOR_SIZE_IN_BYTES);
    dump_buf(read_buf, 16);
#endif

    return 0;
}

#endif /* LFS_ON_RAM */