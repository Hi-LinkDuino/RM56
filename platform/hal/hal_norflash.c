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
#include "plat_types.h"
#include "plat_addr_map.h"
#include "cmsis.h"
#include "hal_cache.h"
#include "hal_cmu.h"
#include "hal_location.h"
#include "hal_norflaship.h"
#include "hal_norflash.h"
#include "hal_bootmode.h"
#include "hal_sysfreq.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "norflash_cfg.h"
#include "norflash_drv.h"
#include "pmu.h"
#include "string.h"

/* Demo:
 *
 *  uint8_t data[1024];
 *  hal_norflash_open(HAL_FLASH_ID_0, HAL_NORFLASH_SPEED_26M, 0);
 *  \/\/ hal_norflash_open(HAL_FLASH_ID_0, HAL_NORFLASH_SPEED_13M, HAL_NORFLASH_OP_MODE_QUAD);
 *  \/\/ hal_norflash_open(HAL_FLASH_ID_0, HAL_NORFLASH_SPEED_13M, HAL_NORFLASH_OP_MODE_QUAD|HAL_NORFLASH_OP_MODE_CONTINUOUS_READ);
 *  hal_norflash_erase(HAL_I2C_ID_0, 0, 4096);
 *  memset(data, 0xcc, 1024);
 *  hal_norflash_write(HAL_I2C_ID_0, 0, data, 1024);
 *  for (i = 0; i < 10; ++i) {
 *      TRACE(2,"[0x%x] - 0x%x\n", 0x08000000 + i, *((volatile uint8_t *)(0x08000000 + i)));
 *  }
*/

#if defined(FLASH_LOCK_CP_ACCESS) && (CHIP_FLASH_CTRL_VER >= 4)
#define FLASH_MEM_READ_BUS_LOCK
#define FLASH_SEC_REG_FIFO_READ
#endif

#define HAL_NORFLASH_CP_ID_LEN              2

#define HAL_NORFLASH_YES                    1
#define HAL_NORFLASH_NO                     0

// Protect lower 64KB by default
#ifndef FLASH_BLOCK_PROTECTION_256M_BIT
#define FLASH_BLOCK_PROTECTION_256M_BIT     0x0044
#endif
// Protect lower 32KB by default
#ifndef FLASH_BLOCK_PROTECTION_128M_BIT
#define FLASH_BLOCK_PROTECTION_128M_BIT     0x0078
#endif
// Protect lower 1/2 by default
#ifndef FLASH_BLOCK_PROTECTION_64M_BIT
#define FLASH_BLOCK_PROTECTION_64M_BIT      0x0034
#endif
#ifndef FLASH_BLOCK_PROTECTION_32M_BIT
#define FLASH_BLOCK_PROTECTION_32M_BIT      0x0038
#endif
#ifndef FLASH_BLOCK_PROTECTION_16M_BIT
#define FLASH_BLOCK_PROTECTION_16M_BIT      0x0034
#endif
#ifndef FLASH_BLOCK_PROTECTION_8M_BIT
#define FLASH_BLOCK_PROTECTION_8M_BIT       0x0030
#endif

struct HAL_Norflash_Context {
    bool opened;
    uint8_t device_id[HAL_NORFLASH_DEVICE_ID_LEN];
#ifdef FLASH_UNIQUE_ID
    uint8_t unique_id[HAL_NORFLASH_UNIQUE_ID_LEN + HAL_NORFLASH_CP_ID_LEN];
#endif
    uint32_t total_size;
    uint32_t block_size;
    uint32_t sector_size;
    uint32_t page_size;
    enum HAL_NORFLASH_RET_T open_state;
};

static struct HAL_Norflash_Context norflash_ctx[HAL_FLASH_ID_NUM];

static const char * const err_not_opened = "norflash not opened";

static const struct HAL_NORFLASH_CONFIG_T norflash_cfg = {
#if defined(FPGA) || defined(FLASH_ULTRA_LOW_SPEED)
    .source_clk = HAL_NORFLASH_SPEED_13M * 2,
    .speed = HAL_NORFLASH_SPEED_13M,
#elif defined(FLASH_LOW_SPEED)
    .source_clk = HAL_NORFLASH_SPEED_26M * 2,
    .speed = HAL_NORFLASH_SPEED_26M,
#elif defined(CHIP_BEST1400) || defined(CHIP_BEST1402)
    .source_clk = HAL_NORFLASH_SPEED_52M,
#ifdef FLASH_DTR
    .speed = HAL_NORFLASH_SPEED_26M,
#else
    .speed = HAL_NORFLASH_SPEED_52M,
#endif
#elif defined(OSC_26M_X4_AUD2BB)
    .source_clk = HAL_NORFLASH_SPEED_52M * 2,
    .speed = HAL_NORFLASH_SPEED_52M,
#else
    .source_clk = HAL_NORFLASH_SPEED_104M * 2,
    .speed = HAL_NORFLASH_SPEED_104M,
#endif
    .mode = HAL_NORFLASH_OP_MODE_STAND_SPI |
            HAL_NORFLASH_OP_MODE_FAST_SPI |
            HAL_NORFLASH_OP_MODE_DUAL_OUTPUT |
            HAL_NORFLASH_OP_MODE_DUAL_IO |
            HAL_NORFLASH_OP_MODE_QUAD_OUTPUT |
            HAL_NORFLASH_OP_MODE_QUAD_IO |
#ifdef FLASH_DTR
            HAL_NORFLASH_OP_MODE_DTR |
#endif
            HAL_NORFLASH_OP_MODE_CONTINUOUS_READ |
            HAL_NORFLASH_OP_MODE_READ_WRAP |
            HAL_NORFLASH_OP_MODE_PAGE_PROGRAM |
            HAL_NORFLASH_OP_MODE_DUAL_PAGE_PROGRAM |
            HAL_NORFLASH_OP_MODE_QUAD_PAGE_PROGRAM |
            0,
    .override_config = 0,
};

#ifdef FLASH_SUSPEND
enum SUSPEND_STATE_T {
    SUSPEND_STATE_NONE,
    SUSPEND_STATE_ERASE,
    SUSPEND_STATE_PROGRAM,
};

static enum SUSPEND_STATE_T suspend_state;
static uint32_t op_next_addr;
static const uint8_t *op_next_buf;
static uint32_t op_remain_len;
#endif

#ifdef FLASH_SECURITY_REGISTER
static uint32_t sec_reg_base;
static uint16_t sec_reg_size;
static uint16_t sec_reg_offset;
static uint16_t sec_reg_total_size;
static uint16_t sec_reg_pp_size;
static bool sec_reg_enabled;
#endif

enum HAL_CMU_FREQ_T hal_norflash_clk_to_cmu_freq(uint32_t clk)
{
    enum HAL_CMU_FREQ_T freq;

    if (clk >= HAL_NORFLASH_SPEED_208M) {
        freq = HAL_CMU_FREQ_208M;
    } else if (clk >= HAL_NORFLASH_SPEED_104M) {
        freq = HAL_CMU_FREQ_104M;
    } else if (clk >= HAL_NORFLASH_SPEED_78M) {
        freq = HAL_CMU_FREQ_78M;
    } else if (clk >= HAL_NORFLASH_SPEED_52M) {
        freq = HAL_CMU_FREQ_52M;
    } else {
        freq = HAL_CMU_FREQ_26M;
    }

#if defined(CHIP_BEST1000) && defined(AUD_PLL_DOUBLE)
    if (freq == HAL_CMU_FREQ_208M) {
        freq = HAL_CMU_FREQ_104M;
    }
#endif
#ifdef SIMU
#ifdef SIMU_FAST_FLASH
#define MAX_SIMU_FLASH_FREQ     HAL_CMU_FREQ_104M
#else
#define MAX_SIMU_FLASH_FREQ     HAL_CMU_FREQ_52M
#endif
    if (freq > MAX_SIMU_FLASH_FREQ) {
        freq = MAX_SIMU_FLASH_FREQ;
    }
#endif

    return freq;
}

