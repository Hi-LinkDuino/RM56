/**
 ****************************************************************************************
 *
 * @file iap.h
 *
 * @brief Isochronous Access Profile - Header file
 *
 * Copyright (C) RivieraWaves 2019-2020
 *
 ****************************************************************************************
 */

#ifndef IAP_H_
#define IAP_H_

/**
 ****************************************************************************************
 * @addtogroup IAP Isochronous Access Profile
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gaf_cfg.h"         // GAF Configuration
#include "gaf.h"             // GAF Definitions

/*
 * DEFINES
 ****************************************************************************************
 */

/// Length of broadcast code
#define IAP_BROADCAST_CODE_LEN      (16)

/*
 * MACROS
 ****************************************************************************************
 */

/// Mask for IAP error code
#define IAP_ERR_CODE(idx)           (0x1000 | idx)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Module type values
enum iap_module_type
{
    /// Common section
    IAP_MODULE_COMMON = 0,
    /// Unicast group management section
    IAP_MODULE_UG     = 1,
    /// Broadcast group management section
    IAP_MODULE_BG     = 2,
    /// Data path management section
    IAP_MODULE_DP     = 3,
    /// Test mode management section
    IAP_MODULE_TM     = 4,
    /// Debug section
    IAP_MODULE_DBG    = 5,

    IAP_MODULE_MAX,
};

/// List of GAF_CMD command codes
enum iap_cmd_codes
{
    /// Common commands
    IAP_GET_QUALITY = GAF_CODE(IAP, COMMON, 0),

    /// Unicast group management commands
    IAP_UG_UPDATE = GAF_CODE(IAP, UG, 0),
    IAP_US_ENABLE = GAF_CODE(IAP, UG, 1),
    IAP_US_DISABLE = GAF_CODE(IAP, UG, 2),
    IAP_UG_REMOVE = GAF_CODE(IAP, UG, 3),

    /// Broadcast group management commands
    IAP_BG_ENABLE = GAF_CODE(IAP, BG, 0),
    IAP_BG_SYNC = GAF_CODE(IAP, BG, 1),
    IAP_BG_DISABLE = GAF_CODE(IAP, BG, 2),
    IAP_BG_REMOVE = GAF_CODE(IAP, BG, 3),

    /// Data path management commands
    IAP_DP_VS_SET_TRIGGER = GAF_CODE(IAP, DP, 0),

    /// Test mode management commands
    IAP_TM_START = GAF_CODE(IAP, TM, 0),
    IAP_TM_CNT_GET = GAF_CODE(IAP, TM, 1),
    IAP_TM_STOP = GAF_CODE(IAP, TM, 2),
};

/// List of GAF_REQ request codes
enum iap_req_codes
{
    /// Common requests
    IAP_INTF_REGISTER = GAF_CODE(IAP, COMMON, 0),

    /// Unicast group management requests
    IAP_UG_ADD = GAF_CODE(IAP, UG, 0),
    IAP_UG_TEST_ADD = GAF_CODE(IAP, UG, 1),
    IAP_US_CONFIG = GAF_CODE(IAP, UG, 2),
    IAP_US_TEST_CONFIG = GAF_CODE(IAP, UG, 3),
    IAP_US_PREPARE = GAF_CODE(IAP, UG, 4),
    IAP_US_BIND = GAF_CODE(IAP, UG, 5),
    IAP_US_RELEASE = GAF_CODE(IAP, UG, 6),

    /// Broadcast group management requests
    IAP_BG_ADD = GAF_CODE(IAP, BG, 0),
    IAP_BG_TEST_ADD = GAF_CODE(IAP, BG, 1),

    /// Data path management requests
    IAP_DP_CONFIG = GAF_CODE(IAP, DP, 0),
    IAP_DP_SETUP = GAF_CODE(IAP, DP, 1),
    IAP_DP_REMOVE = GAF_CODE(IAP, DP, 2),

    /// Debug requests
    IAP_DBG_FEATURES = GAF_CODE(IAP, DBG, 0),
};

