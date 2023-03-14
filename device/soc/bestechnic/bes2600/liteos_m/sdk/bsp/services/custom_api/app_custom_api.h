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
#ifndef __APP_CUSTOM_API__
#define __APP_CUSTOM_API__
#include "bt_if.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef IBRT

/*
 * provide the basic interface for develop ibrt ui and add more interface in this file if which is
 * only requested by the special customer.
*/
void app_custom_ui_ctx_init();

void app_custom_ui_notify_bluetooth_enabled(void);

void app_custom_ui_notify_bluetooth_disabled(void);

void app_custom_ui_handler_vender_evevnt(uint8_t evt_type, uint8_t * buffer, uint32_t length);

#endif

#ifdef __cplusplus
}
#endif

#endif
