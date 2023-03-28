#ifndef __BES_GATTS_ADATER_H__
#define __BES_GATTS_ADATER_H__
/**
 ****************************************************************************************
 * @brief Create ble server database.
 *
 * @param[in]    server_if      server interface id.
 * @param[in]    service_uuid   service UUID
 * @param[in]    uuid_len     2 for uuid 16. 16 for uuid 128
 * @param[in]    is_primary is primary or secondary service.
 * @param[in]    number      service characther attribute number.
 * @param[out]  int32_t      -1 for failed create service else return server_if
 ****************************************************************************************
 */
int32_t bes_gatt_server_add_service(int32_t server_if,
                                                                  uint8_t * service_uuid,
                                                                  uint8_t uuid_len,
                                                                  uint8_t is_primary,
                                                                  int32_t number,
                                                                  bool use_bes_db);

/**
 ****************************************************************************************
 * @brief add ble server character att.
 *
 * @param[in]    server_if      server interface id.
 * @param[in]    service_handle   char hadnle
 * @param[in]    uuid           character uuid.
 * @param[in]    uuid_len     2 for uuid 16. 16 for uuid 128
 * @param[in]    properties   character is write or read, indication or notify.
 * @param[in]    permissions      auth or encripted secure properties.
 * @param[out]  int32_t      -1 for failed create service.
 ****************************************************************************************
 */
int32_t bes_gatt_server_add_att_item(int32_t server_if,
                                                                                     uint8_t handle_offset,
                                                                                     bes_adapter_attm_item_t *att_item);

/**
 ****************************************************************************************
 * @brief add server to ble stack database.
 *
 * @param[in]    server_if      server interface id.
 * @param[out]  int32_t      -1 for failed create service.
 ****************************************************************************************
 */
int32_t bes_gatt_server_start_service(int32_t server_if);

/**
 ****************************************************************************************
 * @brief stop server.
 *
 * @param[in]    server_if      server interface id.
 * @param[out]  int32_t      -1 for failed create service.
 ****************************************************************************************
 */
int32_t bes_gatt_server_stop_service(int32_t server_if);

/**
 ****************************************************************************************
 * @brief delete server.
 *
 * @param[in]    server_if      server interface id.
 * @param[out]  int32_t      -1 for failed delete service.
 ****************************************************************************************
 */
int32_t bes_gatt_server_delete_service(int32_t server_if);

/**
 ****************************************************************************************
 * @brief unregister server.
 *
 * @param[in]    server_if      server interface id.
 * @param[out]  int32_t      -1 for failed create service.
 ****************************************************************************************
 */
int32_t bes_gatt_server_unregister_service(int32_t server_if);

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
                                                                         int32_t value_len);

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
                                                                         int32_t value_len);

#endif
