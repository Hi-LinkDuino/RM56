/**
 ****************************************************************************************
 *
 * @file gapc.h
 *
 * @brief Generic Access Profile Controller - Native API .
 *
 * Copyright (C) RivieraWaves 2009-2020
 *
 ****************************************************************************************
 */


#ifndef _GAPC_H_
#define _GAPC_H_

/**
 ****************************************************************************************
 * @addtogroup GAPC Generic Access Profile Controller
 * @ingroup GAP
 * @brief  Generic Access Profile Controller.
 *
 * The GAP Controller module is responsible for providing an API to the application in
 * to perform GAP action related to a BLE connection (pairing, update parameters,
 * disconnect ...). GAP controller is multi-instantiated, one task instance per BLE
 * connection.
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "gapc_msg.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/*
 * MACROS
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Retrieve connection index from connection handle.
 *
 * @param[in] conhdl Connection handle
 *
 * @return Return found connection index, GAP_INVALID_CONIDX if not found.
 ****************************************************************************************
 */
uint8_t gapc_get_conidx(uint16_t conhdl);

/**
 ****************************************************************************************
 * @brief Retrieve connection handle from connection index.
 *
 * @param[in] conidx Connection index
 *
 * @return Return found connection handle, GAP_INVALID_CONHDL if not found.
 ****************************************************************************************
 */
uint16_t gapc_get_conhdl(uint8_t conidx);


/**
 ****************************************************************************************
 * @brief Check if a connection for provided connection index is established
 *
 * @param[in] conidx Connection index
 *
 * @return Return true if connection is established; false otherwise.
 ****************************************************************************************
 */
bool gapc_is_estab(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Retrieve connection role from connection index.
 *
 * @param[in] conidx Connection index
 *
 * @return Return found connection role
 ****************************************************************************************
 */
uint8_t gapc_get_role(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Retrieve BD address used by peer device on current link.
 *
 * @param[in] conidx Connection index
 *
 * @return Return found connection address
 ****************************************************************************************
 */
struct gap_bdaddr* gapc_peer_bdaddr_get(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Get if peer device supports a specific Low Energy feature
 *
 * @param[in] conidx    Connection index
 * @param[in] feature   Feature bit (@see enum gap_le_feature)
 *
 * @return Return true if remote features has been read and if supported, false otherwise
 ****************************************************************************************
 */
bool gapc_is_le_feat_supported(uint8_t conidx, uint8_t feature);

#if BLE_AUDIO_ENABLED
uint8_t gapc_get_conidx(uint16_t conhdl);
#endif

/// @} GAPC

#endif /* _GAPC_H_ */
