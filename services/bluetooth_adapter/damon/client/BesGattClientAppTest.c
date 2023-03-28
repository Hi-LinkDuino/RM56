#ifdef BLE_V2
#include "string.h"
#include "plat_types.h"
#include "hal_trace.h"
#include "ohos_bt_gatt_client.h"
#include "btadapter_ble_client.h"
#include "BesGattClientAppTest.h"
#include "stdlib.h"
#ifdef BLE_GATT_CLIENT_TEST
#include "at.h"
#endif
BtUuid appUuid;
int g_gattctest_clientId = 0;
int g_gattctest_connId = 0;

void RegisterClientFunCb(int status, int clientId, const BtUuid *appUuid);
void  ConnectClientFunCb(int connId, int status, int clientId, const BdAddr *bdAddr);
void  DisconnectClientFunCb(int connId, int status, int clientId, const BdAddr *bdAddr);
void  ConnectParaUpdateFunCb(int connId, int interval, int latency, int timeout, int status);
void  SearchServiceCompleteFunCb(int connId, int status);
void  ReadCharacteristicFunCb(int connId, int status, BtGattReadData *readData);
void  WriteCharacteristicFunCb(int connId, int status, unsigned short handle);
void  ReadDescriptorFunCb(int connId, int status, BtGattReadData *readData);
void  WriteDescriptorFunCb(int connId, int status, unsigned short handle);
void  ExecuteWriteFunCb(int connId, int status);
void  ConfigureMtuSizeFunCb(int connId, int status, int mtuSize);
void  ReadRemoteRssiFunCb(int clientId, const BdAddr *bdAddr, int rssi, int status);
void  RegisterNotificationFunCb(int connId, int registered, int status, unsigned short handle);
void  NotificationFunCb(int connId, BtGattNotifyData notifyData);

/* FunCb invoked in response to BleGattcRegister */
void RegisterClientFunCb(int status, int clientId, const BtUuid *appUuid)
{
    g_gattctest_clientId = clientId;
    TRACE(1,"%s() g_gattctest_clientId :%x, status:%d\n",__func__,g_gattctest_clientId,status);
}

