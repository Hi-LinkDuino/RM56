/**
 ****************************************************************************************
 *
 * @file l2cap.h
 *
 * @brief Header file - L2CAP Native API.
 *
 * Copyright (C) RivieraWaves 2009-2019
 ****************************************************************************************
 */

#ifndef L2CAP_H_
#define L2CAP_H_

/**
 ****************************************************************************************
 * @addtogroup L2CAP Native API
 * @ingroup HOST
 * @brief Handles ALL messages to/from L2CAP block.
 *
 * The L2CAP module is responsible for SDU and PDU exchange over a BLE link, and Connection
 * oriented channel management.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "compiler.h"

#include <stdbool.h>
#include "l2cap_msg.h"
#include "co_buf.h"

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */
#define TRC_REQ_L2CAP_RX(conhdl, tx_len, buf_ptr)

/*
 * DEFINES
 ****************************************************************************************
 */
/// Invalid L2CAP channel local index
#define L2CAP_INVALID_CHAN_LID (0xFF)

/// Buffer Header length that must be reserved for L2CAP processing
#define L2CAP_BUFFER_HEADER_LEN (0)
/// Buffer Tail length that must be reserved for L2CAP processing
#define L2CAP_BUFFER_TAIL_LEN   (0)

/// Minimal LE MTU value
#define L2CAP_LE_MTU_MIN        (23)
/// Minimal L2CAP COC MTU / MPS value
#define L2CAP_COC_MTU_MIN       (64)

#define PREFERRED_BLE_MTU       (512)

/// Parameter to set in nb_chan of @see l2cap_coc_connect_cfm) to reject connection due to not enough authorization
#define L2CAP_COC_NOT_AUTORIZED (0xFF)

/// L2CAP channels id
/*@TRACE*/
enum l2cap_cid
{
    /// Reserved channel id
    L2CAP_CID_RESERVED                   = 0x0000,
    /// Attribute channel id
    L2CAP_CID_ATTRIBUTE                  = 0x0004,
    /// Signaling channel id
    L2CAP_CID_LE_SIGNALING               = 0x0005,
    /// Security channel id
    L2CAP_CID_SECURITY                   = 0x0006,
    /// Dynamically allocated minimum range
    L2CAP_CID_DYN_MIN                    = 0x0040,
    /// Dynamically allocated maximum range
    L2CAP_CID_DYN_MAX                    = 0x007F,
};

/// L2CAP assigned SPSM
/*@TRACE*/
enum l2cap_spsm_nb
{
    /// Attribute
    L2CAP_SPSM_ATT                   = 0x0027,
    /// Internet Protocol Support  Profile
    L2CAP_SPSM_IPSP                  = 0x0023,
    /// Object Transfer Service
    L2CAP_SPSM_OTS                   = 0x0025,
};

/*
 * CALLBACK DEFINITION
 ****************************************************************************************
 */

