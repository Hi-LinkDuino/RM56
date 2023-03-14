#ifndef __BLE_ADAPTER_DEF_H__
#define __BLE_ADAPTER_DEF_H__

#define ADAPTER_BLE_UUID_16_LEN (0x02)
#define ADAPTER_BLE_UUID_32_LEN (0x04)
#define ADAPTER_BLE_UUID_128_LEN (0x10)

#define ADAPTER_BLE_PROP_BROADCAST (0x01)
#define ADAPTER_BLE_PROP_READ (0x02)
#define ADAPTER_BLE_PROP_WRITE_NO_RSP (0x04)
#define ADAPTER_BLE_PROP_WRITE (0x08)
#define ADAPTER_BLE_PROP_NOTIFY (0x10)
#define ADAPTER_BLE_PROP_INDICATE (0x20)
#define ADAPTER_BLE_PROP_WRITE_SIGNED  (0x40)

#define ADAPTER_BLE_PERM_READ (0x01)
#define ADAPTER_BLE_PERM_READ_ENCRYPTED (0x02)
#define ADAPTER_BLE_PERM_READ_ENCRYPTED_MITM (0x04)
#define ADAPTER_BLE_PERM_WRITE (0x10)
#define ADAPTER_BLE_PERM_WRITE_ENCRYPTED (0x20)
#define ADAPTER_BLE_PERM_WRITE_ENCRYPTED_MITM (0x40)
#define ADAPTER_BLE_MAX_ATT_SIZE (32)
#define ADAPTER_BLE_SVC_MAX_ELEMENT (48)

#define ADAPTER_BLE_OPT_NO_OFFSET (0x8000)

#define ADAPTER_BLE_PRF_CHAR_PRES_FMT_SIZE  (7)


#define BES_GATT_MAX_UUID_LEN (16)
#define BES_GATT_MAX_ATTR_LEN (128)
#define MAX_BDADDR_LEN  6
#define MAX_SERVICE_ELEM_NUM (16)

#define BES_CPU_LE          1

/// Macro used to convert CPU integer define to LSB first 16-bits UUID
#if (!BES_CPU_LE)
#define BES_GATT_UUID_16_LSB(uuid)          (((uuid & 0xFF00) >> 8) | ((uuid & 0x00FF) << 8))
#define BES_UUID_16_TO_ARRAY(uuid)          {(uuid & 0xFF00) >> 8, (uuid & 0x00FF)}
#else
#define BES_GATT_UUID_16_LSB(uuid)          (uuid)
#define BES_SUUID_16_TO_ARRAY(uuid)          {(uuid & 0x00FF), (uuid & 0xFF00) >> 8}
#endif // CPU_LE

#if 0 // redefine ,first in gatt.h
#define GATT_DECL_PRIMARY_SERVICE       { 0x00, 0x28 }
#define GATT_DECL_INCLUDED_SERVICE       { 0x02, 0x28 }
#define GATT_DECL_CHARACTERISTIC_UUID        { 0x03, 0x28 }
#define GATT_DESC_CLIENT_CHAR_CFG_UUID       { 0x02, 0x29 }
#define GATT_DESC_CHAR_USER_DESCRIPTION_UUID { 0x01, 0x29 }
#endif

typedef enum
{
    /// No error
    BES_BLE_ERR_NO_ERROR = 0,
    /// Invalid parameters
    BES_BLE_ERR_INVALID_PARAM,
    /// Command disallowed
    BES_BLE_ERR_COMMAND_DISALLOWED,
    /// Unknown command
    BES_BLE_ERR_UNKNOWN_COMMAND,
    /// Unknown request
    BES_BLE_ERR_UNKNOWN_REQUEST,
    /// Insufficient resources
    BES_BLE_ERR_INSUFFICIENT_RESOURCES,
    /// Invalid connection
    BES_BLE_ERR_INVALID_CONNECTION,
    /// Busy
    BES_BLE_ERR_BUSY,
    /// Failed
    BES_BLE_ERR_FAILED,
    /// Disconnection has occurred
    BES_BLE_ERR_DISCONNECTED ,
}bes_gatt_status_e;

