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

#include <hdf_dlist.h>
#include <hdf_log.h>
#include <hdf_remote_service.h>
#include <hdf_service_status.h>
#include <osal_mem.h>
#define SERVSTAT_LISTENER_INTERFACE_DESCRIPTOR "HDI.IServiceStatusListener.V1_0"
struct ServstatListenerStub {
    struct ServiceStatusListener listener;
    struct HdfRemoteService *remote;
};

int ServstatListenerStubRemoteDispatch(
    struct HdfRemoteService *remote, int cmdid, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)reply;
    struct ServstatListenerStub *stub = (struct ServstatListenerStub *)remote;
    struct ServiceStatus status;

    if (cmdid != SERVIE_STATUS_LISTENER_NOTIFY) {
        return HDF_ERR_NOT_SUPPORT;
    }
    if (!HdfRemoteServiceCheckInterfaceToken(stub->remote, data)) {
        HDF_LOGE("failed to check interface");
        return HDF_ERR_INVALID_PARAM;
    }
    if (ServiceStatusUnMarshalling(&status, data) != HDF_SUCCESS) {
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGI("ServstatListenerStubRemoteDispatch: service name %{public}s", status.serviceName);

    if (stub->listener.callback != NULL) {
        stub->listener.callback(&stub->listener, &status);
        return HDF_SUCCESS;
    }
    return HDF_ERR_INVALID_OBJECT;
}

int ServiceStatusListenerMarshalling(struct ServiceStatusListener *listener, struct HdfSBuf *buf)
{
    if (listener == NULL || buf == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    struct ServstatListenerStub *listenerStub = CONTAINER_OF(listener, struct ServstatListenerStub, listener);
    if (listenerStub->remote == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return HdfSbufWriteRemoteService(buf, listenerStub->remote);
}

struct ServiceStatusListener *HdiServiceStatusListenerNewInstance(void)
{
    struct ServstatListenerStub *stub = OsalMemCalloc(sizeof(struct ServstatListenerStub));
    if (stub == NULL) {
        return NULL;
    }
    static struct HdfRemoteDispatcher remoteDispatch = {
        .Dispatch = ServstatListenerStubRemoteDispatch,
    };

    stub->remote = HdfRemoteServiceObtain((struct HdfObject *)stub, &remoteDispatch);
    if (stub->remote == NULL) {
        OsalMemFree(stub);
        return NULL;
    }
    if (!HdfRemoteServiceSetInterfaceDesc(stub->remote, SERVSTAT_LISTENER_INTERFACE_DESCRIPTOR)) {
        HdfRemoteServiceRecycle(stub->remote);
        stub->remote = NULL;
        OsalMemFree(stub);
        return NULL;
    }
    return &stub->listener;
}

void HdiServiceStatusListenerFree(struct ServiceStatusListener *listener)
{
    if (listener == NULL) {
        return;
    }

    struct ServstatListenerStub *stub = CONTAINER_OF(listener, struct ServstatListenerStub, listener);
    HdfRemoteServiceRecycle(stub->remote);
    OsalMemFree(stub);
}