/// L2CAP Connection Oriented Channel callback set
typedef struct l2cap_chan_coc_cb
{
    /**
     ****************************************************************************************
     * @brief The received SDU buffer must be acquired by upper application module before
     *        function return.
     *        When SDU process is done, the corresponding SDU buffer must be release to
     *        allocate new reception credits onto a L2CAP dynamic channel.
     *
     * @param[in] conidx    Connection Index
     * @param[in] chan_lid  Connected L2CAP channel local index
     * @param[in] status    Reception status
     * @param[in] p_sdu     Buffer that contains SDU data
     ****************************************************************************************
     */
    void (*cb_sdu_rx) (uint8_t conidx, uint8_t chan_lid, uint16_t status, co_buf_t* p_sdu);

    /**
     ****************************************************************************************
     * @brief Function called when SDU has been transmitted or if an error occurs
     *
     * @param[in] conidx    Connection Index
     * @param[in] dummy     Dummy parameter provided by upper layer for command execution
     * @param[in] chan_lid  L2CAP channel local index
     * @param[in] status    Status of the operation (@see enum hl_err)
     * @param[in] p_sdu     Pointer to SDU transmitted
     ****************************************************************************************
     */
    void (*cb_sdu_sent) (uint8_t conidx, uint16_t dummy, uint8_t chan_lid, uint16_t status, co_buf_t* p_sdu);

    /**
     ****************************************************************************************
     * @brief Function called when Connection Oriented Channel creation operation is finished
     *
     * @param[in] conidx    Connection Index
     * @param[in] dummy     Dummy parameter provided by upper layer for command execution
     * @param[in] status    Status of the operation (@see enum hl_err)
     * @param[in] nb_chan   Number of L2CAP channel created.
     ****************************************************************************************
     */
    void (*cb_coc_create_cmp) (uint8_t conidx, uint16_t dummy, uint16_t status, uint8_t nb_chan);

    /**
     ****************************************************************************************
     * @brief Function called when a new Connection Oriented Channel is created
     *
     * @param[in] conidx       Connection Index
     * @param[in] dummy        Dummy parameter provided by upper layer for command execution
     * @param[in] chan_lid     Connected L2CAP channel local index
     * @param[in] local_rx_mtu Local device Maximum Transmit Unit reception size
     * @param[in] peer_rx_mtu  Peer device Maximum Transmit Unit reception size
     ****************************************************************************************
     */
    void (*cb_coc_created) (uint8_t conidx, uint16_t dummy, uint8_t chan_lid, uint16_t local_rx_mtu, uint16_t peer_rx_mtu);

    /**
     ****************************************************************************************
     * @brief Function called when Reconfigure L2CAP channel MTU is terminated
     *
     * @param[in] conidx    Connection Index
     * @param[in] dummy     Dummy parameter provided by upper layer for command execution
     * @param[in] status    Status of the operation (@see enum hl_err)
     ****************************************************************************************
     */
    void (*cb_coc_reconfigure_cmp) (uint8_t conidx, uint16_t dummy, uint16_t status);

    /**
     ****************************************************************************************
     * @brief Function called when Local or Peer RX MTU size has been changed onto the L2CAP channel
     *
     * @param[in] conidx       Connection Index
     * @param[in] dummy        Dummy parameter provided by upper layer for command execution
     * @param[in] chan_lid     L2CAP channel local index
     * @param[in] local_rx_mtu Local device Maximum Transmit Unit reception size
     * @param[in] peer_rx_mtu  Peer device Maximum Transmit Unit reception size
     ****************************************************************************************
     */
    void (*cb_coc_mtu_changed) (uint8_t conidx, uint16_t dummy, uint8_t chan_lid, uint16_t local_rx_mtu, uint16_t peer_rx_mtu);

    /**
     ****************************************************************************************
     * @brief Function called when a Connection Oriented Channel is terminated
     *
     * @param[in] conidx    Connection Index
     * @param[in] dummy     Dummy parameter provided by upper layer for command execution
     * @param[in] chan_lid  L2CAP channel local index
     * @param[in] reason    Termination Reason (@see enum hl_err)
     ****************************************************************************************
     */
    void (*cb_coc_terminated) (uint8_t conidx, uint16_t dummy, uint8_t chan_lid, uint16_t reason);

    /**
     ****************************************************************************************
     * @brief Function called when Connection Oriented Channel Termination operation is finished
     *
     * @param[in] conidx    Connection Index
     * @param[in] dummy     Dummy parameter provided by upper layer for command execution
     * @param[in] chan_lid  Connected L2CAP channel local index
     * @param[in] status    Status of the operation (@see enum hl_err)
     ****************************************************************************************
     */
    void (*cb_coc_terminate_cmp) (uint8_t conidx, uint16_t dummy, uint8_t chan_lid, uint16_t status);
} l2cap_chan_coc_cb_t;


/// L2CAP Channel callback set
typedef struct l2cap_chan_cb
{
    /**
     ****************************************************************************************
     * @brief The received SDU buffer must be acquired by upper application module before
     *        function return.
     *        When SDU process is done, the corresponding SDU buffer must be release to
     *        allocate new reception credits onto a L2CAP dynamic channel.
     *
     * @param[in] conidx    Connection Index
     * @param[in] chan_lid  Connected L2CAP channel local index
     * @param[in] status    Reception status
     * @param[in] p_sdu     Buffer that contains SDU data
     ****************************************************************************************
     */
    void (*cb_sdu_rx) (uint8_t conidx, uint8_t chan_lid, uint16_t status, co_buf_t* p_sdu);

    /**
     ****************************************************************************************
     * @brief Function called when SDU has been transmitted or if an error occurs
     *
     * @param[in] conidx    Connection Index
     * @param[in] dummy     Dummy parameter provided by upper layer for command execution
     * @param[in] chan_lid  L2CAP channel local index
     * @param[in] status    Status of the operation (@see enum hl_err)
     * @param[in] p_sdu     Pointer to SDU transmitted
     ****************************************************************************************
     */
    void (*cb_sdu_sent) (uint8_t conidx, uint16_t dummy, uint8_t chan_lid, uint16_t status, co_buf_t* p_sdu);
} l2cap_chan_cb_t;

