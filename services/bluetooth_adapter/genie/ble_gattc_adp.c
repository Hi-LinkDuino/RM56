#include "string.h"
#include "hal_trace.h"
#include "tg_bluetooth.h"
#include "bt_adaptor_dbg.h"
#include "bleadapter_utils.h"
#include "bleadapter_utils.h"
#include "bes_gatts_adapter.h"
#include "btadapter_ble_client.h"
#include "app_ble_mode_switch.h"
#include "app_ble_customif.h"
#include "bt_service_utils.h"
#include "ble_adv_adp.h"
#include "ke_mem.h"
#ifdef IBRT
#include "app_ibrt_customif_cmd.h"
#include "app_tws_ctrl_thread.h"
#include "app_ibrt_if.h"
#include "app_tws_ibrt.h"
#endif

//#define TMALL_GATT_CLIENT_TEST

#define TG_BLE_GATTC_SERVICE_NUM     32
#define TG_BLE_GATTC_CONNECT_NUM     1

#define BT_GATT_CLIENT_DEFAULT_ADV_INTERVAL     50
#define BT_GATT_CLIENT_DEFAULT_SCAN_INTERVAL    144
#define BT_GATT_CLIENT_DEFAULT_SCAN_WINDOW      32
#define BT_GATT_CLIENT_DEFAULT_SCAN_FILTPOLICY    BLE_SCAN_ALLOW_ADV_ALL_AND_INIT_RPA
#define BT_GATT_CLIENT_DEFAULT_SCAN_TYPE        GAPM_ADV_UNDIRECT


#define single_client_if_id     1

//interface connId start from 1.
#define gattcTransferConnIdIf(connId) (connId + 1)
//stack conidx start from 0.
#define gattcTransferConnIdStack(connId) (connId - 1)

typedef struct __ble_gattc_env
{
    bool occupied;
    bool scanflag;
    bool connectedflag;
    int32_t client_if;
    int32_t client_db_if;
    uint8_t conidx;
    uint8_t connectAddr[MAX_BDADDR_LEN];
    uint8_t gattc_service_num;
    TG_BT_gattcServiceElem gattc_service[ADAPTER_BLE_SVC_MAX_ELEMENT];
    TG_BT_GATTC_CB_FUNC_T gattc_cb;
} ble_gattc_env_t;


//client connected num
uint32_t gGattClientConnectedNum = 0;
ble_gattc_env_t gGattClientEnv[BLE_ADAPTER_PROFILES_NUM];


ble_gattc_env_t *ble_gattc_get_free_gattc_client(void)
{
    for(uint32_t i = 0; i < BLE_ADAPTER_PROFILES_NUM; i++){
        if(!gGattClientEnv[i].occupied){
            memset((uint8_t*)&gGattClientEnv[i], 0, sizeof(ble_gattc_env_t));
            gGattClientEnv[i].occupied = true;
            gGattClientEnv[i].client_if = i + 1;    //start from 1....

            return &gGattClientEnv[i];
        }
    }
    return NULL;
}

ble_gattc_env_t *ble_gattc_get_client_by_if(int32_t client_if)
{
    for(uint32_t i = 0; i < BLE_ADAPTER_PROFILES_NUM; i++){
        if(gGattClientEnv[i].occupied && (gGattClientEnv[i].client_if == client_if)){
            return &gGattClientEnv[i];
        }
    }
    return NULL;
}

ble_gattc_env_t *ble_gattc_get_client_by_addr(uint8_t *addr)
{
    for(uint32_t i = 0; i < BLE_ADAPTER_PROFILES_NUM; i++){
        if(gGattClientEnv[i].occupied && (memcmp(gGattClientEnv[i].connectAddr, addr, MAX_BDADDR_LEN)==0)){
            return &gGattClientEnv[i];
        }
    }
    return NULL;
}

ble_gattc_env_t *ble_gattc_get_client_by_conidx(uint8_t conidx)
{
    for(uint32_t i = 0; i < BLE_ADAPTER_PROFILES_NUM; i++){
        if(gGattClientEnv[i].occupied && (gGattClientEnv[i].conidx == conidx)){
            return &gGattClientEnv[i];
        }
    }
    return NULL;
}

ble_gattc_env_t *ble_gattc_get_client_by_service_if(int32_t client_db_if)
{
    for(uint32_t i = 0; i < BLE_ADAPTER_PROFILES_NUM; i++){
        if(gGattClientEnv[i].occupied && (gGattClientEnv[i].client_db_if == client_db_if)){
            return &gGattClientEnv[i];
        }
    }
    return NULL;
}

