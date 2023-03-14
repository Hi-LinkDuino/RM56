/**
 ****************************************************************************************
 *
 * @file gapm_int.h
 *
 * @brief Generic Access Profile Manager Internal Header.
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 ****************************************************************************************
 */


#ifndef _GAPM_INT_H_
#define _GAPM_INT_H_

/**
 ****************************************************************************************
 * @addtogroup GAPM_INT Generic Access Profile Manager Internal
 * @ingroup GAPM
 * @brief defines for internal GAPM usage
 *
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "gapm.h"
#include "gapm_msg.h"
#include "co_bt.h"
#include "co_list.h"
#include "ke_task.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of GAP Manager process
#define GAPM_IDX_MAX                                 0x01

/// Scan filter size
#define GAPM_SCAN_FILTER_SIZE   10

/// check if current role is supported by configuration
#define GAPM_IS_ROLE_SUPPORTED(role_type)\
    ((gapm_env.role & (role_type)) == (role_type))

/// Number of supported activities in host
#define GAPM_ACTV_NB            (BLE_ACTIVITY_MAX)
/// Invalid activity identifier
#define GAPM_ACTV_INVALID_IDX   (0xFF)

/// Maximum number of advertising reports from different advertisers that can be reassembled in parallel
#define GAPM_REPORT_NB_MAX            (5)


/*
 * INTERNAL API TYPES
 ****************************************************************************************
 */


/// Retrieve information about memory usage
struct gapm_dbg_get_mem_info_cmd
{
    /// GAPM requested operation:
    ///  - GAPM_DBG_GET_MEM_INFO: Get memory usage
    uint8_t operation;
};

/// Indication containing information about memory usage.
/*@TRACE*/
struct gapm_dbg_mem_info_ind
{
    /// peak of memory usage measured
    uint32_t max_mem_used;
    /// memory size currently used into each heaps.
    uint16_t mem_used[KE_MEM_BLOCK_MAX];
};



/// Operation type
enum gapm_op_type
{
    /// Configuration operation
    GAPM_OP_CFG         = 0x00,

    /// Air mode operation (scanning, advertising, connection establishment)
    /// Note: Restriction, only one air operation supported.
    GAPM_OP_AIR         = 0x01,
    /// GAP State for DH Key Generation
    GAPM_OP_DHKEY       = 0x02,
    /// Max number of operations
    GAPM_OP_MAX
};

/// GAPM states
enum gapm_state_id
{
    /// Idle state - no on going operation
    GAPM_IDLE,

    /// Busy state - Configuration operation on going
    GAPM_CFG_BUSY       = 1 << GAPM_OP_CFG,
    /// Busy state - Air operation on going
    GAPM_AIR_BUSY       = 1 << GAPM_OP_AIR,
    /// Busy state - DH Key Calculation operation on going
    GAPM_DHKEY_BUSY     = 1 << GAPM_OP_DHKEY,

    /// Reset state - Reset operation on going
    GAPM_DEVICE_SETUP   = 1 << GAPM_OP_MAX,
};

/// Activity states
enum gapm_actv_state
{
    /// Activity is being created - next state is CREATED
    GAPM_ACTV_CREATING = 0,
    /// Activity has been successfully created
    GAPM_ACTV_CREATED,
    /// Activity is being started - next state is STARTED
    GAPM_ACTV_STARTING,
    /// Activity has been successfully started
    GAPM_ACTV_STARTED,
    /// Activity is being stopped - next state is CREATED
    GAPM_ACTV_STOPPING,
    /// Activity is being deleted - no next state
    GAPM_ACTV_DELETING,
};

/// Connection role
enum gapm_conn_role
{
    /// Master
    GAPM_ROLE_MASTER = 0,
    /// Slave
    GAPM_ROLE_SLAVE,
};

/*
 * MACROS
 ****************************************************************************************
 */



/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Contain a received fragment of advertising or scan response report
struct gapm_report_elem
{
    /// List Header
    struct co_list_hdr list_hdr;
    /// Data length
    uint8_t data_len;
    /// Data
    uint8_t data[];
};

