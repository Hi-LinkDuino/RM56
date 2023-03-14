/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "wifi_hal_crpc_server.h"
#include "wifi_hal_crpc_base.h"
#include "wifi_hal_crpc_chip.h"
#include "wifi_hal_crpc_supplicant.h"
#include "wifi_hal_crpc_sta.h"
#include "wifi_hal_crpc_ap.h"
#include "wifi_hal_crpc_common.h"
#include "wifi_hal_crpc_p2p.h"
#include "securec.h"
#include "wifi_log.h"
#include "wifi_hal_common_func.h"

#undef LOG_TAG
#define LOG_TAG "WifiHalCrpcServer"

/* Defines the mapping between global function names and functions. */
static WifiHalRpcFunc *g_rpcFuncHandle = NULL;
static RpcServer *g_rpcServer = NULL;

void SetRpcServerInited(RpcServer *server)
{
    g_rpcServer = server;
    return;
}

RpcServer *GetRpcServer(void)
{
    return g_rpcServer;
}

static int GetPos(const char *name)
{
    int total = 0;
    while (*name) {
        total += *name;
        ++name;
    }
    if (total < 0) {
        total *= -1;
    }
    return total % RPC_FUNC_NUM;
}

static int PushRpcFunc(const char *name, Rpcfunc func)
{
    if (g_rpcFuncHandle == NULL || name == NULL || func == NULL) {
        return -1;
    }
    int pos = GetPos(name);
    if (g_rpcFuncHandle[pos].func == NULL) {
        StrSafeCopy(g_rpcFuncHandle[pos].funcname, sizeof(g_rpcFuncHandle[pos].funcname), name);
        g_rpcFuncHandle[pos].func = func;
    } else {
        WifiHalRpcFunc *p = g_rpcFuncHandle + pos;
        while (p->next != NULL) {
            p = p->next;
        }
        WifiHalRpcFunc *q = (WifiHalRpcFunc *)calloc(1, sizeof(WifiHalRpcFunc));
        if (q == NULL) {
            return -1;
        }
        StrSafeCopy(q->funcname, sizeof(q->funcname), name);
        q->func = func;
        q->next = NULL;
        p->next = q;
    }
    return 0;
}

static int InitRpcFuncMapBase(void)
{
    int ret = 0;
    ret += PushRpcFunc("GetName", RpcGetName);
    ret += PushRpcFunc("GetType", RpcGetType);
    return ret;
}

static int InitRpcFuncMapChip(void)
{
    int ret = 0;
    ret += PushRpcFunc("GetWifiChip", RpcGetWifiChip);
    ret += PushRpcFunc("GetWifiChipIds", RpcGetWifiChipIds);
    ret += PushRpcFunc("GetChipId", RpcGetChipId);
    ret += PushRpcFunc("CreateIface", RpcCreateIface);
    ret += PushRpcFunc("GetIface", RpcGetIface);
    ret += PushRpcFunc("GetIfaceNames", RpcGetIfaceNames);
    ret += PushRpcFunc("RemoveIface", RpcRemoveIface);
    ret += PushRpcFunc("GetCapabilities", RpcGetCapabilities);
    ret += PushRpcFunc("GetSupportedComboModes", RpcGetSupportedComboModes);
    ret += PushRpcFunc("ConfigComboModes", RpcConfigComboModes);
    ret += PushRpcFunc("GetComboModes", RpcGetComboModes);
    ret += PushRpcFunc("RequestFirmwareDebugDump", RpcRequestFirmwareDebugDump);
    ret += PushRpcFunc("SetPowerMode", RpcSetPowerMode);
    return ret;
}