extern void ble_gattc_test(void);
void ble_gattc_scan_result(ble_bdaddr_t *bleAddr, int8_t rssi, uint8_t evt_type, uint8_t *adv_buf, uint8_t len)
{
    int8_t macAddr[TG_BDADDR_MAX_LEN] = {0};

    btAddressToMac((unsigned char *)macAddr, bleAddr->addr);
    for(uint32_t i = 0; i < BLE_ADAPTER_PROFILES_NUM; i++) {
        if(gGattClientEnv[i].occupied && gGattClientEnv[i].scanflag) {
            if(gGattClientEnv[i].gattc_cb.gattcScanResultCB) {
                LOG_I("%s gattcScanResultCB client_if:%d", __FUNCTION__, gGattClientEnv[i].client_if);
                gGattClientEnv[i].gattc_cb.gattcScanResultCB(macAddr, rssi, adv_buf, len);
            }
        }
    }

    //test----------------------------------
#ifdef TMALL_GATT_CLIENT_TEST
    static uint32_t init_flag = 0;
    if(!init_flag) {
        init_flag = 1;
        ble_gattc_test();
    }
#endif
    //--------------------------------
}

uint32_t ble_gattc_get_client_scan_num(void)
{
    uint32_t scanNum = 0;

    for(uint32_t i = 0; i < BLE_ADAPTER_PROFILES_NUM; i++) {
        if(gGattClientEnv[i].occupied && gGattClientEnv[i].scanflag) {
            scanNum++;
        }
    }

    return scanNum;
}

static TG_BT_gattcType gattServicesGetClientTypeByHandle(ble_gattc_env_t *gattc_env, uint16_t handle)
{
    for(uint8_t i = 0; i < gattc_env->gattc_service_num; i++) {
        if(gattc_env->gattc_service[i].handle == handle) {
            return gattc_env->gattc_service[i].type;
        }
    }

    return 0xFF;
}



void BleGattcInitCallback(uint16_t status)
{
   LOG_I("%s\n",__FUNCTION__);
}
/**
 * @brief Inform the ble connection result
 * @param[in] connId Connection ID
 * @param[in] bdAddr Peer bluetooth device addr
 * @param[in] event See BT_GATTC_EVENT_T enums
 * @return void
 */
void BleGattcLinkEventCallback(int32_t connId, const unsigned char* bdAddr, bes_gatt_link_event_e event)
{
    //not used.
}
/**
 * @brief Inform the ble connection parameters update result
 * @param[in] connId Connection ID
 * @param[in] interval Connection interval
 * @param[in] latency Connection latency
 * @param[in] supervisiontimeout Connection supervisiontimeout
 * @return void
 */
void BleGattcConnParaUpdateCallback(int32_t connId, int32_t interval, int32_t latency, int32_t supertimeout)
{

}
/**
 * @brief Get the services list
 * @param[in] gattServices Services list
 * @return void
 */
void BleGattcServiceDiscoveredCompleteCallback(int32_t connId, bes_gatt_status_e status)
{
    TG_BT_gattcServices gattc_service;
    uint32_t totalSize = 0;
    ble_gattc_env_t *gattc_env = NULL;

    LOG_I("%s connId:%d status:%d", __FUNCTION__, connId, status);

    gattc_env = ble_gattc_get_client_by_conidx(connId);
    if(!gattc_env){
        LOG_E("%s no client", __FUNCTION__);
        return;
    }
    totalSize = gattc_env->gattc_service_num*sizeof(TG_BT_gattcServiceElem);
    if(gattc_env->gattc_cb.gattcServiceDiscoveredCB)
    {
        gattc_service.connId = gattcTransferConnIdIf(gattc_env->conidx);
        gattc_service.count = gattc_env->gattc_service_num;
        gattc_service.pElem = NULL;
        if(gattc_env->gattc_service_num > 0) {
            gattc_service.pElem = (TG_BT_gattcServiceElem*)ke_malloc(totalSize, KE_MEM_KE_MSG);
            ASSERT_ERR(gattc_service.pElem != NULL);
            memcpy((uint8_t*)gattc_service.pElem, (uint8_t*)gattc_env->gattc_service, totalSize);
        }
        LOG_I("%s connId:%d gattcServiceDiscoveredCB", __FUNCTION__, connId);
        gattc_env->gattc_cb.gattcServiceDiscoveredCB(&gattc_service);
        if(gattc_service.pElem) {
            ke_free(gattc_service.pElem);
        }
    }
}

