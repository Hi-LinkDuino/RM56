/**
 ****************************************************************************************
 *
 * @file gaf_inc.h
 *
 * @brief Generic Audio Framework - Header file
 *
 * Copyright (C) RivieraWaves 2019-2020
 *
 ****************************************************************************************
 */

#ifndef GAF_INC_H_
#define GAF_INC_H_

/**
 ****************************************************************************************
 * @addtogroup GAF - Generic Audio Framework
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>         // Standard Integer Definitions
#include "ke_msg.h"         // Kernel Message Definitions

/*
 * GLOBAL FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Generic Audio Framework
 *
 * @param[in] init_type     Initialization type
 ****************************************************************************************
 */
void gaf_init(uint8_t init_type);

/**
 ****************************************************************************************
 * @brief Inform Generic Audio Framework block about a connection established
 *
 * @param[in] con_lid       Connection local index
 ****************************************************************************************
 */
void gaf_connect(uint8_t con_lid);

/**
 ****************************************************************************************
 * @brief Inform Generic Audio Framework block about a disconnection
 *
 * @param[in] con_lid       Connection local index
 ****************************************************************************************
 */
void gaf_disconnect(uint8_t con_lid);


/**
 ****************************************************************************************
 * @brief Check if connection local index is valid and if indicated connection is well
 * established
 *
 * @param[in] con_lid       Connection local index
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t gaf_check_con_lid(uint8_t con_lid);

/// @} GAF_INC

#endif // GAF_INC_H_
