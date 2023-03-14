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
#ifndef CACHE_HAL_H
#define CACHE_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

enum HAL_CACHE_ID_T {
    HAL_CACHE_ID_I_CACHE = 0,
    HAL_CACHE_ID_D_CACHE,
    HAL_CACHE_ID_NUM,
};

struct HAL_CACHE_MON_DATA_T {
    uint64_t r_hit;
    uint64_t r_miss;
    uint64_t w_hit;
    uint64_t w_miss;
};

uint8_t hal_cache_enable(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_disable(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_writebuffer_enable(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_writebuffer_disable(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_writebuffer_flush(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_writeback_enable(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_writeback_disable(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_wrap_enable(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_wrap_disable(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_invalidate_all(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_invalidate(enum HAL_CACHE_ID_T id, uint32_t start_address, uint32_t len);
uint8_t hal_cache_sync_all(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_sync(enum HAL_CACHE_ID_T id, uint32_t start_address, uint32_t len);
uint8_t hal_cache_monitor_enable(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_monitor_disable(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_get_monitor_data(enum HAL_CACHE_ID_T id, struct HAL_CACHE_MON_DATA_T *md);
void hal_cache_print_stats(void);
void hal_cache_sleep(void);
void hal_cache_wakeup(void);

uint8_t hal_cachecp_enable(enum HAL_CACHE_ID_T id);
uint8_t hal_cachecp_disable(enum HAL_CACHE_ID_T id);
uint8_t hal_cachecp_writebuffer_enable(enum HAL_CACHE_ID_T id);
uint8_t hal_cachecp_writebuffer_disable(enum HAL_CACHE_ID_T id);
uint8_t hal_cachecp_writebuffer_flush(enum HAL_CACHE_ID_T id);
uint8_t hal_cachecp_writeback_enable(enum HAL_CACHE_ID_T id);
uint8_t hal_cachecp_writeback_disable(enum HAL_CACHE_ID_T id);
uint8_t hal_cachecp_invalidate(enum HAL_CACHE_ID_T id, uint32_t start_address, uint32_t len);
uint8_t hal_cachecp_sync_all(enum HAL_CACHE_ID_T id);
uint8_t hal_cachecp_monitor_enable(enum HAL_CACHE_ID_T id);
uint8_t hal_cachecp_monitor_disable(enum HAL_CACHE_ID_T id);
uint8_t hal_cachecp_get_monitor_data(enum HAL_CACHE_ID_T id, struct HAL_CACHE_MON_DATA_T *md);
void hal_cachecp_print_stats(void);

#ifdef __cplusplus
}
#endif

#endif /* CACHE_HAL_H */