void ble_gattc_extract_svc_info(ble_gattc_env_t *gattc_env, bes_gatt_services_t* service)
{
    uint8_t uuid_tmp[ADAPTER_BLE_UUID_128_LEN];

    if(!gattc_env || !service) {
        LOG_E("%s param error", __FUNCTION__);
        return;
    }

    for(uint32_t idx = 0; idx < service->count; idx++) {
        gattc_env->gattc_service[gattc_env->gattc_service_num].id = gattc_env->gattc_service_num;
        gattc_env->gattc_service[gattc_env->gattc_service_num].handle = service->pElem[idx].handle;
        switch(service->pElem[idx].type) {
            case BES_GATT_DB_PRIMARY_SERVICE:
            case BES_GATT_DB_SECONDARY_SERVICE:
            case BES_GATT_DB_INCLUDED_SERVICE:
                gattc_env->gattc_service[gattc_env->gattc_service_num].type = service->pElem[idx].type;
                gattc_env->gattc_service[gattc_env->gattc_service_num].start_handle = service->pElem[idx].start_handle;
                gattc_env->gattc_service[gattc_env->gattc_service_num].end_handle = service->pElem[idx].end_handle;
                bt_bytes2hexstr_reverse(gattc_env->gattc_service[gattc_env->gattc_service_num].uuid, TG_BT_GATT_MAX_UUID_LEN,
                        service->pElem[idx].uuid, service->pElem[idx].uuid_len);
            break;
            case BES_GATT_DB_CHARACTERISTIC:
                uuid_tmp[0] = 0x03;
                uuid_tmp[1] = 0x28;
                gattc_env->gattc_service[gattc_env->gattc_service_num].type = TG_BT_GATT_DB_CHARACTERISTIC;
                gattc_env->gattc_service[gattc_env->gattc_service_num].properties =  service->pElem[idx].properties;
                bt_bytes2hexstr_reverse(gattc_env->gattc_service[gattc_env->gattc_service_num].uuid, TG_BT_GATT_MAX_UUID_LEN,
                            uuid_tmp, ADAPTER_BLE_UUID_16_LEN);
            break;
            case BES_GATT_DB_DESCRIPTOR:
                gattc_env->gattc_service[gattc_env->gattc_service_num].type = TG_BT_GATT_DB_DESCRIPTOR;
                gattc_env->gattc_service[gattc_env->gattc_service_num].properties =  service->pElem[idx].properties;
                bt_bytes2hexstr_reverse(gattc_env->gattc_service[gattc_env->gattc_service_num].uuid, TG_BT_GATT_MAX_UUID_LEN,
                            service->pElem[idx].uuid, service->pElem[idx].uuid_len);
            break;
            default:
                break;
        }
        gattc_env->gattc_service_num++;
    }
}

/**
 * @brief Get the services list . Service discover results are saved in gGattClientDb[i].svc_content
 * @param[in] gattServices Services list
 * @return void
 */
void BleGattcServiceDiscoveredCallback(int32_t connId, bes_gatt_services_t* gattServices)
{
    ble_gattc_env_t *gattc_env = NULL;

    LOG_I("%s connId:%d att count:%d", __FUNCTION__, connId, gattServices->count);

    gattc_env = ble_gattc_get_client_by_conidx(connId);
    if(!gattc_env){
        LOG_E("%s no client", __FUNCTION__);
        return;
    }

    ble_gattc_extract_svc_info(gattc_env, gattServices);
}

/**
 * @brief Inform character read result
 * @param[in] connId Connection id
 * @param[in] handle Attribute handle
 * @param[in] status Status
 * @param[in] data Att data read
 * @param[in] size Length of data in bytes
 * @return void
 */
void BleGattcCharacterReadCallback(int32_t connId, int32_t handle, bes_gatt_status_e status,
                                        const uint8_t* data, int32_t size)
{
    ble_gattc_env_t *gattc_env = NULL;

    LOG_I("%s connId:%d handle:%d status:%d", __FUNCTION__, connId, handle, status);

    gattc_env = ble_gattc_get_client_by_conidx(connId);
    if(!gattc_env){
        LOG_E("%s no client", __FUNCTION__);
        return;
    }
    if(gattc_env->gattc_cb.gattcCharacteristicReadCB)
    {
        gattc_env->gattc_cb.gattcCharacteristicReadCB(gattcTransferConnIdIf(connId), handle, status, data, size);
    }
}
/**
 * @brief Inform characteristic write result
 * @param[in] connId Connection id
 * @param[in] handle Attribute handle
 * @param[in] status Status
 * @return void
 */