typedef enum
{
    // ----------------------------------------------------------------------------------
    // -------------------------  ATT Specific Error ------------------------------------
    // ----------------------------------------------------------------------------------
    /// No error
    BES_ATT_ERR_NO_ERROR                                                               = 0x00,
    /// 0x01: Handle is invalid
    BES_ATT_ERR_INVALID_HANDLE                                                         = 0x01,
    /// 0x02: Read permission disabled
    BES_ATT_ERR_READ_NOT_PERMITTED                                                     = 0x02,
    /// 0x03: Write permission disabled
    BES_ATT_ERR_WRITE_NOT_PERMITTED                                                    = 0x03,
    /// 0x04: Incorrect PDU
    BES_ATT_ERR_INVALID_PDU                                                            = 0x04,
    /// 0x05: Authentication privilege not enough
    BES_ATT_ERR_INSUFF_AUTHEN                                                          = 0x05,
    /// 0x06: Request not supported or not understood
    BES_ATT_ERR_REQUEST_NOT_SUPPORTED                                                  = 0x06,
    /// 0x07: Incorrect offset value
    BES_ATT_ERR_INVALID_OFFSET                                                         = 0x07,
    /// 0x08: Authorization privilege not enough
    BES_ATT_ERR_INSUFF_AUTHOR                                                          = 0x08,
    /// 0x09: Capacity queue for reliable write reached
    BES_ATT_ERR_PREPARE_QUEUE_FULL                                                     = 0x09,
    /// 0x0A: Attribute requested not existing
    BES_ATT_ERR_ATTRIBUTE_NOT_FOUND                                                    = 0x0A,
    /// 0x0B: Attribute requested not long
    BES_ATT_ERR_ATTRIBUTE_NOT_LONG                                                     = 0x0B,
    /// 0x0C: Encryption size not sufficient
    BES_ATT_ERR_INSUFF_ENC_KEY_SIZE                                                    = 0x0C,
    /// 0x0D: Invalid length of the attribute value
    BES_ATT_ERR_INVALID_ATTRIBUTE_VAL_LEN                                              = 0x0D,
    /// 0x0E: Operation not fit to condition
    BES_ATT_ERR_UNLIKELY_ERR                                                           = 0x0E,
    /// 0x0F: Attribute requires encryption before operation
    BES_ATT_ERR_INSUFF_ENC                                                             = 0x0F,
    /// 0x10: Attribute grouping not supported
    BES_ATT_ERR_UNSUPP_GRP_TYPE                                                        = 0x10,
    /// 0x11: Resources not sufficient to complete the request
    BES_ATT_ERR_INSUFF_RESOURCE                                                        = 0x11,
    /// 0x80: Application error (also used in PRF Errors)
    BES_ATT_ERR_APP_ERROR                                                              = 0x80,
}bes_gatt_att_status_e;

typedef enum
{
    BES_LINK_EVENT_GATT_CONNECT,
    BES_LINK_EVENT_GATT_DISCONNECT,
    BES_LINK_EVENT_GATT_MAX
} bes_gatt_link_event_e;

typedef enum
{
    BES_GATT_DB_PRIMARY_SERVICE,
    BES_GATT_DB_SECONDARY_SERVICE,
    BES_GATT_DB_INCLUDED_SERVICE,
    BES_GATT_DB_CHARACTERISTIC,
    BES_GATT_DB_DESCRIPTOR,
} bes_gatt_type_e;

typedef enum
{
    BES_GATT_WRITE_NO_RESPONSE = 1,
    BES_GATT_WRITE_WITH_RESPONSE = 2,
    BES_GATT_WRITE_PREPARE = 3,
}bes_gatt_write_type_e;

/// GATT Service Discovery Type
typedef enum
{
    /// Discover all primary services
    BES_GATT_DISCOVER_SVC_PRIMARY_ALL         = 0x00,
    /// Discover primary services using UUID value
    BES_GATT_DISCOVER_SVC_PRIMARY_BY_UUID     = 0x01,
    /// Discover all secondary services
    BES_GATT_DISCOVER_SVC_SECONDARY_ALL       = 0x02,
    /// Discover secondary services using UUID value
    _BES_GATT_DISCOVER_SVC_SECONDARY_BY_UUID   = 0x03,
}bes_gatt_discovery_type_e;

/// GATT Characteristic Discovery Type
typedef enum bes_gatt_char_discovery_type
{
    /// Discover all characteristics
    BES_GATT_DISCOVER_CHAR_ALL      = 0x00,
    /// Discover characteristics using UUID value
    BES_GATT_DISCOVER_CHAR_BY_UUID  = 0x01,
}bes_gatt_char_discovery_type_e;

typedef struct
{
    uint32_t id;
    char uuid[BES_GATT_MAX_UUID_LEN];
    uint8_t uuid_len;
    bes_gatt_type_e type;
    uint32_t handle;
    /*
     * If |type| is |GATT_DB_PRIMARY_SERVICE|, or
     * |GATT_DB_SECONDARY_SERVICE|, this contains the start and end attribute
     * handles.
     */
    uint32_t            start_handle;
    uint32_t            end_handle;
    /*
     * If |type| is |GATT_DB_CHARACTERISTIC|, this contains the properties of
     * the characteristic.
     */
    uint32_t             properties; // see gatt_att_info_bf
} bes_gatt_service_elem_t;

