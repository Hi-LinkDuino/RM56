/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_wifi_product.h"
#include "hdf_wlan_chipdriver_manager.h"
#include "hdf_wlan_utils.h"
#include "securec.h"

#define MAX_WLAN_DEVICE 3
/**
 * @brief Defines the Product Data.
 *
 * @since 1.0
 */
struct HdfWifiProductData {
    char state;                     /* *< WLAN module state */
    struct WifiModule module;       /* *< Structure of the WLAN module */
    struct HdfDeviceObject *device; /* *< Structure of the Device Object */
    struct HdfWlanDevice *wlanDevice[MAX_WLAN_DEVICE];
};

static struct HdfWifiProductData *g_hdfWlanProductData = NULL;

int HdfWlanAddDevice(struct HdfWlanDevice *device)
{
    uint8_t i;
    if (device == NULL) {
        HDF_LOGE("%s:input is NULL!", __func__);
        return HDF_FAILURE;
    }
    if (g_hdfWlanProductData == NULL) {
        HDF_LOGE("%s:please Init product first!", __func__);
        return HDF_FAILURE;
    }
    for (i = 0; i < MAX_WLAN_DEVICE; i++) {
        if (g_hdfWlanProductData->wlanDevice[i] == NULL) {
            g_hdfWlanProductData->wlanDevice[i] = device;
            device->id = i;
            return HDF_SUCCESS;
        }
    }
    HDF_LOGE("%s: device list is full!", __func__);
    return HDF_FAILURE;
}

int HdfWlanInitProduct(struct HdfDeviceObject *device, const struct HdfConfigWlanModuleConfig *config)
{
    int ret;
    if (g_hdfWlanProductData != NULL) {
        HDF_LOGE("%s:already inited!", __func__);
        return HDF_FAILURE;
    }
    g_hdfWlanProductData = OsalMemCalloc(sizeof(struct HdfWifiProductData));
    if (g_hdfWlanProductData == NULL) {
        HDF_LOGE("%s:oom", __func__);
        return HDF_FAILURE;
    }
    ret = InitWifiModule(&(g_hdfWlanProductData->module), config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:InitWifiModule failed! ret=%d", __func__, ret);
        OsalMemFree(g_hdfWlanProductData);
        g_hdfWlanProductData = NULL;
        return ret;
    }
    g_hdfWlanProductData->device = device;

    return HDF_SUCCESS;
}

int HdfWlanSendBroadcastEvent(uint32_t id, const struct HdfSBuf *data)
{
    if (g_hdfWlanProductData == NULL) {
        return HDF_FAILURE;
    }
    return HdfDeviceSendEvent(g_hdfWlanProductData->device, id, data);
}

struct WifiModule *HdfWlanGetModule(void)
{
    if (g_hdfWlanProductData == NULL) {
        return NULL;
    }
    return &g_hdfWlanProductData->module;
}

struct HdfDeviceObject *HdfWlanGetDevice(void)
{
    if (g_hdfWlanProductData == NULL) {
        return NULL;
    }
    return g_hdfWlanProductData->device;
}

struct HdfWlanDevice *HdfWlanGetWlanDevice(uint8_t chipId)
{
    if (chipId >= MAX_WLAN_DEVICE || g_hdfWlanProductData == NULL) {
        return NULL;
    }
    return g_hdfWlanProductData->wlanDevice[chipId];
}

void HdfWlanDeinitProduct(void)
{
    if (g_hdfWlanProductData != NULL) {
        OsalMemFree(g_hdfWlanProductData);
        g_hdfWlanProductData = NULL;
    }
}
