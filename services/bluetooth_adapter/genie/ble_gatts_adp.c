#include "tg_bluetooth.h"
#include "bt_adaptor_dbg.h"
#include "bleadapter_utils.h"
#ifdef IBRT
#include "app_ibrt_customif_cmd.h"
#include "app_tws_ctrl_thread.h"
#include "hal_trace.h"
#include "app_ibrt_if.h"
#include "app_tws_ibrt.h"
#endif
#include "bt_service_utils.h"
#include "bes_gatts_adapter.h"
#include <string.h>


//interface connId start from 1.
#define gattsTransferConnIdIf(connId) (connId + 1)
//stack conidx start from 0.
#define gattsTransferConnIdStack(connId) (connId - 1)

//gatt server
static uint8_t gAdvEnableFlag = 0;
static unsigned char gAdvData[31];
static unsigned short gAdvLen = 0;
//gatt server param
typedef struct __ble_gatts_env
{
    bool occupied;
    int32_t conn_id;
    int32_t server_if;
    int32_t service_db_if;
    bes_adapter_ble_service_db_t *service_db;
    TG_BT_GATTS_CB_FUNC_T gatts_cb;
}ble_gatts_env_t;

ble_gatts_env_t gGattServerEnv[BLE_ADAPTER_PROFILES_NUM];



ble_gatts_env_t * ble_gatts_get_free_gatts_server(void)
{
    for(uint32_t i = 0; i < BLE_ADAPTER_PROFILES_NUM; i++){
        if(!gGattServerEnv[i].occupied){
            gGattServerEnv[i].occupied = true;
            gGattServerEnv[i].server_if = i + 1;   //start from 1

            return &gGattServerEnv[i];
        }
    }
    return NULL;
}

ble_gatts_env_t * ble_gatts_get_server_by_if(uint32_t server_if)
{
    for(uint32_t i = 0; i < BLE_ADAPTER_PROFILES_NUM; i++){
        if(gGattServerEnv[i].occupied && (gGattServerEnv[i].server_if == server_if)){
            return &gGattServerEnv[i];
        }
    }
    return NULL;
}

ble_gatts_env_t * ble_gatts_get_server_by_service_if(uint32_t service_db_if)
{
    for(uint32_t i = 0; i < BLE_ADAPTER_PROFILES_NUM; i++){
        if(gGattServerEnv[i].occupied && (gGattServerEnv[i].service_db_if == service_db_if)){
            return &gGattServerEnv[i];
        }
    }
    return NULL;
}

ble_gatts_env_t * ble_gatts_get_server_by_conn_id(int32_t conn_id)
{
    for(uint32_t i = 0; i < BLE_ADAPTER_PROFILES_NUM; i++){
        if(gGattServerEnv[i].occupied && (gGattServerEnv[i].conn_id == conn_id)){
            return &gGattServerEnv[i];
        }
    }
    return NULL;
}


/**
 * @brief Inform the gatt server init result
 * @param[in] serverIf Indicates id of server interface
 * @return void
 */
static void BesGattServerInitCallback(int32_t service_db_if)
{
    LOG_I("%s serverIf:%d", __FUNCTION__, service_db_if);
}

static void BesGattServerConnCreatedCallback(int32_t service_db_if,
                                                                            uint8_t* addr,
                                                                            int connId)
{
    LOG_I("%s serverIf:%d connId:%d", __FUNCTION__, service_db_if, connId);
}

static void BesGattServerDisconnectCallback(int32_t service_db_if, uint8_t* addr, int connId)
{
    TG_BT_GATTS_EVENT_T evt = TG_BT_GATTS_DISCONNECT;
    ble_gatts_env_t *gatts_env = ble_gatts_get_server_by_service_if(service_db_if);

    LOG_I("%s serverIf:%d connId:%d", __FUNCTION__, service_db_if, connId);
    if(!gatts_env){
        LOG_W("%s error: service_db_if:%d error", __FUNCTION__, service_db_if);
        return;
    }
    if(gatts_env->gatts_cb.gattsConnectionEventCB)
    {
        gatts_env->gatts_cb.gattsConnectionEventCB(evt, gatts_env->server_if, connId);
        LOG_I("%s gattsConnectionEventCB conn_id:%d", __FUNCTION__, connId);
    }
}

