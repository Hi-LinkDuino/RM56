#include "string.h"
#include "plat_types.h"
#include "hal_trace.h"
#include "bleadapter_utils.h"
#include "bes_gatts_adapter.h"
#include "ohos_bt_gatt_server.h"
#include "harmony_utils.h"

static BtGattServerCallbacks * g_gattServ_cb = NULL;

unsigned char  BesGattServerSwapProp(int prop)
{
    unsigned char bes_prop = 0;

    if(prop & OHOS_GATT_CHARACTER_PROPERTY_BIT_BROADCAST)
        bes_prop|= ADAPTER_BLE_PROP_BROADCAST;

    if(prop & OHOS_GATT_CHARACTER_PROPERTY_BIT_READ)
        bes_prop|= ADAPTER_BLE_PROP_READ;
    
    if(prop & OHOS_GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP)
        bes_prop|= ADAPTER_BLE_PROP_WRITE_NO_RSP;    

    if(prop & OHOS_GATT_CHARACTER_PROPERTY_BIT_WRITE)
        bes_prop|= ADAPTER_BLE_PROP_WRITE;

    if(prop & OHOS_GATT_CHARACTER_PROPERTY_BIT_NOTIFY)
        bes_prop|= ADAPTER_BLE_PROP_NOTIFY;

    if(prop & OHOS_GATT_CHARACTER_PROPERTY_BIT_INDICATE)
        bes_prop|= ADAPTER_BLE_PROP_INDICATE;

    if(prop & OHOS_GATT_CHARACTER_PROPERTY_BIT_SIGNED_WRITE)
        bes_prop|= ADAPTER_BLE_PROP_WRITE_SIGNED;

    return bes_prop;
}

unsigned char  BesGattServerSwapPerm(unsigned char perm)
{
    unsigned char bes_perm = 0;

    if(perm & OHOS_GATT_PERMISSION_READ)
        bes_perm |= ADAPTER_BLE_PERM_READ;

    if(perm & OHOS_GATT_PERMISSION_READ_ENCRYPTED)
        bes_perm |= ADAPTER_BLE_PERM_READ_ENCRYPTED;

    if(perm & OHOS_GATT_PERMISSION_READ_ENCRYPTED_MITM)
        bes_perm |= ADAPTER_BLE_PERM_READ_ENCRYPTED_MITM;

    if(perm & OHOS_GATT_PERMISSION_WRITE)
        bes_perm |= ADAPTER_BLE_PERM_WRITE;

    if(perm & OHOS_GATT_PERMISSION_WRITE_ENCRYPTED)
        bes_perm |= ADAPTER_BLE_PERM_WRITE_ENCRYPTED;

    if(perm & OHOS_GATT_PERMISSION_WRITE_ENCRYPTED_MITM)
        bes_perm |= ADAPTER_BLE_PERM_WRITE_ENCRYPTED_MITM;

    return bes_perm;
}

/**
 * @brief Inform the gatt server init result
 * @param[in] serverIf Indicates id of server interface
 * @return void
 */
static void BesGattServerInitCallback(int32_t serverIf)
{
    BTA_TRACE(1, "%s %d", __func__, serverIf);
}

static void BesGattServerConnCreatedCallback(int32_t serverIf, 
                                                                            uint8_t* addr,
                                                                            int connId)
{  
    BdAddr ble_addr;

    memcpy(&ble_addr, addr, OHOS_BD_ADDR_LEN);
    
    if(g_gattServ_cb){
        g_gattServ_cb->connectServerCb(connId, 
                                                                serverIf,
                                                                &ble_addr);
    }
    BTA_TRACE(1, "%s %d %d", __func__, serverIf, connId);
}

static void BesGattServerDisconnectCallback(int32_t serverIf, uint8_t* addr, int connId)
{
    BdAddr ble_addr;

    memcpy(&ble_addr, addr, OHOS_BD_ADDR_LEN);

    if(g_gattServ_cb){
        g_gattServ_cb->disconnectServerCb(connId,
                                          serverIf,
                                          &ble_addr);
    }
    BTA_TRACE(1, "%s %d %d", __func__, serverIf, connId);
}

/**
 * @brief Callback invoked in response to create_service
 * @param[in] serverIf Service parameter
 * @param[in] srvc_handle Service parameter
 * @return void
 */
