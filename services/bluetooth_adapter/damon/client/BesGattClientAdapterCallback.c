#include "plat_types.h"
#include "hal_trace.h"
#include <string.h>
#include "hal_trace.h"
#include "bleadapter_utils.h"

extern void BesGattClientAdapterTest(void);
extern adapter_client_task_env_t * BesGattClientGetEnv(void);

void BesGattcInitCallback(uint16_t status)
{
    BTA_TRACE(1, "%s", __func__);
}
/**
 * @brief Inform the ble connection result
 * @param[in] connId Connection ID
 * @param[in] bdAddr Peer bluetooth device addr
 * @param[in] event See BT_GATTC_EVENT_T enums
 * @return void
 */
void BesGattcLinkEventCallback(int32_t connId, const unsigned char* bdAddr, bes_gatt_link_event_e event)
{
    BTA_TRACE(1, "%s %d", __func__, event);

    if(event == BES_LINK_EVENT_GATT_CONNECT){
        BesGattClientAdapterTest();
    }
}

/**
 * @brief Get the services list
 * @param[in] gattServices Services list
 * @return void
 */
void BesGattcServiceDiscoveredCallback(uint16_t serv_index, bes_gatt_services_t* gattServices)
{
    if (gattServices == NULL){
        return;
    }
    uint16_t start_handle = 0;
    uint8_t uuid_cfg[2] = {0x02, 0x29};

    BTA_TRACE(3, "%s hd:%x sthd:%x", __func__, gattServices->pElem[0].handle, gattServices->pElem[0].start_handle);
#if 1
    for(int i = 0; i < gattServices->count; i ++){
        if(gattServices->pElem[i].type == BES_GATT_DB_PRIMARY_SERVICE)
            start_handle = gattServices->pElem[i].start_handle;

        else if(gattServices->pElem[i].type == BES_GATT_DB_CHARACTERISTIC){
            btadapter_ble_client_start_char_discovery(0,
                                                                                     BES_GATT_DISCOVER_CHAR_ALL,
                                                                                     (uint8_t *)gattServices->pElem[i].uuid,
                                                                                     gattServices->pElem[i].uuid_len,
                                                                                     start_handle,
                                                                                     gattServices->pElem[i].handle + 2);
            break;
        }
    }

    btadapter_ble_client_start_desc_discovery(0,
                                                                            1,
                                                                            0xffff);

    btadapter_ble_client_start_char_discovery(0,
                                                                         BES_GATT_DISCOVER_CHAR_BY_UUID,
                                                                         uuid_cfg,
                                                                         2,
                                                                         start_handle,
                                                                         start_handle + 0xff);

    btadapter_ble_client_read_char(0, 18);
#endif
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
void BesGattcCharacterReadCallback(int32_t connId, int32_t handle, bes_gatt_status_e status,
                                        const uint8_t* data, int32_t size)
{
    BTA_TRACE(1, "%s hd:%x", __func__, handle);
}
/**
 * @brief Inform characteristic write result
 * @param[in] connId Connection id
 * @param[in] handle Attribute handle
 * @param[in] status Status
 * @return void
 */
void BesGattcCharacterWriteCallback(int32_t connId, uint16_t handle, bes_gatt_status_e status)
{
    BTA_TRACE(1, "%s hd:%x", __func__, handle);
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
void BesGattcCharacteChangedCallback(int32_t connId, uint32_t handle,
                                        const uint8_t* data, int32_t size, uint8_t isNotify)
{
    BTA_TRACE(1, "%s hd:%x", __func__, handle);
}

/**
 * @brief Enable characteristic notify callbac
 * @param[in] connId Connection id
 * @param[in] handle Attribute handle
 * @param[in] registered 1 is success
 * @return void
 */
void BesGattcNotificationEnableCallback(int32_t connId, int32_t handle, int32_t registered)
{
    BTA_TRACE(1, "%s hd:%x", __func__, handle);
}

void BesGattcRegisterCallback(void)
{
    bes_gattc_state_callback_t gattcCb;

    gattcCb.initCb = BesGattcInitCallback;
    gattcCb.linkEventCb = BesGattcLinkEventCallback;
    gattcCb.discoveryCb = BesGattcServiceDiscoveredCallback;
    gattcCb.charReadCb = BesGattcCharacterReadCallback;
    gattcCb.charWriteCb = BesGattcCharacterWriteCallback;
    gattcCb.charChangedNotifyCb = BesGattcCharacteChangedCallback;
    gattcCb.notifyEnableCb = BesGattcNotificationEnableCallback;

    btadapter_ble_register_client_callback(&gattcCb);
}


