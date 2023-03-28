#include "string.h"
#include "plat_types.h"
#include "bt_gatt_types.h"
#include "bt_gatt.h"
#include "hal_trace.h"
#include "bleadapter_utils.h"
#include "bes_gatts_adapter.h"

static btgatt_callbacks_t * g_btgatt_cb = NULL;

#define UUID_LEN(x) (x->type/8)
#define UUID_DATA(x) (x->value.u128.data) 

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
    bt_bdaddr_t ble_addr;

    if(addr){
        memcpy(ble_addr.address, addr, 6);
    }
    if(g_btgatt_cb){
        g_btgatt_cb->server->connection_cb(connId, 
                                                                    serverIf,
                                                                    1,
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
    bt_bdaddr_t bta;

    ble_addr = btadapter_ble_get_addr_by_conidx(conn_id);
    if(ble_addr){
        memcpy(bta.address, ble_addr, 6);
    }
    if(g_btgatt_cb){
        g_btgatt_cb->server->request_write_cb(conn_id,
                                                                      0,
                                                                      &bta,
                                                                      handle,
                                                                      offset,
                                                                      length,
                                                                      true,
                                                                      true,
                                                                      data);
    }
    BTA_TRACE(1, "%s %d hd:%x", __func__, server_if, handle); 
    return BES_BLE_ERR_NO_ERROR;
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
    bt_bdaddr_t bta;
    
    ble_addr = btadapter_ble_get_addr_by_conidx(conn_id);
    if(ble_addr){
        memcpy(bta.address, ble_addr, 6);
    }                                   
    if(g_btgatt_cb){
        g_btgatt_cb->server->request_read_cb(conn_id,
                                                                        0,
                                                                       &bta,
                                                                       handle,
                                                                       offset,
                                                                       false);
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
    if(g_btgatt_cb){
        g_btgatt_cb->server->mtu_changed_cb(conidx, mtu);
    }
    BTA_TRACE(1, "%s conidx:%d mtu:%d", __func__, conidx, mtu); 
}


static void BesGattServerRegisterCallback(void)
{
    bes_gatts_state_callback_t gattsCb;

    gattsCb.connCreateCb = BesGattServerConnCreatedCallback;
    gattsCb.addCharCb = BesGattServerAddCharCallback;
    gattsCb.addDescCb = BesGattServerAddDescCallback;
    gattsCb.addServiceCb = BesGattServerAddServiceCallback;
    gattsCb.deleteServerCb = BesGattServerDeleteServerCallback;
    gattsCb.initCb = BesGattServerInitCallback;
    gattsCb.reqReadCb = BesGattServerReqReadCallback;
    gattsCb.reqWriteCb = BesGattServerReqWriteCallback;
    gattsCb.startServerCb = BesGattServerStartServerCallback;
    gattsCb.stopServerCb = BesGattServerStopServerCallback;
    gattsCb.mtuExchangedCb = BesGattServerMtuExchangeCallback;
    btadapter_ble_register_server_callback(&gattsCb);
}

static wh_bt_status_t register_server(bt_uuid_t *uuid)
{
    int32_t server_if = 0;
    int status  = 0;
    bes_adapter_ble_service_db_t* service_db = NULL;
    
    server_if = bes_gatt_server_add_service(-1, 
                                                        UUID_DATA(uuid),
                                                        UUID_LEN(uuid),
                                                        true,
                                                        0,
                                                        false);

    if(server_if >= 0){
        status = 0;
    }else{
        status = -1;
    }

    service_db = btadapter_ble_service_get_service_by_if(server_if);
    if(!service_db){
        status = -1;
    }

    if(g_btgatt_cb){
        g_btgatt_cb->server->register_server_cb(status ,
                                                                     server_if,
                                                                     uuid);
    }

    BTA_TRACE(1, "%s", __func__);
    return WH_BT_STATUS_SUCCESS;
}

static wh_bt_status_t unregister_server(int server_if)
{  
    bes_gatt_server_unregister_service(server_if);
    BTA_TRACE(1, "%s", __func__);
    return WH_BT_STATUS_SUCCESS;
}

static wh_bt_status_t server_connect(int server_if, const bt_bdaddr_t *bd_addr, bool is_direct, int transport)
{
    return WH_BT_STATUS_SUCCESS;
}

static wh_bt_status_t server_disconnect(int server_if, const bt_bdaddr_t *bd_addr, int conn_id)
{
    return WH_BT_STATUS_SUCCESS;
}

static wh_bt_status_t add_service(int server_if, btgatt_srvc_id_t *srvc_id, int num_handles)
{
    int status = 0;
    bt_uuid_t * svc_uuid;
    bes_adapter_attm_item_t att_item;
    bes_adapter_ble_service_db_t* service_db = btadapter_ble_service_get_service_by_if(server_if);

    if(service_db){
        service_db->nattb = num_handles;
        svc_uuid = &srvc_id->id.uuid;
        memcpy(att_item.uuid, UUID_DATA(svc_uuid), UUID_LEN(svc_uuid));
        att_item.ext_perm = 0;
        att_item.prop = ADAPTER_BLE_PROP_READ;
        att_item.perm = 0;
        status = bes_gatt_server_add_att_item(server_if,
                                                            service_db->handle_offset ++,
                                                            &att_item);        
        if(g_btgatt_cb){
               g_btgatt_cb->server->service_added_cb(status ,
                                                                               server_if,
                                                                               srvc_id, 
                                                                               service_db->start_handle);           
        }
    }else
        status = -1;

    if(status == 0)
        return WH_BT_STATUS_SUCCESS;
    else
        return WH_BT_STATUS_FAIL;
}

static wh_bt_status_t add_included_service(int server_if, int service_handle,
                        int included_handle)
{
    uint8_t included_uuid[2] = GATT_DECL_INCLUDED_SERVICE;
    bes_adapter_attm_item_t att_item;
    int status = 0;
    int incl_srvc_handle = 0;
    
    bes_adapter_ble_service_db_t* service_db = btadapter_ble_service_get_service_by_if(server_if);
    if(service_db){
        memcpy(att_item.uuid, included_uuid, 2);
        att_item.ext_perm = included_handle;
        att_item.prop = ADAPTER_BLE_PROP_READ;
        att_item.perm = 0;
        incl_srvc_handle = service_db->start_handle + service_db->handle_offset;
        status = bes_gatt_server_add_att_item(server_if,
                                                            service_db->handle_offset ++,
                                                            &att_item);
    }
     if(g_btgatt_cb){
           g_btgatt_cb->server->included_service_added_cb(status,
                                                                                        server_if,
                                                                                        service_handle,
                                                                                        incl_srvc_handle);
    }

    if(status == 0)
        return WH_BT_STATUS_SUCCESS;
    else
        return WH_BT_STATUS_FAIL;     
}
                        
static wh_bt_status_t add_characteristic(int server_if, int service_handle,
                        bt_uuid_t *uuid, int properties,
                        int permissions)
{
    bes_adapter_attm_item_t att_item;
    int status = 0;
    int char_handle = 0;
    bes_adapter_ble_service_db_t* service_db = btadapter_ble_service_get_service_by_if(server_if);

    if(service_db){
        memcpy(att_item.uuid, UUID_DATA(uuid), UUID_LEN(uuid));
        att_item.ext_perm = 0;
        att_item.prop = ADAPTER_BLE_PROP_READ;
        att_item.perm = 0;
        char_handle = service_db->start_handle + service_db->handle_offset;
        status = bes_gatt_server_add_att_item(server_if,
                                                            service_db->handle_offset ++,
                                                        &att_item);
    }
                
     if(g_btgatt_cb){
           g_btgatt_cb->server->characteristic_added_cb(status,
                                                                                        server_if,
                                                                                        uuid,
                                                                                        service_handle,
                                                                                        char_handle);
    }
     
    if(status == 0)
        return WH_BT_STATUS_SUCCESS;
    else
        return WH_BT_STATUS_FAIL;     
}
                        
static wh_bt_status_t add_descriptor(int server_if, int service_handle,
                    bt_uuid_t *uuid, int permissions)
{
    bes_adapter_attm_item_t att_item;
    int status = 0;
    int descr_handle = 0;
    bes_adapter_ble_service_db_t* service_db = btadapter_ble_service_get_service_by_if(server_if);
    
    if(service_db){
        memcpy(att_item.uuid, UUID_DATA(uuid), UUID_LEN(uuid));
        att_item.ext_perm = 0;
        att_item.prop = ADAPTER_BLE_PROP_READ;
        att_item.perm = 0;
        descr_handle = service_db->start_handle + service_db->handle_offset;
        status = bes_gatt_server_add_att_item(server_if,
                                                            service_db->handle_offset ++,
                                                        &att_item);
    }
                
    if(g_btgatt_cb){
           g_btgatt_cb->server->descriptor_added_cb(status,
                                                                            server_if,
                                                                            uuid,
                                                                            service_handle,
                                                                            descr_handle);
    }

    if(status == 0)
        return WH_BT_STATUS_SUCCESS;
    else
        return WH_BT_STATUS_FAIL;    

}
                
static wh_bt_status_t start_service(int server_if, int service_handle,
                                int transport)
{
    int status = 0;
    bes_adapter_ble_service_db_t* service_db = btadapter_ble_service_get_service_by_if(server_if);

    if(service_db){
        status = bes_gatt_server_start_service(server_if);
        BTA_TRACE(1, "%s start_handle:%d handle_num:%d user_handle_num:%d", service_db->start_handle, service_db->handle_offset, service_db->nattb);
    }else
        status = -1;
    
    if(g_btgatt_cb){
           g_btgatt_cb->server->service_started_cb(status,
                                                                            server_if,
                                                                            service_handle);
    }
     
    if(status == 0)
        return WH_BT_STATUS_SUCCESS;
    else
        return WH_BT_STATUS_FAIL;     
}

static wh_bt_status_t stop_service(int server_if, int service_handle)
{
    int status = -1;
    /*
        service can't stop now
    */
    if(g_btgatt_cb){
           g_btgatt_cb->server->service_started_cb(status,
                                                                            server_if,
                                                                            service_handle);
    }

    return WH_BT_STATUS_FAIL;
}

static wh_bt_status_t delete_service(int server_if, int service_handle)
{
    int status = -1;
    /*
        service can't stop now
    */
    if(g_btgatt_cb){
           g_btgatt_cb->server->service_started_cb(status,
                                                                            server_if,
                                                                            service_handle);
    }

    return WH_BT_STATUS_FAIL;    
}

static wh_bt_status_t send_indication(int server_if, int attribute_handle,
                                                                        int conn_id, int len, int confirm,
                                                                        char *p_value)
{
    int status = 0;
    bes_adapter_ble_service_db_t* service_db = btadapter_ble_service_get_service_by_if(server_if);

    if(service_db){
        status = bes_gatt_server_send_indication(server_if,
                                                                                conn_id,
                                                                                attribute_handle,
                                                                                confirm,
                                                                                p_value,
                                                                                len);
    }else
        status = -1;

    if(g_btgatt_cb){
              g_btgatt_cb->server->indication_sent_cb(conn_id,
                                                                               status);
       }

    if(status == 0)
        return WH_BT_STATUS_SUCCESS;
    else
        return WH_BT_STATUS_FAIL; 
}
                
static wh_bt_status_t send_response(int conn_id, int trans_id, int status,
                        btgatt_response_t *response)
{
    int rStatus = 0;
    const uint8_t * res_data = NULL;
    
    bes_adapter_ble_service_db_t* service_db = btadapter_ble_service_get_service_by_handle(response->handle);

    if(service_db){
        res_data = response->attr_value.value[response->attr_value.offset];
        rStatus = btadapter_ble_service_send_gatt_get_response(service_db,
                                                                                                        conn_id,
                                                                                                        response->attr_value.handle,
                                                                                                        res_data,
                                                                                                        response->attr_value.len,
                                                                                                        status);
    }else
        rStatus = -1;

    if(g_btgatt_cb){
        g_btgatt_cb->server->response_confirmation_cb(status,
                                                                                   response->attr_value.handle);
    }

    if(rStatus == 0)
        return WH_BT_STATUS_SUCCESS;
    else
        return WH_BT_STATUS_FAIL; 
}
                

static void cleanup(void)
{
    
}

extern void app_ble_custom_init(void);                
static wh_bt_status_t init(const btgatt_callbacks_t *callbacks)
{
    g_btgatt_cb = callbacks;
    app_ble_custom_init();
    BesGattServerRegisterCallback();
}

static btgatt_server_interface_t server_iface = {
    .register_server = register_server,
    .unregister_server = unregister_server,
    .connect = server_connect,
    .disconnect = server_disconnect,
    .add_service = add_service,
    .add_included_service = add_included_service,
    .add_characteristic = add_characteristic,
    .add_descriptor = add_descriptor,
    .start_service = start_service,
    .stop_service = stop_service,
    .delete_service = delete_service,
    .send_indication = send_indication,
    .send_response = send_response,
};

static btgatt_interface_t iface = {
    .size = sizeof(iface),
    .init = init,
    .cleanup = cleanup,
    .client = NULL,
    .server = &server_iface,
};

btgatt_interface_t *bt_get_gatt_interface(void)
{
    return &iface;
}
