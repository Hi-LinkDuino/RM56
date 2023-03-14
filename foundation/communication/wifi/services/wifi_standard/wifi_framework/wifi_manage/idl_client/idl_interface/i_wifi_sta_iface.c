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
#include "i_wifi_sta_iface.h"
#include "client.h"
#include "serial.h"
#include "wifi_log.h"
#include "wifi_idl_define.h"
#include "wifi_idl_inner_interface.h"
#include "i_wifi_public_func.h"

#undef LOG_TAG
#define LOG_TAG "WifiIdlStaIface"
#define EVENT_MAX_NUM 8

static IWifiEventCallback g_wifiStaEventCallback = {0};
void SetWifiEventCallback(IWifiEventCallback callback)
{
    g_wifiStaEventCallback = callback;
}

IWifiEventCallback *GetWifiEventCallback(void)
{
    return &g_wifiStaEventCallback;
}

WifiErrorNo GetStaCapabilities(int32_t *capabilities)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "GetStaCapabilities");
    WriteEnd(context);
    if (RpcClientCall(client, "GetStaCapabilities") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("server GetStaCapabilities deal failed!");
    } else {
        ReadInt(context, capabilities);
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo GetDeviceMacAddress(unsigned char *mac, int *lenMac)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "GetDeviceMacAddress");
    WriteInt(context, *lenMac);
    WriteEnd(context);
    if (RpcClientCall(client, "GetDeviceMacAddress") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("server GetDeviceMacAddress deal failed!");
    } else {
        ReadInt(context, lenMac);
        ReadUStr(context, mac, *lenMac + 1);
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo GetFrequencies(int32_t band, int *frequencies, int32_t *size)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "GetFrequencies");
    WriteInt(context, band);
    WriteInt(context, *size);
    WriteEnd(context);
    if (RpcClientCall(client, "GetFrequencies") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("server GetFrequencies deal failed!");
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

WifiErrorNo SetAssocMacAddr(unsigned char *mac, int lenMac)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "SetAssocMacAddr");
    WriteInt(context, lenMac);
    WriteUStr(context, mac, lenMac);
    WriteEnd(context);
    if (RpcClientCall(client, "SetAssocMacAddr") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo SetScanningMacAddress(unsigned char *mac, int lenMac)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "SetScanningMacAddress");
    WriteInt(context, lenMac);
    WriteUStr(context, mac, lenMac);
    WriteEnd(context);
    if (RpcClientCall(client, "SetScanningMacAddress") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo DeauthLastRoamingBssid(unsigned char *mac, int lenMac)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "DeauthLastRoamingBssid");
    WriteInt(context, lenMac);
    WriteUStr(context, mac, lenMac);
    WriteEnd(context);
    if (RpcClientCall(client, "DeauthLastRoamingBssid") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo GetSupportFeature(long *feature)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "GetSupportFeature");
    WriteEnd(context);
    if (RpcClientCall(client, "GetSupportFeature") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result == WIFI_IDL_OPT_OK) {
        ReadLong(context, feature);
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo RunCmd(const char *ifname, int32_t cmdId, unsigned char *buf, int32_t bufSize)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "RunCmd");
    WriteStr(context, ifname);
    WriteInt(context, cmdId);
    WriteInt(context, bufSize);
    WriteUStr(context, buf, bufSize);
    WriteEnd(context);
    if (RpcClientCall(client, "RunCmd") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo SetWifiTxPower(int32_t power)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "SetWifiTxPower");
    WriteInt(context, power);
    WriteEnd(context);
    if (RpcClientCall(client, "SetWifiTxPower") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo RemoveNetwork(int networkId)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "RemoveNetwork");
    WriteInt(context, networkId);
    WriteEnd(context);
    if (RpcClientCall(client, "RemoveNetwork") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo AddNetwork(int *networkId)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "AddNetwork");
    WriteEnd(context);
    if (RpcClientCall(client, "AddNetwork") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("server AddNetwork deal failed!");
    } else {
        ReadInt(context, networkId);
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo EnableNetwork(int networkId)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "EnableNetwork");
    WriteInt(context, networkId);
    WriteEnd(context);
    if (RpcClientCall(client, "EnableNetwork") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("server EnableNetwork deal failed!");
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo DisableNetwork(int networkId)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "DisableNetwork");
    WriteInt(context, networkId);
    WriteEnd(context);
    if (RpcClientCall(client, "DisableNetwork") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("server DisableNetwork deal failed!");
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo SetNetwork(int networkId, SetNetworkConfig *confs, int size)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "SetNetwork");
    WriteInt(context, networkId);
    WriteInt(context, size);
    for (int i = 0; i < size; ++i) {
        WriteInt(context, confs[i].cfgParam);
        WriteStr(context, confs[i].cfgValue);
    }
    WriteEnd(context);
    if (RpcClientCall(client, "SetNetwork") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("server SetNetwork deal failed!");
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo WpaGetNetwork(GetNetworkConfig *confs)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "WpaGetNetwork");
    WriteInt(context, confs->networkId);
    WriteStr(context, confs->param);
    WriteEnd(context);
    if (RpcClientCall(client, "WpaGetNetwork") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("server WpaGetNetwork deal failed!");
    } else {
        ReadStr(context, confs->value, WIFI_NETWORK_CONFIG_VALUE_LENGTH);
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return (WifiErrorNo)result;
}

