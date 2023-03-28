/**
 * @file amsp.h
 * @author BES AI team
 * @version 0.1
 * @date 2020-10-23
 * 
 * @copyright Copyright (c) 2015-2020 BES Technic.
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
 */

#ifndef __AMSP_H__
#define __AMSP_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************header include********************************/
#include "prf.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "rwip_config.h"

/******************************macro defination*****************************/

/******************************type defination******************************/
typedef enum
{
    AMS_PROXY_SVC,

    AMS_PROXY_REMOTE_CMD_CHAR,
    AMS_PROXY_REMOTE_CMD_VAL,
    AMS_PROXY_REMOTE_CMD_CFG,
    AMS_PROXY_READY_CHAR,
    AMS_PROXY_READY_VAL,
    AMS_PROXY_READY_CFG,

    AMS_PROXY_ENT_UPD_CHAR,
    AMS_PROXY_ENT_UPD_VAL,
    AMS_PROXY_ENT_UPD_CFG,

    AMS_PROXY_ENT_ATTR_CHAR,
    AMS_PROXY_ENT_ATTR_VAL,

    AMS_PROXY_NUM,
} ams_gatt_handles;

typedef struct
{
    uint8_t conidx;
    bool ready;
    bool ready_ntf_enable;
    int client_handles[AMS_PROXY_NUM];
} ProxySubEnv_t;

typedef PRF_ENV_TAG(amsp)
{
    // By making this the first struct element, the proxy env can be stored in the
    // 'env' field of prf_task_env.
    prf_hdr_t prf_env;
    uint16_t start_hdl; // Service Start Handle
    ke_state_t state;   // State of different task instances
    ProxySubEnv_t subEnv[BLE_CONNECTION_MAX];
    /// GATT User local index
    uint8_t user_lid;
} PRF_ENV_T(amsp);

/****************************function declearation**************************/

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __AMSP_H__ */