/// Contain a list of fragments received for an advertising or scan response report sent
/// by a given advertiser
struct gapm_report_list
{
    /// List of received reports (@see struct gapm_report_elem)
    struct co_list report_list;
    /// Advertiser address
    struct gap_bdaddr adv_addr;
    /// Received length
    uint16_t length;
};

/// GAP Manager activity structure (common part for advertising, scanning,
/// initiating and periodic synchronization activities)
typedef struct gapm_actv
{
    /// Function to be called for activity start
    uint8_t     (*cb_actv_start)(struct gapm_actv* p_act, struct gapm_activity_start_cmd* p_cmd);
    /// Function to be called for activity stop
    void        (*cb_actv_stop)(struct gapm_actv* p_act);
    /// Function to be called for activity delete
    void        (*cb_actv_delete)(struct gapm_actv*  p_act);

    /// Identifier
    uint8_t      idx;
    /// Type (@see enum gapm_actv_type)
    uint8_t      type;
    /// Subtype - meaning depends on activity type
    ///  - Advertising activity: @see enum gapm_adv_type
    ///  - Scanning activity: @see enum gapm_scan_type
    ///  - Initiating activity: @see enum gapm_init_type
    ///  - Periodic Synchronization activity: @see enum gapm_per_sync_type
    uint8_t      subtype;
    /// State (@see enum gapm_actv_state)
    uint8_t      state;
    /// Information bit field, meaning depends on activity type
    uint8_t      info;
    /// Own address type
    uint8_t      own_addr_type;
    /// Next expected HCI event opcode
    uint16_t     next_exp_opcode;
    /// Task ID of task that has requested creation of the activity
    ke_task_id_t requester;
    /// BD Address used by the activity (can be different if controller privacy is used and
    /// application chose to use a resolvable private address)
    gap_addr_t   addr;
    
    uint16_t     randomAddrRenewIntervalInSecond;
} gapm_actv_t;

/// GAP Manager activity structure for advertising activity
struct gapm_actv_adv_tag
{
    /// Common activity parameters
    gapm_actv_t common;
    /// Data offset for the set advertising data procedure
    uint16_t data_offset;
    /// Advertising mode (@see enum gapm_adv_disc_mode)
    uint8_t mode;
    /// The flag Indicates whether the ADV contains AD Type flags
    uint8_t with_flags;
    /// Stored status
    uint8_t kept_status;
    /// Selected TX power
    uint8_t tx_pwr;
    /// Additional information (@see enum gapm_adv_add_info_bf)
    uint8_t add_info;
};

/// GAP Manager activity structure for scanning activity
struct gapm_actv_scan_tag
{
    /// Common activity parameters
    gapm_actv_t common;
    /// Lists containing fragments for GAPM_REPORT_NB_MAX reports that can be received in parallel
    struct gapm_report_list report_lists[GAPM_REPORT_NB_MAX];
    /// Scan filtering Array
    struct gap_bdaddr *p_scan_filter;
};

/// GAP Manager activity structure for initiating activity
struct gapm_actv_init_tag
{
    /// Common activity parameters
    gapm_actv_t common;
    /// Initiating parameters
    struct gapm_init_param init_param;
    /// Number of connection to be established for automatic connection
    ///    -> Number of devices in the white list when GAPM_ACTIVITY_START_CMD is received
    uint8_t nb_auto_conn;
    /// Stored status
    uint8_t kept_status;
};

/// GAP Manager activity structure for periodic synchronization activity
struct gapm_actv_per_sync_tag
{
    /// Common activity parameters
    gapm_actv_t common;
    /// List of received reports fragment (@see struct gapm_report_elem)
    struct co_list       report_list;
    /// Received length
    uint16_t             length;
    /// Synchronization Handle
    uint16_t             sync_hdl;
    #if (BLE_CENTRAL || BLE_PERIPHERAL)
    /// Connection index where PAST sync info is expected
    uint8_t             conidx;
    #endif // (BLE_CENTRAL || BLE_PERIPHERAL)

};


