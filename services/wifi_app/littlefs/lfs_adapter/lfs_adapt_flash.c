#ifdef LFS_ON_FLASH
#include "cmsis_os.h"
#include "cmsis.h"
// #include "net_memory.h"
#include "pmu.h"
#include "lfs.h"
#include "lfs_adapt.h"
#include "hal_norflash.h"
#include "hal_trace.h"
// #include "app_utils.h"
#include "watchdog/watchdog.h"
#include <string.h>
#include <stdlib.h>

#define LFS_FLASH_DEBUG 0//DEBUG
//0xd00000
//#define LFS_FLASH_BASE_ADDR (0x28d00000 - FLASH_NC_BASE) // offset base on FLASH_NC_BASE
#ifndef LFS_FLASH_BASE_ADDR
//extern uint32_t __reserved_start[];
//const uint32_t LFS_FLASH_BASE_ADDR = ((uint32_t)__reserved_start - FLASH_NC_BASE);
#endif

//#define LFS_FLASH_TOTAL_SIZE (0x200000) //(0x1000 * 2)	// TODO
#define LFS_FLASH_PAGE_SIZE_IN_BYTES (256)
#define LFS_FLASH_SECTOR_SIZE_IN_BYTES (4096)

static osMutexId FlashMutex = NULL;
static osMutexDef_t os_mutex_def_flash;
static void FlashosMutexWait(void)
{
    if (FlashMutex == NULL)
    {
        FlashMutex = osMutexCreate(&os_mutex_def_flash);
    }

    osMutexWait(FlashMutex, osWaitForever);
}

static int bes_hal_flash_read(const uint32_t addr, uint8_t *dst, const uint32_t size)
{
    int ret = 0;
    uint32_t lock = 0;
    volatile char *flashPointer = NULL;

    if (NULL == dst)
    {
        ret = -2;
        goto RETURN;
    }

    FlashosMutexWait();
    lock = int_lock();
    flashPointer = (volatile char *)(FLASH_NC_BASE + addr);
    memcpy(dst, (void *)flashPointer, size);
    int_unlock(lock);
    osMutexRelease(FlashMutex);

RETURN:
    return ret;
}

