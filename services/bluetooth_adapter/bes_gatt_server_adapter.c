#include "plat_types.h"
#include "hal_trace.h"
#include "bleadapter_utils.h"
#include <string.h>
#include "hal_trace.h"

/**
 ****************************************************************************************
 * @brief Create ble server database.
 *
 * @param[in]    server_if      server interface id, -1 means dynamic create server id.
 * @param[in]    service_uuid   service UUID
 * @param[in]    uuid_len     2 for uuid 16. 16 for uuid 128
 * @param[in]    is_primary is primary or secondary service.
 * @param[in]    number      service characther attribute number.
 * @param[in]    use_bes_db   bes stack manage database..
 * @param[out]  int32_t      -1 for failed create service else retrun server_if
 ****************************************************************************************
 */
int32_t bes_gatt_server_add_service(int32_t server_if,
                                                                  uint8_t * service_uuid,
                                                                  uint8_t uuid_len,
                                                                  uint8_t is_primary,
                                                                  int32_t number,
                                                                  bool use_bes_db)
{
    bes_adapter_ble_service_db_t * service_db = NULL;

    service_db = btadapter_ble_service_get_free_db(use_bes_db);
    if(!service_db){
       return -1;
    }

    if(server_if == -1)
        server_if = service_db->inst_id;

    service_db->server_if = server_if;
    service_db->is_primary = is_primary;
    service_db->nattb = number;
    service_db->use_bes_db = use_bes_db;
    if(uuid_len == 2)
        service_db->service_perm = 0; //PERM_UUID_16
    else if(uuid_len == 4)
        service_db->service_perm = 1; //PERM_RIGHT_UUID_32
    else
        service_db->service_perm = 2;//PERM_UUID_128
    service_db->uuid_len = uuid_len;
    memcpy(service_db->uuid, service_uuid, uuid_len);
    BTA_TRACE(1, "%s server_if:%d num:%d %x \n", __func__, server_if, number,  service_db->start_handle);

    if(!use_bes_db && number){
        service_db->stack_srv_db = btadapter_ble_service_alloc_atts_mem(number);
        BTA_TRACE(1, "service_db->stack_srv_db=0x%x", (uint32_t)service_db->stack_srv_db);
    }
    return server_if;
}

/**
 ****************************************************************************************
 * @brief Create ble server database.
 *
 * @param[in]    server_if      server interface id
  * @param[in]   count           update server db  item number.
 */

void bes_gatt_server_update_att_num(uint8_t server_if, uint8_t count)
{
    bes_adapter_ble_service_db_t * service_db = NULL;

    service_db = btadapter_ble_service_get_service_by_if(server_if);
    if(!service_db){
        return;
    }
    service_db->nattb = count;
    if(service_db->stack_srv_db == NULL){
        service_db->stack_srv_db = btadapter_ble_service_alloc_atts_mem(count);
        BTA_TRACE(1, "update_att_num service_db->stack_srv_db=0x%x", (uint32_t)service_db->stack_srv_db);
    }
}
/**
 ****************************************************************************************
 * @brief add ble server character att.
 *
 * @param[in]    server_if      server interface id.
 * @param[in]    service_handle   char hadnle
 * @param[in]    bes_adapter_attm_item_t           att_item.
 ****************************************************************************************
 */
int32_t bes_gatt_server_add_att_item(int32_t server_if,
                                                             uint8_t handle_offset,
                                                             bes_adapter_attm_item_t *att_item)
{
    if (att_item == NULL){
        return -1;
    }
    bes_adapter_ble_service_db_t * service_db = NULL;
    bes_adapter_attm_desc_t *att_db = NULL;

    service_db = btadapter_ble_service_get_service_by_if(server_if);
    if(!service_db){
        return -1;
    }

    if(service_db->use_bes_db){
        att_db = btadapter_ble_service_get_free_att_db(service_db);
        if(!att_db){
            return -1;
        }

        att_db->handle_offset = handle_offset;
        att_db->perm = build_ble_att_info(att_item->prop, att_item->perm);
        att_db->max_size = ADAPTER_BLE_MAX_ATT_SIZE;
        att_db->ext_perm = att_item->ext_perm;
        att_db->uuid_len = att_item->uuid_len;
        att_db->declaration_hdl_offset = att_item->declaration_offset;
        if(att_item->value_len > 0){
            ASSERT(att_item->value_len < ADAPTER_BLE_MAX_ATT_SIZE, "ATT value size too large");
            att_db->data_len = att_item->value_len;
            memcpy(att_db->data, att_item->p_value, att_item->value_len);
        }
        memcpy(att_db->uuid, att_item->uuid, att_item->uuid_len);
        BTA_TRACE(1, "%s besdb server_if:%d, handle:%02x \n", __func__, server_if, handle_offset);
    }else{
        uint16_t status = BES_BLE_ERR_NO_ERROR;
        status = btadapter_ble_service_add_att_to_stack_db(service_db,
                                                                                    handle_offset,
                                                                                    att_item);
        BTA_TRACE(1, "%s sysdb %d server_if:%d, handle:%02x \n", __func__, status, server_if, handle_offset);
        if(status == BES_BLE_ERR_NO_ERROR)
            return 0;
        else
            return -1;
    }
    return 0;
}

