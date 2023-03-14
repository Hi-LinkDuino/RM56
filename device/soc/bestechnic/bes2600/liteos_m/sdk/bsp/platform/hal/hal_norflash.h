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
#ifndef NORFLASH_HAL_H
#define NORFLASH_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "hal_cmu.h"

// 64M Bytes
#define HAL_NORFLASH_ADDR_MASK              0x03FFFFFF

#define HAL_NORFLASH_DEVICE_ID_LEN          3

#define HAL_NORFLASH_CP_ID_LEN              2
#define HAL_NORFLASH_UNIQUE_ID_LEN          16

#define FLASH_SECTOR_SIZE_IN_BYTES          4096
#define FLASH_BLOCK_SIZE_IN_BYTES           (32*1024)

#define NORFLASH_PUYA_ID_PREFIX             0x85

#define NORFLASH_XTS_ID_PREFIX              0x0B

#define NORFLASH_XMC_ID_PREFIX              0x20

#define NORFLASH_ZBIT_ID_PREFIX             0x5E

#define NORFLASH_WB_ID_PREFIX               0xEF

#define NORFLASH_GD_ID_PREFIX               0xC8

enum HAL_NORFLASH_RET_T {
    HAL_NORFLASH_OK,
    HAL_NORFLASH_SUSPENDED,
    HAL_NORFLASH_ERR,
    HAL_NORFLASH_BAD_ID,
    HAL_NORFLASH_BAD_DIV,
    HAL_NORFLASH_BAD_CALIB_ID,
    HAL_NORFLASH_BAD_CFG,
    HAL_NORFLASH_BAD_OP,
    HAL_NORFLASH_BAD_CALIB_MAGIC,
    HAL_NORFLASH_BAD_ADDR,
    HAL_NORFLASH_BAD_LEN,
    HAL_NORFLASH_BAD_REMAP_ID,
    HAL_NORFLASH_BAD_REMAP_OFFSET,
    HAL_NORFLASH_BAD_ERASE_TYPE,
    HAL_NORFLASH_NOT_OPENED,
    HAL_NORFLASH_CFG_NULL,
};

enum HAL_NORFLASH_SPEED {
    HAL_NORFLASH_SPEED_13M  = 13000000,
    HAL_NORFLASH_SPEED_26M  = 26000000,
    HAL_NORFLASH_SPEED_52M  = 52000000,
    HAL_NORFLASH_SPEED_78M  = 78000000,
    HAL_NORFLASH_SPEED_104M = 104000000,
    HAL_NORFLASH_SPEED_130M = 130000000,
    HAL_NORFLASH_SPEED_156M = 156000000,
    HAL_NORFLASH_SPEED_182M = 182000000,
    HAL_NORFLASH_SPEED_208M = 208000000,
    HAL_NORFLASH_SPEED_234M = 234000000,
};

enum HAL_NORFLASH_REMAP_ID_T {
    HAL_NORFLASH_REMAP_ID_0,
    HAL_NORFLASH_REMAP_ID_1,
    HAL_NORFLASH_REMAP_ID_2,
    HAL_NORFLASH_REMAP_ID_3,

    HAL_NORFLASH_REMAP_ID_QTY,
};

enum HAL_NORFLASH_OP_MODE {
    // Different groups can be used together, different flash-device may support different option(s)

    // (1) basic read mode
    // standard spi mode
    HAL_NORFLASH_OP_MODE_STAND_SPI              = (1 << 0),
    // fast spi mode
    HAL_NORFLASH_OP_MODE_FAST_SPI               = (1 << 1),
    // dual mode
    HAL_NORFLASH_OP_MODE_DUAL_OUTPUT            = (1 << 2),
    // dual mode
    HAL_NORFLASH_OP_MODE_DUAL_IO                = (1 << 3),
    // quad mode
    HAL_NORFLASH_OP_MODE_QUAD_OUTPUT            = (1 << 4),
    // quad mode
    HAL_NORFLASH_OP_MODE_QUAD_IO                = (1 << 5),
    // dtr mode
    HAL_NORFLASH_OP_MODE_DTR                    = (1 << 6),

