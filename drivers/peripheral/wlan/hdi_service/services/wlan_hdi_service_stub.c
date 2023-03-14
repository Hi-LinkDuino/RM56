/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "wlan_hdi_service_stub.h"
#include <osal_time.h>
#include <osal_mem.h>
#include <securec.h>
#include "hdf_device_object.h"

struct IWiFi *g_wifi = NULL;
struct IWiFiAp *g_apFeature = NULL;
struct IWiFiSta *g_staFeature = NULL;
struct IWiFiBaseFeature *g_baseFeature = NULL;
const uint32_t DEFAULT_COMBO_SIZE = 10;
const uint32_t RESET_TIME = 20;
const int32_t WLAN_FREQ_MAX_NUM = 35;
const int32_t WLAN_MAX_NUM_STA_WITH_AP = 4;
#define ETH_ADDR_LEN 6

static int32_t WifiServiceCallback(struct HdfDeviceObject *device, struct HdfRemoteService *callback,
    uint32_t eventId, void *data, const char *ifName)
{
    (void)device;
    int ret = HDF_SUCCESS;
    int32_t *code;
    WifiScanResult *scanResult;

    HDF_LOGI("WifiServiceCallback enter , eventId = %{public}u", eventId);
    struct HdfSBuf *dataSbuf = HdfSbufTypedObtain(SBUF_IPC);
    if (dataSbuf == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_FAILURE;
    }
    switch (eventId) {
        case WIFI_EVENT_RESET_DRIVER:
            if (!HdfSbufWriteString(dataSbuf, ifName)) {
                HDF_LOGE("%{public}s: write ifeature->ifName failed!", __func__);
                goto finished;
            }
            code = (int32_t *)data;
            if (!HdfSbufWriteInt32(dataSbuf, *code)) {
                HDF_LOGE("%s: code write failed!", __func__);
                goto finished;
            }
            break;
        case WIFI_EVENT_SCAN_RESULT:
            if (!HdfSbufWriteString(dataSbuf, ifName)) {
                HDF_LOGE("%{public}s: write ifeature->ifName failed!", __func__);
                goto finished;
            }
            scanResult = (WifiScanResult *)data;
            if (!HdfSbufWriteBuffer(dataSbuf, (const void *)scanResult, sizeof(WifiScanResult))) {
                HDF_LOGE("%{public}s:write ifeature->ifName failed!", __func__);
                goto finished;
            }
            break;
        default:
            goto finished;
    }
    ret = callback->dispatcher->Dispatch(callback, eventId, dataSbuf, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("failed to do callback, error code: %d", ret);
    }

finished:
    HdfSbufRecycle(dataSbuf);
    return ret;
}

static const struct WifiHdi wifiHdiImpl = {
    .callback = WifiServiceCallback,
};

const struct WifiHdi *WifiHdiImplInstance()
{
    return &wifiHdiImpl;
}

static int32_t HdiWifiConstruct(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)reply;

    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = WifiConstruct(&g_wifi);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s construct WiFi failed! error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    return ret;
}

static int32_t HdiWifiDeConstruct(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)reply;

    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = WifiDestruct(&g_wifi);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s destruct WiFi failed! error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    return ret;
}

static int32_t WlanServiceStubStart(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)reply;
    int32_t ret;

    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    ret = g_wifi->start(g_wifi);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s start WiFi failed! error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    return ret;
}

static int32_t WlanServiceStubStop(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)reply;
    int32_t ret;

    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    ret = g_wifi->stop(g_wifi);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s stop WiFi failed! error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    return ret;
}

static int32_t WlanServiceStubGetSupportFeature(struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    int32_t ret;
    uint32_t len = PROTOCOL_80211_IFTYPE_NUM + 1;
    uint8_t support[PROTOCOL_80211_IFTYPE_NUM + 1] = {0};

    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    ret = g_wifi->getSupportFeature(support, len);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s g_wifi->getSupportFeature get support feature failed! error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    HdfSbufWriteUint32(reply, len);
    HdfSbufWriteUnpadBuffer(reply, support, len);
    return ret;
}

