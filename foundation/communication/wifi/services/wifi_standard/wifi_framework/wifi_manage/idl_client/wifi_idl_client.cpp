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

#include "wifi_idl_client.h"
#include <cstdio>
#include "wifi_global_func.h"
#include "wifi_log.h"
#include "wifi_idl_define.h"
#include "wifi_idl_inner_interface.h"
#include "i_wifi.h"
#include "i_wifi_chip.h"
#include "i_wifi_chip_event_callback.h"
#include "i_wifi_hotspot_iface.h"
#include "i_wifi_sta_iface.h"
#include "i_wifi_supplicant_iface.h"
#include "i_wifi_p2p_iface.h"

#undef LOG_TAG
#define LOG_TAG "WifiIdlClient"

namespace OHOS {
namespace Wifi {
const int BUFFER_SIZE = 4096;

#define CHECK_CLIENT_NOT_NULL           \
    do {                                \
        if (pRpcClient == nullptr) {    \
            return WIFI_IDL_OPT_FAILED; \
        }                               \
    } while (0)

WifiIdlClient::WifiIdlClient()
{
    pRpcClient = nullptr;
}

WifiIdlClient::~WifiIdlClient()
{
    if (pRpcClient != nullptr) {
        ReleaseRpcClient(pRpcClient);
        pRpcClient = nullptr;
    }
}

int WifiIdlClient::InitClient(void)
{
    const std::string idlSockPath = "/data/misc/wifi/unix_sock.sock";
    pRpcClient = CreateRpcClient(idlSockPath.c_str());
    if (pRpcClient == nullptr) {
        LOGE("init rpc client failed!");
        return -1;
    }
    return 0;
}

void WifiIdlClient::ExitAllClient(void)
{
    if (pRpcClient == nullptr) {
        return;
    }
    NotifyClear();
    return;
}

WifiErrorNo WifiIdlClient::StartWifi(void)
{
    CHECK_CLIENT_NOT_NULL;
    return Start();
}

WifiErrorNo WifiIdlClient::StopWifi(void)
{
    CHECK_CLIENT_NOT_NULL;
    return Stop();
}

WifiErrorNo WifiIdlClient::ReqConnect(int networkId)
{
    CHECK_CLIENT_NOT_NULL;
    return Connect(networkId);
}

WifiErrorNo WifiIdlClient::ReqReconnect(void)
{
    CHECK_CLIENT_NOT_NULL;
    return Reconnect();
}

WifiErrorNo WifiIdlClient::ReqReassociate(void)
{
    CHECK_CLIENT_NOT_NULL;
    return Reassociate();
}

WifiErrorNo WifiIdlClient::ReqDisconnect(void)
{
    CHECK_CLIENT_NOT_NULL;
    return Disconnect();
}

WifiErrorNo WifiIdlClient::GetStaCapabilities(unsigned int &capabilities)
{
    CHECK_CLIENT_NOT_NULL;
    return GetCapabilities((uint32_t *)&capabilities);
}

WifiErrorNo WifiIdlClient::GetStaDeviceMacAddress(std::string &mac)
{
    CHECK_CLIENT_NOT_NULL;
    char szMac[WIFI_IDL_BSSID_LENGTH + 1] = {0};
    int len = WIFI_IDL_BSSID_LENGTH + 1;
    WifiErrorNo err = GetDeviceMacAddress((unsigned char *)szMac, &len);
    if (err == WIFI_IDL_OPT_OK) {
        mac = std::string(szMac);
    }
    return err;
}

WifiErrorNo WifiIdlClient::GetSupportFrequencies(int band, std::vector<int> &frequencies)
{
    CHECK_CLIENT_NOT_NULL;

    int values[WIFI_IDL_GET_MAX_BANDS] = {0};
    int size = WIFI_IDL_GET_MAX_BANDS;

    if (GetFrequencies(band, values, &size) != 0) {
        return WIFI_IDL_OPT_FAILED;
    }

    for (int i = 0; i < size; i++) {
        frequencies.push_back(values[i]);
    }

    return WIFI_IDL_OPT_OK;
}

WifiErrorNo WifiIdlClient::SetConnectMacAddr(const std::string &mac)
{
    CHECK_CLIENT_NOT_NULL;
    if (CheckMacIsValid(mac) != 0) {
        return WIFI_IDL_OPT_INPUT_MAC_INVALID;
    }
    int len = mac.length();
    return SetAssocMacAddr((unsigned char *)mac.c_str(), len);
}

WifiErrorNo WifiIdlClient::SetScanMacAddress(const std::string &mac)
{
    CHECK_CLIENT_NOT_NULL;
    if (CheckMacIsValid(mac) != 0) {
        return WIFI_IDL_OPT_INPUT_MAC_INVALID;
    }
    int len = mac.length();
    return SetScanningMacAddress((unsigned char *)mac.c_str(), len);
}

WifiErrorNo WifiIdlClient::DisconnectLastRoamingBssid(const std::string &mac)
{
    CHECK_CLIENT_NOT_NULL;
    if (CheckMacIsValid(mac) != 0) {
        return WIFI_IDL_OPT_INPUT_MAC_INVALID;
    }
    int len = mac.length();
    return DeauthLastRoamingBssid((unsigned char *)mac.c_str(), len);
}

WifiErrorNo WifiIdlClient::ReqGetSupportFeature(long &feature)
{
    CHECK_CLIENT_NOT_NULL;
    return GetSupportFeature(&feature);
}

WifiErrorNo WifiIdlClient::SendRequest(const WifiStaRequest &request)
{
    CHECK_CLIENT_NOT_NULL;
    /* RunCmd */
    LOGD("Start run cmd %{public}d about iface %s", request.cmdId, request.ifName.c_str());
    return WIFI_IDL_OPT_OK;
}

WifiErrorNo WifiIdlClient::SetTxPower(int power)
{
    CHECK_CLIENT_NOT_NULL;
    return SetWifiTxPower((int32_t)power);
}

WifiErrorNo WifiIdlClient::Scan(const WifiScanParam &scanParam)
{
    CHECK_CLIENT_NOT_NULL;
    ScanSettings settings;
    if (memset_s(&settings, sizeof(settings), 0, sizeof(settings)) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    bool bfail = false;
    do {
        if (scanParam.hiddenNetworkSsid.size() > 0) {
            settings.hiddenSsidSize = scanParam.hiddenNetworkSsid.size();
            settings.hiddenSsid = ConVectorToCArrayString(scanParam.hiddenNetworkSsid);
            if (settings.hiddenSsid == nullptr) {
                bfail = true;
                break;
            }
        }
        if (scanParam.scanFreqs.size() > 0) {
            settings.freqSize = scanParam.scanFreqs.size();
            settings.freqs = (int *)calloc(settings.freqSize, sizeof(int));
            if (settings.freqs == nullptr) {
                bfail = true;
                break;
            }
            for (int i = 0; i < settings.freqSize; ++i) {
                settings.freqs[i] = scanParam.scanFreqs[i];
            }
        }
        if (scanParam.scanStyle > 0) {
            settings.scanStyle = scanParam.scanStyle;
        }
    } while (0);
    WifiErrorNo err = WIFI_IDL_OPT_FAILED;
    if (!bfail) {
        err = StartScan(&settings);
    }
    if (settings.freqs != nullptr) {
        free(settings.freqs);
        settings.freqs = nullptr;
    }
    if (settings.hiddenSsid != nullptr) {
        for (int i = 0; i < settings.hiddenSsidSize; ++i) {
            free(settings.hiddenSsid[i]);
            settings.hiddenSsid[i] = nullptr;
        }
        free(settings.hiddenSsid);
        settings.hiddenSsid = nullptr;
    }
    return err;
}

WifiErrorNo WifiIdlClient::ReqGetNetworkList(std::vector<WifiWpaNetworkInfo> &networkList)
{
    CHECK_CLIENT_NOT_NULL;
    WifiNetworkInfo infos[WIFI_IDL_GET_MAX_NETWORK_LIST];
    if (memset_s(infos, sizeof(infos), 0, sizeof(infos)) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int size = WIFI_IDL_GET_MAX_NETWORK_LIST;
    WifiErrorNo err = GetNetworkList(infos, &size);
    if (err != WIFI_IDL_OPT_OK) {
        return err;
    }
    for (int i = 0; i < size; ++i) {
        WifiWpaNetworkInfo tmp;
        tmp.id = infos[i].id;
        tmp.ssid = infos[i].ssid;
        tmp.bssid = infos[i].bssid;
        tmp.flag = infos[i].flags;
        networkList.emplace_back(tmp);
    }
    return err;
}

WifiErrorNo WifiIdlClient::QueryScanInfos(std::vector<InterScanInfo> &scanInfos)
{
    CHECK_CLIENT_NOT_NULL;
    int size = WIFI_IDL_GET_MAX_SCAN_INFO;
    ScanInfo* results = GetScanInfos(&size);
    if (results == NULL) {
        return size == 0 ? WIFI_IDL_OPT_OK : WIFI_IDL_OPT_FAILED;
    }
    for (int i = 0; i < size; ++i) {
        InterScanInfo tmp;
        tmp.ssid = results[i].ssid;
        tmp.bssid = results[i].bssid;
        tmp.frequency = results[i].frequency;
        tmp.rssi = results[i].signalLevel;
        tmp.timestamp = results[i].timestamp;
        tmp.capabilities = results[i].capability;
        scanInfos.emplace_back(tmp);
    }
    free(results);
    results = nullptr;
    return WIFI_IDL_OPT_OK;
}

WifiErrorNo WifiIdlClient::ConvertPnoScanParam(const WifiPnoScanParam &param, PnoScanSettings *pSettings) const
{
    if (param.scanInterval > 0) {
        pSettings->scanInterval = param.scanInterval;
    }
    pSettings->minRssi2Dot4Ghz = param.minRssi2Dot4Ghz;
    pSettings->minRssi5Ghz = param.minRssi5Ghz;
    if (param.hiddenSsid.size() > 0) {
        pSettings->hiddenSsidSize = param.hiddenSsid.size();
        pSettings->hiddenSsid = ConVectorToCArrayString(param.hiddenSsid);
        if (pSettings->hiddenSsid == nullptr) {
            return WIFI_IDL_OPT_FAILED;
        }
    }
    if (param.savedSsid.size() > 0) {
        pSettings->savedSsidSize = param.savedSsid.size();
        pSettings->savedSsid = ConVectorToCArrayString(param.savedSsid);
        if (pSettings->savedSsid == nullptr) {
            return WIFI_IDL_OPT_FAILED;
        }
    }
    if (param.scanFreqs.size() > 0) {
        pSettings->freqSize = param.scanFreqs.size();
        pSettings->freqs = (int *)calloc(pSettings->freqSize, sizeof(int));
        if (pSettings->freqs == nullptr) {
            return WIFI_IDL_OPT_FAILED;
        }
        for (int i = 0; i < pSettings->freqSize; ++i) {
            pSettings->freqs[i] = param.scanFreqs[i];
        }
    }
    return WIFI_IDL_OPT_OK;
}

WifiErrorNo WifiIdlClient::ReqStartPnoScan(const WifiPnoScanParam &scanParam)
{
    CHECK_CLIENT_NOT_NULL;
    PnoScanSettings settings;
    if (memset_s(&settings, sizeof(settings), 0, sizeof(settings)) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    WifiErrorNo err = ConvertPnoScanParam(scanParam, &settings);
    if (err == WIFI_IDL_OPT_OK) {
        err = StartPnoScan(&settings);
    }
    if (settings.freqs != nullptr) {
        free(settings.freqs);
        settings.freqs = nullptr;
    }
    if (settings.hiddenSsid != nullptr) {
        for (int i = 0; i < settings.hiddenSsidSize; ++i) {
            free(settings.hiddenSsid[i]);
            settings.hiddenSsid[i] = nullptr;
        }
        free(settings.hiddenSsid);
        settings.hiddenSsid = nullptr;
    }
    if (settings.savedSsid != nullptr) {
        for (int i = 0; i < settings.savedSsidSize; ++i) {
            free(settings.savedSsid[i]);
            settings.savedSsid[i] = nullptr;
        }
        free(settings.savedSsid);
        settings.savedSsid = nullptr;
    }
    return err;
}

WifiErrorNo WifiIdlClient::ReqStopPnoScan(void)
{
    CHECK_CLIENT_NOT_NULL;
    return StopPnoScan();
}

WifiErrorNo WifiIdlClient::RemoveDevice(int networkId)
{
    CHECK_CLIENT_NOT_NULL;
    if (networkId < 0) {
        return WIFI_IDL_OPT_INVALID_PARAM;
    }
    return RemoveNetwork(networkId);
}

WifiErrorNo WifiIdlClient::ClearDeviceConfig(void) const
{
    CHECK_CLIENT_NOT_NULL;
    return RemoveNetwork(-1);
}

WifiErrorNo WifiIdlClient::GetNextNetworkId(int &networkId)
{
    CHECK_CLIENT_NOT_NULL;
    return AddNetwork(&networkId);
}

WifiErrorNo WifiIdlClient::ReqEnableNetwork(int networkId)
{
    CHECK_CLIENT_NOT_NULL;
    return EnableNetwork(networkId);
}

WifiErrorNo WifiIdlClient::ReqDisableNetwork(int networkId)
{
    CHECK_CLIENT_NOT_NULL;
    return DisableNetwork(networkId);
}

WifiErrorNo WifiIdlClient::GetDeviceConfig(WifiIdlGetDeviceConfig &config)
{
    CHECK_CLIENT_NOT_NULL;
    GetNetworkConfig conf;
    if (memset_s(&conf, sizeof(conf), 0, sizeof(conf)) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    conf.networkId = config.networkId;
    if (strncpy_s(conf.param, sizeof(conf.param), config.param.c_str(), config.param.length()) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int ret = WpaGetNetwork(&conf);
    if (ret != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    config.value = conf.value;
    return WIFI_IDL_OPT_OK;
}

int WifiIdlClient::PushDeviceConfigString(
    SetNetworkConfig *pConfig, DeviceConfigType type, const std::string &msg) const
{
    if (msg.length() > 0) {
        pConfig->cfgParam = type;
        if (strncpy_s(pConfig->cfgValue, sizeof(pConfig->cfgValue), msg.c_str(), msg.length()) != EOK) {
            return 0;
        }
        return 1;
    } else {
        return 0;
    }
}

int WifiIdlClient::PushDeviceConfigInt(SetNetworkConfig *pConfig, DeviceConfigType type, int i) const
{
    pConfig->cfgParam = type;
    if (snprintf_s(pConfig->cfgValue, sizeof(pConfig->cfgValue), sizeof(pConfig->cfgValue) - 1, "%d", i) < 0) {
        return 0;
    }
    return 1;
}

int WifiIdlClient::PushDeviceConfigAuthAlgorithm(
    SetNetworkConfig *pConfig, DeviceConfigType type, unsigned int alg) const
{
    pConfig->cfgParam = type;
    if (alg & 0x1) {
        if (strcat_s(pConfig->cfgValue, sizeof(pConfig->cfgValue), "OPEN ") != EOK) {
            return 0;
        }
    }
    if (alg & 0x2) {
        if (strcat_s(pConfig->cfgValue, sizeof(pConfig->cfgValue), "SHARED ") != EOK) {
            return 0;
        }
    }
    if (alg & 0x4) {
        if (strcat_s(pConfig->cfgValue, sizeof(pConfig->cfgValue), "LEAP ") != EOK) {
            return 0;
        }
    }
    return 1;
}

WifiErrorNo WifiIdlClient::CheckValidDeviceConfig(const WifiIdlDeviceConfig &config) const
{
    if (config.psk.length() > 0) {
        if (config.psk.length() < WIFI_IDL_PSK_MIN_LENGTH || config.psk.length() > WIFI_IDL_PSK_MAX_LENGTH) {
            return WIFI_IDL_OPT_FAILED;
        }
    }
    if (config.authAlgorithms >= AUTH_ALGORITHM_MAX) { /* max is 0111 */
        return WIFI_IDL_OPT_FAILED;
    }
    return WIFI_IDL_OPT_OK;
}

WifiErrorNo WifiIdlClient::SetDeviceConfig(int networkId, const WifiIdlDeviceConfig &config)
{
    CHECK_CLIENT_NOT_NULL;
    if (CheckValidDeviceConfig(config) != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    SetNetworkConfig conf[DEVICE_CONFIG_END_POS];
    if (memset_s(conf, sizeof(conf), 0, sizeof(conf)) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int num = 0;
    num += PushDeviceConfigString(conf + num, DEVICE_CONFIG_SSID, config.ssid);
    num += PushDeviceConfigString(conf + num, DEVICE_CONFIG_PSK, config.psk);
    num += PushDeviceConfigString(conf + num, DEVICE_CONFIG_KEYMGMT, config.keyMgmt);
    num += PushDeviceConfigString(conf + num, DEVICE_CONFIG_EAP, config.eap);
    num += PushDeviceConfigString(conf + num, DEVICE_CONFIG_IDENTITY, config.identity);
    num += PushDeviceConfigString(conf + num, DEVICE_CONFIG_PASSWORD, config.password);
    num += PushDeviceConfigString(conf + num, DEVICE_CONFIG_BSSID, config.bssid);
    int i = 0;
    num += PushDeviceConfigString(conf + num, DEVICE_CONFIG_WEP_KEY_0, config.wepKeys[i++]);
    num += PushDeviceConfigString(conf + num, DEVICE_CONFIG_WEP_KEY_1, config.wepKeys[i++]);
    num += PushDeviceConfigString(conf + num, DEVICE_CONFIG_WEP_KEY_2, config.wepKeys[i++]);
    num += PushDeviceConfigString(conf + num, DEVICE_CONFIG_WEP_KEY_3, config.wepKeys[i++]);
    if (config.priority >= 0) {
        num += PushDeviceConfigInt(conf + num, DEVICE_CONFIG_PRIORITY, config.priority);
    }
    if (config.scanSsid == 1) {
        num += PushDeviceConfigInt(conf + num, DEVICE_CONFIG_SCAN_SSID, config.scanSsid);
    }
    if (config.wepKeyIdx >= 0) {
        num += PushDeviceConfigInt(conf + num, DEVICE_CONFIG_WEP_KEY_IDX, config.wepKeyIdx);
    }
    if (config.authAlgorithms > 0) {
        num += PushDeviceConfigAuthAlgorithm(conf + num, DEVICE_CONFIG_AUTH_ALGORITHMS, config.authAlgorithms);
    }
    if (num == 0) {
        return WIFI_IDL_OPT_OK;
    }
    return SetNetwork(networkId, conf, num);
}

WifiErrorNo WifiIdlClient::SaveDeviceConfig(void)
{
    CHECK_CLIENT_NOT_NULL;
    return SaveNetworkConfig();
}

WifiErrorNo WifiIdlClient::ReqRegisterStaEventCallback(const WifiEventCallback &callback)
{
    CHECK_CLIENT_NOT_NULL;
    IWifiEventCallback cEventCallback;
    if (memset_s(&cEventCallback, sizeof(cEventCallback), 0, sizeof(cEventCallback)) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    if (callback.onConnectChanged != nullptr) {
        cEventCallback.onConnectChanged = OnConnectChanged;
        cEventCallback.onWpaStateChanged = OnWpaStateChanged;
        cEventCallback.onSsidWrongkey = OnWpaSsidWrongKey;
        cEventCallback.onWpsOverlap = OnWpsOverlap;
        cEventCallback.onWpsTimeOut = OnWpsTimeOut;
        cEventCallback.onWpsConnectionFull = onWpaConnectionFull;
        cEventCallback.onWpsConnectionReject = onWpaConnectionReject;
    }
    return RegisterStaEventCallback(cEventCallback);
}

WifiErrorNo WifiIdlClient::ReqStartWpsPbcMode(const WifiIdlWpsConfig &config)
{
    CHECK_CLIENT_NOT_NULL;
    WifiWpsParam param;
    if (memset_s(&param, sizeof(param), 0, sizeof(param)) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    param.anyFlag = config.anyFlag;
    param.multiAp = config.multiAp;
    if (strncpy_s(param.bssid, sizeof(param.bssid), config.bssid.c_str(), config.bssid.length()) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    return StartWpsPbcMode(&param);
}

WifiErrorNo WifiIdlClient::ReqStartWpsPinMode(const WifiIdlWpsConfig &config, int &pinCode)
{
    CHECK_CLIENT_NOT_NULL;
    WifiWpsParam param;
    if (memset_s(&param, sizeof(param), 0, sizeof(param)) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    param.anyFlag = config.anyFlag;
    param.multiAp = config.multiAp;
    if (strncpy_s(param.bssid, sizeof(param.bssid), config.bssid.c_str(), config.bssid.length()) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    if (!config.pinCode.empty()) {
        if (strncpy_s(param.pinCode, sizeof(param.pinCode), config.pinCode.c_str(), config.pinCode.length()) != EOK) {
            return WIFI_IDL_OPT_FAILED;
        }
    }
    return StartWpsPinMode(&param, &pinCode);
}

WifiErrorNo WifiIdlClient::ReqStopWps()
{
    CHECK_CLIENT_NOT_NULL;
    return StopWps();
}

WifiErrorNo WifiIdlClient::ReqGetRoamingCapabilities(WifiIdlRoamCapability &capability)
{
    CHECK_CLIENT_NOT_NULL;
    WifiRoamCapability tmp;
    if (memset_s(&tmp, sizeof(tmp), 0, sizeof(tmp)) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    WifiErrorNo err = GetRoamingCapabilities(&tmp);
    if (err == WIFI_IDL_OPT_OK) {
        capability.maxBlocklistSize = tmp.maxBlocklistSize;
        capability.maxTrustlistSize = tmp.maxTrustlistSize;
    }
    return err;
}

char **WifiIdlClient::ConVectorToCArrayString(const std::vector<std::string> &vec) const
{
    int size = vec.size();
    if (size == 0) {
        return nullptr;
    }
    char **list = (char **)calloc(size, sizeof(char *));
    if (list == nullptr) {
        return nullptr;
    }
    int i = 0;
    for (; i < size; ++i) {
        int len = vec[i].length();
        list[i] = (char *)calloc(len + 1, sizeof(char));
        if (list[i] == nullptr) {
            break;
        }
        if (strncpy_s(list[i], len + 1, vec[i].c_str(), len) != EOK) {
            break;
        }
    }
    if (i < size) {
        for (int j = 0; j <= i; ++j) {
            free(list[j]);
        }
        free(list);
        list = nullptr;
        return nullptr;
    } else {
        return list;
    }
}

WifiErrorNo WifiIdlClient::ReqSetRoamConfig(const WifiIdlRoamConfig &config)
{
    CHECK_CLIENT_NOT_NULL;
    char **blocklist = nullptr;
    int blocksize = config.blocklistBssids.size();
    char **trustlist = nullptr;
    int trustsize = config.trustlistBssids.size();
    if (blocksize == 0 && trustsize == 0) {
        return WIFI_IDL_OPT_FAILED;
    }
    WifiErrorNo err = WIFI_IDL_OPT_FAILED;
    do {
        if (blocksize > 0) {
            blocklist = ConVectorToCArrayString(config.blocklistBssids);
            if (blocklist == nullptr) {
                break;
            }
        }
        if (trustsize > 0) {
            trustlist = ConVectorToCArrayString(config.trustlistBssids);
            if (trustlist == nullptr) {
                break;
            }
        }
        err = SetRoamConfig(blocklist, blocksize, trustlist, trustsize);
    } while (0);
    if (blocklist != nullptr) {
        for (int i = 0; i < blocksize; ++i) {
            free(blocklist[i]);
            blocklist[i] = nullptr;
        }
        free(blocklist);
        blocklist = nullptr;
    }
    if (trustlist != nullptr) {
        for (int i = 0; i < trustsize; ++i) {
            free(trustlist[i]);
            trustlist[i] = nullptr;
        }
        free(trustlist);
        trustlist = nullptr;
    }
    return err;
}

WifiErrorNo WifiIdlClient::ReqGetConnectSignalInfo(const std::string &endBssid, WifiWpaSignalInfo &info) const
{
    CHECK_CLIENT_NOT_NULL;
    WpaSignalInfo req = {0};
    WifiErrorNo err = GetConnectSignalInfo(endBssid.c_str(), &req);
    if (err == WIFI_IDL_OPT_OK) {
        info.signal = req.signal;
        info.txrate = req.txrate;
        info.rxrate = req.rxrate;
        info.noise = req.noise;
        info.frequency = req.frequency;
    }
    return err;
}

WifiErrorNo WifiIdlClient::StartAp(void)
{
    CHECK_CLIENT_NOT_NULL;
    return StartSoftAp();
}

WifiErrorNo WifiIdlClient::StopAp(void)
{
    CHECK_CLIENT_NOT_NULL;
    return StopSoftAp();
}

WifiErrorNo WifiIdlClient::SetSoftApConfig(const HotspotConfig &config)
{
    CHECK_CLIENT_NOT_NULL;
    HostapdConfig tmp;
    if (memset_s(&tmp, sizeof(tmp), 0, sizeof(tmp)) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    tmp.ssidLen = config.GetSsid().length();
    if (strncpy_s(tmp.ssid, sizeof(tmp.ssid), config.GetSsid().c_str(), tmp.ssidLen) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    std::string preSharedKey = config.GetPreSharedKey();
    tmp.preSharedKeyLen = preSharedKey.length();
    if (strncpy_s(tmp.preSharedKey, sizeof(tmp.preSharedKey), preSharedKey.c_str(), tmp.preSharedKeyLen) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    tmp.securityType = static_cast<int>(config.GetSecurityType());
    tmp.band = static_cast<int>(config.GetBand());
    tmp.channel = config.GetChannel();
    tmp.maxConn = config.GetMaxConn();
    return SetHostapdConfig(&tmp);
}

WifiErrorNo WifiIdlClient::GetStationList(std::vector<std::string> &result)
{
    CHECK_CLIENT_NOT_NULL;

    char *staInfos = new (std::nothrow) char[BUFFER_SIZE]();
    if (staInfos == nullptr) {
        return WIFI_IDL_OPT_FAILED;
    }
    int32_t size = BUFFER_SIZE;
    WifiErrorNo err = GetStaInfos(staInfos, &size);
    if (err != WIFI_IDL_OPT_OK) {
        delete[] staInfos;
        return WIFI_IDL_OPT_FAILED;
    }
    std::string strStaInfo = std::string(staInfos);
    SplitString(strStaInfo, ",", result);
    delete[] staInfos;
    return WIFI_IDL_OPT_OK;
}

WifiErrorNo WifiIdlClient::AddBlockByMac(const std::string &mac)
{
    CHECK_CLIENT_NOT_NULL;
    if (CheckMacIsValid(mac) != 0) {
        return WIFI_IDL_OPT_INPUT_MAC_INVALID;
    }
    int len = mac.length();
    return SetMacFilter((unsigned char *)mac.c_str(), len);
}

WifiErrorNo WifiIdlClient::DelBlockByMac(const std::string &mac)
{
    CHECK_CLIENT_NOT_NULL;
    if (CheckMacIsValid(mac) != 0) {
        return WIFI_IDL_OPT_INPUT_MAC_INVALID;
    }
    int len = mac.length();
    return DelMacFilter((unsigned char *)mac.c_str(), len);
}

WifiErrorNo WifiIdlClient::RemoveStation(const std::string &mac)
{
    CHECK_CLIENT_NOT_NULL;
    if (CheckMacIsValid(mac) != 0) {
        return WIFI_IDL_OPT_INPUT_MAC_INVALID;
    }
    int len = mac.length();
    return DisassociateSta((unsigned char *)mac.c_str(), len);
}

WifiErrorNo WifiIdlClient::GetFrequenciesByBand(int32_t band, std::vector<int> &frequencies)
{
    CHECK_CLIENT_NOT_NULL;

    int values[WIFI_IDL_GET_MAX_BANDS] = {0};
    int size = WIFI_IDL_GET_MAX_BANDS;
    if (GetValidFrequenciesForBand(band, values, &size) != 0) {
        return WIFI_IDL_OPT_FAILED;
    }

    for (int i = 0; i < size; i++) {
        frequencies.push_back(values[i]);
    }

    return WIFI_IDL_OPT_OK;
}

WifiErrorNo WifiIdlClient::RegisterApEvent(IWifiApMonitorEventCallback callback) const
{
    CHECK_CLIENT_NOT_NULL;
    IWifiApEventCallback cEventCallback;
    if (memset_s(&cEventCallback, sizeof(cEventCallback), 0, sizeof(cEventCallback)) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    if (callback.onStaJoinOrLeave != nullptr) {
        cEventCallback.onStaJoinOrLeave = OnApStaJoinOrLeave;
        cEventCallback.onApEnableOrDisable = OnApEnableOrDisable;
    }

    return RegisterAsscociatedEvent(cEventCallback);
}

WifiErrorNo WifiIdlClient::SetWifiCountryCode(const std::string &code)
{
    CHECK_CLIENT_NOT_NULL;
    if (code.length() != WIFI_IDL_COUNTRY_CODE_LENGTH) {
        return WIFI_IDL_OPT_INVALID_PARAM;
    }
    return SetCountryCode(code.c_str());
}

WifiErrorNo WifiIdlClient::ReqDisconnectStaByMac(const std::string &mac)
{
    CHECK_CLIENT_NOT_NULL;
    if (CheckMacIsValid(mac) != 0) {
        return WIFI_IDL_OPT_INPUT_MAC_INVALID;
    }
    return DisassociateSta((unsigned char *)mac.c_str(), strlen(mac.c_str()));
}

WifiErrorNo WifiIdlClient::GetWifiChipObject(int id, IWifiChip &chip)
{
    CHECK_CLIENT_NOT_NULL;
    LOGD("Get wifi chip object accord %{public}d, %{public}d", id, chip.i);
    return WIFI_IDL_OPT_OK;
}

WifiErrorNo WifiIdlClient::GetChipIds(std::vector<int> &ids)
{
    CHECK_CLIENT_NOT_NULL;
    LOGD("start GetChipIds %{public}zu", ids.size()); /* fixed compile error, -Werror,-Wunused-parameter */
    return WIFI_IDL_OPT_OK;
}

WifiErrorNo WifiIdlClient::GetUsedChipId(int &id)
{
    CHECK_CLIENT_NOT_NULL;
    id = 0; /* fixed compile error, -Werror,-Wunused-parameter */
    return WIFI_IDL_OPT_OK;
}

WifiErrorNo WifiIdlClient::GetChipCapabilities(int &capabilities)
{
    CHECK_CLIENT_NOT_NULL;
    capabilities = 0; /* fixed compile error, -Werror,-Wunused-parameter */
    return WIFI_IDL_OPT_OK;
}

WifiErrorNo WifiIdlClient::GetSupportedModes(std::vector<int> &modes)
{
    CHECK_CLIENT_NOT_NULL;
    int size = WIFI_IDL_INTERFACE_SUPPORT_COMBINATIONS;
    int supportModes[WIFI_IDL_INTERFACE_SUPPORT_COMBINATIONS] = {0};
    WifiErrorNo err = GetSupportedComboModes(supportModes, &size);
    if (err == WIFI_IDL_OPT_OK) {
        for (int i = 0; i < size; ++i) {
            modes.push_back(supportModes[i]);
        }
    }
    return err;
}

WifiErrorNo WifiIdlClient::ConfigRunModes(int mode)
{
    CHECK_CLIENT_NOT_NULL;
    LOGD("start ConfigRunModes mode %{public}d", mode);
    return WIFI_IDL_OPT_OK;
}

WifiErrorNo WifiIdlClient::GetCurrentMode(int &mode)
{
    CHECK_CLIENT_NOT_NULL;
    mode = 0; /* fixed compile error, -Werror,-Wunused-parameter */
    return WIFI_IDL_OPT_OK;
}

WifiErrorNo WifiIdlClient::RegisterChipEventCallback(WifiChipEventCallback &callback)
{
    CHECK_CLIENT_NOT_NULL;
    IWifiChipEventCallback cEventCallback;
    if (memset_s(&cEventCallback, sizeof(cEventCallback), 0, sizeof(cEventCallback)) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    cEventCallback.onIfaceAdded = callback.onIfaceAdded;
    cEventCallback.onIfaceRemoved = callback.onIfaceRemoved;
    return RegisterEventCallback(cEventCallback);
}

WifiErrorNo WifiIdlClient::RequestFirmwareDebugInfo(std::string &debugInfo)
{
    CHECK_CLIENT_NOT_NULL;
    debugInfo.clear(); /* fixed compile error, -Werror,-Wunused-parameter */
    return WIFI_IDL_OPT_OK;
}

WifiErrorNo WifiIdlClient::SetWifiPowerMode(int mode)
{
    CHECK_CLIENT_NOT_NULL;
    LOGD("start SetWifiPowerMode mode %{public}d", mode);
    return WIFI_IDL_OPT_OK;
}

WifiErrorNo WifiIdlClient::ReqStartSupplicant(void)
{
    CHECK_CLIENT_NOT_NULL;
    return StartSupplicant();
}

WifiErrorNo WifiIdlClient::ReqStopSupplicant(void)
{
    CHECK_CLIENT_NOT_NULL;
    return StopSupplicant();
}

WifiErrorNo WifiIdlClient::ReqConnectSupplicant(void)
{
    CHECK_CLIENT_NOT_NULL;
    return ConnectSupplicant();
}

WifiErrorNo WifiIdlClient::ReqDisconnectSupplicant(void)
{
    CHECK_CLIENT_NOT_NULL;
    return DisconnectSupplicant();
}

WifiErrorNo WifiIdlClient::ReqRequestToSupplicant(const std::string &request)
{
    CHECK_CLIENT_NOT_NULL;
    unsigned char *p = (unsigned char *)request.c_str();
    return RequestToSupplicant(p, request.length());
}

WifiErrorNo WifiIdlClient::ReqRegisterSupplicantEventCallback(SupplicantEventCallback &callback)
{
    CHECK_CLIENT_NOT_NULL;
    ISupplicantEventCallback cEventCallback;
    if (memset_s(&cEventCallback, sizeof(cEventCallback), 0, sizeof(cEventCallback)) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    if (callback.onScanNotify != nullptr) {
        cEventCallback.onScanNotify = OnScanNotify;
    }
    return RegisterSupplicantEventCallback(cEventCallback);
}

WifiErrorNo WifiIdlClient::ReqUnRegisterSupplicantEventCallback(void)
{
    CHECK_CLIENT_NOT_NULL;
    ISupplicantEventCallback cEventCallback;
    if (memset_s(&cEventCallback, sizeof(cEventCallback), 0, sizeof(cEventCallback)) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    return RegisterSupplicantEventCallback(cEventCallback);
}

WifiErrorNo WifiIdlClient::ReqSetPowerSave(bool enable)
{
    CHECK_CLIENT_NOT_NULL;
    int mode = 0;
    if (enable) {
        mode = 1;
    }

    return SetPowerSave(mode);
}

WifiErrorNo WifiIdlClient::ReqWpaSetCountryCode(const std::string &countryCode)
{
    CHECK_CLIENT_NOT_NULL;
    if (countryCode.length() != WIFI_IDL_COUNTRY_CODE_LENGTH) {
        return WIFI_IDL_OPT_INVALID_PARAM;
    }
    return WpaSetCountryCode(countryCode.c_str());
}

WifiErrorNo WifiIdlClient::ReqWpaGetCountryCode(std::string &countryCode)
{
    CHECK_CLIENT_NOT_NULL;
    const int idlCountryCodeLen = 32;
    char code[idlCountryCodeLen] = {0};
    WifiErrorNo ret = WpaGetCountryCode(code, idlCountryCodeLen);
    if (ret == WIFI_IDL_OPT_OK) {
        countryCode = code;
    }
    return ret;
}

WifiErrorNo WifiIdlClient::ReqWpaAutoConnect(int enable)
{
    CHECK_CLIENT_NOT_NULL;
    return WpaAutoConnect(enable);
}

WifiErrorNo WifiIdlClient::ReqWpaBlocklistClear()
{
    CHECK_CLIENT_NOT_NULL;
    return WpaBlocklistClear();
}

WifiErrorNo WifiIdlClient::ReqP2pStart(void) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pStart();
}

WifiErrorNo WifiIdlClient::ReqP2pStop(void) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pStop();
}

WifiErrorNo WifiIdlClient::ReqP2pSetDeviceName(const std::string &name) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pSetDeviceName(name.c_str());
}

WifiErrorNo WifiIdlClient::ReqP2pSetSsidPostfixName(const std::string &postfixName) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pSetSsidPostfixName(postfixName.c_str());
}

WifiErrorNo WifiIdlClient::ReqP2pSetWpsDeviceType(const std::string &type) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pSetWpsDeviceType(type.c_str());
}

WifiErrorNo WifiIdlClient::ReqP2pSetWpsSecondaryDeviceType(const std::string &type) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pSetWpsSecondaryDeviceType(type.c_str());
}

WifiErrorNo WifiIdlClient::ReqP2pSetWpsConfigMethods(const std::string &config) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pSetWpsConfigMethods(config.c_str());
}

WifiErrorNo WifiIdlClient::ReqP2pGetDeviceAddress(std::string &deviceAddress) const
{
    CHECK_CLIENT_NOT_NULL;
    char address[WIFI_IDL_P2P_DEV_ADDRESS_LEN] = {0};
    WifiErrorNo ret = P2pGetDeviceAddress(address, WIFI_IDL_P2P_DEV_ADDRESS_LEN);
    if (ret == WIFI_IDL_OPT_OK) {
        deviceAddress = address;
    }
    return ret;
}

WifiErrorNo WifiIdlClient::ReqP2pFlush() const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pFlush();
}

WifiErrorNo WifiIdlClient::ReqP2pFlushService() const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pFlushService();
}

WifiErrorNo WifiIdlClient::ReqP2pSaveConfig() const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pSaveConfig();
}

WifiErrorNo WifiIdlClient::ReqP2pRegisterCallback(const P2pHalCallback &callbacks) const
{
    CHECK_CLIENT_NOT_NULL;

    IWifiEventP2pCallback cEventCallback;
    if (memset_s(&cEventCallback, sizeof(cEventCallback), 0, sizeof(cEventCallback)) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    if (callbacks.onConnectSupplicant != nullptr) {
        cEventCallback.onP2pSupplicantConnect = OnP2pConnectSupplicant;
        cEventCallback.onDeviceFound = OnP2pDeviceFound;
        cEventCallback.onDeviceLost = OnP2pDeviceLost;
        cEventCallback.onGoNegotiationRequest = OnP2pGoNegotiationRequest;
        cEventCallback.onGoNegotiationSuccess = OnP2pGoNegotiationSuccess;
        cEventCallback.onGoNegotiationFailure = OnP2pGoNegotiationFailure;
        cEventCallback.onInvitationReceived = OnP2pInvitationReceived;
        cEventCallback.onInvitationResult = OnP2pInvitationResult;
        cEventCallback.onGroupFormationSuccess = OnP2pGroupFormationSuccess;
        cEventCallback.onGroupFormationFailure = OnP2pGroupFormationFailure;
        cEventCallback.onGroupStarted = OnP2pGroupStarted;
        cEventCallback.onGroupRemoved = OnP2pGroupRemoved;
        cEventCallback.onProvisionDiscoveryPbcRequest = OnP2pProvisionDiscoveryPbcRequest;
        cEventCallback.onProvisionDiscoveryPbcResponse = OnP2pProvisionDiscoveryPbcResponse;
        cEventCallback.onProvisionDiscoveryEnterPin = OnP2pProvisionDiscoveryEnterPin;
        cEventCallback.onProvisionDiscoveryShowPin = OnP2pProvisionDiscoveryShowPin;
        cEventCallback.onProvisionDiscoveryFailure = OnP2pProvisionDiscoveryFailure;
        cEventCallback.onFindStopped = OnP2pFindStopped;
        cEventCallback.onServiceDiscoveryResponse = OnP2pServiceDiscoveryResponse;
        cEventCallback.onStaDeauthorized = OnP2pStaDeauthorized;
        cEventCallback.onStaAuthorized = OnP2pStaAuthorized;
        cEventCallback.connectSupplicantFailed = OnP2pConnectSupplicantFailed;
        cEventCallback.onP2pServDiscReq = OnP2pServDiscReq;
    }

    return RegisterP2pEventCallback(cEventCallback);
}

WifiErrorNo WifiIdlClient::ReqP2pSetupWpsPbc(const std::string &groupInterface, const std::string &bssid) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pSetupWpsPbc(groupInterface.c_str(), bssid.c_str());
}

WifiErrorNo WifiIdlClient::ReqP2pSetupWpsPin(
    const std::string &groupInterface, const std::string &address, const std::string &pin, std::string &result) const
{
    CHECK_CLIENT_NOT_NULL;
    if (!pin.empty() && pin.size() != WIFI_IDL_PIN_CODE_LENGTH) {
        return WIFI_IDL_OPT_INVALID_PARAM;
    }
    char szPinCode[WIFI_IDL_PIN_CODE_LENGTH + 1] = {0};
    WifiErrorNo ret =
        P2pSetupWpsPin(groupInterface.c_str(), address.c_str(), pin.c_str(), szPinCode, sizeof(szPinCode));
    if (ret == WIFI_IDL_OPT_OK) {
        result = szPinCode;
    }
    return ret;
}

WifiErrorNo WifiIdlClient::ReqP2pRemoveNetwork(int networkId) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pRemoveNetwork(networkId);
}

WifiErrorNo WifiIdlClient::ReqP2pListNetworks(std::map<int, WifiP2pGroupInfo> &mapGroups) const
{
    CHECK_CLIENT_NOT_NULL;
    P2pNetworkList infoList = {0};
    WifiErrorNo ret = P2pListNetworks(&infoList);
    if (ret != WIFI_IDL_OPT_OK) {
        return ret;
    }
    if (infoList.infos == nullptr) {
        return ret;
    }
    for (int i = 0; i < infoList.infoNum; ++i) {
        WifiP2pGroupInfo groupInfo;
        groupInfo.SetNetworkId(infoList.infos[i].id);
        groupInfo.SetGroupName(infoList.infos[i].ssid);
        WifiP2pDevice device;
        device.SetDeviceAddress(infoList.infos[i].bssid);
        groupInfo.SetOwner(device);
        if (strstr(infoList.infos[i].flags, "P2P-PERSISTENT") != nullptr) {
            groupInfo.SetIsPersistent(true);
        }
        mapGroups.insert(std::pair<int, WifiP2pGroupInfo>(infoList.infos[i].id, groupInfo));
    }
    free(infoList.infos);
    infoList.infos = nullptr;
    return ret;
}

WifiErrorNo WifiIdlClient::ReqP2pSetGroupMaxIdle(const std::string &groupInterface, size_t time) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pSetGroupMaxIdle(groupInterface.c_str(), time);
}

WifiErrorNo WifiIdlClient::ReqP2pSetPowerSave(const std::string &groupInterface, bool enable) const
{
    CHECK_CLIENT_NOT_NULL;
    int flag = enable;
    return P2pSetPowerSave(groupInterface.c_str(), flag);
}

WifiErrorNo WifiIdlClient::ReqP2pSetWfdEnable(bool enable) const
{
    CHECK_CLIENT_NOT_NULL;
    int flag = enable;
    return P2pSetWfdEnable(flag);
}

WifiErrorNo WifiIdlClient::ReqP2pSetWfdDeviceConfig(const std::string &config) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pSetWfdDeviceConfig(config.c_str());
}

WifiErrorNo WifiIdlClient::ReqP2pStartFind(size_t timeout) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pStartFind(timeout);
}

WifiErrorNo WifiIdlClient::ReqP2pStopFind() const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pStopFind();
}

WifiErrorNo WifiIdlClient::ReqP2pSetExtListen(bool enable, size_t period, size_t interval) const
{
    CHECK_CLIENT_NOT_NULL;
    if (enable) {
        if (period < WIFI_IDL_P2P_LISTEN_MIN_TIME || period > WIFI_IDL_P2P_LISTEN_MAX_TIME ||
            interval < WIFI_IDL_P2P_LISTEN_MIN_TIME || interval > WIFI_IDL_P2P_LISTEN_MAX_TIME || period > interval) {
            return WIFI_IDL_OPT_INVALID_PARAM;
        }
    }
    int flag = enable;
    return P2pSetExtListen(flag, period, interval);
}

WifiErrorNo WifiIdlClient::ReqP2pSetListenChannel(size_t channel, unsigned char regClass) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pSetListenChannel(channel, regClass);
}

WifiErrorNo WifiIdlClient::ReqP2pConnect(const WifiP2pConfigInternal &config, bool isJoinExistingGroup,
    std::string &pin) const
{
    CHECK_CLIENT_NOT_NULL;
    P2pConnectInfo info = {0};
    info.mode = isJoinExistingGroup;
    info.persistent = config.GetNetId();
    if (isJoinExistingGroup) {
        info.goIntent = 0;
    } else {
        info.goIntent = config.GetGroupOwnerIntent();
    }
    if (info.goIntent < WIFI_IDL_P2P_MIN_GO_INTENT || info.goIntent > WIFI_IDL_P2P_MAX_GO_INTENT) {
        info.goIntent = WIFI_IDL_P2P_DEFAULT_GO_INTENT;
    }
    std::string address = config.GetDeviceAddress();
    if (address.size() < WIFI_IDL_BSSID_LENGTH) {
        LOGI("ReqP2pConnect Device Address is too short");
        return WIFI_IDL_OPT_INVALID_PARAM;
    }
    WpsMethod mode = config.GetWpsInfo().GetWpsMethod();
    if (mode == WpsMethod::WPS_METHOD_LABEL) {
        mode = WpsMethod::WPS_METHOD_KEYPAD;
    }
    info.provdisc = (int)mode;
    std::string pinCode = config.GetWpsInfo().GetPin();
    if (mode == WpsMethod::WPS_METHOD_PBC && !pinCode.empty()) {
        LOGI("ReqP2pConnect Expected empty pin for PBC.");
        return WIFI_IDL_OPT_INVALID_PARAM;
    }
    if (strncpy_s(info.peerDevAddr, sizeof(info.peerDevAddr), address.c_str(), address.length()) != EOK ||
        strncpy_s(info.pin, sizeof(info.pin), pinCode.c_str(), pinCode.length()) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    WifiErrorNo ret = P2pConnect(&info);
    if (ret == WIFI_IDL_OPT_OK) {
        pin = info.pin;
    }
    return ret;
}

WifiErrorNo WifiIdlClient::ReqP2pCancelConnect() const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pCancelConnect();
}

WifiErrorNo WifiIdlClient::ReqP2pProvisionDiscovery(const WifiP2pConfigInternal &config) const
{
    CHECK_CLIENT_NOT_NULL;
    WpsMethod mode = config.GetWpsInfo().GetWpsMethod();
    if (mode == WpsMethod::WPS_METHOD_LABEL) {
        mode = WpsMethod::WPS_METHOD_DISPLAY;
    } else if (mode == WpsMethod::WPS_METHOD_DISPLAY) {
        mode = WpsMethod::WPS_METHOD_KEYPAD;
    } else if (mode == WpsMethod::WPS_METHOD_KEYPAD) {
        mode = WpsMethod::WPS_METHOD_DISPLAY;
    } else if (mode != WpsMethod::WPS_METHOD_PBC) {
        return WIFI_IDL_OPT_FAILED;
    }
    return P2pProvisionDiscovery(config.GetDeviceAddress().c_str(), static_cast<int>(mode));
}

WifiErrorNo WifiIdlClient::ReqP2pAddGroup(bool isPersistent, int networkId, int freq) const
{
    CHECK_CLIENT_NOT_NULL;
    int flag = isPersistent;
    return P2pAddGroup(flag, networkId, freq);
}

WifiErrorNo WifiIdlClient::ReqP2pRemoveGroup(const std::string &groupInterface) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pRemoveGroup(groupInterface.c_str());
}

WifiErrorNo WifiIdlClient::ReqP2pInvite(const WifiP2pGroupInfo &group, const std::string &deviceAddr) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pInvite(group.IsPersistent(),
        deviceAddr.c_str(),
        group.GetOwner().GetDeviceAddress().c_str(),
        group.GetInterface().c_str());
}

WifiErrorNo WifiIdlClient::ReqP2pReinvoke(int networkId, const std::string &deviceAddr) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pReinvoke(networkId, deviceAddr.c_str());
}

WifiErrorNo WifiIdlClient::ReqP2pGetGroupCapability(const std::string &deviceAddress, uint32_t &cap) const
{
    CHECK_CLIENT_NOT_NULL;
    int capacity = 0;
    WifiErrorNo ret = P2pGetGroupCapability(deviceAddress.c_str(), &capacity);
    if (ret == WIFI_IDL_OPT_OK) {
        cap = capacity;
    }
    return ret;
}

WifiErrorNo WifiIdlClient::ReqP2pAddService(const WifiP2pServiceInfo &info) const
{
    CHECK_CLIENT_NOT_NULL;
    WifiErrorNo ret = WIFI_IDL_OPT_OK;
    P2pServiceInfo servInfo = {0};
    std::vector<std::string> queryList = info.GetQueryList();
    for (auto iter = queryList.begin(); iter != queryList.end(); iter++) {
        std::vector<std::string> vec;
        SplitString(*iter, " ", vec);
        if (vec.size() < WIFI_IDL_P2P_SERVICE_TYPE_MIN_SIZE) {
            return WIFI_IDL_OPT_FAILED;
        }
        if (memset_s(&servInfo, sizeof(servInfo), 0, sizeof(servInfo)) != EOK) {
            return WIFI_IDL_OPT_FAILED;
        }
        const std::string &tmp = vec[WIFI_IDL_P2P_SERVICE_TYPE_2_POS];
        if (vec[0] == "upnp") {
            servInfo.mode = 0;
            servInfo.version = atoi(vec[1].c_str());
            if (strncpy_s(servInfo.name, sizeof(servInfo.name), tmp.c_str(), tmp.length()) != EOK) {
                return WIFI_IDL_OPT_FAILED;
            }
            ret = P2pAddService(&servInfo);
        } else if (vec[0] == "bonjour") {
            servInfo.mode = 1;
            if (strncpy_s(servInfo.query, sizeof(servInfo.query), vec[1].c_str(), vec[1].length()) != EOK ||
                strncpy_s(servInfo.resp, sizeof(servInfo.resp), tmp.c_str(), tmp.length()) != EOK) {
                return WIFI_IDL_OPT_FAILED;
            }
            ret = P2pAddService(&servInfo);
        } else {
            ret = WIFI_IDL_OPT_FAILED;
        }
        if (ret != WIFI_IDL_OPT_OK) {
            break;
        }
    }
    return ret;
}

WifiErrorNo WifiIdlClient::ReqP2pRemoveService(const WifiP2pServiceInfo &info) const
{
    CHECK_CLIENT_NOT_NULL;
    WifiErrorNo ret = WIFI_IDL_OPT_OK;
    P2pServiceInfo servInfo = {0};
    std::vector<std::string> queryList = info.GetQueryList();
    for (auto iter = queryList.begin(); iter != queryList.end(); iter++) {
        std::vector<std::string> vec;
        SplitString(*iter, " ", vec);
        if (vec.size() < WIFI_IDL_P2P_SERVICE_TYPE_MIN_SIZE) {
            return WIFI_IDL_OPT_FAILED;
        }
        if (memset_s(&servInfo, sizeof(servInfo), 0, sizeof(servInfo)) != EOK) {
            return WIFI_IDL_OPT_FAILED;
        }
        const std::string &tmp = vec[WIFI_IDL_P2P_SERVICE_TYPE_2_POS];
        if (vec[0] == "upnp") {
            servInfo.mode = 0;
            servInfo.version = atoi(vec[1].c_str());
            if (strncpy_s(servInfo.name, sizeof(servInfo.name), tmp.c_str(), tmp.length()) != EOK) {
                return WIFI_IDL_OPT_FAILED;
            }
            ret = P2pRemoveService(&servInfo);
        } else if (vec[0] == "bonjour") {
            servInfo.mode = 1;
            if (strncpy_s(servInfo.query, sizeof(servInfo.query), vec[1].c_str(), vec[1].length()) != EOK) {
                return WIFI_IDL_OPT_FAILED;
            }
            ret = P2pRemoveService(&servInfo);
        } else {
            ret = WIFI_IDL_OPT_FAILED;
        }
        if (ret != WIFI_IDL_OPT_OK) {
            break;
        }
    }
    return ret;
}

WifiErrorNo WifiIdlClient::ReqP2pReqServiceDiscovery(
    const std::string &deviceAddress, const std::vector<unsigned char> &tlvs, std::string &reqID) const
{
    CHECK_CLIENT_NOT_NULL;
    if (deviceAddress.size() != WIFI_IDL_BSSID_LENGTH || tlvs.empty()) {
        return WIFI_IDL_OPT_INVALID_PARAM;
    }
    unsigned size = (tlvs.size() << 1) + 1;
    char *pTlvs = (char *)calloc(size, sizeof(char));
    if (pTlvs == nullptr || Val2HexChar(tlvs, pTlvs, size) < 0) {
        free(pTlvs);
        pTlvs = nullptr;
        return WIFI_IDL_OPT_FAILED;
    }
    char retBuf[WIFI_IDL_P2P_TMP_BUFFER_SIZE_128] = {0};
    WifiErrorNo ret = P2pReqServiceDiscovery(deviceAddress.c_str(), pTlvs, retBuf, sizeof(retBuf));
    if (ret == WIFI_IDL_OPT_OK) {
        reqID = retBuf;
    }
    free(pTlvs);
    pTlvs = nullptr;
    return ret;
}

WifiErrorNo WifiIdlClient::ReqP2pCancelServiceDiscovery(const std::string &id) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pCancelServiceDiscovery(id.c_str());
}

WifiErrorNo WifiIdlClient::ReqP2pSetRandomMac(bool enable) const
{
    CHECK_CLIENT_NOT_NULL;
    int flag = enable;
    return P2pSetRandomMac(flag);
}

WifiErrorNo WifiIdlClient::ReqP2pSetMiracastType(int type) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pSetMiracastType(type);
}

WifiErrorNo WifiIdlClient::ReqSetPersistentReconnect(int mode) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pSetPersistentReconnect(mode);
}

WifiErrorNo WifiIdlClient::ReqRespServiceDiscovery(
    const WifiP2pDevice &device, int frequency, int dialogToken, const std::vector<unsigned char> &tlvs) const
{
    CHECK_CLIENT_NOT_NULL;
    if (tlvs.empty()) {
        return WIFI_IDL_OPT_INVALID_PARAM;
    }
    unsigned size = (tlvs.size() << 1) + 1;
    char *pTlvs = (char *)calloc(size, sizeof(char));
    if (pTlvs == nullptr || Val2HexChar(tlvs, pTlvs, size) < 0) {
        if (pTlvs != nullptr) {
            free(pTlvs);
            pTlvs = nullptr;
        }
        return WIFI_IDL_OPT_FAILED;
    }
    WifiErrorNo ret = P2pRespServerDiscovery(device.GetDeviceAddress().c_str(), frequency, dialogToken, pTlvs);
    free(pTlvs);
    pTlvs = nullptr;
    return ret;
}

WifiErrorNo WifiIdlClient::ReqSetServiceDiscoveryExternal(bool isExternalProcess) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pSetServDiscExternal(isExternalProcess);
}

