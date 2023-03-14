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

#include "native_c/wifi_device.h"
#include "native_cpp/wifi_standard/include/wifi_device.h"
#include "native_cpp/wifi_standard/include/wifi_scan.h"
#include "native_c/wifi_scan_info.h"
#include "native_c/wifi_device_config.h"
#include "wifi_logger.h"
#include "wifi_c_utils.h"
#include "wifi_common_util.h"

DEFINE_WIFILOG_LABEL("WifiCDevice");

static std::map<WifiSecurityType, std::string> g_secTypeKeyMgmtMap = {
    {WIFI_SEC_TYPE_OPEN, "NONE"},
    {WIFI_SEC_TYPE_WEP, "WEP"},
    {WIFI_SEC_TYPE_PSK, "WPA-PSK"},
    {WIFI_SEC_TYPE_SAE, "SAE"},
};

std::unique_ptr<OHOS::Wifi::WifiDevice> wifiDevicePtr = OHOS::Wifi::WifiDevice::GetInstance(WIFI_DEVICE_ABILITY_ID);
std::unique_ptr<OHOS::Wifi::WifiScan> wifiScanPtr = OHOS::Wifi::WifiScan::GetInstance(WIFI_SCAN_ABILITY_ID);

WifiErrorCode EnableWifi()
{
    CHECK_PTR_RETURN(wifiDevicePtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiDevicePtr->EnableWifi());
}

WifiErrorCode DisableWifi()
{
    CHECK_PTR_RETURN(wifiDevicePtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiDevicePtr->DisableWifi());
}

int IsWifiActive()
{
    if (wifiDevicePtr == nullptr) {
        return false;
    }

    bool isActive = false;
    OHOS::Wifi::ErrCode ret = wifiDevicePtr->IsWifiActive(isActive);
    return (ret == OHOS::Wifi::WIFI_OPT_SUCCESS) && isActive;
}

WifiErrorCode Scan()
{
    CHECK_PTR_RETURN(wifiScanPtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiScanPtr->Scan());
}

WifiErrorCode GetScanInfoList(WifiScanInfo *result, unsigned int *size)
{
    CHECK_PTR_RETURN(wifiScanPtr, ERROR_WIFI_NOT_AVAILABLE);
    if (result == nullptr || size == nullptr) {
        WIFI_LOGE("Scan info input parameter is nullptr!");
        return ERROR_WIFI_UNKNOWN;
    }

    std::vector<OHOS::Wifi::WifiScanInfo> vecScnIanfos;
    OHOS::Wifi::ErrCode ret = wifiScanPtr->GetScanInfoList(vecScnIanfos);
    *size = (int)vecScnIanfos.size();
    for (auto& each : vecScnIanfos) {
        if (memcpy_s(result->ssid, WIFI_MAX_SSID_LEN, each.ssid.c_str(), each.ssid.size() + 1) != EOK) {
            return ERROR_WIFI_UNKNOWN;
        }
        if (OHOS::Wifi::MacStrToArray(each.bssid, result->bssid) != EOK) {
            WIFI_LOGE("Scan info convert bssid error!");
            return ERROR_WIFI_UNKNOWN;
        }
        result->securityType = static_cast<int>(each.securityType);
        result->rssi = each.rssi;
        result->band = each.band;
        result->frequency = each.frequency;
        ++result;
    }
    return GetCErrorCode(ret);
}

static std::string GetKeyMgmtBySecType(const int securityType)
{
    WifiSecurityType key = WifiSecurityType(securityType);
    std::map<WifiSecurityType, std::string>::const_iterator iter = g_secTypeKeyMgmtMap.find(key);
    return iter == g_secTypeKeyMgmtMap.end() ? "NONE" : iter->second;
}

static int GetSecTypeByKeyMgmt(const std::string& keyMgmt)
{
    for (auto& each : g_secTypeKeyMgmtMap) {
        if (each.second == keyMgmt) {
            return static_cast<int>(each.first);
        }
    }
    return static_cast<int>(WIFI_SEC_TYPE_OPEN);
}

