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

#include "i_wifi_p2p_iface.h"
#include "client.h"
#include "serial.h"

#include "wifi_log.h"
#include "wifi_idl_define.h"
#include "wifi_idl_inner_interface.h"
#include "i_wifi_public_func.h"
#include <securec.h>

#undef LOG_TAG
#define LOG_TAG "WifiIdlP2pIface"

#define P2P_EVENT_MAX_NUM 32

static IWifiEventP2pCallback g_wifiP2pEventCallback = {0};
void SetWifiP2pEventCallback(IWifiEventP2pCallback callback)
{
    g_wifiP2pEventCallback = callback;
}

IWifiEventP2pCallback *GetWifiP2pEventCallback(void)
{
    return &g_wifiP2pEventCallback;
}

WifiErrorNo P2pStart(void)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pStart");
    WriteEnd(context);
    if (RpcClientCall(client, "P2pStart") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pStop(void)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pStop");
    WriteEnd(context);
    if (RpcClientCall(client, "P2pStop") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pSetRandomMac(int enable)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pSetRandomMac");
    WriteInt(context, enable);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pSetRandomMac") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pSetDeviceName(const char *name)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pSetDeviceName");
    WriteStr(context, name);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pSetDeviceName") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pSetSsidPostfixName(const char *postfixName)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pSetSsidPostfixName");
    WriteStr(context, postfixName);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pSetSsidPostfixName") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pSetWpsDeviceType(const char *type)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pSetWpsDeviceType");
    WriteStr(context, type);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pSetWpsDeviceType") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pSetWpsSecondaryDeviceType(const char *type)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pSetWpsSecondaryDeviceType");
    WriteStr(context, type);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pSetWpsSecondaryDeviceType") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pSetWpsConfigMethods(const char *config)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pSetWpsConfigMethods");
    WriteStr(context, config);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pSetWpsConfigMethods") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pGetDeviceAddress(char *deviceAddress, int size)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pGetDeviceAddress");
    WriteInt(context, size);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pGetDeviceAddress") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("P2pGetDeviceAddress failed!");
    } else {
        ReadStr(context, deviceAddress, size);
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pFlush()
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pFlush");
    WriteEnd(context);
    if (RpcClientCall(client, "P2pFlush") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pFlushService()
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pFlushService");
    WriteEnd(context);
    if (RpcClientCall(client, "P2pFlushService") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pSaveConfig()
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pSaveConfig");
    WriteEnd(context);
    if (RpcClientCall(client, "P2pSaveConfig") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

static int GetP2pCallbackEvents(int *events, int size)
{
    int p2pEvents[] = {
        WIFI_IDL_CBK_CMD_P2P_SUPPLICANT_CONNECT,
        WIFI_IDL_CBK_CMD_P2P_DEVICE_FOUND_EVENT,
        WIFI_IDL_CBK_CMD_P2P_DEVICE_LOST_EVENT,
        WIFI_IDL_CBK_CMD_P2P_GO_NEGOTIATION_REQUEST_EVENT,
        WIFI_IDL_CBK_CMD_P2P_GO_NEGOTIATION_SUCCESS_EVENT,
        WIFI_IDL_CBK_CMD_P2P_GO_NEGOTIATION_FAILURE_EVENT,
        WIFI_IDL_CBK_CMD_P2P_INVITATION_RECEIVED_EVENT,
        WIFI_IDL_CBK_CMD_P2P_INVITATION_RESULT_EVENT,
        WIFI_IDL_CBK_CMD_P2P_GROUP_FORMATION_SUCCESS_EVENT,
        WIFI_IDL_CBK_CMD_P2P_GROUP_FORMATION_FAILURE_EVENT,
        WIFI_IDL_CBK_CMD_P2P_GROUP_STARTED_EVENT,
        WIFI_IDL_CBK_CMD_P2P_GROUP_REMOVED_EVENT,
        WIFI_IDL_CBK_CMD_P2P_PROV_DISC_PBC_REQ_EVENT,
        WIFI_IDL_CBK_CMD_P2P_PROV_DISC_PBC_RSP_EVENT,
        WIFI_IDL_CBK_CMD_P2P_PROV_DISC_ENTER_PIN_EVENT,
        WIFI_IDL_CBK_CMD_P2P_PROV_DISC_SHOW_PIN_EVENT,
        WIFI_IDL_CBK_CMD_P2P_PROV_DISC_FAILURE_EVENT,
        WIFI_IDL_CBK_CMD_P2P_FIND_STOPPED_EVENT,
        WIFI_IDL_CBK_CMD_P2P_SERV_DISC_RESP_EVENT,
        WIFI_IDL_CBK_CMD_AP_STA_DISCONNECTED_EVENT,
        WIFI_IDL_CBK_CMD_AP_STA_CONNECTED_EVENT,
        WIFI_IDL_CBK_CMD_SUP_CONN_FAILED_EVENT,
        WIFI_IDL_CBK_CMD_P2P_SERV_DISC_REQ_EVENT
    };
    int max = sizeof(p2pEvents) / sizeof(p2pEvents[0]);
    int num = 0;
    for (; num < max && num < size; ++num) {
        events[num] = p2pEvents[num];
    }
    return num;
}

WifiErrorNo RegisterP2pEventCallback(IWifiEventP2pCallback callback)
{
    int events[P2P_EVENT_MAX_NUM];
    int num = GetP2pCallbackEvents(events, P2P_EVENT_MAX_NUM);
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    if (callback.onP2pSupplicantConnect == NULL) { /* UnRegisterEventCallback */
        WriteFunc(context, "UnRegisterEventCallback");
    } else {
        WriteFunc(context, "RegisterEventCallback");
    }
    WriteInt(context, num);
    for (int i = 0; i < num; ++i) {
        WriteInt(context, events[i]);
    }
    WriteEnd(context);
    if (RpcClientCall(client, "RegisterP2pEventCallback") != WIFI_IDL_OPT_OK) {
        if (callback.onP2pSupplicantConnect == NULL) {
            SetWifiP2pEventCallback(callback);
        }
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result == WIFI_IDL_OPT_OK || callback.onP2pSupplicantConnect == NULL) {
        SetWifiP2pEventCallback(callback);
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pSetupWpsPbc(const char *groupIfc, const char *address)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pSetupWpsPbc");
    WriteStr(context, groupIfc);
    WriteStr(context, address);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pSetupWpsPbc") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pSetupWpsPin(const char *groupIfc, const char *address, const char *pin, char *result, int resultLen)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pSetupWpsPin");
    WriteStr(context, groupIfc);
    WriteStr(context, address);
    WriteStr(context, pin);
    WriteInt(context, resultLen);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pSetupWpsPin") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int retCode = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &retCode);
    if (retCode == WIFI_IDL_OPT_OK) {
        ReadStr(context, result, resultLen);
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return retCode;
}

WifiErrorNo P2pRemoveNetwork(int networkId)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pRemoveNetwork");
    WriteInt(context, networkId);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pRemoveNetwork") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pListNetworks(P2pNetworkList *infoList)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pListNetworks");
    WriteEnd(context);
    if (RpcClientCall(client, "P2pListNetworks") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    do {
        if (result != WIFI_IDL_OPT_OK) {
            LOGE("P2pListNetworks failed!");
            break;
        }
        int infoNum = 0;
        ReadInt(context, &infoNum);
        infoList->infoNum = infoNum;
        if (infoNum <= 0) {
            break;
        }
        infoList->infos = (P2pNetworkInfo *)calloc(infoNum, sizeof(P2pNetworkInfo));
        if (infoList->infos == NULL) {
            result = WIFI_IDL_OPT_FAILED;
            break;
        }
        for (int i = 0; i < infoNum; i++) {
            ReadInt(context, &infoList->infos[i].id);
            ReadStr(context, infoList->infos[i].ssid, sizeof(infoList->infos[i].ssid));
            ReadStr(context, infoList->infos[i].bssid, sizeof(infoList->infos[i].bssid));
            ReadStr(context, infoList->infos[i].flags, sizeof(infoList->infos[i].flags));
        }
    } while (0);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pSetGroupMaxIdle(const char *groupIfc, int time)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pSetGroupMaxIdle");
    WriteStr(context, groupIfc);
    WriteInt(context, time);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pSetGroupMaxIdle") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pSetPowerSave(const char *groupIfc, int enable)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pSetPowerSave");
    WriteStr(context, groupIfc);
    WriteInt(context, enable);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pSetPowerSave") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pSetWfdEnable(int enable)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pSetWfdEnable");
    WriteInt(context, enable);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pSetWfdEnable") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pSetWfdDeviceConfig(const char *config)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pSetWfdDeviceConfig");
    WriteStr(context, config);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pSetWfdDeviceConfig") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pStartFind(int timeout)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pStartFind");
    WriteInt(context, timeout);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pStartFind") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pStopFind()
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pStopFind");
    WriteEnd(context);
    if (RpcClientCall(client, "P2pStopFind") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pSetExtListen(int enable, int period, int interval)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pSetExtListen");
    WriteInt(context, enable);
    WriteInt(context, period);
    WriteInt(context, interval);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pSetExtListen") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pSetListenChannel(int channel, int regClass)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pSetListenChannel");
    WriteInt(context, channel);
    WriteInt(context, regClass);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pSetListenChannel") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pConnect(P2pConnectInfo *info)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pConnect");
    WriteInt(context, info->mode);
    WriteInt(context, info->provdisc);
    WriteInt(context, info->goIntent);
    WriteInt(context, info->persistent);
    WriteStr(context, info->peerDevAddr);
    WriteStr(context, info->pin);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pConnect") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result == WIFI_IDL_OPT_OK) {
        if (memset_s(info->pin, sizeof(info->pin), 0, sizeof(info->pin)) != EOK) {
            result = WIFI_IDL_OPT_FAILED;
        } else {
            ReadStr(context, info->pin, sizeof(info->pin));
        }
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pCancelConnect()
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pCancelConnect");
    WriteEnd(context);
    if (RpcClientCall(client, "P2pCancelConnect") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pProvisionDiscovery(const char *peerBssid, int mode)
{
    LOGD("P2pProvisionDiscovery addr: %{private}s, mode: %{public}d", peerBssid, mode);
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pProvisionDiscovery");
    WriteStr(context, peerBssid);
    WriteInt(context, mode);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pProvisionDiscovery") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pAddGroup(int isPersistent, int networkId, int freq)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pAddGroup");
    WriteInt(context, isPersistent);
    WriteInt(context, networkId);
    WriteInt(context, freq);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pAddGroup") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pRemoveGroup(const char *interface)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pRemoveGroup");
    WriteStr(context, interface);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pRemoveGroup") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pInvite(int persisitent, const char *peerBssid, const char *goBssid, const char *ifname)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pInvite");
    WriteInt(context, persisitent);
    WriteStr(context, peerBssid);
    WriteStr(context, goBssid);
    WriteStr(context, ifname);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pInvite") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pReinvoke(int networkId, const char *bssid)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pReinvoke");
    WriteInt(context, networkId);
    WriteStr(context, bssid);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pReinvoke") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pGetGroupCapability(const char *bssid, int *cap)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pGetGroupCapability");
    WriteStr(context, bssid);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pGetGroupCapability") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("P2pGetGroupCapability failed!");
    } else {
        ReadInt(context, cap);
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pAddService(const P2pServiceInfo *info)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pAddService");
    WriteInt(context, info->mode);
    if (!info->mode) {
        WriteInt(context, info->version);
        WriteStr(context, info->name);
    } else {
        WriteStr(context, info->query);
        WriteStr(context, info->resp);
    }
    WriteEnd(context);
    if (RpcClientCall(client, "P2pAddService") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pRemoveService(const P2pServiceInfo *info)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pRemoveService");
    WriteInt(context, info->mode);
    if (!info->mode) {
        WriteInt(context, info->version);
        WriteStr(context, info->name);
    } else {
        WriteStr(context, info->query);
    }
    WriteEnd(context);
    if (RpcClientCall(client, "P2pRemoveService") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pReqServiceDiscovery(const char *bssid, const char *msg, char *retMsg, int size)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pReqServiceDiscovery");
    WriteStr(context, bssid);
    WriteStr(context, msg);
    WriteInt(context, size);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pReqServiceDiscovery") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result == WIFI_IDL_OPT_OK) {
        ReadStr(context, retMsg, size);
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pCancelServiceDiscovery(const char *id)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pCancelServiceDiscovery");
    WriteStr(context, id);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pCancelServiceDiscovery") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pSetMiracastType(int type)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pSetMiracastType");
    WriteInt(context, type);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pSetMiracastType") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pSetPersistentReconnect(int mode)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pSetPersistentReconnect");
    WriteInt(context, mode);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pSetPersistentReconnect") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pRespServerDiscovery(const char *deviceAddress, int frequency, int dialogToken, const char *tlvs)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pRespServerDiscovery");
    WriteInt(context, frequency);
    WriteInt(context, dialogToken);
    WriteStr(context, deviceAddress);
    WriteStr(context, tlvs);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pRespServerDiscovery") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pSetServDiscExternal(int mode)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pSetServDiscExternal");
    WriteInt(context, mode);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pSetServDiscExternal") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pGetPeer(const char *deviceAddress, P2pDeviceInfo *peerInfo)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pGetPeer");
    WriteStr(context, deviceAddress);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pGetPeer") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("P2pGetPeer failed!");
    } else {
        ReadStr(context, peerInfo->p2pDeviceAddress, sizeof(peerInfo->p2pDeviceAddress));
        ReadStr(context, peerInfo->deviceName, sizeof(peerInfo->deviceName));
        ReadStr(context, peerInfo->primaryDeviceType, sizeof(peerInfo->primaryDeviceType));
        ReadInt(context, &peerInfo->configMethods);
        ReadInt(context, &peerInfo->deviceCapabilities);
        ReadInt(context, &peerInfo->groupCapabilities);
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pGetFrequencies(int32_t band, int *frequencies, int32_t *size)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pGetFrequencies");
    WriteInt(context, band);
    WriteInt(context, *size);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pGetFrequencies") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }

    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("server P2pGetFrequencies deal failed!");
    } else {
        ReadInt(context, size);
        for (int i = 0; i < *size; ++i) {
            ReadInt(context, frequencies + i);
        }
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pSetGroupConfig(int networkId, P2pGroupConfig *pConfig, int size)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pSetGroupConfig");
    WriteInt(context, networkId);
    WriteInt(context, size);
    for (int i = 0; i < size; ++i) {
        WriteInt(context, pConfig[i].cfgParam);
        WriteStr(context, pConfig[i].cfgValue);
    }
    WriteEnd(context);
    if (RpcClientCall(client, "P2pSetGroupConfig") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("server P2pSetGroupConfig deal failed!");
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pGetGroupConfig(int networkId, P2pGroupConfig *pConfig, int size)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pGetGroupConfig");
    WriteInt(context, networkId);
    WriteInt(context, size);
    for (int i = 0; i < size; ++i) {
        WriteInt(context, pConfig[i].cfgParam);
    }
    WriteEnd(context);
    if (RpcClientCall(client, "P2pGetGroupConfig") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("server P2pGetGroupConfig deal failed!");
    } else {
        for (int i = 0; i < size; i++) {
            ReadStr(context, pConfig[i].cfgValue, sizeof(pConfig[i].cfgValue));
        }
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo P2pAddNetwork(int *networkId)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pAddNetwork");
    WriteEnd(context);
    if (RpcClientCall(client, "P2pAddNetwork") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("server P2pAddNetwork deal failed!");
    } else {
        ReadInt(context, networkId);
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo Hid2dConnect(Hid2dConnectInfo *info)
{
    RpcClient *client = GetP2pRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "P2pHid2dConnect");
    WriteStr(context, info->ssid);
    WriteStr(context, info->bssid);
    WriteStr(context, info->passphrase);
    WriteInt(context, info->frequency);
    WriteEnd(context);
    if (RpcClientCall(client, "P2pHid2dConnect") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}