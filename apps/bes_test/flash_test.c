/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
//#include "hwtest.h"
#include "cmsis.h"
#include "hal_cache.h"
#include "hal_iomux.h"
#include "hal_location.h"
#include "hal_norflash.h"
#include "hal_timer.h"
#include "hwtimer_list.h"
#include "string.h"
#include "hal_trace.h"
#include "norflash_api.h"

#define FLH_SIMPLE_TEST
//#define FLH_MODE_TEST
#define FLH_FACTORY_TEST
//#define FLH_SPEED_TEST
//#define FLH_SUSPEND_TEST

/* flash factory test cases */
#ifdef FLH_FACTORY_TEST
//#define FLH_TEST_ER
#define FLH_TEST_EWR
//#define FLH_TEST_CRATCH
#endif

/* flash debug trace */
#define DEBUG_FLASH

#define TEST_FLASH_1

#ifdef TEST_FLASH_1
static enum HAL_FLASH_ID_T flash_id = HAL_FLASH_ID_1;
POSSIBLY_UNUSED
static uint32_t flash_base_addr = FLASH1_BASE;
static uint32_t flash_nc_base_addr = FLASH1_NC_BASE;
#else
static enum HAL_FLASH_ID_T flash_id = HAL_FLASH_ID_0;
POSSIBLY_UNUSED
static uint32_t flash_base_addr = FLASH_BASE;
POSSIBLY_UNUSED
static uint32_t flash_nc_base_addr = FLASH_NC_BASE;
#endif

#ifdef DEBUG_FLASH
#define FLH_DEBUG(...) TRACE(0, ##__VA_ARGS__)
#else
#define FLH_DEBUG(...) do{}while(1)
#endif

#define FLASH_BUF_SIZE 0x10000

unsigned char flash_src_buf[FLASH_BUF_SIZE];

POSSIBLY_UNUSED
SRAM_TEXT_LOC static void init_flash(void)
{
    return 0;
    int ret;
    uint8_t flash_dev_id[HAL_NORFLASH_DEVICE_ID_LEN];
    const struct HAL_NORFLASH_CONFIG_T *cfg;

#if defined(FLASH1_CTRL_BASE) && defined(CHIP_BEST2003)
    if (flash_id == HAL_FLASH_ID_1) {
        hal_cmu_set_flash0_size(HAL_FLASH0_SIZE_0M);
    }
#endif

    hal_cmu_pll_enable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_SYS);
    hal_cmu_flash_all_select_pll(HAL_CMU_PLL_AUD);

    cfg = hal_norflash_get_init_config();
    ret = hal_norflash_open(flash_id, cfg);

    hal_norflash_get_id(flash_id, flash_dev_id, ARRAY_SIZE(flash_dev_id));
    TRACE(3,"FLASH_ID: %02X-%02X-%02X", flash_dev_id[0], flash_dev_id[1], flash_dev_id[2]);

    hal_norflash_show_calib_result(flash_id);

    ASSERT(ret == 0, "Failed to open norflash: %d", ret);
}

/******************************* flash mode test ********************************/
#ifdef FLH_MODE_TEST
//#define FLH_TEST_SPI
//#define FLH_TEST_DUAL
#define FLH_TEST_QUAD
//#define FLH_TEST_SPI_WRAP
//#define FLH_TEST_DUAL_WRAP
//#define FLH_TEST_QUAD_WRAP

unsigned char flash_dst_buf[FLASH_BUF_SIZE];

struct flash_test_info {
    uint32_t base;
    uint32_t total;
    uint32_t sector;
    uint32_t block;
    uint32_t page;
    int mode;
};

static struct flash_test_info tag;

static void flash_udelay(int us)
{
    hal_sys_timer_delay(US_TO_TICKS(us));
}

int flash_rw(unsigned int addr, unsigned int len)
{
    unsigned int k;
    int err = 0;

    len  = len > FLASH_BUF_SIZE ? FLASH_BUF_SIZE : len;
    FLH_DEBUG("%s %x, %d\n", __func__, addr, len);

    for(k = 0; k < len; k++) {
        flash_src_buf[k] = k;
        flash_dst_buf[k] = 0;
    }

    flash_udelay(100);

    FLH_DEBUG("erase ...\n");
    hal_norflash_erase(flash_id, addr, len);

    FLH_DEBUG("write ...\n");
    hal_norflash_write(flash_id, addr, flash_src_buf, len);

    FLH_DEBUG("read ...\n");
    hal_norflash_read(flash_id, addr, flash_dst_buf, len);

    for (k = 0; k < len; k++) {
        if (flash_dst_buf[k] != flash_src_buf[k])
            err++;
    }

    FLH_DEBUG("%s done %d\n", __func__, err);
    return err;
}

int flash_wrap_rw(unsigned int addr, unsigned int len)
{
    unsigned int k;
    int err = 0;

    len = tag.page;

    len  = len > FLASH_BUF_SIZE ? FLASH_BUF_SIZE : len;
    FLH_DEBUG("%s %x, %d\n", __func__, addr, len);


    FLH_DEBUG("erase ...\n");
    hal_norflash_erase(flash_id, flash_base_addr, tag.sector);
    hal_norflash_erase(flash_id, flash_base_addr + tag.total - tag.sector, tag.sector);

    FLH_DEBUG("write ...\n");
    memset(flash_src_buf, 0xa5, len);
    addr = flash_base_addr;
    hal_norflash_write(flash_id, addr, flash_src_buf, len);

    memset(flash_src_buf, 0xde, len);
    addr = flash_base_addr + tag.total - tag.page;
    hal_norflash_write(flash_id, addr, flash_src_buf, len);

    len = 64;
    memset(flash_dst_buf, 0x00, len);
    addr = flash_base_addr + tag.total - len / 2;
    FLH_DEBUG("read from addr %x, %d bytes ...\n", addr, len);
    hal_norflash_read(flash_id, addr, flash_dst_buf, len);

    unsigned char *pd = flash_dst_buf;
    FLH_DEBUG("\n");
    for(k = 0; k < 64; k += 8) {
            FLH_DEBUG("%2x, %2x, %2x, %2x, %2x, %2x, %2x, %2x\n",
                        pd[0], pd[1], pd[2], pd[3], pd[4], pd[5], pd[6], pd[7]);
            pd += 8;
    }
    FLH_DEBUG("\n");

    FLH_DEBUG("%s done %d\n", __func__, err);
    return err;
}