static void GetStaticIpFromC(const IpConfig& ipConfig, OHOS::Wifi::StaticIpAddress& staticIp)
{
    /* Just IPV4 now */
    staticIp.ipAddress.address.addressIpv4 = ipConfig.ipAddress;
    staticIp.gateway.addressIpv4 = ipConfig.gateway;
    if (WIFI_MAX_DNS_NUM > 0) {
        staticIp.dnsServer1.addressIpv4 = ipConfig.dnsServers[0];
    }
    /* Has backup DNS server */
    if (WIFI_MAX_DNS_NUM > 1) {
        staticIp.dnsServer2.addressIpv4 = ipConfig.dnsServers[1];
    }
    /* netmask: automatic calculate netmask, don't support customized set this value currently */
}

static void GetStaticIpFromCpp(const OHOS::Wifi::StaticIpAddress& staticIp, IpConfig& ipConfig)
{
    /* Just IPV4 now */
    ipConfig.ipAddress = staticIp.ipAddress.address.addressIpv4;
    ipConfig.gateway = staticIp.gateway.addressIpv4;
    if (WIFI_MAX_DNS_NUM > 0) {
        ipConfig.dnsServers[0] = staticIp.dnsServer1.addressIpv4;
    }
    /* Has backup DNS server */
    if (WIFI_MAX_DNS_NUM > 1) {
        ipConfig.dnsServers[1] = staticIp.dnsServer2.addressIpv4;
    }
    /* netmask: not support now */
}

static void ConvertDeviceConfigFromC(const WifiDeviceConfig *config, OHOS::Wifi::WifiDeviceConfig& deviceConfig)
{
    deviceConfig.ssid = config->ssid;
    if (OHOS::Wifi::IsMacArrayEmpty(config->bssid)) {
        deviceConfig.bssid = "";
    } else {
        deviceConfig.bssid = OHOS::Wifi::MacArrayToStr(config->bssid);
    }
    deviceConfig.preSharedKey = config->preSharedKey;
    deviceConfig.keyMgmt = GetKeyMgmtBySecType(config->securityType);
    deviceConfig.networkId = config->netId;
    deviceConfig.frequency = config->freq;
    /* wapiPskType is not support, don't verify now */
    if (config->ipType == DHCP) {
        deviceConfig.wifiIpConfig.assignMethod = OHOS::Wifi::AssignIpMethod::DHCP;
    } else if (config->ipType == STATIC_IP) {
        deviceConfig.wifiIpConfig.assignMethod = OHOS::Wifi::AssignIpMethod::STATIC;
        GetStaticIpFromC(config->staticIp, deviceConfig.wifiIpConfig.staticIpAddress);
    } else {
        deviceConfig.wifiIpConfig.assignMethod = OHOS::Wifi::AssignIpMethod::UNASSIGNED;
    }
    deviceConfig.hiddenSSID = config->isHiddenSsid;
}

static OHOS::Wifi::ErrCode ConvertDeviceConfigFromCpp(const OHOS::Wifi::WifiDeviceConfig& deviceConfig,
    WifiDeviceConfig *result)
{
    if (memcpy_s(result->ssid, WIFI_MAX_SSID_LEN, deviceConfig.ssid.c_str(), deviceConfig.ssid.size() + 1) != EOK) {
        return OHOS::Wifi::WIFI_OPT_FAILED;
    }
    if (OHOS::Wifi::MacStrToArray(deviceConfig.bssid, result->bssid) != EOK) {
        WIFI_LOGE("device config convert bssid error!");
        return OHOS::Wifi::WIFI_OPT_FAILED;
    }
    if (memcpy_s(result->preSharedKey, WIFI_MAX_KEY_LEN, deviceConfig.preSharedKey.c_str(), WIFI_MAX_KEY_LEN) != EOK) {
        return OHOS::Wifi::WIFI_OPT_FAILED;
    }
    result->securityType = GetSecTypeByKeyMgmt(deviceConfig.keyMgmt);
    result->netId = deviceConfig.networkId;
    result->freq = deviceConfig.frequency;
    /* wapiPskType is not support now */
    if (deviceConfig.wifiIpConfig.assignMethod == OHOS::Wifi::AssignIpMethod::DHCP) {
        result->ipType = DHCP;
    } else if (deviceConfig.wifiIpConfig.assignMethod == OHOS::Wifi::AssignIpMethod::STATIC) {
        result->ipType = STATIC_IP;
        GetStaticIpFromCpp(deviceConfig.wifiIpConfig.staticIpAddress, result->staticIp);
    } else {
        result->ipType = UNKNOWN;
    }
    result->isHiddenSsid = deviceConfig.hiddenSSID;
    return OHOS::Wifi::WIFI_OPT_SUCCESS;
}