WifiErrorNo SaveNetworkConfig(void)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "SaveNetworkConfig");
    WriteEnd(context);
    if (RpcClientCall(client, "SaveNetworkConfig") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo StartScan(const ScanSettings *settings)
{
    if (settings == NULL) {
        return WIFI_IDL_OPT_FAILED;
    }
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "StartScan");
    WriteInt(context, settings->hiddenSsidSize);
    for (int i = 0; i < settings->hiddenSsidSize; ++i) {
        WriteInt(context, strlen(settings->hiddenSsid[i]));
        WriteStr(context, settings->hiddenSsid[i]);
    }
    WriteInt(context, settings->freqSize);
    for (int i = 0; i < settings->freqSize; ++i) {
        WriteInt(context, settings->freqs[i]);
    }
    WriteInt(context, settings->scanStyle);
    WriteEnd(context);
    if (RpcClientCall(client, "StartScan") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo GetNetworkList(WifiNetworkInfo *infos, int *size)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "GetNetworkList");
    WriteInt(context, *size);
    WriteEnd(context);
    if (RpcClientCall(client, "GetNetworkList") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("server GetNetworkList deal failed!");
    } else {
        ReadInt(context, size);
        for (int i = 0; i < *size; ++i) {
            ReadInt(context, &infos[i].id);
            ReadStr(context, infos[i].ssid, WIFI_SSID_LENGTH);
            ReadStr(context, infos[i].bssid, WIFI_BSSID_LENGTH);
            ReadStr(context, infos[i].flags, WIFI_NETWORK_FLAGS_LENGTH);
        }
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

ScanInfo* GetScanInfos(int *size)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "GetScanInfos");
    WriteInt(context, *size);
    WriteEnd(context);
    if (RpcClientCall(client, "GetScanInfos") != WIFI_IDL_OPT_OK) {
        return NULL;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ScanInfo* scanInfos = NULL;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("server GetScanInfos deal failed!");
    } else {
        ReadInt(context, size);
        LOGI("GetScanInfos size: %{public}d", *size);
        if (*size > 0) {
            scanInfos = (ScanInfo *)calloc(*size, sizeof(ScanInfo));
            if (scanInfos != NULL) {
                for (int i = 0; i < *size; ++i) {
                    ReadStr(context, scanInfos[i].bssid, WIFI_BSSID_LENGTH);
                    ReadInt(context, &scanInfos[i].frequency);
                    ReadInt(context, &scanInfos[i].signalLevel);
                    ReadStr(context, scanInfos[i].capability, WIFI_SCAN_INFO_CAPABILITIES_LENGTH);
                    ReadStr(context, scanInfos[i].ssid, WIFI_SSID_LENGTH);
                    ReadInt64(context, &scanInfos[i].timestamp);
                }
            } else {
                LOGE("GetScanInfos alloc mem failed!");
            }
        }
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return scanInfos;
}

WifiErrorNo StartPnoScan(const PnoScanSettings *settings)
{
    if (settings == NULL) {
        return WIFI_IDL_OPT_FAILED;
    }
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "StartPnoScan");
    WriteInt(context, settings->scanInterval);
    WriteInt(context, settings->minRssi2Dot4Ghz);
    WriteInt(context, settings->minRssi5Ghz);
    WriteInt(context, settings->hiddenSsidSize);
    for (int i = 0; i < settings->hiddenSsidSize; ++i) {
        WriteInt(context, strlen(settings->hiddenSsid[i]));
        WriteStr(context, settings->hiddenSsid[i]);
    }
    WriteInt(context, settings->savedSsidSize);
    for (int i = 0; i < settings->savedSsidSize; ++i) {
        WriteInt(context, strlen(settings->savedSsid[i]));
        WriteStr(context, settings->savedSsid[i]);
    }
    WriteInt(context, settings->freqSize);
    for (int i = 0; i < settings->freqSize; ++i) {
        WriteInt(context, settings->freqs[i]);
    }
    WriteEnd(context);
    if (RpcClientCall(client, "StartPnoScan") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo StopPnoScan(void)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "StopPnoScan");
    WriteEnd(context);
    if (RpcClientCall(client, "StopPnoScan") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

static int CheckRegisterEvent(int *events, int size)
{
    int staEvents[] = {
        WIFI_IDL_CBK_CMD_FAILURE,
        WIFI_IDL_CBK_CMD_STARTED,
        WIFI_IDL_CBK_CMD_STOPED,
        WIFI_IDL_CBK_CMD_CONNECT_CHANGED,
        WIFI_IDL_CBK_CMD_WPA_STATE_CHANGEM,
        WIFI_IDL_CBK_CMD_SSID_WRONG_KEY,
        WIFI_IDL_CBK_CMD_WPS_OVERLAP,
        WIFI_IDL_CBK_CMD_WPS_TIME_OUT,
        WIFI_IDL_CBK_CMD_WPS_CONNECTION_FULL,
        WIFI_IDL_CBK_CMD_WPS_CONNECTION_REJECT
    };
    int max = sizeof(staEvents) / sizeof(staEvents[0]);
    int num = 0;
    for (; num < max && num < size; ++num) {
        events[num] = staEvents[num];
    }
    return num;
}

WifiErrorNo RegisterStaEventCallback(IWifiEventCallback callback)
{
    int events[EVENT_MAX_NUM];
    int num = CheckRegisterEvent(events, EVENT_MAX_NUM);
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    if (callback.onConnectChanged == NULL) {
        WriteFunc(context, "UnRegisterEventCallback");
    } else {
        WriteFunc(context, "RegisterEventCallback");
    }
    WriteInt(context, num);
    for (int i = 0; i < num; ++i) {
        WriteInt(context, events[i]);
    }
    WriteEnd(context);
    if (RpcClientCall(client, "RegisterStaEventCallback") != WIFI_IDL_OPT_OK) {
        if (callback.onConnectChanged == NULL) {
            SetWifiEventCallback(callback);
        }
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result == WIFI_IDL_OPT_OK || callback.onConnectChanged == NULL) {
        SetWifiEventCallback(callback);
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo StartWpsPbcMode(WifiWpsParam *param)
{
    if (param == NULL) {
        return WIFI_IDL_OPT_FAILED;
    }
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "StartWpsPbcMode");
    WriteInt(context, param->anyFlag);
    WriteInt(context, param->multiAp);
    WriteStr(context, param->bssid);
    WriteEnd(context);
    if (RpcClientCall(client, "StartWpsPbcMode") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo StartWpsPinMode(WifiWpsParam *param, int *pinCode)
{
    if (param == NULL || pinCode == NULL) {
        return WIFI_IDL_OPT_FAILED;
    }
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "StartWpsPinMode");
    WriteInt(context, param->anyFlag);
    WriteInt(context, param->multiAp);
    WriteStr(context, param->bssid);
    WriteStr(context, param->pinCode);
    WriteEnd(context);
    if (RpcClientCall(client, "StartWpsPinMode") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result == WIFI_IDL_OPT_OK) {
        ReadInt(context, pinCode);
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo StopWps(void)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "StopWps");
    WriteEnd(context);
    if (RpcClientCall(client, "StopWps") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo GetRoamingCapabilities(WifiRoamCapability *capability)
{
    if (capability == NULL) {
        return WIFI_IDL_OPT_FAILED;
    }
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "GetRoamingCapabilities");
    WriteEnd(context);
    if (RpcClientCall(client, "GetRoamingCapabilities") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result == WIFI_IDL_OPT_OK) {
        ReadInt(context, &capability->maxBlocklistSize);
        ReadInt(context, &capability->maxTrustlistSize);
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo SetRoamConfig(char **blocklist, int blocksize, char **trustlist, int trustsize)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "SetRoamConfig");
    WriteInt(context, blocksize);
    if (blocksize > 0 && blocklist != NULL) {
        for (int i = 0; i < blocksize; ++i) {
            WriteStr(context, blocklist[i]);
        }
    }
    WriteInt(context, trustsize);
    if (trustsize > 0 && trustlist != NULL) {
        for (int i = 0; i < trustsize; ++i) {
            WriteStr(context, trustlist[i]);
        }
    }
    WriteEnd(context);
    if (RpcClientCall(client, "SetRoamConfig") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo WpaAutoConnect(int enable)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "WpaAutoConnect");
    WriteInt(context, enable);
    WriteEnd(context);
    if (RpcClientCall(client, "WpaAutoConnect") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("WpaAutoConnect failed!");
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}


WifiErrorNo WpaBlocklistClear(void)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "WpaBlocklistClear");
    WriteEnd(context);
    if (RpcClientCall(client, "WpaBlocklistClear") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("WpaBlocklistClear failed!");
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo GetConnectSignalInfo(const char *endBssid, WpaSignalInfo *info)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "GetConnectSignalInfo");
    WriteStr(context, endBssid);
    WriteEnd(context);
    if (RpcClientCall(client, "GetConnectSignalInfo") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("GetConnectSignalInfo failed!");
    } else {
        ReadInt(context, &info->signal);
        ReadInt(context, &info->txrate);
        ReadInt(context, &info->rxrate);
        ReadInt(context, &info->noise);
        ReadInt(context, &info->frequency);
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}