enum HAL_NORFLASH_RET_T hal_norflash_get_size(enum HAL_FLASH_ID_T id, uint32_t *total_size,
                                              uint32_t *block_size, uint32_t *sector_size,
                                              uint32_t *page_size)
{
    ASSERT(norflash_ctx[id].opened, err_not_opened);

    if (total_size) {
        *total_size = norflash_ctx[id].total_size;
    }
    if (block_size) {
        *block_size = norflash_ctx[id].block_size;
    }
    if (sector_size) {
        *sector_size = norflash_ctx[id].sector_size;
    }
    if (page_size) {
        *page_size = norflash_ctx[id].page_size;
    }
    return HAL_NORFLASH_OK;
}

enum HAL_NORFLASH_RET_T hal_norflash_get_boundary(enum HAL_FLASH_ID_T id, uint32_t address,
                                                  uint32_t *block_boundary, uint32_t *sector_boundary)
{
    ASSERT(norflash_ctx[id].opened, err_not_opened);

    static const uint32_t flash_base[] = { FLASH_BASE, FLASHX_BASE, FLASH_NC_BASE, FLASHX_NC_BASE, };
    int i;

    for (i = 0; i < ARRAY_SIZE(flash_base); i++) {
        if (flash_base[i] <= address && address < flash_base[i] + norflash_ctx[id].total_size) {
            address -= flash_base[i];
            if (block_boundary)
                *block_boundary  = flash_base[i] + (address/norflash_ctx[id].block_size)*norflash_ctx[id].block_size;
            if (sector_boundary)
                *sector_boundary = flash_base[i] + (address/norflash_ctx[id].sector_size)*norflash_ctx[id].sector_size;
            break;
        }
    }

    if (i == ARRAY_SIZE(flash_base)) {
        return HAL_NORFLASH_BAD_ADDR;
    }

    return HAL_NORFLASH_OK;
}

enum HAL_NORFLASH_RET_T hal_norflash_get_id(enum HAL_FLASH_ID_T id, uint8_t *value, uint32_t len)
{
    len = MIN(len, sizeof(norflash_ctx[id].device_id));

    memcpy(value, norflash_ctx[id].device_id, len);

    return HAL_NORFLASH_OK;
}

#ifdef FLASH_UNIQUE_ID
enum HAL_NORFLASH_RET_T hal_norflash_get_unique_id(enum HAL_FLASH_ID_T id, uint8_t *value, uint32_t len)
{
    ASSERT(norflash_ctx[id].opened, err_not_opened);

    len = MIN(len, sizeof(norflash_ctx[id].unique_id));

    memcpy(value, norflash_ctx[id].unique_id, len);

    return HAL_NORFLASH_OK;
}
#endif

#ifdef FLASH_PROTECTION
static uint32_t hal_norflash_get_bp_cfg(enum HAL_FLASH_ID_T id)
{
    uint32_t val = 0;
#if defined(CHIP_HAS_EXT_NORFLASH)
if(id == HAL_FLASH_ID_1){
    return val;
}
#endif
    // Set BP and CMP bits
    // E.g., for 32M bit flash, BP=0b00010 CMP=1 can protect 0x000000 - 0x3DFFFF
    if (FLASH_SIZE > hal_norflash_get_flash_total_size(id)) {
        return val;
    }

    if (norflash_ctx[id].device_id[0] != 0xC8 &&        // GigaDevice
            norflash_ctx[id].device_id[0] != 0x85 &&    // Puya
            norflash_ctx[id].device_id[0] != 0x20       // Xinxin
            ) {
        return val;
    }

    if (FLASH_SIZE == 0x2000000) {
        val = FLASH_BLOCK_PROTECTION_256M_BIT;
    } else if (FLASH_SIZE == 0x1000000) {
            val = FLASH_BLOCK_PROTECTION_128M_BIT;
    } else if (FLASH_SIZE == 0x800000) {
        val = FLASH_BLOCK_PROTECTION_64M_BIT;
    } else if (FLASH_SIZE == 0x400000) {
        val = FLASH_BLOCK_PROTECTION_32M_BIT;
    } else if (FLASH_SIZE == 0x200000) {
        val = FLASH_BLOCK_PROTECTION_16M_BIT;
    } else if (FLASH_SIZE == 0x100000) {
        val = FLASH_BLOCK_PROTECTION_8M_BIT;
    } else {
        // CAUTION:
        // Different configuration for GD and Puya
    }

    return val;
}
#endif

#if defined(CONFIG_SMP)
extern void nuttx_smp_end_protection();
extern void nuttx_smp_start_protection();
#endif

enum HAL_NORFLASH_RET_T hal_norflash_enable_protection(enum HAL_FLASH_ID_T id)
{
    int ret = 0;

#ifdef FLASH_PROTECTION
    uint32_t lock;

    if (!norflash_ctx[id].opened) {
        return HAL_NORFLASH_NOT_OPENED;
    }

    lock = int_lock_global();

    norflash_pre_operation(id);

    ret = norflash_set_block_protection(id, hal_norflash_get_bp_cfg(id));

    norflash_post_operation(id);

    int_unlock_global(lock);
#endif

    return ret ? HAL_NORFLASH_BAD_OP : HAL_NORFLASH_OK;
}

enum HAL_NORFLASH_RET_T hal_norflash_disable_protection(enum HAL_FLASH_ID_T id)
{
    int ret = 0;

#ifdef FLASH_PROTECTION
    uint32_t lock;

    if (!norflash_ctx[id].opened) {
        return HAL_NORFLASH_NOT_OPENED;
    }

    lock = int_lock_global();

    norflash_pre_operation(id);

    ret = norflash_set_block_protection(id, 0);

    norflash_post_operation(id);

    int_unlock_global(lock);
#endif

    return ret ? HAL_NORFLASH_BAD_OP : HAL_NORFLASH_OK;
}

#ifdef FLASH_SECURITY_REGISTER
enum HAL_NORFLASH_RET_T _align_security_register_addr_len(uint32_t *addr, uint32_t *len)
{
    uint32_t remain_len;

    if (!sec_reg_enabled) {
        return HAL_NORFLASH_BAD_OP;
    }
    // Check address and length
    if (sec_reg_total_size <= *addr) {
        return HAL_NORFLASH_BAD_ADDR;
    }
    remain_len = sec_reg_total_size - *addr;
    if (*len > remain_len) {
        return HAL_NORFLASH_BAD_LEN;
    }
    // Align to register boundary
    remain_len = *addr & (sec_reg_size - 1);
    if (remain_len) {
        *addr -= remain_len;
        *len += remain_len;
    }
    remain_len = *len & (sec_reg_size - 1);
    if (remain_len) {
        *len += sec_reg_size - remain_len;
    }

    return HAL_NORFLASH_OK;
}

int hal_norflash_security_register_is_locked(enum HAL_FLASH_ID_T id, uint32_t start_address, uint32_t len)
{
    uint32_t remain_len;
    int ret = 0;
    uint32_t reg_base;
    uint32_t reg_pos;
    uint32_t pos;
    uint32_t lock_size;
    uint32_t reg_id;
    uint32_t id_map;
    uint32_t locked;

    ASSERT(norflash_ctx[id].opened, err_not_opened);

    ret = _align_security_register_addr_len(&start_address, &len);
    if (ret != HAL_NORFLASH_OK) {
        return 0;
    }

    pos = start_address;
    remain_len = len;

    reg_base = sec_reg_base;
    reg_pos = 0;
    reg_id = 0;
    id_map = 0;

    while (remain_len > 0) {
        if (reg_pos <= pos && pos < reg_pos + sec_reg_size) {
            lock_size = sec_reg_size - (pos - reg_pos);
            if (remain_len > lock_size) {
                remain_len -= lock_size;
            } else {
                remain_len = 0;
            }
            pos += lock_size;
            id_map |= (1 << reg_id);
        }
        reg_pos += sec_reg_size;
        reg_base += sec_reg_offset;
        reg_id++;
    }

    norflash_pre_operation(id);
    ret = norflash_security_register_get_lock_status(id, id_map, &locked);
    norflash_post_operation(id);

    if (ret) {
        return 0;
    }

    return !!locked;
}

enum HAL_NORFLASH_RET_T hal_norflash_security_register_lock(enum HAL_FLASH_ID_T id, uint32_t start_address, uint32_t len)
{
    uint32_t remain_len;
    int ret = 0;
    uint32_t reg_base;
    uint32_t reg_pos;
    uint32_t pos;
    uint32_t lock_size;
    uint32_t reg_id;
    uint32_t id_map;

    ASSERT(norflash_ctx[id].opened, err_not_opened);