int flash_mode_rw(unsigned int mode)
{
    static struct HAL_NORFLASH_CONFIG_T cfg;
    uint8_t id[HAL_NORFLASH_DEVICE_ID_LEN];
    uint32_t total, block, sector, page;
    int r;

    memset(&cfg, 0, sizeof(cfg));
    cfg.source_clk = HAL_NORFLASH_SPEED_13M * 2;
    cfg.speed = HAL_NORFLASH_SPEED_13M;
    cfg.mode = (enum HAL_NORFLASH_OP_MODE)mode;
    cfg.override_config = 0;

    FLH_DEBUG("%s mode: %x\n", __func__, mode);
    r = hal_norflash_open(flash_id, &cfg);
    if (r) {
        FLH_DEBUG("open failed %d\n", r);
        return r;
    }

    hal_norflash_get_id(flash_id, id, HAL_NORFLASH_DEVICE_ID_LEN);
    FLH_DEBUG("id: %x-%x-%x\n", id[0], id[1], id[2]);

    hal_norflash_get_size(flash_id, &total, &block, &sector, &page);
    FLH_DEBUG("size: total: %d KB, block: %d KB, sector: %d KB, page: %d B\n",
                total/1024, block/1024, sector/1024, page);

    tag.base = flash_base_addr;
    tag.total = total;
    tag.sector = sector;
    tag.block = block;
    tag.page = page;
    tag.mode = mode;

    flash_udelay(100);

    r =  flash_rw(flash_base_addr, 0x8000);  //32KB
    r += flash_rw(flash_base_addr, 0x10000); //64KB
    r += flash_rw(FLASHX_BASE, 0x8000); //64KB
    r += flash_rw(FLASHX_BASE, 0x10000); //64KB
    r += flash_rw(FLASH_NC_BASE, 0x8000);  //32KB
    r += flash_rw(FLASH_NC_BASE, 0x10000); //64KB
    r += flash_rw(FLASHX_NC_BASE, 0x8000); //64KB
    r += flash_rw(FLASHX_NC_BASE, 0x10000); //64KB

    if (mode & HAL_NORFLASH_OP_MODE_READ_WRAP) {
        r +=  flash_wrap_rw(flash_base_addr, 0x8000);
    }

    hal_norflash_close(flash_id);
    return r;
}

void flash_more_cases(void)
{
    int r = 0;

    hal_cmu_simu_init();

#ifdef FLH_TEST_SPI
    FLH_DEBUG("Flash STAND SPI mode tests\n");
    r += flash_mode_rw(HAL_NORFLASH_OP_MODE_STAND_SPI | HAL_NORFLASH_OP_MODE_PAGE_PROGRAM);
    r += flash_mode_rw(HAL_NORFLASH_OP_MODE_STAND_SPI | HAL_NORFLASH_OP_MODE_CONTINUOUS_READ | HAL_NORFLASH_OP_MODE_PAGE_PROGRAM);
    r += flash_mode_rw(HAL_NORFLASH_OP_MODE_STAND_SPI
                    | HAL_NORFLASH_OP_MODE_CONTINUOUS_READ
                    | HAL_NORFLASH_OP_MODE_HIGH_PERFORMANCE | HAL_NORFLASH_OP_MODE_PAGE_PROGRAM);
#endif

#ifdef FLH_TEST_DUAL
    FLH_DEBUG("Flash DUAL tests\n");
    r += flash_mode_rw(HAL_NORFLASH_OP_MODE_DUAL_IO | HAL_NORFLASH_OP_MODE_DUAL_PAGE_PROGRAM);
    r += flash_mode_rw(HAL_NORFLASH_OP_MODE_DUAL_IO | HAL_NORFLASH_OP_MODE_CONTINUOUS_READ | HAL_NORFLASH_OP_MODE_DUAL_PAGE_PROGRAM);
    r += flash_mode_rw(HAL_NORFLASH_OP_MODE_DUAL_IO
                    | HAL_NORFLASH_OP_MODE_CONTINUOUS_READ
                    | HAL_NORFLASH_OP_MODE_HIGH_PERFORMANCE | HAL_NORFLASH_OP_MODE_DUAL_PAGE_PROGRAM);
#endif

#ifdef FLH_TEST_QUAD
    FLH_DEBUG("Flash QUAD mode tests\n");
    r += flash_mode_rw(HAL_NORFLASH_OP_MODE_QUAD_IO | HAL_NORFLASH_OP_MODE_QUAD_PAGE_PROGRAM);
    r += flash_mode_rw(HAL_NORFLASH_OP_MODE_QUAD_IO | HAL_NORFLASH_OP_MODE_CONTINUOUS_READ | HAL_NORFLASH_OP_MODE_QUAD_PAGE_PROGRAM);
    r += flash_mode_rw(HAL_NORFLASH_OP_MODE_QUAD_IO
                    | HAL_NORFLASH_OP_MODE_CONTINUOUS_READ
                    | HAL_NORFLASH_OP_MODE_HIGH_PERFORMANCE | HAL_NORFLASH_OP_MODE_QUAD_PAGE_PROGRAM);
#endif

#ifdef FLH_TEST_SPI_WRAP
    FLH_DEBUG("Flash SPI wrap tests\n");
    r += flash_mode_rw(HAL_NORFLASH_OP_MODE_STAND_SPI | HAL_NORFLASH_OP_MODE_READ_WRAP | HAL_NORFLASH_OP_MODE_PAGE_PROGRAM);
#endif

#ifdef FLH_TEST_DUAL_WRAP
    FLH_DEBUG("Flash DUAL wrap tests\n");
    r += flash_mode_rw(HAL_NORFLASH_OP_MODE_DUAL_IO | HAL_NORFLASH_OP_MODE_READ_WRAP | HAL_NORFLASH_OP_MODE_DUAL_PAGE_PROGRAM);
#endif

#ifdef FLH_TEST_QUAD_WRAP
    FLH_DEBUG("Flash QUAD wrap tests\n");
    r += flash_mode_rw(HAL_NORFLASH_OP_MODE_QUAD_IO | HAL_NORFLASH_OP_MODE_READ_WRAP | HAL_NORFLASH_OP_MODE_QUAD_PAGE_PROGRAM);
#endif

    if (!r) {
        hal_cmu_simu_pass();
        FLH_DEBUG("Flash test okay !!!\n");
    } else {
        hal_cmu_simu_fail();
        FLH_DEBUG("Flash test error ### %d\n", r);
    }
}
#endif /* FLH_MODE_TEST */