static int InitRpcFuncMapSupplicant(void)
{
    int ret = 0;
    ret += PushRpcFunc("StartSupplicant", RpcStartSupplicant);
    ret += PushRpcFunc("StopSupplicant", RpcStopSupplicant);
    ret += PushRpcFunc("ConnectSupplicant", RpcConnectSupplicant);
    ret += PushRpcFunc("DisconnectSupplicant", RpcDisconnectSupplicant);
    ret += PushRpcFunc("RequestToSupplicant", RpcRequestToSupplicant);
    ret += PushRpcFunc("SetPowerSave", RpcSetPowerSave);
    ret += PushRpcFunc("WpaSetCountryCode", RpcWpaSetCountryCode);
    ret += PushRpcFunc("WpaGetCountryCode", RpcWpaGetCountryCode);
    return ret;
}

static int InitRpcFuncMapSta(void)
{
    int ret = 0;
    ret += PushRpcFunc("Start", RpcStart);
    ret += PushRpcFunc("Stop", RpcStop);
    ret += PushRpcFunc("StartScan", RpcStartScan);
    ret += PushRpcFunc("GetScanInfos", RpcGetScanInfos);
    ret += PushRpcFunc("StartPnoScan", RpcStartPnoScan);
    ret += PushRpcFunc("StopPnoScan", RpcStopPnoScan);
    ret += PushRpcFunc("Connect", RpcConnect);
    ret += PushRpcFunc("Reconnect", RpcReconnect);
    ret += PushRpcFunc("Reassociate", RpcReassociate);
    ret += PushRpcFunc("Disconnect", RpcDisconnect);
    ret += PushRpcFunc("GetStaCapabilities", RpcGetStaCapabilities);
    ret += PushRpcFunc("GetDeviceMacAddress", RpcGetDeviceMacAddress);
    ret += PushRpcFunc("GetFrequencies", RpcGetFrequencies);
    ret += PushRpcFunc("SetAssocMacAddr", RpcSetAssocMacAddr);
    ret += PushRpcFunc("SetScanningMacAddress", RpcSetScanningMacAddress);
    ret += PushRpcFunc("DeauthLastRoamingBssid", RpcDeauthLastRoamingBssid);
    ret += PushRpcFunc("GetSupportFeature", RpcGetSupportFeature);
    ret += PushRpcFunc("RunCmd", RpcRunCmd);
    ret += PushRpcFunc("SetWifiTxPower", RpcSetWifiTxPower);
    ret += PushRpcFunc("RemoveNetwork", RpcRemoveNetwork);
    ret += PushRpcFunc("AddNetwork", RpcAddNetwork);
    ret += PushRpcFunc("EnableNetwork", RpcEnableNetwork);
    ret += PushRpcFunc("DisableNetwork", RpcDisableNetwork);
    ret += PushRpcFunc("SetNetwork", RpcSetNetwork);
    ret += PushRpcFunc("SaveNetworkConfig", RpcSaveNetworkConfig);
    ret += PushRpcFunc("StartWpsPbcMode", RpcStartWpsPbcMode);
    ret += PushRpcFunc("StartWpsPinMode", RpcStartWpsPinMode);
    ret += PushRpcFunc("StopWps", RpcStopWps);
    ret += PushRpcFunc("GetRoamingCapabilities", RpcGetRoamingCapabilities);
    ret += PushRpcFunc("SetRoamConfig", RpcSetRoamConfig);
    ret += PushRpcFunc("WpaGetNetwork", RpcWpaGetNetwork);
    ret += PushRpcFunc("WpaAutoConnect", RpcWpaAutoConnect);
    ret += PushRpcFunc("WpaBlocklistClear", RpcWpaBlocklistClear);
    ret += PushRpcFunc("GetNetworkList", RpcGetNetworkList);
    ret += PushRpcFunc("GetConnectSignalInfo", RpcGetConnectSignalInfo);
    return ret;
}

static int InitRpcFuncMapAp(void)
{
    int ret = 0;
    ret += PushRpcFunc("StartSoftAp", RpcStartSoftAp);
    ret += PushRpcFunc("StopSoftAp", RpcStopSoftAp);
    ret += PushRpcFunc("SetHostapdConfig", RpcSetHostapdConfig);
    ret += PushRpcFunc("GetStaInfos", RpcGetStaInfos);
    ret += PushRpcFunc("SetCountryCode", RpcSetCountryCode);
    ret += PushRpcFunc("SetMacFilter", RpcSetMacFilter);
    ret += PushRpcFunc("DelMacFilter", RpcDelMacFilter);
    ret += PushRpcFunc("DisassociateSta", RpcDisassociateSta);
    ret += PushRpcFunc("GetValidFrequenciesForBand", RpcGetValidFrequenciesForBand);
    return ret;
}