typedef struct _bes_gatt_services_s
{
    int32_t count;
    bes_gatt_service_elem_t pElem[ADAPTER_BLE_SVC_MAX_ELEMENT];
} bes_gatt_services_t;

typedef struct __adapter_client_db{
    bool        occupied;
    uint32_t    client_if;
    uint8_t     ble_addr[6];
    //16 32 128
    uint16_t    uuid_len;
    //connection index
    uint8_t     conidx;
    void*       prf_env;
    void*       task_env;
    uint8_t     is_read_by_uuid;
    //save discovered svc
    bes_gatt_services_t svc_content;
}bes_adapter_client_db_t;

typedef struct __adapter_client_task_env{
    //gatt user local index
    uint8_t user_lid;
    //client task id
    unsigned short task_id;
    void * prf_env;
}adapter_client_task_env_t;

typedef struct __bes_adapter_attm_item{
    unsigned char uuid[16];
    unsigned char uuid_len;
    unsigned short prop;
    unsigned short perm;
    unsigned short ext_perm;
    //*******************/
    unsigned short declaration_offset;
    uint8_t * p_value;
    uint8_t   value_len;
}bes_adapter_attm_item_t;

typedef struct __adapter_attm_desc{
    bool occupied;
    unsigned char uuid[16];
    unsigned char uuid_len;
    unsigned short handle_offset;
    unsigned short declaration_hdl_offset;
    unsigned short perm;
    unsigned short ext_perm;
    unsigned short max_size;
    unsigned char data_len;
    unsigned char data[ADAPTER_BLE_MAX_ATT_SIZE];
}bes_adapter_attm_desc_t;

typedef struct __adapter_service_db{
    bool occupied;
    bool use_bes_db;
    unsigned char inst_id;
    unsigned char is_primary;
    unsigned char uuid[16];
    unsigned char uuid_len;
    unsigned char nattb;
    unsigned char service_perm;
    unsigned short handle_offset;
    unsigned short start_handle;
    unsigned short task_id;
    unsigned short user_lid;
    unsigned short uuid_mem_size;
    void*             stack_srv_db;
    void *            prf_env;
    int32_t           server_if;
    bes_adapter_attm_desc_t attb[BLE_ADAPTER_ATTB_NUM];
}bes_adapter_ble_service_db_t;

typedef struct
{
    int32_t server_if;
    char uuid[BES_GATT_MAX_UUID_LEN];
    int32_t srvc_handle;
    int32_t char_handle;
} bes_gatts_add_char_para_t ;

typedef struct
{
    int32_t server_if;
    char uuid[BES_GATT_MAX_UUID_LEN];
    int32_t srvc_handle;
    int32_t descr_handle;
} bes_gatts_add_desc_para_t;

typedef struct
{
    int32_t conn_id;
    int32_t trans_id;
    char btaddr[MAX_BDADDR_LEN];
    int32_t attr_handle;
    int32_t offset;
    int32_t length;
    uint8_t need_rsp;
    uint8_t is_prep;
    uint8_t value[BES_GATT_MAX_ATTR_LEN];
} bes_gatts_write_req_para_t;

typedef struct
{
    int32_t conn_id;
    int32_t trans_id;
    char btaddr[MAX_BDADDR_LEN];
    int32_t attr_handle;
    int32_t offset;
    uint8_t is_long;
} bes_gatts_read_req_para_t;

//ble client
#define MAX_GATTC_SVC_NUM (3)

/// service handles
typedef struct
{
    /// start handle
    uint16_t shdl;
    /// end handle
    uint16_t ehdl;
}bes_gattc_svc_hdl_t;

typedef struct
{
    uint8_t svc_num;
    bes_gattc_svc_hdl_t svc[MAX_GATTC_SVC_NUM];
}bes_gattc_svc_notify_t;

typedef void (*bes_gattc_init_cb)(uint16_t status);
/**
 * @brief Inform the ble connection result
 * @param[in] connId Connection ID
 * @param[in] bdAddr Peer bluetooth device addr
 * @param[in] event See BT_GATTC_EVENT_T enums
 * @return void
 */