void BleGattcCharacterWriteCallback(int32_t connId, uint16_t handle, bes_gatt_status_e status)
{
    ble_gattc_env_t *gattc_env = NULL;

    LOG_I("%s connId:%d handle:%d status:%d", __func__, connId, handle, status);
    gattc_env = ble_gattc_get_client_by_conidx(connId);
    if(!gattc_env){
        LOG_E("%s error, no client", __func__);
        return;
    }
#if 0
    TG_BT_gattcType type;
    type = gattServicesGetClientTypeByHandle(gattc_env, handle);
    switch(type) {
        case TG_BT_GATT_DB_CHARACTERISTIC:
            if(gattc_env->gattc_cb.gattcCharacteristicWriteCB) {
                gattc_env->gattc_cb.gattcCharacteristicWriteCB(gattcTransferConnIdIf(connId), handle, status);
            }
        break;
        case TG_BT_GATT_DB_DESCRIPTOR:
            if(gattc_env->gattc_cb.gattcCnDescriptorWriteCB) {
                gattc_env->gattc_cb.gattcCnDescriptorWriteCB(gattcTransferConnIdIf(connId), handle, status);
            }
        break;
        default:
            break;
    }
#else
    if(gattc_env->gattc_cb.gattcCharacteristicWriteCB) {
        gattc_env->gattc_cb.gattcCharacteristicWriteCB(gattcTransferConnIdIf(connId), handle, status);
    }
#endif
}
/**
 * @brief Inform descriptor read result
 * @param[in] connId Connection id
 * @param[in] handle Attribute handle
 * @param[in] status Status
 * @param[in] data Att data read
 * @param[in] size Length of data in bytes
 * @return void
 */
void BleGattcDescriptorReadCallback(int32_t connId, int32_t handle, bes_gatt_status_e status,
                                        const uint8_t* data, int32_t size)
{

}
/**
 * @brief Inform descriptor write result
 * @param[in] connId Connection id
 * @param[in] handle Attribute handle
 * @param[in] status Status
 * @return void
 */
void BleGattcDescriptorWriteCallback(int32_t connId, uint16_t handle, bes_gatt_status_e status)
{
    ble_gattc_env_t *gattc_env = NULL;

    LOG_I("%s connId:%d handle:%d status:%d", __func__, connId, handle, status);
    gattc_env = ble_gattc_get_client_by_conidx(connId);
    if(!gattc_env){
        LOG_E("%s error, no client", __func__);
        return;
    }

    if(gattc_env->gattc_cb.gattcCnDescriptorWriteCB) {
        gattc_env->gattc_cb.gattcCnDescriptorWriteCB(gattcTransferConnIdIf(connId), handle, status);
    }
}
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
void BleGattcCharacteChangedCallback(int32_t connId, uint32_t handle,
                                        const uint8_t* data, int32_t size, uint8_t isNotify)
{
    LOG_I("%s connId:%d handle:%d", __func__, connId, handle);
}

/**
 * @brief Enable characteristic notify callbac
 * @param[in] connId Connection id
 * @param[in] handle Attribute handle
 * @param[in] registered 1 is success
 * @return void
 */
void BleGattcNotificationEnableCallback(int32_t connId, int32_t handle, int32_t registered)
{
    LOG_I("%s connId:%d handle:%d registered:%d", __func__, connId, handle, registered);
}

void BleGattcRegisterCallback(void)
{
    bes_gattc_state_callback_t gattcCb = {0};

    gattcCb.initCb = BleGattcInitCallback;
    gattcCb.linkEventCb = BleGattcLinkEventCallback;
    gattcCb.connUpdateCb = BleGattcConnParaUpdateCallback;
    gattcCb.discoveryCmplCb = BleGattcServiceDiscoveredCompleteCallback;
    gattcCb.discoveryCb = BleGattcServiceDiscoveredCallback;
    gattcCb.charReadCb = BleGattcCharacterReadCallback;
    gattcCb.charWriteCb = BleGattcCharacterWriteCallback;
    gattcCb.descReadCb = BleGattcDescriptorReadCallback;
    gattcCb.descWriteCb = BleGattcDescriptorWriteCallback;
    gattcCb.charChangedNotifyCb = BleGattcCharacteChangedCallback;
    gattcCb.notifyEnableCb = BleGattcNotificationEnableCallback;
    btadapter_ble_register_client_callback(&gattcCb);
}

void tg_gattc_link_event_report_callback(int32_t conidx, uint8_t* addr, ble_evnet_type_e event)
{
    int8_t bdAddr[TG_BDADDR_MAX_LEN];
    ble_gattc_env_t *gattc_env = NULL;

    if (BLE_CONNECT_EVENT == event) {
        gattc_env = ble_gattc_get_client_by_addr(addr);
        if(!gattc_env) {
            LOG_I("%s conidx:%d connect gattc_env is null", __FUNCTION__, conidx);
            return;
        }
        gGattClientConnectedNum ++;
        gattc_env->conidx = conidx;
        gattc_env->connectedflag = true;
        if(gattc_env->gattc_cb.gattcGattEventCB) {
            btAddressToMac((unsigned char *)bdAddr, addr);
            gattc_env->gattc_cb.gattcGattEventCB(gattcTransferConnIdIf(conidx), bdAddr, TG_BT_GATT_EVENT_CONNECT);
            LOG_I("%s conidx:%d bdAddr:%s", __FUNCTION__, conidx, bdAddr);
        }
    } else if(BLE_DISCONNECT_EVENT == event) {
        gattc_env =ble_gattc_get_client_by_conidx(conidx);
        if(!gattc_env) {
            LOG_I("%s conidx:%d disconnect gattc_env is null", __FUNCTION__, conidx);
            return;
        }
        gGattClientConnectedNum --;
        gattc_env->conidx = 0;
        gattc_env->connectedflag = false;
        if(gattc_env->gattc_cb.gattcGattEventCB){
            btAddressToMac((unsigned char *)bdAddr, gattc_env->connectAddr);
            gattc_env->gattc_cb.gattcGattEventCB(gattcTransferConnIdIf(conidx), bdAddr, TG_BT_GATT_EVENT_DISCONNECT);
            LOG_I("%s conidx:%d", __FUNCTION__, conidx);
        }
    }
}