WifiErrorNo WifiIdlClient::ReqGetP2pPeer(const std::string &deviceAddress, WifiP2pDevice &device) const
{
    CHECK_CLIENT_NOT_NULL;
    P2pDeviceInfo peerInfo;
    if (memset_s(&peerInfo, sizeof(peerInfo), 0, sizeof(peerInfo)) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    WifiErrorNo ret = P2pGetPeer(deviceAddress.c_str(), &peerInfo);
    if (ret == WIFI_IDL_OPT_OK) {
        device.SetDeviceAddress(peerInfo.p2pDeviceAddress);
        device.SetDeviceName(peerInfo.deviceName);
        device.SetPrimaryDeviceType(peerInfo.primaryDeviceType);
        device.SetWpsConfigMethod(peerInfo.configMethods);
        device.SetDeviceCapabilitys(peerInfo.deviceCapabilities);
        device.SetGroupCapabilitys(peerInfo.groupCapabilities);
    }
    return ret;
}

WifiErrorNo WifiIdlClient::ReqP2pGetSupportFrequencies(int band, std::vector<int> &frequencies) const
{
    CHECK_CLIENT_NOT_NULL;
    int values[WIFI_IDL_GET_MAX_BANDS] = {0};
    int size = WIFI_IDL_GET_MAX_BANDS;

    if (P2pGetFrequencies(band, values, &size) != 0) {
        return WIFI_IDL_OPT_FAILED;
    }

    for (int i = 0; i < size; i++) {
        frequencies.push_back(values[i]);
    }

    return WIFI_IDL_OPT_OK;
}

int WifiIdlClient::PushP2pGroupConfigString(
    P2pGroupConfig *pConfig, P2pGroupConfigType type, const std::string &str) const
{
    if (str.length() > 0) {
        pConfig->cfgParam = type;
        if (strncpy_s(pConfig->cfgValue, sizeof(pConfig->cfgValue), str.c_str(), str.length()) != EOK) {
            return 0;
        }
        return 1;
    } else {
        return 0;
    }
}

int WifiIdlClient::PushP2pGroupConfigInt(P2pGroupConfig *pConfig, P2pGroupConfigType type, int i) const
{
    pConfig->cfgParam = type;
    if (snprintf_s(pConfig->cfgValue, sizeof(pConfig->cfgValue), sizeof(pConfig->cfgValue) - 1, "%d", i) < 0) {
        return 0;
    }
    return 1;
}

WifiErrorNo WifiIdlClient::ReqP2pSetGroupConfig(int networkId, const IdlP2pGroupConfig &config) const
{
    CHECK_CLIENT_NOT_NULL;
    P2pGroupConfig conf[GROUP_CONFIG_END_POS];
    if (memset_s(conf, sizeof(conf), 0, sizeof(conf)) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int num = 0;
    num += PushP2pGroupConfigString(conf + num, GROUP_CONFIG_SSID, config.ssid);
    num += PushP2pGroupConfigString(conf + num, GROUP_CONFIG_BSSID, config.bssid);
    // If the PSK length is less than 8 or greater than 63, Do not set this psk field.
    if (config.psk.length() >= WIFI_IDL_PSK_MIN_LENGTH && config.psk.length() < WIFI_IDL_PSK_MAX_LENGTH) {
        std::string tmp = "\"" + config.psk + "\"";
        num += PushP2pGroupConfigString(conf + num, GROUP_CONFIG_PSK, tmp);
    } else if (config.psk.length() == WIFI_IDL_PSK_MAX_LENGTH) {
        num += PushP2pGroupConfigString(conf + num, GROUP_CONFIG_PSK, config.psk);
    }
    num += PushP2pGroupConfigString(conf + num, GROUP_CONFIG_PROTO, config.proto);
    num += PushP2pGroupConfigString(conf + num, GROUP_CONFIG_KEY_MGMT, config.keyMgmt);
    num += PushP2pGroupConfigString(conf + num, GROUP_CONFIG_PAIRWISE, config.pairwise);
    num += PushP2pGroupConfigString(conf + num, GROUP_CONFIG_AUTH_ALG, config.authAlg);

    num += PushP2pGroupConfigInt(conf + num, GROUP_CONFIG_MODE, config.mode);
    num += PushP2pGroupConfigInt(conf + num, GROUP_CONFIG_DISABLED, config.disabled);
    if (num == 0) {
        return WIFI_IDL_OPT_OK;
    }
    return P2pSetGroupConfig(networkId, conf, num);
}

WifiErrorNo WifiIdlClient::ReqP2pGetGroupConfig(int networkId, IdlP2pGroupConfig &config) const
{
    CHECK_CLIENT_NOT_NULL;
    P2pGroupConfig confs[GROUP_CONFIG_END_POS];
    if (memset_s(confs, sizeof(confs), 0, sizeof(confs)) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int size = static_cast<P2pGroupConfigType>(GROUP_CONFIG_SSID);
    for (; size < GROUP_CONFIG_END_POS; size++) {
        confs[size].cfgParam = static_cast<P2pGroupConfigType>(size);
    }
    if (P2pGetGroupConfig(networkId, confs, size) != 0) {
        return WIFI_IDL_OPT_FAILED;
    }
    config.ssid = confs[GROUP_CONFIG_SSID].cfgValue;
    config.bssid = confs[GROUP_CONFIG_BSSID].cfgValue;
    config.psk = confs[GROUP_CONFIG_PSK].cfgValue;
    config.proto = confs[GROUP_CONFIG_PROTO].cfgValue;
    config.keyMgmt = confs[GROUP_CONFIG_KEY_MGMT].cfgValue;
    config.pairwise = confs[GROUP_CONFIG_PAIRWISE].cfgValue;
    config.authAlg = confs[GROUP_CONFIG_AUTH_ALG].cfgValue;
    config.mode = atoi(confs[GROUP_CONFIG_MODE].cfgValue);
    config.disabled = atoi(confs[GROUP_CONFIG_DISABLED].cfgValue);
    return WIFI_IDL_OPT_OK;
}

WifiErrorNo WifiIdlClient::ReqP2pAddNetwork(int &networkId) const
{
    CHECK_CLIENT_NOT_NULL;
    return P2pAddNetwork(&networkId);
}

WifiErrorNo WifiIdlClient::ReqP2pHid2dConnect(const Hid2dConnectConfig &config) const
{
    CHECK_CLIENT_NOT_NULL;
    Hid2dConnectInfo info;
    if (memset_s(&info, sizeof(info), 0, sizeof(info)) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    if (strncpy_s(info.ssid, sizeof(info.ssid), config.GetSsid().c_str(), config.GetSsid().length()) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    if (strncpy_s(info.bssid, sizeof(info.bssid), config.GetBssid().c_str(), config.GetBssid().length()) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    if (strncpy_s(info.passphrase, sizeof(info.passphrase),
        config.GetPreSharedKey().c_str(), config.GetPreSharedKey().length()) != EOK) {
        return WIFI_IDL_OPT_FAILED;
    }
    info.frequency = config.GetFrequency();
    WifiErrorNo ret = Hid2dConnect(&info);
    return ret;
}
}  // namespace Wifi
}  // namespace OHOS