static int InitRpcFuncMapCommon(void)
{
    int ret = 0;
    ret += PushRpcFunc("RegisterEventCallback", RpcRegisterEventCallback);
    ret += PushRpcFunc("UnRegisterEventCallback", RpcUnRegisterEventCallback);
    ret += PushRpcFunc("NotifyClear", RpcNotifyClear);
    return ret;
}

static int InitRpcFuncMapP2p(void)
{
    int ret = 0;
    ret += PushRpcFunc("P2pStart", RpcP2pStart);
    ret += PushRpcFunc("P2pStop", RpcP2pStop);
    ret += PushRpcFunc("P2pSetRandomMac", RpcP2pSetRandomMac);
    ret += PushRpcFunc("P2pSetDeviceName", RpcP2pSetDeviceName);
    ret += PushRpcFunc("P2pSetSsidPostfixName", RpcP2pSetSsidPostfixName);
    ret += PushRpcFunc("P2pSetWpsDeviceType", RpcP2pSetWpsDeviceType);
    ret += PushRpcFunc("P2pSetWpsSecondaryDeviceType", RpcP2pSetWpsSecondaryDeviceType);
    ret += PushRpcFunc("P2pSetWpsConfigMethods", RpcP2pSetWpsConfigMethods);
    ret += PushRpcFunc("P2pGetDeviceAddress", RpcP2pGetDeviceAddress);
    ret += PushRpcFunc("P2pFlush", RpcP2pFlush);
    ret += PushRpcFunc("P2pFlushService", RpcP2pFlushService);
    ret += PushRpcFunc("P2pSaveConfig", RpcP2pSaveConfig);
    ret += PushRpcFunc("P2pSetupWpsPbc", RpcP2pSetupWpsPbc);
    ret += PushRpcFunc("P2pSetupWpsPin", RpcP2pSetupWpsPin);
    ret += PushRpcFunc("P2pRemoveNetwork", RpcP2pRemoveNetwork);
    ret += PushRpcFunc("P2pListNetworks", RpcP2pListNetworks);
    ret += PushRpcFunc("P2pSetGroupMaxIdle", RpcP2pSetGroupMaxIdle);
    ret += PushRpcFunc("P2pSetPowerSave", RpcP2pSetPowerSave);
    ret += PushRpcFunc("P2pSetWfdEnable", RpcP2pSetWfdEnable);
    ret += PushRpcFunc("P2pSetWfdDeviceConfig", RpcP2pSetWfdDeviceConfig);
    ret += PushRpcFunc("P2pStartFind", RpcP2pStartFind);
    ret += PushRpcFunc("P2pStopFind", RpcP2pStopFind);
    ret += PushRpcFunc("P2pSetExtListen", RpcP2pSetExtListen);
    ret += PushRpcFunc("P2pSetListenChannel", RpcP2pSetListenChannel);
    ret += PushRpcFunc("P2pConnect", RpcP2pConnect);
    ret += PushRpcFunc("P2pCancelConnect", RpcP2pCancelConnect);
    ret += PushRpcFunc("P2pProvisionDiscovery", RpcP2pProvisionDiscovery);
    ret += PushRpcFunc("P2pAddGroup", RpcP2pAddGroup);
    ret += PushRpcFunc("P2pRemoveGroup", RpcP2pRemoveGroup);
    ret += PushRpcFunc("P2pInvite", RpcP2pInvite);
    ret += PushRpcFunc("P2pReinvoke", RpcP2pReinvoke);
    ret += PushRpcFunc("P2pGetGroupCapability", RpcP2pGetGroupCapability);
    ret += PushRpcFunc("P2pAddService", RpcP2pAddService);
    ret += PushRpcFunc("P2pRemoveService", RpcP2pRemoveService);
    ret += PushRpcFunc("P2pReqServiceDiscovery", RpcP2pReqServiceDiscovery);
    ret += PushRpcFunc("P2pCancelServiceDiscovery", RpcP2pCancelServiceDiscovery);
    ret += PushRpcFunc("P2pSetMiracastType", RpcP2pSetMiracastType);
    ret += PushRpcFunc("P2pRespServerDiscovery", RpcP2pRespServerDiscovery);
    ret += PushRpcFunc("P2pSetServDiscExternal", RpcP2pSetServDiscExternal);
    ret += PushRpcFunc("P2pSetPersistentReconnect", RpcP2pSetPersistentReconnect);
    ret += PushRpcFunc("P2pGetPeer", RpcP2pGetPeer);
    ret += PushRpcFunc("P2pGetFrequencies", RpcP2pGetFrequencies);
    ret += PushRpcFunc("P2pSetGroupConfig", RpcP2pSetGroupConfig);
    ret += PushRpcFunc("P2pGetGroupConfig", RpcP2pGetGroupConfig);
    ret += PushRpcFunc("P2pAddNetwork", RpcP2pAddNetwork);
    ret += PushRpcFunc("P2pHid2dConnect", RpcP2pHid2dConnect);
    return ret;
}

