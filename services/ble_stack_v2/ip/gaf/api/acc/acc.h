/**
 ****************************************************************************************
 *
 * @file acc.h
 *
 * @brief Audio Content Control - Header file
 *
 * Copyright (C) RivieraWaves 2019-2020
 *
 ****************************************************************************************
 */

#ifndef ACC_H_
#define ACC_H_

/**
 ****************************************************************************************
 * @addtogroup ACC Audio Content Control
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gaf_cfg.h"         // GAF Configuration
#include "gaf.h"             // GAF API

/*
 * MACROS
 ****************************************************************************************
 */


/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Module type values
enum acc_module_type
{
    /// Common
    ACC_MODULE_COMMON = 0,
    /// Telephone Bearer Server
    ACC_MODULE_TBS    = 1,
    /// Telephone Bearer Client
    ACC_MODULE_TBC    = 2,
    /// Media Control Server
    ACC_MODULE_MCS    = 3,
    /// Media Control Client
    ACC_MODULE_MCC    = 4,
    /// Object Transfer Server
    ACC_MODULE_OTS    = 5,
    /// Object Transfer Client
    ACC_MODULE_OTC    = 6,

    ACC_MODULE_MAX,
};

/// List of GAF_REQ request codes
enum acc_msg_req_codes
{
    ACC_GET_FEATURES = GAF_CODE(ACC, COMMON, 0),
};


/// @}

#endif // ACC_H_
