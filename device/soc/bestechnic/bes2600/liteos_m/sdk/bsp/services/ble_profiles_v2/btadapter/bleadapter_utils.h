#ifndef __BLE_ADAPTER_H__
#define __BLE_ADAPTER_H__
#include "bleadapter_def.h"

#define BTA_TRACE REL_TRACE

#ifdef __cplusplus
extern "C" {
#endif

extern bes_gattc_state_callback_t g_BesGattcStateCb;
extern bes_gatts_state_callback_t g_BesGattsStateCb;

uint8_t btadapter_ble_uuid_type_to_len(uint32_t uuid_type);

uint8_t btadapter_ble_uuid_len_to_type(uint32_t uuid_len);

uint16_t build_ble_att_info(int32_t properties, int32_t permissions);

bool btadapter_check_ntf_perm(uint16_t prop);

bool btadapter_check_read_perm(uint16_t prop);

bool btadapter_check_write_perm(uint16_t prop);

uint16_t build_ble_des_info(int32_t permissions);

uint16_t btadapter_ble_service_get_start_task_id(void);

//ble server
bes_adapter_ble_service_db_t * btadapter_ble_service_get_free_db(bool use_bes_db);

bes_adapter_ble_service_db_t * btadapter_ble_service_get_service_by_if(int32_t server_if);

bes_adapter_ble_service_db_t * btadapter_ble_service_get_service_by_handle(uint16_t handle);

void bes_gatt_server_update_att_num(uint8_t server_if, uint8_t count);

bes_adapter_ble_service_db_t * btadapter_ble_service_get_service_by_user_lid(uint8_t user_lid);

bes_adapter_attm_desc_t  * btadapter_ble_service_get_free_att_db(bes_adapter_ble_service_db_t *service_db);

void * btadapter_ble_service_alloc_atts_mem(uint16_t numbers);

/**
 ****************************************************************************************
 * @brief get  attribute database from service database by handle.
 *
 * @param[in]    bes_adapter_ble_service_db_t.
 * @param[out]    bes_adapter_attm_desc_t.
 ****************************************************************************************
 */
bes_adapter_attm_desc_t  * btadapter_ble_service_get_att_db_by_handle(bes_adapter_ble_service_db_t *service_db, uint16_t handle);

void btadapter_ble_service_db_init(void);

void btadapter_ble_register_server_callback(bes_gatts_state_callback_t * cb);

void btadapter_ble_service_start_service(bes_adapter_ble_service_db_t * service_db);

void btadapter_ble_service_stop_service(bes_adapter_ble_service_db_t * service_db);

void bleadapter_service_delete_service(bes_adapter_ble_service_db_t * service_db);

bes_gatt_status_e btadapter_ble_service_send_notifiction(bes_adapter_ble_service_db_t * service_db,
                                                                                                                            bool isNotification,
                                                                                                                            uint8_t conidx,
                                                                                                                            uint16_t handle,
                                                                                                                            const uint8_t* ptrData,
                                                                                                                            uint32_t length);

bes_gatt_status_e btadapter_ble_service_send_gatt_get_response(bes_adapter_ble_service_db_t * service_db,
                                                                                uint8_t conidx,
                                                                                uint16_t handle,
                                                                                const uint8_t* ptrData,
                                                                                uint32_t length,
                                                                                uint16_t att_status);

void btadapter_ble_service_send_gatt_write_response( uint8_t conidx,
                                                                                                        bes_adapter_ble_service_db_t* service_db,
                                                                                                        uint16_t att_status);

//indication/notifications

void btadapter_ble_service_update_att_db_value(bes_adapter_attm_desc_t * att_item,
                                                                                                                uint8_t offset,
                                                                                                                uint8_t* p_value,
                                                                                                                uint8_t len);

uint16_t btadapter_ble_service_cal_uuid_mem_size(uint16_t uuid_len);

uint16_t btadapter_ble_service_add_att_to_stack_db(bes_adapter_ble_service_db_t *service_db,
                                                                                                        uint8_t handle_offset,
                                                                                                        bes_adapter_attm_item_t *att_item);

uint16_t btadapter_ble_service_enable_stack_db(bes_adapter_ble_service_db_t *service_db,
                                                                                                            uint8_t sec_lvl);

//client

uint8_t btadapter_ble_client_db_update_conidx_by_addr(uint8_t conidx, const uint8_t *addr);

uint8_t btadapter_ble_client_db_flush_by_conidx(uint8_t conidx);

bes_adapter_client_db_t * btadapter_ble_client_get_db_connect(uint32_t client_if);

bes_adapter_client_db_t * btadapter_ble_client_get_db_by_if(uint32_t client_if);

bes_adapter_client_db_t* btadapter_ble_client_get_db_by_addr(const uint8_t *addr);

bes_adapter_client_db_t* btadapter_ble_client_get_db_by_id(uint8_t conidx);

bes_adapter_client_db_t* btadapter_ble_client_get_db_free(void);

uint8_t* btadapter_ble_client_get_bdaddr_by_if(int clientId);

void btadapter_ble_register_client_callback(bes_gattc_state_callback_t * cb);

void btadapter_ble_client_start_service(void);

uint16_t btadapter_ble_client_get_start_task_id(void);

bes_gatt_status_e btadapter_ble_client_register_svc_notify(uint8_t conidx,
                                                                bes_gattc_svc_notify_t * svc_notify);
bes_gatt_status_e btadapter_ble_client_unregister_svc_notify(uint8_t conidx,
                                                                bes_gattc_svc_notify_t * svc_notify);

bes_gatt_status_e btadapter_ble_client_start_service_discovery(uint8_t conidx,
                                                                                                                        bes_gatt_discovery_type_e discType,
                                                                                                                        uint8_t * uuid,
                                                                                                                        uint8_t uuid_len,
                                                                                                                        uint16_t start_hdl,
                                                                                                                        uint16_t end_hdl);

bes_gatt_status_e btadapter_ble_client_start_char_discovery(uint8_t conidx,
                                                                                                                        bes_gatt_char_discovery_type_e discType,
                                                                                                                        uint8_t * uuid,
                                                                                                                        uint8_t uuid_len,
                                                                                                                        uint16_t start_hdl,
                                                                                                                        uint16_t end_hdl);

bes_gatt_status_e btadapter_ble_client_start_desc_discovery(uint8_t conidx,
                                                                                                                        uint16_t start_hdl,
                                                                                                                        uint16_t end_hdl);
uint8_t * btadapter_ble_get_addr_by_conidx(uint8_t conidx);

uint8_t btadapter_ble_get_conidx_by_addr(uint8_t *bdAddr);

void btadapter_ble_client_read_char(uint8_t conidx, uint16_t handle);

void btadapter_ble_client_read_by_uuid(uint8_t conidx,
                                                                                        uint16_t start_handle,
                                                                                        uint16_t end_handle,
                                                                                        uint8_t uuid_len,
                                                                                        uint8_t* uuid);

uint16_t btadapter_ble_client_write(int32_t connId,
                                                                       uint32_t handle,
                                                                       bes_gatt_write_type_e writeType,
                                                                       const uint8_t* data,
                                                                       int32_t size);

uint16_t btadapter_ble_client_exe_write(int32_t connId,int32_t execute);

uint16_t btadapter_ble_client_mtu_configure(int32_t connId,int32_t mtu_size);

void btadapter_ble_client_update_conn_para(uint8_t  conidx,
                                                    uint32_t min_interval_in_ms,
                                                    uint32_t max_interval_in_ms,
                                                    uint32_t supervision_timeout_in_ms,
                                                    uint8_t  slaveLatency);

#ifdef __cplusplus
}
#endif
#endif
