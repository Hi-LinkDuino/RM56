/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_wlan_utils.h"
#include <securec.h>
#include "hdf_log.h"
#include "wifi_module.h"
#include "hdf_wlan_chipdriver_manager.h"

const char * const HDF_WIFI_PLATFORM_DRIVER_NAME = "HDF_WIFI_PLATFORM_DRIVER";

const char *GetPlatformDriverName(void)
{
    return HDF_WIFI_PLATFORM_DRIVER_NAME;
}

struct HdfWifiNetDeviceData *GetPlatformData(const struct NetDevice *netDev)
{
    if (netDev == NULL) {
        return NULL;
    }

    if (netDev->classDriverName != HDF_WIFI_PLATFORM_DRIVER_NAME) {
        return NULL;
    }

    return (struct HdfWifiNetDeviceData *)(netDev->classDriverPriv);
}

struct HdfChipDriver *GetChipDriver(const struct NetDevice *netDev)
{
    struct HdfWifiNetDeviceData *data = GetPlatformData(netDev);
    if (data != NULL) {
        return data->chipDriver;
    }
    return NULL;
}

struct WlanHwCapability *GetHwCapability(struct NetDevice *netDev)
{
    struct HdfChipDriver *chipDriver = GetChipDriver(netDev);
    struct WlanHwCapability *capability = NULL;
    int32_t ret;
    if (chipDriver == NULL) {
        HDF_LOGE("%s:bad net device found!", __func__);
        return NULL;
    }
    if (chipDriver->ops == NULL || chipDriver->ops->GetHwCapability == NULL) {
        HDF_LOGE("%s: chipdriver not implemented", __func__);
        return NULL;
    }
    ret = chipDriver->ops->GetHwCapability(netDev, &capability);
    if (ret != HDF_SUCCESS || capability == NULL) {
        HDF_LOGE("%s:GetHwCapability failed!ifName=%s,ret=%d", __func__, netDev->name, ret);
        return NULL;
    }
    return capability;
}

static uint32_t g_allocatedIfMap = 0;

#define SET_BIT(target, bit) (target) = ((target) | ((0x1) << (bit)))
#define CLEAR_BIT(target, bit) (target) = ((target) & (~((0x1) << (bit))))

int32_t RenewNetDevice(NetDevice **netDev)
{
    char ifName[IFNAMSIZ] = "";
    NetDevice *result = NULL;
    struct HdfWifiNetDeviceData *data = NULL;
    int32_t ret;
    if (netDev == NULL || *netDev == NULL) {
        HDF_LOGE("%s:RenewNetDevice netDev NULL ptr!", __func__);
        return HDF_FAILURE;
    }
    if ((*netDev)->classDriverName != HDF_WIFI_PLATFORM_DRIVER_NAME) {
        HDF_LOGE("%s:bad net device!", __func__);
        return HDF_FAILURE;
    }
    if (strcpy_s(ifName, IFNAMSIZ, (*netDev)->name) != EOK) {
        HDF_LOGE("%s:Copy ifName failed!", __func__);
        return HDF_FAILURE;
    }
    data = GetPlatformData(*netDev);
    if (data == NULL) {
        HDF_LOGE("%s:GetPlatformData failed!", __func__);
        return HDF_FAILURE;
    }

    ret = NetDeviceDeInit(*netDev);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    *netDev = NULL;
#ifdef _PRE_HDF_LINUX
    result = NetDeviceInit(ifName, strlen(ifName), WIFI_LINK, FULL_OS);
#else
    result = NetDeviceInit(ifName, strlen(ifName), WIFI_LINK, LITE_OS);
#endif
    if (result == NULL) {
        HDF_LOGE("%s:alloc NetDevice return NULL!", __func__);
        if (data->device != NULL) {
            CLEAR_BIT(data->device->netIfMap, data->netInterfaceId);
        }
        CLEAR_BIT(g_allocatedIfMap, data->netInterfaceId);
        OsalMemFree(data);
        *netDev = NULL;
        return HDF_FAILURE;
    }
    result->classDriverName = HDF_WIFI_PLATFORM_DRIVER_NAME;
    result->classDriverPriv = data;
    *netDev = result;
    return HDF_SUCCESS;
}