    ret = _align_security_register_addr_len(&start_address, &len);
    if (ret != HAL_NORFLASH_OK) {
        return ret;
    }

    pos = start_address;
    remain_len = len;

    reg_base = sec_reg_base;
    reg_pos = 0;
    reg_id = 0;
    id_map = 0;

    while (remain_len > 0 && ret == 0) {
        if (reg_pos <= pos && pos < reg_pos + sec_reg_size) {
            lock_size = sec_reg_size - (pos - reg_pos);
            id_map |= (1 << reg_id);
            if (remain_len > lock_size) {
                remain_len -= lock_size;
            } else {
                remain_len = 0;
            }
            pos += lock_size;
        }
        reg_pos += sec_reg_size;
        reg_base += sec_reg_offset;
        reg_id++;
    }

    norflash_pre_operation(id);
    ret = norflash_security_register_lock(id, id_map);
    norflash_post_operation(id);

    return ret;
}

enum HAL_NORFLASH_RET_T hal_norflash_security_register_erase(enum HAL_FLASH_ID_T id, uint32_t start_address, uint32_t len)
{
    uint32_t remain_len;
    enum HAL_NORFLASH_RET_T ret = HAL_NORFLASH_OK;
    uint32_t reg_base;
    uint32_t reg_pos;
    uint32_t pos;
    uint32_t erase_size;

    ASSERT(norflash_ctx[id].opened, err_not_opened);

    ret = _align_security_register_addr_len(&start_address, &len);
    if (ret != HAL_NORFLASH_OK) {
        return ret;
    }

    pos = start_address;
    remain_len = len;

    reg_base = sec_reg_base;
    reg_pos = 0;
#if defined(CONFIG_SMP)
    nuttx_smp_start_protection();
#endif
    norflash_pre_operation(id);

#if (CHIP_FLASH_CTRL_VER >= 2)
    if (norflash_ctx[id].total_size > (1 << 24)) {
        norflash_exit_4byteaddr_mode(id);
    }
#endif

    while (remain_len > 0 && ret == HAL_NORFLASH_OK) {
        if (reg_pos <= pos && pos < reg_pos + sec_reg_size) {
            // erase a register
            erase_size = sec_reg_size - (pos - reg_pos);
            ret = norflash_security_register_erase(id, reg_base + (pos - reg_pos));
            if (remain_len > erase_size) {
                remain_len -= erase_size;
            } else {
                remain_len = 0;
            }
            pos += erase_size;
        }
        reg_pos += sec_reg_size;
        reg_base += sec_reg_offset;
    }

#if (CHIP_FLASH_CTRL_VER >= 2)
    if (norflash_ctx[id].total_size > (1 << 24)) {
        norflash_enter_4byteaddr_mode(id);
    }
#endif

    norflash_post_operation(id);
#if defined(CONFIG_SMP)
    nuttx_smp_end_protection();
#endif

    return ret;
}

enum HAL_NORFLASH_RET_T hal_norflash_security_register_write(enum HAL_FLASH_ID_T id, uint32_t start_address, const uint8_t *buffer, uint32_t len)
{
    const uint8_t *current_buffer;
    uint32_t remain_len;
    enum HAL_NORFLASH_RET_T ret = HAL_NORFLASH_OK;
    uint32_t reg_base;
    uint32_t reg_pos;
    uint32_t pos;
    uint32_t write_size;
    uint32_t each_write;
    uint32_t pp_remain;

    ASSERT(norflash_ctx[id].opened, err_not_opened);

    if (!sec_reg_enabled) {
        return HAL_NORFLASH_BAD_OP;
    }
    // Check address and length
    if (sec_reg_total_size <= start_address) {
        return HAL_NORFLASH_BAD_ADDR;
    }
    remain_len = sec_reg_total_size - start_address;
    if (len > remain_len) {
        return HAL_NORFLASH_BAD_LEN;
    }

    pos = start_address;
    current_buffer = buffer;
    remain_len = len;

    reg_base = sec_reg_base;
    reg_pos = 0;

    norflash_pre_operation(id);

#if (CHIP_FLASH_CTRL_VER >= 2)
    if (norflash_ctx[id].total_size > (1 << 24)) {
        norflash_exit_4byteaddr_mode(id);
    }
#endif

    while (remain_len > 0 && ret == HAL_NORFLASH_OK) {
        if (reg_pos <= pos && pos < reg_pos + sec_reg_size) {
            // write a register
            if (pos + remain_len <= reg_pos + sec_reg_size) {
                write_size = remain_len;
            } else {
                write_size = sec_reg_size - (pos - reg_pos);
            }
            remain_len -= write_size;
            while (write_size > 0 && ret == HAL_NORFLASH_OK) {
                if (write_size > sec_reg_pp_size) {
                    each_write = sec_reg_pp_size;
                } else {
                    each_write = write_size;
                }
                // Align to security register program page size
                pp_remain = sec_reg_pp_size - ((pos - reg_pos) & (sec_reg_pp_size - 1));
                if (each_write > pp_remain) {
                    each_write = pp_remain;
                }
                ret = norflash_security_register_write(id, reg_base + (pos - reg_pos), current_buffer, each_write);
                write_size -= each_write;
                pos += each_write;
                current_buffer += each_write;
            }
        }
        reg_pos += sec_reg_size;
        reg_base += sec_reg_offset;
    }

#if (CHIP_FLASH_CTRL_VER >= 2)
    if (norflash_ctx[id].total_size > (1 << 24)) {
        norflash_enter_4byteaddr_mode(id);
    }
#endif

    norflash_post_operation(id);

    return ret;
}

enum HAL_NORFLASH_RET_T hal_norflash_security_register_read(enum HAL_FLASH_ID_T id, uint32_t start_address, uint8_t *buffer, uint32_t len)
{
    uint8_t *current_buffer;
    uint32_t remain_len, read_size;
    int ret = 0;
    uint32_t reg_base;
    uint32_t reg_pos;
    uint32_t pos;
#ifdef FLASH_SEC_REG_FIFO_READ
    uint32_t each_read;
#endif
    uint32_t lock;

    ASSERT(norflash_ctx[id].opened, err_not_opened);

    if (!sec_reg_enabled) {
        return HAL_NORFLASH_BAD_OP;
    }
    // Check address and length
    if (sec_reg_total_size <= start_address) {
        return HAL_NORFLASH_BAD_ADDR;
    }
    remain_len = sec_reg_total_size - start_address;
    if (len > remain_len) {
        return HAL_NORFLASH_BAD_LEN;
    }

    pos = start_address;
    current_buffer = buffer;
    remain_len = len;

    reg_base = sec_reg_base;
    reg_pos = 0;

    lock = int_lock_global();

    norflash_pre_operation(id);

#ifdef FLASH_SEC_REG_FIFO_READ
#if (CHIP_FLASH_CTRL_VER >= 2)
    if (norflash_ctx[id].total_size > (1 << 24)) {
        norflash_exit_4byteaddr_mode(id);
    }
#endif
#else
    uint32_t mode = norflash_security_register_enable_read(id);
    uint32_t read_addr;
#endif

#if (CHIP_FLASH_CTRL_VER >= 3)
    // Set the dummy cycles for security register read command
    norflaship_dummy_others(id, 8);
    norflaship_busy_wait(id);
#endif

    while (remain_len > 0 && ret == 0) {
        if (reg_pos <= pos && pos < reg_pos + sec_reg_size) {
            // read a register
            if (pos + remain_len <= reg_pos + sec_reg_size) {
                read_size = remain_len;
            } else {
                read_size = sec_reg_size - (pos - reg_pos);
            }
            remain_len -= read_size;
#ifdef FLASH_SEC_REG_FIFO_READ
            while (read_size > 0 && ret == 0) {
                if (read_size > NORFLASHIP_RXFIFO_SIZE) {
                    each_read = NORFLASHIP_RXFIFO_SIZE;
                } else {
                    each_read = read_size;
                }
                ret = norflash_security_register_read(id, reg_base + (pos - reg_pos), current_buffer, each_read);
                read_size -= each_read;
                pos += each_read;
                current_buffer += each_read;
            }
#else
            read_addr = reg_base + (pos - reg_pos);
#if !defined(PROGRAMMER) || defined(PROGRAMMER_INFLASH)
            hal_cache_invalidate(HAL_CACHE_ID_I_CACHE, read_addr, read_size);
            hal_cache_invalidate(HAL_CACHE_ID_D_CACHE, read_addr, read_size);
#endif
            memcpy(current_buffer, (void *)read_addr, read_size);
#if !defined(PROGRAMMER) || defined(PROGRAMMER_INFLASH)
            hal_cache_invalidate(HAL_CACHE_ID_I_CACHE, read_addr, read_size);
            hal_cache_invalidate(HAL_CACHE_ID_D_CACHE, read_addr, read_size);
#endif
            pos += read_size;
            current_buffer += read_size;
#endif
        }
        reg_pos += sec_reg_size;
        reg_base += sec_reg_offset;
    }

#if (CHIP_FLASH_CTRL_VER >= 3)
    norflaship_busy_wait(id);
    norflaship_dummy_others(id, 0);
#endif

#ifdef FLASH_SEC_REG_FIFO_READ
#if (CHIP_FLASH_CTRL_VER >= 2)
    if (norflash_ctx[id].total_size > (1 << 24)) {
        norflash_enter_4byteaddr_mode(id);
    }
#endif
#else
    norflash_security_register_disable_read(id, mode);
#endif