/******************************* flash speed test ********************************/
#ifdef FLH_SPEED_TEST
void flash_speed_test(void)
{
    const unsigned int cnt = 20;
    const unsigned char *data = (unsigned char *)0x20000000;
    const int len_array[] = { 0x8000, 0x10000, };
    int ret;
    unsigned int addr = flash_base_addr;
    unsigned int len;
    unsigned int k, m;
    uint32_t time;

    static const struct HAL_NORFLASH_CONFIG_T cfg = {
        .source_clk = HAL_NORFLASH_SPEED_13M * 2,
        .speed = HAL_NORFLASH_SPEED_13M,
        .mode  = HAL_NORFLASH_OP_MODE_QUAD_IO | HAL_NORFLASH_OP_MODE_QUAD_PAGE_PROGRAM | HAL_NORFLASH_OP_MODE_CONTINUOUS_READ,
        .override_config = 0,
    };

    ret = hal_norflash_open(flash_id, &cfg);

    ASSERT(ret == 0, "Failed to open norflash: %d", ret);

    for (m = 0; m < ARRAY_SIZE(len_array); m++) {
        len = len_array[m];

        time = hal_sys_timer_get();
        for (k = 0; k < cnt; k++) {
            hal_norflash_erase(flash_id, addr, len);
            addr += len;
        }
        time = (hal_sys_timer_get() - time) / cnt;
        TRACE(2,"Erase %d bytes: %d ms", len, TICKS_TO_MS(time));

        time = hal_sys_timer_get();
        for (k = 0; k < cnt; k++) {
            hal_norflash_write(flash_id, addr, data + (k & 1) * len, len);
            addr += len;
        }
        time = (hal_sys_timer_get() - time) / cnt;
        TRACE(2,"Burn %d bytes: %d ms", len, TICKS_TO_MS(time));

        time = hal_sys_timer_get();
        for (k = 0; k < cnt; k++) {
            hal_norflash_erase(flash_id, addr, len);
            hal_norflash_write(flash_id, addr, data + (k & 1) * len, len);
            addr += len;
        }
        time = (hal_sys_timer_get() - time) / cnt;
        TRACE(2,"Erase and burn %d bytes: %d ms", len, TICKS_TO_MS(time));
    }
}
#endif /* FLH_SPEED_TEST */


/******************************* flash factory test ********************************/
#ifdef FLH_FACTORY_TEST

#define FLASH_TICKS_TO_MS(tick) FAST_TICKS_TO_MS(tick)

uint32_t flash_get_timer_tick(uint32_t cur_tick)
{
    return hal_fast_sys_timer_get() - cur_tick;
}

#define FLASH_INFO_SET_LOC(info)   (*(volatile uint32_t *)(0x20000000) = (uint32_t)(info))
#define FLASH_INFO_ERRADDR_CNT     (128)

enum flash_case_id {
    FLASH_TC_ER  = (1 << 0),
    FLASH_TC_EWR = (1 << 1),
    FLASH_TC_SCRATCH = (1 << 2),
};

struct flash_info {
    uint32_t system_freq;//00
    uint32_t flash_freq; //04
    uint32_t total_size; //08
    uint32_t block_size; //0c
    uint32_t sector_size;//10
    uint32_t page_size;  //14
    uint32_t erase_time; //18
    uint32_t read_time;  //1c
    uint32_t write_time; //20
    uint32_t flashid;     //24
    uint32_t chipid;     //28
    uint32_t errcnt;     //2c
    uint32_t caseid;     //30
    uint32_t reserved[3];//34,38,3c
    uint32_t erraddr[FLASH_INFO_ERRADDR_CNT]; //40
    uint32_t errdata[FLASH_INFO_ERRADDR_CNT];
};

struct flash_info bes_info = {0};

void flash_info_init(uint32_t chipid, uint32_t sysclk, uint32_t flashclk)
{
    int i;
    struct flash_info *info = &bes_info;

    info->system_freq = sysclk;
    info->flash_freq  = flashclk;
    info->total_size  = 0;
    info->block_size  = 0;
    info->sector_size = 0;
    info->page_size   = 0;
    info->erase_time  = 0;
    info->read_time   = 0;
    info->write_time  = 0;
    info->flashid     = 0;
    info->chipid      = chipid;
    info->errcnt      = 0;
    info->caseid      = 0;

    for(i = 0;i < FLASH_INFO_ERRADDR_CNT; i++) {
        info->erraddr[i] = 0;
        info->errdata[i] = 0;
    }
    FLASH_INFO_SET_LOC(info);
}

int flash_info_set_error(uint32_t erraddr, uint32_t errdata)
{
    struct flash_info *info = &bes_info;

    if (info->errcnt < FLASH_INFO_ERRADDR_CNT) {
        info->erraddr[info->errcnt] = erraddr;
        info->errdata[info->errcnt] = errdata;
        info->errcnt++;
    }
    return (FLASH_INFO_ERRADDR_CNT - info->errcnt);
}

void flash_info_set_case_id(uint32_t id)
{
    bes_info.caseid |= id;
}