/* FunCb invoked in response to BleGattcConnect */
void  ConnectClientFunCb(int connId, int status, int clientId, const BdAddr *bdAddr)
{
    g_gattctest_connId = connId;
    TRACE(1,"%s() g_gattctest_connId :%x, status:%d, clientId:%d\n",__func__,g_gattctest_connId,status,clientId);
}
/* FunCb invoked in response to BleGattcDisconnect */
void  DisconnectClientFunCb(int connId, int status, int clientId, const BdAddr *bdAddr)
{
    TRACE(1,"%s() connId :%x, status:%d, clientId:%d,addr:\n",__func__,connId,status,clientId);
    if(bdAddr)
    {
        DUMP8("0x%02x ",bdAddr->addr,6);
    }
}
/* FunCb invoked when the connection parameters for a given connection changed */
void  ConnectParaUpdateFunCb(int connId, int interval, int latency, int timeout, int status)
{
    TRACE(1,"%s() connId :%x, status:%d, interval:%d, latency:%d, timeout:%d, \n",
                __func__,connId,status,interval, latency, timeout);
}
/* Invoked in response to BleGattcSearchServices when the GATT service discovery has been completed */
void  SearchServiceCompleteFunCb(int connId, int status)
{

}
/* Reports result of a GATT read operation */
void  ReadCharacteristicFunCb(int connId, int status, BtGattReadData *readData)
{

}
/* GATT write characteristic operation FunCb */
void  WriteCharacteristicFunCb(int connId, int status, unsigned short handle)
{

}
/* FunCb invoked in response to BleGattcReadDescriptor */
void  ReadDescriptorFunCb(int connId, int status, BtGattReadData *readData)
{

}
/* FunCb invoked in response to BleGattcWriteDescriptor */
void  WriteDescriptorFunCb(int connId, int status, unsigned short handle)
{

}
/* GATT execute prepared write FunCb */
void  ExecuteWriteFunCb(int connId, int status)
{

}
/* FunCb invoked when the MTU size for a given connection changes */
void  ConfigureMtuSizeFunCb(int connId, int status, int mtuSize)
{

}
/* FunCb invoked in response to BleGattcReadRemoteRssi */
void  ReadRemoteRssiFunCb(int clientId, const BdAddr *bdAddr, int rssi, int status)
{
    TRACE(1,"%s()  rssi:%d,status:%d\n",__func__,rssi,status);
}
/* FunCb invoked in response to BleGattcRegisterNotifications, register/deregister */
void  RegisterNotificationFunCb(int connId, int registered, int status, unsigned short handle)
{

}
/* FunCb invoked when a remote device sends a notification/indication that a client has registered for */
void  NotificationFunCb(int connId, BtGattNotifyData notifyData)
{

}
static BtGattClientCallbacks g_gatt_client_call_back_funcs = {
    .registerClientCb = RegisterClientFunCb,
    .connectClientCb = ConnectClientFunCb,
    .disconnectCb = DisconnectClientFunCb,
    .connectParaUpdateCb = ConnectParaUpdateFunCb,
    .searchServiceCompleteCb = SearchServiceCompleteFunCb,
    .readCharacteristicCb = ReadCharacteristicFunCb,
    .writeCharacteristicCb = WriteCharacteristicFunCb,
    .readDescriptorCb = ReadDescriptorFunCb,
    .writeDescriptorCb = WriteDescriptorFunCb,
    .executeWriteCb = ExecuteWriteFunCb,
    .configureMtuSizeCb = ConfigureMtuSizeFunCb,
    .readRemoteRssiCb = ReadRemoteRssiFunCb,
    .registerNotificationCb = RegisterNotificationFunCb,
    .notificationCb = NotificationFunCb,
};
void BleGattcRegisterFunsCallbacks(void)
{
    TRACE(1,"%s()\n",__func__);
    BleGattcRegisterCallbacks(&g_gatt_client_call_back_funcs);
}

