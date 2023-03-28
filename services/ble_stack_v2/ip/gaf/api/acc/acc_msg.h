/**
 ****************************************************************************************
 *
 * @file acc_msg.h
 *
 * @brief Audio Content Control - Definition of Kernel Messages
 *
 * Copyright (C) RivieraWaves 2019-2020
 *
 ****************************************************************************************
 */

#ifndef ACC_MSG_H_
#define ACC_MSG_H_

/**
 ****************************************************************************************
 * @addtogroup ACC Audio Content Control - Definition of Kernel Messages
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "acc.h"            // ACC Definitions

#if (GAF_ACC)

#include "gaf_msg.h"        // Generic Audio Framework API Message Definitions

/*
 * ENUMERATIONS
 ****************************************************************************************
 */


/// Features bit field meaning
enum acc_msg_feat_bf
{
    /// Telephone Bearer Service Server supported
    ACC_FEAT_TBS_SUPP_POS = 0,
    ACC_FEAT_TBS_SUPP_BIT = CO_BIT(ACC_FEAT_TBS_SUPP_POS),

    /// Telephone Bearer Service Client supported
    ACC_FEAT_TBC_SUPP_POS = 1,
    ACC_FEAT_TBC_SUPP_BIT = CO_BIT(ACC_FEAT_TBC_SUPP_POS),

    /// Media Control Service Server supported
    ACC_FEAT_MCS_SUPP_POS = 2,
    ACC_FEAT_MCS_SUPP_BIT = CO_BIT(ACC_FEAT_MCS_SUPP_POS),

    /// Media Control Service Client supported
    ACC_FEAT_MCC_SUPP_POS = 3,
    ACC_FEAT_MCC_SUPP_BIT = CO_BIT(ACC_FEAT_MCC_SUPP_POS),

    /// Object Transfer Service Server supported
    ACC_FEAT_OTS_SUPP_POS = 4,
    ACC_FEAT_OTS_SUPP_BIT = CO_BIT(ACC_FEAT_OTS_SUPP_POS),

    /// Object Transfer Service Client supported
    ACC_FEAT_OTC_SUPP_POS = 5,
    ACC_FEAT_OTC_SUPP_BIT = CO_BIT(ACC_FEAT_OTC_SUPP_POS),
};

/*
 * API MESSAGES
 ****************************************************************************************
 */

/// Structure for ACC_GET_FEATURES request message
typedef struct acc_get_features_req
{
    /// Request code (@see enum acc_msg_req_codes)
    uint16_t req_code;
} acc_get_features_req_t;

/// Structure for ACC_GET_FEATURES response message
typedef struct acc_get_features_rsp
{
    /// Request code (@see enum acc_msg_req_codes)
    uint16_t req_code;
    /// Status
    uint16_t status;
    /// Features bit field
    uint32_t feat_bf;
} acc_get_features_rsp_t;

#endif //(GAF_ACC)

/// @}

#endif // ACC_MSG_H_