    norflash_post_operation(id);

    int_unlock_global(lock);

    return (ret ? HAL_NORFLASH_ERR : HAL_NORFLASH_OK);
}

static enum HAL_NORFLASH_RET_T hal_norflash_parse_security_register_config(enum HAL_FLASH_ID_T id)
{
    union DRV_NORFLASH_SEC_REG_CFG_T cfg;
    uint32_t reg_cnt;

    cfg = norflash_get_security_register_config(id);

    if (!cfg.s.enabled) {
        return HAL_NORFLASH_BAD_OP;
    }

    if (cfg.s.base == SEC_REG_BASE_0X1000) {
        sec_reg_base = 0x1000;
    } else if (cfg.s.base == SEC_REG_BASE_0X0000) {
        sec_reg_base = 0;
    } else if (cfg.s.base == SEC_REG_BASE_0X2000) {
        sec_reg_base = 0x2000;
    } else {
        return HAL_NORFLASH_BAD_CFG;
    }
    sec_reg_base += FLASH_NC_BASE;

    if (cfg.s.size == SEC_REG_SIZE_1024) {
        sec_reg_size = 1024;
    } else if (cfg.s.size == SEC_REG_SIZE_512) {
        sec_reg_size = 512;
    } else if (cfg.s.size == SEC_REG_SIZE_256) {
        sec_reg_size = 256;
    } else if (cfg.s.size == SEC_REG_SIZE_4096) {
        sec_reg_size = 4096;
    } else {
        return HAL_NORFLASH_BAD_CFG;
    }

    if (cfg.s.offset == SEC_REG_OFFSET_0X1000) {
        sec_reg_offset = 0x1000;
    } else if (cfg.s.offset == SEC_REG_OFFSET_0X0100) {
        sec_reg_offset = 0x0100;
    } else {
        return HAL_NORFLASH_BAD_CFG;
    }

    if (sec_reg_size > sec_reg_offset) {
        return HAL_NORFLASH_BAD_CFG;
    }

    if (cfg.s.cnt == SEC_REG_CNT_3) {
        reg_cnt = 3;
    } else if (cfg.s.cnt == SEC_REG_CNT_4) {
        reg_cnt = 4;
    } else if (cfg.s.cnt == SEC_REG_CNT_2) {
        reg_cnt = 2;
    } else if (cfg.s.cnt == SEC_REG_CNT_1) {
        reg_cnt = 1;
    } else {
        return HAL_NORFLASH_BAD_CFG;
    }

    if (cfg.s.pp == SEC_REG_PP_256) {
        sec_reg_pp_size = 256;
    } else if (cfg.s.pp == SEC_REG_PP_1024) {
        sec_reg_pp_size = 1024;
    } else {
        return HAL_NORFLASH_BAD_CFG;
    }
#if (CHIP_FLASH_CTRL_VER <= 1)
#ifdef FLASH_SEC_REG_PP_1024
    // To write more than 256 bytes on flash controller V1, SPI rate must be lowered to avoid tx FIFO underflow.
    // Otherwise, the data must be split into pieces with size no more than 256 bytes.
#else
    sec_reg_pp_size = 256;
#endif
#endif

    sec_reg_total_size = sec_reg_size * reg_cnt;

    return HAL_NORFLASH_OK;
}
#endif

static void _norflash_set_source_freq(enum HAL_FLASH_ID_T id, enum HAL_CMU_FREQ_T freq)
{
#ifdef FLASH1_CTRL_BASE
    if (id == HAL_FLASH_ID_1) {
        hal_cmu_flash1_set_freq(freq);
        return;
    }
#endif
    hal_cmu_flash_set_freq(freq);
}

static void hal_norflash_reset_timing(enum HAL_FLASH_ID_T id)
{
    const uint32_t default_div = 8;

    // Restore default divider
    norflaship_div(id, default_div);
    norflash_init_sample_delay_by_div(id, default_div);
}