static void BesGattServerAddServiceCallback(int32_t serverIf, int32_t srvc_handle)
{
    BTA_TRACE(1, "%s %d", __func__, serverIf);
}

/**
 * @brief Callback invoked when a characteristic has been added to a service
 * @param[in] bt_gatts_add_char Characteristic parameter(server if, uuid ,
 *                                      service handle and characteristic handle)
 * @return void
 */
static void BesGattServerAddCharCallback(bes_gatts_add_char_para_t *bt_gatts_add_char )
{
    BTA_TRACE(1, "%s %d", __func__, bt_gatts_add_char->server_if);
}

/**
 * @brief Callback invoked when a descriptor has been added to a characteristic
 * @param[in] bt_gatts_add_desc Descriptor parameter(server if, uuid ,
 *                                      service handle and descriptor handle)
 * @return void
 */
static void BesGattServerAddDescCallback(bes_gatts_add_desc_para_t *bt_gatts_add_desc)
{
    BTA_TRACE(1, "%s %d hd%x", __func__, bt_gatts_add_desc->server_if, bt_gatts_add_desc->srvc_handle);
}

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
 * @return void
 */
static uint16_t BesGattServerReqWriteCallback(int32_t server_if, 
                                                                    int32_t conn_id,
                                                                    uint16_t task_id,
                                                                    int32_t handle, 
                                                                    int32_t offset, 
                                                                    int32_t length, 
                                                                    uint8_t * data)
{
    uint8_t * ble_addr;
    BtReqWriteCbPara writeCbPara;
    bes_adapter_ble_service_db_t* service_db;

    ble_addr = btadapter_ble_get_addr_by_conidx(conn_id);

    writeCbPara.connId = conn_id;
    writeCbPara.bdAddr = (BdAddr*)ble_addr;
    writeCbPara.attrHandle = handle;
    writeCbPara.isPrep = false;
    writeCbPara.needRsp = true;
    writeCbPara.offset = offset;
    writeCbPara.length = length;
    writeCbPara.value = data;
    if(g_gattServ_cb){
        g_gattServ_cb->requestWriteCb(writeCbPara);
    }

    service_db = btadapter_ble_service_get_service_by_if(server_if);
    btadapter_ble_service_send_gatt_write_response(conn_id,
                                                                                        service_db,
                                                                                        BES_ATT_ERR_NO_ERROR);
    
    BTA_TRACE(1, "%s %d hd:%x", __func__, server_if, handle); 
    return BES_BLE_ERR_NO_ERROR;
}

void BesGattServerNTFCallback(int32_t server_if, 
                                                                    int32_t conn_id,
                                                                    uint16_t task_id,
                                                                    uint16_t status)
{
    uint8_t * ble_addr;
    BtReqWriteCbPara writeCbPara;

    ble_addr = btadapter_ble_get_addr_by_conidx(conn_id);
    BTA_TRACE(1, "%s %d conn_id:%d", __func__, server_if, conn_id);
}


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
 static void BesGattServerReqReadCallback(int32_t server_if, 
                                                                    int32_t conn_id,
                                                                    uint16_t task_id,
                                                                    int32_t handle, 
                                                                    int32_t offset)
{
    uint8_t * ble_addr;
    BtReqReadCbPara readCbPara;
    
    ble_addr = btadapter_ble_get_addr_by_conidx(conn_id);
    
    readCbPara.connId = conn_id;
    readCbPara.bdAddr = (BdAddr*)ble_addr;
    readCbPara.attrHandle = handle;
    readCbPara.offset = offset;
    readCbPara.transId = 1;
    readCbPara.isLong = false;
    
    if(g_gattServ_cb){
        g_gattServ_cb->requestReadCb(readCbPara);
    }
    BTA_TRACE(2, "%s %d hd:%x", __func__, server_if, handle);
}

/**
 * @brief Callback invoked when GATT Service is started
 * @return void
 */
static void BesGattServerStartServerCallback(int32_t server_if)
{
    BTA_TRACE(1, "%s %d", __func__, server_if); 
}

/**
 * @brief Callback invoked when GATT Service is stop
 * @return void
 */
static void BesGattServerStopServerCallback(int32_t server_if)
{
    BTA_TRACE(1, "%s %d", __func__, server_if); 
}

/**
 * @brief Callback invoked when GATT Service is delete
 * @return void
 */