int InitRpcFunc(void)
{
    if (g_rpcFuncHandle != NULL) {
        return 0;
    }
    g_rpcFuncHandle = (WifiHalRpcFunc *)calloc(RPC_FUNC_NUM, sizeof(WifiHalRpcFunc));
    if (g_rpcFuncHandle == NULL) {
        return -1;
    }

    int ret = 0;
    ret += InitRpcFuncMapBase();
    ret += InitRpcFuncMapChip();
    ret += InitRpcFuncMapSupplicant();
    ret += InitRpcFuncMapSta();
    ret += InitRpcFuncMapAp();
    ret += InitRpcFuncMapCommon();
    ret += InitRpcFuncMapP2p();
    if (ret < 0) {
        return -1;
    }

    if (InitCallbackMsg() < 0) {
        return -1;
    }
    return 0;
}

void ReleaseRpcFunc(void)
{
    for (int i = 0; i < RPC_FUNC_NUM; ++i) {
        WifiHalRpcFunc *p = g_rpcFuncHandle[i].next;
        while (p != NULL) {
            WifiHalRpcFunc *q = p->next;
            free(p);
            p = q;
        }
    }
    free(g_rpcFuncHandle);
    g_rpcFuncHandle = NULL;
    ReleaseCallbackMsg();
    return;
}

Rpcfunc GetRpcFunc(const char *func)
{
    if (g_rpcFuncHandle == NULL || func == NULL) {
        return NULL;
    }
    int pos = GetPos(func);
    WifiHalRpcFunc *p = g_rpcFuncHandle + pos;
    while (p && strcmp(p->funcname, func) != 0) {
        p = p->next;
    }
    if (p == NULL) {
        return NULL;
    }
    return p->func;
}

/* Processing client requests */
int OnTransact(RpcServer *server, Context *context)
{
    if ((server == NULL) || (context == NULL)) {
        return -1;
    }

    char func[RPC_FUNCNAME_MAX_LEN] = {0};
    int ret = ReadFunc(context, func, RPC_FUNCNAME_MAX_LEN);
    if (ret < 0) {
        return -1;
    }
    LOGI("run %{public}s", func);
    Rpcfunc pFunc = GetRpcFunc(func);
    if (pFunc == NULL) {
        LOGD("unsupport function[%{public}s]", func);
        WriteBegin(context, 0);
        WriteInt(context, WIFI_HAL_FAILED);
        WriteStr(context, "unsupport function");
        WriteEnd(context);
    } else {
        ret = pFunc(server, context);
        if (ret < 0) {
            WriteBegin(context, 0);
            WriteInt(context, WIFI_HAL_FAILED);
            WriteStr(context, "server deal failed!");
            WriteEnd(context);
        }
    }
    return 0;
}