/// Error codes
enum iap_err
{
    /// No error
    IAP_ERR_NO_ERROR               = 0,
    /// Invalid parameters
    IAP_ERR_INVALID_PARAM          = IAP_ERR_CODE(0x01),
    /// Command disallwed
    IAP_ERR_COMMAND_DISALLOWED     = IAP_ERR_CODE(0x02),
    /// Insufficient ressources
    IAP_ERR_INSUFFICIENT_RESOURCES = IAP_ERR_CODE(0x03),
    /// Stream local index is valid but stream does not exist
    IAP_ERR_STREAM_NOT_FOUND       = IAP_ERR_CODE(0x04),
    /// Group local index is valid but group does not exists
    IAP_ERR_GROUP_NOT_FOUND        = IAP_ERR_CODE(0x05),
    /// Controller error
    IAP_ERR_LL_ERROR               = IAP_ERR_CODE(0x06),
    /// Busy
    IAP_ERR_BUSY                   = IAP_ERR_CODE(0x07),
    /// No more group can be added
    IAP_ERR_NO_GROUP_AVAILABLE     = IAP_ERR_CODE(0x08),
    /// Required number of streams cannot be added
    IAP_ERR_NO_STREAM_AVAILABLE    = IAP_ERR_CODE(0x09),
    /// Group already exists and cannot be added
    IAP_ERR_GROUP_ALREADY_EXISTS   = IAP_ERR_CODE(0x0A),
    /// Unexpected error
    IAP_ERR_UNEXPECTED_ERROR       = IAP_ERR_CODE(0x0B),
    /// Unknown request
    IAP_ERR_UNKNOWN_REQUEST        = IAP_ERR_CODE(0x0C),
    /// Unknown command
    IAP_ERR_UNKNOWN_COMMAND        = IAP_ERR_CODE(0x0D),
    /// Procedure cancelled by upper layer
    IAP_ERR_CANCELLED              = IAP_ERR_CODE(0x0E),
    /// Synchronization lost
    IAP_ERR_SYNC_LOST              = IAP_ERR_CODE(0x0F),
    /// Interface already exists
    IAP_ERR_INTF_EXISTS            = IAP_ERR_CODE(0x10),
    /// No interface available
    IAP_ERR_NO_INTF_AVAILABLE      = IAP_ERR_CODE(0x11),
    /// Unknown interface
    IAP_ERR_UNKNOWN_INTERFACE      = IAP_ERR_CODE(0x12),
    /// Data path can be setup because stream is not enabled
    IAP_ERR_STREAM_NOT_ENABLED     = IAP_ERR_CODE(0x13),

    /// Data path is setup and cannot be reconfigured
    IAP_ERR_DP_SETUP               = IAP_ERR_CODE(0x14),
    /// Data path is not setup and cannot be removed
    IAP_ERR_DP_NOT_SETUP           = IAP_ERR_CODE(0x15),
    /// Data path is not configured and cannot be setup
    IAP_ERR_DP_NOT_CONFIGURED      = IAP_ERR_CODE(0x16),
};

/// Group type values
enum iap_group_types
{
    /// Unicast group
    IAP_GROUP_TYPE_UG = 0,
    /// Broadcast group
    IAP_GROUP_TYPE_BG,
};

/// Data path values
enum iap_dp
{
    /// HCI
    IAP_DP_HCI = 0,
    /// ISO over HCI Data Path
    IAP_DP_ISOOHCI = 0xF0,
    /// ISO Payload Generator
    IAP_DP_ISOGEN = 0xF2,
    /// Data path disabled
    IAP_DP_DISABLED = 0xFF,
};

/// Data path direction values
enum iap_dp_direction
{
    /// Input (Host to Controller)
    IAP_DP_DIRECTION_INPUT = 0,
    /// Output (Controller to Host)
    IAP_DP_DIRECTION_OUTPUT,

    IAP_DP_DIRECTION_MAX,
};

/// Data path direction bit field
enum iap_dp_direction_bf
{
    /// Input data path
    IAP_DP_DIRECTION_IN_POS = IAP_DP_DIRECTION_INPUT,
    IAP_DP_DIRECTION_IN_BIT = CO_BIT(IAP_DP_DIRECTION_IN_POS),

    /// Output data path
    IAP_DP_DIRECTION_OUT_POS = IAP_DP_DIRECTION_OUTPUT,
    IAP_DP_DIRECTION_OUT_BIT = CO_BIT(IAP_DP_DIRECTION_OUT_POS),

    /// Both direction
    IAP_DP_DIRECTION_BOTH = IAP_DP_DIRECTION_IN_BIT | IAP_DP_DIRECTION_OUT_BIT,
};

/// Data path update types
enum iap_dp_update
{
    /// Setup
    IAP_DP_UPDATE_SETUP = 0,
    /// Remove
    IAP_DP_UPDATE_REMOVE,
};