/// GAP Manager environment structure
typedef struct gapm_env_
{
    /// Request operation Kernel message
    void*           operation[GAPM_OP_MAX];

    /// Array of pointers to the allocated activities
    gapm_actv_t*    actvs[GAPM_ACTV_NB];

    #if (BLE_ISO_MODE_0)
    const gapm_am0_cb_t*  p_am0_cb;
    #endif // (BLE_ISO_MODE_0)

    #if (BLE_CENTRAL || BLE_PERIPHERAL)
    /// Bit field use to know if periodic ADV sync transfer establishment on-going onto a specific connection index
    uint32_t        past_estab_bf;
    #endif // (BLE_CENTRAL || BLE_PERIPHERAL)

    /// Task that receive test mode events.
    uint16_t        test_report_task;
    /// token number counter to ensure a request and confirmation have an unique identifier.
    uint16_t        token_id_cnt;

    #if (BLE_BROADCASTER)
    /// Maximum length of advertising data that can be pushed to the controller
    uint16_t        max_adv_data_len;
    #endif //(BLE_BROADCASTER)

    /// Duration before regenerate device address when privacy is enabled. - in seconds
    uint16_t        renew_dur;
    /// Device IRK used for resolvable random BD address generation (MSB -> LSB)
    gap_sec_key_t   irk;

    /// Current device Address
    gap_addr_t      addr;
    gap_addr_t      connectedAddr[BLE_CONNECTION_MAX];
    /// Privacy Configuration (@see gapm_priv_cfg)
    uint8_t         priv_cfg;
    /// Device Role
    uint8_t         role;
    /// Number of BLE connection
    uint8_t         connections;
    /// Pairing mode authorized (@see enum gapm_pairing_mode)
    uint8_t         pairing_mode;

    #if (BLE_BROADCASTER)
    /// Number of advertising sets supported by controller
    uint8_t         max_adv_set;
    /// Number of created advertising activities
    uint8_t         nb_adv_actv;
    #endif //(BLE_BROADCASTER)
    #if (BLE_OBSERVER)
    /// Activity identifier for currently started scanning activity
    uint8_t         scan_actv_idx;
    #endif //(BLE_OBSERVER)
    #if (BLE_CENTRAL)
    /// Activity identifier for currently started initiating activity
    uint8_t         init_actv_idx;
    #endif //(BLE_CENTRAL)
    /// Activity identifier currently used for air operation
    uint8_t         actv_idx;

    #if (BLE_OBSERVER)
    /// Activity in Periodic sync establishment state
    uint8_t         per_sync_estab_actv_idx;
    /// Used to know if transmission of advertising reports to application must be stopped
    /// When enabled, advertising reports are dropped without informing host.
    bool            adv_report_flow_off;
    #endif //(BLE_OBSERVER)

    /// Number of created activities
    uint8_t         created_actvs;
    /// Number of started activities
    uint8_t         started_actvs;
    /// Number of started activities that can lead to connection establishment (connectable
    /// advertising or initiating)
    uint8_t         connect_actvs;
    /// Number of devices in the white list
    uint8_t         nb_dev_wl;
    /// State of GAPM ADDR module
    uint8_t         gapm_addr_state;
} gapm_env_t;

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */
/// GAP Manager environment variable.
extern gapm_env_t gapm_env;


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Send operation completed message according to operation type.
 * Perform also am operation clean-up
 *
 * @param[in] op_type Operation type
 * @param[in] status  Command status
 *****************************************************************************************
 */
void gapm_send_complete_evt(uint8_t op_type, uint8_t status);

/**
 ****************************************************************************************
 * @brief Send operation completed message with status error code not related to a
 * running operation.
 *
 * @param[in] operation Operation code
 * @param[in] requester requester of operation
 * @param[in] status    Error status code
 ****************************************************************************************
 */
void gapm_send_error_evt(uint8_t operation, const ke_task_id_t requester, uint8_t status);


/**
 ****************************************************************************************
 * @brief Get operation pointer
 *
 * @param[in] op_type       Operation type.
 *
 * @return operation pointer on going
 ****************************************************************************************
 */