static enum HAL_NORFLASH_RET_T _norflash_open(enum HAL_FLASH_ID_T id, const struct HAL_NORFLASH_CONFIG_T *cfg,
                                              bool reopen, bool calib, uint32_t timing_idx)
{
    struct HAL_NORFLASH_CONFIG_T norcfg;
    enum HAL_CMU_FREQ_T source_freq;
    int result;
    uint32_t op;
    int found;

    if (reopen) {
        ASSERT(norflash_ctx[id].opened, err_not_opened);
    }

    // Place the config into ram
    if (cfg == NULL) {
        return HAL_NORFLASH_CFG_NULL;
    }
    memcpy(&norcfg, cfg, sizeof(norcfg));
    norflaship_busy_wait(id);
#if (CHIP_FLASH_CTRL_VER >= 2)
    norflaship_nand_sel(id, false);
#endif

    // Reset states
    norflash_ctx[id].opened = false;
    norflash_ctx[id].open_state = HAL_NORFLASH_NOT_OPENED;
#ifdef FLASH_SUSPEND
    if(!reopen)
    {
        suspend_state = SUSPEND_STATE_NONE;
    }
#endif
#ifdef FLASH_SECURITY_REGISTER
    sec_reg_enabled = false;
#endif

    if (!reopen) {
#if (CHIP_FLASH_CTRL_VER >= 2)
        // Set the direction of 4 IO pins to output when in idle
        norflaship_set_idle_io_dir(id, 0);
#endif

        // Reset norflash source clock
#if 0//defined(CHIP_HAS_EXT_NORFLASH)
        if(id == HAL_FLASH_ID_1){
            source_freq = hal_norflash_clk_to_cmu_freq(norcfg.source_clk);
            _norflash_set_source_freq(id, source_freq);
        }else
#else        
        _norflash_set_source_freq(id, HAL_CMU_FREQ_26M);
#endif
        // Reset controller timing
        hal_norflash_reset_timing(id);

        // Reset norflash in slow clock configuration
        norflash_reset(id);

        // Get device ID
        norflash_get_id(id, norflash_ctx[id].device_id, sizeof(norflash_ctx[id].device_id));

        // For each driver in array, match chip and select drv_ops
        found = norflash_match_chip(id, norflash_ctx[id].device_id, sizeof(norflash_ctx[id].device_id));
        if (!found) {
            result = HAL_NORFLASH_BAD_ID;
            goto _exit;
        }

        norflash_get_size(id, &norflash_ctx[id].total_size, &norflash_ctx[id].block_size,
            &norflash_ctx[id].sector_size, &norflash_ctx[id].page_size);

#ifdef FLASH_SECURITY_REGISTER
        result = hal_norflash_parse_security_register_config(id);
        if (result == HAL_NORFLASH_OK) {
            sec_reg_enabled = true;
        } else if (result != HAL_NORFLASH_BAD_OP) {
            goto _exit;
        }
#endif

#if (CHIP_FLASH_CTRL_VER <= 1) && !defined(FLASH_LOW_SPEED) && !defined(OSC_26M_X4_AUD2BB)
        // 1) Flash controller V2 or later
        //    No requirement on system_freq
        // 2) Flash controller V1
        //    Requirement on system_freq when running in quad mode (4-line mode):
        //      Byte Access:     flash_line_speed < 2 * system_freq
        //      Halfword Access: flash_line_speed < 4 * system_freq
        //      Word Access:     flash_line_speed < 8 * system_freq
        //    The maximum flash_line_speed is 120M in spec, and PLL_FREQ / 2 in our system.
        //    Normally it is 24.576M * 8 / 2 ~= 100M.
        //    So the safe system_freq should be larger than 50M/25M/12.5M for byte/halfword/word access.
        //    Cached access to flash is always safe, because it is always word-aligned (system_freq is never below 26M).
        //    However, uncached access (e.g., access to audio/user/factory data sections) is under risk.
        hal_sysfreq_set_min_freq(HAL_CMU_FREQ_52M);
#endif

#ifdef FLASH_UNIQUE_ID
        // 1) Flash controller V1:
        //    Use slow clock to avoid rx fifo overflow
        // 2) Flash controller V2/3/4/5:
        //    Use slow clock to avoid stopping SPI clock
        //    (One bit might be read as a fixed 1 at the SPI clock stopping edge in some timing combinations)
        // 3) Flash controller V6 or later:
        //    Can use fast clock
        norflash_get_unique_id(id, norflash_ctx[id].unique_id, sizeof(norflash_ctx[id].unique_id));
#endif

#if (CHIP_FLASH_CTRL_VER >= 2)
        // Set address mode after getting unique id (which needs 3-byte-address mode)
        if (norflash_ctx[id].total_size > (1 << 24)) {
            norflash_enter_4byteaddr_mode(id);
        }
#endif
    }

    // Set norflash source clock
    source_freq = hal_norflash_clk_to_cmu_freq(norcfg.source_clk);
    _norflash_set_source_freq(id, source_freq);

    /* over write config */
    if (norcfg.override_config) {
        /* div */
        norflaship_div(id, norcfg.div);
        /* sample delay */
        norflaship_samdly(id, norcfg.samdly);
        /* ru en */
        norflaship_ruen(id, norcfg.spiruen);
        /* rd en */
        norflaship_rden(id, norcfg.spirden);
        /* rd cmd */
        norflaship_rdcmd(id, norcfg.rdcmd);
        /* frd cmd */
        norflaship_frdcmd(id, norcfg.frdcmd);
        /* qrd cmd */
        norflaship_qrdcmd(id, norcfg.qrdcmd);
    } else {
        // Init divider
        result = norflash_init_div(id, &norcfg);
        if (result != 0) {
            result = HAL_NORFLASH_BAD_DIV;
            goto _exit;
        }

        if (!reopen || calib) {
            if (reopen) {
                norflash_pre_operation(id);
            }
            // Calib with the new divider
            result = norflash_sample_delay_calib(id, DRV_NORFLASH_CALIB_FLASH_ID);
            if (result != 0) {
                result = HAL_NORFLASH_BAD_CALIB_ID;
                goto _exit;
            }
            if (reopen) {
                norflash_post_operation(id);
            }
        } else {
            norflash_set_sample_delay_index(id, timing_idx);
        }
    }

    if (!reopen) {
#if defined(PROGRAMMER) || defined(OTA_PROGRAMMER)
        norflash_init_status(id, 0);
#elif defined(FLASH_PROTECTION)
        norflash_set_block_protection(id, hal_norflash_get_bp_cfg(id));
#endif
    }

    op = norcfg.mode;
    if (norcfg.speed >= HAL_NORFLASH_SPEED_104M) {
        op |= HAL_NORFLASH_OP_MODE_HIGH_PERFORMANCE;
    }

    // Divider will be set to normal read mode
    result = norflash_set_mode(id, op);
    if (result != 0) {
        result = HAL_NORFLASH_BAD_OP;
        goto _exit;
    }

    // -----------------------------
    // From now on, norflash_pre_operation(id) must be called before
    // sending any command to flash
    // -----------------------------

    if (!reopen) {
#if defined(CHIP_BEST1400) || defined(CHIP_BEST1402)
        if (norcfg.dec_enable && (norcfg.dec_size > 0)) {
            norflaship_dec_saddr(id, norcfg.dec_addr);
            norflaship_dec_eaddr(id, norcfg.dec_addr + norcfg.dec_size);
            norflaship_dec_index(id, norcfg.dec_idx);
            norflaship_dec_enable(id);
        } else {
            norflaship_dec_disable(id);
        }
#endif

#if defined(CHIP_BEST2001) && (defined(PSRAMUHS_ENABLE) || defined(PSRAM_ENABLE))
        norflaship_fetch_disable(id);
#endif
    }

    if (!norcfg.override_config && (!reopen || calib)) {
        result = norflash_sample_delay_calib(id, DRV_NORFLASH_CALIB_MAGIC_WORD);
        if (result != 0) {
            result = HAL_NORFLASH_BAD_CALIB_MAGIC;
            goto _exit;
        }
    }

    norflash_ctx[id].opened = true;

    result = HAL_NORFLASH_OK;

_exit:
    if (result != HAL_NORFLASH_OK) {
        _norflash_set_source_freq(id, HAL_CMU_FREQ_26M);
        hal_norflash_reset_timing(id);
        // Flash might be accessed again
    }

    norflash_ctx[id].open_state = result;

    return result;
}

enum HAL_NORFLASH_RET_T hal_norflash_open(enum HAL_FLASH_ID_T id, const struct HAL_NORFLASH_CONFIG_T *cfg)
{
    return _norflash_open(id, cfg, false, true, 0);
}

enum HAL_NORFLASH_RET_T hal_norflash_reopen(enum HAL_FLASH_ID_T id, const struct HAL_NORFLASH_CONFIG_T *cfg)
{
    return _norflash_open(id, cfg, true, true, 0);
}

enum HAL_NORFLASH_RET_T hal_norflash_apply_config(enum HAL_FLASH_ID_T id, const struct HAL_NORFLASH_CONFIG_T *cfg, uint32_t timing_idx)
{
    return _norflash_open(id, cfg, true, false, timing_idx);
}

uint32_t hal_norflash_get_timing_index(enum HAL_FLASH_ID_T id)
{
    ASSERT(norflash_ctx[id].opened, err_not_opened);
    return norflash_get_sample_delay_index(id);
}

void hal_norflash_show_calib_result(enum HAL_FLASH_ID_T id)
{
    norflash_show_calib_result(id);
}

enum HAL_NORFLASH_RET_T hal_norflash_erase_chip(enum HAL_FLASH_ID_T id)
{
    uint32_t total_size = 0;

    ASSERT(norflash_ctx[id].opened, err_not_opened);

    total_size = norflash_ctx[id].total_size;

    return hal_norflash_erase(id, 0, total_size);
}

static enum HAL_NORFLASH_RET_T hal_norflash_erase_int(enum HAL_FLASH_ID_T id, uint32_t start_address, uint32_t len, int suspend)
{
    uint32_t remain_len, current_address, total_size, block_size, sector_size;
    enum HAL_NORFLASH_RET_T ret = HAL_NORFLASH_OK;
    total_size      = norflash_ctx[id].total_size;
    block_size      = norflash_ctx[id].block_size;
    sector_size     = norflash_ctx[id].sector_size;

    start_address &= HAL_NORFLASH_ADDR_MASK;
#if defined(CHIP_HAS_EXT_NORFLASH)
    if(id == HAL_FLASH_ID_1){
        start_address -= 0x2000000;
    }
#endif
    // Check address and length
    if (total_size <= start_address) {
        return HAL_NORFLASH_BAD_ADDR;
    }
    remain_len      = total_size - start_address;
    if (len > remain_len) {
        return HAL_NORFLASH_BAD_LEN;
    }

#ifdef PUYA_FLASH_ERASE_PAGE_ENABLE
    // Align to page boundary
    uint32_t page_size;

    page_size       = norflash_ctx[id].page_size;
    remain_len      = start_address & (page_size - 1);
    if (remain_len) {
        start_address -= remain_len;
        len         += remain_len;
    }
    remain_len      = len & (page_size - 1);
    if (remain_len) {
        len         += page_size - remain_len;
    }
#else
    // Align to sector boundary
    remain_len      = start_address & (sector_size - 1);
    if (remain_len) {
        start_address -= remain_len;
        len         += remain_len;
    }
    remain_len      = len & (sector_size - 1);
    if (remain_len) {
        len         += sector_size - remain_len;
    }
#endif
    current_address = start_address;
    remain_len      = len;

#if defined(CONFIG_SMP)
    nuttx_smp_start_protection();
#endif
    norflash_pre_operation(id);

