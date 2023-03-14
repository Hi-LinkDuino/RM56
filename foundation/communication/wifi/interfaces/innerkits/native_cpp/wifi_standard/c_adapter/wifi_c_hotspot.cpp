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

#include "native_c/wifi_hotspot.h"
#include "native_c/wifi_hotspot_config.h"
#include "native_c/wifi_device_config.h"
#include "native_cpp/wifi_standard/include/wifi_hotspot.h"
#include "wifi_logger.h"
#include "wifi_c_utils.h"
#include "ip_tools.h"
#include "wifi_common_util.h"

DEFINE_WIFILOG_LABEL("WifiCHotspot");

std::unique_ptr<OHOS::Wifi::WifiHotspot> hotspotPtr = OHOS::Wifi::WifiHotspot::GetInstance(WIFI_HOTSPOT_ABILITY_ID);

WifiErrorCode EnableHotspot()
{
    CHECK_PTR_RETURN(hotspotPtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(hotspotPtr->EnableHotspot());
}

WifiErrorCode DisableHotspot()
{
    CHECK_PTR_RETURN(hotspotPtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(hotspotPtr->DisableHotspot());
}

int IsHotspotActive(void)
{
    if (hotspotPtr == nullptr) {
        return false;
    }
    return hotspotPtr->IsHotspotActive();
}

/* Others type is not support for AP */
static std::map<WifiSecurityType, OHOS::Wifi::KeyMgmt> g_mapSecTypeToKeyMgmt = {
    {WifiSecurityType::WIFI_SEC_TYPE_OPEN, OHOS::Wifi::KeyMgmt::NONE},
    {WifiSecurityType::WIFI_SEC_TYPE_PSK, OHOS::Wifi::KeyMgmt::WPA_PSK},
};

static OHOS::Wifi::KeyMgmt GetKeyMgmtFromSecurityType(int secType)
{
    WifiSecurityType key = WifiSecurityType(secType);
    std::map<WifiSecurityType, OHOS::Wifi::KeyMgmt>::iterator iter = g_mapSecTypeToKeyMgmt.find(key);
    return iter == g_mapSecTypeToKeyMgmt.end() ? OHOS::Wifi::KeyMgmt::NONE : iter->second;
}

static int GetSecurityTypeFromKeyMgmt(OHOS::Wifi::KeyMgmt keyMgmt)
{
    for (auto& each : g_mapSecTypeToKeyMgmt) {
        if (each.second == keyMgmt) {
            return static_cast<int>(each.first);
        }
    }
    return static_cast<int>(WifiSecurityType::WIFI_SEC_TYPE_OPEN);
}

static bool IsSecurityTypeSupported(int secType)
{
    WifiSecurityType key = WifiSecurityType(secType);
    std::map<WifiSecurityType, OHOS::Wifi::KeyMgmt>::iterator iter = g_mapSecTypeToKeyMgmt.find(key);
    return iter != g_mapSecTypeToKeyMgmt.end();
}

static WifiErrorCode GetHotspotConfigFromC(const HotspotConfig *config, OHOS::Wifi::HotspotConfig& hotspotConfig)
{
    hotspotConfig.SetSsid(config->ssid);
    if (!IsSecurityTypeSupported(config->securityType)) {
        WIFI_LOGE("Ap security is not supported!");
        return ERROR_WIFI_NOT_SUPPORTED;
    }
    hotspotConfig.SetSecurityType(GetKeyMgmtFromSecurityType(config->securityType));
    hotspotConfig.SetBand(OHOS::Wifi::BandType(config->band));
    hotspotConfig.SetChannel(config->channelNum);
    hotspotConfig.SetPreSharedKey(config->preSharedKey);
    return WIFI_SUCCESS;
}

static WifiErrorCode GetHotspotConfigFromCpp(const OHOS::Wifi::HotspotConfig& hotspotConfig, HotspotConfig *result)
{
    if (memcpy_s(result->ssid, WIFI_MAX_SSID_LEN,
        hotspotConfig.GetSsid().c_str(), hotspotConfig.GetSsid().size() + 1) != EOK) {
        return ERROR_WIFI_UNKNOWN;
    }
    result->securityType = GetSecurityTypeFromKeyMgmt(hotspotConfig.GetSecurityType());
    result->band = static_cast<int>(hotspotConfig.GetBand());
    result->channelNum = hotspotConfig.GetChannel();
    if (memcpy_s(result->preSharedKey, WIFI_MAX_KEY_LEN,
        hotspotConfig.GetPreSharedKey().c_str(), hotspotConfig.GetPreSharedKey().size() + 1) != EOK) {
        return ERROR_WIFI_UNKNOWN;
    }
    return WIFI_SUCCESS;
}

WifiErrorCode SetHotspotConfig(const HotspotConfig *config)
{
    CHECK_PTR_RETURN(hotspotPtr, ERROR_WIFI_NOT_AVAILABLE);
    OHOS::Wifi::HotspotConfig hotspotConfig;
    WifiErrorCode ret = GetHotspotConfigFromC(config, hotspotConfig);
    if (ret != WIFI_SUCCESS) {
        return ret;
    }
    return GetCErrorCode(hotspotPtr->SetHotspotConfig(hotspotConfig));
}

WifiErrorCode GetHotspotConfig(HotspotConfig *result)
{
    CHECK_PTR_RETURN(hotspotPtr, ERROR_WIFI_NOT_AVAILABLE);
    OHOS::Wifi::HotspotConfig hotspotConfig;
    OHOS::Wifi::ErrCode ret = hotspotPtr->GetHotspotConfig(hotspotConfig);
    if (ret == OHOS::Wifi::WIFI_OPT_SUCCESS) {
        WifiErrorCode retValue = GetHotspotConfigFromCpp(hotspotConfig, result);
        if (retValue != WIFI_SUCCESS) {
            WIFI_LOGE("Get hotspot config from cpp error!");
            return retValue;
        }
    }
    return GetCErrorCode(ret);
}

static WifiErrorCode GetStaListFromCpp(const std::vector<OHOS::Wifi::StationInfo>& vecStaList, StationInfo *result)
{
    for (auto& each : vecStaList) {
        if (result->name != nullptr) {
            if (memcpy_s(result->name, DEVICE_NAME_LEN, each.deviceName.c_str(), each.deviceName.size() + 1) != EOK) {
                return ERROR_WIFI_UNKNOWN;
            }
        } else {
            WIFI_LOGE("WARN: device name is not pre-allocate memory!");
        }

        if (OHOS::Wifi::MacStrToArray(each.bssid, result->macAddress) != EOK) {
            WIFI_LOGE("Get sta list convert bssid error!");
            return ERROR_WIFI_UNKNOWN;
        }
        result->ipAddress = OHOS::Wifi::IpTools::ConvertIpv4Address(each.ipAddr);
    }
    return WIFI_SUCCESS;
}

WifiErrorCode GetStationList(StationInfo *result, unsigned int *size)
{
    if (result == nullptr) {
        WIFI_LOGE("Station info list receive addr is null!");
        return ERROR_WIFI_UNKNOWN;
    }
    CHECK_PTR_RETURN(hotspotPtr, ERROR_WIFI_NOT_AVAILABLE);

    std::vector<OHOS::Wifi::StationInfo> vecStaList;
    OHOS::Wifi::ErrCode ret = hotspotPtr->GetStationList(vecStaList);
    *size = (int)vecStaList.size();
    if (ret == OHOS::Wifi::WIFI_OPT_SUCCESS) {
        WifiErrorCode retValue = GetStaListFromCpp(vecStaList, result);
        if (retValue != WIFI_SUCCESS) {
            WIFI_LOGE("Get station list from cpp error!");
            return retValue;
        }
    }
    return GetCErrorCode(ret);
}

WifiErrorCode DisassociateSta(unsigned char *mac, int macLen)
{
    return GetCErrorCode(OHOS::Wifi::WIFI_OPT_NOT_SUPPORTED);
}

WifiErrorCode AddTxPowerInfo(int power)
{
    return GetCErrorCode(OHOS::Wifi::WIFI_OPT_NOT_SUPPORTED);
}
