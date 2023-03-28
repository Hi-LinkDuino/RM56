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
#ifdef ARM_CMSE
#include "plat_types.h"
#include "plat_addr_map.h"
#include "cmsis.h"
#include "hal_location.h"
#include "hal_norflash.h"
#include "string.h"

#ifdef INTSRAM_RUN
#define FLASH_SE_TEXT_LOC
#else
#define FLASH_SE_TEXT_LOC           FLASH_TEXT_LOC
#endif

#define NORFLASH_SE_INTERFACE       FLASH_SE_TEXT_LOC __attribute__((cmse_nonsecure_entry))

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_get_id_se(enum HAL_FLASH_ID_T id, uint8_t *value, uint32_t len)
{
    return hal_norflash_get_id(id, value, len);
}

NORFLASH_SE_INTERFACE uint32_t hal_norflash_get_flash_total_size_se(enum HAL_FLASH_ID_T id)
{
    return hal_norflash_get_flash_total_size(id);
}
NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_get_size_se0(uint32_t *total_size,
                                              uint32_t *block_size, uint32_t *sector_size,
                                              uint32_t *page_size)
{
    return hal_norflash_get_size(HAL_FLASH_ID_0, total_size, block_size, sector_size, page_size);
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_get_size_se1(uint32_t *total_size,
                                              uint32_t *block_size, uint32_t *sector_size,
                                              uint32_t *page_size)
{
    return hal_norflash_get_size(HAL_FLASH_ID_1, total_size, block_size, sector_size, page_size);
}

NORFLASH_SE_INTERFACE void hal_norflash_show_calib_result_se(enum HAL_FLASH_ID_T id)
{
    return hal_norflash_show_calib_result(id);
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_erase_se(enum HAL_FLASH_ID_T id, uint32_t start_address, uint32_t len)
{
    return hal_norflash_erase(id, start_address, len);
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_write_se(enum HAL_FLASH_ID_T id, uint32_t start_address, const uint8_t *buffer, uint32_t len)
{
    return hal_norflash_write(id, start_address, buffer, len);
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_read_se(enum HAL_FLASH_ID_T id, uint32_t start_address, uint8_t *buffer, uint32_t len)
{
    // return hal_norflash_read(id, start_address, buffer, len);
#ifdef CHIP_BEST2003
    //2003 access FLASH secure region by FLASHX_BASE
    uint32_t total_size = hal_norflash_get_flash_total_size(id);
    start_address = (start_address & (total_size -1)) | (FLASHX_BASE);
#endif
    memcpy((void *)buffer, (void *)start_address, len);
    return HAL_NORFLASH_OK;
}

NORFLASH_SE_INTERFACE void hal_norflash_sleep_se(enum HAL_FLASH_ID_T id)
{
    return hal_norflash_sleep(id);
}

NORFLASH_SE_INTERFACE void hal_norflash_wakeup_se(enum HAL_FLASH_ID_T id)
{
    return hal_norflash_wakeup(id);
}

NORFLASH_SE_INTERFACE int hal_norflash_opened_se(enum HAL_FLASH_ID_T id)
{
    return hal_norflash_opened(id);
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_get_open_state_se(enum HAL_FLASH_ID_T id)
{
    return hal_norflash_get_open_state(id);
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_write_suspend_se0(uint32_t start_address, const uint8_t *buffer, uint32_t len, int suspend)
{
    return hal_norflash_write_suspend(HAL_FLASH_ID_0, start_address, buffer, len, suspend);
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_write_suspend_se1(uint32_t start_address, const uint8_t *buffer, uint32_t len, int suspend)
{
    return hal_norflash_write_suspend(HAL_FLASH_ID_1, start_address, buffer, len, suspend);
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_erase_suspend_se(enum HAL_FLASH_ID_T id, uint32_t start_address, uint32_t len, int suspend)
{
    return hal_norflash_erase_suspend(id, start_address, len, suspend);
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_write_resume_se(enum HAL_FLASH_ID_T id, int suspend)
{
    return hal_norflash_write_resume(id, suspend);
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_erase_resume_se(enum HAL_FLASH_ID_T id, int suspend)
{
    return hal_norflash_erase_resume(id, suspend);
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_enable_remap_se(enum HAL_FLASH_ID_T id)
{
    return hal_norflash_enable_remap(id, HAL_NORFLASH_REMAP_ID_0);
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_disable_remap_se(enum HAL_FLASH_ID_T id)
{
    return hal_norflash_disable_remap(id, HAL_NORFLASH_REMAP_ID_0);
}

NORFLASH_SE_INTERFACE int hal_norflash_get_remap_status_se(enum HAL_FLASH_ID_T id)
{
    return hal_norflash_get_remap_status(id, HAL_NORFLASH_REMAP_ID_0);
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_config_remap_se0(enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t addr, uint32_t len, uint32_t offset)
{
    return hal_norflash_config_remap(HAL_FLASH_ID_0, remap_id, addr, len, offset);
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_config_remap_se1(enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t addr, uint32_t len, uint32_t offset)
{
    return hal_norflash_config_remap(HAL_FLASH_ID_1, remap_id, addr, len, offset);
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_disable_protection_se(enum HAL_FLASH_ID_T id)
{
    return hal_norflash_disable_protection(id);
}

#ifdef FLASH_UNIQUE_ID
NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_get_unique_id_se(enum HAL_FLASH_ID_T id, uint8_t *value, uint32_t len)
{
    return hal_norflash_get_unique_id(id, value, len);
}
#endif

#ifdef FLASH_SECURITY_REGISTER
NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_security_register_lock_se(enum HAL_FLASH_ID_T id, uint32_t start_address, uint32_t len)
{
    return hal_norflash_security_register_lock(id, start_address, len);
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_security_register_read_se(enum HAL_FLASH_ID_T id, uint32_t start_address, uint8_t *buffer, uint32_t len)
{
    return hal_norflash_security_register_read(id, start_address, buffer, len);
}
#endif

SRAM_TEXT_LOC enum HAL_NORFLASH_RET_T hal_norflash_read_remap_sram(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t start_address, uint8_t *buffer, uint32_t len)
{
    hal_norflash_disable_remap(id, remap_id);

#ifdef CHIP_BEST2003
    //2003 access FLASH secure region by FLASHX_BASE
    uint32_t total_size = hal_norflash_get_flash_total_size(id);
    start_address = (start_address & (total_size -1)) | (FLASHX_BASE);
#endif
    memcpy((void *)buffer, (void *)start_address, len);

    hal_norflash_enable_remap(id, remap_id);
    return HAL_NORFLASH_OK;
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_read_remap_se0(enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t start_address, uint8_t *buffer, uint32_t len)
{
    return hal_norflash_read_remap_sram(HAL_FLASH_ID_0, remap_id, start_address, buffer, len);
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_read_remap_se1(enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t start_address, uint8_t *buffer, uint32_t len)
{
    return hal_norflash_read_remap_sram(HAL_FLASH_ID_1, remap_id, start_address, buffer, len);
}

SRAM_TEXT_LOC enum HAL_NORFLASH_RET_T hal_norflash_write_remap_sram(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t start_address, uint8_t *buffer, uint32_t len)
{
    enum HAL_NORFLASH_RET_T ret;

    hal_norflash_disable_remap(id, remap_id);
    ret = hal_norflash_write(id, start_address, buffer, len);
    hal_norflash_enable_remap(id, remap_id);

    return ret;
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_write_remap_se0(enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t start_address, uint8_t *buffer, uint32_t len)
{
    return hal_norflash_write_remap_sram(HAL_FLASH_ID_0, remap_id, start_address, buffer, len);
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_write_remap_se1(enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t start_address, uint8_t *buffer, uint32_t len)
{
    return hal_norflash_write_remap_sram(HAL_FLASH_ID_1, remap_id, start_address, buffer, len);
}

SRAM_TEXT_LOC enum HAL_NORFLASH_RET_T hal_norflash_erase_remap_sram(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t start_address, uint32_t len)
{
    enum HAL_NORFLASH_RET_T ret;

    hal_norflash_disable_remap(id, remap_id);
    ret = hal_norflash_erase(id, start_address, len);
    hal_norflash_enable_remap(id, remap_id);

    return ret;
}

NORFLASH_SE_INTERFACE enum HAL_NORFLASH_RET_T hal_norflash_erase_remap_se(enum HAL_FLASH_ID_T id, enum HAL_NORFLASH_REMAP_ID_T remap_id, uint32_t start_address, uint32_t len)
{
    return hal_norflash_erase_remap_sram(id, remap_id, start_address, len);
}
#endif