/**
 * @brief gatt client initial
 *        should callback gattcInitCB when finished
 * @param[in]: None.
 * @return: success client_if, error: -1
 */
int32_t tg_bt_initGattc(TG_BT_GATTC_CB_FUNC_T *callback)
{
    static bool init_flag = false;
    ble_gattc_env_t *gattc_env = NULL;

    LOG_I("%s", __FUNCTION__);
    if(!init_flag)
    {
        init_flag = true;
        gGattClientConnectedNum = 0;
        BleGattcRegisterCallback();
        tg_ble_adv_gattc_init(ble_gattc_scan_result);
        btadapter_ble_client_start_service();
        app_ble_link_event_callback_register(tg_gattc_link_event_report_callback);
    }
    gattc_env = ble_gattc_get_free_gattc_client();
    if(!gattc_env){
        LOG_E("%s no client", __FUNCTION__);
        return -1;
    }

    memcpy((uint8_t*)&gattc_env->gattc_cb, (uint8_t*)callback, sizeof(TG_BT_GATTC_CB_FUNC_T));
    if(gattc_env->gattc_cb.gattcInitedCB){
        gattc_env->gattc_cb.gattcInitedCB(gattc_env->client_if);
        LOG_I("%s client_if:%d", __FUNCTION__, gattc_env->client_if);
    }

    return gattc_env->client_if;
}

/**
 * @brief gatt client deinitial
 * @param[in]: None.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_deinitGattc(int32_t client_if)
{
    ble_gattc_env_t *gattc_env = NULL;
    LOG_I("%s client_if:%d", __FUNCTION__, client_if);

    gattc_env = ble_gattc_get_client_by_if(client_if);
    if(!gattc_env){
        LOG_E("%s no client", __FUNCTION__);
        return -1;
    }
    memset((uint8_t*)gattc_env, 0, sizeof(ble_gattc_env_t));

    return 0;
}


/**
 * @brief gatt client advertise broadcast packet data set
 * @param[in] data:  advertise data.
 * @param[in] len:  advertise data length.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattcSetAdvData(uint8_t *data, int len)
{
    LOG_I("%s set adv data len:%d", __FUNCTION__, len);
    return tg_ble_adv_gattc_set_adv_data(data, len, NULL, 0);
}

/**
 * @brief gatt client enable/disable advertise broadcast
 * @param[in] enable: 1 means enable; 0 means disable.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattcEnableAdv(int32_t client_if, bool enable)
{
    int32_t state = 0;
    ble_gattc_env_t *gattc_env = NULL;

    LOG_I("%s client_if:%d enable:%d", __FUNCTION__, client_if, enable);
    gattc_env = ble_gattc_get_client_by_if(client_if);
    if(!gattc_env){
        LOG_E("%s no client", __FUNCTION__);
        return -1;
    }

    if(enable) {
        state = tg_ble_adv_gattc_start_adv(BT_GATT_CLIENT_DEFAULT_ADV_INTERVAL, GAPM_ADV_NON_CONN);
    } else {
        state = tg_ble_adv_gattc_stop_adv();
    }

    if(gattc_env->gattc_cb.gattcAdvEnabledCB)
    {
        gattc_env->gattc_cb.gattcAdvEnabledCB(state);
    }

    return state;
}

/**
 * @brief gatt client start scan
 * @param[in] index: application index
 * @return: None
 */
int32_t tg_bt_startScan(int32_t index)
{
    BLE_SCAN_PARAM_T param;
    ble_gattc_env_t *gattc_env = NULL;

    LOG_I("%s index:%d", __FUNCTION__, index);

    //for compatibility single call
    if(index == -1) {
        index = single_client_if_id;
    }

    gattc_env = ble_gattc_get_client_by_if(index);
    if(!gattc_env){
        LOG_E("%s no client", __FUNCTION__);
        return -1;
    }

    gattc_env->scanflag = true;
    if(ble_gattc_get_client_scan_num() > 1) {
        return 0;
    }

    param.scanType = BT_GATT_CLIENT_DEFAULT_SCAN_TYPE;
    param.scanWindow = BT_GATT_CLIENT_DEFAULT_SCAN_WINDOW;
    param.scanInterval = BT_GATT_CLIENT_DEFAULT_SCAN_INTERVAL;

    return tg_ble_adv_gattc_start_scan(BT_GATT_CLIENT_DEFAULT_SCAN_FILTPOLICY, &param);
}