int flash_er_test(void)
{
    uint32_t addr, i, cur_time;
    int ret;
    struct flash_info *info = &bes_info;
    uint32_t total_size, block_size, sector_size, page_size, flashid;

    TRACE(2,"%s satrt, info at(%8x)", __func__, (uint32_t)info);

    //clear I/D cache
    hal_cache_disable(HAL_CACHE_ID_I_CACHE);
    hal_cache_disable(HAL_CACHE_ID_D_CACHE);
    hal_cache_enable(HAL_CACHE_ID_I_CACHE);
    hal_cache_enable(HAL_CACHE_ID_D_CACHE);

    init_flash();

    flashid = 0;
    hal_norflash_get_id(flash_id, (uint8_t *)&flashid, sizeof(flashid));
    TRACE(1,"flash id: %x", flashid);

    hal_norflash_get_size(flash_id, &total_size,
                          &block_size, &sector_size, &page_size);
    TRACE(4,"flash info: total: %d KB, block: %d KB, sector: %d KB, page: %d B",
            total_size/1024, block_size/1024, sector_size/1024, page_size);

    info->total_size  = total_size;
    info->block_size  = block_size;
    info->sector_size = sector_size;
    info->page_size   = page_size;
    info->flashid     = flashid;
    flash_info_set_case_id(FLASH_TC_ER);

#if 0
    //check flash size == 4 MB
    if (total_size != (4<<20)) {
        info->errcnt = (1<<16);
        ret = -1;
        TRACE(1,"norflash total size error %d", ret);
        goto _err;
    }
#endif

#if 1
    // erase norflash chip
    cur_time = flash_get_timer_tick(0);
    ret = hal_norflash_erase(flash_id, flash_base_addr, total_size);
    if (ret) {
        TRACE(1,"erase norflash error %d", ret);
        goto _err;
    }
    cur_time = flash_get_timer_tick(cur_time);
    info->erase_time = FLASH_TICKS_TO_MS(cur_time);
    TRACE(1,"erase norflash cost: %d ms", FLASH_TICKS_TO_MS(cur_time));
#endif

    hal_cache_disable(HAL_CACHE_ID_I_CACHE);
    hal_cache_disable(HAL_CACHE_ID_D_CACHE);
    hal_cache_enable(HAL_CACHE_ID_I_CACHE);
    hal_cache_enable(HAL_CACHE_ID_D_CACHE);

#if 0
    // write data to norflash
    cur_time = flash_get_timer_tick(0);
    ret = hal_norflash_write(flash_id, flash_base_addr, (uint8_t *)flash_buf, sizeof(flash_buf));
    if (ret) {
        TRACE(1,"write norflash error %d", ret);
        goto _err;
    }
    cur_time = flash_get_timer_tick(cur_time);
    info->write_time = FLASH_TICKS_TO_MS(cur_time);
    TRACE(1,"write norflash cost: %d ms", FLASH_TICKS_TO_MS(cur_time));
#endif

    // read norflash chip
    cur_time = flash_get_timer_tick(0);
    TRACE(2,"norflash range: %8x ~ %8x", flash_base_addr, flash_base_addr + total_size);
    i = 0;
    for (addr = flash_base_addr; addr < flash_base_addr + total_size; addr +=4) {
        uint32_t temp = *(uint32_t *)addr;

        if (temp != 0xFFFFFFFF) {
            if (i < FLASH_INFO_ERRADDR_CNT) {
                info->erraddr[i] = addr;
                info->errdata[i] = temp;
                info->errcnt = i;;
            }
            i++;
            TRACE(2,"norflash data error at %8X = %8X should be FFFFFFFF",
                addr, temp);
        }
    }
    cur_time = flash_get_timer_tick(cur_time);
    info->read_time = FLASH_TICKS_TO_MS(cur_time);
    TRACE(1,"read norflash cost: %d ms", FLASH_TICKS_TO_MS(cur_time));

    TRACE(0,"norflash test done !!!");
    return 0;
_err:
    TRACE(1,"norflash test error ### %d", ret);
    return ret;
}

#define FLASH_SECTOR_SIZE (4096 * 8)
#define FLASH_PAGE_SIZE   256
static uint32_t flash_sector_buf[FLASH_SECTOR_SIZE / 4];