/* Defines the bidirectional list of global callback event parameters. */
static WifiHalEventCallback *g_wifiHalEventCallback = NULL;

int InitCallbackMsg(void)
{
    if (g_wifiHalEventCallback != NULL) {
        return 0;
    }
    g_wifiHalEventCallback = (WifiHalEventCallback *)calloc(1, sizeof(WifiHalEventCallback));
    if (g_wifiHalEventCallback == NULL) {
        return -1;
    }
    pthread_mutex_init(&g_wifiHalEventCallback->mutex, NULL);
    for (int i = 0; i < WIFI_HAL_MAX_EVENT - WIFI_FAILURE_EVENT; ++i) {
        g_wifiHalEventCallback->cbmsgs[i].pre = g_wifiHalEventCallback->cbmsgs + i;
        g_wifiHalEventCallback->cbmsgs[i].next = g_wifiHalEventCallback->cbmsgs + i;
    }
    return 0;
}

void ReleaseCallbackMsg(void)
{
    if (g_wifiHalEventCallback == NULL) {
        return;
    }
    for (int i = 0; i < WIFI_HAL_MAX_EVENT - WIFI_FAILURE_EVENT; ++i) {
        WifiHalEventCallbackMsg *head = g_wifiHalEventCallback->cbmsgs + i;
        WifiHalEventCallbackMsg *p = head->next;
        while (p != head) {
            WifiHalEventCallbackMsg *q = p->next;
            free(p);
            p = q;
        }
    }
    pthread_mutex_destroy(&g_wifiHalEventCallback->mutex);
    free(g_wifiHalEventCallback);
    g_wifiHalEventCallback = NULL;
    return;
}

int PushBackCallbackMsg(int event, WifiHalEventCallbackMsg *msg)
{
    if (g_wifiHalEventCallback == NULL || event >= WIFI_HAL_MAX_EVENT || event < WIFI_FAILURE_EVENT || msg == NULL) {
        return -1;
    }
    int pos = event - WIFI_FAILURE_EVENT;
    pthread_mutex_lock(&g_wifiHalEventCallback->mutex);
    WifiHalEventCallbackMsg *head = g_wifiHalEventCallback->cbmsgs + pos;
    if (head->next == head) { /* Empty Queue */
        msg->pre = head;
        head->next = msg;
        msg->next = head;
        head->pre = msg;
    } else {
        msg->pre = head->pre;
        head->pre->next = msg;
        msg->next = head;
        head->pre = msg;
    }
    pthread_mutex_unlock(&g_wifiHalEventCallback->mutex);
    return 0;
}

int PopBackCallbackMsg(int event)
{
    if (g_wifiHalEventCallback == NULL || event >= WIFI_HAL_MAX_EVENT || event < WIFI_FAILURE_EVENT) {
        return -1;
    }
    int pos = event - WIFI_FAILURE_EVENT;
    pthread_mutex_lock(&g_wifiHalEventCallback->mutex);
    WifiHalEventCallbackMsg *head = g_wifiHalEventCallback->cbmsgs + pos;
    if (head->next != head) { /* The queue is not empty. */
        WifiHalEventCallbackMsg *tail = head->pre;
        head->pre = tail->pre;
        tail->pre->next = head;
    }
    pthread_mutex_unlock(&g_wifiHalEventCallback->mutex);
    return 0;
}

WifiHalEventCallbackMsg *FrontCallbackMsg(int event)
{
    if (g_wifiHalEventCallback == NULL || event >= WIFI_HAL_MAX_EVENT || event < WIFI_FAILURE_EVENT) {
        return NULL;
    }
    int pos = event - WIFI_FAILURE_EVENT;
    WifiHalEventCallbackMsg *head = g_wifiHalEventCallback->cbmsgs + pos;
    if (head->next != head) { /* The queue is not empty. */
        return head->next;
    } else {
        return NULL;
    }
}