static int bes_hal_flash_write(const uint32_t addr, const uint8_t *src, const uint32_t size)
{
    int ret = 0;
    uint32_t lock;
    uint32_t num = 0;
    uint32_t left_len = 0;
    uint32_t align_len = 0;
    uint32_t fill_len = 0;
    uint32_t flash_offset = addr;
    uint32_t lengthToBurn = size;
    uint8_t *ptrSource = NULL;
    volatile char *flashPointer = NULL;
    uint8_t *buf = NULL;

    ptrSource = (uint8_t *)src;
    if (NULL == ptrSource)
    {
        ret = -2;
        goto RETURN;
    }

    FlashosMutexWait();
    if (addr % FLASH_BLOCK_SIZE_IN_BYTES != 0)
    {
        //buf = (uint8_t *)nts_malloc(FLASH_BLOCK_SIZE_IN_BYTES);
        buf = (uint8_t *)malloc(FLASH_BLOCK_SIZE_IN_BYTES);
        if (!buf)
        {
            TRACE(0, "%s %d, nts_malloc error", __func__, __LINE__);
            ret = -1;
            goto end;
        }
    }
    else
    {
        if (lengthToBurn % FLASH_BLOCK_SIZE_IN_BYTES != 0)
        {
            //buf = (uint8_t *)nts_malloc(FLASH_BLOCK_SIZE_IN_BYTES);
            buf = (uint8_t *)malloc(FLASH_BLOCK_SIZE_IN_BYTES);
            if (!buf)
            {
                TRACE(0, "%s %d, nts_malloc error", __func__, __LINE__);
                ret = -1;
                goto end;
            }
        }
    }

    pmu_flash_write_config();
    if (flash_offset % FLASH_BLOCK_SIZE_IN_BYTES != 0)
    {
        left_len = FLASH_BLOCK_SIZE_IN_BYTES - flash_offset % FLASH_BLOCK_SIZE_IN_BYTES;
        fill_len = (left_len >= lengthToBurn) ? lengthToBurn : left_len;

        align_len = flash_offset;
        align_len -= align_len % FLASH_BLOCK_SIZE_IN_BYTES;
        memset(buf, 0, FLASH_BLOCK_SIZE_IN_BYTES);
        // read first
        lock = int_lock();
        flashPointer = (volatile char *)(FLASH_NC_BASE + align_len);
        memcpy(buf, (void *)flashPointer, FLASH_BLOCK_SIZE_IN_BYTES);

        ret = hal_norflash_erase(HAL_FLASH_ID_0, align_len, FLASH_BLOCK_SIZE_IN_BYTES);
        if (ret != HAL_NORFLASH_OK)
        {
            TRACE(0, "error %s %d, hal_norflash_erase ret:%d", __func__, __LINE__, ret);
            pmu_flash_read_config();
            int_unlock(lock);
            goto end;
        }

        memcpy((buf + flash_offset % FLASH_BLOCK_SIZE_IN_BYTES), ptrSource, fill_len);
        ret = hal_norflash_write(HAL_FLASH_ID_0, align_len, buf, FLASH_BLOCK_SIZE_IN_BYTES);
        if (ret != HAL_NORFLASH_OK)
        {
            TRACE(0, "error %s %d, hal_norflash_write ret:%d", __func__, __LINE__, ret);
            pmu_flash_read_config();
            int_unlock(lock);
            goto end;
        }
        int_unlock(lock);

#ifdef __WATCHER_DOG_RESET__
        watchdog_ping();
#endif

        lengthToBurn -= fill_len;
        flash_offset += fill_len;
        ptrSource += fill_len;
    }

    if (lengthToBurn > 0)
    {
        for (num = 0; num < lengthToBurn / FLASH_BLOCK_SIZE_IN_BYTES; num++)
        {
            lock = int_lock();
            ret = hal_norflash_erase(HAL_FLASH_ID_0, flash_offset, FLASH_BLOCK_SIZE_IN_BYTES);
            if (ret != HAL_NORFLASH_OK)
            {
                TRACE(0, "error %s %d, hal_norflash_erase ret:%d", __func__, __LINE__, ret);
                pmu_flash_read_config();
                int_unlock(lock);
                goto end;
            }
            ret = hal_norflash_write(HAL_FLASH_ID_0, flash_offset, ptrSource, FLASH_BLOCK_SIZE_IN_BYTES);
            if (ret != HAL_NORFLASH_OK)
            {
                TRACE(0, "error %s %d, hal_norflash_write ret:%d", __func__, __LINE__, ret);
                pmu_flash_read_config();
                int_unlock(lock);
                goto end;
            }
            int_unlock(lock);

#ifdef __WATCHER_DOG_RESET__
            watchdog_ping();
#endif
            flash_offset += FLASH_BLOCK_SIZE_IN_BYTES;
            ptrSource += FLASH_BLOCK_SIZE_IN_BYTES;
        }

        left_len = lengthToBurn % FLASH_BLOCK_SIZE_IN_BYTES;
        if (left_len)
        {
            memset(buf, 0, FLASH_BLOCK_SIZE_IN_BYTES);
            // read first
            lock = int_lock();
            flashPointer = (volatile char *)(FLASH_NC_BASE + flash_offset);
            memcpy(buf, (void *)flashPointer, FLASH_BLOCK_SIZE_IN_BYTES);

            ret = hal_norflash_erase(HAL_FLASH_ID_0, flash_offset, FLASH_BLOCK_SIZE_IN_BYTES);
            if (ret != HAL_NORFLASH_OK)
            {
                TRACE(0, "error %s %d, hal_norflash_erase ret:%d", __func__, __LINE__, ret);
                pmu_flash_read_config();
                int_unlock(lock);
                goto end;
            }

            memcpy(buf, ptrSource, left_len);
            ret = hal_norflash_write(HAL_FLASH_ID_0, flash_offset, buf, FLASH_BLOCK_SIZE_IN_BYTES);
            if (ret != HAL_NORFLASH_OK)
            {
                TRACE(0, "error %s %d, hal_norflash_write ret:%d", __func__, __LINE__, ret);
                pmu_flash_read_config();
                int_unlock(lock);
                goto end;
            }
            int_unlock(lock);

#ifdef __WATCHER_DOG_RESET__
            watchdog_ping();
#endif
        }
    }
    pmu_flash_read_config();

end:
    if (addr % FLASH_BLOCK_SIZE_IN_BYTES != 0)
    {
        //nts_free(buf);
        free(buf);
        buf = NULL;
    }
    else
    {
        if (lengthToBurn % FLASH_BLOCK_SIZE_IN_BYTES != 0)
        {
            //A
            //nts_free(buf);
            free(buf);
            buf = NULL;
        }
    }
    osMutexRelease(FlashMutex);

RETURN:
    return ret;
}

