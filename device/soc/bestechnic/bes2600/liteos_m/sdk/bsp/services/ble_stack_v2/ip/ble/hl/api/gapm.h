/**
 ****************************************************************************************
 *
 * @file gapm.h
 *
 * @brief Generic Access Profile Manager - Native API .
 *
 * Copyright (C) RivieraWaves 2009-2020
 *
 ****************************************************************************************
 */


#ifndef _GAPM_H_
#define _GAPM_H_

/**
 ****************************************************************************************
 * @addtogroup GAPM Generic Access Profile Manager
 * @ingroup GAP
 * @brief Generic Access Profile Manager.
 *
 * The GAP Manager module is responsible for providing an API to the application in order
 * to manage all non connected stuff such as configuring device to go in desired mode
 * (discoverable, connectable, etc.) and perform required actions (scanning, connection,
 * etc.). GAP Manager is also responsible of managing GAP Controller state according to
 * corresponding BLE connection states.
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gapm_msg.h"
#include <stdbool.h>

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * STRUCTURE DEFINITION
 ****************************************************************************************
 */

// Only valid if Audio mode 0 is supported
/// Audio mode 0 HCI handlers
typedef struct gapm_am0_cb
{
    /**
     ***************************************************************************************
     * @brief Handle completion of MIC Less encryption Set command
     *
     * @param[in] conidx        Connection Index
     * @param[in] status        HCI execution status converted to HL error code (@see enum hl_err)
     ***************************************************************************************
     */
    void (*cb_mic_less_set_cmp)(uint8_t conidx, uint16_t status);

    /**
     ***************************************************************************************
     * @brief Handle completion of audio mode 0 stream setup hci command.
     *
     * @param[in] conidx        Connection Index
     * @param[in] status        HCI execution status converted to HL error code (@see enum hl_err)
     * @param[in] stream_hdl    Created stream connection handle
     ***************************************************************************************
     */
    void (*cb_setup_am0_stream_cmp)(uint8_t conidx, uint16_t status, uint16_t stream_hdl);
    /**
     ***************************************************************************************
     * @brief Handle completion of audio mode 0 stream remove hci command
     *
     * @param[in] stream_hdl    Stream connection handle
     * @param[in] status        HCI execution status converted to HL error code (@see enum hl_err)
     ***************************************************************************************
     */
    void (*cb_remove_am0_stream_cmp)(uint16_t stream_hdl, uint16_t status);

    /**
     ***************************************************************************************
     * @brief Handle completion of audio mode 0 datapath setup command
     *
     * @param[in] stream_hdl    Stream connection handle
     * @param[in] status        HCI execution status converted to HL error code (@see enum hl_err)
     ***************************************************************************************
     */
    void (*cb_setup_am0_datapath_cmp)(uint16_t stream_hdl, uint16_t status);

    /**
     ***************************************************************************************
     * @brief Handle completion of audio mode 0 datapath remove command
     *
     * @param[in] stream_hdl    Stream connection handle
     * @param[in] status        HCI execution status converted to HL error code (@see enum hl_err)
     ***************************************************************************************
     */
    void (*cb_remove_am0_datapath_cmp)(uint16_t stream_hdl, uint16_t status);

    /**
     ***************************************************************************************
     * @brief Handle ISO Gen statistic event
     *
     * @param[in] stream_hdl        Stream connection handle
     * @param[in] nb_tx             Statistics - Number of transmission attempts
     * @param[in] nb_tx_ok          Statistics - Number of transmission attempts that succeed
     * @param[in] nb_tx_not_granted Statistics - Number of Not granted packet packets
     * @param[in] nb_rx             Statistics - Number of reception attempt
     * @param[in] nb_rx_ok          Statistics - Number of reception attempts that succeed
     * @param[in] nb_rx_not_granted Statistics - Number of Not granted packet packets
     * @param[in] nb_rx_data_err    Statistics - Number of wrongly received packet (invalid data)
     * @param[in] nb_rx_crc_err     Statistics - Number of CRC Errors
     * @param[in] nb_rx_sync_err    Statistics - Number of SYNC Errors
     * @param[in] nb_rx_empty       Statistics - Number of received empty packets
     ***************************************************************************************
     */
    void (*cb_isogen_stat_evt)(uint16_t stream_hdl, uint32_t nb_tx, uint32_t nb_tx_ok, uint32_t nb_tx_not_granted,
                               uint32_t nb_rx, uint32_t nb_rx_ok, uint32_t nb_rx_not_granted, uint32_t nb_rx_data_err,
                               uint32_t nb_rx_crc_err, uint32_t nb_rx_sync_err, uint32_t nb_rx_empty);
} gapm_am0_cb_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ***************************************************************************************
 * @brief Function executed when procedure execution is over.
 *
 * @param[in] token         Procedure token provided by upper layer application
 * @param[in] status        Procedure execution status
 * @param[in] power_lvl     Advertising channel Tx power level
 ***************************************************************************************
 */
typedef void (*gapm_adv_tx_power_cb)(uint32_t token, uint16_t status, int8_t power_lvl);

/**
 ****************************************************************************************
 * @brief Generate a new token for any HL procedures
 *
 * @return New Generated token
 ***************************************************************************************
 */
uint16_t gapm_token_id_get(void);

/**
 ***************************************************************************************
 * @brief Get device advertising power level
 *
 * @param[in] token Procedure token provided by upper layer application
 * @param[in] res_cb
 *
 * @return Execution status
 ***************************************************************************************
 */
uint16_t gapm_adv_tx_power_get(uint32_t token, gapm_adv_tx_power_cb res_cb);


/**
 ***************************************************************************************
 * @brief Control flow of advertising reports. If disabled, reports are dropped without
 *        informing application.
 *
 * @param[in] enable   True to enable report, false to drop them
 *
 * @return Execution status
 ***************************************************************************************
 */
void gapm_adv_report_flow_ctrl(bool enable);

/**
 ***************************************************************************************
 * @brief Get advertising handle
 *
 * @param[in] actv_idx   Activity index
 *
 * @return Required advertising handle
 ***************************************************************************************
 */
uint8_t gapm_adv_hdl_get(uint8_t actv_idx);

/**
 ***************************************************************************************
 * @brief Get address used for an advertising activity
 *
 * @param[in] actv_idx   Activity index
 *
 * @return Pointer to required address
 ***************************************************************************************
 */
gap_addr_t* gapm_adv_get_addr(uint8_t actv_idx);

/**
 ***************************************************************************************
 * @brief Get periodic synchronization handle
 *
 * @param[in] actv_idx   Activity index
 *
 * @return Required periodic synchronization handle
 ***************************************************************************************
 */
uint16_t gapm_per_sync_hdl_get(uint8_t actv_idx);


/**
 ***************************************************************************************
 * @brief Set default handler for Audio mode 0 HCI events
 *
 * @param[in] p_am0_cb Pointer to handler callback set / NULL to unregister
 ***************************************************************************************
 */
uint16_t gapm_am0_handler_set(const gapm_am0_cb_t* p_am0_cb);

gap_addr_t* gapm_get_bdaddr(void);

gap_addr_t* gapm_get_connected_bdaddr(uint8_t conidx);

uint8_t gapm_get_conidx_by_bdaddr(uint8_t *btAddr);

uint8_t gapm_btgatt_con_create(uint8_t* btadress,uint16_t conn_handle);

void gapm_update_irk(uint8_t *irk);


/// @} GAPM

#endif /* _GAPM_H_ */