/**
 ****************************************************************************************
 * @brief add server to ble stack database.
 *
 * @param[in]    server_if      server interface id.
 * @param[out]  int32_t      -1 for failed create service.
 ****************************************************************************************
 */
int32_t bes_gatt_server_start_service(int32_t server_if)
{
    bes_adapter_ble_service_db_t * service_db = NULL;

    BTA_TRACE(1, "startService:%d \n",server_if);
    service_db = btadapter_ble_service_get_service_by_if(server_if);
    if(!service_db){
        return -1;
    }

    BTA_TRACE(1, "startService start:%d \n",service_db->start_handle);
    btadapter_ble_service_start_service(service_db);

    BTA_TRACE(1, "%s server_if:%d, handle:%02x \n", __func__, server_if, service_db->start_handle);
    return 0;
}

/**
 ****************************************************************************************
 * @brief stop server.
 *
 * @param[in]    server_if      server interface id.
 * @param[out]  int32_t      -1 for failed stop service.
 ****************************************************************************************
 */
int32_t bes_gatt_server_stop_service(int32_t server_if){
    bes_adapter_ble_service_db_t * service_db = NULL;

    service_db = btadapter_ble_service_get_service_by_if(server_if);
    if(!service_db){
        return -1;
    }

    btadapter_ble_service_stop_service(service_db);
    BTA_TRACE(1, "stopService:%d\n", service_db->start_handle);
    return 0;
}

/**
 ****************************************************************************************
 * @brief delete server.
 *
 * @param[in]    server_if      server interface id.
 * @param[out]  int32_t      -1 for failed delete service.
 ****************************************************************************************
 */
int32_t bes_gatt_server_delete_service(int32_t server_if){
    bes_adapter_ble_service_db_t * service_db = NULL;

    service_db = btadapter_ble_service_get_service_by_if(server_if);
    if(!service_db){
        return -1;
    }

    bleadapter_service_delete_service(service_db);
    BTA_TRACE(1, "deleteService:%d\n", service_db->start_handle);
    return 0;
}

/**
 ****************************************************************************************
 * @brief unregister server.
 *
 * @param[in]    server_if      server interface id.
 * @param[out]  int32_t      -1 for failed create service.
 ****************************************************************************************
 */
int32_t bes_gatt_server_unregister_service(int32_t server_if){
    bes_adapter_ble_service_db_t* service_db = NULL;
    service_db = btadapter_ble_service_get_service_by_if(server_if);
    if(service_db){
        service_db->occupied = false;
    }
    return 0;
}

/**
 ****************************************************************************************
 * @brief send gatt read response.
 *
 * @param[in]    server_if    server interface id.
 * @param[in]    conn_id     connection index.
 * @param[in]    att_status   read character status from database.
 * @param[in]    handle       attribuet hadnle.
 * @param[in]    p_value     response data from host.
 * @param[in]    value_len   length for response data.
 * @param[out]  int32_t      -1 for failed create service.
 ****************************************************************************************
 */
int32_t bes_gatt_server_send_gatt_read_response(int32_t server_if,
                                                                         int32_t conn_id,
                                                                         uint16_t att_status,
                                                                         int32_t handle,
                                                                         int8_t * p_value,
                                                                         int32_t value_len)
{
    bes_adapter_ble_service_db_t * service_db = NULL;

    service_db = btadapter_ble_service_get_service_by_if(server_if);
       if(!service_db){
           return -1;
       }

    btadapter_ble_service_send_gatt_get_response(service_db,
                                                                conn_id,
                                                                handle,
                                                                (const uint8_t*)p_value,
                                                                value_len,
                                                                att_status);

    BTA_TRACE(1, "%s conn_id:%d  handle:%02x \n", __func__, conn_id, handle);
    return 0;
}

/**
 ****************************************************************************************
 * @brief send gatt notify or indication.
 *
 * @param[in]    server_if    server interface id.
 * @param[in]    conn_id     connection index.
 * @param[in]    handle       attribuet hadnle.
 * @param[in]    fg_confirm true for indication, false for notify.
 * @param[in]    p_value     response data from host.
 * @param[in]    value_len   length for response data.
 * @param[out]  int32_t      -1 for failed create service.
 ****************************************************************************************
 */
int32_t bes_gatt_server_send_indication(int32_t server_if,
                                                                         int32_t conn_id,
                                                                         uint16_t handle,
                                                                         int32_t fg_confirm,
                                                                         int8_t * p_value,
                                                                         int32_t value_len)
{
    bes_adapter_ble_service_db_t * service_db = NULL;
    int ret;
    service_db = btadapter_ble_service_get_service_by_if(server_if);
    if(!service_db){
        return -1;
    }

    if(fg_confirm)
        ret = btadapter_ble_service_send_notifiction(service_db,
                                                                false,
                                                                conn_id,
                                                                handle,
                                                                (const uint8_t*)p_value,
                                                                value_len);
    else
        ret = btadapter_ble_service_send_notifiction(service_db,
                                                                true,
                                                                conn_id,
                                                                handle,
                                                                (const uint8_t*)p_value,
                                                                value_len);
    BTA_TRACE(1, "%s conn_id:%d  handle:%02x \n", __func__, conn_id, handle);
    return ret;
}


