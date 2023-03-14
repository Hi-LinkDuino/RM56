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

#include "wifi_hal_crpc_sta.h"
#include "wifi_hal_crpc_base.h"
#include "wifi_hal_sta_interface.h"
#include "wifi_hal_define.h"

int RpcStart(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    WifiErrorNo err = Start();
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    return 0;
}

int RpcStop(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    WifiErrorNo err = Stop();
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    return 0;
}

int RpcStartScan(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    ScanSettings setting = {0};
    int ret = -1;
    do {
        if (ReadInt(context, &setting.hiddenSsidSize) < 0) {
            break;
        }
        if (setting.hiddenSsidSize > 0) {
            setting.hiddenSsid = ReadCharArray(context, setting.hiddenSsidSize);
            if (setting.hiddenSsid == NULL) {
                break;
            }
        }
        if (ReadInt(context, &setting.freqSize) < 0) {
            break;
        }
        if (setting.freqSize > 0) {
            setting.freqs = ReadIntArray(context, setting.freqSize);
            if (setting.freqs == NULL) {
                break;
            }
        }
        int temp = 0;
        if (ReadInt(context, &temp) < 0) {
            break;
        }
        setting.scanStyle = (ScanStyle)temp;

        ret += 1; /* deal success, set ret = 0 */
        WifiErrorNo err = StartScan(&setting);
        WriteBegin(context, 0);
        WriteInt(context, err);
        WriteEnd(context);
    } while (0);
    if (setting.hiddenSsid != NULL) {
        for (int i = 0; i < setting.hiddenSsidSize; ++i) {
            free(setting.hiddenSsid[i]);
            setting.hiddenSsid[i] = NULL;
        }
        free(setting.hiddenSsid);
        setting.hiddenSsid = NULL;
    }
    if (setting.freqs != NULL) {
        free(setting.freqs);
        setting.freqs = NULL;
    }
    return ret;
}

int RpcGetScanInfos(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int maxSize = 0;
    if (ReadInt(context, &maxSize) < 0) {
        return -1;
    }
    ScanInfo *results = NULL;
    if (maxSize > 0) {
        results = (ScanInfo *)calloc(maxSize, sizeof(ScanInfo));
    }
    if (results == NULL) {
        return -1;
    }
    WifiErrorNo err = GetScanInfos(results, &maxSize);
    WriteBegin(context, 0);
    WriteInt(context, err);
    struct timespec clockTime = {0, 0};
    const int secComplex = 1000;
    clock_gettime(CLOCK_MONOTONIC, &clockTime);
    if (err == WIFI_HAL_SUCCESS) {
        WriteInt(context, maxSize);
        for (int i = 0; i < maxSize; ++i) {
            WriteStr(context, results[i].bssid);
            WriteInt(context, results[i].freq);
            WriteInt(context, results[i].siglv);
            WriteStr(context, results[i].flags);
            WriteStr(context, results[i].ssid);
            int64_t currTime = (int64_t)clockTime.tv_sec * secComplex * secComplex + clockTime.tv_nsec / secComplex;
            WriteInt64(context, currTime);
            if (results[i].infoElems != NULL) {
                free(results[i].infoElems);
                results[i].infoElems = NULL;
            }
        }
    }
    WriteEnd(context);
    free(results);
    results = NULL;
    return 0;
}

static int ReadPnoScanSettings(Context *context, PnoScanSettings *pSetting)
{
    if (ReadInt(context, &pSetting->scanInterval) < 0 || ReadInt(context, &pSetting->minRssi2Dot4Ghz) < 0 ||
        ReadInt(context, &pSetting->minRssi5Ghz) < 0 || ReadInt(context, &pSetting->hiddenSsidSize) < 0) {
        return -1;
    }
    if (pSetting->hiddenSsidSize > 0) {
        pSetting->hiddenSsid = ReadCharArray(context, pSetting->hiddenSsidSize);
        if (pSetting->hiddenSsid == NULL) {
            return -1;
        }
    }
    if (ReadInt(context, &pSetting->savedSsidSize) < 0) {
        return -1;
    }
    if (pSetting->savedSsidSize > 0) {
        pSetting->savedSsid = ReadCharArray(context, pSetting->savedSsidSize);
        if (pSetting->savedSsid == NULL) {
            return -1;
        }
    }
    if (ReadInt(context, &pSetting->freqSize) < 0) {
        return -1;
    }
    if (pSetting->freqSize > 0) {
        pSetting->freqs = ReadIntArray(context, pSetting->freqSize);
        if (pSetting->freqs == NULL) {
            return -1;
        }
    }
    return 0;
}

