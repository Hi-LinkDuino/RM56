#include "string.h"
#include "plat_types.h"
#include "hal_trace.h"
#include "bleadapter_utils.h"
#include "bes_gatts_adapter.h"
#include "ohos_bt_gatt_client.h"
#include "harmony_utils.h"
#include "btadapter_ble_client.h"
#include "app_ble_mode_switch.h"
#include "bt_drv_reg_op.h"

uint8_t g_gattc_user_lid = 0;
struct prf_task_cbs * g_p_btadapter_client_itf =NULL;
prf_data_t g_gatt_client_profile_env = {0};
static adapter_client_task_env_t g_gatt_client_task_env = {0};
static BtGattClientCallbacks *g_p_gatt_client_call_back_funcs = NULL;
static BtUuid g_appUuid ={0};

extern void gapm_init(uint8_t init_type);

/*
 * @brief gatt client register, callback return clientId
 * @param[in] <appUuid> specified by upper layer
 * @return 0-success, other-fail
 */
int BleGattcRegister(BtUuid appUuid)
{
    int status = OHOS_GATT_SUCCESS;
    BleGattcRegisterCallback();
    g_gatt_client_task_env.task_id = btadapter_ble_client_get_start_task_id();
    g_appUuid.uuidLen= appUuid.uuidLen;
    if(appUuid.uuidLen)
    {
        memcpy(g_appUuid.uuid,appUuid.uuid,appUuid.uuidLen);
    }
    btadapter_ble_client_start_service();
    return status;
}

/*
 * @brief gatt client deregister
 * @param[in] <clientId> client Id
 * @return 0-success, other-fail
 */
int BleGattcUnRegister(int clientId)
{
    /*if(g_p_btadapter_client_itf){
        g_p_btadapter_client_itf->cb_destroy(&g_gatt_client_profile_env,0);
        // btadapter_ble_client_deinit(prf_data_t* p_env, uint8_t reason)
    }*/
    TRACE(0,"%s() clientId:%d\n",__func__,clientId);
    gapm_init(2/*RWIP_RST*/);
   return 0;
}

/*
 * @brief Create a connection to a remote LE or dual-mode device
 * @param[in] <clientId> client Id
 * @param[in] <bdAddr> remote address
 * @param[in] <isDirect> is a direct connection or a background auto connection
 * @param[in] <transport> BtTransportId  0--invalid  br_edr --1 or ble --2
 * @return 0-success, other-fail
 */
int BleGattcConnect(int clientId, const BdAddr *bdAddr, bool isDirect, int transport)
{
    ble_bdaddr_t  ble_bdaddr={ 0 };
    int status = OHOS_GATT_SUCCESS;

    if(!bdAddr)
    {
        TRACE(0,"%s() bdAddr is NULL\n",__func__);
        status = OHOS_GATT_INVALID_HANDLE;
        return -1;
    }
    const uint8_t * p= bdAddr->addr;
    TRACE(2, "%s()  isDirect:%d,transport:%d, clientId: %d addr %02X:%02X:%02X:%02X:%02X:%02X \n",
        __func__, clientId, isDirect, transport, p[0],p[1],p[2],p[3],p[4],p[5]);

    if(isDirect&&transport==OHOS_BT_TRANSPORT_LE)
    {
         memcpy(ble_bdaddr.addr,bdAddr->addr,6);
         ble_bdaddr.addr_type = 0; /*GAPM_STATIC_ADDR*/
         app_ble_start_connect(&ble_bdaddr);
     }

     return status;
}

/*
 * @brief Disconnect a remote device or cancel a pending connection
 * @param[in] <clientId> client Id
 * @param[in] <bdAddr> remote address
 * @param[in] <connId>   connection index.
 * @return 0-success, other-fail
 */
int BleGattcDisconnect(int clientId, const BdAddr *bdAddr, int connId)
{
    int status = OHOS_GATT_SUCCESS;
     if(!bdAddr)
    {
        TRACE(0,"%s() bdAddr is NULL\n",__func__);
        status = OHOS_GATT_INVALID_HANDLE;
        return -1;
    }
    TRACE(0,"%s() connId:%d\n",__func__, connId);
    app_ble_start_disconnect(connId);
    return status;
}

/*
 * @brief Send a connection parameter update request to the remote device.
 * @param[in] <bdAddr> remote address
 * @param[in] <BtGattcConnPara> connection param refer to "HCI_LE_Connection_Update".
 * @return 0-success, other-fail
 */