/// L2CAP SPSM callback set
typedef struct l2cap_coc_spsm_cb
{
    /**
     ****************************************************************************************
     * @brief Function call when peer device requests to create several Connection oriented
     *        channels
     *
     *        In response of this function, the upper layer application must call
     *        @see l2cap_coc_connect_cfm function
     *
     * @param[in] conidx      Connection Index
     * @param[in] token       Token provided by L2CAP module that must be reused in confirmation function
     * @param[in] nb_chan     Number of L2CAP channel requested to be created in parallel
     * @param[in] spsm        Simplified Protocol/Service Multiplexer
     * @param[in] peer_rx_mtu Peer device Maximum Transmit Unit reception size
     ****************************************************************************************
     */
    void (*cb_coc_connect_req) (uint8_t conidx, uint16_t token, uint8_t nb_chan, uint16_t spsm, uint16_t peer_rx_mtu);
} l2cap_coc_spsm_cb_t;

/*
 * FUCTION DEFINITION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Command used to register a Simplified Protocol/Service Multiplexer in order to accept
 *        L2CAP connection oriented channel (COC) from a peer device. Security level
 *        parameter ensures that a certain level of security is met before requesting
 *        application to accept or reject channel establishment.
 *
 * @param[in] spsm     Simplified Protocol/Service Multiplexer
 * @param[in] sec_lvl_bf Security level bit field (@see enum l2cap_sec_lvl_bf)
 * @param[in] p_cb       Pointer to set of callback functions to be used for communication
 *                       with the Upper Layer
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t l2cap_coc_spsm_add(uint16_t spsm, uint8_t  sec_lvl_bf, const l2cap_coc_spsm_cb_t* p_cb);

/**
 ****************************************************************************************
 * @brief Command used to unregister a Simplified Protocol/Service Multiplexer.
 *
 * @param[in] spsm     Simplified Protocol/Service Multiplexer
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t l2cap_coc_spsm_remove(uint16_t spsm);

/**
 ****************************************************************************************
 * @brief Command use to initiate creation of several L2CAP connection oriented channels.
 *        The operation completes when all requested L2CAP channels are created or when
 *        an error is detected.
 *        If all channels cannot be created, reason is provided in error code.
 *
 * @param[in] conidx        Connection Index
 * @param[in] dummy         Dummy parameter provided by upper layer for command execution
 * @param[in] spsm        Simplified Protocol/Service Multiplexer
 * @param[in] nb_chan       Number of L2CAP channel to create in parallel
 * @param[in] local_rx_mtu  Local reception Maximum Transmit Unit Size
 * @param[in] p_cb          Pointer to set of callback functions to be used for
 *                          communication with the Upper Layer.
 *
 * @return Status of the function execution (@see enum hl_err)
 *         Consider status only if an error occurs; else wait for execution completion
 ****************************************************************************************
 */
uint16_t l2cap_coc_create(uint8_t conidx, uint16_t dummy, uint16_t spsm, uint8_t nb_chan, uint16_t local_rx_mtu,
                          const l2cap_chan_coc_cb_t* p_cb);