/**
 * @brief Callback invoked in response to create_service
 * @param[in] serverIf Service parameter
 * @param[in] srvc_handle Service parameter
 * @return void
 */
static void BesGattServerAddServiceCallback(int32_t service_db_if, int32_t srvc_handle)
{
    LOG_I("%s serverIf:%d", __FUNCTION__, service_db_if);
}

/**
 * @brief Callback invoked when a characteristic has been added to a service
 * @param[in] bt_gatts_add_char Characteristic parameter(server if, uuid ,
 *                                      service handle and characteristic handle)
 * @return void
 */
static void BesGattServerAddCharCallback(bes_gatts_add_char_para_t *bt_gatts_add_char )
{
    LOG_I("%s server_if:%d", __FUNCTION__, bt_gatts_add_char->server_if);
}

/**
 * @brief Callback invoked when a descriptor has been added to a characteristic
 * @param[in] bt_gatts_add_desc Descriptor parameter(server if, uuid ,
 *                                      service handle and descriptor handle)
 * @return void
 */
static void BesGattServerAddDescCallback(bes_gatts_add_desc_para_t *bt_gatts_add_desc)
{
    LOG_I("%s server_if:%d srvc_handle:%x", __FUNCTION__, bt_gatts_add_desc->server_if, bt_gatts_add_desc->srvc_handle);
}

/**
 * @brief Callback invoked when a remote device has requested to write to a characteristic or descriptor
 * @return void
 */
static uint16_t BesGattServerReqWriteCallback(int32_t service_db_if,
                                                                    int32_t conn_id,
                                                                    uint16_t task_id,
                                                                    int32_t handle,
                                                                    int32_t offset,
                                                                    int32_t length,
                                                                    uint8_t * data)
{
    uint8_t * ble_addr;
    TG_BT_GATTS_REQ_WRITE_RST_T wParam;
    ble_gatts_env_t *gatts_env = ble_gatts_get_server_by_service_if(service_db_if);
    LOG_I("%s server_if:%d handle:%x", __FUNCTION__, service_db_if, handle);
    if(!gatts_env){
        LOG_W("%s error: service_db_if:%d error", __FUNCTION__, service_db_if);
        return -1;
    }
    gatts_env->conn_id = conn_id;
    ble_addr = btadapter_ble_get_addr_by_conidx(conn_id);
    if(gatts_env->gatts_cb.gattsReqWriteCB){
        wParam.attr_handle = handle;
        btAddressToMac((unsigned char *)wParam.btaddr, ble_addr);
        wParam.conn_id = conn_id;
        wParam.is_prep = false;
        wParam.need_rsp = true;
        wParam.offset = offset;
        wParam.trans_id = 1;
        wParam.length = length > TG_BT_GATT_MAX_ATTR_LEN ? TG_BT_GATT_MAX_ATTR_LEN : length;

        memcpy(wParam.value, data, length);
        gatts_env->gatts_cb.gattsReqWriteCB(&wParam, gatts_env->server_if);
        LOG_I("%s gattsReqWriteCB", __FUNCTION__);
    }
    btadapter_ble_service_send_gatt_write_response(conn_id, gatts_env->service_db, BES_ATT_ERR_NO_ERROR);
    return BES_BLE_ERR_NO_ERROR;
}

void BesGattServerNTFCallback(int32_t service_db_if,
                                                                    int32_t conn_id,
                                                                    uint16_t task_id,
                                                                    uint16_t status)
{
    uint8_t * ble_addr;

    ble_addr = btadapter_ble_get_addr_by_conidx(conn_id);
    LOG_I("%s server_if:%d conn_id:%d", __func__, service_db_if, conn_id);
}


