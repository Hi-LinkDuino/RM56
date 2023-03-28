#ifdef LFS_ON_SD_CARD
#include "lfs.h"
#include "lfs_adapt.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_sdmmc.h"
#include "hal_timer.h"
#include "cmsis_os.h"
#include "hal_trace.h"

#define LFS_SD_CARD_DEBUG 0

#define SD_CARD_BLOCK_SIZE_IN_BYTES (512)

static uint8_t sd_device_opened = 0;

#if 0
/* sd iomux for best2000-iwa/iwe */
static struct HAL_IOMUX_PIN_FUNCTION_MAP best2000_sd_iomux[] = {
    {HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_SDIO_CLK, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
    {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_SDIO_CMD, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
    {HAL_IOMUX_PIN_P1_6, HAL_IOMUX_FUNC_SDIO_DATA0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},#endif
};
#endif

#if 1
/* sd iomux for best2001 */
static struct HAL_IOMUX_PIN_FUNCTION_MAP best2001_sd_iomux[] = {
    {HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_SDMMC_CMD, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
    {HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_SDMMC_CLK, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
    {HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_SDMMC_DATA0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
#if 0
	{HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_SDMMC_DATA1, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
	{HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_SDMMC_DATA2, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
	{HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_SDMMC_DATA3, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
#endif
};
#endif

#if 1
static struct HAL_IOMUX_PIN_FUNCTION_MAP fix_board_error_pinmux[] = {
    /* just for bes mainboard */
    {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    {HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    {HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
};

void sd_card_fix_board_error()
{
    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)fix_board_error_pinmux, sizeof(fix_board_error_pinmux) / sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));
    for (int i = 0; i < sizeof(fix_board_error_pinmux) / sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP); i++)
    {
        hal_gpio_pin_set_dir(fix_board_error_pinmux[i].pin, HAL_GPIO_DIR_IN, 0);
    }
}
#endif

static void iomux_sdmmc(void)
{
    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)best2001_sd_iomux, sizeof(best2001_sd_iomux) / sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));
}

static uint32_t sector_size = 0, sector_count = 0;

int lfs_sd_card_open(void)
{
    if (sd_device_opened)
        return -1;

#if 1
    sd_card_fix_board_error(); /* for bes2300wp and mainboard hardware error */
#endif
    iomux_sdmmc();
    if (0 != hal_sdmmc_open(HAL_SDMMC_ID_0))
    {
        TRACE(0, "sd open error");
    }
    sd_device_opened = 1;
    return 0;
}

int lfs_sd_card_close(void)
{
    if (sd_device_opened)
    {
        hal_sdmmc_close(HAL_SDMMC_ID_0);
        sd_device_opened = 0;
    }

    return 0;
}

int lfs_sd_card_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    uint32_t addr = block * SD_CARD_BLOCK_SIZE_IN_BYTES + off;
    uint32_t start_block = addr / SD_CARD_BLOCK_SIZE_IN_BYTES;
    uint32_t block_count = size / SD_CARD_BLOCK_SIZE_IN_BYTES;
    int ret = 0;

#if LFS_SD_CARD_DEBUG
    TRACE(0, "[lfs] blk_read, block = %d, off = %d, size = %d, start_block = %d, block_count = %d\n", block, off, size, start_block, block_count);
#endif

    /* bd->read(buffer, (bd_addr_t)block * c->block_size + off, size); */
    ret = hal_sdmmc_read_blocks(HAL_SDMMC_ID_0, start_block, block_count, (uint8_t *)buffer);
    if (ret == block_count)
        return 0;
    TRACE(0, "blk_read error. ret = %d, block_count = %d\n", ret, block_count);
    return -1;
}

int lfs_sd_card_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    uint32_t addr = block * SD_CARD_BLOCK_SIZE_IN_BYTES + off;
    uint32_t start_block = addr / SD_CARD_BLOCK_SIZE_IN_BYTES;
    uint32_t block_count = size / SD_CARD_BLOCK_SIZE_IN_BYTES;
    int ret = 0;

#if LFS_SD_CARD_DEBUG
    TRACE(0, "[lfs] blk_read, block = %d, off = %d, size = %d, start_block = %d, block_count = %d\n", block, off, size, start_block, block_count);
#endif