static __INLINE void* gapm_get_operation_ptr(uint8_t op_type)
{
    ASSERT_ERR(op_type < GAPM_OP_MAX);
    // return operation pointer
    return gapm_env.operation[op_type];
}


/**
 ****************************************************************************************
 * @brief Set operation pointer
 *
 * @param[in] op_type       Operation type.
 * @param[in] op            Operation pointer.
 *
 ****************************************************************************************
 */
static __INLINE void gapm_set_operation_ptr(uint8_t op_type, void* op)
{
    ASSERT_ERR(op_type < GAPM_OP_MAX);
    // update operation pointer
    gapm_env.operation[op_type] = op;
}


/**
 ****************************************************************************************
 * @brief Check if current operation can be processed or not.
 * if it can be proceed, initialize an operation request.
 * If a command complete event with error code can be triggered.
 *
 * Function returns how the message should be handled by message handler.
 *
 * @param[in] op_type       Operation type.
 * @param[in] op_msg        Requested operation message (note op_msg cannot be null)
 * @param[in] supp_ops      Supported operations array.
 *                          Latest array value shall be GAPM_NO_OP.
 *
 * @return operation can be executed if message status equals KE_MSG_NO_FREE,
 * else nothing to do, just exit from the handler.
 ****************************************************************************************
 */
int gapm_process_op(uint8_t op_type, void* op_msg, enum gapm_operation* supp_ops);

/**
 ****************************************************************************************
 * @brief Get operation on going
 *
 * @param[in] op_type       Operation type.
 *
 * @return operation code on going
 ****************************************************************************************
 */
uint8_t gapm_get_operation(uint8_t op_type);

/**
 ****************************************************************************************
 * @brief Get requester of on going operation
 *
 * @param[in] op_type       Operation type.
 *
 * @return task that requests to execute the operation
 ****************************************************************************************
 */
ke_task_id_t gapm_get_requester(uint8_t op_type);




#if (BLE_GAPC)
/**
 ****************************************************************************************
 * @brief A connection has been created, initialize host stack to be ready for connection.
 *
 * @param[in] msgid      Message id
 * @param[in] con_params Connection parameters from lower layers
 *
 * @return Connection index allocated to the new connection.
 ****************************************************************************************
 */
uint8_t gapm_con_create(ke_msg_id_t const msgid, struct hci_le_enh_con_cmp_evt const *con_params);
#endif // (BLE_GAPC)

/**
 ****************************************************************************************
 * @brief Checks validity of the address type
 *
 * @param[in] addr_type   Address type
 ****************************************************************************************
 */
uint8_t gapm_addr_check( uint8_t addr_type);

/**
 ****************************************************************************************
 * @brief Checks validity of the Data Length Suggested values
 *
 * @param[in] sugg_oct   Suggested octets
 * @param[in] sugg_time  Suggested time
 ****************************************************************************************
 */
uint8_t gapm_dle_val_check(uint16_t sugg_oct, uint16_t sugg_time);



/**
 ****************************************************************************************
 * @brief Update task state
 *
 * @param[in] operation that modify the state
 * @param[in] set state to busy (true) or idle (false)
 *
 ****************************************************************************************
 */
void gapm_update_state(uint8_t operation, bool busy);

/**
 ****************************************************************************************
 * @brief  Reset the environment by cleaning all existing activities
 *
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 ****************************************************************************************
 */
void gapm_actv_reset(uint8_t init_type);

/**
 ****************************************************************************************
 * @brief Allocate an activity structure for a given identifier. Provided identifer is
 * then no more marked as available.
 * It is considered here that provided activity identifier is an available one.
 *
 * @param[in] actv_idx      Activity index
 * @param[in] size          Size of the activity structure to be allocated
 *
 * @return Pointer to the allocated activity structure
 ****************************************************************************************
 */
gapm_actv_t *gapm_actv_alloc(uint8_t actv_idx, uint8_t size);