int RpcStartPnoScan(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    PnoScanSettings setting = {0};
    int ret = ReadPnoScanSettings(context, &setting);
    if (ret == 0) {
        WifiErrorNo err = StartPnoScan(&setting);
        WriteBegin(context, 0);
        WriteInt(context, err);
        WriteEnd(context);
    }
    if (setting.hiddenSsid != NULL) {
        for (int i = 0; i < setting.hiddenSsidSize; ++i) {
            free(setting.hiddenSsid[i]);
            setting.hiddenSsid[i] = NULL;
        }
        free(setting.hiddenSsid);
        setting.hiddenSsid = NULL;
    }
    if (setting.savedSsid != NULL) {
        for (int i = 0; i < setting.savedSsidSize; ++i) {
            free(setting.savedSsid[i]);
            setting.savedSsid[i] = NULL;
        }
        free(setting.savedSsid);
        setting.savedSsid = NULL;
    }
    if (setting.freqs != NULL) {
        free(setting.freqs);
        setting.freqs = NULL;
    }

    return ret;
}

int RpcStopPnoScan(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    WifiErrorNo err = StopPnoScan();
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    return 0;
}

int RpcConnect(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int networkId = 0;
    if (ReadInt(context, &networkId) < 0) {
        return -1;
    }
    WifiErrorNo err = Connect(networkId);
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    return 0;
}

int RpcReconnect(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    WifiErrorNo err = Reconnect();
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    return 0;
}

int RpcReassociate(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    WifiErrorNo err = Reassociate();
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    return 0;
}

int RpcDisconnect(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    WifiErrorNo err = Disconnect();
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    return 0;
}

int RpcGetStaCapabilities(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int32_t capabilities = 0;
    WifiErrorNo err = GetStaCapabilities(&capabilities);
    WriteBegin(context, 0);
    WriteInt(context, err);
    if (err == WIFI_HAL_SUCCESS) {
        WriteInt(context, capabilities);
    }
    WriteEnd(context);
    return 0;
}

int RpcGetDeviceMacAddress(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int maxSize = 0;
    if (ReadInt(context, &maxSize) < 0 || maxSize <= 0) {
        return -1;
    }
    unsigned char *mac = NULL;
    if (maxSize > 0) {
        mac = (unsigned char *)calloc(maxSize + 1, sizeof(unsigned char));
    }
    if (mac == NULL) {
        return -1;
    }
    WifiErrorNo err = GetDeviceMacAddress(mac, &maxSize);
    WriteBegin(context, 0);
    WriteInt(context, err);
    if (err == WIFI_HAL_SUCCESS) {
        WriteInt(context, maxSize);
        WriteUStr(context, mac, maxSize);
    }
    WriteEnd(context);
    free(mac);
    mac = NULL;
    return 0;
}

int RpcGetFrequencies(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int band = 0;
    int maxSize = 0;
    if (ReadInt(context, &band) < 0 || ReadInt(context, &maxSize) < 0 || maxSize <= 0) {
        return -1;
    }
    int *frequencies = (int *)calloc(maxSize, sizeof(int));
    if (frequencies == NULL) {
        return -1;
    }
    WifiErrorNo err = GetFrequencies(band, frequencies, &maxSize);
    WriteBegin(context, 0);
    WriteInt(context, err);
    if (err == WIFI_HAL_SUCCESS) {
        WriteInt(context, maxSize);
        for (int i = 0; i < maxSize; ++i) {
            WriteInt(context, frequencies[i]);
        }
    }
    WriteEnd(context);
    free(frequencies);
    frequencies = NULL;
    return 0;
}

