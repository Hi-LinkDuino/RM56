/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "i_wifi_hotspot_iface.h"
#include "client.h"
#include "serial.h"
#include "wifi_log.h"
#include "wifi_idl_define.h"
#include "wifi_idl_inner_interface.h"
#include "i_wifi_public_func.h"

#undef LOG_TAG
#define LOG_TAG "WifiIdlHotspotIface"

#define AP_EVENT_MAX_NUM 8

static IWifiApEventCallback g_wifiApEventCallback = {0};
void SetWifiApEventCallback(IWifiApEventCallback callback)
{
    g_wifiApEventCallback = callback;
}

IWifiApEventCallback *GetWifiApEventCallback(void)
{
    return &g_wifiApEventCallback;
}

WifiErrorNo StartSoftAp(void)
{
    RpcClient *client = GetApRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "StartSoftAp");
    WriteEnd(context);

    if (RpcClientCall(client, "StartSoftAp") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }

    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo StopSoftAp(void)
{
    RpcClient *client = GetApRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "StopSoftAp");
    WriteEnd(context);

    if (RpcClientCall(client, "StopSoftAp") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }

    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo SetHostapdConfig(HostapdConfig *config)
{
    RpcClient *client = GetApRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "SetHostapdConfig");
    WriteStr(context, config->ssid);
    WriteInt(context, config->ssidLen);
    WriteStr(context, config->preSharedKey);
    WriteInt(context, config->preSharedKeyLen);
    WriteInt(context, config->securityType);
    WriteInt(context, config->band);
    WriteInt(context, config->channel);
    WriteInt(context, config->maxConn);
    WriteEnd(context);
    if (RpcClientCall(client, "SetHostapdConfig") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo GetStaInfos(char *infos, int32_t *size)
{
    RpcClient *client = GetApRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "GetStaInfos");
    WriteInt(context, *size);
    WriteEnd(context);
    if (RpcClientCall(client, "GetStaInfos") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("server GetStaInfos deal failed!");
    } else {
        ReadInt(context, size);
        ReadStr(context, infos, *size);
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo SetMacFilter(unsigned char *mac, int lenMac)
{
    RpcClient *client = GetApRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "SetMacFilter");
    WriteInt(context, lenMac);
    WriteUStr(context, mac, lenMac);
    WriteEnd(context);
    if (RpcClientCall(client, "SetMacFilter") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo DelMacFilter(unsigned char *mac, int lenMac)
{
    RpcClient *client = GetApRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "DelMacFilter");
    WriteInt(context, lenMac);
    WriteUStr(context, mac, lenMac);
    WriteEnd(context);
    if (RpcClientCall(client, "DelMacFilter") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo DisassociateSta(unsigned char *mac, int lenMac)
{
    RpcClient *client = GetApRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "DisassociateSta");
    WriteInt(context, lenMac);
    WriteUStr(context, mac, lenMac);
    WriteEnd(context);
    if (RpcClientCall(client, "DisassociateSta") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo GetValidFrequenciesForBand(int32_t band, int *frequencies, int32_t *size)
{
    RpcClient *client = GetApRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "GetValidFrequenciesForBand");
    WriteInt(context, band);
    WriteInt(context, *size);
    WriteEnd(context);
    if (RpcClientCall(client, "GetValidFrequenciesForBand") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("server GetValidFrequenciesForBand deal failed!");
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

WifiErrorNo SetCountryCode(const char *code)
{
    RpcClient *client = GetApRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "SetCountryCode");
    WriteStr(context, code);
    WriteEnd(context);
    if (RpcClientCall(client, "SetCountryCode") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

static int GetApCallbackEvents(int *events, int size)
{
    int apEvents[] = {
        WIFI_IDL_CBK_CMD_STA_JOIN,
        WIFI_IDL_CBK_CMD_STA_LEAVE,
        WIFI_IDL_CBK_CMD_AP_ENABLE,
        WIFI_IDL_CBK_CMD_AP_DISABLE
    };
    int max = sizeof(apEvents) / sizeof(apEvents[0]);
    int num = 0;
    for (; num < max && num < size; ++num) {
        events[num] = apEvents[num];
    }
    return num;
}

WifiErrorNo RegisterAsscociatedEvent(IWifiApEventCallback callback)
{
    int events[AP_EVENT_MAX_NUM];
    int num = GetApCallbackEvents(events, AP_EVENT_MAX_NUM);
    RpcClient *client = GetApRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    if (callback.onStaJoinOrLeave == NULL) {
        WriteFunc(context, "UnRegisterEventCallback");
    } else {
        WriteFunc(context, "RegisterEventCallback");
    }
    WriteInt(context, num);
    for (int i = 0; i < num; ++i) {
        WriteInt(context, events[i]);
    }
    WriteEnd(context);
    if (RpcClientCall(client, "RegisterAsscociatedEvent") != WIFI_IDL_OPT_OK) {
        if (callback.onStaJoinOrLeave == NULL) {
            SetWifiApEventCallback(callback);
        }
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result == WIFI_IDL_OPT_OK || callback.onStaJoinOrLeave == NULL) {
        SetWifiApEventCallback(callback);
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}