/**
  * @brief Callback invoked when a remote device has requested to read a characteristic or descriptor
  * @return void
  */
 static void BesGattServerReqReadCallback(int32_t service_db_if,
                                                                    int32_t conn_id,
                                                                    uint16_t task_id,
                                                                    int32_t handle,
                                                                    int32_t offset)
{
    uint8_t * ble_addr;
    TG_BT_GATTS_REQ_READ_RST_T rParam;
    ble_gatts_env_t *gatts_env = ble_gatts_get_server_by_service_if(service_db_if);

    LOG_I("%s server_if:%d handle:%x", __func__, service_db_if, handle);
    if(!gatts_env){
        LOG_W("%s error: service_db_if:%d error", __FUNCTION__, service_db_if);
        return;
    }
    gatts_env->conn_id = conn_id;
    ble_addr = btadapter_ble_get_addr_by_conidx(conn_id);
    if(gatts_env->gatts_cb.gattsReqReadCB){
        rParam.attr_handle = handle;
        btAddressToMac((unsigned char *)rParam.btaddr, ble_addr);
        rParam.conn_id = conn_id;
        rParam.trans_id = 1;
        rParam.is_long = false;
        rParam.offset = offset;
        gatts_env->gatts_cb.gattsReqReadCB(&rParam, gatts_env->server_if);
        LOG_I("%s gattsReqReadCB", __FUNCTION__);
    }
}

/**
 * @brief Callback invoked when GATT Service is started
 * @return void
 */
static void BesGattServerStartServerCallback(int32_t service_db_if)
{
    ble_gatts_env_t *gatts_env = ble_gatts_get_server_by_service_if(service_db_if);

    LOG_I("%s server_if:%d", __FUNCTION__, gatts_env->server_if);
    if(!gatts_env){
        LOG_W("%s error: service_db_if:%d error", __FUNCTION__, service_db_if);
        return;
    }
    if(gatts_env->gatts_cb.gattsStartServerCB){
        gatts_env->gatts_cb.gattsStartServerCB(gatts_env->server_if);
        LOG_I("%s gattsStartServerCB server_if:%d", __FUNCTION__, gatts_env->server_if);
    }
}

/**
 * @brief Callback invoked when GATT Service is stop
 * @return void
 */
static void BesGattServerStopServerCallback(int32_t service_db_if)
{
    ble_gatts_env_t *gatts_env = ble_gatts_get_server_by_service_if(service_db_if);

    LOG_I("%s server_if:%d", __FUNCTION__, service_db_if);
    if(!gatts_env){
        LOG_W("%s error: service_db_if:%d error", __FUNCTION__, service_db_if);
        return;
    }

    if(gatts_env->gatts_cb.gattsStopServerCB){
        gatts_env->gatts_cb.gattsStopServerCB(gatts_env->server_if);
        LOG_I("%s gattsStopServerCB server_if:%d", __FUNCTION__, gatts_env->server_if);
    }
}

/**
 * @brief Callback invoked when GATT Service is delete
 * @return void
 */
static void BesGattServerDeleteServerCallback(int32_t service_db_if)
{
    ble_gatts_env_t *gatts_env = ble_gatts_get_server_by_service_if(service_db_if);

    LOG_I("%s server_if:%d", __FUNCTION__, service_db_if);
    if(!gatts_env){
        LOG_W("%s error: service_db_if:%d error", __FUNCTION__, service_db_if);
        return;
    }

    if(gatts_env->gatts_cb.gattsDeleteServerCB){
        gatts_env->gatts_cb.gattsDeleteServerCB(gatts_env->server_if);
        LOG_I("%s gattsDeleteServerCB server_if:%d", __FUNCTION__, gatts_env->server_if);
    }
}

static void BesGattServerMtuExchangeCallback(uint8_t conidx, uint16_t mtu)
{
    LOG_I("%s conidx:%d mtu:%d", __func__, conidx, mtu);
}