int BleGattcConnectParaUpdate(const BdAddr *bdAddr, BtGattcConnPara connPara)
{
    uint8_t conidx=0;
    int status = OHOS_GATT_SUCCESS;
    BdAddr dev_addr;
    if(bdAddr)
    {
        memcpy(dev_addr.addr, bdAddr->addr, OHOS_BD_ADDR_LEN);
    }
    conidx = btadapter_ble_get_conidx_by_addr(dev_addr.addr);

    TRACE(0,"%s() conidx:%d\n",__func__,conidx);
    if(conidx == 0xFF)
    {
        return OHOS_GATT_INVALID_HANDLE;
    }

    btadapter_ble_client_update_conn_para(conidx, connPara.connIntervalMin, connPara.connIntervalMax,
                                                connPara.supervisionTimeout, connPara.connLatency);
    return status;
}

/*
 * @brief This function is called to request a GATT service discovery on a GATT server.
          Optionally, the results can be filtered for a given UUID.
 * @param[in] <bdAddr> remote address
 * @param[in] <filterUuid> a UUID of the service application is interested in. If Null, discover for all services
 * @return 0-success, other-fail
 */
int BleGattcSearchServices(int connId, BtUuid filterUuid)
{
    int status = OHOS_GATT_SUCCESS;
    if(filterUuid.uuid)
    {
        TRACE(1,"%s() discover service by uuid\n",__func__);
        status = btadapter_ble_client_start_service_discovery(connId,
                                                BES_GATT_DISCOVER_SVC_PRIMARY_BY_UUID,
                                                (uint8_t *)filterUuid.uuid,
                                                filterUuid.uuidLen,
                                                0x01,
                                                0xffff
                                                );
    }
   else
   {
        TRACE(1,"%s() discover all service\n",__func__);
        status = btadapter_ble_client_start_service_discovery(connId,
                                                BES_GATT_DISCOVER_SVC_PRIMARY_ALL,
                                                NULL,
                                                0,
                                                0x01,
                                                0xffff
                                                );
    }
    //BesGattcServiceDiscoveredCallback()  => g_p_gatt_client_call_back_funcs->searchServiceCompleteCb()
    return status;
}

/*
 * @brief This function is called to read a characteristics value from the server.
 * @param[in] <connId> connection ID
 * @param[in] <handle> characteritic handle to read
 * @return 0-success, other-fail
 */
int BleGattcReadCharacteristic(int connId, int handle)
{
    uint8_t uuid[4]={0};
    int status = OHOS_GATT_SUCCESS;
    status = btadapter_ble_client_start_char_discovery(connId,
                                                                         BES_GATT_DISCOVER_CHAR_BY_UUID,
                                                                          uuid,
                                                                          2,
                                                                          handle,
                                                                          handle);
    return status;
}
/*
 * @brief This function is called to write a characteristics value to the server.
 * @param[in] <connId> connection ID
 * @param[in] <handle> characteritic handle to read
 * @param[in] <writeType> BtGattWriteType, default: write need rsp
 * @param[in] <len> the data length
 * @param[in] <value> the data to be writen
 * @return 0-success, other-fail
 */
int BleGattcWriteCharacteristic(int connId, int handle, int writeType, int len, char *value)
{
    int status = OHOS_GATT_SUCCESS;
    if(!value)
    {
        return -1;
    }
    status = btadapter_ble_client_write(connId,handle,writeType,(uint8_t *)value,len);
    return status;
}

/*
 * @brief This function is called to read a characteristics value from the server.
 * @param[in] <connId> connection ID
 * @param[in] <handle> descriptor handle to read
 * @return 0-success, other-fail
 */
int BleGattcReadDescriptor(int connId, int handle)
{
    int status = OHOS_GATT_SUCCESS;
    status = btadapter_ble_client_start_desc_discovery(connId,
                                                                         handle,
                                                                         handle);
    return status;
}

/*
 * @brief This function is called to write a descriptor value to the server.
 * @param[in] <connId> connection ID
 * @param[in] <handle> descriptor handle to read
 * @param[in] <writeType> BtGattWriteType, default: write need rsp
 * @param[in] <len> the data length
 * @param[in] <value> the data to be writen
 * @return 0-success, other-fail
 */
int BleGattcWriteDescriptor(int connId, int handle, int writeType, int len, char *value)
{
    int status = OHOS_GATT_SUCCESS;
    if(!value){
        return -1;
    }
    status = btadapter_ble_client_write(connId,handle,writeType,(const uint8_t *)value,len);
    return status;
}