    if (current_address == 0 && remain_len >= total_size) {
        // erase whole chip
        ret = norflash_erase(id, current_address, DRV_NORFLASH_ERASE_CHIP, suspend);
    } else {
        while (remain_len > 0 && ret == HAL_NORFLASH_OK) {
            if (remain_len >= block_size && ((current_address & (block_size - 1)) == 0)) {
                // if large enough to erase a block and current_address is block boundary - erase a block
                ret = norflash_erase(id, current_address, DRV_NORFLASH_ERASE_BLOCK, suspend);
                remain_len -= block_size;
                current_address += block_size;
            } else {
#ifdef PUYA_FLASH_ERASE_PAGE_ENABLE
                if (remain_len >= sector_size && ((current_address & (sector_size - 1)) == 0)) {
                    // if large enough to erase a sector and current_address is sector boundary - erase a sector
                    ret = norflash_erase(id, current_address, DRV_NORFLASH_ERASE_SECTOR, suspend);
                    remain_len -= sector_size;
                    current_address += sector_size;
                } else {
                   // erase a page
                    ret = norflash_erase(id, current_address, DRV_NORFLASH_ERASE_PAGE, suspend);
                    if (remain_len > page_size) {
                        remain_len -= page_size;
                    } else {
                        remain_len = 0;
                    }
                    current_address += page_size;
                }
#else
                // erase a sector
                ret = norflash_erase(id, current_address, DRV_NORFLASH_ERASE_SECTOR, suspend);
                if (remain_len > sector_size) {
                    remain_len -= sector_size;
                } else {
                    remain_len = 0;
                }
                current_address += sector_size;
#endif
            }
        }
    }

    norflash_post_operation(id);
#if defined(CONFIG_SMP)
    nuttx_smp_end_protection();
#endif
#ifdef FLASH_SUSPEND
    if (ret == HAL_NORFLASH_SUSPENDED) {
        suspend_state = SUSPEND_STATE_ERASE;
        op_next_addr = current_address;
        op_remain_len = remain_len;
    } else {
        suspend_state = SUSPEND_STATE_NONE;
    }
#endif

    return ret;
}

enum HAL_NORFLASH_RET_T hal_norflash_erase_suspend(enum HAL_FLASH_ID_T id, uint32_t start_address, uint32_t len, int suspend)
{
    ASSERT(norflash_ctx[id].opened, err_not_opened);

#ifdef FLASH_SUSPEND
    if (suspend_state != SUSPEND_STATE_NONE) {
        return HAL_NORFLASH_BAD_OP;
    }
    if ((norflash_get_supported_mode(id) & HAL_NORFLASH_OP_MODE_SUSPEND) == 0) {
        suspend = 0;
    }
#endif

    return hal_norflash_erase_int(id, start_address, len, suspend);
}

enum HAL_NORFLASH_RET_T hal_norflash_erase(enum HAL_FLASH_ID_T id, uint32_t start_address, uint32_t len)
{
    return hal_norflash_erase_suspend(id, start_address, len, 0);
}

enum HAL_NORFLASH_RET_T hal_norflash_erase_resume(enum HAL_FLASH_ID_T id, int suspend)
{
#ifdef FLASH_SUSPEND
    enum HAL_NORFLASH_RET_T ret;

    ASSERT(norflash_ctx[id].opened, err_not_opened);

    if (suspend_state != SUSPEND_STATE_ERASE) {
        return HAL_NORFLASH_BAD_OP;
    }
    if ((norflash_get_supported_mode(id) & HAL_NORFLASH_OP_MODE_SUSPEND) == 0) {
        return HAL_NORFLASH_BAD_OP;
    }

    ret = norflash_erase_resume(id, suspend);
    if (ret == HAL_NORFLASH_SUSPENDED) {
        return ret;
    }

    return hal_norflash_erase_int(id, op_next_addr, op_remain_len, suspend);
#else
    return HAL_NORFLASH_OK;
#endif
}

static enum HAL_NORFLASH_RET_T hal_norflash_write_int(enum HAL_FLASH_ID_T id, uint32_t start_address, const uint8_t *buffer, uint32_t len, int suspend)
{
    const uint8_t *current_buffer;
    uint32_t remain_len, current_address, total_size, page_size, write_size;
    uint32_t pp_remain;
    enum HAL_NORFLASH_RET_T ret = HAL_NORFLASH_OK;

    total_size      = norflash_ctx[id].total_size;
    page_size       = norflash_ctx[id].page_size;

    start_address &= HAL_NORFLASH_ADDR_MASK;
#if defined(CHIP_HAS_EXT_NORFLASH)
    if(id == HAL_FLASH_ID_1){
        start_address -= 0x2000000;
    }
#endif

    // Check address and length
    if (total_size <= start_address) {
        return HAL_NORFLASH_BAD_ADDR;
    }
    remain_len      = total_size - start_address;
    if (len > remain_len) {
        return HAL_NORFLASH_BAD_LEN;
    }

    current_address = start_address;
    current_buffer  = buffer;
    remain_len      = len;

#if defined(CONFIG_SMP)
    nuttx_smp_start_protection();
#endif
    norflash_pre_operation(id);

    while (remain_len > 0 && ret == HAL_NORFLASH_OK) {
        if (remain_len > page_size) {
            write_size = page_size;
        } else {
            write_size = remain_len;
        }
        // Align to program page
        pp_remain = page_size - (current_address & (page_size - 1));
        if (write_size > pp_remain) {
            write_size = pp_remain;
        }

        ret = norflash_write(id, current_address, current_buffer, write_size, suspend);

        current_address += write_size;
        current_buffer  += write_size;
        remain_len      -= write_size;
    }

    norflash_post_operation(id);
#if defined(CONFIG_SMP)
    nuttx_smp_end_protection();
#endif

#ifdef FLASH_SUSPEND
    if (ret == HAL_NORFLASH_SUSPENDED) {
        suspend_state = SUSPEND_STATE_PROGRAM;
        op_next_addr = current_address;
        op_next_buf = current_buffer;
        op_remain_len = remain_len;
    } else {
        suspend_state = SUSPEND_STATE_NONE;
    }
#endif

    return ret;
}

enum HAL_NORFLASH_RET_T hal_norflash_write_suspend(enum HAL_FLASH_ID_T id, uint32_t start_address, const uint8_t *buffer, uint32_t len, int suspend)
{
    ASSERT(norflash_ctx[id].opened, err_not_opened);

#ifdef FLASH_SUSPEND
    if (suspend_state != SUSPEND_STATE_NONE) {
        return HAL_NORFLASH_BAD_OP;
    }
    if ((norflash_get_supported_mode(id) & HAL_NORFLASH_OP_MODE_SUSPEND) == 0) {
        suspend = 0;
    }
#endif

    return hal_norflash_write_int(id, start_address, buffer, len, suspend);
}

enum HAL_NORFLASH_RET_T hal_norflash_write(enum HAL_FLASH_ID_T id, uint32_t start_address, const uint8_t *buffer, uint32_t len)
{
    return hal_norflash_write_suspend(id, start_address, buffer, len, 0);
}

enum HAL_NORFLASH_RET_T hal_norflash_write_resume(enum HAL_FLASH_ID_T id, int suspend)
{
#ifdef FLASH_SUSPEND
    enum HAL_NORFLASH_RET_T ret;

    ASSERT(norflash_ctx[id].opened, err_not_opened);

    if (suspend_state != SUSPEND_STATE_PROGRAM) {
        return HAL_NORFLASH_BAD_OP;
    }
    if ((norflash_get_supported_mode(id) & HAL_NORFLASH_OP_MODE_SUSPEND) == 0) {
        return HAL_NORFLASH_BAD_OP;
    }

    ret = norflash_write_resume(id, suspend);
    if (ret == HAL_NORFLASH_SUSPENDED) {
        return ret;
    }

    return hal_norflash_write_int(id, op_next_addr, op_next_buf, op_remain_len, suspend);
#else
    return HAL_NORFLASH_OK;
#endif
}

enum HAL_NORFLASH_RET_T hal_norflash_suspend_check_irq(enum HAL_FLASH_ID_T id, uint32_t irq_num)
{
#ifdef FLASH_SUSPEND
    int ret;