static void BesGattServerRegisterCallback(void)
{
    bes_gatts_state_callback_t gattsCb;

    gattsCb.connCreateCb = BesGattServerConnCreatedCallback;
    gattsCb.disconnectCb = BesGattServerDisconnectCallback;
    gattsCb.addCharCb = BesGattServerAddCharCallback;
    gattsCb.addDescCb = BesGattServerAddDescCallback;
    gattsCb.addServiceCb = BesGattServerAddServiceCallback;
    gattsCb.deleteServerCb = BesGattServerDeleteServerCallback;
    gattsCb.initCb = BesGattServerInitCallback;
    gattsCb.reqReadCb = BesGattServerReqReadCallback;
    gattsCb.reqWriteCb = BesGattServerReqWriteCallback;
    gattsCb.sendNtfCb = BesGattServerNTFCallback;
    gattsCb.startServerCb = BesGattServerStartServerCallback;
    gattsCb.stopServerCb = BesGattServerStopServerCallback;
    gattsCb.mtuExchangedCb = BesGattServerMtuExchangeCallback;
    btadapter_ble_register_server_callback(&gattsCb);
}


/**
 * @brief gatt service initial
 *        should callback gattsInitCB when finished
 * @param[in]: None.
 * @return: server_if on success or error code otherwise
 */
int32_t tg_bt_initGatts(TG_BT_GATTS_CB_FUNC_T *callback)
{
    static bool init_flag = false;
    ble_gatts_env_t *gatts_env = NULL;

    if(!init_flag){
        LOG_I("%s init", __FUNCTION__);
        BesGattServerRegisterCallback();
        init_flag = true;
    }
    gatts_env = ble_gatts_get_free_gatts_server();
    if(!gatts_env){
        LOG_W("%s there is no gatts_server", __FUNCTION__);
        return -1;
    }

    //memcpy callbacks
    memcpy((uint8_t*)&gatts_env->gatts_cb, (uint8_t*)callback, sizeof(TG_BT_GATTS_CB_FUNC_T));
    if(gatts_env->gatts_cb.gattsInitCB){
        LOG_I("%s server_if:%d", __FUNCTION__, gatts_env->server_if);
        gatts_env->gatts_cb.gattsInitCB(gatts_env->server_if);
    }

    return gatts_env->server_if;
}

