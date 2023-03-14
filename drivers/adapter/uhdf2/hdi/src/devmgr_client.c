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

#include <securec.h>
/*
 * The "devmgr_service_stub.h" header file should be used. Actually, only the interface ID definition and service name
 * are required here. This dependency can be removed when interface proxy impl is automatically generated from IDL.
 */
#include <devmgr_service_stub.h>
#include <hdf_base.h>
#include <hdf_log.h>
#include <hdf_remote_service.h>
#include <osal_mem.h>
#include <servmgr_hdi.h>
#include "devmgr_hdi.h"

#define HDF_LOG_TAG devmgr_interface

static void DevmgrFreeQueryDeviceListImpl(struct DeviceInfoList *list);

static int32_t DeviceManagerHdiCall(struct HDIDeviceManager *iDevMgr, int32_t id,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (iDevMgr->remote == NULL || iDevMgr->remote->dispatcher == NULL ||
        iDevMgr->remote->dispatcher->Dispatch == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    return iDevMgr->remote->dispatcher->Dispatch(iDevMgr->remote, id, data, reply);
}

static int32_t HdfObtainDeviceInfo(struct DeviceInfoList *list, struct HdfSBuf *reply)
{
    struct DeviceInfoNode *node = NULL;
    const char *svrName = NULL;
    int32_t svrNameLen;
    char *base = NULL;
    int32_t deviceType;

    while ((svrName = HdfSbufReadString(reply))) {
        svrNameLen = strlen(svrName) + 1;
        base = (char *)OsalMemCalloc(sizeof(*node) + svrNameLen);
        if (base == NULL) {
            DevmgrFreeQueryDeviceListImpl(list);
            return HDF_FAILURE;
        }
        node = (struct DeviceInfoNode *)base;
        node->svcName = base + sizeof(*node);
        if (strcpy_s(node->svcName, svrNameLen, svrName) != EOK) {
            HDF_LOGE("strcpy service name %{public}s failed", svrName);
            OsalMemFree(base);
            continue;
        }
        HDF_LOGD("%{public}s %{public}s", __func__, svrName);
        HdfSbufReadInt32(reply, &deviceType);
        if (deviceType != HDF_LOCAL_SERVICE && deviceType != HDF_REMOTE_SERVICE) {
            HDF_LOGE("device type error %{public}d ", deviceType);
            OsalMemFree(base);
            continue;
        }
        node->deviceType = deviceType;
        DListInsertTail(&node->node, &list->list);
        list->deviceCnt++;
    }

    return HDF_SUCCESS;
}

static int32_t DevmgrQueryDeviceInfo(struct HDIDeviceManager *iDevMgr, struct DeviceInfoList *list, int32_t type)
{
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *data = NULL;
    int32_t ret;

    reply = HdfSbufTypedObtain(SBUF_IPC);
    if (reply == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    data = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL) {
        HdfSbufRecycle(reply);
        return HDF_ERR_MALLOC_FAIL;
    }
    do {
        if (!HdfRemoteServiceWriteInterfaceToken(iDevMgr->remote, data)) {
            ret = HDF_FAILURE;
            break;
        }
        list->deviceCnt = 0;
        DListHeadInit(&list->list);

        HdfSbufWriteInt32(data, type);
        ret = DeviceManagerHdiCall(iDevMgr, DEVMGR_SERVICE_QUERY_DEVICE, data, reply);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("DevmgrProxyQueryDevice failed");
            break;
        }

        ret = HdfObtainDeviceInfo(list, reply);
    } while (0);

    HdfSbufRecycle(reply);
    HdfSbufRecycle(data);
    return ret;
}