/**
 * @brief gatt client stop scan
 * @param[in] None
 * @return: None
 */
int32_t tg_bt_stopScan(int32_t index)
{
    ble_gattc_env_t *gattc_env = NULL;

    LOG_I("%s index:%d", __FUNCTION__, index);
    //for compatibility single call
    if(index == -1) {
        index = single_client_if_id;
    }

    gattc_env = ble_gattc_get_client_by_if(index);
    if(!gattc_env){
        LOG_E("%s no client", __FUNCTION__);
        return -1;
    }
    gattc_env->scanflag = false;
    if(ble_gattc_get_client_scan_num() > 0) {
        return 0;
    }

    return tg_ble_adv_gattc_stop_scan();
}

/**
 * @brief gatt client setup connection
 * @param[in] target device address
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattcConnect(int32_t client_if, const char *bdAddr)
{
    ble_bdaddr_t  ble_bdaddr={ 0 };
    ble_gattc_env_t *gattc_env = NULL;
    LOG_I("%s client_if:%d", __FUNCTION__, client_if);

    if(gGattClientConnectedNum > BLE_CONNECTION_MAX) {
        LOG_I("%s connect error: there is no free connect, connect num:%d", __FUNCTION__, gGattClientConnectedNum);
    }

    gattc_env = ble_gattc_get_client_by_if(client_if);
    if(!gattc_env){
        LOG_E("%s no client", __FUNCTION__);
        return -1;
    }
    gattc_env->conidx = 0;
    gattc_env->connectedflag = 0;
    ble_bdaddr.addr_type = 0;
    macToBtAddress((uint8_t*)bdAddr, ble_bdaddr.addr);
    memcpy(gattc_env->connectAddr, ble_bdaddr.addr, GAP_BD_ADDR_LEN);
    app_ble_start_connect(&ble_bdaddr);

    return 0;
}

/**
 * @brief gatt client remove connection
 * @param[in] connection id
 * @param[in] target device address
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattcDisconnect(int32_t connId, const char *bdAddr)
{

    LOG_I("%s connId:%d", __FUNCTION__, gattcTransferConnIdStack(connId));

    app_ble_start_disconnect(gattcTransferConnIdStack(connId));
    return 0;
}

/**
 * @brief gatt client discovery gatt services
 * @param[in] connection id
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattcDiscoverServices(int32_t connId)
{
    bes_gatt_status_e status;
    ble_gattc_env_t *gattc_env = NULL;

    LOG_I("%s connId:%d", __FUNCTION__, gattcTransferConnIdStack(connId));

    gattc_env = ble_gattc_get_client_by_conidx(gattcTransferConnIdStack(connId));
    if(!gattc_env){
        LOG_E("%s no client", __FUNCTION__);
        return -1;
    }
    gattc_env->gattc_service_num = 0;
    status = btadapter_ble_client_start_service_discovery(gattcTransferConnIdStack(connId), BES_GATT_DISCOVER_SVC_PRIMARY_ALL,
                                                    NULL, 0, 0x0001, 0xffff);

    if(status == GAP_ERR_NO_ERROR) {
        return 0;
    } else {
        return -1;
    }
}

/**
 * @brief gatt client read characteristic
 * @param[in] connection id
 * @param[in] attribute handle
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattcReadCharacteristic(int32_t connId, uint32_t handle)
{
    uint8_t conidx = gattcTransferConnIdStack(connId);

    LOG_I("%s connId:%d handle:%d", __FUNCTION__, connId, handle);
    btadapter_ble_client_read_char(connId, handle);
    return 0;
}

/**
 * @brief gatt client write characteristic
 * @param[in] connection id
 * @param[in] attribute handle
 * @param[in] writeType 1-write no response;2-write with response;3-write prepare
 * @param[in] payload
 * @param[in] payload length in bytes
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattcWriteCharacteristic(int32_t connId, uint32_t handle
        , TG_BT_gattcWriteType writeType, const uint8_t *data, int32_t size)
{
    uint8_t conidx = gattcTransferConnIdStack(connId);

    LOG_I("%s connId:%d handle:%d writeType:%d", __FUNCTION__, connId, handle, writeType);

    btadapter_ble_client_write(connId, handle, writeType, data, size);
    return 0;
}

/**
 * @brief gatt client write descriptor
 * @param[in] connection id
 * @param[in] attribute handle
 * @param[in] writeType 1-write no response;2-write with response;3-write prepare
 * @param[in] payload
 * @param[in] payload length in bytes
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattcWriteDescriptor(int32_t connId, uint32_t handle
        , TG_BT_gattcWriteType writeType, const uint8_t *data, int32_t size)
{
    uint8_t conidx = gattcTransferConnIdStack(connId);

    LOG_I("%s connId:%d handle:%d writeType:%d", __FUNCTION__, connId, handle, writeType);

    btadapter_ble_client_write(connId, handle, writeType, data, size);
    return 0;
}

/**
 * @brief gatt client enable characteristic notify
 * @param[in] target device address
 * @param[in] attribute handle
 * @param[in] enable or not
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattcSetCharacteristicNotification(char *bdAddr, uint32_t handle, bool enable)
{
    LOG_E("%s addr:%s handle:%d enable:%d", __FUNCTION__, bdAddr, handle, enable);
    return 0;
}

/**
 * @brief gatt client change mtu
 * @param[in] connection id
 * @param[in] prefer mtu
 * @param[in] enable or not
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattcChangeMTU(int32_t connId, int32_t mtu)
{
    uint8_t conidx = gattcTransferConnIdStack(connId);

    LOG_I("%s conidx:%d, mtu:%d", __func__, conidx, mtu);
    btadapter_ble_client_mtu_configure(conidx, mtu);
    return 0;
}

/**
 * @brief gatt adv state
 * @param[in] None.
 * @return: on/off
 */