static int32_t WlanServiceStubGetSupportCombo(struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    int32_t ret;
    uint64_t combo[DEFAULT_COMBO_SIZE] = {0};

    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    ret = g_wifi->getSupportCombo(combo, DEFAULT_COMBO_SIZE);
    if (ret == HDF_ERR_NOT_SUPPORT) {
        HDF_LOGW("%s: not support to getting combo!, error code: %d", __func__, ret);
        return HDF_ERR_NOT_SUPPORT;
    } else if (ret == HDF_SUCCESS) {
        for (int i = 0; i < ETH_ADDR_LEN; i++) {
            ret = HdfSbufWriteUint64(reply, combo[i]);
            if (ret != HDF_SUCCESS) {
                HDF_LOGE("%s: write combo failed!, error code: %d", __func__, ret);
            }
        }
    }
    return ret;
}

static void FreeFeature(struct FeatureInfo *feature)
{
    if (feature->ifName != NULL) {
        free(feature->ifName);
        feature->ifName = NULL;
    }
    OsalMemFree(feature);
}

static int32_t WlanServiceStudCreateFeature(struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    int32_t ret;
    uint8_t wlanType;

    if (data == NULL) {
        HDF_LOGE("%s: Data is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint8(data, &wlanType)) {
        HDF_LOGE("%s: read wlanType failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    struct FeatureInfo *feature = (struct FeatureInfo *)OsalMemCalloc(sizeof(struct FeatureInfo));
    if (feature == NULL) {
        HDF_LOGE("%s: OsalMemCalloc failed", __func__);
        return HDF_FAILURE;
    }
    if (wlanType == PROTOCOL_80211_IFTYPE_AP) {
        ret = g_wifi->createFeature(wlanType, (struct IWiFiBaseFeature **)&g_apFeature);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: createFeature failed, error code: %d", __func__, ret);
            OsalMemFree(feature);
            return HDF_FAILURE;
        }
        feature->type = g_apFeature->baseFeature.type;
        feature->ifName = strdup((g_apFeature->baseFeature).ifName);
    } else if (wlanType == PROTOCOL_80211_IFTYPE_STATION) {
        ret = g_wifi->createFeature(wlanType, (struct IWiFiBaseFeature **)&g_staFeature);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: createFeature failed, error code: %d", __func__, ret);
            OsalMemFree(feature);
            return HDF_FAILURE;
        }
        feature->type = g_staFeature->baseFeature.type;
        feature->ifName = strdup((g_staFeature->baseFeature).ifName);
    } else {
        HDF_LOGE("%s: wlan type is Invalid", __func__);
        FreeFeature(feature);
        ret = HDF_FAILURE;
    }
    if (!HdfSbufWriteString(reply, feature->ifName)) {
        HDF_LOGE("HdfSbufWriteString is failed!");
        FreeFeature(feature);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(reply, feature->type)) {
        HDF_LOGE("%s: feature->type write failed!", __func__);
        FreeFeature(feature);
        return HDF_FAILURE;
    }
    FreeFeature(feature);
    return ret;
}

static int32_t WlanServiceStudGetFeatureByIfName(struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    int32_t ret;
    struct IWiFiBaseFeature *baseFeature = NULL;

    if (data == NULL) {
        HDF_LOGE("%s: Data is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    const char *ifName = HdfSbufReadString(data);
    ret = g_wifi->getFeatureByIfName(ifName, (struct IWiFiBaseFeature **)&baseFeature);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s get FeatureByIfName failed!, error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGI("%{public}s: start WlanDestroyFeature! baseFeature->type = %{public}d", __func__, baseFeature->type);
    if (!HdfSbufWriteInt32(reply, baseFeature->type)) {
        HDF_LOGE("%{public}s: write wlanType failed!", __func__);
        return HDF_FAILURE;
    }
    return ret;
}

static int32_t HdfWlanAddRemoteObj(struct HdfDeviceIoClient *client, struct HdfRemoteService *callbackObj)
{
    struct HdfWlanRemoteNode *pos = NULL;
    struct DListHead *head = &HdfStubDriver()->remoteListHead;

    if ((client == NULL) || (callbackObj == NULL)) {
        HDF_LOGE("%s: client == NULL or callbackObj == NULL", __func__);
        return HDF_FAILURE;
    }
    if (!DListIsEmpty(head)) {
        DLIST_FOR_EACH_ENTRY(pos, head, struct HdfWlanRemoteNode, node) {
            if (pos->client == client) {
                HDF_LOGE("%s: pos->client == client", __func__);
                return HDF_FAILURE;
            }
        }
    }
    struct HdfWlanRemoteNode *newRemoteNode =
        (struct HdfWlanRemoteNode*)OsalMemCalloc(sizeof(struct HdfWlanRemoteNode));
    if (newRemoteNode == NULL) {
        HDF_LOGE("%s:newRemoteNode == NULL", __func__);
        return HDF_FAILURE;
    }
    newRemoteNode->client = client;
    newRemoteNode->callbackObj = callbackObj;
    DListInsertTail(&newRemoteNode->node, head);
    return HDF_SUCCESS;
}

static int32_t HdfWLanCallbackFun(uint32_t event, void *data, const char *ifName)
{
    struct HdfWlanRemoteNode *pos = NULL;
    struct DListHead *head = &HdfStubDriver()->remoteListHead;
    DLIST_FOR_EACH_ENTRY(pos, head, struct HdfWlanRemoteNode, node) {
        if (pos->client == NULL) {
            HDF_LOGW("%s: pos->client == NULL", __func__);
            continue;
        }
        if ((pos->callbackObj == NULL) || (pos->callbackObj->dispatcher == NULL)
            || (pos->callbackObj->dispatcher->Dispatch == NULL)) {
            HDF_LOGE("%s: ptr null!", __func__);
            return HDF_FAILURE;
        }
        int32_t ret = WifiHdiImplInstance()->callback(pos->client->device, pos->callbackObj, event, data, ifName);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: dispatch code fialed, error code: %d", __func__, ret);
            return ret;
        }
    }
    return HDF_SUCCESS;
}

static int32_t WlanServiceStudRegCallback(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)reply;
    int32_t ret;

    if (data == NULL) {
        HDF_LOGE("%s: Data is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct HdfRemoteService *callback = HdfSbufReadRemoteService(data);
    if (callback == NULL) {
        HDF_LOGE("%s: obtain callback failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    (void)OsalMutexLock(&HdfStubDriver()->mutex);
    ret = HdfWlanAddRemoteObj(client, callback);
    if (ret != HDF_SUCCESS) {
        (void)OsalMutexUnlock(&HdfStubDriver()->mutex);
        HDF_LOGE("%s: HdfSensorAddRemoteObj false", __func__);
        return ret;
    }
    ret = g_wifi->registerEventCallback(HdfWLanCallbackFun, "wlan0");
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Register failed!, error code: %d", __func__, ret);
    }
    (void)OsalMutexUnlock(&HdfStubDriver()->mutex);
    return ret;
}

static void HdfWlanDelRemoteObj(struct HdfDeviceIoClient *client)
{
    struct HdfWlanRemoteNode *pos = NULL;
    struct HdfWlanRemoteNode *tmp = NULL;
    struct DListHead *head = &HdfStubDriver()->remoteListHead;

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, head, struct HdfWlanRemoteNode, node) {
        if (pos->client == client) {
            DListRemove(&(pos->node));
            OsalMemFree(pos);
            break;
        }
    }
}

static int32_t WlanServiceStubUnRegCallback(struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    (void)reply;
    (void)data;
    (void)OsalMutexLock(&HdfStubDriver()->mutex);
    HdfWlanDelRemoteObj(client);

    if (DListIsEmpty(&HdfStubDriver()->remoteListHead)) {
        int32_t ret = g_wifi->unregisterEventCallback(HdfWLanCallbackFun, "wlan0");
        if (ret != HDF_SUCCESS) {
            (void)OsalMutexUnlock(&HdfStubDriver()->mutex);
            HDF_LOGE("%s: Unregister failed!, error code: %d", __func__, ret);
            return ret;
        }
    }
    (void)OsalMutexUnlock(&HdfStubDriver()->mutex);
    return HDF_SUCCESS;
}

static int32_t WlanServiceStubDestoryFeature(struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    int32_t ret;
    int32_t wlanType;

    if (data == NULL) {
        HDF_LOGE("%s: data is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    const char *name = HdfSbufReadString(data);
    if (name == NULL) {
        HDF_LOGE(" %s: read name failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadInt32(data, &wlanType)) {
        HDF_LOGE(" %s: read chipid failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (wlanType == PROTOCOL_80211_IFTYPE_AP) {
        ret = strcpy_s((g_apFeature->baseFeature).ifName, (strlen(name) + 1), name);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: strcpy_s is failed!, error code: %d", __func__, ret);
            return HDF_FAILURE;
        }
        ret = g_wifi->destroyFeature(&(g_apFeature->baseFeature));
    } else if (wlanType == PROTOCOL_80211_IFTYPE_STATION) {
        ret = strcpy_s((g_staFeature->baseFeature).ifName, (strlen(name) + 1), name);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: strcpy_s is failed!, error code: %d", __func__, ret);
            return HDF_FAILURE;
        }
        ret = g_wifi->destroyFeature(&(g_staFeature->baseFeature));
    } else {
        HDF_LOGE("%s: wlan type is invalid", __func__);
        ret = HDF_FAILURE;
    }
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s destroy feature failed!, error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    return ret;
}

static int32_t WlanServiceStubResetDriver(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)reply;
    int32_t ret;
    const uint8_t chipId = 0;

    if (data == NULL) {
        HDF_LOGE("%s: data is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint8(data, (uint8_t *)&chipId)) {
        HDF_LOGE(" %s: read chipid failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = g_wifi->resetDriver(chipId, "wlan0");
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s reset driver failed! error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    OsalSleep(RESET_TIME);
    return ret;
}

static int32_t WlanServiceStubGetAsscociateSta(struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    int32_t ret;
    struct StaInfo staInfo[WLAN_MAX_NUM_STA_WITH_AP] = {{0}};
    uint32_t len = 0;

    if (data == NULL) {
        HDF_LOGE("%s: data is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    const char *name = HdfSbufReadString(data);
    if (name == NULL) {
        HDF_LOGE("%s: name is NULL", __func__);
        return HDF_FAILURE;
    }
    ret = strcpy_s(((g_apFeature->baseFeature).ifName), (strlen(name) + 1), name);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: strcpy_s is failed!, error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    ret = g_apFeature->getAsscociatedStas(g_apFeature, staInfo, sizeof(struct StaInfo), &len);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s get asscociated sta failed!, error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    HdfSbufWriteUint32(reply, len);
    if (!HdfSbufWriteBuffer(data, (const void *)staInfo, sizeof(struct StaInfo))) {
        HDF_LOGE("%{public}s:write ifeature->ifName failed!", __func__);
        return HDF_FAILURE;
    }
    return ret;
}

static int32_t WlanServiceStubSetCountryCode(struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    (void)reply;
    int32_t ret;

    if (data == NULL) {
        HDF_LOGE("%s: data is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    const char *name = HdfSbufReadString(data);
    if (name == NULL) {
        HDF_LOGE("%s: name is NULL", __func__);
        return HDF_FAILURE;
    }
    const char *code = HdfSbufReadString(data);
    if (code == NULL) {
        HDF_LOGE("%s: code is NULL", __func__);
        return HDF_FAILURE;
    }
    ret = strcpy_s(((g_apFeature->baseFeature).ifName), (strlen(name) + 1), name);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: strcpy_s is failed!, error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    ret = g_apFeature->setCountryCode(g_apFeature, code, strlen(code));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s set country code failed!, error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    return ret;
}

static int32_t WlanServiceStubGetNetworkName(struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    if (data == NULL) {
        HDF_LOGE("%s: data is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    const char *name = HdfSbufReadString(data);
    if (name == NULL) {
        HDF_LOGE("%s: name is NULL", __func__);
        return HDF_FAILURE;
    }
    int ret = strcpy_s(((g_apFeature->baseFeature).ifName), (strlen(name) + 1), name);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: strcpy_s is failed!, error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    const char *ifName = g_apFeature->baseFeature.getNetworkIfaceName(&g_apFeature->baseFeature);
    if (ifName == NULL) {
        HDF_LOGE("%s get network iface name failed!, error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    HdfSbufWriteString(reply, ifName);
    return HDF_SUCCESS;
}

static int32_t WlanServiceStubGetFeatureType(struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    int32_t type;
    int32_t feature_type = 0;

    if (data == NULL) {
        HDF_LOGE("%s: data is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    HdfSbufReadInt32(data, &feature_type);
    g_apFeature->baseFeature.type = feature_type;
    type = g_apFeature->baseFeature.getFeatureType(&g_apFeature->baseFeature);
    HdfSbufWriteInt32(reply, type);
    return HDF_SUCCESS;
}

static int32_t WlanServcieStubSetMacAddr(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)reply;
    int32_t ret;
    uint8_t mac[ETH_ADDR_LEN] = {0};
    int32_t wlanType = PROTOCOL_80211_IFTYPE_AP;

    if (data == NULL) {
        HDF_LOGE("%s: data is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    const char *name = HdfSbufReadString(data);
    if (name == NULL) {
        HDF_LOGE("%s: name is NULL", __func__);
        return HDF_FAILURE;
    }
    for (int i = 0; i < ETH_ADDR_LEN; i++) {
        if (!HdfSbufReadUint8(data, &mac[i])) {
            HDF_LOGE(" %s: read mac failed", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
    }
    if (wlanType == PROTOCOL_80211_IFTYPE_AP) {
        ret = strcpy_s((g_apFeature->baseFeature).ifName, (strlen(name) + 1), name);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: strcpy_s is failed!, error code: %d", __func__, ret);
            return HDF_FAILURE;
        }
        ret = g_apFeature->baseFeature.setMacAddress((struct IWiFiBaseFeature *)g_apFeature,
                                                     (unsigned char *)mac, ETH_ADDR_LEN);
    } else if (wlanType == PROTOCOL_80211_IFTYPE_STATION) {
        ret = strcpy_s((g_staFeature->baseFeature).ifName, (strlen(name) + 1), name);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: strcpy_s is failed!, error code: %d", __func__, ret);
            return HDF_FAILURE;
        }
        ret = g_staFeature->baseFeature.setMacAddress((struct IWiFiBaseFeature *)g_staFeature,
                                                      (unsigned char *)mac, ETH_ADDR_LEN);
    } else {
        HDF_LOGE("%s: wlan type is invalid", __func__);
        ret = HDF_FAILURE;
    }
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s set mac address failed!, error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    return ret;
}

static int32_t WlanServiceGetMacAddr(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint8_t mac[ETH_ADDR_LEN] = {0};
    int32_t wlanType;

    if (data == NULL) {
        HDF_LOGE("%s: data is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    const char *name = HdfSbufReadString(data);
    if (name == NULL) {
        HDF_LOGE("%s: name is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(data, &wlanType)) {
        HDF_LOGE(" %s: read chipid failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (wlanType == PROTOCOL_80211_IFTYPE_AP) {
        ret = strcpy_s((g_apFeature->baseFeature).ifName, (strlen(name) + 1), name);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: strcpy_s is failed!, error code: %d", __func__, ret);
            return HDF_FAILURE;
        }
        ret = g_apFeature->baseFeature.getDeviceMacAddress((struct IWiFiBaseFeature *)g_apFeature,
                                                           (unsigned char *)mac, ETH_ADDR_LEN);
    } else if (wlanType == PROTOCOL_80211_IFTYPE_STATION) {
        ret = strcpy_s((g_staFeature->baseFeature).ifName, (strlen(name) + 1), name);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: strcpy_s is failed!, error code: %d", __func__, ret);
            return HDF_FAILURE;
        }
        ret = g_staFeature->baseFeature.getDeviceMacAddress((struct IWiFiBaseFeature *)g_staFeature,
                                                            (unsigned char *)mac, ETH_ADDR_LEN);
    } else {
        HDF_LOGE("%s: wlan type is invalid", __func__);
        ret = HDF_FAILURE;
    }
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s get mac address failed!, error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    HdfSbufWriteUnpadBuffer(reply, mac, ETH_ADDR_LEN);
    return ret;
}

static int32_t WlanServiceStubGetFreqWithband(struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    int32_t ret;
    int32_t freq[WLAN_FREQ_MAX_NUM] = {0};
    int32_t wlanBand = 0;
    uint32_t size = 0;
    uint32_t count = 0;

    if (data == NULL) {
        HDF_LOGE("%s: data is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    const char *name = HdfSbufReadString(data);
    if (name == NULL) {
        HDF_LOGE("%s: name is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(data, &wlanBand)) {
        HDF_LOGE(" %s: read wlan_band failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, &size)) {
        HDF_LOGE(" %s: read size failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = strcpy_s((g_apFeature->baseFeature).ifName, (strlen(name) + 1), name);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: strcpy_s is failed!, error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    ret = g_apFeature->baseFeature.getValidFreqsWithBand((struct IWiFiBaseFeature *)g_apFeature,
                                                         wlanBand, freq, size, &count);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s get valid freqs failed!, error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    HdfSbufWriteUint32(reply, count);
    for (uint32_t i = 0; i < count; i++) {
        if (!HdfSbufWriteInt32(reply, freq[i])) {
            HDF_LOGE("%s: write freq failed", __func__);
            return HDF_FAILURE;
        }
    }
    return ret;
}

static int32_t WlanServiceStubSetTxPowr(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)reply;
    int32_t ret;
    int32_t power;

    if (data == NULL) {
        HDF_LOGE("%s: data is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    const char *name = HdfSbufReadString(data);
    if (name == NULL) {
        HDF_LOGE("%s: name is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(data, &power)) {
        HDF_LOGE(" %s: read power failed ", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = strcpy_s((g_apFeature->baseFeature).ifName, (strlen(name) + 1), name);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: strcpy_s is failed!, error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    ret = g_apFeature->baseFeature.setTxPower((struct IWiFiBaseFeature *)g_apFeature, power);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s set tx power failed!, error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    return ret;
}

static int32_t WlanServiceStubGetChipId(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint8_t chipId = 0;
    int32_t wlanType;

    if (data == NULL) {
        HDF_LOGE("%s: data is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    const char *name = HdfSbufReadString(data);
    if (name == NULL) {
        HDF_LOGE("%s: name is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(data, &wlanType)) {
        HDF_LOGE(" %s: read wlan_band failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (wlanType == PROTOCOL_80211_IFTYPE_AP) {
        ret = strcpy_s((g_apFeature->baseFeature).ifName, (strlen(name) + 1), name);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: strcpy_s is failed!, error code: %d", __func__, ret);
            return HDF_FAILURE;
        }
        ret = g_apFeature->baseFeature.getChipId((struct IWiFiBaseFeature *)g_apFeature, &chipId);
    } else if (wlanType == PROTOCOL_80211_IFTYPE_STATION) {
        ret = strcpy_s((g_staFeature->baseFeature).ifName, (strlen(name) + 1), name);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: strcpy_s is failed!, error code: %d", __func__, ret);
            return HDF_FAILURE;
        }
        ret = g_staFeature->baseFeature.getChipId((struct IWiFiBaseFeature *)g_staFeature, &chipId);
    } else {
        HDF_LOGE("%s: wlan type is Invalid", __func__);
        ret = HDF_FAILURE;
    }
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s get chipid failed!, error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint8(reply, chipId)) {
        HDF_LOGE("%s write chipId failed", __func__);
        return HDF_FAILURE;
    }
    return ret;
}

static int32_t WlanServiceStubGetNameByChipId(struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    int32_t ret;
    uint32_t num = 0;
    uint8_t chipId = 10;
    char *ifName = NULL;

    if (data == NULL) {
        HDF_LOGE("%s: data is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint8(data, &chipId)) {
        HDF_LOGE("%s: read chipid failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = g_staFeature->baseFeature.getIfNamesByChipId(chipId, &ifName, &num);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s get name failed!, error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint8(reply, num)) {
        HDF_LOGE("%s: write num failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteString(reply, ifName)) {
        HDF_LOGE("%s: write ifNames failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    return ret;
}

static int32_t WlanServiceStubSetScanMacAddr(struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    (void)reply;
    int32_t ret;
    uint8_t scanMac[ETH_ADDR_LEN] = {0};
    uint8_t length = 0;

    if (data == NULL) {
        HDF_LOGE("%s: data is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    const char *name = HdfSbufReadString(data);
    if (name == NULL) {
        HDF_LOGE("%s: name is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint8(data, &length)) {
        HDF_LOGE(" %s: read length failed", __func__);
        return HDF_FAILURE;
    }
    for (int i = 0; i < ETH_ADDR_LEN; i++) {
        if (!HdfSbufReadUint8(data, &scanMac[i])) {
           HDF_LOGE(" %s: read scanMac failed", __func__);
           return HDF_FAILURE;
        }
    }
    ret = strcpy_s((g_staFeature->baseFeature).ifName, (strlen(name) + 1), name);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: strcpy_s is failed!, error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    ret = g_staFeature->setScanningMacAddress(g_staFeature, (unsigned char *)scanMac, length);

    return ret;
}

static int32_t WlanServiceStubGetNetdevInfo(struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    int32_t ret;

    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct NetDeviceInfoResult *netDeviceInfoResult =
        (struct NetDeviceInfoResult *)OsalMemCalloc(sizeof(struct NetDeviceInfoResult));
    if (netDeviceInfoResult == NULL) {
        HDF_LOGE("%s: mem calloc fail!", __func__);
        return HDF_FAILURE;
    }
    ret = g_wifi->getNetDevInfo(netDeviceInfoResult);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get netdev info failed!, error code: %d", __func__, ret);
        OsalMemFree(netDeviceInfoResult);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteBuffer(reply, netDeviceInfoResult, sizeof(struct NetDeviceInfoResult))) {
        HDF_LOGE("%s: write buffer fail!", __func__);
        ret = HDF_ERR_IO;
    }
    OsalMemFree(netDeviceInfoResult);
    return ret;
}

static int32_t WlanServiceStubStartScan(struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    (void)reply;
    int32_t ret;
    uint32_t dataSize = 0;
    WifiScan *scan;

    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    const char *ifName = HdfSbufReadString(data);
    if (ifName == NULL) {
        HDF_LOGE("%s: name is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadBuffer(data, (const void **)(&scan), &dataSize) || dataSize != sizeof(WifiScan)) {
        return HDF_FAILURE;
    }
    ret = g_staFeature->startScan(ifName, scan);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s get netdev info failed!, error code: %d", __func__, ret);
    }
    return ret;
}

int32_t WlanHdiServiceOnRemoteRequest(struct HdfDeviceIoClient *client, int cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    switch (cmdId) {
        case WLAN_SERVICE_CONSTRUCT:
            return HdiWifiConstruct(client, data, reply);
        case WLAN_SERVICE_DECONSTRUCT:
            return HdiWifiDeConstruct(client, data, reply);
        case WLAN_SERVICE_START:
            return WlanServiceStubStart(client, data, reply);
        case WLAN_SERVICE_STOP:
            return WlanServiceStubStop(client, data, reply);
        case WLAN_SERVICE_GET_SUPPORT_FEATURE:
            return WlanServiceStubGetSupportFeature(client, data, reply);
        case WLAN_SERVICE_GET_SUPPORT_COMBO:
            return WlanServiceStubGetSupportCombo(client, data, reply);
        case WLAN_SERVICE_CREATE_FEATURE:
            return WlanServiceStudCreateFeature(client, data, reply);
        case WLAN_SERVICE_GET_FEATURE_NAME:
            return WlanServiceStudGetFeatureByIfName(client, data, reply);
        case WLAN_SERVICE_REGISTER_CALLBACK:
            return WlanServiceStudRegCallback(client, data, reply);
        case WLAN_SERVICE_UNREGISTER_CALLBACK:
            return WlanServiceStubUnRegCallback(client, data, reply);
        case WLAN_SERVICE_DESTROY_FEATURE:
            return WlanServiceStubDestoryFeature(client, data, reply);
        case WLAN_SERVICE_RESET_DRIVER:
            return WlanServiceStubResetDriver(client, data, reply);
        case WLAN_SERVICE_GET_ASSCOCIATE_STA:
            return WlanServiceStubGetAsscociateSta(client, data, reply);
        case WLAN_SERVICE_SET_COUNTRY_CODE:
            return WlanServiceStubSetCountryCode(client, data, reply);
        case WLAN_SERVICE_GET_NETWORK_NAME:
            return WlanServiceStubGetNetworkName(client, data, reply);
        case WLAN_SERVICE_GET_FEATURE_TYPE:
            return WlanServiceStubGetFeatureType(client, data, reply);
        case WLAN_SERVICE_SET_MAC_ADDR:
            return WlanServcieStubSetMacAddr(client, data, reply);
        case WLAN_SERVICE_GET_MAC_ADDR:
            return WlanServiceGetMacAddr(client, data, reply);
        case WLAN_SERVICE_GET_FREQ_WITHBAND:
            return WlanServiceStubGetFreqWithband(client, data, reply);
        case WLAN_SERVICE_SET_TX_POWR:
            return WlanServiceStubSetTxPowr(client, data, reply);
        case WLAN_SERVICE_GET_CHIPID:
            return WlanServiceStubGetChipId(client, data, reply);
        case WLAN_SERVICE_GET_NAME_BYCHIPID:
            return WlanServiceStubGetNameByChipId(client, data, reply);
        case WLAN_SERVICE_SET_SACN_MACADDR:
            return WlanServiceStubSetScanMacAddr(client, data, reply);
        case WLAN_SERVICE_GET_NETDEV_INFO:
            return WlanServiceStubGetNetdevInfo(client, data, reply);
        case WLAN_SERVICE_START_SCAN:
            return WlanServiceStubStartScan(client, data, reply);
        default:
            HDF_LOGW("SampleServiceDispatch: not support cmd %d", cmdId);
            return HDF_ERR_INVALID_PARAM;
    }
    return HDF_SUCCESS;
}

struct HdfWlanStubData *HdfStubDriver(void)
{
    static struct HdfWlanStubData registerManager;
    return &registerManager;
}