/**
 ****************************************************************************************
 * @brief Retrieve activity for which an HCI command complete event has been received.
 * Note that at any time only one HCI command complete event can be expected by GAPM.
 * Hence only one activity can be found.
 * This function also check that the operation code of the complete command well
 * match with the command that has been sent for the foun activity.
 *
 * @param[in|out] pp_actv    Pointer to the variable that will contain address of
 *                           activity structure for which command complete event
 *                           has been received.
 * @param[in] opcode         Command operation code for which a command complete event has
 *                           been received
 *
 * @return true if activity has been retrieved, else false
 ****************************************************************************************
 */
bool gapm_actv_retrieve_cmd_cmp_evt(gapm_actv_t **pp_actv, uint16_t opcode);

/**
 ****************************************************************************************
 * @brief Get own address type to be sent in HCI commands.
 *
 * @param[in] app_addr_type     Address type provided by application using GAPM_SET_DEV_CONFIG_CMD.
 *
 * @return Own address type value
 *      - 0: Public Device Address
 *      - 1: Random Device Address
 *      - 2: Controller generates the RPA based on the local IRK from the resolving list.
 * If the resolving list contains no matching entry, use the public address
 *      - 3: Controller generates the RPA based on the local IRK from the resolving list.
 * If the resolving list contains no matching entry, use the random address that has
 * been set using HCI LE Set Advertising Set Random Address.
 ****************************************************************************************
 */
uint8_t gapm_actv_get_hci_own_addr_type(uint8_t app_addr_type);

/**
 ****************************************************************************************
 * @brief Notify the activity module that activity creation procedure is over.
 *
 * @param[in] p_actv    Pointer to the structure describing the created activity
 * @param[in] status    Indicate if an error has been raised during activity creation
 ****************************************************************************************
 */
void gapm_actv_created(gapm_actv_t *p_actv, uint8_t status);

/**
 ****************************************************************************************
 * @brief Notify the activity module that a start activity procedure is over
 *
 * @param[in] p_actv    Pointer to the structure describing the started activity
 * @param[in] status     Indicate if an error has been raised during activity start procedure
 ****************************************************************************************
 */
void gapm_actv_started(gapm_actv_t *p_actv, uint8_t status);

/**
 ****************************************************************************************
 * @brief Notify the activity module that an activity has well been stopped.
 *
 * @param[in] p_actv    Pointer to the structure describing the stopped activity
 * @param[in] status    Activity stop operation status
 ****************************************************************************************
 */
void gapm_actv_stopped(gapm_actv_t *p_actv, uint8_t status);

/**
 ****************************************************************************************
 * @brief Function used once an activity has been cleaned and can be deleted
 *
 * @param[in] p_actv    Pointer to the structure describing the stopped activity
 * @param[in] error     Indicate if activity has well been stopped
 ****************************************************************************************
 */
void gapm_actv_deleted(gapm_actv_t *p_actv);

/**
 ****************************************************************************************
 * @brief Callback function used by GAPM ADDR module in order to inform the GAPM ACTV
 * module that a random address (generated or not) has been set in the controller
 * upon its request.
 *
 * @param[in] p_actv    Pointer to the activity structure for which random address has
 *                      been set.
 ****************************************************************************************
 */
void gapm_actv_rand_addr_set_ind(gapm_actv_t *p_actv);

#if (BLE_BROADCASTER)
/**
 ****************************************************************************************
 * @brief Allocate an activity structure to be used for an advertising activity.
 *
 * @param[in] actv_idx      Allocated activity identifier
 * @param[in] p_param       GAPM_ACTIVITY_CREATE_CMD message parameters
 ****************************************************************************************
 */
uint8_t gapm_adv_create(uint8_t actv_idx, struct gapm_activity_create_adv_cmd *p_param);
#endif //(BLE_BROADCASTER)

#if (BLE_OBSERVER)
/**
 ****************************************************************************************
 * @brief Clean content of a scan activity, more specifically the stored fragments
 * of advertising report and the filtering list.
 *
 * @param[in] p_actv    Pointer to the scanning activity structure.
 ****************************************************************************************
 */
void gapm_scan_actv_clean(gapm_actv_t *p_actv);