#ifdef BLE_GATT_CLIENT_TEST
int gatt_client_test(size_t argc, char *argv[])
{
    static BdAddr bdAddr = {.addr = {0x66,0x55,0x44,0x33,0x22,0x11}};
    BtGattcConnPara connPara ={
        .connIntervalMin = 10,
        .connIntervalMax = 20,
        .connLatency =2,
        .supervisionTimeout =50,
        .minConnectionEventLen =20,
        .maxConnectionEventLen =250
    };
    char char_value[4] = {0x11,0x22,0x33,0x44};
    char desc_value[4] = {0x88,0x77,0x66,0x55};
    int execute_write = 0;
    int mtuSize = 30;
    int enable = 0;
    BtUuid filterUuid = {0};
    bool isDirect = 1;
    int transport = 2;
    int handle = 0x100;
    int writeType = 0, len = 0;

    if(argc < 1)
    {
         TRACE(1,"btrf gatt_client  cmd_xxx\n");
        return -1;
    }
     TRACE(1,"%s ... %d, %s ,%s \n",__func__,argc,argv[0],argc>1?argv[1]:"NULL");

    if(strncmp(argv[0], "reg", strlen(argv[0])) == 0)
     {
        if(argc == 2 && atoi(argv[1]) == 1)
        {
            return 0;
        }
        BleGattcRegisterFunsCallbacks();
        BleGattcRegister(appUuid);

        TRACE(1,"BleGattcRegister  end\n");

     }
     else if(strncmp(argv[0], "unreg", strlen(argv[0])) == 0)
     {
        BleGattcUnRegister(g_gattctest_clientId);
     }
     else if(strncmp(argv[0], "conn", strlen(argv[0])) == 0)
     {

          if(argc > 1&&(strlen(argv[1])) == 17)
         {
            str2mac(argv[1],bdAddr.addr);
            TRACE(1,"addr: %x:%x:%x:%x:%x:%x\n",bdAddr.addr[0],bdAddr.addr[1],bdAddr.addr[2],
                bdAddr.addr[3],bdAddr.addr[4],bdAddr.addr[5]);
         }
         if(argc > 2)
            isDirect = atoi(argv[2]);
         if(argc > 3)
            transport = atoi(argv[3]);
         TRACE(1,"gatt client connect isDirect=%d,transport=%d \n",isDirect,transport);
        if(argc > 4&&atoi(argv[4]) == 1)
        {
            return 0;
        }
        BleGattcConnect(g_gattctest_clientId,&bdAddr,isDirect,transport);
     }
     else if(strncmp(argv[0], "discon", strlen(argv[0])) == 0)
     {
        BleGattcDisconnect(g_gattctest_clientId,&bdAddr,g_gattctest_connId);
     }
     else if(strncmp(argv[0], "conupdate", strlen(argv[0])) == 0)
     {
        BleGattcConnectParaUpdate(&bdAddr,connPara);
     }
     else if(strncmp(argv[0], "searchservice", strlen(argv[0])) == 0)
     {
        BleGattcSearchServices(g_gattctest_connId,filterUuid);
     }
     else if(strncmp(argv[0], "readchara", strlen(argv[0])) == 0)
     {
        if(argc > 1)
            handle = atoi(argv[1]);
        BleGattcReadCharacteristic(g_gattctest_connId,handle);
     }
     else if(strncmp(argv[0], "writechara", strlen(argv[0])) == 0)
     {
        if(argc > 1)
            handle = atoi(argv[1]);
        BleGattcWriteCharacteristic(g_gattctest_connId,handle,BES_GATT_WRITE_WITH_RESPONSE,4,char_value);
     }
     else if(strncmp(argv[0], "readdesc", strlen(argv[0])) == 0)
     {
        if(argc > 1)
            handle = atoi(argv[1]);
        BleGattcReadDescriptor(g_gattctest_connId,handle);
     }
     else if(strncmp(argv[0], "writedesc", strlen(argv[0])) == 0)
     {
        if(argc > 1)
            handle = atoi(argv[1]);
        if(argc > 2)
            writeType = atoi(argv[2]);
        if(argc > 3)
            len =  atoi(argv[3]);
        BleGattcWriteDescriptor(g_gattctest_connId,handle,BES_GATT_WRITE_WITH_RESPONSE,4,desc_value);
     }
     else if(strncmp(argv[0], "writexe", strlen(argv[0])) == 0)
     {
        if(argc > 1)
        execute_write = atoi(argv[1]);
        BleGattcExecuteWrite(g_gattctest_connId,execute_write);
     }
     else if(strncmp(argv[0], "configmtu", strlen(argv[0])) == 0)
     {
        if(argc > 1)
            mtuSize = atoi(argv[1]);
        TRACE(1,"%s()  mtuSize=%d\n ",__func__,mtuSize);
        BleGattcConfigureMtuSize(g_gattctest_connId,mtuSize);
     }
     else if(strncmp(argv[0], "readrssi", strlen(argv[0])) == 0)
     {
        BleGattcReadRemoteRssi(g_gattctest_connId,&bdAddr);
     }
     else if(strncmp(argv[0], "regnotify", strlen(argv[0])) == 0)
     {
        if(argc > 1)
        {
            enable = atoi(argv[1]);
            TRACE(1,"%s() regnotify indicate argv[1]:%s, enable=%d\n ",__func__,argv[1],enable);
        }
        if(argc > 2)
            handle = atoi(argv[2]);

        BleGattcRegisterNotifications(g_gattctest_clientId,&bdAddr,handle,enable);
     }
     else
     {
       TRACE(1,"%s() cmd not support!\n",__func__);
     }
     return 0;
}
#endif
#endif