/*
 * @brief This function is called to send an execute write request to the server(or cancel the prepare write).
 * @param[in] <connId> connection ID
 * @param[in] <execute> [1-execute, 0-cancel], to execute or cancel the prepare write request(s).
 * @return 0-success, other-fail
 */
int BleGattcExecuteWrite(int connId, int execute)
{
    int status = OHOS_GATT_SUCCESS;
    status = btadapter_ble_client_exe_write(connId,execute);
    if(g_p_gatt_client_call_back_funcs)
    {
        g_p_gatt_client_call_back_funcs->executeWriteCb(connId, status);
        // ->executeWriteCb(int connId, int status)
    }
    return status;
}

/*
 * @brief This function is called to configure the ATT MTU size for a connection on an LE transport.
 * @param[in] <connId> connection ID
 * @param[in] <mtuSize> attribute MTU size.
 * @return 0-success, other-fail
 */
int BleGattcConfigureMtuSize(int connId, int mtuSize)
{
    int status = OHOS_GATT_SUCCESS;
    status = btadapter_ble_client_mtu_configure(connId,mtuSize);
    if(g_p_gatt_client_call_back_funcs)
    {
        g_p_gatt_client_call_back_funcs->configureMtuSizeCb(connId, status,mtuSize);
        // ->configureMtuSizeCb(int connId, int status, int mtuSize)
    }
    return status;
}

/*
 * @brief Read the RSSI for a connected remote device.
 * @param[in] <clientId> client Id
 * @param[in] <bdAddr> remote address
 * @return 0-success, other-fail
 */
int BleGattcReadRemoteRssi(int connId, const BdAddr *bdAddr)
{
    int status = OHOS_GATT_SUCCESS;
    rx_agc_t rx_val = {0};
    //status = btadapter_ble_client_read_remote_rssi(clientId,bdAddr);
    status = bt_drv_reg_op_read_ble_rssi_in_dbm(connId, &rx_val);
    TRACE(1,"%s()  rssi:%d,connId:%d,status:%d\n",__func__,connId,rx_val.rssi,status);
    if(g_p_gatt_client_call_back_funcs)
    {
        g_p_gatt_client_call_back_funcs->readRemoteRssiCb(connId, bdAddr, rx_val.rssi, status);
        // ->(int clientId, const BdAddr *bdAddr, int rssi, int status)
    }
    return status;
}

/*
 * @brief Enable or disable notifications/indications for a given characteristic.
 * @param[in] <clientId> client Id
 * @param[in] <bdAddr> remote address
 * @param[in] <handle> characteristic handle
 * @param[in] <enable> 1-register, 0-deregister
 * @return 0-success, other-fail
 */
int BleGattcRegisterNotifications(int clientId, const BdAddr *bdAddr, int handle, int enable)
{
    int status = OHOS_GATT_SUCCESS;
    //todo
    return status;
}

/*
 * @brief Callback invoked for gatt client function
 * @param[in] <BtGattClientCallbacks> Callback funcs
 * @return 0-success, other-fail
 */
int BleGattcRegisterCallbacks(BtGattClientCallbacks *func)
{
    if(func)
    {
        TRACE(1, "%s()  func :%p\n", __func__,func);
        g_p_gatt_client_call_back_funcs = func;
    }
    else
    {
        TRACE(1, "%s()  func is NULL\n", __func__);
    }
    return 0;
}


void BleGattcInitCallback(uint16_t status)
{
   TRACE(1,"%s() g_p_gatt_client_call_back_funcs %p,status:%d,user_lid:%d\n",__func__,
    g_p_gatt_client_call_back_funcs,status,g_gatt_client_task_env.user_lid);

    if(g_p_gatt_client_call_back_funcs)
    {
        g_p_gatt_client_call_back_funcs->registerClientCb(status, g_gatt_client_task_env.user_lid, &g_appUuid);
        // ->registerClientCb(int status, int clientId, const BtUuid *appUuid)  this function defined by upper layer
    }

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
    BdAddr BtAddr = {0};
    int32_t status = OHOS_BT_STATUS_SUCCESS;

    TRACE(1, "%s %d", __func__, event);

    if(bdAddr)
    {
        memcpy(BtAddr.addr,bdAddr,6);
    }
    else
    {
       TRACE(1, "%s bdAddr is NULL\n", __func__);
    }
    if(g_p_gatt_client_call_back_funcs)
    {
        if(BES_LINK_EVENT_GATT_CONNECT == event)
        {
            g_p_gatt_client_call_back_funcs->connectClientCb(connId,status, g_gatt_client_task_env.user_lid, &BtAddr);
            // ConnectClientFunCb(int connId, int status, int clientId, const BdAddr *bdAddr)
        }
        else if(BES_LINK_EVENT_GATT_DISCONNECT == event)
        {
            g_p_gatt_client_call_back_funcs->disconnectCb(connId,status, g_gatt_client_task_env.user_lid, &BtAddr);
            // DisconnectClientFunCb(int connId, int status, int clientId, const BdAddr *bdAddr)
        }

    }
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
    int32_t status = OHOS_BT_STATUS_SUCCESS;
    TRACE(1, "%s()\n", __func__);
    if(g_p_gatt_client_call_back_funcs)
    {
        g_p_gatt_client_call_back_funcs->connectParaUpdateCb(connId, interval,latency,supertimeout,status);
        // ConnectParaUpdateFunCb(int connId, int interval, int latency, int timeout, int status)
    }
}
/**
 * @brief Get the services list
 * @param[in] gattServices Services list
 * @return void
 */