static int bes_hal_flash_erase(const uint32_t addr, const uint32_t size)
{
    int i = 0;
    int ret = 0;
    uint32_t lock = 0;
    uint32_t start_addr = addr;
    uint16_t start_sec = (addr / LFS_FLASH_SECTOR_SIZE_IN_BYTES);
    uint16_t end_sec = ((addr + size - 1) / LFS_FLASH_SECTOR_SIZE_IN_BYTES);

    if (addr % LFS_FLASH_SECTOR_SIZE_IN_BYTES != 0)
    {
        TRACE(0, "%s %d, error addr:%08x", __func__, __LINE__, addr);
        return -2;
    }

    FlashosMutexWait();
    lock = int_lock();
    pmu_flash_write_config();

    for (i = start_sec; i <= end_sec; i++)
    {
        ret = hal_norflash_erase(HAL_FLASH_ID_0, start_addr, LFS_FLASH_SECTOR_SIZE_IN_BYTES);
        if (ret != HAL_NORFLASH_OK)
        {
            TRACE(0, "error %s %d, ret:%d", __func__, __LINE__, ret);
            goto end;
        }
        start_addr += LFS_FLASH_SECTOR_SIZE_IN_BYTES;
    }

end:
    pmu_flash_read_config();
    int_unlock(lock);
    osMutexRelease(FlashMutex);

    return ret;
}

#if LFS_FLASH_DEBUG
static void dump_buf(uint8_t *buf, uint32_t size);
static void lfs_flash_dump(uint32_t addr)
{
    uint32_t dump[128] = {0};
    bes_hal_flash_read(addr, dump, 128);
    dump_buf(dump, 128);
}
#endif

int lfs_flash_open(void)
{
#if LFS_FLASH_DEBUG
    TRACE(0, "lfs flash start addr = %08x", LFS_FLASH_BASE_ADDR);
    //lfs_flash_dump(LFS_FLASH_BASE_ADDR);
#endif
}

int lfs_flash_close(void)
{
    return 0;
}

int lfs_flash_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    uint32_t start_addr = LFS_FLASH_BASE_ADDR + block * LFS_FLASH_SECTOR_SIZE_IN_BYTES + off;
#if LFS_FLASH_DEBUG
    TRACE(0, "[lfs] blk_read, block = %d, off = %d, size = %d, star_addr = 0x%0x\n", block, off, size, start_addr);
#endif
    return bes_hal_flash_read(start_addr, buffer, size);
}

#if LFS_FLASH_DEBBUG
void dump_all_data_partion(void)
{
    uint32_t page_count = 0;
    uint8_t buf[LFS_FLASH_PAGE_SIZE_IN_BYTES] = {0};
    int ret = 0;
    uint32_t start_addr = 0;

    while (1)
    {
        start_addr = LFS_FLASH_BASE_ADDR + page_count * LFS_FLASH_PAGE_SIZE_IN_BYTES;
        ret = bes_hal_flash_read(start_addr, buf, LFS_FLASH_PAGE_SIZE_IN_BYTES);
        if (ret < 0)
        {
            TRACE(0, "demp err = %d", ret);
            break;
        }
        TRACE(0, "0x%0x", start_addr);
        dump_buf(buf, LFS_FLASH_PAGE_SIZE_IN_BYTES);
        TRACE(0, "\n");
        if (start_addr == LFS_FLASH_BASE_ADDR + LFS_FLASH_TOTAL_SIZE)
        {
            break;
        }
        page_count++;
    }
}
#endif

int lfs_flash_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    uint32_t start_addr = LFS_FLASH_BASE_ADDR + block * LFS_FLASH_SECTOR_SIZE_IN_BYTES + off;
#if LFS_FLASH_DEBUG
    TRACE(0, "[lfs] blk_prog, block = %d, off = %d, size = %d, start_addr = %d\n", block, off, size, start_addr);
#endif
    return bes_hal_flash_write(start_addr, buffer, size);
}

int lfs_flash_erase(const struct lfs_config *c, lfs_block_t block)
{
    uint32_t start_addr = LFS_FLASH_BASE_ADDR + block * LFS_FLASH_SECTOR_SIZE_IN_BYTES;
#if LFS_FLASH_DEBUG
    TRACE(0, "[lfs] blk_erase, block = %d, start_addr = 0x%0x\n", block, start_addr);
#endif

    return bes_hal_flash_erase(start_addr, LFS_FLASH_SECTOR_SIZE_IN_BYTES);
}

int lfs_flash_sync(const struct lfs_config *c)
{
#if LFS_FLASH_DEBUG
    TRACE(0, "[lfs] blk_sync\n");
#endif
    return 0;
}

