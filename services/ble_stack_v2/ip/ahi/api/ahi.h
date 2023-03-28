/**
 ****************************************************************************************
 *
 * @file ahi.h
 *
 * @brief This file contains definitions related to the Application Host Interface
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef AHI_H_
#define AHI_H_

/**
 ****************************************************************************************
 * @addtogroup AHI Application Host Interface
 * @ingroup AHI
 * @brief Application Host Interface, based on AHI functionality.
 *
 *@{
 *
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (AHI_TL_SUPPORT)
#include <stdint.h>

/*
 * Defines
 ****************************************************************************************
 */
/// Kernel message type
#define AHI_KE_MSG_TYPE          0x10
/// Kernel message header length for transport through interface between App and SW stack.
#define AHI_KE_MSG_HDR_LEN       8
/// Maximum number of advertising reports that can be queued
#define AHI_MAX_ADV_REPORT       10

/// GAPM Reset size (LC + KE_HEADER + Reset_operation code)
#define AHI_RESET_MSG_LEN    (1+AHI_KE_MSG_HDR_LEN+1)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */
extern const uint8_t ahi_reset_msg[AHI_RESET_MSG_LEN];


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief AHI initialization function: initializes states and transport.
 *****************************************************************************************
 */
void ahi_init(void);

/**
 ****************************************************************************************
 * @brief Handle received AHI header through transport layer
 *
 * @param[in]  p_hdr_data   Pointer to received header
 * @param[out] p_rem_length Pointer to returned remaining length to receive.
 *
 * @return Pointer to data buffer where remaining data can be pushed.
 ****************************************************************************************
 */
uint8_t* ahi_rx_hdr_handle(uint8_t* p_hdr_data, uint16_t* p_rem_length);

/**
 ****************************************************************************************
 * @brief Inform AHI that payload data has been received
 *
 * @param[in]  p_data Pointer to received payload
 ****************************************************************************************
 */
void ahi_rx_done(uint8_t* p_data);

/**
 ****************************************************************************************
 * @brief Reset pattern received, handle it at AHI level
 ****************************************************************************************
 */
void ahi_reset(void);


/**
 ****************************************************************************************
 * @brief Retrieve Task Identifier from Task number
 * (automatically update index of task in returned task id)
 *
 * @param task Task number
 * @return Task Identifier
 ****************************************************************************************
 */
uint16_t ahi_get_id_from_task(uint16_t task);

/**
 ****************************************************************************************
 * @brief Retrieve Task Number from Task Identifier
 * (automatically update index of task in returned task id)
 *
 * @param id Task Identifier
 * @return Task Number
 ****************************************************************************************
 */
uint16_t ahi_get_task_from_id(uint16_t id);


#endif //AHI_TL_SUPPORT

/// @} AHI
#endif // AHI_H_