int flash_ewr_test(void)
{
    uint32_t addr, i, j, k, cur_time;
    int ret;
    struct flash_info *info = &bes_info;
    uint32_t total_size, block_size, sector_size, page_size, flashid;

    TRACE(2,"%s satrt, info at(%8x)", __func__, (uint32_t)info);

    //clear I/D cache
    hal_cache_disable(HAL_CACHE_ID_I_CACHE);
    hal_cache_disable(HAL_CACHE_ID_D_CACHE);
    hal_cache_enable(HAL_CACHE_ID_I_CACHE);
    hal_cache_enable(HAL_CACHE_ID_D_CACHE);

    init_flash();

    flashid = 0;
    hal_norflash_get_id(flash_id, (uint8_t *)&flashid, sizeof(flashid));
    TRACE(1,"flash id: %x", flashid);

    hal_norflash_get_size(flash_id, &total_size,
                          &block_size, &sector_size, &page_size);
    TRACE(4,"flash info: total: %d KB, block: %d KB, sector: %d KB, page: %d B",
            total_size/1024, block_size/1024, sector_size/1024, page_size);

    info->total_size  = total_size;
    info->block_size  = block_size;
    info->sector_size = sector_size;
    info->page_size   = page_size;
    info->flashid     = flashid;

    flash_info_set_case_id(FLASH_TC_ER);

    hal_cache_disable(HAL_CACHE_ID_I_CACHE);
    hal_cache_disable(HAL_CACHE_ID_D_CACHE);
    hal_cache_enable(HAL_CACHE_ID_I_CACHE);
    hal_cache_enable(HAL_CACHE_ID_D_CACHE);

    // erase norflash chip
    cur_time = flash_get_timer_tick(0);
    ret = hal_norflash_erase(flash_id, flash_base_addr, total_size);
    if (ret) {
        TRACE(1,"erase norflash error %d", ret);
        goto _err;
    }
    cur_time = flash_get_timer_tick(cur_time);
    info->erase_time = FLASH_TICKS_TO_MS(cur_time);
    TRACE(1,"erase norflash cost: %d ms", FLASH_TICKS_TO_MS(cur_time));

    //flush I/D cache
    hal_cache_disable(HAL_CACHE_ID_I_CACHE);
    hal_cache_disable(HAL_CACHE_ID_D_CACHE);
    hal_cache_enable(HAL_CACHE_ID_I_CACHE);
    hal_cache_enable(HAL_CACHE_ID_D_CACHE);

    uint32_t *pbuf;

    // fill buffer data
    TRACE(0,"fill data ...");
    cur_time = flash_get_timer_tick(0);
    pbuf = flash_sector_buf;
    for (i = 0; i < NORFLASH_API_SECTOR_SIZE/FLASH_PAGE_SIZE; i += 8) {
        for(k = 0; k < 4; k++) {
            for(j = 0; j < FLASH_PAGE_SIZE/4; j+=2) {
                pbuf[j+0] = 0xFFFFFFFF;
                pbuf[j+1] = 0x00000000;
            }
            pbuf += FLASH_PAGE_SIZE/4;
        }
        for(k = 0; k < 4; k++) {
            for(j = 0; j < FLASH_PAGE_SIZE/4; j+=2) {
                pbuf[j+0] = 0x00000000;
                pbuf[j+1] = 0xFFFFFFFF;
            }
            pbuf += FLASH_PAGE_SIZE/4;
        }
    }

#if 0
    // dump buffer data
    TRACE(0,"dump data ...");
    pbuf = flash_sector_buf;
    for(i = 0; i < FLASH_SECTOR_SIZE/4;i+=8) {
        TRACE(8,"%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X",
            pbuf[i+0],pbuf[i+1],pbuf[i+2],pbuf[i+3],
            pbuf[i+4],pbuf[i+5],pbuf[i+6],pbuf[i+7]);
            hal_sys_timer_delay(MS_TO_TICKS(50));
    }
#endif

//#define SKIP_VAL 4

    // write data to flash
    TRACE(0,"write flash ...");
    pbuf = flash_sector_buf;
    for (i = 0; i < total_size / NORFLASH_API_SECTOR_SIZE; i++) {
#ifdef SKIP_VAL
        if (((i+1) % SKIP_VAL) == 0) {
#endif
            ret = hal_norflash_write(flash_id, flash_base_addr + (i * NORFLASH_API_SECTOR_SIZE),
                    (uint8_t *)pbuf, NORFLASH_API_SECTOR_SIZE);
            if (ret) {
                TRACE(1,"write norflash error %d", ret);
                goto _err;
            }
#ifdef SKIP_VAL
        }
#endif
    }
    cur_time = flash_get_timer_tick(cur_time);
    info->write_time = FLASH_TICKS_TO_MS(cur_time);
    TRACE(1,"write norflash cost: %d ms", FLASH_TICKS_TO_MS(cur_time));

    TRACE(0,"check data ...");
    cur_time = flash_get_timer_tick(0);
    TRACE(2,"norflash range: %8x ~ %8x", flash_base_addr, flash_base_addr + total_size);
    // check data
    k = 1;
    for (addr = flash_base_addr; addr < flash_base_addr + total_size; addr += NORFLASH_API_SECTOR_SIZE) {
        uint32_t *ps = flash_sector_buf;
        uint32_t *pd = (uint32_t *)addr;
#ifdef SKIP_VAL
        if ((k % SKIP_VAL) == 0) {
#endif
            for (i = 0; i < NORFLASH_API_SECTOR_SIZE/4; i++) {
                if (pd[i] != ps[i]) {
                    flash_info_set_error((uint32_t)&pd[i], pd[i]);
                    TRACE(3,"norflash data error at %8X = %8X should be %8X",
                        (uint32_t)&pd[i], pd[i], ps[i]);
                }
            }
#ifdef SKIP_VAL
        }
#endif
        k++;
    }

    cur_time = flash_get_timer_tick(cur_time);
    info->read_time = FLASH_TICKS_TO_MS(cur_time);
    TRACE(1,"read norflash cost: %d ms", FLASH_TICKS_TO_MS(cur_time));

    TRACE(0,"norflash test done !!!");
    return 0;
_err:
    TRACE(1,"norflash test error ### %d", ret);
    return ret;
}

/* flash scratch test */
#define FLASH_DATA_SECTOR_SIZE 4096
#define FLASH_DATA_PAGE_SIZE 256
#define FLASH_DATA_PAGE_NUM  8
#define FLASH_DATA_PATT_NUM  4
#define FLASH_DATA_PATT_LEN  64

uint8_t flash_data_pattern[FLASH_DATA_PAGE_NUM][FLASH_DATA_PATT_NUM] = {
    {0x55, 0xAA, 0x55, 0xAA}, //page 0
    {0xAA, 0x55, 0xAA, 0x55}, //page 1
    {0xAA, 0x55, 0xAA, 0x55}, //page 2
    {0x55, 0xAA, 0x55, 0xAA}, //page 3
    {0x55, 0xAA, 0x55, 0xAA}, //page 4
    {0xAA, 0x55, 0xAA, 0x55}, //page 5
    {0xAA, 0x55, 0xAA, 0x55}, //page 6
    {0x55, 0xAA, 0x55, 0xAA}, //page 7
};

uint8_t flash_page_in[FLASH_DATA_PAGE_NUM][FLASH_DATA_PAGE_SIZE];
uint8_t flash_page_out[FLASH_DATA_PAGE_NUM][FLASH_DATA_PAGE_SIZE];
uint8_t flash_sector_out[FLASH_DATA_SECTOR_SIZE * 2];