lfs_size_t lfs_flash_get_read_size(void)
{
    // Minimum size of a block read. All read operations will be a
    // multiple of this value.
    return LFS_FLASH_PAGE_SIZE_IN_BYTES;
}

lfs_size_t lfs_flash_get_prog_size(void)
{
    // Minimum size of a block program. All program operations will be a
    // multiple of this value.
    return LFS_FLASH_PAGE_SIZE_IN_BYTES;
}

lfs_size_t lfs_flash_get_block_size(void)
{
    // Size of an erasable block. This does not impact ram consumption and
    // may be larger than the physical erase size. However, non-inlined files
    // take up at minimum one block. Must be a multiple of the read
    // and program sizes.
    return LFS_FLASH_SECTOR_SIZE_IN_BYTES;
}

lfs_size_t lfs_flash_get_block_count(void)
{
    // Number of erasable blocks on the device.
    //uint32_t flash_total_size = hal_norflash_get_flash_total_size(HAL_NORFLASH_ID_0);
    //uint32_t useable_flash_size = flash_total_size - LFS_FLASH_BASE_ADDR;
    uint32_t useable_flash_size = LFS_FLASH_TOTAL_SIZE;
    uint32_t sector_num = useable_flash_size / LFS_FLASH_SECTOR_SIZE_IN_BYTES;
    TRACE(0, "useable_flash_size = %d(0x%0x), useable sector_num = %d\n", useable_flash_size, useable_flash_size, sector_num);
    return sector_num;
}

int lfs_flash_get_block_cycles(void)
{
    // Number of erase cycles before littlefs evicts metadata logs and moves
    // the metadata to another block. Suggested values are in the
    // range 100-1000, with large values having better performance at the cost
    // of less consistent wear distribution.
    //
    // Set to -1 to disable block-level wear-leveling.
    return 100;
}

lfs_size_t lfs_flash_get_cache_size(void)
{
    // Size of block caches. Each cache buffers a portion of a block in RAM.
    // The littlefs needs a read cache, a program cache, and one additional
    // cache per file. Larger caches can improve performance by storing more
    // data and reducing the number of disk accesses. Must be a multiple of
    // the read and program sizes, and a factor of the block size.
    return LFS_FLASH_PAGE_SIZE_IN_BYTES;
}

lfs_size_t lfs_flash_get_lookahead_size(void)
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

    while (remain_len)
    {
        dump_len = 16;
        dump_len = dump_len > remain_len ? remain_len : dump_len;
        hal_trace_dump("0x%02x ", 1, dump_len, buf + offset);
        offset += dump_len;
        remain_len -= dump_len;
        osDelay(1);
    }
}

int lfs_flash_test(void)
{
    int r = 0;
    uint8_t read_buf[LFS_FLASH_SECTOR_SIZE_IN_BYTES] = {0};
    uint8_t write_buf[LFS_FLASH_SECTOR_SIZE_IN_BYTES] = {0};
    lfs_flash_get_block_size();
    r = lfs_flash_read(NULL, 0, 0, read_buf, LFS_FLASH_SECTOR_SIZE_IN_BYTES);
    TRACE(0, "1. read ret = %d\n", r);
    dump_buf(read_buf, 16);

#if 1
    TRACE(0, "2. earse and read\n");
    lfs_flash_erase(NULL, 0);
    lfs_flash_read(NULL, 0, 0, read_buf, LFS_FLASH_SECTOR_SIZE_IN_BYTES);
    dump_buf(read_buf, 16);
#endif

#if 1
    write_buf[0] = 0x0A;
    write_buf[1] = 0x0B;
    write_buf[2] = 0x0C;
    write_buf[3] = 0x0D;
    write_buf[4] = 0x0E;
    lfs_flash_prog(NULL, 0, 0, write_buf, LFS_FLASH_SECTOR_SIZE_IN_BYTES);
    lfs_flash_read(NULL, 0, 0, read_buf, LFS_FLASH_SECTOR_SIZE_IN_BYTES);
    TRACE(0, "3. write and read\n");
    dump_buf(read_buf, 16);
#endif

#if 1
    lfs_flash_erase(NULL, 0);
    TRACE(0, "3. earse and read\n");
    lfs_flash_read(NULL, 0, 0, read_buf, LFS_FLASH_SECTOR_SIZE_IN_BYTES);
    dump_buf(read_buf, 16);
#endif

    return 0;
}

#endif /* LFS_ON_FLASH */