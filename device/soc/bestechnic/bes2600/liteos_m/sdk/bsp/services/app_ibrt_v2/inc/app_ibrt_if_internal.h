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
#ifndef __APP_IBRT_IF_INTERNAL__
#define __APP_IBRT_IF_INTERNAL__
#ifdef __cplusplus
extern "C" {
#endif

void app_ibrt_if_ble_role_switch_start(void);
void app_ibrt_if_ai_role_switch_prepare(uint32_t *wait_ms);
void app_ibrt_if_ai_role_switch_handle(void);
void app_ibrt_if_ota_role_switch_handle(void);
bool app_ibrt_if_ota_is_in_progress(void);
#ifdef __cplusplus
}
#endif

#endif /*__APP_IBRT_IF_INTERNAL__*/