    ret = norflash_suspend_check_irq(irq_num);
    if (ret) {
        return HAL_NORFLASH_ERR;
    }
#endif

    return HAL_NORFLASH_OK;
}

enum HAL_NORFLASH_RET_T hal_norflash_read(enum HAL_FLASH_ID_T id, uint32_t start_address, uint8_t *buffer, uint32_t len)
{
    uint8_t *current_buffer;
    uint32_t remain_len, current_address, total_size, read_size;

    ASSERT(norflash_ctx[id].opened, err_not_opened);

    total_size      = norflash_ctx[id].total_size;

    start_address &= HAL_NORFLASH_ADDR_MASK;
#if defined(CHIP_HAS_EXT_NORFLASH)
    if(id == HAL_FLASH_ID_1){
        start_address -= 0x2000000;
    }
#endif

    // Check address and length
    if (total_size <= start_address) {
        return HAL_NORFLASH_BAD_ADDR;
    }
    remain_len      = total_size - start_address;
    if (len > remain_len) {
        return HAL_NORFLASH_BAD_LEN;
    }

    read_size       = NORFLASHIP_RXFIFO_SIZE;
    remain_len      = len;
    current_address = start_address;
    current_buffer  = buffer;

    norflash_pre_operation(id);

    while (remain_len > 0) {
        read_size = (remain_len > NORFLASHIP_RXFIFO_SIZE) ? NORFLASHIP_RXFIFO_SIZE : remain_len;
        norflash_read(id, current_address, current_buffer, read_size);

        current_address += read_size;
        current_buffer  += read_size;
        remain_len      -= read_size;
    }

    norflash_post_operation(id);

    return HAL_NORFLASH_OK;
}

enum HAL_NORFLASH_RET_T hal_norflash_close(enum HAL_FLASH_ID_T id)
{
    return HAL_NORFLASH_OK;
}

void hal_norflash_sleep(enum HAL_FLASH_ID_T id)
{
    if (!norflash_ctx[id].opened) {
        return;
    }

#ifdef FLASH_DEEP_SLEEP
    norflash_sleep(id);
#else
    norflash_pre_operation(id);
#endif

    norflaship_busy_wait(id);
    norflaship_sleep(id);
}

void hal_norflash_wakeup(enum HAL_FLASH_ID_T id)
{
    if (!norflash_ctx[id].opened) {
        return;
    }

    norflaship_wakeup(id);

#ifdef FLASH_DEEP_SLEEP
    norflash_wakeup(id);
#else
    norflash_post_operation(id);
#endif

#if 0
    // Flush prefetch buffer
    *(volatile uint32_t *)FLASH_NC_BASE;
    *(volatile uint32_t *)(FLASH_NC_BASE + 0x1000);
#else
    norflaship_clear_rxfifo(id);
    norflaship_busy_wait(id);
#endif
}

int hal_norflash_busy()
{
#ifdef FLASH1_CTRL_BASE
    if (norflaship_is_busy(HAL_FLASH_ID_1))
        return 1;
#endif
    return norflaship_is_busy(HAL_FLASH_ID_0);
}

uint32_t hal_norflash_mem_read_bus_lock(enum HAL_FLASH_ID_T id)
{
#ifdef FLASH_MEM_READ_BUS_LOCK
    return norflaship_mem_read_bus_lock(id);
#else
    return 0;
#endif
}

void hal_norflash_mem_read_bus_unlock(enum HAL_FLASH_ID_T id, uint32_t status)
{
#ifdef FLASH_MEM_READ_BUS_LOCK
    norflaship_mem_read_bus_unlock(id, status);
#endif
}

static void hal_norflash_prefetch_idle(enum HAL_FLASH_ID_T id)
{
    hal_sys_timer_delay(4);
    if (norflaship_is_busy(id)) {
        hal_sys_timer_delay(4);
    }
}

void hal_norflash_set_boot_freq(enum HAL_CMU_FREQ_T freq)
{
#if !defined(PROGRAMMER) || defined(PROGRAMMER_INFLASH)
    // Make sure flash is still working after freq change
    // At least best1000 and best2001 require the timing reset

    // Wait until norflash becomes idle
    hal_norflash_prefetch_idle(HAL_FLASH_ID_0);

    // Reset flash timing
    hal_norflash_reset_timing(HAL_FLASH_ID_0);

#if 0//会导致开不了机 def CHIP_HAS_EXT_NORFLASH
    // Wait until norflash becomes idle
    hal_norflash_prefetch_idle(HAL_FLASH_ID_1);

    // Reset flash timing
    hal_norflash_reset_timing(HAL_FLASH_ID_1);
#endif
#endif

    hal_cmu_flash_set_freq(freq);
}

const struct HAL_NORFLASH_CONFIG_T *hal_norflash_get_init_config(void)
{
    return &norflash_cfg;
}

enum HAL_NORFLASH_RET_T hal_norflash_init(void)
{
    enum HAL_NORFLASH_RET_T ret;
    const struct HAL_NORFLASH_CONFIG_T *cfg;

    cfg = hal_norflash_get_init_config();

    // Pmu codes might be located in flash
    pmu_flash_freq_config(cfg->speed);

    // Avoid flash access from here
    hal_norflash_prefetch_idle(HAL_FLASH_ID_0);

    ret = hal_norflash_open(HAL_FLASH_ID_0, cfg);
    return ret;
}

#if defined(CHIP_HAS_EXT_NORFLASH)&&defined(FLASH1_CTRL_BASE)
enum HAL_NORFLASH_RET_T hal_norflash2_init(void)
{
    enum HAL_NORFLASH_RET_T ret = 0;
    struct HAL_NORFLASH_CONFIG_T cfg;
    

    memcpy(&cfg, hal_norflash_get_init_config(), sizeof(cfg));
    cfg.source_clk = HAL_NORFLASH_SPEED_104M * 2,
    cfg.speed = HAL_NORFLASH_SPEED_104M;

    TRACE(3, "%s: source_clk=%d, speed=%d", __func__, cfg.source_clk, cfg.speed);

    hal_cmu_flash1_enable();
    
#if defined(FLASH1_CTRL_BASE) && defined(CHIP_BEST2003)
    // hal_cmu_set_flash0_size(HAL_FLASH0_SIZE_32M);
#endif
    
    //hal_cmu_pll_enable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_SYS);
    //hal_cmu_flash_all_select_pll(HAL_CMU_PLL_AUD);

    // norflaship_busy_wait(HAL_FLASH_ID_1);
    // return ret;
    
    ret = hal_norflash_open(HAL_FLASH_ID_1, &cfg);

    uint8_t flash_dev_id[HAL_NORFLASH_DEVICE_ID_LEN];
    hal_norflash_get_id(HAL_FLASH_ID_1, flash_dev_id, ARRAY_SIZE(flash_dev_id));
    TRACE(3,"FLASH_ID: %02X-%02X-%02X", flash_dev_id[0], flash_dev_id[1], flash_dev_id[2]);

    norflaship_dummy_qior(HAL_FLASH_ID_1, 3);

    while(flash_dev_id[0] == 0xff && flash_dev_id[1]==0xff && flash_dev_id[2] == 0xff){
        norflash_get_id(HAL_FLASH_ID_1, flash_dev_id, sizeof(flash_dev_id));
    }
    TRACE(3,"FLASH_ID: %02X-%02X-%02X", flash_dev_id[0], flash_dev_id[1], flash_dev_id[2]);
    TRACE(3, "FLASH size: 0x%x", norflash_ctx[HAL_FLASH_ID_1].total_size);
    hal_norflash_show_calib_result(HAL_FLASH_ID_1);
    
    ASSERT(ret == 0, "Failed to open norflash: %d", ret);
    return ret;
}
#endif

enum HAL_NORFLASH_RET_T hal_norflash_deinit(void)
{
    // Avoid flash access from here
    hal_norflash_prefetch_idle(HAL_FLASH_ID_0);

#if (CHIP_FLASH_CTRL_VER >= 2)
    // Set the direction of 4 IO pins to output when in idle
    norflaship_set_idle_io_dir(HAL_FLASH_ID_0, 0);
#endif

    // Reset norflash source clock
    hal_cmu_flash_set_freq(HAL_CMU_FREQ_26M);

    // Reset controller timing
    hal_norflash_reset_timing(HAL_FLASH_ID_0);

    // Reset norflash in slow clock configuration
    norflash_reset(HAL_FLASH_ID_0);