int RpcSetAssocMacAddr(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int maxSize = 0;
    if (ReadInt(context, &maxSize) < 0) {
        return -1;
    }
    int len = maxSize + 1;
    unsigned char *mac = (unsigned char *)calloc(len, sizeof(unsigned char));
    if (mac == NULL) {
        return -1;
    }
    if (ReadUStr(context, mac, len) != 0) {
        free(mac);
        mac = NULL;
        return -1;
    }
    WifiErrorNo err = SetAssocMacAddr(mac, maxSize);
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    free(mac);
    return 0;
}

int RpcSetScanningMacAddress(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int maxSize = 0;
    if (ReadInt(context, &maxSize) < 0 || maxSize < 0) {
        return -1;
    }
    int len = maxSize + 1;
    unsigned char *mac = (unsigned char *)calloc(len, sizeof(unsigned char));
    if (mac == NULL) {
        return -1;
    }
    if (ReadUStr(context, mac, len) != 0) {
        free(mac);
        return -1;
    }
    WifiErrorNo err = SetScanningMacAddress(mac, maxSize);
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    free(mac);
    return 0;
}

int RpcDeauthLastRoamingBssid(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int maxSize = 0;
    if (ReadInt(context, &maxSize) < 0 || maxSize < 0) {
        return -1;
    }
    int len = maxSize + 1;
    unsigned char *mac = (unsigned char *)calloc(len, sizeof(unsigned char));
    if (mac == NULL) {
        return -1;
    }
    if (ReadUStr(context, mac, len) != 0) {
        free(mac);
        return -1;
    }
    WifiErrorNo err = DeauthLastRoamingBssid(mac, maxSize);
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    free(mac);
    return 0;
}

int RpcGetSupportFeature(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    long feature = 0;
    WifiErrorNo err = GetSupportFeature(&feature);
    WriteBegin(context, 0);
    WriteInt(context, err);
    if (err == WIFI_HAL_SUCCESS) {
        WriteLong(context, feature);
    }
    WriteEnd(context);
    return 0;
}

int RpcRunCmd(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    char ifname[WIFI_IFACE_NAME_MAXLEN] = {0};
    char *pIfName = NULL;
    int ret = ReadStr(context, ifname, WIFI_IFACE_NAME_MAXLEN);
    if (ret < 0) {
        return -1;
    } else if (ret > 0) {
        int len = ret + 1;
        pIfName = (char *)calloc(len, sizeof(char));
        if (pIfName == NULL) {
            return -1;
        }
        ReadStr(context, pIfName, len);
    }
    int cmdid = 0;
    int bufsize = 0;
    if (ReadInt(context, &cmdid) < 0 || ReadInt(context, &bufsize) < 0 || bufsize < 0) {
        free(pIfName);
        pIfName = NULL;
        return -1;
    }
    int len = bufsize + 1;
    unsigned char *buf = (unsigned char *)calloc(len, sizeof(unsigned char));
    if (buf == NULL) {
        free(pIfName);
        pIfName = NULL;
        return -1;
    }
    if (ReadUStr(context, buf, len) != 0) {
        free(pIfName);
        free(buf);
        pIfName = NULL;
        buf = NULL;
        return -1;
    }
    WifiErrorNo err = RunCmd((pIfName == NULL) ? ifname : pIfName, cmdid, buf, bufsize);
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    free(pIfName);
    free(buf);
    pIfName = NULL;
    buf = NULL;
    return 0;
}

int RpcSetWifiTxPower(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int32_t power = 0;
    if (ReadInt(context, &power) < 0) {
        return -1;
    }
    WifiErrorNo err = SetWifiTxPower(power);
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    return 0;
}

int RpcRemoveNetwork(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int32_t networkId = 0;
    if (ReadInt(context, &networkId) < 0) {
        return -1;
    }
    WifiErrorNo err = RemoveNetwork(networkId);
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    return 0;
}