/**
 * @brief gatt service deinitial
 * @param[in]: None.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_deinitGatts(int32_t server_if)
{
    ble_gatts_env_t *gatts_env = ble_gatts_get_server_by_if(server_if);

    LOG_I("%s", __FUNCTION__);
    if(!gatts_env){
        LOG_W("%s deinit gatts error", __FUNCTION__);
        return -1;
    }

    memset((uint8_t*)gatts_env->service_db, 0, sizeof(bes_adapter_ble_service_db_t));
    memset((uint8_t*)gatts_env, 0, sizeof(ble_gatts_env_t));
    LOG_I("%s server_if:%d", __FUNCTION__, server_if);
    return 0;
}


/**
 * @brief gatt service add service
 *        should callback gattsAddServiceCB when finished
 * @param[in] server_if:  service interface id, returned in gattsInitCB.
 * @param[in] service_uuid:  service UUID.
 * @param[in] is_primary:  1 means primary, 0 means included.
 * @param[in] number:  attribute handle number occupied by this service.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattsAddService(int32_t server_if, int8_t *service_uuid, uint8_t is_primary, int32_t number)
{
    uint32_t status = 0;
    uint32_t uuid_len;
    uint8_t uuid_tmp[ADAPTER_BLE_UUID_128_LEN];
    uint16_t uuid;
    bes_adapter_attm_item_t att_item;
    ble_gatts_env_t *gatts_env = ble_gatts_get_server_by_if(server_if);

    LOG_I("%s server_if:%d number:%d uuid:%s", __FUNCTION__, server_if, number, service_uuid);
    if(!gatts_env){
        LOG_W("%s error: server_if:%d error", __FUNCTION__, server_if);
        return -1;
    }
    uuid_len = bt_hexstr2bytes(service_uuid, uuid_tmp);
    uuid = uuid_tmp[0] << 8 | uuid_tmp[1];
    gatts_env->service_db_if = bes_gatt_server_add_service(-1, (uint8_t*)&uuid, uuid_len, is_primary, number, false);
    if(-1 == gatts_env->service_db_if){
        LOG_W("%s error: no services_db", __FUNCTION__);
        return -1;
    }
    gatts_env->service_db = btadapter_ble_service_get_service_by_if(gatts_env->service_db_if);
    memset(&att_item, 0 ,sizeof(att_item));
    att_item.uuid[0] = 0x00;
    att_item.uuid[1] = 0x28;
    att_item.uuid_len = ADAPTER_BLE_UUID_16_LEN;
    att_item.prop = ADAPTER_BLE_PROP_READ;
    att_item.perm = ADAPTER_BLE_PERM_READ;
    att_item.ext_perm = 0;
    status = bes_gatt_server_add_att_item(gatts_env->service_db_if, gatts_env->service_db->handle_offset ++,
                                            &att_item);
    if(-1 == status){
        LOG_W("%s error: add att error", __FUNCTION__);
        return -1;
    }
    if(gatts_env->gatts_cb.gattsAddServiceCB){
        TG_BT_GATTS_ADD_SRVC_RST_T bt_gatts_add_srvc;
        bt_gatts_add_srvc.server_if = server_if;
        bt_gatts_add_srvc.srvc_handle = gatts_env->service_db->start_handle;
        bt_gatts_add_srvc.srvc_id.is_primary = is_primary;
        bt_gatts_add_srvc.srvc_id.id.inst_id = gatts_env->service_db->inst_id;
        memcpy( bt_gatts_add_srvc.srvc_id.id.uuid, service_uuid, strlen((char *)service_uuid));
        gatts_env->gatts_cb.gattsAddServiceCB(&bt_gatts_add_srvc);
        LOG_I("gattsAddServiceCBc server_if:%d srvc_handle:%d \n", server_if, gatts_env->service_db->start_handle);
    }
    return 0;
}

/**
 * @brief gatt service add character
 *        should callback gattsAddCharCB when finished
 * @param[in] server_if:  service interface id, returned in gattsInitCB.
 * @param[in] service_handle: service handle, returned in gattsAddServiceCB.
 * @param[in] uuid:  character UUID.
 * @param[in] properties:  gatt properties set.
 * @param[in] permissions:  gatt permission set.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattsAddChar(int32_t server_if, int32_t service_handle, int8_t *uuid, int32_t properties,
                  int32_t permissions)
{
    uint32_t status = 0;
    uint8_t uuid_tmp[ADAPTER_BLE_UUID_128_LEN];
    uint32_t uuid_len;
    int char_handle = 0;
    bes_adapter_attm_item_t att_item;
    ble_gatts_env_t *gatts_env = ble_gatts_get_server_by_if(server_if);

    LOG_I("%s server_if:%d service_handle:%d uuid:%s", __FUNCTION__, server_if, service_handle, uuid);
    if(!gatts_env){
        LOG_W("%s error: server_if:%d error", __FUNCTION__, server_if);
        return -1;
    }

    memset(&att_item, 0 ,sizeof(att_item));
    att_item.uuid[0] = 0x03;
    att_item.uuid[1] = 0x28;
    att_item.uuid_len = ADAPTER_BLE_UUID_16_LEN;
    att_item.prop = ADAPTER_BLE_PROP_READ;
    att_item.perm = ADAPTER_BLE_PERM_READ;
    att_item.ext_perm = 0;
    status = bes_gatt_server_add_att_item(gatts_env->service_db_if, gatts_env->service_db->handle_offset ++,
                                            &att_item);
    if(-1 == status){
        LOG_W("%s error: add att error", __FUNCTION__);
        return -1;
    }

    uuid_len = bt_hexstr2bytes(uuid, uuid_tmp);

    memset(&att_item, 0 ,sizeof(att_item));
    att_item.prop = properties;
    att_item.perm = permissions;
    att_item.ext_perm = BES_GATT_MAX_ATTR_LEN;
    att_item.uuid_len = uuid_len;
    //memcpy(att_item.uuid, uuid_tmp, uuid_len);
    att_item.uuid[0] = uuid_tmp[1];
    att_item.uuid[1] = uuid_tmp[0];
    char_handle = gatts_env->service_db->start_handle + gatts_env->service_db->handle_offset;
    status = bes_gatt_server_add_att_item(gatts_env->service_db_if, gatts_env->service_db->handle_offset ++,
                                            &att_item);
    if(-1 == status){
        LOG_W("%s error: add att error", __FUNCTION__);
        return -1;
    }
    if(gatts_env->gatts_cb.gattsAddCharCB){
        TG_BT_GATTS_ADD_CHAR_RST_T bt_gatts_add_char;
        bt_gatts_add_char.char_handle = char_handle;
        bt_gatts_add_char.srvc_handle = service_handle;
        bt_gatts_add_char.server_if = server_if;
        memcpy(bt_gatts_add_char.uuid, uuid, strlen((char *)uuid));
        gatts_env->gatts_cb.gattsAddCharCB(&bt_gatts_add_char);
        LOG_I("gattsAddCharCB server_if:%d, handle:%02x \n", server_if, service_handle);
    }
    return 0;
}

/**
 * @brief gatt service add character descriptor
 *        should callback gattsAddDescCB when finished
 * @param[in] server_if:  service interface id, returned in gattsInitCB.
 * @param[in] service_handle:  service handle, returned in gattsAddServiceCB.
 * @param[in] uuid:  descriptor UUID.
 * @param[in] permissions:  gatt permission set.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattsAddDesc(int32_t server_if, int32_t service_handle, int8_t *uuid, int32_t permissions)
{
    int status = 0;
    int descr_handle = 0;
    uint8_t uuid_tmp[ADAPTER_BLE_UUID_16_LEN];
    bes_adapter_attm_item_t att_item;
    ble_gatts_env_t *gatts_env = ble_gatts_get_server_by_if(server_if);

    LOG_I("%s server_if:%d service_handle:%d uuid:%s", __FUNCTION__, server_if, service_handle, uuid);
    if(!gatts_env){
        LOG_W("%s error: server_if:%d error", __FUNCTION__, server_if);
        return -1;
    }
    memset(&att_item, 0 ,sizeof(att_item));
    att_item.prop = ADAPTER_BLE_PROP_READ;
    att_item.perm = permissions;
    att_item.ext_perm = ADAPTER_BLE_MAX_ATT_SIZE;
    bt_hexstr2bytes(uuid, uuid_tmp);
    att_item.uuid[0] = uuid_tmp[1];  //0x02
    att_item.uuid[1] = uuid_tmp[0];  //0x29
    att_item.uuid_len = ADAPTER_BLE_UUID_16_LEN;
    descr_handle = gatts_env->service_db->start_handle + gatts_env->service_db->handle_offset;
    status = bes_gatt_server_add_att_item(gatts_env->service_db_if, gatts_env->service_db->handle_offset ++,
                                            &att_item);
    if(-1 == status){
        LOG_W("%s error: add att error", __FUNCTION__);
        return -1;
    }
    if(gatts_env->gatts_cb.gattsAddDescCB){
        TG_BT_GATTS_ADD_DESCR_RST_T bt_gatts_add_desc;
        bt_gatts_add_desc.descr_handle = descr_handle;
        bt_gatts_add_desc.srvc_handle = service_handle;
        bt_gatts_add_desc.server_if = server_if;
        memcpy(bt_gatts_add_desc.uuid, uuid, strlen((char *)uuid));
        gatts_env->gatts_cb.gattsAddDescCB(&bt_gatts_add_desc);
        LOG_I("gattsAddDescCB server_if:%d, handle:%02x \n", server_if, service_handle);
    }

    return 0;
}

/**
 * @brief gatt service start service
 * @param[in] server_if:  service interface id, returned in gattsInitCB.
 * @param[in] service_handle:  service handle, returned in gattsAddServiceCB.
 * @param[in] is_primary:  1 means primary, 0 means included.
 * @param[in] number:  attribute handle number occupied by this service.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattsStartService(int32_t server_if, int32_t service_handle, int32_t transport)
{
    int status = 0;
    ble_gatts_env_t *gatts_env = ble_gatts_get_server_by_if(server_if);

    LOG_I("%s server_if:%d service_handle:%d", __FUNCTION__, server_if, service_handle);
    if(!gatts_env){
        LOG_W("%s error: server_if:%d error", __FUNCTION__, server_if);
        return -1;
    }
    status = bes_gatt_server_start_service(gatts_env->service_db_if);
    if(status) {
        LOG_W("%s start server error status:%d", __FUNCTION__, status);
    }

    return status;
}

/**
 * @brief gatt service stop service
 * @param[in] server_if:  service interface id, returned in gattsInitCB.
 * @param[in] service_handle:  service handle, returned in gattsAddServiceCB.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattsStopService(int32_t server_if, int32_t service_handle)
{
    int status = 0;
    ble_gatts_env_t *gatts_env = ble_gatts_get_server_by_if(server_if);

    LOG_I("%s server_if:%d service_handle:%d", __FUNCTION__, server_if, service_handle);
    if(!gatts_env){
        LOG_W("%s error: server_if:%d error", __FUNCTION__, server_if);
        return -1;
    }
    status = bes_gatt_server_stop_service(gatts_env->service_db_if);
    return status;
}

/**
 * @brief gatt service delete service
 * @param[in] server_if:  service interface id, returned in gattsInitCB.
 * @param[in] service_handle:  service handle, returned in gattsAddServiceCB.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattsDeleteService(int32_t server_if, int32_t service_handle)
{
    int status = 0;
    ble_gatts_env_t *gatts_env = ble_gatts_get_server_by_if(server_if);

    LOG_I("%s server_if:%d service_handle:%d", __FUNCTION__, server_if, service_handle);
    if(!gatts_env){
        LOG_W("%s error: server_if:%d error", __FUNCTION__, server_if);
        return -1;
    }
    status = bes_gatt_server_delete_service(gatts_env->service_db_if);
    return status;
}

/**
 * @brief gatt unregister all services on this server interface
 * @param[in] server_if:  service interface id, returned in gattsInitCB.
 * @return: None
 */
