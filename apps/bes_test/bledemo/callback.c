#include "plat_types.h"
#include "hal_trace.h"
#include "bleadapter_utils.h"
#include <string.h>
#include "hal_trace.h"

/**
 * @brief Inform the gatt server init result
 * @param[in] serverIf Indicates id of server interface
 * @return void
 */
static void InitCallback(int32_t serverIf)
{
    TRACE(1, "bledemo.%s %d", __func__, serverIf);
}

/**
 * @brief Callback invoked in response to create_service
 * @param[in] serverIf Service parameter
 * @param[in] srvc_handle Service parameter
 * @return void
 */
static void AddServiceCallback(int32_t serverIf, int32_t srvc_handle)
{
    TRACE(1, "bledemo.%s %d", __func__, serverIf);
}

/**
 * @brief Callback invoked when a characteristic has been added to a service
 * @param[in] bt_gatts_add_char Characteristic parameter(server if, uuid ,
 *                                      service handle and characteristic handle)
 * @return void
 */
static void AddCharCallback(bes_gatts_add_char_para_t *bt_gatts_add_char )
{
    if (bt_gatts_add_char == NULL){
        return;
    }
    TRACE(1, "bledemo.%s %d", __func__, bt_gatts_add_char->server_if);
}

/**
 * @brief Callback invoked when a descriptor has been added to a characteristic
 * @param[in] bt_gatts_add_desc Descriptor parameter(server if, uuid ,
 *                                      service handle and descriptor handle)
 * @return void
 */
static void AddDescCallback(bes_gatts_add_desc_para_t *bt_gatts_add_desc)
{
    if (bt_gatts_add_desc == NULL){
        return;
    }
    TRACE(1, "bledemo.%s %d hd%x", __func__, bt_gatts_add_desc->server_if, bt_gatts_add_desc->srvc_handle);
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
static uint16_t ReqWriteCallback(int32_t server_if, 
                                                                    int32_t conn_id,
                                                                    uint16_t task_id,
                                                                    int32_t handle, 
                                                                    int32_t offset, 
                                                                    int32_t length, 
                                                                    uint8_t * data)
{
    TRACE(1, "bledemo.%s %d hd:%x", __func__, server_if, handle); 
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
static void ReqReadCallback(int32_t server_if, 
                                                                    int32_t conn_id,
                                                                    uint16_t task_id,
                                                                    int32_t handle, 
                                                                    int32_t offset)
{
    uint16_t status = BES_ATT_ERR_NO_ERROR;
    bes_adapter_attm_desc_t *att_elem = NULL;
    
    bes_adapter_ble_service_db_t * service_db = btadapter_ble_service_get_service_by_if(server_if);
    if(service_db){
        att_elem= btadapter_ble_service_get_att_db_by_handle(service_db, handle);
        if(!att_elem){
            status = BES_BLE_ERR_INSUFFICIENT_RESOURCES;
            btadapter_ble_service_send_gatt_get_response(service_db,
                                                                                                conn_id,
                                                                                                handle,
                                                                                                NULL,
                                                                                                0,
                                                                                                status);            
        }else{
             btadapter_ble_service_send_gatt_get_response(service_db,
                                                                                                conn_id,
                                                                                                handle,
                                                                                                att_elem->data,
                                                                                                2,
                                                                                                status);
        }
    }else{

        btadapter_ble_service_send_gatt_get_response(service_db,
                                                                                                    conn_id,
                                                                                                    handle,
                                                                                                    att_elem->data,
                                                                                                    2,
                                                                                                    status); 
    }
    TRACE(2, "bledemo.%s %d hd:%x", __func__, server_if, handle);
}

/**
 * @brief Callback invoked when GATT Service is started
 * @return void
 */
static void StartServerCallback(int32_t server_if)
{
    TRACE(1, "bledemo.%s %d", __func__, server_if); 
}

/**
 * @brief Callback invoked when GATT Service is stop
 * @return void
 */
static void StopServerCallback(int32_t server_if)
{
    TRACE(1, "bledemo.%s %d", __func__, server_if); 
}

/**
 * @brief Callback invoked when GATT Service is delete
 * @return void
 */
static void DeleteServerCallback(int32_t server_if)
{
    TRACE(1, "bledemo.%s %d", __func__, server_if); 
}

void BleDemoCallback(void)
{
    bes_gatts_state_callback_t gattsCb = {0};
    
    gattsCb.addCharCb = AddCharCallback;
    gattsCb.addDescCb = AddDescCallback;
    gattsCb.addServiceCb = AddServiceCallback;
    gattsCb.deleteServerCb = DeleteServerCallback;
    gattsCb.initCb = InitCallback;
    gattsCb.reqReadCb = ReqReadCallback;
    gattsCb.reqWriteCb = ReqWriteCallback;
    gattsCb.startServerCb = StartServerCallback;
    gattsCb.stopServerCb = StopServerCallback;
    btadapter_ble_register_server_callback(&gattsCb);
}