    /* bd->program(buffer, (bd_addr_t)block * c->block_size + off, size) */
    ret = hal_sdmmc_write_blocks(HAL_SDMMC_ID_0, start_block, block_count, (uint8_t *)buffer);
    if (ret == block_count)
        return 0;
    TRACE(0, "blk_write errro. ret = %d, block_count = %d\n", ret, block_count);
    return -1;
}

int lfs_sd_card_erase(const struct lfs_config *c, lfs_block_t block)
{
#if LFS_SD_CARD_DEBUG
    TRACE(0, "[lfs] blk_earse, block = %d, \n", block);
#endif

    return 0;
}

int lfs_sd_card_sync(const struct lfs_config *c)
{
#if LFS_SD_CARD_DEBUG
    TRACE(0, "[lfs] blk_sync\n");
#endif

    return 0;
}

lfs_size_t lfs_sd_card_get_read_size(void)
{
    // Minimum size of a block read. All read operations will be a
    // multiple of this value.
    return SD_CARD_BLOCK_SIZE_IN_BYTES;
}

lfs_size_t lfs_sd_card_get_prog_size(void)
{
    // Minimum size of a block program. All program operations will be a
    // multiple of this value.
    return SD_CARD_BLOCK_SIZE_IN_BYTES;
}

lfs_size_t lfs_sd_card_get_block_size(void)
{
    // Size of an erasable block. This does not impact ram consumption and
    // may be larger than the physical erase size. However, non-inlined files
    // take up at minimum one block. Must be a multiple of the read
    // and program sizes.
    return SD_CARD_BLOCK_SIZE_IN_BYTES;
}

lfs_size_t lfs_sd_card_get_block_count(void)
{
    // Number of erasable blocks on the device.
    hal_sdmmc_info(HAL_SDMMC_ID_0, &sector_count, &sector_size);
    int block_count = sector_count * (sector_size / SD_CARD_BLOCK_SIZE_IN_BYTES);
    TRACE(0, "sector_count = %d, sector_size = %d, block_count = %d\n", sector_count, sector_size, block_count);
    return block_count;
}

int lfs_sd_card_get_block_cycles(void)
{
    // Number of erase cycles before littlefs evicts metadata logs and moves
    // the metadata to another block. Suggested values are in the
    // range 100-1000, with large values having better performance at the cost
    // of less consistent wear distribution.
    //
    // Set to -1 to disable block-level wear-leveling.
    return 100;
}

lfs_size_t lfs_sd_card_get_cache_size(void)
{
    // Size of block caches. Each cache buffers a portion of a block in RAM.
    // The littlefs needs a read cache, a program cache, and one additional
    // cache per file. Larger caches can improve performance by storing more
    // data and reducing the number of disk accesses. Must be a multiple of
    // the read and program sizes, and a factor of the block size.
    return 512;
}

lfs_size_t lfs_sd_card_get_lookahead_size(void)
{
    // Size of the lookahead buffer in bytes. A larger lookahead buffer
    // increases the number of blocks found during an allocation pass. The
    // lookahead buffer is stored as a compact bitmap, so each byte of RAM
    // can track 8 blocks. Must be a multiple of 8.
    return 128;
}

int lsf_sd_card_test(void)
{
    int ret = 0;
    uint32_t start_block = 0;
    uint32_t block_count = 1;
    uint8_t buffer[512] = {0};
    buffer[0] = 0x0A;
    buffer[1] = 0x0B;
    buffer[2] = 0x0C;
    uint8_t read_buf[512] = {0};

    ret = hal_sdmmc_write_blocks(HAL_SDMMC_ID_0, start_block, block_count, buffer);
    TRACE(0, "sd_test write ret = %d\n", ret);

#if 0    
    ret = hal_sdmmc_read_blocks(HAL_SDMMC_ID_0, start_block , block_count, read_buf);
    TRACE(0, "sd_test read ret = %d\n", ret);
    TRACE(0, "read_buf[0] = 0x%0x\n", read_buf[0]);
    TRACE(0, "read_buf[1] = 0x%0x\n", read_buf[1]);
    TRACE(0, "read_buf[2] = 0x%0x\n", read_buf[2]);
    TRACE(0, "read_buf[3] = 0x%0x\n", read_buf[3]);
#endif

    ret = hal_sdmmc_read_blocks(HAL_SDMMC_ID_0, start_block, block_count, read_buf);
    TRACE(0, "sd_test read ret = %d\n", ret);
    TRACE(0, "read_buf[0] = 0x%0x\n", read_buf[0]);
    TRACE(0, "read_buf[1] = 0x%0x\n", read_buf[1]);
    TRACE(0, "read_buf[2] = 0x%0x\n", read_buf[2]);
    TRACE(0, "read_buf[3] = 0x%0x\n", read_buf[3]);
    return 0;
}

#endif /* LFS_ON_SD_CARD */