typedef void (*bes_gattc_link_event_cb)(int32_t connId, const unsigned char* bdAddr, bes_gatt_link_event_e event);
/**
 * @brief Inform the ble connection parameters update result
 * @param[in] connId Connection ID
 * @param[in] interval Connection interval
 * @param[in] latency Connection latency
 * @param[in] supervisiontimeout Connection supervisiontimeout
 * @return void
 */
typedef void (*bes_gattc_conn_para_update_cb)(int32_t connId, int32_t interval, int32_t latency, int32_t supervisiontimeout);

/**
 * @brief gatt client discovery services complete callback
 *  @param[in] connId Connection id
  * @param[in] write complete Status
 * @return void
 */
typedef void (*bes_gattc_discovery_complete_cb)(int32_t connId, bes_gatt_status_e status);
/**
 * @brief Get the services list
 * @param[in] gattServices Services list
 * @return void
 */
typedef void (*bes_gattc_service_discovered_cb)(uint16_t srv_index, bes_gatt_services_t* gattServices);
/**
 * @brief Inform character read result
 * @param[in] connId Connection id
 * @param[in] handle Attribute handle
 * @param[in] status Status
 * @param[in] data Att data read
 * @param[in] size Length of data in bytes
 * @return void
 */
typedef void (*bes_gattc_character_read_cb)(int32_t connId, int32_t handle, bes_gatt_status_e status,
                                        const uint8_t* data, int32_t size);

/**
 * @brief Inform characteristic write result
 * @param[in] connId Connection id
 * @param[in] handle Attribute handle
 * @param[in] status Status
 * @return void
 */
typedef void (*bes_gattc_character_write_cb)(int32_t connId, uint16_t handle, bes_gatt_status_e status);
/**
 * @brief Inform descriptor write result
 * @param[in] connId Connection id
 * @param[in] handle Attribute handle
 * @param[in] status Status
 * @return void
 */
typedef void (*bes_gattc_descriptor_write_cb)(int32_t connId, uint16_t handle, bes_gatt_status_e status);
/**
 * @brief Inform descriptor read result
 * @param[in] connId Connection id
 * @param[in] handle Attribute handle
 * @param[in] status Status
 * @param[in] data Att data read
 * @param[in] size Length of data in bytes
 * @return void
 */
typedef void (*bes_gattc_descriptor_read_cb)(int32_t connId, int32_t handle, bes_gatt_status_e status,
                                        const uint8_t* data, int32_t size);

/**
 * @brief Notify characteristic changed
 * @param[in] connId Connection id
 * @param[in] handle Attribute handle
 * @param[in] status Status
 * @param[in] data Payload
 * @param[in] size Length of data in bytes
 * @param[in] isNotify Is notify or not
 * @return void
 */
typedef void (*bes_gattc_characte_changed_cb)(int32_t connId, uint32_t handle,
                                        const uint8_t* data, int32_t size, uint8_t isNotify);

/**
 * @brief Enable characteristic notify callbac
 * @param[in] connId Connection id
 * @param[in] handle Attribute handle
 * @param[in] registered 1 is success
 * @return void
 */
typedef  void (*bes_gattc_notification_enable_cb)(int32_t connId, int32_t handle, int32_t registered);

typedef struct __BesGattcStateCb{
    bes_gattc_init_cb initCb;
    bes_gattc_link_event_cb linkEventCb;
    bes_gattc_conn_para_update_cb connUpdateCb;
    bes_gattc_discovery_complete_cb discoveryCmplCb;
    bes_gattc_service_discovered_cb discoveryCb;
    bes_gattc_character_read_cb charReadCb;
    bes_gattc_character_write_cb charWriteCb;
    bes_gattc_descriptor_read_cb descReadCb;
    bes_gattc_descriptor_write_cb descWriteCb;
    bes_gattc_characte_changed_cb charChangedNotifyCb;
    bes_gattc_notification_enable_cb notifyEnableCb;
}bes_gattc_state_callback_t;

/*************************************************************************************/
//BLE SERVER
/**
 * @brief Inform the gatt server init result
 * @param[in] serverIf Indicates id of server interface
 * @return void
 */
typedef void (*bes_gatts_init_cb)(int32_t serverIf);

/**
 * @brief Inform the gatt server a device has connected
 * @param[in] serverIf Indicates id of server interface
 * @param[in] addr Indicates id of the device address
 * @param[in] connId Indicates id of the connection index
 * @return void
 */
typedef void (*bes_gatts_conn_create_cb)(int32_t serverIf,
                                                                            uint8_t* addr,
                                                                            int connId);

/**
 * @brief Inform the gatt server a device has disconnected
 * @param[in] serverIf Indicates id of server interface
 * @param[in] addr Indicates id of the device address
 * @param[in] connId Indicates id of the connection index
 * @return void
 */
