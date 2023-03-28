/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    tg_bluetooth.h
 * @brief:   vendor bluetooth apis defination
 * @author:
 * @date:    2019/12/15
 * @version: 1.0
 */
#if 0
#include "tg_bluetooth.h"
#include "bt_adaptor_dbg.h"
#ifdef IBRT
#include "app_ibrt_customif_cmd.h"
#include "app_tws_ctrl_thread.h"
#include "hal_trace.h"
#include "app_ibrt_if.h"
#include "app_tws_ibrt.h"
#endif
#include "nvrecord_env.h"
#include "nvrecord_bt.h"
#include "bt_adaptor_dbg.h"
//#include "bluetooth.h"
#include "factory_section.h"

/**
 * @brief gatt service register callback set
 * @param[in] callback:  gatt service callback set.
 * @return: None
 */
void tg_bt_gattsSetCallback(TG_BT_GATTS_CB_FUNC_T *callback)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
}

/**
 * @brief gatt service initial
 *        should callback gattsInitCB when finished
 * @param[in]: None.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_initGatts(void)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
}

/**
 * @brief gatt service deinitial
 * @param[in]: None.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_deinitGatts(void)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
}

/**
 * @brief gatt service get registed callback
 * @param[in]: None.
 * @return: callback set
 */
TG_BT_GATTS_CB_FUNC_T *tg_bt_gattsGetCallback(void)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return NULL;
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
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
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
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
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
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
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
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
}

/**
 * @brief gatt service stop service
 * @param[in] server_if:  service interface id, returned in gattsInitCB.
 * @param[in] service_handle:  service handle, returned in gattsAddServiceCB.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattsStopService(int32_t server_if, int32_t service_handle)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
}

/**
 * @brief gatt service delete service
 * @param[in] server_if:  service interface id, returned in gattsInitCB.
 * @param[in] service_handle:  service handle, returned in gattsAddServiceCB.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattsDeleteService(int32_t server_if, int32_t service_handle)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
}

/**
 * @brief gatt unregister all services on this server interface
 * @param[in] server_if:  service interface id, returned in gattsInitCB.
 * @return: None
 */
int32_t tg_bt_gattsUnregisterService(int32_t server_if)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
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
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
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
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
}

/**
 * @brief gatt service set server interface
 * @param[in] serverIf: service interface id, returned in gattsInitCB.
 * @return: None
 */
void tg_bt_gattsSetServerIf(int32_t serverIf)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
}

/**
 * @brief gatt service delete server interface
 * @param[in] serverIf: service interface id, returned in gattsInitCB.
 * @return: None
 */
void tg_bt_gattsDelServerIf(int32_t serverIf)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
}

/**
 * @brief gatt server advertise broadcast packet data set
 * @param[in] data:  advertise data.
 * @param[in] len:  advertise data length.
 * @return: None
 */
int32_t tg_bt_gattsSetAdvData(uint8_t *data, int len)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
}

/**
 * @brief gatt server enable/disable advertise broadcast
 * @param[in] enable: 1 means enable; 0 means disable.
 * @return: None
 */
int32_t tg_bt_gattsEnableAdv(bool enable)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
}


/* GATT Client */

/**
 * @brief gatt client initial
 *        should callback gattcInitCB when finished
 * @param[in]: None.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_initGattc(void)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
}

/**
 * @brief gatt client deinitial
 * @param[in]: None.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_deinitGattc(void)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
}


/**
 * @brief gatt client register callback set
 * @param[in] callback:  gatt client callback set.
 * @return: None
 */
void tg_bt_gattcSetCallback(TG_BT_GATTC_CB_FUNC_T *callback)
{
    LOG_E("enter %s", __FUNCTION__);
}

/**
 * @brief gatt client advertise broadcast packet data set
 * @param[in] data:  advertise data.
 * @param[in] len:  advertise data length.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattcSetAdvData(uint8_t *data, int len)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
}

/**
 * @brief gatt client enable/disable advertise broadcast
 * @param[in] enable: 1 means enable; 0 means disable.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattcEnableAdv(bool enable)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
}

/**
 * @brief gatt client start scan
 * @param[in] index: application index
 * @return: None
 */
int32_t tg_bt_startScan(int32_t index)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
}

/**
 * @brief gatt client stop scan
 * @param[in] None
 * @return: None
 */
int32_t tg_bt_stopScan(int32_t index)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
}

/**
 * @brief gatt client setup connection
 * @param[in] target device address
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattcConnect(const char *bdAddr)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
}

/**
 * @brief gatt client remove connection
 * @param[in] connection id
 * @param[in] target device address
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattcDisconnect(int32_t connId, const char *bdAddr)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
}

/**
 * @brief gatt client discovery gatt services
 * @param[in] connection id
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattcDiscoverServices(int32_t connId)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
}

/**
 * @brief gatt client read characteristic
 * @param[in] connection id
 * @param[in] attribute handle
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattcReadCharacteristic(int32_t connId, uint32_t handle)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
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
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
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
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
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
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
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
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return -1;
}

/**
 * @brief gatt adv state
 * @param[in] None.
 * @return: on/off
 */
uint8_t tg_bt_get_gatt_scan_enable(void)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return 0;
}

/**
 * @brief return scan result.
 * @param[in] bdAddr: addr, like "F8:A7:63:45:13:71"
 * @param[in] rssi: rssi
 * @param[in] pData: adv data
 * @param[in] size: data length
 * @return: None.
 */
void tg_bt_gattc_ScanResult(const int8_t *bdAddr, int32_t rssi, uint8_t *pData, int32_t size)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
}
#endif
