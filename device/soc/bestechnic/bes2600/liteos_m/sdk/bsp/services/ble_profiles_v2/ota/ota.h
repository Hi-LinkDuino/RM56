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


#ifndef _OTA_H_
#define _OTA_H_

/**
 ****************************************************************************************
 * @addtogroup OTA Datapath Profile Server
 * @ingroup OTAP
 * @brief Datapath Profile Server
 *
 * Datapath Profile Server provides functionalities to upper layer module
 * application. The device using this profile takes the role of Datapath Server.
 *
 * The interface of this role to the Application is:
 *  - Enable the profile role (from APP)
 *  - Disable the profile role (from APP)
 *  - Send data to peer device via notifications  (from APP)
 *  - Receive data from peer device via write no response (from APP)
 *
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_OTA)
#include "prf_types.h"
#include "prf.h"
#include "prf_utils.h"


/*
 * DEFINES
 ****************************************************************************************
 */
#define OTA_MAX_LEN                        (509)    

/*
 * DEFINES
 ****************************************************************************************
 */
/// Possible states of the OTA task
enum
{
    /// Idle state
    OTA_IDLE,
    /// Connected state
    OTA_BUSY,

    /// Number of defined states.
    OTA_STATE_MAX,
};

///Attributes State Machine
enum
{
    OTA_IDX_SVC,

    OTA_IDX_CHAR,
    OTA_IDX_VAL,
    OTA_IDX_NTF_CFG,

    OTA_IDX_NB,
};

/// Messages for OTA Profile 
enum ota_msg_id
{
    OTA_CCC_CHANGED = TASK_FIRST_MSG(TASK_ID_OTA),

    OTA_TX_DATA_SENT,

    OTA_DATA_RECEIVED,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Datapath Profile Server environment variable
typedef PRF_ENV_TAG(ota)
{
    /// profile environment
    prf_hdr_t   prf_env;
    uint8_t     ntfIndEnableFlag[BLE_CONNECTION_MAX];
    /// Service Start Handle
    uint16_t    shdl;
    /// GATT user local identifier
    uint8_t     user_lid;
    /// recorded the latest write request token
    uint16_t    rx_token;
} PRF_ENV_T(ota);

struct ble_ota_rx_data_ind_t
{
    uint16_t    length;
    uint8_t     data[0];
};

struct ble_ota_tx_sent_ind_t
{
    uint8_t     status;
};

struct ble_ota_send_data_req_t
{
    uint8_t     connecionIndex;
    uint32_t    length;
    uint8_t     value[__ARRAY_EMPTY];
};

struct ble_ota_tx_notif_config_t
{
    bool        ntfIndEnableFlag;
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Retrieve HRP service profile interface
 *
 * @return HRP service profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* ota_prf_itf_get(void);


/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * Initialize task handler
 *
 * @param task_desc Task descriptor to fill
 ****************************************************************************************
 */
void ota_task_init(struct ke_task_desc *task_desc);

bool ota_send_ind_ntf_generic(bool isNotification, uint8_t conidx, const uint8_t* ptrData, uint32_t length);

void ota_send_rx_cfm(uint8_t conidx);

#endif /* #if (BLE_OTA) */

/// @} OTA

#endif /* _OTA_H_ */