typedef void (*bes_gatts_disconn_create_cb)(int32_t serverIf,
                                                                            uint8_t* addr,
                                                                            int connId);

/**
 * @brief Callback invoked in response to create_service
 * @param[in] serverIf Service parameter
 * @param[in] srvc_handle Service parameter
 * @return void
 */
typedef void (*bes_gatts_add_service_cb)(int32_t serverIf, int32_t srvc_handle);

/**
 * @brief Callback invoked when a characteristic has been added to a service
 * @param[in] bt_gatts_add_char Characteristic parameter(server if, uuid ,
 *                                      service handle and characteristic handle)
 * @return void
 */
typedef void (*bes_gatts_add_char_cb)(bes_gatts_add_char_para_t *bt_gatts_add_char );

/**
 * @brief Callback invoked when a descriptor has been added to a characteristic
 * @param[in] bt_gatts_add_desc Descriptor parameter(server if, uuid ,
 *                                      service handle and descriptor handle)
 * @return void
 */
typedef void (*bes_gatts_add_desc_cb)(bes_gatts_add_desc_para_t *bt_gatts_add_desc);

/**
 * @brief Callback invoked when a remote device has requested to write to a characteristic or descriptor
 * @param[in] bt_gatts_req_write (connection id,
 *                              transaction id,
 *                              remote device address,
 *                              attribute handle,
 *                              offset,
 *                              length,
 *                              need response flag,
 *                              data of value)
 * @return uint16_t status
 */
typedef uint16_t  (*bes_gatts_req_write_cb)(int32_t server_if,
                                                                    int32_t conn_id,
                                                                    uint16_t task_id,
                                                                    int32_t handle,
                                                                    int32_t offset,
                                                                    int32_t length,
                                                                    uint8_t * data);

typedef void (*bes_gatts_send_ntf_cb)(int32_t server_if,
                                                                    int32_t conn_id,
                                                                    uint16_t task_id,
                                                                    uint16_t status);


/**
  * @brief Callback invoked when a remote device has requested to read a characteristic or descriptor
  * @param[in] bt_gatts_req_read (connection id,
  *                              transaction id,
  *                              remote device address,
  *                              attribute handle,
  *                              offset,
  *                              is long or not)
  * @return void
  */
 typedef void (*bes_gatts_req_read_cb)(int32_t server_if,
                                                                    int32_t conn_id,
                                                                    uint16_t task_id,
                                                                    int32_t handle,
                                                                    int32_t offset);

/**
  * @brief Callback invoked when a remote device has requested to att info
  * @param[in] get_att_info (server_if ,
  *                              conn_id,
  *                              transaction id,
  *                              attribute handle)
  * should send att response
  * @return uint16_t att_len
  */

typedef uint16_t (*bes_gatts_get_att_info_cb)(int32_t server_if,
                                                                                int32_t conn_id,
                                                                                 uint16_t task_id,
                                                                                 int32_t handle);


/**
 * @brief Callback invoked when GATT Service is started
 * @return void
 */
typedef void(*bes_gatts_start_server_cb)(int32_t serverIf);

/**
 * @brief Callback invoked when GATT Service is stop
 * @return void
 */
typedef void(*bes_gatts_stop_server_cb)(int32_t serverIf);

/**
 * @brief Callback invoked when GATT Service is delete
 * @return void
 */
typedef void(*bes_gatts_delete_server_cb)(int32_t serverIf);

/**
 * @brief Callback invoked when mtu size exchanged.
 * @return void
 */
typedef void (*bes_gatts_mtu_exchanged_cb)(uint8_t conidx, uint16_t mtu);


typedef struct __BesGattsStateCb{
    bes_gatts_init_cb initCb;
    bes_gatts_conn_create_cb connCreateCb;
    bes_gatts_disconn_create_cb disconnectCb;
    bes_gatts_add_service_cb addServiceCb;
    bes_gatts_add_char_cb addCharCb;
    bes_gatts_add_desc_cb addDescCb;
    bes_gatts_req_write_cb reqWriteCb;
    bes_gatts_send_ntf_cb sendNtfCb;
    bes_gatts_req_read_cb reqReadCb;
    bes_gatts_get_att_info_cb get_att_info;
    bes_gatts_start_server_cb startServerCb;
    bes_gatts_stop_server_cb stopServerCb;
    bes_gatts_delete_server_cb deleteServerCb;
    bes_gatts_mtu_exchanged_cb mtuExchangedCb;
}bes_gatts_state_callback_t;
#endif