int flash_scratch_test(void)
{
    uint8_t *pattern;
    uint8_t *page;
    uint32_t i,j,k;
    int ret;
    uint32_t errcnt = 0;
    uint32_t flashid = 0, total_size, block_size, sector_size, page_size;
    struct flash_info *info = &bes_info;
    uint32_t cur_time;

    TRACE(2,"%s satrt, info at(%8x)", __func__, (uint32_t)info);

    cur_time = flash_get_timer_tick(0);

    // init flash
    init_flash();

    // disable flash
#if 1
    hal_cache_disable(HAL_CACHE_ID_I_CACHE);
    hal_cache_disable(HAL_CACHE_ID_D_CACHE);
#else
    hal_cache_enable(HAL_CACHE_ID_I_CACHE, 0);
    hal_cache_enable(HAL_CACHE_ID_D_CACHE, 0);
#endif

    // read flash info
    hal_norflash_get_id(flash_id, (uint8_t *)&flashid, sizeof(flashid));
    TRACE(1,"flash id: %x", flashid);

    hal_norflash_get_size(flash_id, &total_size,
                         &block_size, &sector_size, &page_size);

    TRACE(4,"flash info: total: %d KB, block: %d KB, sector: %d KB, page: %d B",
            total_size/1024, block_size/1024, sector_size/1024, page_size);

    info->total_size  = total_size;
    info->block_size  = block_size;
    info->sector_size = sector_size;
    info->page_size   = page_size;
    info->flashid     = flashid;

    flash_info_set_case_id(FLASH_TC_SCRATCH);

    // fill patterns
    for (i = 0; i < FLASH_DATA_PAGE_NUM; i++) {
        page = &flash_page_in[i][0];
        pattern = &flash_data_pattern[i][0];
        for(k = 0; k < FLASH_DATA_PATT_LEN; k++)
            page[k] = pattern[0];

        page += FLASH_DATA_PATT_LEN;
        for(k = 0; k < FLASH_DATA_PATT_LEN; k++)
            page[k] = pattern[1];

        page += FLASH_DATA_PATT_LEN;
        for(k = 0; k < FLASH_DATA_PATT_LEN; k++)
            page[k] = pattern[2];

        page += FLASH_DATA_PATT_LEN;
        for(k = 0; k < FLASH_DATA_PATT_LEN; k++)
            page[k] = pattern[3];
    }

#if 0
    // dump data
    for (i = 0; i < FLASH_DATA_PAGE_NUM; i++) {
        page = &flash_page_in[i][0];
        k = 0;
        TRACE(1,"\npage[%d]", i);
        for (j = 0; j < FLASH_DATA_PAGE_SIZE; j += 8, k+= 8) {
            TRACE(8,"%02X %02X %02X %02X %02X %02X %02X %02X",
                page[k+0],
                page[k+1],
                page[k+2],
                page[k+3],
                page[k+4],
                page[k+5],
                page[k+6],
                page[k+7]);
        }
        hal_sys_timer_delay(MS_TO_TICKS(100));
    }
#endif
    // write page to flash
    for (i = 0; i < FLASH_DATA_PAGE_NUM; i++) {
        uint32_t addr = flash_base_addr + (i * FLASH_DATA_PAGE_SIZE);
        page = &flash_page_in[i][0];

        TRACE(2,"write page[%d], size=%d B", i, FLASH_DATA_PAGE_SIZE);

         ret = hal_norflash_write(flash_id, addr, (uint8_t *)page, FLASH_DATA_PAGE_SIZE);
        if (ret) {
            TRACE(2,"write error at page[%d], %d", i, ret);
            goto _err;
        }
    }

    // read page from flash
    for (i = 0; i < FLASH_DATA_PAGE_NUM; i++) {
        uint32_t addr = flash_base_addr + (i * FLASH_DATA_PAGE_SIZE);
        page = &flash_page_out[i][0];

        TRACE(2,"read page[%d], size=%d B", i, FLASH_DATA_PAGE_SIZE);

         ret = hal_norflash_read(flash_id, addr, (uint8_t *)page, FLASH_DATA_PAGE_SIZE);
        if (ret) {
            TRACE(2,"read error at page[%d], %d", i, ret);
            goto _err;
        }
    }

    //check page data
    k = 0;
    for (i = 0; i < FLASH_DATA_PAGE_NUM; i++) {
        page = &flash_page_out[i][0];
        for (j = 0; j < FLASH_DATA_PAGE_SIZE; j++) {
            if (page[j] != flash_page_in[i][j]) {
                flash_info_set_error((uint32_t)&(flash_page_in[i][j]),
                                        flash_page_in[i][j]);
                errcnt++;
                k++;
                TRACE(5,"page[%d][%d] data error at %8x = %2x, should be %2x",
                    i, j, (flash_base_addr + i * FLASH_DATA_PAGE_SIZE + j),
                    page[j], flash_page_in[i][j]);
            }
        }
    }

    if (k) {
        TRACE(2,"%s, page test error ### %d", __func__, k);
    } else {
        TRACE(1,"%s, page test pass", __func__);
    }

    // erase sector 1
    ret = hal_norflash_erase(flash_id, flash_base_addr, sector_size);
    if (ret) {
        TRACE(1,"erase norflash error %d", ret);
        goto _err;
    }

    // read sector 1
    uint8_t *sector_buf = flash_sector_out;
    ret = hal_norflash_read(flash_id, flash_base_addr, sector_buf, sector_size);
    if (ret) {
        TRACE(2,"%s sector read error %d", __func__, ret);
        goto _err;
    }

    // check sector data after erase
    k = 0;
    for(i = 0; i < sector_size; i++) {
        if (sector_buf[i] != 0xFF) {
            flash_info_set_error((uint32_t)&(sector_buf[i]), sector_buf[i]);
            errcnt++;
            k++;
            TRACE(2,"sector data error at %8x = %2x, should be 0xFF",
                    (flash_base_addr + i), sector_buf[i]);
        }
    }
    if (k) {
        TRACE(2,"%s, sector test error ### %d", __func__, k);
    } else {
        TRACE(1,"%s, sector test pass", __func__);
    }

    cur_time = flash_get_timer_tick(cur_time);
    info->read_time = TICKS_TO_MS(cur_time);
    TRACE(1,"scratch test cost: %d ms", TICKS_TO_MS(cur_time));

    if (!errcnt) {
        TRACE(1,"%s, test okay !!!", __func__);
        return 0;
    }
_err:
    TRACE(2,"%s, test error ### %d", __func__, errcnt);
    return -1;
}