static int32_t DevmgrQueryUsableDeviceInfo(struct HDIDeviceManager *self, struct DeviceInfoList *list)
{
    if (self == NULL || list == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    return DevmgrQueryDeviceInfo(self, list, HDF_SERVICE_USABLE);
}

static int32_t DevmgrQueryUnusableDeviceInfo(struct HDIDeviceManager *self, struct DeviceInfoList *list)
{
    if (self == NULL || list == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    return DevmgrQueryDeviceInfo(self, list, HDF_SERVICE_UNUSABLE);
}

static void DevmgrFreeQueryDeviceListImpl(struct DeviceInfoList *list)
{
    if (list == NULL) {
        return;
    }

    struct DeviceInfoNode *devNode = NULL;
    struct DeviceInfoNode *tmp = NULL;
    DLIST_FOR_EACH_ENTRY_SAFE(devNode, tmp, &list->list, struct DeviceInfoNode, node) {
        DListRemove(&devNode->node);
        OsalMemFree(devNode);
    }
    list->deviceCnt = 0;
}

static void DevmgrFreeQueryDeviceList(struct HDIDeviceManager *self, struct DeviceInfoList *list)
{
    (void)self;
    DevmgrFreeQueryDeviceListImpl(list);
}

int32_t DevmgrLoadDevice(struct HDIDeviceManager *iDevMgr, const char *serviceName)
{
    int32_t status = HDF_FAILURE;
    if (iDevMgr == NULL || serviceName == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGI("load device: %{public}s", serviceName);
    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    do {
        if (data == NULL) {
            status = HDF_ERR_MALLOC_FAIL;
            break;
        }
        if (!HdfRemoteServiceWriteInterfaceToken(iDevMgr->remote, data)) {
            status = HDF_FAILURE;
            break;
        }
        if (!HdfSbufWriteString(data, serviceName)) {
            HDF_LOGE("%{public}s: writing service name failed!", __func__);
            break;
        }
        status = DeviceManagerHdiCall(iDevMgr, DEVMGR_SERVICE_LOAD_DEVICE, data, NULL);
        if (status != HDF_SUCCESS) {
            HDF_LOGE("failed to load device %{public}s", serviceName);
        }
    } while (0);

    HdfSbufRecycle(data);
    return status;
}

int32_t DevmgrUnloadDevice(struct HDIDeviceManager *iDevMgr, const char *serviceName)
{
    int32_t status = HDF_FAILURE;
    if (iDevMgr == NULL || serviceName == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGI("unload device: %{public}s", serviceName);

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    do {
        if (data == NULL) {
            status = HDF_ERR_MALLOC_FAIL;
            break;
        }
        if (!HdfRemoteServiceWriteInterfaceToken(iDevMgr->remote, data)) {
            status = HDF_FAILURE;
            break;
        }
        if (!HdfSbufWriteString(data, serviceName)) {
            HDF_LOGE("%{public}s: writing service name failed!", __func__);
            break;
        }
        status = DeviceManagerHdiCall(iDevMgr, DEVMGR_SERVICE_UNLOAD_DEVICE, data, NULL);
        if (status != HDF_SUCCESS) {
            HDF_LOGE("failed to unload device %{public}s", serviceName);
        }
    } while (0);

    HdfSbufRecycle(data);
    return status;
}

static void HDIDeviceManagerConstruct(struct HDIDeviceManager *inst)
{
    inst->FreeQueryDeviceList = DevmgrFreeQueryDeviceList;
    inst->QueryUsableDeviceInfo = DevmgrQueryUsableDeviceInfo;
    inst->QueryUnusableDeviceInfo = DevmgrQueryUnusableDeviceInfo;
    inst->LoadDevice = DevmgrLoadDevice;
    inst->UnloadDevice = DevmgrUnloadDevice;
}

struct HDIDeviceManager *HDIDeviceManagerGet(void)
{
    struct HDIServiceManager *serviceMgr = HDIServiceManagerGet();
    if (serviceMgr == NULL) {
        return NULL;
    }
    struct HdfRemoteService *remote = serviceMgr->GetService(serviceMgr, DEVICE_MANAGER_SERVICE);
    HDIServiceManagerRelease(serviceMgr);
    if (remote == NULL) {
        HDF_LOGE("%{public}s: hdi service %{public}s not found", __func__, DEVICE_MANAGER_SERVICE);
        return NULL;
    }

    struct HDIDeviceManager *iDevMgr = OsalMemAlloc(sizeof(struct HDIDeviceManager));
    if (iDevMgr == NULL) {
        HDF_LOGE("%{public}s: OOM", __func__);
        HdfRemoteServiceRecycle(remote);
        return NULL;
    }
    if (!HdfRemoteServiceSetInterfaceDesc(remote, "HDI.IDeviceManager.V1_0")) {
        HDF_LOGE("%{public}s: failed to init interface desc", __func__);
        HdfRemoteServiceRecycle(remote);
        return NULL;
    }
    iDevMgr->remote = remote;
    HDIDeviceManagerConstruct(iDevMgr);
    return iDevMgr;
}

void HDIDeviceManagerRelease(struct HDIDeviceManager *devmgr)
{
    if (devmgr == NULL) {
        return;
    }

    HdfRemoteServiceRecycle(devmgr->remote);
    OsalMemFree(devmgr);
}