int PopFrontCallbackMsg(int event)
{
    if (g_wifiHalEventCallback == NULL || event >= WIFI_HAL_MAX_EVENT || event < WIFI_FAILURE_EVENT) {
        return -1;
    }
    int pos = event - WIFI_FAILURE_EVENT;
    pthread_mutex_lock(&g_wifiHalEventCallback->mutex);
    WifiHalEventCallbackMsg *head = g_wifiHalEventCallback->cbmsgs + pos;
    if (head->next != head) { /* The queue is not empty. */
        WifiHalEventCallbackMsg *p = head->next;
        head->next = p->next;
        p->next->pre = head;
        free(p);
        p = NULL;
    }
    pthread_mutex_unlock(&g_wifiHalEventCallback->mutex);
    return 0;
}

/* Processing callback messages */
static void DealCommonCbk(int event, Context *context)
{
    WifiHalEventCallbackMsg *cbmsg = FrontCallbackMsg(event);
    if (cbmsg != NULL) {
        WriteInt(context, cbmsg->msg.scanStatus);
    }
    return;
}

static void DealIfaceCbk(int event, Context *context)
{
    WifiHalEventCallbackMsg *cbmsg = FrontCallbackMsg(event);
    if (cbmsg != NULL) {
        WriteInt(context, cbmsg->msg.ifMsg.type);
        WriteStr(context, cbmsg->msg.ifMsg.ifname);
    }
    return;
}

static void DealConnectionChangedCbk(int event, Context *context)
{
    WifiHalEventCallbackMsg *cbmsg = FrontCallbackMsg(event);
    if (cbmsg != NULL) {
        WriteInt(context, cbmsg->msg.connMsg.status);
        WriteInt(context, cbmsg->msg.connMsg.networkId);
        WriteStr(context, cbmsg->msg.connMsg.bssid);
    }
    return;
}

static void DealConnectWpsResultCbk(int event, Context *context)
{
    WifiHalEventCallbackMsg *cbmsg = FrontCallbackMsg(event);
    if (cbmsg != NULL) {
        WriteInt(context, cbmsg->msg.connMsg.status);
    }
    return;
}

static void DealStaApCallback(int event, Context *context)
{
    switch (event) {
        case WIFI_ADD_IFACE_EVENT:
        case WIFI_STA_JOIN_EVENT:
        case WIFI_STA_LEAVE_EVENT:
            DealIfaceCbk(event, context);
            break;
        case WIFI_SCAN_INFO_NOTIFY_EVENT:
            DealCommonCbk(event, context);
            break;
        case WIFI_WPA_STATE_EVENT:
        case WIFI_SSID_WRONG_KEY:
        case WIFI_WPS_OVERLAP:
        case WIFI_WPS_TIME_OUT:
        case WIFI_CONNECTION_FULL_EVENT:
        case WIFI_CONNECTION_REJECT_EVENT:
            DealConnectWpsResultCbk(event, context);
            break;
        case WIFI_CONNECT_CHANGED_NOTIFY_EVENT:
            DealConnectionChangedCbk(event, context);
            break;
        default:
            break;
    }
    return;
}

static void DealP2pDeviceFoundCbk(int event, Context *context)
{
    WifiHalEventCallbackMsg *cbmsg = FrontCallbackMsg(event);
    if (cbmsg != NULL) {
        WriteInt(context, cbmsg->msg.deviceInfo.configMethods);
        WriteInt(context, cbmsg->msg.deviceInfo.deviceCapabilities);
        WriteInt(context, cbmsg->msg.deviceInfo.groupCapabilities);
        WriteInt(context, cbmsg->msg.deviceInfo.wfdLength);
        WriteStr(context, cbmsg->msg.deviceInfo.srcAddress);
        WriteStr(context, cbmsg->msg.deviceInfo.p2pDeviceAddress);
        WriteStr(context, cbmsg->msg.deviceInfo.primaryDeviceType);
        WriteStr(context, cbmsg->msg.deviceInfo.deviceName);
        WriteStr(context, cbmsg->msg.deviceInfo.wfdDeviceInfo);
    }
    return;
}