uint8_t tg_bt_get_gatt_scan_enable(void)
{
    LOG_I("%s", __FUNCTION__);
    return ble_gattc_get_client_scan_num();
}


#ifdef TMALL_GATT_CLIENT_TEST

/***************************************************************************
 * 测试gatt client的多端调用:
 * 创建客户端，打开scan，搜索到指定的地址的广播，就触发连接，然后读取所有的service
 * 先单连接测试，然后测试两条链路
 * 使用天猫精灵测试读写数据，连接读写秤
 *
 * 注册两个client，在接收到对应地址广播的时候，进行创建连接，连接上了读取service
 *
 *
 ***************************************************************************/
#include "cmsis_os.h"


int32_t client1;
int32_t client1_connid;
uint8_t server1_addr[] = "DC:23:4D:E8:8F:47";

int32_t client2;
int32_t client2_connid;
uint8_t server2_addr[] = "DC:23:4D:E8:8F:48";

int32_t client3;
int32_t client3_connid;
uint8_t server3_addr[] = "DC:23:4D:E8:8F:45";

int32_t client4;
int32_t client4_connid;
uint8_t server4_addr[] = "DC:23:4D:E8:8F:46";

uint32_t connect_state = 0;
int32_t read_service_connid = 0;

static void tg_ble_gattc_test_read_service_timer_cb(void const *n);
osTimerDef (BLE_GATTC_TEST_READ_SERVICE_TIMEOUT, tg_ble_gattc_test_read_service_timer_cb);
osTimerId   ble_gattc_test_read_service_timer = NULL;


static void tg_ble_gattc_test_read_service_timer_cb(void const *n)
{
    tg_bt_gattcDiscoverServices(read_service_connid);
}


void tg_ble_gattc_test_inited_cb(int32_t client_if)
{
    LOG_I("%s client_if:%d", __FUNCTION__, client_if);
}

void tg_ble_gattc_test_scan_result_cb(const int8_t *bdAddr, int32_t rssi, uint8_t *pData, int32_t size)
{
    if(memcmp(bdAddr, server1_addr, TG_BDADDR_MAX_LEN) == 0 && (connect_state == 0)) {
        LOG_I("%s connid:%d bdAddr:%s", __FUNCTION__, client1, bdAddr);
        tg_bt_gattcConnect(client1, bdAddr);
        connect_state++;  //1
    }

    if(memcmp(bdAddr, server2_addr, TG_BDADDR_MAX_LEN) == 0 && (connect_state == 2)) {
        LOG_I("%s connid:%d bdAddr:%s", __FUNCTION__, client2, bdAddr);
        tg_bt_gattcConnect(client2, bdAddr);
        connect_state++; //3
    }

    // if(memcmp(bdAddr, server3_addr, TG_BDADDR_MAX_LEN) == 0 && (connect_state == 4)) {
    //     LOG_I("%s connid:%d bdAddr:%s", __FUNCTION__, client3, bdAddr);
    //     tg_bt_gattcConnect(client3, bdAddr);
    //     connect_state++; //5
    // }

    // if(memcmp(bdAddr, server4_addr, TG_BDADDR_MAX_LEN) == 0 && (connect_state == 6)) {
    //     LOG_I("%s connid:%d bdAddr:%s", __FUNCTION__, client4, bdAddr);
    //     tg_bt_gattcConnect(client4, bdAddr);
    //     connect_state++; //7
    // }
}