    // (2) extend read mode
    // read accelerate (no cmd bettween read operation) :
    // may need Dual or Quad Mode
    HAL_NORFLASH_OP_MODE_CONTINUOUS_READ        = (1 << 12),
    // read high performance mode
    HAL_NORFLASH_OP_MODE_HIGH_PERFORMANCE       = (1 << 13),
    // read wrap mode
    HAL_NORFLASH_OP_MODE_READ_WRAP              = (1 << 14),

    // (3) program mode.
    // page program mode
    HAL_NORFLASH_OP_MODE_PAGE_PROGRAM           = (1 << 16),
    // dual program mode
    HAL_NORFLASH_OP_MODE_DUAL_PAGE_PROGRAM      = (1 << 17),
    // quad program mode
    HAL_NORFLASH_OP_MODE_QUAD_PAGE_PROGRAM      = (1 << 18),
    // erase in standard spi mode
    HAL_NORFLASH_OP_MODE_ERASE_IN_STD           = (1 << 19),

    // (4) advanced features
    // suspend and resume
    HAL_NORFLASH_OP_MODE_SUSPEND                = (1 << 24),

    HAL_NORFLASH_OP_MODE_RESERVED               = 0xFFFFFFFF,
};

struct HAL_NORFLASH_CONFIG_T {
    uint32_t source_clk;
    uint32_t speed;
    enum HAL_NORFLASH_OP_MODE mode;

    /* internal use : can be config if need to (useful for rom) */
    uint8_t override_config:1;
    uint8_t neg_phase:1;
    uint8_t pos_neg:1;
    uint8_t reserved_3:1;
    uint8_t samdly:3;
    uint8_t div; /* least 2 */
    uint8_t dualmode:1;
    uint8_t holdpin:1;
    uint8_t wprpin:1;
    uint8_t quadmode:1;

    uint8_t spiruen:3;
    uint8_t spirden:3;

    uint8_t dualiocmd;
    uint8_t rdcmd;
    uint8_t frdcmd;
    uint8_t qrdcmd; /* quad io cmd */
#if defined(CHIP_BEST1400) || defined(CHIP_BEST1402)
    uint8_t  dec_enable;  /* 1: enable decoder, 0: disable decoder */
    uint8_t  dec_idx;     /* decoder key index ,from 0 to 3 */
    uint32_t dec_addr;    /* start address where to decode */
    uint32_t dec_size;    /* bytes number will be decoded */
#endif
};

#ifdef ARM_CMNS
#define NORFLASH_API_WRAP(n)                                         n##_se
#else
#define NORFLASH_API_WRAP(n)                                         n
#endif