WifiErrorCode AddDeviceConfig(const WifiDeviceConfig *config, int *result)
{
    CHECK_PTR_RETURN(wifiDevicePtr, ERROR_WIFI_NOT_AVAILABLE);
    OHOS::Wifi::WifiDeviceConfig deviceConfig;
    ConvertDeviceConfigFromC(config, deviceConfig);
    int addResult = -1;
    OHOS::Wifi::ErrCode ret = wifiDevicePtr->AddDeviceConfig(deviceConfig, addResult);
    *result = addResult;
    return GetCErrorCode(ret);
}

WifiErrorCode GetDeviceConfigs(WifiDeviceConfig *result, unsigned int *size)
{
    CHECK_PTR_RETURN(wifiDevicePtr, ERROR_WIFI_NOT_AVAILABLE);
    if (result == nullptr) {
        WIFI_LOGE("Get device configs result array is null!");
        return ERROR_WIFI_UNKNOWN;
    }

    std::vector<OHOS::Wifi::WifiDeviceConfig> vecDeviceConfigs;
    OHOS::Wifi::ErrCode ret = wifiDevicePtr->GetDeviceConfigs(vecDeviceConfigs);
    if (ret != OHOS::Wifi::WIFI_OPT_SUCCESS) {
        WIFI_LOGE("Get device configs error!");
        return GetCErrorCode(ret);
    }
    *size = (int)vecDeviceConfigs.size();
    for (auto& each : vecDeviceConfigs) {
        OHOS::Wifi::ErrCode retValue = ConvertDeviceConfigFromCpp(each, result++);
        if (retValue != OHOS::Wifi::WIFI_OPT_SUCCESS) {
            ret = retValue;
            WIFI_LOGE("Convert device configs error!");
        }
    }
    return GetCErrorCode(ret);
}

WifiErrorCode RemoveDevice(int networkId)
{
    CHECK_PTR_RETURN(wifiDevicePtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiDevicePtr->RemoveDevice(networkId));
}

WifiErrorCode DisableDeviceConfig(int networkId)
{
    return GetCErrorCode(OHOS::Wifi::WIFI_OPT_NOT_SUPPORTED);
}

WifiErrorCode EnableDeviceConfig(int networkId)
{
    return GetCErrorCode(OHOS::Wifi::WIFI_OPT_NOT_SUPPORTED);
}

WifiErrorCode ConnectTo(int networkId)
{
    CHECK_PTR_RETURN(wifiDevicePtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiDevicePtr->ConnectToNetwork(networkId));
}

WifiErrorCode ConnectToDevice(const WifiDeviceConfig *config)
{
    CHECK_PTR_RETURN(wifiDevicePtr, ERROR_WIFI_NOT_AVAILABLE);
    OHOS::Wifi::WifiDeviceConfig deviceConfig;
    ConvertDeviceConfigFromC(config, deviceConfig);
    return GetCErrorCode(wifiDevicePtr->ConnectToDevice(deviceConfig));
}

WifiErrorCode Disconnect()
{
    CHECK_PTR_RETURN(wifiDevicePtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiDevicePtr->Disconnect());
}

