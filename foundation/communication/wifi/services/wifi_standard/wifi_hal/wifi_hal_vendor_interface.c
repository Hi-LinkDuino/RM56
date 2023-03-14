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

#include "wifi_hal_vendor_interface.h"
#include "wifi_log.h"
#undef LOG_TAG
#define LOG_TAG "WifiHalVenderInterface"

WifiErrorNo ConvertErrorCode(HalVendorError code)
{
    switch (code) {
        case HAL_VENDOR_SUCCESS:
            return WIFI_HAL_SUCCESS;
        case HAL_VENDOR_ERROR_UNKNOWN:
            return WIFI_HAL_VENDOR_UNKNOWN;
        case HAL_VENDOR_ERROR_UNINITIALIZED:
            return WIFI_HAL_VENDOR_UNINITIALIZED;
        case HAL_VENDOR_ERROR_NOT_SUPPORTED:
            return WIFI_HAL_NOT_SUPPORT;
        case HAL_VENDOR_ERROR_NOT_AVAILABLE:
            return WIFI_HAL_VENDOR_NOT_AVAILABLE;
        case HAL_VENDOR_ERROR_INVALID_ARGS:
            return WIFI_HAL_VENDOR_INVALID_ARGS;
        case HAL_VENDOR_ERROR_INVALID_REQUEST_ID:
            return WIFI_HAL_VENDOR_INVALID_REQUEST_ID;
        case HAL_VENDOR_ERROR_TIMED_OUT:
            return WIFI_HAL_VENDOR_TIMED_OUT;
        case HAL_VENDOR_ERROR_TOO_MANY_REQUESTS:
            return WIFI_HAL_VENDOR_TOO_MANY_REQUESTS;
        case HAL_VENDOR_ERROR_OUT_OF_MEMORY:
            return WIFI_HAL_VENDOR_OUT_OF_MEMORY;
        case HAL_VENDOR_ERROR_BUSY:
            return WIFI_HAL_VENDOR_BUSY;
        default:
            return WIFI_HAL_SUCCESS;
    }
}

HalVendorError WifiInitialize(void)
{
    return HAL_VENDOR_ERROR_NOT_SUPPORTED;
}

HalVendorError WifiCleanUp(void)
{
    return HAL_VENDOR_ERROR_NOT_SUPPORTED;
}

HalVendorError WifiGetSupportedFeature(long *feature)
{
    LOGD("WifiGetSupportedFeature");
    if (feature != NULL) {
        *feature = 0; /* fixed compile error, -Werror,-Wunused-parameter */
    }
    return HAL_VENDOR_ERROR_NOT_SUPPORTED;
}

HalVendorError WifiSetScanningMacAddress(const char *szMac, int macSize)
{
    LOGD("WifiSetScanningMacAddress mac %{private}s, size %{public}d", (szMac == NULL) ? "" : szMac, macSize);
    return HAL_VENDOR_ERROR_NOT_SUPPORTED;
}

HalVendorError WifiDeauthLastRoamingBssid(const char *szMac, int macSize)
{
    LOGD("WifiDeauthLastRoamingBssid mac %{private}s, size %{public}d", (szMac == NULL) ? "" : szMac, macSize);
    return HAL_VENDOR_ERROR_NOT_SUPPORTED;
}

HalVendorError WifiSetWifiTxPower(int32_t power)
{
    LOGD("WifiSetWifiTxPower power %{public}d", power);
    return HAL_VENDOR_ERROR_NOT_SUPPORTED;
}

HalVendorError WifiConfigComboModes(int32_t mode)
{
    LOGD("WifiConfigComboModes mode %{public}d", mode);
    return HAL_VENDOR_ERROR_NOT_SUPPORTED;
}

HalVendorError WifiGetComboModes(int32_t *id)
{
    LOGD("WifiGetComboModes");
    if (id != NULL) {
        *id = 0; /* fixed compile error, -Werror,-Wunused-parameter */
    }
    return HAL_VENDOR_ERROR_NOT_SUPPORTED;
}

HalVendorError WifiRequestFirmwareDebugDump(unsigned char *bytes, int32_t *size)
{
    LOGD("WifiRequestFirmwareDebugDump");
    /* fixed compile error, -Werror,-Wunused-parameter */
    if (bytes != NULL && size != NULL) {
        LOGD("WifiRequestFirmwareDebugDump bytes %{private}s, size %{public}d", bytes, *size);
    }
    return HAL_VENDOR_ERROR_NOT_SUPPORTED;
}

HalVendorError WifiSetMiracastMode(int mode)
{
    LOGD("WifiSetMiracastMode mode %{public}d", mode);
    return HAL_VENDOR_ERROR_NOT_SUPPORTED;
}
HalVendorError InitDefaultHalVendorFunc(WifiHalVendorFunc *func)
{
    if (func == NULL) {
        LOGD("InitDefaultHalVendorFunc func is NULL");
        return HAL_VENDOR_ERROR_UNKNOWN;
    }
    func->wifiInitialize = WifiInitialize;
    func->wifiCleanUp = WifiCleanUp;
    func->wifiGetSupportedFeature = WifiGetSupportedFeature;
    func->wifiSetScanningMacAddress = WifiSetScanningMacAddress;
    func->wifiDeauthLastRoamingBssid = WifiDeauthLastRoamingBssid;
    func->wifiSetWifiTxPower = WifiSetWifiTxPower;
    func->wifiConfigComboModes = WifiConfigComboModes;
    func->wifiGetComboModes = WifiGetComboModes;
    func->wifiRequestFirmwareDebugDump = WifiRequestFirmwareDebugDump;
    func->wifiSetMiracastMode = WifiSetMiracastMode;
    return HAL_VENDOR_SUCCESS;
}