/**
 ****************************************************************************************
 * @brief Command use to accept or reject connection oriented channel establishement
 *
 * @param[in] conidx        Connection Index
 * @param[in] token         Value provided by L2CAP layer that must be reused in confirmation function
 * @param[in] nb_chan       Number of L2CAP channel local device accepts to create in parallel
 *                          0: reject all channel creation
 * @param[in] local_rx_mtu  Local reception Maximum Transmit Unit Size
 * @param[in] p_cb          Pointer to set of callback functions to be used for
 *                          communication with the Upper Layer.
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t l2cap_coc_connect_cfm(uint8_t conidx, uint16_t token, uint8_t nb_chan, uint16_t local_rx_mtu,
                              const l2cap_chan_coc_cb_t* p_cb);

/**
 ****************************************************************************************
 * @brief Command use to reconfigure a L2CAP connection oriented channel RX MTU
 *
 * @param[in] conidx        Connection Index
 * @param[in] dummy         Dummy parameter provided by upper layer for command execution
 * @param[in] local_rx_mtu  New Local reception Maximum Transmit Unit Size
 * @param[in] nb_chan       Number of L2CAP Channel local index in provided array
 * @param[in] p_chan_lid    Pointer to an array of L2CAP Channel local index
 *
 *
 * @return Status of the function execution (@see enum hl_err)
 *         Consider status only if an error occurs; else wait for execution completion
 ****************************************************************************************
 */
uint16_t l2cap_coc_reconfigure(uint8_t conidx, uint16_t dummy, uint16_t local_rx_mtu, uint8_t nb_chan,
                               uint8_t* p_chan_lid);

/**
 ****************************************************************************************
 * @brief Command use to initiate disconnection of a L2CAP connection oriented channel.
 *
 * @param[in] conidx        Connection Index
 * @param[in] dummy         Dummy parameter provided by upper layer for command execution
 * @param[in] chan_lid      L2CAP Channel local index
 *
 * @return Status of the function execution (@see enum hl_err)
 *         Consider status only if an error occurs; else wait for execution completion
 ****************************************************************************************
 */
uint16_t l2cap_coc_terminate(uint8_t conidx, uint16_t dummy, uint8_t chan_lid);

/**
 ****************************************************************************************
 * @brief Internal function used to register a fixed L2CAP channel such as ATT, Signaling or SMP.
 *        This channel does not support  segmentation and reassembly mechanism.
 *
 * @param[in]  conidx       Connection Index
 * @param[in]  cid          L2CAP Channel Identifier for both reception and transmission
 * @param[in]  mtu          Maximum Transmit Unit Size for both reception and transmission
 * @param[in]  p_cb         Pointer to set of callback functions to be used for communication with the Upper Layer.
 * @param[out] p_chan_lid   Pointer to L2CAP Channel local index
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t l2cap_chan_fix_register(uint8_t conidx, uint16_t cid, uint16_t mtu, const l2cap_chan_cb_t* p_cb,
                                 uint8_t* p_chan_lid);

/**
 ****************************************************************************************
 * @brief Internal function used to unregister a dynamic or a fixed channel.
 *        After sending this command, it’s no more possible to receive or send SDUs onto
 *        the channel.
 *
 * @param[in]  conidx       Connection Index
 * @param[in]  chan_lid     L2CAP Channel local index
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t l2cap_chan_unregister(uint8_t conidx, uint8_t chan_lid);

/**
 ****************************************************************************************
 * @brief Internal function used to update the L2CAP Fix Channel MTU
 *
 * @param[in]  conidx       Connection Index
 * @param[in]  chan_lid     Channel Local identifier
 * @param[in]  mtu          Maximum Transmit Unit Size for both reception and transmission
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t l2cap_chan_fix_mtu_update(uint8_t conidx, uint8_t chan_lid, uint16_t mtu);

/**
 ****************************************************************************************
 * @brief Function used to send a SDU onto a specific L2CAP fixed or dynamic channel.
 *
 * @note Upper layer shall wait for @see cb_sdu_sen callback before sending other
 *       message to prevent buffer overflow on the device.
 *
 * In debug mode, only L2CAP segment can be transmitted. This can be start segment or continuation segment.
 * L2CAP length and channel ID (CID) fields must be present in SDU data.
 *
 * @param[in] conidx        Connection Index
 * @param[in] dummy         Dummy parameter provided by upper layer for command execution
 * @param[in] chan_lid      L2CAP Channel local index
 * @param[in] dbg_bf        Debug bit field (@see enum l2cap_dbg_bf)
 * @param[in] p_sdu         Buffer that contains SDU data
 *
 * @return Status of the function execution (@see enum hl_err)
 *         Consider status only if an error occurs; else wait for execution completion
 ****************************************************************************************
 */
uint16_t l2cap_chan_sdu_send(uint8_t conidx, uint16_t dummy, uint8_t chan_lid, uint8_t dbg_bf, co_buf_t* p_sdu);

/// @} L2CAP

#endif // L2CAP_H_