int32_t tg_bt_gattsUnregisterService(int32_t server_if)
{
    ble_gatts_env_t *gatts_env = ble_gatts_get_server_by_if(server_if);

    LOG_I("%s server_if:%d", __FUNCTION__, server_if);
    if(!gatts_env){
        LOG_W("%s error: server_if:%d error", __FUNCTION__, server_if);
        return -1;
    }

    memset((uint8_t*)gatts_env->service_db, 0, sizeof(bes_adapter_ble_service_db_t));

    return 0;
}

/**
 * @brief gatt send reponse when received remote read/write att command
 * @param[in] conn_id:  bluetooth connection handle.
 * @param[in] trans_id:  bluetooth transaction id in
 * @param[in] status:  reserved, should set to 0
 * @param[in] handle:  attribute handle which want response
 * @param[in] p_value:  attribute value which want response
 * @param[in] offset:  attribute value offset which want response
 * @param[in] value_len:  attribute value length which want response
 * @param[in] auth_req: reserved, should set to 0
 * @return: None
 */
int32_t tg_bt_gattsSendResponse(int32_t conn_id, int32_t trans_id, int32_t status, int32_t handle, int8_t *p_value,
                       int32_t offset, int32_t value_len, int32_t auth_req)
{
    int state = 0;
    ble_gatts_env_t *gatts_env = ble_gatts_get_server_by_conn_id(conn_id);

    LOG_I("%s conn_id:%d handle:%d", __FUNCTION__, conn_id, handle);
    if(!gatts_env){
        LOG_W("%s error: conn_id:%d error", __FUNCTION__, conn_id);
        return -1;
    }

    state = btadapter_ble_service_send_gatt_get_response(gatts_env->service_db, conn_id, handle,
                                                            (uint8_t*)p_value, value_len, status);

    return state;
}

