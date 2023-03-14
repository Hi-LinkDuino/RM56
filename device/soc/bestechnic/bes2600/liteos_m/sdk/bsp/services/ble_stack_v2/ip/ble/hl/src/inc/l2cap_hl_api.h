/**
 ****************************************************************************************
 *
 * @file l2cap_hl_api.h
 *
 * @brief Header file - L2CAP Internal HL API
 *
 * Copyright (C) RivieraWaves 2009-2019
 ****************************************************************************************
 */

#ifndef L2CAP_HL_API_H_
#define L2CAP_HL_API_H_

/**
 ****************************************************************************************
 * @addtogroup L2CAP
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#if (BLE_L2CAP)

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Create and Initialize the L2CAP module.
 *
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 ****************************************************************************************
 */
void ble_l2cap_init(uint8_t init_type);

/**
 ****************************************************************************************
 * @brief Handle BLE link creation
 *
 * @param[in] conidx        Connection Index
 ****************************************************************************************
 */
void l2cap_create(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Handle BLE link disconnection
 *
 * @param[in] conidx        Connection Index
 ****************************************************************************************
 */
void l2cap_cleanup(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Set the information about of lower layer buffers
 *
 * @param[in] buf_size     Buffer size
 * @param[in] nb_buf       Number of buffer available
 ****************************************************************************************
 */
void l2cap_chan_ll_buf_info_set(uint16_t buf_size, uint16_t nb_buf);


/**
 ****************************************************************************************
 * @brief Retrieve number of available buffers
 *
 * @return Number of available buffers
 ****************************************************************************************
 */
uint16_t l2cap_chan_ll_buf_nb_get(void);

/**
 ****************************************************************************************
 * @brief Internal function used to control reception of ACL packet at L2CAP level.
 *        When reception is off, received packet are put in a reception queue till
 *        reception is enabled again. When BLE connection is established without
 *        confirmation from application, RX is disabled by default.used to configure
 *        or reconfigure a registered channel.
 *
 * @param[in]  conidx       Connection Index
 * @param[in]  enable       0: Reception disabled, received ACL packets are put in a queue
 *                          1: Reception enabled
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t l2cap_rx_ctrl(uint8_t conidx, uint8_t enable);


/**
 ****************************************************************************************
 * @brief Internal function used to control usage of Enhanced L2CAP COC negotiation.
 *
 * @param[in]  conidx       Connection Index
 * @param[in]  enable       0: Disable enhanced COC negotiation
 *                          1: Enable enhanced COC negotiation
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t l2cap_coc_enhanced_nego_set(uint8_t conidx, uint8_t enable);


/**
 ****************************************************************************************
 * @brief Compute maximum SDU size that can be transmitted with current number of available credit.
 *
 * @param[in]  conidx       Connection Index
 * @param[in]  chan_lid     L2CAP Channel Local Identifier
 *
 * @return Maximum SDU size that can be transmitted with current number of available credit.
 ****************************************************************************************
 */
uint16_t l2cap_chan_max_sdu_tx_size_get(uint8_t conidx, uint8_t chan_lid);

void l2cap_data_rec_over_bt(uint8_t* ptrData, uint16_t dataLen);

#endif // (BLE_L2CAP)

/// @} L2CAP

#endif // L2CAP_HL_API_H_
