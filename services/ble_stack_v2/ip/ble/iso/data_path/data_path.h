/**
 ****************************************************************************************
 *
 * @file data_path.h
 *
 * @brief Main API file for the Link Layer Data path manager
 *
 * Copyright (C) RivieraWaves 2009-2017
 *
 ****************************************************************************************
 */

#ifndef DATA_PATH_H_
#define DATA_PATH_H_

/**
 ****************************************************************************************
 * @defgroup DATA_PATH Link Layer ISO data path
 * @ingroup ROOT
 * @brief Link Layer ISO data path
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#if (BLE_ISO_PRESENT)

#include <stdbool.h>        // boolean definition
#include <stdint.h>         // integer definition


/*
 * DEFINES
 ****************************************************************************************
 */

/// Isochronous Channel data path selection
enum dp_type
{
    // -------- VENDOR SPECIFIC --------- //

    /// ISO Payload Generator
    ISO_DP_ISOGEN                   = 0xF2,
};


/// Invalid iso status field
enum sdu_status
{
    /// Correct SDU reception
    SDU_OK                 = 0x0,
    /// SDU partially received no error detected
    SDU_PARTIALLY_RECEIVED = 0x1,
    /// Some ISO packet in SDU reception has been lost
    SDU_PKT_LOST           = 0x2,
    /// A CRC error detected during reception
    SDU_CRC_ERR            = 0x3,
    /// SDU size exceed (due to a length error into an ISO packet)
    SDU_SIZE_EXCEED        = 0x4,
    /// No sync detected during reception of the packet
    SDU_SYNC_ERR           = 0x5,
    /// Wrong fragment received or unexpected LLID
    SDU_UNEXPECTED_FORMAT  = 0x6,
    /// SDU Not granted (never used for transmission or reception)
    SDU_NOT_GRANTED        = 0x7,
};


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Data path driver common interface
struct data_path_itf
{
    /**
     * Start the data path driver
     *
     * @param[in]     channel            Isochronous channel
     * @param[in]     channel type       Isochronous channel type (cis or bis channel)
     * @param[in]     sdu_interval       SDU interval (in us)
     * @param[in]     trans_latency      Transport latency (in us)
     * @param[in]     max_sdu            Maximum SDU size (in bytes) (12 bits meaningful)
     *
     * @return status of the operation (@see enum co_error)
     */
    #if BLE_AUDIO_ENABLED
    uint8_t (*cb_start)(uint8_t channel, uint8_t chan_type, uint32_t sdu_interval, uint32_t trans_latency, uint16_t max_sdu);
    #else
    uint8_t (*cb_start)(uint8_t channel, uint32_t sdu_interval, uint32_t trans_latency, uint16_t max_sdu);
    #endif

    /**
     * Stop the data path driver
     *
     * @param[in] channel Isochronous channel
     * @param[in] reason  Reason for stopping data path
     */
    void (*cb_stop)(uint8_t channel, uint8_t reason);

    /**
     ****************************************************************************************
     * Get SDU buffer structure for transmission or reception on an Isochronous link
     *
     * @param[in]     channel      Isochronous Channel number
     * @param[in|out] p_ref_time   ISOAL provides the reference anchor point for the new SDU, data path returns
     *                             the SDU reference time (only for input direction) (in us, based on Bluetooth timestamp)
     * @param[out]    p_sdu_len    Size of SDU (only for input direction)
     *
     * @return Pointer of the SDU buffer structure
     ****************************************************************************************
     */
    uint8_t* (*cb_sdu_get) (uint8_t channel, uint32_t* p_ref_time, uint16_t* p_sdu_len);

    /**
     * Indicate SDU transmission or reception is completed
     *
     * @param[in] channel  Isochronous channel
     * @param[in] sdu_len  Size of SDU received (only for output direction)
     * @param[in] ref_time SDU synchronization reference (only for output direction) (in us, based on Bluetooth timestamp)
     * @param[in] p_buf    Pointer of the SDU buffer
     * @param[in] status   Reception or transmission buffer status (@see enum sdu_status)
     */
    void (*cb_sdu_done)(uint8_t channel, uint16_t sdu_len, uint32_t ref_time, uint8_t* p_buf, uint8_t status);
};


/*
 * GLOBAL VARIABLE DEFINITION
 *****************************************************************************************
 */



/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the BLE Data Path driver
 *
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 ****************************************************************************************
 */
void data_path_init(uint8_t init_type);

/**
 ****************************************************************************************
 * @brief Retrieve the data path interface according to the direction
 *
 * @param[in]  type      Type of data path interface (@see enum iso_dp_type)
 * @param[in]  direction Data Path direction (@see enum iso_rx_tx_select)
 *
 * @return Pointer to the interface of the data path driver, NULL if no driver found
 ****************************************************************************************
 */
const struct data_path_itf* data_path_itf_get(uint8_t type, uint8_t direction);

/**
 ****************************************************************************************
 * Get if current data path is disabled or not.
 *
 * @param[in] dp         Data path
 *
 * @return True if data path is disabled, False else.
 ****************************************************************************************
 */
bool data_path_is_disabled(const struct data_path_itf* dp);

#endif // (BLE_ISO_PRESENT)
/// @} DATA_PATH

#endif // DATA_PATH_H_
