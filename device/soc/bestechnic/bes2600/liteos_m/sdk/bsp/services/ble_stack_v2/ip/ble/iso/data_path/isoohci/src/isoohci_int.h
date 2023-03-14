/**
 ****************************************************************************************
 *
 * @file isoohci_int.h
 *
 * @brief Declaration of the functions used for Isochronous Payload over HCI Transport layer Internals
 *
 * Copyright (C) RivieraWaves 2009-2017
 *
 ****************************************************************************************
 */

#ifndef ISOOHCI_INT_H_
#define ISOOHCI_INT_H_

/**
 ****************************************************************************************
 * @addtogroup ISOOHCI_INT
 * @ingroup ISOOHCI
 * @brief Isochronous Payload over HCI Transport Layer Internals
 *
 * This module implements the primitives used for Isochronous Payload over HCI Transport layer
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_ISOOHCI)

#include <stdint.h>
#include <stdbool.h>
#include "co_math.h"

#include "arch.h"
#include "isoohci.h"

#include "co_list.h"
#include "co_utils.h"
#include "rwble_config.h"

/*
 * MACROS
 ****************************************************************************************
 */
/*
 * DEFINES
 ****************************************************************************************
 */

/// Size of the HCI ISO input buffer pool (in bytes)
#define ISOOHCI_IN_BUF_POOL_SIZE   ((CO_ALIGN4_HI(BLE_HCI_ISO_IN_BUF_NB * (sizeof(isoohci_buffer_t) + BLE_HCI_ISO_IN_BUF_SIZE))))

/// Invalid buffer index
#define ISOOHCI_INVALID_BUF_IDX    (0xFF)


/*
 * ENUMERATIONS
 ****************************************************************************************
 */
#if BLE_AUDIO_ENABLED
enum ble_iso_chan_type
{
#if (BLE_CIS)
    /// Connected Isochronous Stream Channel
    ISO_CHAN_CIS = 0,
#endif // (BLE_CIS)

#if (BLE_BIS)
    /// Broadcast Isochronous Stream Channel
    ISO_CHAN_BIS,
#endif // (BLE_BIS)

    /// Undefined channel
    ISO_CHAN_UNDEF,
};
#endif

/*
 * PROTOCOL STRUCTURES
 ****************************************************************************************
 */

/// SDU Buffer structure
typedef struct isoohci_buffer
{
    /// used to put buffer in a queue
    co_list_hdr_t   hdr;
    /// Time_Stamp
    uint32_t        time_stamp;
    /// Packet Sequence Number
    uint16_t        pkt_seq_nb;
    /// length of the ISO SDU (in bytes)
    uint16_t        sdu_length;
    /// Channel identifier
    uint8_t         channel;
    /// Buffer index
    uint8_t         buf_idx;
    /// Reception status (@see enum hci_iso_pkt_stat_flag)
    uint8_t         status;
    /// Area reserved for HCI header (used only for output direction - from Controller to Host)
    uint8_t         hci_iso_hdr[HCI_TRANSPORT_HDR_LEN + HCI_ISO_HDR_LEN];
    /**
     * ISO_Data_Load header
     *     - Output: always present, timestamp always included
     *     - Input: only in first fragment of an SDU, timestamp may or may not be present
     */
    uint8_t         hci_iso_data_load_hdr[HCI_ISO_DATA_LOAD_HDR_LEN_MAX];
    /// SDU
    uint8_t         sdu[__ARRAY_EMPTY];
} isoohci_buffer_t;


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/// Timing information about last transmitted SDU
struct isoohci_in_last_sdu_tx_sync
{
    /// The time stamp, in microseconds, of the reference anchor point of a transmitted SDU (Range 0x00000000-0xFFFFFFFF)
    uint32_t time_stamp;
    /// The time offset, in microseconds, that is associated with a transmitted SDU (Range 0x000000-0xFFFFFF)
    uint32_t time_offset;
    /// The packet sequence number of an SDU (Range 0x0000-0xFFFF)
    uint16_t pkt_seq_nb;
};

/// Isochronous Buffer management structure
struct isoohci_in_info
{
    /// Queue of buffers received from HCI
    co_list_t   in_queue;
    /// Timing information about last transmitted SDU
    struct isoohci_in_last_sdu_tx_sync last_sdu;
    /// Buffer of the current SDU in reception
    struct isoohci_buffer* current_buf;
    /// Max SDU Size (in bytes)
    uint16_t max_sdu_size;
    /// Length of SDU data received in current SDU
    uint16_t sdu_rx_len;
};

/// Isochronous Buffer management structure
struct isoohci_out_info
{
    /// SDU packet counter
    uint32_t    sdu_cnt;
    /// Buffer queue
    co_list_t   buffer_queue;
    /// Buffer queue (in use by transport)
    co_list_t   rx_queue;
    /// Total number of buffers
    uint8_t     nb_buf;
    /// Number of buffer in use (received from transport, in transmission to Host)
    uint8_t     nb_buf_in_use;
    /// Buffer index
    uint8_t     buf_idx;
    /// Data path to be removed
    bool remove;
};

/// Isochronous Payload over HCI TL environment definition
struct isoohci_env_tag
{
    /// Pool of input buffers (shared among all input paths)
    co_list_t   in_pool;
    /// Host to controller information
    struct isoohci_in_info*  p_in[BLE_ACTIVITY_MAX];
    /// Pointer array of available ISO channel path
    struct isoohci_out_info* p_out[BLE_ACTIVITY_MAX];
    /// list of TX buffer waiting to be sent
    struct co_list           hci_wait_tx_queue;
    /// list of TX buffer in HCI transmission queue
    struct co_list           hci_in_tx_queue;
    /// Buffer used for trash reception over transport layer
    uint8_t* trash_buffer;
    /// Event message table index
    uint8_t msg_tab_idx;
    /// Index of message to be sent
    uint8_t msg_tab_send_idx;
};


/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

/// Isochronous Payload over HCI TL environment
extern struct isoohci_env_tag isoohci_env;

/// Host To Controller interface
extern const struct data_path_itf isoohci_in_itf;
/// Controller to Host interface
extern const struct data_path_itf isoohci_out_itf;


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Defer Processing of Isochronous data into a kernel event. Host to controller
 ****************************************************************************************
 */
void isoohci_in_defer_handler(void);
/**
 ****************************************************************************************
 * @brief Defer Processing of Isochronous data into a kernel event. Controller to host
 ****************************************************************************************
 */
void isoohci_out_defer_handler(void);

/**
 ****************************************************************************************
 * @brief register a callback to inform upper handle data when iso receive data
 ****************************************************************************************
 */
void isoohci_data_comed_callback_register(void *callback);

/**
 ****************************************************************************************
 * @brief deregister the callback of data comed
 ****************************************************************************************
 */
void isoohci_data_comed_callback_deregister(void);

#endif //(BLE_ISOOHCI)

/// @} ISOOHCI

#endif // ISOOHCI_INT_H_