int flash_factory_test(void)
{
    uint32_t sys_freq = 52000000;
    int err = 0;

    hal_iomux_set_analog_i2c();

    hal_cmu_simu_init();

    TRACE(1,"%s start", __func__);

//    hal_sysfreq_req(HAL_SYSFREQ_USER_INIT, HAL_CMU_FREQ_52M);
//    sys_freq = hal_sys_timer_calc_cpu_freq(5,0);
//    TRACE(1,"cpu: %d HZ", sys_freq);

    flash_info_init(0x18E2, sys_freq, 208000000);

#ifdef FLH_TEST_ER
    err += flash_er_test();
#endif
#ifdef FLH_TEST_EWR
    err += flash_ewr_test();
#endif
#ifdef FLH_TEST_CRATCH
    err += flash_scratch_test();
#endif
    if (!err) {
        TRACE(1,"%s, test okay !\n", __func__);
        hal_cmu_simu_pass();
        return 0;
    }

    hal_cmu_simu_fail();
    TRACE(1,"%s, test error ###\n", __func__);
    return -1;
}
#endif /* FLH_FACTORY_TEST */

#ifdef FLH_SUSPEND_TEST
static HWTIMER_ID hw_timer = NULL;

static uint32_t timer_cnt, total_timer_cnt;

static const uint32_t timer_interval_ms = 2;

static void timer_handler(void *param)
{
    hwtimer_start(hw_timer, MS_TO_TICKS(timer_interval_ms));
    timer_cnt++;
    total_timer_cnt++;
    if ((total_timer_cnt % 1000) == 0) {
        TR_INFO(0, "total_timer_cnt=%u", total_timer_cnt);
    }
}

int flash_suspend_test(void)
{
    uint32_t addr = FLASH_NC_BASE + 0x20000;
    uint32_t len = sizeof(flash_src_buf);
    uint32_t max_suspend_cnt = 10000;
    enum HAL_NORFLASH_RET_T nret = HAL_NORFLASH_OK;
    uint32_t lock;
    uint32_t i;
    uint32_t suspend_cnt;
    uint8_t loop;

    hw_timer = hwtimer_alloc(timer_handler, 0);
    hwtimer_start(hw_timer, MS_TO_TICKS(timer_interval_ms));
    TR_INFO(0, "Start timer %u ms", timer_interval_ms);

    loop = 1;
    while (loop) {
        suspend_cnt = 0;
_erase:
        lock = int_lock();
        timer_cnt = 0;
        if (nret == HAL_NORFLASH_SUSPENDED) {
            nret = hal_norflash_erase_resume(flash_id, true);
        } else {
            nret = hal_norflash_erase_suspend(flash_id, addr, len, true);
        }
        int_unlock(lock);
        if (nret == HAL_NORFLASH_SUSPENDED) {
            suspend_cnt++;
            if (suspend_cnt == 1 || (suspend_cnt % 1000) == 0) {
                TR_INFO(0, "[%u] erase suspended: timer_cnt=%u suspend_cnt=%u", loop, timer_cnt, suspend_cnt);
            }
            ASSERT(suspend_cnt <= max_suspend_cnt, "[%u] erase timeout: suspend_cnt=%u", loop, suspend_cnt);
            goto _erase;
        }
        ASSERT(nret == HAL_NORFLASH_OK, "Failed to erase: %d", nret);
        TR_INFO(0, "[%u] erase ok: timer_cnt=%u suspend_cnt=%u", loop, timer_cnt, suspend_cnt);
        for (i = 0; i < sizeof(flash_src_buf); i++) {
            flash_src_buf[i] = loop;
        }
        suspend_cnt = 0;
_write:
        lock = int_lock();
        timer_cnt = 0;
        if (nret == HAL_NORFLASH_SUSPENDED) {
            nret = hal_norflash_write_resume(flash_id, true);
        } else {
            nret = hal_norflash_write_suspend(flash_id, addr, flash_src_buf, len, true);
        }
        int_unlock(lock);
        if (nret == HAL_NORFLASH_SUSPENDED) {
            suspend_cnt++;
            if (suspend_cnt == 1 || (suspend_cnt % 1000) == 0) {
                TR_INFO(0, "[%u] write suspended: timer_cnt=%u suspend_cnt=%u", loop, timer_cnt, suspend_cnt);
            }
            ASSERT(suspend_cnt <= max_suspend_cnt, "[%u] write timeout: suspend_cnt=%u", loop, suspend_cnt);
            goto _write;
        }
        ASSERT(nret == HAL_NORFLASH_OK, "Failed to write: %d", nret);
        TR_INFO(0, "[%u] write ok: timer_cnt=%u suspend_cnt=%u", loop, timer_cnt, suspend_cnt);
        for (i = 0; i < sizeof(flash_src_buf); i++) {
            uint8_t d;

            d = *(volatile uint8_t *)(addr + i);
            ASSERT(d == flash_src_buf[i], "[%u] verifed failed @ %u: flash=0x%02X buf=0x%02X",
                loop, i, d, flash_src_buf[i]);
        }
        TR_INFO(0, "[%u] verify ok: timer_cnt=%u", loop, timer_cnt);
        loop++;
    }

    hwtimer_stop(hw_timer);
    TR_INFO(0, "flash suspend test done");

    return 0;
}
#endif