/**
 * @brief gatt send indication/notification to remote
 * @param[in] server_if:  service interface id, returned in gattsInitCB.
 * @param[in] handle:  attribute handle which want indication
 * @param[in] conn_id:  bluetooth connection handle
 * @param[in] fg_confirm:  1 means indication, 0 means notification
 * @param[in] p_value:  attribute value which want indication
 * @param[in] value_len:  attribute value length which want indication
 * @return: None
 */
int32_t tg_bt_gattsSendIndication(int32_t server_if, int32_t handle, int32_t conn_id, int32_t fg_confirm,
                         int8_t *p_value, int32_t value_len)
{
    int status = 0;
    ble_gatts_env_t *gatts_env = ble_gatts_get_server_by_if(server_if);

    LOG_I("%s server_if:%d handle:%d conn_id:%d", __FUNCTION__, server_if, handle, conn_id);
    if(!gatts_env){
        LOG_W("%s error: server_if:%d error", __FUNCTION__, server_if);
        return -1;
    }
    status = bes_gatt_server_send_indication(gatts_env->service_db_if, conn_id, handle, fg_confirm,
                                              p_value, value_len);
    return status;
}


/**
 * @brief gatt server advertise broadcast packet data set
 * @param[in] data:  advertise data.
 * @param[in] len:  advertise data length.
 * @return: None
 */