int RpcAddNetwork(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int networkId = 0;
    WifiErrorNo err = AddNetwork(&networkId);
    WriteBegin(context, 0);
    WriteInt(context, err);
    if (err == WIFI_HAL_SUCCESS) {
        WriteInt(context, networkId);
    }
    WriteEnd(context);
    return 0;
}

int RpcEnableNetwork(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int networkId = 0;
    if (ReadInt(context, &networkId) < 0) {
        return -1;
    }
    WifiErrorNo err = EnableNetwork(networkId);
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    return 0;
}

int RpcDisableNetwork(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int networkId = 0;
    if (ReadInt(context, &networkId) < 0) {
        return -1;
    }
    WifiErrorNo err = DisableNetwork(networkId);
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    return 0;
}

int RpcSetNetwork(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int networkId = 0;
    if (ReadInt(context, &networkId) < 0) {
        return -1;
    }
    int size = 0;
    if (ReadInt(context, &size) < 0) {
        return -1;
    }
    if (size <= 0) {
        return -1;
    }

    SetNetworkConfig *confs = (SetNetworkConfig *)calloc(size, sizeof(SetNetworkConfig));
    if (confs == NULL) {
        return -1;
    }

    int flag = 0;
    for (int i = 0; i < size; ++i) {
        if (ReadInt(context, (int *)&(confs[i].cfgParam)) < 0 ||
            ReadStr(context, confs[i].cfgValue, sizeof(confs[i].cfgValue)) != 0) {
            flag = 1;
            break;
        }
    }
    WifiErrorNo err = WIFI_HAL_FAILED;
    if (flag == 0) {
        err = SetNetwork(networkId, confs, size);
    }
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    free(confs);
    confs = NULL;
    return 0;
}

int RpcSaveNetworkConfig(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    WifiErrorNo err = SaveNetworkConfig();
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    return 0;
}

int RpcStartWpsPbcMode(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    WifiWpsParam param = {0};
    if (ReadInt(context, &param.anyFlag) < 0) {
        return -1;
    }
    if (ReadInt(context, &param.multiAp) < 0) {
        return -1;
    }
    if (ReadStr(context, param.bssid, sizeof(param.bssid)) != 0) {
        return -1;
    }
    WifiErrorNo err = StartWpsPbcMode(&param);
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    return 0;
}

int RpcStartWpsPinMode(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    WifiWpsParam param = {0};
    if (ReadInt(context, &param.anyFlag) < 0) {
        return -1;
    }
    if (ReadInt(context, &param.multiAp) < 0) {
        return -1;
    }
    if (ReadStr(context, param.bssid, sizeof(param.bssid)) != 0) {
        return -1;
    }
    int pinCode = 0;
    WifiErrorNo err = StartWpsPinMode(&param, &pinCode);
    WriteBegin(context, 0);
    WriteInt(context, err);
    if (err == WIFI_HAL_SUCCESS) {
        WriteInt(context, pinCode);
    }
    WriteEnd(context);
    return 0;
}

int RpcStopWps(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    WifiErrorNo err = StopWps();
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    return 0;
}

int RpcGetRoamingCapabilities(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    WifiRoamCapability capability = {0};
    WifiErrorNo err = GetRoamingCapabilities(&capability);
    WriteBegin(context, 0);
    WriteInt(context, err);
    if (err == WIFI_HAL_SUCCESS) {
        WriteInt(context, capability.maxBlocklistSize);
        WriteInt(context, capability.maxTrustlistSize);
    }
    WriteEnd(context);
    return 0;
}

static char **ReadRoamBlockList(Context *context, int size)
{
    if (size <= 0) {
        return NULL;
    }

    char **list = (char **)calloc(size, sizeof(char *));
    if (list == NULL) {
        return NULL;
    }
    int i = 0;
    for (; i < size; ++i) {
        int len = ReadStr(context, NULL, 0);
        if (len < 0) {
            break;
        }
        ++len;
        list[i] = (char *)calloc(len, sizeof(char));
        if (list[i] == NULL) {
            break;
        }
        ReadStr(context, list[i], len);
    }
    if (i < size) {
        for (int j = 0; j <= i; ++j) {
            free(list[j]);
            list[j] = NULL;
        }
        free(list);
        list = NULL;
        return NULL;
    } else {
        return list;
    }
}