void tg_ble_gattc_test_event_cb(int32_t connId, const int8_t *bdAddr, TG_BT_gattcEvent event)
{
    LOG_I("%s connId:%d event:%d bdAddr:%s", __FUNCTION__, connId, event, bdAddr);

    connect_state++;

    if (memcmp(bdAddr, server1_addr, TG_BDADDR_MAX_LEN) == 0) {
        client1_connid = connId;
    } else if(memcmp(bdAddr, server2_addr, TG_BDADDR_MAX_LEN) == 0) {
        client2_connid = connId;
    }
    // else if(memcmp(bdAddr, server3_addr, TG_BDADDR_MAX_LEN) == 0) {
    //     client3_connid = connId;
    // } else if(memcmp(bdAddr, server4_addr, TG_BDADDR_MAX_LEN) == 0) {
    //     client4_connid = connId;
    // }

    //read service
    if(connId == client2_connid) {
        read_service_connid = client1_connid;
        osTimerStop(ble_gattc_test_read_service_timer);
        osTimerStart(ble_gattc_test_read_service_timer, 3000);
    }

}

void tg_ble_gattc_test_service_discovered_cb(TG_BT_gattcServices *gattServices)
{
    LOG_I("%s connId:%d count:%d", __FUNCTION__, gattServices->connId, gattServices->count);

    //read service
    if(gattServices->connId < client2_connid) {
        read_service_connid ++;
        osTimerStop(ble_gattc_test_read_service_timer);
        osTimerStart(ble_gattc_test_read_service_timer, 3000);
    }
}

void tg_ble_gattc_test_character_read_cb(int32_t connId, uint32_t handle, int32_t status,
                                           const uint8_t *data, int32_t size)
{

}

void tg_ble_gattc_test_charater_write_cb(int32_t connId, uint32_t handle, int32_t status)
{

}

void tg_ble_gattc_test_descriptor_read_cb(int32_t connId, uint32_t handle, int32_t status,
                                       const uint8_t *data, int32_t size)
{

}

void tg_ble_gattc_test_descriptor_write_cb(int32_t connId, uint32_t handle, int32_t status)
{

}

void tg_ble_gattc_test_character_changed_cb(int32_t connId, uint32_t handle, const int8_t *bdaddr,
                                              const uint8_t *data, int32_t size, uint8_t isNotify)
{

}

void tg_ble_gattc_test_charater_notify_cb(int32_t connId, uint32_t handle, int32_t registered)
{

}

void tg_ble_gattc_test_adv_enabled_cb(int32_t status)
{
    LOG_I("%s status:%d", __FUNCTION__, status);
}

TG_BT_GATTC_CB_FUNC_T client1_cb =
{
    .gattcInitedCB = tg_ble_gattc_test_inited_cb,
    .gattcScanResultCB = tg_ble_gattc_test_scan_result_cb,
    .gattcGattEventCB = tg_ble_gattc_test_event_cb,
    .gattcServiceDiscoveredCB = tg_ble_gattc_test_service_discovered_cb,
    .gattcCharacteristicReadCB = tg_ble_gattc_test_character_read_cb,
    .gattcCharacteristicWriteCB = tg_ble_gattc_test_charater_write_cb,
    .gattcDescriptorReadCB = tg_ble_gattc_test_descriptor_read_cb,
    .gattcCnDescriptorWriteCB = tg_ble_gattc_test_descriptor_write_cb,
    .gattcCCharacteristicChangedCB = tg_ble_gattc_test_character_changed_cb,
    .gattcSetCharacteristicNotiCB = tg_ble_gattc_test_charater_notify_cb,
    .gattcAdvEnabledCB = tg_ble_gattc_test_adv_enabled_cb
};


void ble_gattc_test(void)
{
    int32_t status;
    client1 = tg_bt_initGattc(&client1_cb);
    client2 = tg_bt_initGattc(&client1_cb);
    client3 = tg_bt_initGattc(&client1_cb);
    client4 = tg_bt_initGattc(&client1_cb);

    status = tg_bt_startScan(client1);
    if(status == -1) {
        LOG_I("%s start scan error, status:%d", __FUNCTION__, status);
    }

    status = tg_bt_startScan(client2);
    if(status == -1) {
        LOG_I("%s start scan error, status:%d", __FUNCTION__, status);
    }

    status = tg_bt_startScan(client3);
    if(status == -1) {
        LOG_I("%s start scan error, status:%d", __FUNCTION__, status);
    }

    status = tg_bt_startScan(client4);
    if(status == -1) {
        LOG_I("%s start scan error, status:%d", __FUNCTION__, status);
    }

    if(NULL == ble_gattc_test_read_service_timer)
    {
        ble_gattc_test_read_service_timer = osTimerCreate(osTimer(BLE_GATTC_TEST_READ_SERVICE_TIMEOUT), osTimerOnce, NULL);
    }
}




#endif
