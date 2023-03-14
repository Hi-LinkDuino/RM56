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

#include <devsvc_manager_proxy.h>
#include <devsvc_manager_stub.h>
#include <hdf_base.h>
#include <hdf_log.h>
#include <osal_mem.h>
#include "hdf_service_status.h"
#include "servmgr_hdi.h"

struct HDIServiceManagerClient {
    struct HDIServiceManager iservmgr;
    struct HdfRemoteService *remote;
};
int ServiceStatusListenerMarshalling(struct ServiceStatusListener *listener, struct HdfSBuf *buf);

static int32_t ServiceManagerHdiCall(struct HDIServiceManagerClient *servMgrClient, int32_t id,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (servMgrClient->remote == NULL || servMgrClient->remote->dispatcher == NULL ||
        servMgrClient->remote->dispatcher->Dispatch == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    return servMgrClient->remote->dispatcher->Dispatch(servMgrClient->remote, id, data, reply);
}

struct HdfRemoteService *HDIServMgrGetService(struct HDIServiceManager *iServMgr, const char* serviceName)
{
    if (iServMgr == NULL || serviceName == NULL) {
        return NULL;
    }
    struct HDIServiceManagerClient *servMgrClient = CONTAINER_OF(iServMgr, struct HDIServiceManagerClient, iservmgr);
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfRemoteService *service = NULL;

    do {
        data = HdfSbufTypedObtain(SBUF_IPC);
        reply = HdfSbufTypedObtain(SBUF_IPC);
        if (data == NULL || reply == NULL) {
            break;
        }

        if (!HdfRemoteServiceWriteInterfaceToken(servMgrClient->remote, data) ||
            !HdfSbufWriteString(data, serviceName)) {
            break;
        }
        int status = ServiceManagerHdiCall(servMgrClient, DEVSVC_MANAGER_GET_SERVICE, data, reply);
        if (status == HDF_SUCCESS) {
            service = HdfSbufReadRemoteService(reply);
        } else {
            HDF_LOGI("%{public}s: %{public}s not found", __func__, serviceName);
        }
    } while (0);

    if (reply != NULL) {
        HdfSbufRecycle(reply);
    }
    if (data != NULL) {
        HdfSbufRecycle(data);
    }

    return service;
}

int32_t HDIServMgrRegisterServiceStatusListener(struct HDIServiceManager *self,
    struct ServiceStatusListener *listener, uint16_t deviceClass)
{
    if (self == NULL || listener == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct HDIServiceManagerClient *servMgrClient = CONTAINER_OF(self, struct HDIServiceManagerClient, iservmgr);

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfRemoteServiceWriteInterfaceToken(servMgrClient->remote, data) ||
        !HdfSbufWriteUint16(data, deviceClass) ||
        ServiceStatusListenerMarshalling(listener, data) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    int32_t ret = ServiceManagerHdiCall(servMgrClient, DEVSVC_MANAGER_REGISER_SVCLISTENER, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("failed to register hdi service listener");
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t HDIServMgrUnregisterServiceStatusListener(struct HDIServiceManager *self,
    struct ServiceStatusListener *listener)
{
    if (self == NULL || listener == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct HDIServiceManagerClient *servMgrClient = CONTAINER_OF(self, struct HDIServiceManagerClient, iservmgr);
    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfRemoteServiceWriteInterfaceToken(servMgrClient->remote, data) ||
        ServiceStatusListenerMarshalling(listener, data) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    int32_t ret = ServiceManagerHdiCall(servMgrClient, DEVSVC_MANAGER_UNREGISER_SVCLISTENER, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("failed to unregister hdi service listener");
    }
    HdfSbufRecycle(data);
    return ret;
}

void HDIServiceManagerConstruct(struct HDIServiceManager *inst)
{
    inst->GetService = HDIServMgrGetService;
    inst->RegisterServiceStatusListener = HDIServMgrRegisterServiceStatusListener;
    inst->UnregisterServiceStatusListener = HDIServMgrUnregisterServiceStatusListener;
}

struct HDIServiceManager *HDIServiceManagerGet(void)
{
    struct HdfRemoteService *remote = HdfRemoteServiceGet(DEVICE_SERVICE_MANAGER_SA_ID);
    if (remote == NULL) {
        HDF_LOGE("%{public}s: hdi service %{public}s not found", __func__, DEVICE_SERVICE_MANAGER);
        return NULL;
    }

    struct HDIServiceManagerClient *iServMgrClient = OsalMemAlloc(sizeof(struct HDIServiceManagerClient));
    if (iServMgrClient == NULL) {
        HDF_LOGE("%{public}s: OOM", __func__);
        HdfRemoteServiceRecycle(remote);
        return NULL;
    }
    if (!HdfRemoteServiceSetInterfaceDesc(remote, "HDI.IServiceManager.V1_0")) {
        HDF_LOGE("%{public}s: failed to init interface desc", __func__);
        HdfRemoteServiceRecycle(remote);
        return NULL;
    }
    iServMgrClient->remote = remote;

    HDIServiceManagerConstruct(&iServMgrClient->iservmgr);
    return &iServMgrClient->iservmgr;
}

void HDIServiceManagerRelease(struct HDIServiceManager *servmgr)
{
    if (servmgr == NULL) {
        return;
    }
    struct HDIServiceManagerClient *iServMgrClient = CONTAINER_OF(servmgr, struct HDIServiceManagerClient, iservmgr);
    HdfRemoteServiceRecycle(iServMgrClient->remote);
    OsalMemFree(iServMgrClient);
}