int RpcSetRoamConfig(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int blocksize = 0;
    char **blocklist = NULL;
    int size = 0;
    char **trustlist = NULL;
    int ret = -1;

    do {
        if (ReadInt(context, &blocksize) < 0) {
            break;
        }
        if (blocksize > 0) {
            blocklist = ReadRoamBlockList(context, blocksize);
            if (blocklist == NULL) {
                break;
            }
        }

        if (ReadInt(context, &size) < 0) {
            break;
        }
        if (size > 0) {
            trustlist = ReadRoamBlockList(context, size);
            if (trustlist == NULL) {
                break;
            }
        }
        ret += 1; /* deal success, set ret = 0 */
        WifiErrorNo err = SetRoamConfig(blocklist, blocksize, trustlist, size);
        WriteBegin(context, 0);
        WriteInt(context, err);
        WriteEnd(context);
    } while (0);
    if (blocklist != NULL) {
        for (int i = 0; i < blocksize; ++i) {
            free(blocklist[i]);
            blocklist[i] = NULL;
        }
        free(blocklist);
        blocklist = NULL;
    }
    if (trustlist != NULL) {
        for (int i = 0; i < size; ++i) {
            free(trustlist[i]);
            trustlist[i] = NULL;
        }
        free(trustlist);
        trustlist = NULL;
    }
    return ret;
}

int RpcWpaGetNetwork(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    GetNetworkConfig conf = {0};
    if (ReadInt(context, &(conf.networkId)) < 0 || ReadStr(context, conf.param, sizeof(conf.param)) != 0) {
        return -1;
    }
    WifiErrorNo err = WpaGetNetWork(&conf);
    WriteBegin(context, 0);
    WriteInt(context, err);
    if (err == 0) {
        WriteStr(context, conf.value);
    }
    WriteEnd(context);
    return 0;
}

int RpcWpaAutoConnect(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int enable = 0;
    if (ReadInt(context, &enable) < 0) {
        return -1;
    }
    WifiErrorNo err = WpaAutoConnect(enable);
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    return 0;
}

int RpcWpaBlocklistClear(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    WifiErrorNo err = WpaBlocklistClear();
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    return 0;
}

int RpcGetNetworkList(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int maxSize = 0;
    if (ReadInt(context, &maxSize) < 0) {
        return -1;
    }
    if (maxSize <= 0) {
        return -1;
    }

    WifiNetworkInfo *infos = (WifiNetworkInfo *)calloc(maxSize, sizeof(WifiNetworkInfo));
    if (infos == NULL) {
        return -1;
    }

    WifiErrorNo err = GetNetworkList(infos, &maxSize);
    WriteBegin(context, 0);
    WriteInt(context, err);
    if (err == WIFI_HAL_SUCCESS) {
        WriteInt(context, maxSize);
        for (int i = 0; i < maxSize; ++i) {
            WriteInt(context, infos[i].id);
            WriteStr(context, infos[i].ssid);
            WriteStr(context, infos[i].bssid);
            WriteStr(context, infos[i].flags);
        }
    }
    WriteEnd(context);
    free(infos);
    infos = NULL;
    return 0;
}

int RpcGetConnectSignalInfo(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    char endBssid[WIFI_BSSID_LENGTH] = {0};
    if (ReadStr(context, endBssid, sizeof(endBssid)) != 0) {
        return -1;
    }
    WpaSignalInfo info = {0};
    WifiErrorNo err = GetConnectSignalInfo(endBssid, &info);
    WriteBegin(context, 0);
    WriteInt(context, err);
    if (err == WIFI_HAL_SUCCESS) {
        WriteInt(context, info.signal);
        WriteInt(context, info.txrate);
        WriteInt(context, info.rxrate);
        WriteInt(context, info.noise);
        WriteInt(context, info.frequency);
    }
    WriteEnd(context);
    return 0;
}