/// Reason codes
enum iap_reason_codes
{ // Some codes are never used
    /// Request coming from upper layer
    IAP_REASON_UPPER_REQUEST = 0,
    /// Request coming from peer device
    IAP_REASON_PEER_REQUEST,
    /// Synchronization lost
    IAP_REASON_SYNC_LOST,
    /// Connection lost due to encryption error
    IAP_REASON_MIC_ERROR,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// CIS/Connection binding information structure
typedef struct iap_us_bind_info
{
    /// Stream local index
    uint8_t stream_lid;
    /// Connection local index
    uint8_t con_lid;
} iap_us_bind_info_t;

/// Unicast group common parameters structure
typedef struct iap_ug_param_common
{
    /// SDU interval from Master to Slave in microseconds
    /// From 0xFF (255us) to 0xFFFF (1.048575s)
    uint32_t sdu_intv_m2s_us;
    /// SDU interval from Slave to Master in microseconds
    /// From 0xFF (255us) to 0xFFFF (1.048575s)
    uint32_t sdu_intv_s2m_us;
    /// Sequential or interleaved scheduling
    uint8_t packing;
    /// Unframed or framed mode
    uint8_t framing;
    /// Worst slow clock accuracy of slaves
    uint8_t sca;
} iap_ug_param_common_t;

/// Unicast group information structure
typedef struct iap_ug_param
{
    /// Common parameters
    iap_ug_param_common_t common;
    /// Maximum time (in milliseconds) for an SDU to be transported from master controller to slave
    /// controller. From 0x5 (5ms) to 0xFA0 (4s)
    uint16_t tlatency_m2s_ms;
    /// Maximum time (in milliseconds) for an SDU to be transported from slave controller to master
    /// controller. From 0x5 (5ms) to 0xFA0 (4s)
    uint16_t tlatency_s2m_ms;
} iap_ug_param_t;

/// Unicast group test information structure
typedef struct iap_ug_test_param
{
    /// Common information
    iap_ug_param_common_t common;
    /// Flush timeout in milliseconds for each payload sent from Master to Slave
    uint16_t ft_m2s_ms;
    /// Flush timeout in milliseconds for each payload sent from Slave to Master
    uint16_t ft_s2m_ms;
    /// ISO interval in milliseconds
    uint16_t iso_intv_ms;
} iap_ug_test_param_t;

/// Unicast stream common information structure
typedef struct iap_us_param_common
{
    /// Maximum size of an SDU provided by master host
    uint16_t max_sdu_m2s;
    /// Maximum size of an SDU provided by slave host
    uint16_t max_sdu_s2m;
    /// PHYs on which packets may be transmitted from master to slave
    uint8_t phy_m2s;
    /// PHYs on which packets may be transmitted from slave to master
    uint8_t phy_s2m;
} iap_us_param_common_t;

/// Unicast stream information structure
typedef struct iap_us_param
{
    /// Common information
    iap_us_param_common_t common;
    /// Maximum number of times every data PDU should be retransmitted for master to slave. From 0x0 to 0xF
    uint8_t rtn_m2s;
    /// Maximum number of times every data PDU should be retransmitted for slave to master. From 0x0 to 0xF
    uint8_t rtn_s2m;
} iap_us_param_t;

/// Unicast stream test information structure
typedef struct iap_us_test_param
{
    /// Common information
    iap_us_param_common_t common;
    /// Maximum size of the payload from master to slave. From 0x0 to 0xFB
    uint8_t max_pdu_m2s;
    /// Maximum size of the payload from slave to master. From 0x0 to 0xFB
    uint8_t max_pdu_s2m;
    /// Burst number from master to slave. From 0x0 to 0xF
    uint8_t bn_m2s;
    /// Burst number from slave to master. From 0x0 to 0xF
    uint8_t bn_s2m;
    /// Maximum number of subevents in each stream interval. From 0x1 to 0x1F
    uint8_t nse;
} iap_us_test_param_t;

/// Unicast group configuration structure (provided by controller after stream establisment)
typedef struct iap_ug_config
{
    /// Group synchronization delay time in microseconds
    uint32_t sync_delay_us;
    /// The maximum time, in microseconds, for transmission of SDUs of all CISes from master to slave
    /// (range 0x0000EA to 0x7FFFFF)
    uint32_t tlatency_m2s_us;
    /// The maximum time, in microseconds, for transmission of SDUs of all CISes from slave to master
    /// (range 0x0000EA to 0x7FFFFF)
    uint32_t tlatency_s2m_us;
} iap_ug_config_t;

/// Unicast stream configuration structure (provided by controller after stream establisment)
typedef struct iap_us_config
{
    /// Stream synchronization delay time in microseconds
    uint32_t sync_delay_us;
    /// Master to slave PHY, bit 0: 1Mbps, bit 1: 2Mbps, bit 2: LE-Coded
    uint8_t phy_m2s;
    /// Slave to master PHY, bit 0: 1Mbps, bit 1: 2Mbps, bit 2: LE-Coded
    uint8_t phy_s2m;
} iap_us_config_t;

/// Broadcast group common parameter structure
typedef struct iap_bg_param_common
{
    /// SDU interval in microseconds
    uint32_t sdu_intv_us;
    /// Maximum size of an SDU
    uint16_t max_sdu;
    /// Sequential or interleaved scheduling
    uint8_t packing;
    /// Unframed or framed mode
    uint8_t framing;
    /// Bitfield indicating PHYs than can be used by the controller for transmissions of SDUs
    uint8_t phy_bf;
} iap_bg_param_common_t;

/// Broadcast group information structure
typedef struct iap_bg_param
{
    /// Common information
    iap_bg_param_common_t common_param;
    /// Maximum time (in milliseconds) between the first transmission of an SDU to the end of the last
    /// transmission of the same SDU
    uint16_t max_tlatency_ms;
    /// Number of times every PDU should be transmitted
    uint8_t rtn;
} iap_bg_param_t;

/// Broadcast group test information structure
typedef struct iap_bg_test_param
{
    /// Common information
    iap_bg_param_common_t common_param;
    /// ISO interval in multiple of 1.25ms. From 0x4 (5ms) to 0xC80 (4s)
    uint16_t iso_intv_frame;
    /// Number of subevents in each interval of each stream in the group
    uint8_t nse;
    /// Maximum size of a PDU
    uint8_t max_pdu;
    /// Burst number (number of new payload in each interval). From 1 to 7
    uint8_t bn;
    /// Number of times the scheduled payload is transmitted in a given event. From 0x1 to 0xF
    uint8_t irc;
    /// Isochronous Interval spacing of payloads transmitted in the pre-transmission subevents.
    /// From 0x00 to 0x0F
    uint8_t pto;
} iap_bg_test_param_t;

/// Broadcast group configuration structure (provided by controller after stream establisment)
typedef struct iap_bg_config
{
    /// Transmission delay time in microseconds of all BISs in the BIG (in us range 0x0000EA-0x7FFFFF)
    uint32_t sync_delay_us;
    /// The maximum delay time, in microseconds, for transmission of SDUs of all BISes
    /// (in us range 0x0000EA-0x7FFFFF)
    uint32_t tlatency_us;
    /// PHY used, bit 0: 1Mbps, bit 1: 2Mbps, bit 2: LE-Coded
    uint8_t phy;
    /// The number of subevents in each BIS event in the BIG, range 0x01-0x1E
    uint8_t nse;
    /// The number of new payloads in each BIS event, range 0x01-0x07
    uint8_t bn;
    /// Offset used for pre-transmissions, range 0x00-0x0F
    uint8_t pto;
    /// The number of times a payload is transmitted in a BIS event, range 0x01-0x0F
    uint8_t irc;
    /// Maximum size of the payload in octets, range 0x00-0xFB
    uint8_t max_pdu;
} iap_bg_config_t;

/// Data path configuration structure
typedef struct iap_dp_config
{
    /// Controller Delay in microseconds
    uint32_t        ctl_delay_us;
    /// Codec ID
    uint8_t         codec_id[GAF_CODEC_ID_LEN];
    /// Datapath ID
    uint8_t         dp_id;
    /// Codec Configuration length
    uint8_t         cfg_len;
    /// Additional Codec Configuration length
    uint8_t         add_cfg_len;
    /// Pointer to Codec Configuration (LTV format)
    const uint8_t*  p_cfg;
    /// Pointer to additional Codec Configuration (LTV format)
    const uint8_t*  p_add_cfg;
} iap_dp_config_t;

/*
 * CALLBACK FUNCTIONS DEFINITION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Callback function called when a command has been fully processed
 *
 * @param[in] cmd_code      Command code
 * @param[in] status        Status
 * @param[in] group_lid     Group local index
 * @param[in] stream_lid    Stream local index
 ****************************************************************************************
 */
typedef void (*iap_cb_cmp_evt)(uint8_t intf_lid, uint16_t cmd_code, uint16_t status,
                               uint8_t group_lid, uint8_t stream_lid);

/**
 ****************************************************************************************
 * @brief Callback function called when an unicast stream has been enabled
 *
 * @param[in] stream_lid    Stream local index
 ****************************************************************************************
 */
typedef void (*iap_cb_us_enabled)(uint8_t intf_lid, uint8_t group_lid, uint8_t stream_lid,
                                  iap_ug_config_t* p_ug_cfg, iap_us_config_t* p_us_cfg);

/**
 ****************************************************************************************
 * @brief Callback function called when an unicast stream has been disabled
 *
 * @param[in] stream_lid    Stream local index
 ****************************************************************************************
 */
typedef void (*iap_cb_us_disabled)(uint8_t intf_lid, uint8_t stream_lid, uint8_t reason);

/**
 ****************************************************************************************
 * @brief Callback function called when an broadcast group has been enabled
 *
 * @param[in] group_lid    Group local index
 ****************************************************************************************
 */
typedef void (*iap_cb_bg_enabled)(uint8_t intf_lid, uint8_t group_lid, iap_bg_config_t* p_cfg);

/**
 ****************************************************************************************
 * @brief Callback function called when an broadcast group has been disabled
 *
 * @param[in] group_lid    Group local index
 * @param[in] reason       Reason
 * @param[in] subreason    Received reason if group is not removed due to upper layer request.
 ****************************************************************************************
 */
typedef void (*iap_cb_bg_disabled)(uint8_t intf_lid, uint8_t group_lid, uint8_t reason);

/**
 ****************************************************************************************
 * @brief Callback function called when synchronization with a broadcast group has been established
 *
 * @param[in] group_lid             Group local index
 * @param[in] transport_delay_us    Maximum time in microseconds for reception of SDEs of all streams
 ****************************************************************************************
 */ // TODO FIXME: use nowhere
typedef void (*iap_cb_bg_sync)(uint8_t intf_lid, uint8_t group_lid, uint32_t transport_delay_us);

/**
 ****************************************************************************************
 * @brief Callback function called when data path state for a stream has been updated
 *
 * @param[in] intf_lid      Interface local index
 * @param[in] group_lid     Group local index
 * @param[in] stream_lid    Stream local index
 * @param[in] dp_update     Data path update type
 * @param[in] direction     Direction for setup update, direction bit field for remove update
 * @param[in] status        Update status
 ****************************************************************************************
 */
typedef void (*iap_cb_dp_update)(uint8_t intf_lid, uint8_t group_lid, uint8_t stream_lid, uint8_t dp_update,
                                 uint8_t direction, uint16_t status);

/**
 ****************************************************************************************
 * @brief Callback function called when test mode counter have been received
 *
 * @param[in] stream_lid    Stream local index
 * @param[in] nb_rx         Number of received packets
 * @param[in] nb_missed     Number of missed packets
 * @param[in] nb_failed     Number of failed packets
 ****************************************************************************************
 */
typedef void (*iap_cb_tm_cnt)(uint8_t intf_lid, uint8_t stream_lid, uint32_t nb_rx,
                              uint32_t nb_missed, uint32_t nb_failed);

/**
 ****************************************************************************************
 * @brief Callback function called when test mode counter have been received
 *
 * @param[in] stream_lid    Stream local index
 ****************************************************************************************
 */
typedef void (*iap_cb_quality_cmp_evt)(uint8_t intf_lid, uint16_t status, uint8_t stream_lid,
                                       uint32_t tx_unacked_packets, uint32_t tx_flushed_packets,
                                       uint32_t tx_last_subevent_packets, uint32_t retransmitted_packets,
                                       uint32_t crc_error_packets, uint32_t rx_unreceived_packets,
                                       uint32_t duplicate_packets);

/// Set of callback functions structure
typedef struct iap_cb
{
    /// Command complete event callback function
    iap_cb_cmp_evt cb_cmp_evt;
    /// Unicast stream enabled indication callback function
    iap_cb_us_enabled cb_us_enabled;
    /// Unicast stream disabled indication callback function
    iap_cb_us_disabled cb_us_disabled;
    /// Broadcast group enabled indication callback function
    iap_cb_bg_enabled cb_bg_enabled;
    /// Broadcast group disabled indication callback function
    iap_cb_bg_disabled cb_bg_disabled;
    /// Data path update indication callback function
    iap_cb_dp_update cb_dp_update;
    /// Test mode counter indication callback function
    iap_cb_tm_cnt cb_tm_cnt;
    /// Get link quality command complete event callback function
    iap_cb_quality_cmp_evt cb_quality_cmp_evt;
} iap_cb_t;

/*
 * API FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Register a new interface
 *
 * @param[out] p_intf_lid   Address at which allocated interface index must be returned
 * @param[in] p_cb          Pointer to set of callback functions to be used for this interface
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t iap_intf_register(uint8_t* p_intf_lid, const iap_cb_t* p_cb);

/**
 ****************************************************************************************
 * @brief Get quality information for an ISO Link
 *
 * @param[in] stream_lid       Stream local index
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t iap_gen_get_quality(uint8_t stream_lid);

#if (GAF_UNICAST_SUPP)
/**
 ****************************************************************************************
 * @brief Add a unicast group
 *
 * @param[in] p_group_info    Pointer to Unicast Group information structure
 * @param[in] cig_id          CIG ID
 * @param[in] intf_lid        Interface local index for the group
 * @param[out] p_group_lid    Pointer at which allocated group local index is returned
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_ug_add(iap_ug_param_t* p_params, uint8_t cig_id, uint8_t intf_lid, uint8_t* p_group_lid);

/**
 ****************************************************************************************
 * @brief Add a unicast group for test
 *
 * @param[in] p_group_info    Pointer to Unicast Group test information structure
 * @param[in] cig_id          CIG ID
 * @param[in] intf_lid        Interface local index for the group
 * @param[out] p_group_lid    Pointer at which allocated group local index is returned
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_ug_test_add(iap_ug_test_param_t *p_group_info, uint8_t cig_id, uint8_t intf_lid,
                         uint8_t* p_group_lid);
/**
 ****************************************************************************************
 * @brief Configure a unicast stream
 *
 * @param[in] group_lid       Stream local index
 * @param[in] p_stream_info   Pointer to Unicast Stream information structure
 * @param[in] cis_id          CIS ID
 * @param[out] p_stream_lid   Pointer at which allocated stream local index will be returned
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_us_config(uint8_t group_lid, iap_us_param_t* p_stream_info, uint8_t cis_id,
                       uint8_t* p_stream_lid);

/**
 ****************************************************************************************
 * @brief Configure a unicast stream for test
 *
 * @param[in] group_lid       Group local index
 * @param[in] p_stream_info   Pointer to Unicast Stream test information structure
 * @param[in] cis_id          CIS ID
 * @param[out] p_stream_lid   Pointer at which allocated stream local index will be returned
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_us_test_config(uint8_t group_lid, iap_us_test_param_t *p_stream_info, uint8_t cis_id,
                            uint8_t* p_stream_lid);

/**
 ****************************************************************************************
 * @brief Inform IAP that establishment of an unicast stream can be accepted
 *
 * @param[in] cig_id          CIG ID
 * @param[in] cis_id          CIS ID
 * @param[in] con_lid         Connection local index
 * @param[in] intf_lid        Interface local index for the group
 * @param[out] p_group_lid    Pointer at which group local index will be returned
 * @param[out] p_stream_lid   Pointer at which stream local index will be returned
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_us_prepare(uint8_t cig_id, uint8_t cis_id, uint8_t con_lid, uint8_t intf_lid,
                        uint8_t *p_group_lid, uint8_t* p_stream_lid);

/**
 ****************************************************************************************
 * @brief Bind an unicast stream with a connection
 *
 * @param[in] stream_lid     Stream local index
 * @param[in] con_lid        Connection local index
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_us_bind(uint8_t stream_lid, uint8_t con_lid);

/**
 ****************************************************************************************
 * @brief Update content of an unicast group
 *
 * @param[in] group_lid     Group local index
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_ug_update(uint8_t group_lid);

/**
 ****************************************************************************************
 * @brief Enable bounded stream in a unicast group
 *
 * @param[in] group_lid     Group local index
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_us_enable(uint8_t group_lid);

/**
 ****************************************************************************************
 * @brief Disable an unicast stream
 *
 * @param[in] stream_lid      Stream local index
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_us_disable(uint8_t stream_lid);

/**
 ****************************************************************************************
 * @brief Release an unicast stream
 *
 * @param[in] stream_lid      Stream local index
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_us_release(uint8_t stream_lid);

/**
 ****************************************************************************************
 * @brief Remove a unicast group
 *
 * @param[in] group_lid       Group local index
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_ug_remove(uint8_t group_lid);

/**
 ****************************************************************************************
 * @brief Get unicast group parameters (normal)
 *
 * @param[in] group_lid       Group local index
 *
 * @return Pointer to group parameter structure
 ****************************************************************************************
 */
iap_ug_param_t* iap_ug_get_param(uint8_t group_lid);

/**
 ****************************************************************************************
 * @brief Get unicast group parameters (test)
 *
 * @param[in] group_lid       Group local index
 *
 * @return Pointer to group parameter structure
 ****************************************************************************************
 */
iap_ug_test_param_t* iap_ug_get_test_param(uint8_t group_lid);

/**
 ****************************************************************************************
 * @brief Get unicast group configuration
 *
 * @param[in] group_lid       Group local index
 *
 * @return Pointer to group configuration structure
 ****************************************************************************************
 */
iap_ug_config_t* iap_ug_get_config(uint8_t group_lid);

/**
 ****************************************************************************************
 * @brief Get unicast stream parameter (normal)
 *
 * @param[in] stream_lid       Stream local index
 *
 * @return Pointer to stream parameter structure
 ****************************************************************************************
 */
iap_us_param_t* iap_us_get_param(uint8_t stream_lid);

/**
 ****************************************************************************************
 * @brief Get unicast stream parameter (test)
 *
 * @param[in] stream_lid       Stream local index
 *
 * @return Pointer to stream parameter structure
 ****************************************************************************************
 */
iap_us_test_param_t* iap_us_get_test_param(uint8_t stream_lid);

/**
 ****************************************************************************************
 * @brief Get unicast stream configuration
 *
 * @param[in] stream_lid       Stream local index
 *
 * @return Pointer to stream configuration structure
 ****************************************************************************************
 */
iap_us_config_t* iap_us_get_config(uint8_t stream_lid);
#endif //(GAF_UNICAST_SUPP)

#if (GAF_BROADCAST_SUPP)
#if (GAF_BROADCAST_MST_SUPP)
/**
 ****************************************************************************************
 * @brief Add a broadcast group
 *
 * @param[in] p_params          Pointer to Broadcast Group information structure
 * @param[in] big_handle        BIG handle
 * @param[in] nb_streams        Number of streams in the group
 * @param[in] intf_lid          Interface local index for the group
 * @param[out] p_group_lid      Pointer at which allocated group local index will be returned
 * @param[out] p_stream_lids    Pointer at which allocated stream local indexes will be returned
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_bg_add(iap_bg_param_t* p_params, uint8_t big_handle, uint8_t nb_streams,
                    uint8_t intf_lid, uint8_t* p_group_lid, uint8_t* p_stream_lids);

/**
 ****************************************************************************************
 * @brief Add a broadcast group for test
 *
 * @param[in] p_params          Pointer to Broadcast Group information structure
 * @param[in] big_handle        BIG handle
 * @param[in] nb_streams        Number of streams in the group
 * @param[in] intf_lid          Interface local index for the group
 * @param[out] p_group_lid      Pointer at which allocated group local index will be returned
 * @param[out] p_stream_lids    Pointer at which allocated stream local indexes will be returned
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_bg_test_add(iap_bg_test_param_t* p_params, uint8_t big_handle, uint8_t nb_streams,
                         uint8_t intf_lid, uint8_t* p_group_lid, uint8_t* p_stream_lids);

/**
 ****************************************************************************************
 * @brief Enable a broadcast group
 *
 * @param[in] group_lid         Group local index
 * @param[in] adv_actv_lid      Advertising activity local index
 * @param[in] p_broadcast_code  Pointer to 16-byte code used to generate encryption key used to encrypt payloads.
 *                              NULL if stream in the group are not encrypted
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_bg_enable(uint8_t group_lid, uint8_t adv_actv_lid, uint8_t* p_broadcast_code);
#endif //(GAF_BROADCAST_MST_SUPP)

#if (GAF_BROADCAST_SLV_SUPP)
/**
 ****************************************************************************************
 * @brief Request to synchronize with a broadcast group
 *
 * @param[in] p_broadcast_code  Group local index
 * @param[in] p_streams         List of stream indices/positions in ascending order
 * @param[in] sync_timeout      Synchronization timeout
 * @param[in] handle            BIG handle
 * @param[in] sync_actv_lid     Synchronization activity local index
 * @param[in] mse               Maximum number of subevents the controller should use to received
 *                              data payloads in each interval
 * @param[in] intf_lid          Interface local index for the group
 * @param[out] p_group_lid      Pointer at which allocated group local index will be returned
 * @param[out] p_stream_lids    Pointer at which allocated stream local indexes will be returned
 *                              size of array shall be greater or equals number of bit present in stream_pos_bf
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_bg_sync(const uint8_t* p_broadcast_code, uint32_t stream_pos_bf, uint16_t sync_timeout,
                     uint8_t handle, uint8_t sync_actv_lid, uint8_t mse, uint8_t intf_lid,
                     uint8_t* p_group_lid, uint8_t* p_stream_lids);
#endif //(GAF_BROADCAST_SLV_SUPP)

/**
 ****************************************************************************************
 * @brief Disable a broadcast group
 *
 * @param[in] group_lid         Group local index
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_bg_disable(uint8_t group_lid);

/**
 ****************************************************************************************
 * @brief Disable a broadcast group
 *
 * @param[in] group_lid         Group local index
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_bg_remove(uint8_t group_lid);

/**
 ****************************************************************************************
 * @brief Get broadcast group information (normal parameters)
 *
 * @param[in] group_lid       Group local index
 *
 * @return Pointer to group information structure
 ****************************************************************************************
 */
iap_bg_param_t* iap_bg_param_get(uint8_t group_lid);

/**
 ****************************************************************************************
 * @brief Get broadcast group information (test parameters)
 *
 * @param[in] group_lid       Group local index
 *
 * @return Pointer to group information structure
 ****************************************************************************************
 */
iap_bg_test_param_t* iap_bg_test_param_get(uint8_t group_lid);

/**
 ****************************************************************************************
 * @brief Get broadcast group configuration
 *
 * @param[in] group_lid       Group local index
 *
 * @return Pointer to group configuration structure
 ****************************************************************************************
 */
iap_bg_config_t* iap_bg_config_get(uint8_t group_lid);
#endif //(GAF_BROADCAST_SUPP)

/**
 ****************************************************************************************
 * @brief Configure or reconfigure stream data path for a given direction
 *
 * @param[in] stream_lid        Stream local index
 * @param[in] direction         Direction for which datapath is configured
 * @param[in] p_dp_config       Pointer to Data Path configuration structure
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_dp_config(uint8_t stream_lid, uint8_t direction, iap_dp_config_t* p_dp_config);

/**
 ****************************************************************************************
 * @brief Setup data path on a stream
 *
 * @param[in] stream_lid    Stream local index
 * @param[in] direction     Direction for which data path must be setup
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_dp_setup(uint8_t stream_lid, uint8_t direction);

/**
 ****************************************************************************************
 * @brief Remove data path on a stream
 *
 * @param[in] stream_lid    Stream local index
 * @param[in] direction_bf  Bit field indicated directions for which data path must be removed
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_dp_remove(uint8_t stream_lid, uint8_t direction_bf);

/**
 ****************************************************************************************
 * @brief Get data path configuration for a stream
 *
 * @param[in] stream_lid    Stream local index
 * @param[in] direction     Direction for which data path configuration must be retrieved
 ****************************************************************************************
 */
iap_dp_config_t* iap_dp_config_get(uint8_t stream_lid, uint8_t direction);

#if (BLE_EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Vendor specific command used to set a periodic trigger
 *
 * @param[in] stream_lid            Stream local index
 * @param[in] direction             TX = 0, RX = 1
 * @param[in] enable                0 Disable, 1 Enable
 * @param[in] trigger_offset_us     Trigger offset in microseconds
 * For TX direction, trigger offset is applied using SDU at source reference
 * For RX direction, trigger offset is applied using SDU at destination reference
 * (SDU at source + transport latency)
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_dp_vs_set_trigger(uint8_t stream_lid, uint8_t direction, uint8_t enable,
                               int32_t trigger_offset_us);
#endif //(BLE_EMB_PRESENT)

/**
 ****************************************************************************************
 * @brief Start test mode on a stream
 *
 * @param[in] stream_lid    Stream local index
 * @param[in] transmit      Transmit (!=0) or receive (=0) test payloads
 * @param[in] payload_type  Payload type
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_tm_start(uint8_t stream_lid, uint8_t transmit, uint8_t payload_type);

/**
 ****************************************************************************************
 * @brief Get test mode counter for a stream
 *
 * @param[in] stream_lid    Stream local index
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_tm_cnt_get(uint8_t stream_lid);

/**
 ****************************************************************************************
 * @brief Stop test mode on a stream
 *
 * @param[in] stream_lid    Stream local index
 *
 * @return An handling status
 ****************************************************************************************
 */
uint16_t iap_tm_stop(uint8_t stream_lid);

/**
 ****************************************************************************************
 * @brief Get connection handle for a given stream
 *
 * @param[in] stream_lid    Stream local index
 *
 * @return Connection handle value
 ****************************************************************************************
 */
uint16_t iap_sm_stream_lid_to_conhdl(uint8_t stream_lid);

/**
 ****************************************************************************************
 * @brief Get stream associated with a given connection handle
 *
 * @param[in]  conhdl        Connection handle
 * @param[out] p_stream_lid  Pointer to Stream local index
 * @param[out] p_group_lid   Pointer to Group local index
 *
 * @return Function execution status
 ****************************************************************************************
 */
uint16_t iap_sm_stream_conhdl_to_lid(uint16_t conhdl, uint8_t* p_stream_lid, uint8_t* p_group_lid);

/**
 ****************************************************************************************
 * @brief Get connection handle for a given stream
 *
 * @param[in] stream_lid    Stream local index
 *
 * @return Connection handle value
 ****************************************************************************************
 */
uint16_t iap_sm_stream_conhdl_get_with_id(uint8_t group_id, uint8_t stream_id);

/// @}

#endif // IAP_H_