int32_t tg_bt_gattsSetAdvData(uint8_t *data, int len)
{
    LOG_I("%s", __FUNCTION__);
    return 0;
}

/**
 * @brief gatt server enable/disable advertise broadcast
 * @param[in] enable: 1 means enable; 0 means disable.
 * @return: None
 */
int32_t tg_bt_gattsEnableAdv(bool enable)
{
    LOG_I("%s", __FUNCTION__);
    return 0;
}


//---------------------------------------debug-----------------------------
//print the gatt_db database, when use it, put it in gatt_db.c file
// void gatt_dbg_db_svc_list_print(void)
// {
//     gatt_db_svc_t* p_current_svc = gatt_env.p_db;
//     uint8_t nb_svc = 0;
//     uint8_t cursor = 0;
//     gatt_db_att_t *p_att = NULL;
//     gatt_db_svc_t* p_svc = NULL;
//     TRACE(1, "%s", __FUNCTION__);
//     // count number of services
//     while((p_current_svc != NULL))
//     {
//         nb_svc++;
//         p_current_svc = p_current_svc->p_next;
//     }

//     // Fill Service information
//     p_current_svc = gatt_env.p_db;
//     while((p_current_svc != NULL))
//     {
//         TRACE(2, "service idx:%d nb_att:%d", cursor, p_current_svc->nb_att);
//         TRACE(4, "start_hdl:%d end_hdl:%d info:%d user_lid:%d", p_current_svc->start_hdl, GATT_DB_END_HDL_GET(p_current_svc),
//                                     p_current_svc->perm, p_current_svc->user_lid);

//         if(GETF(p_current_svc->perm, GATT_SVC_UUID_TYPE) == GATT_UUID_16)
//         {
//             DUMP8("%02x", &(p_current_svc->uuid), GATT_UUID_16_LEN);
//         }
//         else
//         {
//             DUMP8("%02x", ((uint8_t*) p_current_svc) + p_current_svc->uuid, GATT_UUID_128_LEN);
//         }
//         for(uint32_t i = 0; i < p_current_svc->nb_att; i++)
//         {
//             uint16_t handle = p_current_svc->start_hdl + i;
//             uint16_t status = gatt_db_att_get(handle, &p_svc, &p_att);
//             if( status == GAP_ERR_NO_ERROR)
//             {
//                 TRACE(3, "handle:%d perm:%x ext_info:%x", handle, p_att->perm, p_att->ext_info);
//                 if(GETF(p_att->perm, GATT_ATT_UUID_TYPE) == GATT_UUID_16)
//                 {
//                     DUMP8("%02x", &(p_att->uuid), GATT_UUID_16_LEN);
//                 }
//                 else
//                 {
//                     DUMP8("%02x", ((uint8_t*) p_svc) + p_att->uuid, GATT_UUID_128_LEN);
//                 }
//             }
//         }
//         cursor++;
//         p_current_svc = p_current_svc->p_next;
//     }
// }

//------------------------------------------------------------------------------------