int32_t GetPlatformIfName(uint8_t id, char *ifName, uint32_t ifNameSize)
{
    if (ifName == NULL || ifNameSize == 0) {
        HDF_LOGE("%s:para is null!", __func__);
        return HDF_FAILURE;
    }

    if (snprintf_s(ifName, ifNameSize, ifNameSize - 1, "wlan%d", id) < 0) {
        HDF_LOGE("%s:format ifName failed!netDevice id=%hhu", __func__, id);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

struct NetDevice *AllocPlatformNetDevice(struct HdfWlanDevice *device)
{
    uint8_t i;
    int32_t ret;
    uint8_t id = MAX_IF_COUNT;
    struct NetDevice *result = NULL;
    char ifName[IFNAMSIZ] = {0};
    struct HdfWifiNetDeviceData *data = NULL;
    if (device == NULL) {
        return NULL;
    }
    data = (struct HdfWifiNetDeviceData *)OsalMemCalloc(sizeof(struct HdfWifiNetDeviceData));
    if (data == NULL) {
        HDF_LOGE("%s:AllocPlatformNetDevice data NULL ptr!", __func__);
        return NULL;
    }
    do {
        for (i = 0; i < MAX_IF_COUNT; i++) {
            if (((g_allocatedIfMap >> i) & 0x1) != 0x1) {
                id = i;
                break;
            }
        }
        if (id >= MAX_IF_COUNT) {
            HDF_LOGE("%s: not enough if!map=%u", __func__, g_allocatedIfMap);
            break;
        }

        ret = GetPlatformIfName(id, ifName, IFNAMSIZ);
        if (ret != HDF_SUCCESS) {
            break;
        }
#ifdef _PRE_HDF_LINUX
        result = NetDeviceInit(ifName, strlen(ifName), WIFI_LINK, FULL_OS);
#else
        result = NetDeviceInit(ifName, strlen(ifName), WIFI_LINK, LITE_OS);
#endif
    } while (false);
    if (result == NULL) {
        HDF_LOGE("%s:alloc NetDevice return NULL!", __func__);
        OsalMemFree(data);
        return result;
    }
    result->classDriverName = HDF_WIFI_PLATFORM_DRIVER_NAME;
    result->classDriverPriv = data;
    data->netInterfaceId = id;
    SET_BIT(device->netIfMap, id);
    SET_BIT(g_allocatedIfMap, id);
    return result;
}

int32_t ReleasePlatformNetDevice(struct NetDevice *netDev)
{
    uint8_t id;
    struct HdfWifiNetDeviceData *data = NULL;
    int32_t ret;

    if (netDev == NULL) {
        HDF_LOGE("%s:ReleasePlatformNetDevice netDev NULL ptr!", __func__);
        return HDF_FAILURE;
    }
    if (netDev->classDriverName != HDF_WIFI_PLATFORM_DRIVER_NAME) {
        HDF_LOGE("%s:Wrong net device!", __func__);
        return HDF_FAILURE;
    }

    data = GetPlatformData(netDev);
    if (data == NULL) {
        HDF_LOGE("%s:GetPlatformData failed!", __func__);
        return HDF_FAILURE;
    }
    id = data->netInterfaceId;

    ret = NetDeviceDeInit(netDev);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (data->device != NULL) {
        CLEAR_BIT(data->device->netIfMap, id);
    }
    CLEAR_BIT(g_allocatedIfMap, id);
    OsalMemFree(data);
    return HDF_SUCCESS;
}

char *HdfWlanGetIfNames(const uint8_t chipId, uint8_t *ifNameCount)
{
    struct HdfWlanDevice *wlanDevice = NULL;
    uint32_t netIfMapTemp;
    char *ifNames = NULL;
    uint32_t bufferSize;
    uint8_t i, j;
    int32_t ret;
    if (ifNameCount == NULL) {
        HDF_LOGE("%s: para is NULL", __func__);
        return NULL;
    }

    wlanDevice = HdfWlanGetWlanDevice(chipId);
    if (wlanDevice == NULL) {
        HDF_LOGE("%s:wlanDevice is NULL, not found!", __func__);
        return NULL;
    }
    *ifNameCount = 0;
    netIfMapTemp = wlanDevice->netIfMap;
    for (i = 0; i < MAX_IF_COUNT; i++) {
        if (((netIfMapTemp >> i) & 0x1) != 0) {
            (*ifNameCount)++;
        }
    }
    if (*ifNameCount == 0) {
        // Never alloc 0 size
        bufferSize = sizeof(char);
    } else {
        bufferSize = IFNAMSIZ * (uint32_t)(*ifNameCount);
    }
    ifNames = (char *)OsalMemCalloc(bufferSize);
    if (ifNames == NULL) {
        HDF_LOGE("%s:HdfWlanGetIfNames ifNames NULL ptr!", __func__);
        return NULL;
    }
    if (*ifNameCount == 0) {
        return ifNames;
    }
    for (i = 0, j = 0; i < MAX_IF_COUNT; i++) {
        if (((netIfMapTemp >> i) & 0x1) == 0) {
            continue;
        }
        ret = GetPlatformIfName(i, ifNames + (j * IFNAMSIZ), IFNAMSIZ);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:Get ifName failed!ret=%d", __func__, ret);
            OsalMemFree(ifNames);
            return NULL;
        }
        j++;
    }

    return ifNames;
}
