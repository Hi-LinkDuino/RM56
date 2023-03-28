/**
 ****************************************************************************************
 *
 * @file app_gaf.h
 *
 * @brief BLE Audio Generic Audio Framework
 *
 * Copyright 2015-2019 BES.
 *
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
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP_BAP
 * @{
 ****************************************************************************************
 */

#ifndef APP_GAF_H_
#define APP_GAF_H_

#if BLE_AUDIO_ENABLED

#ifdef __cplusplus
extern "C"{
#endif

#include "app_gaf_define.h"
#include "app_bap_capa_srv_msg.h"

extern const struct app_subtask_handlers app_gaf_handlers;

void app_gaf_earbuds_init(app_bap_capa_srv_dir_t *sink_capa_info, app_bap_capa_srv_dir_t *src_capa_info);

void app_gaf_mobile_init(void);

#ifdef __cplusplus
}
#endif

#endif
#endif // APP_GAF_H_

/// @} APP_GAF