    norflash_ctx[HAL_FLASH_ID_0].opened = false;

    return HAL_NORFLASH_OK;
}

uint32_t hal_norflash_get_flash_total_size(enum HAL_FLASH_ID_T id)
{
    return norflash_ctx[id].total_size;
}

int hal_norflash_opened(enum HAL_FLASH_ID_T id)
{
    return norflash_ctx[id].opened;
}

enum HAL_NORFLASH_RET_T hal_norflash_get_open_state(enum HAL_FLASH_ID_T id)
{
    return norflash_ctx[id].open_state;
}

void hal_norflash_reset_remap(enum HAL_FLASH_ID_T id)
{
    norflaship_reset_remap(id);
}

BOOT_TEXT_FLASH_LOC
void hal_norflash_boot_reset_remap(enum HAL_FLASH_ID_T id)
{
    norflaship_boot_reset_remap(id);
}

__STATIC_FORCEINLINE
enum HAL_NORFLASH_RET_T _config_remap(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id,
                                      uint32_t addr, uint32_t len, uint32_t offset,
                                      uint32_t flash_size, void (*busy_wait)(enum HAL_FLASH_ID_T id),
                                      int (*remap_sec)(enum HAL_FLASH_ID_T id, uint32_t index, uint32_t addr, uint32_t len, uint32_t new_addr)
                                     )
{
#if (CHIP_FLASH_CTRL_VER >= 2)
    uint32_t remap_from;
    int ret;

    STATIC_ASSERT((FLASH_BASE & HAL_NORFLASH_ADDR_MASK) == 0, "Bad FLASH_BASE");
#ifndef CHIP_BEST1501SIMU
    STATIC_ASSERT((FLASH_SIZE & (FLASH_SIZE - 1)) == 0, "Bad FLASH_SIZE");
#endif

    // Offset must be aligned with BLOCK ERASE SIZE (32K) to avoid possible
    // erase error or erase performance degradation
    if (offset == 0 || offset >= flash_size || (offset & (REMAP_BLOCK_SIZE - 1))) {
        return HAL_NORFLASH_BAD_REMAP_OFFSET;
    }

    remap_from = addr & HAL_NORFLASH_ADDR_MASK;

#ifdef CHIP_BEST2300P
    uint32_t remap_to, remap_len, remap_end, region_len;
    uint8_t msb_pos;

    if (remap_id != HAL_NORFLASH_REMAP_ID_0) {
        return HAL_NORFLASH_BAD_REMAP_ID;
    }
    if (offset & (offset - 1)) {
        return HAL_NORFLASH_BAD_ADDR;
    }
    if (offset > flash_size / 2) {
        return HAL_NORFLASH_BAD_REMAP_OFFSET;
    }
    flash_size = offset * 2;

    if (remap_from < flash_size / (1 << (NORFLASHIP_REMAP_NUM + 1))) {
        return HAL_NORFLASH_BAD_ADDR;
    }
    if (remap_from % (flash_size / (1 << (NORFLASHIP_REMAP_NUM + 1)))) {
        return HAL_NORFLASH_BAD_ADDR;
    }

    if (remap_from + len > flash_size / 2) {
        return HAL_NORFLASH_BAD_LEN;
    }

    if (busy_wait) {
        busy_wait(id);
    }

    remap_id = 0;
    remap_end = flash_size / 2;
    remap_len = remap_end - remap_from;
    while (remap_len && remap_id < NORFLASHIP_REMAP_NUM) {
        msb_pos = 31 - __CLZ(remap_len);
        region_len = 1 << msb_pos;
        remap_from = remap_end - region_len;
        remap_to = flash_size - remap_end; // OR: (flash_size / 2 + (flash_size / 2 - remap_end))
        ret = remap_sec(id, remap_id, remap_from, region_len, remap_to);
        if (ret) {
            return HAL_NORFLASH_BAD_OP;
        }
        remap_len -= region_len;
        remap_end -= region_len;
        remap_id++;
    }

    if (remap_len) {
        return HAL_NORFLASH_BAD_LEN;
    }
#else
    if (remap_id >= HAL_NORFLASH_REMAP_ID_QTY) {
        return HAL_NORFLASH_BAD_REMAP_ID;
    }
    // Align len to sector size
    len = (len + (REMAP_SECTOR_SIZE - 1)) & ~(REMAP_SECTOR_SIZE - 1);
    if (len > offset) {
        return HAL_NORFLASH_BAD_LEN;
    }
    if (remap_from + offset + len > flash_size) {
        return HAL_NORFLASH_BAD_ADDR;
    }

    if (busy_wait) {
        busy_wait(id);
    }

    ret = remap_sec(id, remap_id, remap_from, len, remap_from + offset);
    if (ret) {
        return HAL_NORFLASH_BAD_OP;
    }
#endif

    return HAL_NORFLASH_OK;
#else
    return HAL_NORFLASH_ERR;
#endif
}

enum HAL_NORFLASH_RET_T hal_norflash_config_remap(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t addr, uint32_t len, uint32_t offset)
{
    uint32_t flash_size;

    if (!norflash_ctx[id].opened) {
        return HAL_NORFLASH_NOT_OPENED;
    }

    flash_size = norflash_ctx[id].total_size;
#ifndef PROGRAMMER
    if (flash_size > FLASH_SIZE) {
        flash_size = FLASH_SIZE;
    }
#endif

    return _config_remap(id, remap_id, addr, len, offset, flash_size, norflaship_busy_wait, norflaship_config_remap_section);
}

BOOT_TEXT_FLASH_LOC
enum HAL_NORFLASH_RET_T hal_norflash_boot_config_remap(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t addr, uint32_t len, uint32_t offset)
{
    uint32_t flash_size;

#if defined(CHIP_BEST2300P) && !defined(FLASH_REMAP_CHECK_FLASH_SIZE)
#define FLASH_REMAP_CHECK_FLASH_SIZE
#endif
#ifdef FLASH_REMAP_CHECK_FLASH_SIZE
    flash_size = FLASH_SIZE;
#else
    flash_size = ~0UL;
#endif
    return _config_remap(id, remap_id, addr, len, offset, flash_size, NULL, norflaship_boot_config_remap_section);
}

enum HAL_NORFLASH_RET_T hal_norflash_enable_remap(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id)
{
#if (CHIP_FLASH_CTRL_VER >= 2)
    norflaship_busy_wait(id);

    norflaship_enable_remap(id, remap_id);

    norflaship_busy_wait(id);
    norflaship_clear_fifos(id);

    return HAL_NORFLASH_OK;
#else
    return HAL_NORFLASH_ERR;
#endif
}

BOOT_TEXT_FLASH_LOC
enum HAL_NORFLASH_RET_T hal_norflash_boot_enable_remap(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id)
{
#if (CHIP_FLASH_CTRL_VER >= 2)
    norflaship_boot_enable_remap(id, remap_id);
    return HAL_NORFLASH_OK;
#else
    return HAL_NORFLASH_ERR;
#endif
}

enum HAL_NORFLASH_RET_T hal_norflash_disable_remap(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id)
{
#if (CHIP_FLASH_CTRL_VER >= 2)
    norflaship_busy_wait(id);

    norflaship_disable_remap(id, remap_id);

    norflaship_busy_wait(id);
    norflaship_clear_fifos(id);

    return HAL_NORFLASH_OK;
#else
    return HAL_NORFLASH_ERR;
#endif
}

BOOT_TEXT_FLASH_LOC
enum HAL_NORFLASH_RET_T hal_norflash_boot_disable_remap(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id)
{
#if (CHIP_FLASH_CTRL_VER >= 2)
    norflaship_boot_disable_remap(id, remap_id);
    return HAL_NORFLASH_OK;
#else
    return HAL_NORFLASH_ERR;
#endif
}

int hal_norflash_get_remap_status(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id)
{
#if (CHIP_FLASH_CTRL_VER >= 2)
    return norflaship_get_remap_status(id, remap_id);
#else
    return HAL_NORFLASH_ERR;
#endif
}

BOOT_TEXT_FLASH_LOC
int hal_norflash_boot_get_remap_status(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id)
{
#if (CHIP_FLASH_CTRL_VER >= 2)
    return norflaship_boot_get_remap_status(id, remap_id);
#else
    return HAL_NORFLASH_ERR;
#endif
}