/**
 ****************************************************************************************
 * @brief Allocate an activity structure to be used for a scanning activity.
 *
 * @param[in] actv_idx      Allocated activity identifier
 * @param[in] p_param       GAPM_ACTIVITY_CREATE_CMD message parameters
 *
 * @return GAP_ERR_NO_ERROR if creation of activity can be started,
 *         GAP_ERR_COMMAND_DISALLOWED if activity cannot be started
 *         GAP_ERR_INSUFF_RESOURCES if not enough memory is found for activity structure
 *         allocation.
 ****************************************************************************************
 */
uint8_t gapm_scan_create(uint8_t actv_idx, struct gapm_activity_create_cmd *p_param);
#endif //(BLE_OBSERVER)

#if (BLE_CENTRAL)
/**
 ****************************************************************************************
 * @brief Allocate an activity structure to be used for an initiating activity.
 *
 * @param[in] actv_idx      Allocated activity identifier
 * @param[in] p_param       GAPM_ACTIVITY_CREATE_CMD message parameters
 ****************************************************************************************
 */
uint8_t gapm_init_create(uint8_t actv_idx, struct gapm_activity_create_cmd *p_param);

/**
 ****************************************************************************************
 * @brief Function used by GAPM_ACTV module to inform GAPM_INIT that a new connection
 * has been established.
 * If current initiating type is a name discovery, it triggers sending of a GAPC_GET_INFO_CMD
 * in order to retrieve the peer device name.
 * If current initiating type is an automatic connection establishment, a new HCI LE
 * Extended Create Connection command will be sent to the controller if not all device in
 * the white list have been connected.
 * If no more connection is expected, the activity is stopped.
 *
 * @param[in] conidx    Connection index allocated for the newly established connection
 *
 * @return True if the activity has to be stopped, else false.
 ****************************************************************************************
 */
bool gapm_init_connection_ind(uint8_t conidx);
#endif //(BLE_CENTRAL)

#if (BLE_OBSERVER)
 /**
  ****************************************************************************************
  * @brief Allocate an activity structure to be used for a periodic synchronization activity.
  *
  * @param[in] actv_idx      Allocated activity identifier
  * @param[in] p_param       GAPM_ACTIVITY_CREATE_CMD message parameters
  *
  * @return GAP_ERR_NO_ERROR if activity creation has ended properly, GAP_ERR_INSUFF_RESOURCES
  * if no available memory has been found for activity allocation.
  ****************************************************************************************
  */
uint8_t gapm_per_sync_create(uint8_t actv_idx, struct gapm_activity_create_cmd *p_param);

/**
 ****************************************************************************************
 * @brief Request to clear all report fragments stored in the periodic synchronization
 * activity
 *
 * @param[in] p_actv_per_sync      Pointer to the activity structure
 ****************************************************************************************
 */
void gapm_per_sync_clear_fragments(struct gapm_actv_per_sync_tag *p_actv_per_sync);
#endif //(BLE_OBSERVER)

/**
 ****************************************************************************************
 * @brief Initialize GAPM address manager module
 ****************************************************************************************
 */
void gapm_addr_init(void);

/**
 ****************************************************************************************
 * @brief Function to be used to check if address type provided for a scanning or an
 * initiating activity is valid or not.
 * As a same address shall be shared between all these activities, all created scanning and
 * initiating activities shall have the same address type.
 *
 * @param[in] addr_type     Address type to be checked
 *
 * @return true if provided address type is valid, else false
 ****************************************************************************************
 */
bool gapm_is_addr_type_valid(uint8_t addr_type);

/**
 ****************************************************************************************
 * @brief Request to clear all report fragments stored in the periodic synchronization
 * activity
 *
 * @param[in] p_actv      Pointer to the activity structure for which a random has to be
 *                        generated and/or set in the controller.
 ****************************************************************************************
 */
void gapm_addr_set_rand_addr(gapm_actv_t *p_actv);

/**
 ****************************************************************************************
 * @brief Sends a basic HCI command (with no parameter)
 *
 * @param[in] opcode       Command opcode
 ****************************************************************************************
 */
void hci_basic_cmd_send_2_controller(uint16_t opcode);

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

/// @} GAPM_INT

#endif /* _GAPM_INT_H_ */