#ifndef ARM_CMNS
/* hal api */
void hal_norflash_set_boot_freq(enum HAL_CMU_FREQ_T freq);
const struct HAL_NORFLASH_CONFIG_T *hal_norflash_get_init_config(void);
enum HAL_NORFLASH_RET_T hal_norflash_init(void);
enum HAL_NORFLASH_RET_T hal_norflash_deinit(void);
enum HAL_NORFLASH_RET_T hal_norflash_open(enum HAL_FLASH_ID_T id, const struct HAL_NORFLASH_CONFIG_T *cfg);
enum HAL_NORFLASH_RET_T hal_norflash_reopen(enum HAL_FLASH_ID_T id, const struct HAL_NORFLASH_CONFIG_T *cfg);
enum HAL_NORFLASH_RET_T hal_norflash_apply_config(enum HAL_FLASH_ID_T id, const struct HAL_NORFLASH_CONFIG_T *cfg, uint32_t timing_idx);
uint32_t hal_norflash_get_timing_index(enum HAL_FLASH_ID_T id);
void hal_norflash_show_calib_result(enum HAL_FLASH_ID_T id);
enum HAL_CMU_FREQ_T hal_norflash_clk_to_cmu_freq(uint32_t clk);
enum HAL_NORFLASH_RET_T hal_norflash_get_size(enum HAL_FLASH_ID_T id, uint32_t *total_size, uint32_t *block_size, uint32_t *sector_size, uint32_t *page_size);
enum HAL_NORFLASH_RET_T hal_norflash_get_boundary(enum HAL_FLASH_ID_T id, uint32_t address, uint32_t* block_boundary, uint32_t* sector_boundary);
enum HAL_NORFLASH_RET_T hal_norflash_get_id(enum HAL_FLASH_ID_T id, uint8_t *value, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_get_unique_id(enum HAL_FLASH_ID_T id, uint8_t *value, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_enable_protection(enum HAL_FLASH_ID_T id);
enum HAL_NORFLASH_RET_T hal_norflash_disable_protection(enum HAL_FLASH_ID_T id);
enum HAL_NORFLASH_RET_T hal_norflash_erase_chip(enum HAL_FLASH_ID_T id);
enum HAL_NORFLASH_RET_T hal_norflash_erase_suspend(enum HAL_FLASH_ID_T id, uint32_t start_address, uint32_t len, int suspend);
enum HAL_NORFLASH_RET_T hal_norflash_erase(enum HAL_FLASH_ID_T id, uint32_t start_address, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_erase_resume(enum HAL_FLASH_ID_T id, int suspend);
enum HAL_NORFLASH_RET_T hal_norflash_write_suspend(enum HAL_FLASH_ID_T id, uint32_t start_address, const uint8_t *buffer, uint32_t len, int suspend);
enum HAL_NORFLASH_RET_T hal_norflash_write(enum HAL_FLASH_ID_T id, uint32_t start_address, const uint8_t *buffer, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_write_resume(enum HAL_FLASH_ID_T id, int suspend);
enum HAL_NORFLASH_RET_T hal_norflash_suspend_check_irq(enum HAL_FLASH_ID_T id, uint32_t irq_num);
enum HAL_NORFLASH_RET_T hal_norflash_read(enum HAL_FLASH_ID_T id, uint32_t start_address, uint8_t *buffer, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_close(enum HAL_FLASH_ID_T id);
void hal_norflash_sleep(enum HAL_FLASH_ID_T id);
void hal_norflash_wakeup(enum HAL_FLASH_ID_T id);
int hal_norflash_busy(void);
uint32_t hal_norflash_mem_read_bus_lock(enum HAL_FLASH_ID_T id);
void hal_norflash_mem_read_bus_unlock(enum HAL_FLASH_ID_T id, uint32_t status);
uint32_t hal_norflash_get_flash_total_size(enum HAL_FLASH_ID_T id);
int hal_norflash_opened(enum HAL_FLASH_ID_T id);
enum HAL_NORFLASH_RET_T hal_norflash_get_open_state(enum HAL_FLASH_ID_T id);
int hal_norflash_security_register_is_locked(enum HAL_FLASH_ID_T id, uint32_t start_address, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_security_register_lock(enum HAL_FLASH_ID_T id, uint32_t start_address, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_security_register_erase(enum HAL_FLASH_ID_T id, uint32_t start_address, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_security_register_write(enum HAL_FLASH_ID_T id, uint32_t start_address, const uint8_t *buffer, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_security_register_read(enum HAL_FLASH_ID_T id, uint32_t start_address, uint8_t *buffer, uint32_t len);
void hal_norflash_reset_remap(enum HAL_FLASH_ID_T id);
enum HAL_NORFLASH_RET_T hal_norflash_config_remap(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t addr, uint32_t len, uint32_t offset);
enum HAL_NORFLASH_RET_T hal_norflash_enable_remap(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id);
enum HAL_NORFLASH_RET_T hal_norflash_disable_remap(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id);
int hal_norflash_get_remap_status(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id);
void hal_norflash_boot_reset_remap(enum HAL_FLASH_ID_T id);
enum HAL_NORFLASH_RET_T hal_norflash_boot_config_remap(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t addr, uint32_t len, uint32_t offset);
enum HAL_NORFLASH_RET_T hal_norflash_boot_disable_remap(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id);
enum HAL_NORFLASH_RET_T hal_norflash_boot_enable_remap(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id);
int hal_norflash_boot_get_remap_status(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id);
#else /* !ARM_CMNS */
#define hal_norflash_get_id hal_norflash_get_id_se
#define hal_norflash_get_flash_total_size hal_norflash_get_flash_total_size_se
#define hal_norflash_get_size hal_norflash_get_size_se
#define hal_norflash_show_calib_result hal_norflash_show_calib_result_se
#define hal_norflash_erase hal_norflash_erase_se
#define hal_norflash_write hal_norflash_write_se
#define hal_norflash_read hal_norflash_read_se
#define hal_norflash_sleep hal_norflash_sleep_se
#define hal_norflash_wakeup hal_norflash_wakeup_se
#define hal_norflash_opened hal_norflash_opened_se
#define hal_norflash_get_open_state hal_norflash_get_open_state_se
#define hal_norflash_write_resume hal_norflash_write_resume_se
#define hal_norflash_erase_resume hal_norflash_erase_resume_se
#define hal_norflash_write_suspend hal_norflash_write_suspend_se
#define hal_norflash_erase_suspend hal_norflash_erase_suspend_se
#define hal_norflash_enable_remap hal_norflash_enable_remap_se
#define hal_norflash_disable_remap hal_norflash_disable_remap_se
#define hal_norflash_config_remap hal_norflash_config_remap_se
#define hal_norflash_get_remap_status hal_norflash_get_remap_status_se
#define hal_norflash_get_unique_id hal_norflash_get_unique_id_se
#define hal_norflash_disable_protection hal_norflash_disable_protection_se
#define hal_norflash_security_register_lock hal_norflash_security_register_lock_se
#define hal_norflash_security_register_read hal_norflash_security_register_read_se
#define hal_norflash_erase_remap hal_norflash_erase_remap_se
#define hal_norflash_write_remap hal_norflash_write_remap_se
#define hal_norflash_read_remap hal_norflash_read_remap_se

enum HAL_NORFLASH_RET_T hal_norflash_get_id_se(enum HAL_FLASH_ID_T id, uint8_t *value, uint32_t len);
uint32_t hal_norflash_get_flash_total_size_se(enum HAL_FLASH_ID_T id);
enum HAL_NORFLASH_RET_T hal_norflash_get_size_se0(uint32_t *total_size,
    uint32_t *block_size, uint32_t *sector_size, uint32_t *page_size);
enum HAL_NORFLASH_RET_T hal_norflash_get_size_se1(uint32_t *total_size,
    uint32_t *block_size, uint32_t *sector_size, uint32_t *page_size);
static inline enum HAL_NORFLASH_RET_T hal_norflash_get_size_se(enum HAL_FLASH_ID_T id, uint32_t *total_size,
    uint32_t *block_size, uint32_t *sector_size, uint32_t *page_size)
{
    if (id == HAL_FLASH_ID_0)
        return hal_norflash_get_size_se0(total_size, block_size, sector_size, page_size);
    else if (id == HAL_FLASH_ID_1)
        return hal_norflash_get_size_se1(total_size, block_size, sector_size, page_size);
    else
        return HAL_NORFLASH_BAD_ID;
}
void hal_norflash_show_calib_result_se(enum HAL_FLASH_ID_T id);
enum HAL_NORFLASH_RET_T hal_norflash_erase_se(enum HAL_FLASH_ID_T id, uint32_t start_address, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_write_se(enum HAL_FLASH_ID_T id, uint32_t start_address, const uint8_t *buffer, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_read_se(enum HAL_FLASH_ID_T id, uint32_t start_address, uint8_t *buffer, uint32_t len);
void hal_norflash_sleep_se(enum HAL_FLASH_ID_T id);
void hal_norflash_wakeup_se(enum HAL_FLASH_ID_T id);
int hal_norflash_opened_se(enum HAL_FLASH_ID_T id);
enum HAL_NORFLASH_RET_T hal_norflash_get_open_state_se(enum HAL_FLASH_ID_T id);
enum HAL_NORFLASH_RET_T hal_norflash_write_resume_se(enum HAL_FLASH_ID_T id, int suspend);
enum HAL_NORFLASH_RET_T hal_norflash_erase_resume_se(enum HAL_FLASH_ID_T id, int suspend);
enum HAL_NORFLASH_RET_T hal_norflash_enable_remap_se(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id);
enum HAL_NORFLASH_RET_T hal_norflash_disable_remap_se(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id);
enum HAL_NORFLASH_RET_T hal_norflash_config_remap_se0(enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t addr, uint32_t len, uint32_t offset);
enum HAL_NORFLASH_RET_T hal_norflash_config_remap_se1(enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t addr, uint32_t len, uint32_t offset);
static inline enum HAL_NORFLASH_RET_T hal_norflash_config_remap_se(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t addr, uint32_t len, uint32_t offset)
{
    if (id == HAL_FLASH_ID_0)
        return hal_norflash_config_remap_se0(remap_id, addr, len, offset);
    else if (id == HAL_FLASH_ID_1)
        return hal_norflash_config_remap_se1(remap_id, addr, len, offset);
    else
        return HAL_NORFLASH_BAD_ID;
}
int hal_norflash_get_remap_status_se(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id);
enum HAL_NORFLASH_RET_T hal_norflash_write_suspend_se0(uint32_t start_address, const uint8_t *buffer, uint32_t len, int suspend);
enum HAL_NORFLASH_RET_T hal_norflash_write_suspend_se1(uint32_t start_address, const uint8_t *buffer, uint32_t len, int suspend);
static inline enum HAL_NORFLASH_RET_T hal_norflash_write_suspend_se(enum HAL_FLASH_ID_T id, uint32_t start_address, const uint8_t *buffer, uint32_t len, int suspend)
{
    if (id == HAL_FLASH_ID_0)
        return hal_norflash_write_suspend_se0(start_address, buffer, len, suspend);
    else if (id == HAL_FLASH_ID_1)
        return hal_norflash_write_suspend_se1(start_address, buffer, len, suspend);
    else
        return HAL_NORFLASH_BAD_ID;
}
enum HAL_NORFLASH_RET_T hal_norflash_get_unique_id_se(enum HAL_FLASH_ID_T id, uint8_t *value, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_security_register_lock_se(enum HAL_FLASH_ID_T id, uint32_t start_address, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_security_register_read_se(enum HAL_FLASH_ID_T id, uint32_t start_address, uint8_t *buffer, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_erase_suspend_se(enum HAL_FLASH_ID_T id, uint32_t start_address, uint32_t len, int suspend);
enum HAL_NORFLASH_RET_T hal_norflash_disable_protection_se(enum HAL_FLASH_ID_T id);

enum HAL_NORFLASH_RET_T hal_norflash_read_remap_se0(enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t start_address, uint8_t *buffer, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_read_remap_se1(enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t start_address, uint8_t *buffer, uint32_t len);
static inline enum HAL_NORFLASH_RET_T hal_norflash_read_remap_se(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t start_address, uint8_t *buffer, uint32_t len)
{
    if (id == HAL_FLASH_ID_0)
        return hal_norflash_read_remap_se0(remap_id, start_address, buffer, len);
    else if (id == HAL_FLASH_ID_1)
        return hal_norflash_read_remap_se1(remap_id, start_address, buffer, len);
    else
        return HAL_NORFLASH_BAD_ID;
}
enum HAL_NORFLASH_RET_T hal_norflash_write_remap_se0(enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t start_address, uint8_t *buffer, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_write_remap_se1(enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t start_address, uint8_t *buffer, uint32_t len);
static inline enum HAL_NORFLASH_RET_T hal_norflash_write_remap_se(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t start_address, uint8_t *buffer, uint32_t len)
{
    if (id == HAL_FLASH_ID_0)
        return hal_norflash_write_remap_se0(remap_id, start_address, buffer, len);
    else if (id == HAL_FLASH_ID_1)
        return hal_norflash_write_remap_se1(remap_id, start_address, buffer, len);
    else
        return HAL_NORFLASH_BAD_ID;
}
enum HAL_NORFLASH_RET_T hal_norflash_erase_remap_se(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t start_address, uint32_t len);
#endif /* ARM_CMNS */

#ifdef __cplusplus
}
#endif

#endif /* NORFLASH_HAL_H */