void BleGattcServiceDiscoveredCompleteCallback(int32_t connId, bes_gatt_status_e status)
{
    TRACE(1, "%s()  status :%d\n", __func__,status);
    if(g_p_gatt_client_call_back_funcs)
    {
        g_p_gatt_client_call_back_funcs->searchServiceCompleteCb(connId, status);
    }
}

/**
 * @brief Get the services list . Service discover results are saved in gGattClientDb[i].svc_content
 * @param[in] gattServices Services list
 * @return void
 */
void BleGattcServiceDiscoveredCallback(uint16_t serv_index, bes_gatt_services_t* gattServices)
{
    int i=0;
    if (gattServices == NULL){
        TRACE(1, "%s()  gattServices is NULL\n", __func__);
        return;
    }
    //gattServices  saved in the global variable gGattClientDb[i].svc_content
    TRACE(1,"%s(), service count:%d \n",__func__,gattServices->count);

    for(i=0;i<gattServices->count;i++)
    {
        TRACE(1," id:0x%02x  type:%x,handle:0x%x, start_handle:0x%x,end_handle:0x%x, proerties:0x%x uuid_len:%d,uuid:\n",
            gattServices->pElem[i].id,gattServices->pElem[i].type,gattServices->pElem[i].handle,
            gattServices->pElem[i].start_handle,gattServices->pElem[i].end_handle,
            gattServices->pElem[i].properties,gattServices->pElem[i].uuid_len);
        DUMP8("0x%02x ",gattServices->pElem[i].uuid,gattServices->pElem[i].uuid_len);
    }

    //TRACE(3, "%s hd:%x sthd:%x", __func__, gattServices->pElem[0].handle, gattServices->pElem[0].start_handle);
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
     BtGattReadData readData;

     readData.handle = handle;
     readData.data = data;
     readData.dataLen = size;
    if(g_p_gatt_client_call_back_funcs)
    {
        g_p_gatt_client_call_back_funcs->readCharacteristicCb(connId, status,&readData);
        // ->readCharacteristicCb(int connId, int status, BtGattReadData *readData)        defined by upper layer
    }
    TRACE(1, "%s hd:%x", __func__, handle);
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
    if(g_p_gatt_client_call_back_funcs)
    {
        g_p_gatt_client_call_back_funcs->writeCharacteristicCb(connId, status,handle);
        // ->writeCharacteristicCb(int connId, int status, unsigned short handle)
    }
    TRACE(1, "%s hd:%x", __func__, handle);
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
    BtGattReadData readData ={
        .handle = handle,
        .dataLen = size,
        .data = data
    };

     if(g_p_gatt_client_call_back_funcs)
    {
        g_p_gatt_client_call_back_funcs->readDescriptorCb(connId, status,&readData);
        // ->readDescriptorCb(int connId, int status, BtGattReadData *readData)
    }
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
    if(g_p_gatt_client_call_back_funcs)
    {
        g_p_gatt_client_call_back_funcs->writeDescriptorCb(connId, status,handle);
        // ->writeDescriptorCb(int connId, int status, unsigned short handle)
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
    TRACE(1, "%s hd:%x", __func__, handle);
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
    BtGattNotifyData notifyData ={
        .handle = handle,
        .isNotify = registered,
    };
    TRACE(1, "%s hd:%x", __func__, handle);
    if(g_p_gatt_client_call_back_funcs)
    {
        g_p_gatt_client_call_back_funcs->notificationCb(connId, notifyData);
        // ->(int connId, BtGattNotifyData notifyData)
    }
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
