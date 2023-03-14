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
#include "wifi_hal_chip_interface.h"
#include <stdlib.h>
#include <string.h>
#include "securec.h"
#include "wifi_log.h"
#include "wifi_hal_adapter.h"
#undef LOG_TAG
#define LOG_TAG "WifiHalChipInterface"

WifiErrorNo GetWifiChip(uint8_t id, WifiChip *chip)
{
    LOGD("GetWifiChip() id: %{public}u", id);
    if (chip != NULL) {
        chip->chip = 0; /* fixed compile error, -Werror,-Wunused-parameter */
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetWifiChipIds(uint8_t *ids, int32_t *size)
{
    LOGD("GetWifiChipIds()");
    if (ids != NULL && size != NULL) {
        LOGD("input size %{public}d", *size);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetChipId(int32_t *id)
{
    LOGD("GetChipId()");
    if (id != NULL) {
        *id = 0; /* fixed compile error, -Werror,-Wunused-parameter */
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo CreateIface(int32_t type, WifiIface *iface)
{
    LOGD("CreateIface() type: %{public}d", type);
    if (iface == NULL) {
        return WIFI_HAL_FAILED;
    }
    const int bufferSize = 8;
    char name[bufferSize] = {0};
    if (strcpy_s(iface->name, sizeof(iface->name), name) != EOK) {
        return WIFI_HAL_FAILED;
    }

    iface->type = type;
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetIface(const char *ifname, WifiIface *iface)
{
    if (ifname == NULL || iface == NULL) {
        return WIFI_HAL_FAILED;
    }
    LOGD("GetIface() ifname: %{public}s", ifname);

    WifiIface tmpIface;
    tmpIface.index = 0; /* fixed compile error, -Werror,-Wunused-parameter */
    tmpIface.type = 0;
    tmpIface.name[0] = '\0';
    if (strcpy_s(tmpIface.macAddr, sizeof(tmpIface.macAddr), "00:00:00:00:00:00") != EOK) {
        return WIFI_HAL_FAILED;
    }

    iface = &tmpIface;
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetIfaceNames(int32_t type, char *ifaces, int32_t size)
{
    LOGD("GetIfaceNames() type: %{public}d size: %{public}d", type, size);
    if (ifaces != NULL) {
        ifaces[0] = '\0'; /* fixed compile error, -Werror,-Wunused-parameter */
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo RemoveIface(const char *ifname)
{
    if (ifname == NULL) {
        return WIFI_HAL_FAILED;
    }
    LOGD("RemoveIface() ifname:%{public}s", ifname);
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetCapabilities(uint32_t *capabilities)
{
    LOGD("GetCapabilities()");
    if (capabilities != NULL) {
        *capabilities = 0; /* fixed compile error, -Werror,-Wunused-parameter */
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetSupportedComboModes(int32_t *modes, int32_t *size)
{
    LOGD("GetSupportedComboModes()");
    if (modes == NULL || size == NULL) {
        return WIFI_HAL_FAILED;
    }
    return WIFI_HAL_NOT_SUPPORT;
}

WifiErrorNo ConfigComboModes(int32_t mode)
{
    LOGD("ConfigComboModes() mode: %{public}d", mode);
    WifiHalVendorInterface *pInterface = GetWifiHalVendorInterface();
    if (pInterface == NULL) {
        return WIFI_HAL_GET_VENDOR_HAL_FAILED;
    }
    HalVendorError err = pInterface->func.wifiConfigComboModes(mode);
    return ConvertErrorCode(err);
}

WifiErrorNo GetComboModes(int32_t *id)
{
    LOGD("GetComboModes()");
    if (id == NULL) {
        return WIFI_HAL_FAILED;
    }
    WifiHalVendorInterface *pInterface = GetWifiHalVendorInterface();
    if (pInterface == NULL) {
        return WIFI_HAL_GET_VENDOR_HAL_FAILED;
    }
    HalVendorError err = pInterface->func.wifiGetComboModes(id);
    return ConvertErrorCode(err);
}

WifiErrorNo RequestFirmwareDebugDump(unsigned char *bytes, int32_t *size)
{
    LOGD("RequestFirmwareDebugDump()");
    if (bytes == NULL || size == NULL) {
        return WIFI_HAL_FAILED;
    }
    WifiHalVendorInterface *pInterface = GetWifiHalVendorInterface();
    if (pInterface == NULL) {
        return WIFI_HAL_GET_VENDOR_HAL_FAILED;
    }
    HalVendorError err = pInterface->func.wifiRequestFirmwareDebugDump(bytes, size);
    return ConvertErrorCode(err);
}

WifiErrorNo SetPowerMode(uint8_t mode)
{
    LOGD("SetPowerMode() %{public}u", mode);
    return WIFI_HAL_SUCCESS;
}