static void BesGattServerDeleteServerCallback(int32_t server_if)
{
    BTA_TRACE(1, "%s %d", __func__, server_if); 
}

static void BesGattServerMtuExchangeCallback(uint8_t conidx, uint16_t mtu)
{
    if(g_gattServ_cb){
        g_gattServ_cb->mtuChangeCb(conidx, mtu);
    }
    BTA_TRACE(1, "%s conidx:%d mtu:%d", __func__, conidx, mtu); 
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

int BleGattsRegister(BtUuid appUuid)
{
    int32_t server_if = 0;
    int status  = BT_STS_SUCCESS;
    bes_adapter_ble_service_db_t* service_db = NULL;
    
    server_if = bes_gatt_server_add_service(-1, 
                                                        (uint8_t*)appUuid.uuid,
                                                        appUuid.uuidLen,
                                                        true,
                                                        8,
                                                        false);

    if(server_if >= 0){
        status = BT_STS_SUCCESS;
    }else{
        status = -1;
    }

    service_db = btadapter_ble_service_get_service_by_if(server_if);
    if(!service_db){
        status = -1;
    }

    if(g_gattServ_cb){
        g_gattServ_cb->registerServerCb(status ,
                                                             server_if,
                                                             &appUuid);
    }

    BTA_TRACE(1, "%s", __func__);
    return bes_status_to_ohos(status);
}

int BleGattsUnRegister(int serverId)
{  
    bes_gatt_server_unregister_service(serverId);
    BTA_TRACE(1, "%s", __func__);
    return OHOS_BT_STATUS_SUCCESS;
}

int BleGattsAddService(int serverId, BtUuid srvcUuid, bool isPrimary, int number)
{
    int status = BT_STS_SUCCESS;
    bes_adapter_attm_item_t att_item;
    bes_adapter_ble_service_db_t* service_db = btadapter_ble_service_get_service_by_if(serverId);

    if(service_db){
        memset(&att_item, 0 ,sizeof(att_item));
        service_db->nattb = number;
        service_db->is_primary = isPrimary;
        memcpy(att_item.uuid, srvcUuid.uuid, srvcUuid.uuidLen);
        att_item.uuid_len = srvcUuid.uuidLen;
        att_item.ext_perm = 0;
        att_item.prop = ADAPTER_BLE_PROP_READ;
        att_item.perm = 0;
        bes_gatt_server_update_att_num(serverId, number);
        status = bes_gatt_server_add_att_item(serverId,
                                                                        service_db->handle_offset ++,
                                                                        &att_item);        
        if(g_gattServ_cb){
               g_gattServ_cb->serviceAddCb(status ,
                                                               serverId,
                                                               &srvcUuid, 
                                                               service_db->start_handle);           
        }
    }else
        status = -1;

    return bes_status_to_ohos(status);
}
                       
int BleGattsAddCharacteristic(int serverId, int srvcHandle, BtUuid characUuid,
                                                                int properties, int permissions)

{
    bes_adapter_attm_item_t att_item;
    int status = BT_STS_SUCCESS;
    int char_handle = 0;
    bes_adapter_ble_service_db_t* service_db = btadapter_ble_service_get_service_by_if(serverId);

    if(service_db){
        memset(&att_item, 0 ,sizeof(att_item));
        memcpy(att_item.uuid, characUuid.uuid, characUuid.uuidLen);
        att_item.uuid_len = characUuid.uuidLen;
        if(att_item.uuid_len == OHOS_BLE_UUID_MAX_LEN)
            att_item.ext_perm = 256;
        else
            att_item.ext_perm = 0;
        att_item.prop = BesGattServerSwapProp(properties);
        att_item.perm = BesGattServerSwapPerm(permissions);
        char_handle = service_db->start_handle + service_db->handle_offset;
        status = bes_gatt_server_add_att_item(serverId,
                                                            service_db->handle_offset ++,
                                                            &att_item);
    }
                
     if(g_gattServ_cb){
           g_gattServ_cb->characteristicAddCb(status,
                                                                    serverId,
                                                                    &characUuid,
                                                                    srvcHandle,
                                                                    char_handle);
    }
     
    return bes_status_to_ohos(status);
}
                        
int BleGattsAddDescriptor(int serverId, int srvcHandle, BtUuid descUuid, int permissions)
{
    bes_adapter_attm_item_t att_item;
    int status = BT_STS_SUCCESS;
    int descr_handle = 0;
    bes_adapter_ble_service_db_t* service_db = btadapter_ble_service_get_service_by_if(serverId);
    
    if(service_db){
        memset(&att_item, 0 ,sizeof(att_item));
        memcpy(att_item.uuid, descUuid.uuid, descUuid.uuidLen);
        att_item.uuid_len = descUuid.uuidLen;
        if(att_item.uuid_len == OHOS_BLE_UUID_MAX_LEN)
            att_item.ext_perm = 512;
        else
            att_item.ext_perm = 0;
        att_item.prop = 0;
        att_item.perm = BesGattServerSwapPerm(permissions);
        descr_handle = service_db->start_handle + service_db->handle_offset;
        status = bes_gatt_server_add_att_item(serverId,
                                                                        service_db->handle_offset ++,
                                                                        &att_item);
    }
                
    if(g_gattServ_cb){
           g_gattServ_cb->descriptorAddCb(status,
                                                                serverId,
                                                                &descUuid,
                                                                srvcHandle,
                                                                descr_handle);
    }

    return bes_status_to_ohos(status);
}
                
int BleGattsStartService(int serverId, int srvcHandle)
{
    int status = BT_STS_SUCCESS;
    bes_adapter_ble_service_db_t* service_db = btadapter_ble_service_get_service_by_if(serverId);

    if(service_db){
        status = bes_gatt_server_start_service(serverId);
        BTA_TRACE(1, "%s start_handle:%d handle_num:%d user_handle_num:%d", __func__, service_db->start_handle, service_db->handle_offset, service_db->nattb);
    }else
        status = -1;
    
    if(g_gattServ_cb){
           g_gattServ_cb->serviceStartCb(status,
                                                            serverId,
                                                            srvcHandle);
    }
     
    return bes_status_to_ohos(status);
}

int BleGattsStopService(int serverId, int srvcHandle)
{
    int status = OHOS_BT_STATUS_FAIL;
    /*
        service can't stop now
    */
    if(g_gattServ_cb){
           g_gattServ_cb->serviceStopCb(status,
                                                            serverId,
                                                            srvcHandle);
    }

    return status;
}

int BleGattsDeleteService(int serverId, int srvcHandle)
{
    int status = OHOS_BT_STATUS_FAIL;
    /*
        service can't stop now
    */
    if(g_gattServ_cb){
           g_gattServ_cb->serviceDeleteCb(status,
                                                                serverId,
                                                                srvcHandle);
    }

    return status;    
}

int BleGattsSendIndication(int serverId, GattsSendIndParam *param)
{
    if (param == NULL){
        return -1;
    }

    int status = BT_STS_SUCCESS;
    bes_adapter_ble_service_db_t* service_db = btadapter_ble_service_get_service_by_if(serverId);

    if(service_db){
        status = bes_gatt_server_send_indication(serverId,
                                                                                param->connectId,
                                                                                param->attrHandle,
                                                                                param->confirm,
                                                                                (int8_t*)param->value,
                                                                                param->valueLen);
    }else
        status = BT_STS_FAILED;

    if(g_gattServ_cb){
              g_gattServ_cb->indicationSentCb(param->connectId,
                                                                               status);
       }

    return bes_status_to_ohos(status);
}
                
int BleGattsSendResponse(int serverId, GattsSendRspParam *param)
{
    if (param == NULL){
        return -1;
    }

    int status = BT_STS_SUCCESS;
    
    bes_adapter_ble_service_db_t* service_db = btadapter_ble_service_get_service_by_if(serverId);

    if(service_db){
        status = btadapter_ble_service_send_gatt_get_response(service_db,
                                                                                                        param->connectId,
                                                                                                        param->attrHandle,
                                                                                                        (uint8_t*)param->value,
                                                                                                        param->valueLen,
                                                                                                        param->status);
    }else
        status = -1;

    if(g_gattServ_cb){
        g_gattServ_cb->responseConfirmationCb(status,
                                                                        param->attrHandle);
    }

    return bes_status_to_ohos(status);
}
         
int BleGattsRegisterCallbacks(BtGattServerCallbacks *func)
{
    g_gattServ_cb = func;
    BesGattServerRegisterCallback();
    return OHOS_BT_STATUS_SUCCESS;
}