static void DealP2pNegoriationCbk(int event, Context *context)
{
    WifiHalEventCallbackMsg *cbmsg = FrontCallbackMsg(event);
    if (cbmsg != NULL) {
        if (event == P2P_DEVICE_LOST_EVENT) {
            WriteStr(context, cbmsg->msg.connMsg.bssid);
        }
        if (event == P2P_GO_NEGOTIATION_REQUEST_EVENT) {
            WriteInt(context, cbmsg->msg.connMsg.status);
            WriteStr(context, cbmsg->msg.connMsg.bssid);
        }
    }
    return;
}

static void DealP2pInviationCbk(int event, Context *context)
{
    WifiHalEventCallbackMsg *cbmsg = FrontCallbackMsg(event);
    if (cbmsg != NULL) {
        if (event == P2P_INVITATION_RECEIVED_EVENT) {
            WriteInt(context, cbmsg->msg.invitaInfo.type);
            WriteInt(context, cbmsg->msg.invitaInfo.persistentNetworkId);
            WriteInt(context, cbmsg->msg.invitaInfo.operatingFrequency);
            WriteStr(context, cbmsg->msg.invitaInfo.srcAddress);
            WriteStr(context, cbmsg->msg.invitaInfo.goDeviceAddress);
            WriteStr(context, cbmsg->msg.invitaInfo.bssid);
        }
        if (event == P2P_INVITATION_RESULT_EVENT) {
            WriteInt(context, cbmsg->msg.invitaInfo.persistentNetworkId);
            WriteStr(context, cbmsg->msg.invitaInfo.bssid);
        }
        if (event == P2P_GROUP_FORMATION_FAILURE_EVENT) {
            WriteStr(context, cbmsg->msg.invitaInfo.bssid);
        }
    }
    return;
}

static void DealP2pGroupInfoCbk(int event, Context *context)
{
    WifiHalEventCallbackMsg *cbmsg = FrontCallbackMsg(event);
    if (cbmsg != NULL) {
        if (event == P2P_GROUP_STARTED_EVENT) {
            WriteInt(context, cbmsg->msg.groupInfo.isGo);
            WriteInt(context, cbmsg->msg.groupInfo.isPersistent);
            WriteInt(context, cbmsg->msg.groupInfo.frequency);
            WriteStr(context, cbmsg->msg.groupInfo.groupIfName);
            WriteStr(context, cbmsg->msg.groupInfo.ssid);
            WriteStr(context, cbmsg->msg.groupInfo.psk);
            WriteStr(context, cbmsg->msg.groupInfo.passphrase);
            WriteStr(context, cbmsg->msg.groupInfo.goDeviceAddress);
        }
        if (event == P2P_GROUP_REMOVED_EVENT) {
            WriteInt(context, cbmsg->msg.groupInfo.isGo);
            WriteStr(context, cbmsg->msg.groupInfo.groupIfName);
        }
    }
    return;
}

static void DealP2pDeviceInfoCbk(int event, Context *context)
{
    WifiHalEventCallbackMsg *cbmsg = FrontCallbackMsg(event);
    if (cbmsg != NULL) {
        if (event == P2P_PROV_DISC_PBC_REQ_EVENT || event == P2P_PROV_DISC_PBC_RSP_EVENT ||
            event == P2P_PROV_DISC_ENTER_PIN_EVENT) {
            WriteStr(context, cbmsg->msg.deviceInfo.srcAddress);
        }
        if (event == P2P_PROV_DISC_SHOW_PIN_EVENT) {
            WriteStr(context, cbmsg->msg.deviceInfo.srcAddress);
            WriteStr(context, cbmsg->msg.deviceInfo.deviceName);
        }
        if (event == AP_STA_DISCONNECTED_EVENT || event == AP_STA_CONNECTED_EVENT) {
            WriteStr(context, cbmsg->msg.deviceInfo.p2pDeviceAddress);
        }
    }
    return;
}