static OHOS::Wifi::ErrCode GetLinkedInfoFromCpp(const OHOS::Wifi::WifiLinkedInfo& linkedInfo, WifiLinkedInfo *result)
{
    if (memcpy_s(result->ssid, WIFI_MAX_SSID_LEN, linkedInfo.ssid.c_str(), linkedInfo.ssid.size() + 1) != EOK) {
        return OHOS::Wifi::WIFI_OPT_FAILED;
    }
    if (OHOS::Wifi::MacStrToArray(linkedInfo.bssid, result->bssid) != EOK) {
        WIFI_LOGE("linked info convert bssid error!");
        return OHOS::Wifi::WIFI_OPT_FAILED;
    }
    result->rssi = linkedInfo.rssi;
    result->band = linkedInfo.band;
    result->frequency = linkedInfo.frequency;
    result->connState = linkedInfo.connState == OHOS::Wifi::ConnState::CONNECTED ? WIFI_CONNECTED : WIFI_DISCONNECTED;
    /* disconnectedReason not support */
    result->ipAddress = linkedInfo.ipAddress;
    return OHOS::Wifi::WIFI_OPT_SUCCESS;
}

WifiErrorCode GetLinkedInfo(WifiLinkedInfo *result)
{
    CHECK_PTR_RETURN(wifiDevicePtr, ERROR_WIFI_NOT_AVAILABLE);
    OHOS::Wifi::WifiLinkedInfo linkedInfo;
    OHOS::Wifi::ErrCode ret = wifiDevicePtr->GetLinkedInfo(linkedInfo);
    if (ret == OHOS::Wifi::WIFI_OPT_SUCCESS) {
        OHOS::Wifi::ErrCode retValue = GetLinkedInfoFromCpp(linkedInfo, result);
        if (retValue != OHOS::Wifi::WIFI_OPT_SUCCESS) {
            WIFI_LOGE("Get linked info from cpp error!");
            ret = retValue;
        }
    }
    return GetCErrorCode(ret);
}

WifiErrorCode GetDeviceMacAddress(unsigned char *result)
{
    CHECK_PTR_RETURN(wifiDevicePtr, ERROR_WIFI_NOT_AVAILABLE);
    std::string mac;
    OHOS::Wifi::ErrCode ret = wifiDevicePtr->GetDeviceMacAddress(mac);
    if (ret == OHOS::Wifi::WIFI_OPT_SUCCESS) {
        if (OHOS::Wifi::MacStrToArray(mac, result) != EOK) {
            WIFI_LOGE("get mac convert to array error!");
            return ERROR_WIFI_UNKNOWN;
        }
    }
    return GetCErrorCode(ret);
}

WifiErrorCode AdvanceScan(WifiScanParams *params)
{
    return GetCErrorCode(OHOS::Wifi::WIFI_OPT_NOT_SUPPORTED);
}

WifiErrorCode GetIpInfo(IpInfo *info)
{
    return GetCErrorCode(OHOS::Wifi::WIFI_OPT_NOT_SUPPORTED);
}

int GetSignalLevel(int rssi, int band)
{
    CHECK_PTR_RETURN(wifiDevicePtr, ERROR_WIFI_NOT_AVAILABLE);
    int level = -1;
    OHOS::Wifi::ErrCode ret = wifiDevicePtr->GetSignalLevel(rssi, band, level);
    if (ret != OHOS::Wifi::WIFI_OPT_SUCCESS) {
        WIFI_LOGW("Get wifi signal level fail: %{public}d", ret);
    }
    return level;
}

WifiErrorCode SetLowLatencyMode(int enabled)
{
    CHECK_PTR_RETURN(wifiDevicePtr, ERROR_WIFI_NOT_AVAILABLE);
    bool ret = wifiDevicePtr->SetLowLatencyMode(enabled);
    return ret ? WIFI_SUCCESS : ERROR_WIFI_NOT_AVAILABLE;
}