#ifdef FLH_SIMPLE_TEST
SRAM_TEXT_LOC void flash_simple_test(void)
{
    uint32_t total_size, block_size, sector_size, page_size, flashid;
    uint32_t flash_base = flash_nc_base_addr;
    uint8_t *flash_p = (uint8_t *)flash_base;
    uint32_t flash_base1 = flash_nc_base_addr + (1 << 24);
    uint8_t *flash_p1 = (uint8_t *)flash_base1;
    int i;
    uint8_t buf[256];
    uint32_t lock;
    enum HAL_NORFLASH_RET_T ret;

    lock = int_lock();

    //init_flash();

    flashid = 0;
    hal_norflash_get_id(flash_id, (uint8_t *)&flashid, sizeof(flashid));
    TRACE(1,"flash id: %x", flashid);

    hal_norflash_get_size(flash_id, &total_size,
                          &block_size, &sector_size, &page_size);
    TRACE(4,"flash info: total: %d KB, block: %d KB, sector: %d KB, page: %d B",
            total_size/1024, block_size/1024, sector_size/1024, page_size);

    TRACE_FLUSH();

#if 0
    hal_norflash_erase(HAL_FLASH_ID_0, flash_base, sector_size);
    if (total_size > (1 << 24)) {
        hal_norflash_erase(HAL_FLASH_ID_0, flash_base1, sector_size);
    }
    return;
#endif

    ret = hal_norflash_erase(flash_id, flash_base, sector_size);
    ASSERT(ret == HAL_NORFLASH_OK, "%s/%u: erase error: %d. addr=0x%08X len=0x%08X", __func__, __LINE__, ret, flash_base, sector_size);
    TRACE(0, "After erase 0x%08X", flash_base);
    TRACE(0, "Dump %p", flash_p);
    for (i = 0; i + 16 <= sizeof(buf); i += 16) {
        DUMP8("%02X ", &flash_p[i], 16);
    }
    if (total_size > (1 << 24)) {
        TRACE(0, "Dump %p", flash_p1);
        for (i = 0; i + 16 <= sizeof(buf); i += 16) {
            DUMP8("%02X ", &flash_p1[i], 16);
        }
    }
    TRACE_FLUSH();
    TRACE(0, "NorflashRead 0x%08X", flash_base);
    ret = hal_norflash_read(flash_id, flash_base, buf, sizeof(buf));
    ASSERT(ret == HAL_NORFLASH_OK, "%s/%u: read error: %d. addr=0x%08X len=0x%08X", __func__, __LINE__, ret, flash_base, sizeof(buf));
    for (i = 0; i + 16 <= sizeof(buf); i += 16) {
        DUMP8("%02X ", &buf[i], 16);
    }
    TRACE_FLUSH();

    for (i = 0; i < sizeof(buf); i++) {
        buf[i] = i;
    }
    ret = hal_norflash_write(flash_id, flash_base, buf, sizeof(buf));
    ASSERT(ret == HAL_NORFLASH_OK, "%s/%u: write error: %d. addr=0x%08X len=0x%08X", __func__, __LINE__, ret, flash_base, sizeof(buf));
    TRACE(0, "After write 0x%08X", flash_base);
    TRACE(0, "Dump %p", flash_p);
    for (i = 0; i + 16 <= sizeof(buf); i += 16) {
        DUMP8("%02X ", &flash_p[i], 16);
    }
    if (total_size > (1 << 24)) {
        TRACE(0, "Dump %p", flash_p1);
        for (i = 0; i + 16 <= sizeof(buf); i += 16) {
            DUMP8("%02X ", &flash_p1[i], 16);
        }
    }
    TRACE_FLUSH();
    TRACE(0, "NorflashRead 0x%08X", flash_base);
    ret = hal_norflash_read(flash_id, flash_base, buf, sizeof(buf));
    ASSERT(ret == HAL_NORFLASH_OK, "%s/%u: read error: %d. addr=0x%08X len=0x%08X", __func__, __LINE__, ret, flash_base, sizeof(buf));
    for (i = 0; i + 16 <= sizeof(buf); i += 16) {
        DUMP8("%02X ", &buf[i], 16);
    }
    TRACE_FLUSH();

    if (total_size > (1 << 24)) {
        for (i = 0; i < sizeof(buf); i++) {
            buf[i] = ~i;
        }
        ret = hal_norflash_erase(flash_id, flash_base1, sector_size);
        ASSERT(ret == HAL_NORFLASH_OK, "%s/%u: erase error: %d. addr=0x%08X len=0x%08X", __func__, __LINE__, ret, flash_base1, sector_size);
        TRACE(0, "After erase 0x%08X", flash_base1);
        TRACE(0, "Dump %p", flash_p);
        for (i = 0; i + 16 <= sizeof(buf); i += 16) {
            DUMP8("%02X ", &flash_p[i], 16);
        }
        TRACE(0, "Dump %p", flash_p1);
        for (i = 0; i + 16 <= sizeof(buf); i += 16) {
            DUMP8("%02X ", &flash_p1[i], 16);
        }
        TRACE_FLUSH();

        for (i = 0; i < sizeof(buf); i++) {
            buf[i] = i;
        }
        ret = hal_norflash_write(flash_id, flash_base1, buf, sizeof(buf));
        ASSERT(ret == HAL_NORFLASH_OK, "%s/%u: write error: %d. addr=0x%08X len=0x%08X", __func__, __LINE__, ret, flash_base1, sizeof(buf));
        TRACE(0, "After write 0x%08X", flash_base1);
        TRACE(0, "Dump %p", flash_p);
        for (i = 0; i + 16 <= sizeof(buf); i += 16) {
            DUMP8("%02X ", &flash_p[i], 16);
        }
        TRACE(0, "Dump %p", flash_p1);
        for (i = 0; i + 16 <= sizeof(buf); i += 16) {
            DUMP8("%02X ", &flash_p1[i], 16);
        }
        TRACE_FLUSH();
    }

    int_unlock(lock);

    hal_cmu_simu_pass();
    while (1) {}
}
#endif

// FLASH_TEST
void flash_test(void)
{
    TRACE(1,"%s, start", __func__);

#ifdef FPGA
    hal_cache_enable(HAL_CACHE_ID_I_CACHE);
    hal_cache_enable(HAL_CACHE_ID_D_CACHE);
    hal_cache_wrap_enable(HAL_CACHE_ID_I_CACHE);
    hal_cache_wrap_enable(HAL_CACHE_ID_D_CACHE);
    hal_cache_writebuffer_enable(HAL_CACHE_ID_D_CACHE);
    hal_cache_writeback_enable(HAL_CACHE_ID_D_CACHE);
#endif

#ifdef FLH_SIMPLE_TEST
    flash_simple_test();
#endif
#ifdef FLH_MODE_TEST
    flash_more_cases();
#endif
#ifdef FLH_SPEED_TEST
    flash_speed_test();
#endif
#ifdef FLH_FACTORY_TEST
    flash_factory_test();
#endif
#ifdef FLH_SUSPEND_TEST
    flash_suspend_test();
#endif
}