static void DealP2pServerInfoCbk(int event, Context *context)
{
    WifiHalEventCallbackMsg *cbmsg = FrontCallbackMsg(event);
    if (cbmsg != NULL) {
        WriteInt(context, cbmsg->msg.serverInfo.updateIndicator);
        WriteStr(context, cbmsg->msg.serverInfo.srcAddress);
        if (cbmsg->msg.serverInfo.tlvs != NULL) {
            WriteInt(context, strlen(cbmsg->msg.serverInfo.tlvs));
            WriteStr(context, cbmsg->msg.serverInfo.tlvs);
            free(cbmsg->msg.serverInfo.tlvs);
            cbmsg->msg.serverInfo.tlvs = NULL;
        } else {
            WriteInt(context, 0);
        }
    }
    return;
}

static void DealP2pServerDiscReqCbk(int event, Context *context)
{
    WifiHalEventCallbackMsg *cbmsg = FrontCallbackMsg(event);
    if (cbmsg != NULL) {
        WriteInt(context, cbmsg->msg.serDiscReqInfo.freq);
        WriteInt(context, cbmsg->msg.serDiscReqInfo.dialogToken);
        WriteInt(context, cbmsg->msg.serDiscReqInfo.updateIndic);
        WriteStr(context, cbmsg->msg.serDiscReqInfo.mac);
        if (cbmsg->msg.serDiscReqInfo.tlvs != NULL) {
            WriteInt(context, strlen(cbmsg->msg.serDiscReqInfo.tlvs));
            WriteStr(context, cbmsg->msg.serDiscReqInfo.tlvs);
            free(cbmsg->msg.serDiscReqInfo.tlvs);
            cbmsg->msg.serDiscReqInfo.tlvs = NULL;
        } else {
            WriteInt(context, 0);
        }
    }
    return;
}

static void DealP2pCallback(int event, Context *context)
{
    switch (event) {
        case WIFI_P2P_SUP_CONNECTION_EVENT:
        case P2P_GO_NEGOTIATION_FAILURE_EVENT:
            DealCommonCbk(event, context);
            break;
        case P2P_DEVICE_FOUND_EVENT:
            DealP2pDeviceFoundCbk(event, context);
            break;
        case P2P_DEVICE_LOST_EVENT:
        case P2P_GO_NEGOTIATION_REQUEST_EVENT:
            DealP2pNegoriationCbk(event, context);
            break;
        case P2P_INVITATION_RECEIVED_EVENT:
        case P2P_INVITATION_RESULT_EVENT:
        case P2P_GROUP_FORMATION_FAILURE_EVENT:
            DealP2pInviationCbk(event, context);
            break;
        case P2P_GROUP_STARTED_EVENT:
        case P2P_GROUP_REMOVED_EVENT:
            DealP2pGroupInfoCbk(event, context);
            break;
        case P2P_PROV_DISC_PBC_REQ_EVENT:
        case P2P_PROV_DISC_PBC_RSP_EVENT:
        case P2P_PROV_DISC_ENTER_PIN_EVENT:
        case P2P_PROV_DISC_SHOW_PIN_EVENT:
        case AP_STA_DISCONNECTED_EVENT:
        case AP_STA_CONNECTED_EVENT:
            DealP2pDeviceInfoCbk(event, context);
            break;
        case P2P_SERV_DISC_RESP_EVENT:
            DealP2pServerInfoCbk(event, context);
            break;
        case P2P_SERV_DISC_REQ_EVENT:
            DealP2pServerDiscReqCbk(event, context);
            break;
        default:
            break;
    }
    return;
}

/* Callback request */
int OnCallbackTransact(const RpcServer *server, int event, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    WriteBegin(context, 1);
    WriteInt(context, event);
    DealStaApCallback(event, context);
    DealP2pCallback(event, context);
    WriteEnd(context);
    return 0;
}

int EndCallbackTransact(const RpcServer *server, int event)
{
    if (server == NULL) {
        return -1;
    }
    return PopFrontCallbackMsg(event);
}