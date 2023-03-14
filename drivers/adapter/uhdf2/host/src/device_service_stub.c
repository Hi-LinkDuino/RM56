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

#include "device_service_stub.h"
#include "devsvc_manager_clnt.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"
#include "osal_mem.h"

int DeviceServiceStubDispatch(
    struct HdfRemoteService *stub, int code, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct DeviceServiceStub *service = (struct DeviceServiceStub *)stub;
    struct IDeviceIoService *ioService = service->super.deviceObject.service;
    int ret = HDF_FAILURE;

    if (ioService == NULL) {
        return HDF_FAILURE;
    }

    struct HdfDeviceIoClient client = {
        .device = &service->super.deviceObject,
        .priv = NULL,
    };

    if (ioService->Dispatch != NULL) {
        ret = ioService->Dispatch(&client, code, data, reply);
    }
    return ret;
}

static struct HdfRemoteDispatcher g_deviceServiceDispatcher = {
    .Dispatch = DeviceServiceStubDispatch
};

int DeviceServiceStubPublishService(struct HdfDeviceNode *service)
{
    int status = HDF_FAILURE;
    struct DeviceServiceStub *fullService = (struct DeviceServiceStub *)service;

    if (service->servName == NULL) {
        HDF_LOGE("device %x miss service name", service->devId);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (fullService->remote != NULL) {
        HDF_LOGE("%{public}s:service %{public}s already published", __func__, service->servName);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (service->policy != SERVICE_POLICY_PUBLIC && service->policy != SERVICE_POLICY_CAPACITY) {
        return HDF_ERR_NOT_SUPPORT;
    }

    fullService->remote = HdfRemoteServiceObtain((struct HdfObject *)fullService, &g_deviceServiceDispatcher);
    if (fullService->remote == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    do {
        if (service->interfaceDesc != NULL &&
            !HdfRemoteServiceSetInterfaceDesc(fullService->remote, service->interfaceDesc)) {
            HDF_LOGE("failed to set device service interface desc");
            break;
        }
        struct DevSvcManagerClnt *serviceManager = DevSvcManagerClntGetInstance();
        if (serviceManager == NULL) {
            HDF_LOGE("device service stub failed to publish, svcmgr clnt invalid");
            status = HDF_DEV_ERR_NO_DEVICE;
            break;
        }

        status = DevSvcManagerClntAddService(service->servName, service->deviceObject.deviceClass,
            &fullService->super.deviceObject, service->servInfo);
        if (status != HDF_SUCCESS) {
            break;
        }
        service->servStatus = true;
        return HDF_SUCCESS;
    } while (0);

    HdfRemoteServiceRecycle(fullService->remote);
    fullService->remote = NULL;
    return status;
}

int DeviceServiceStubRemoveService(struct HdfDeviceNode *deviceNode)
{
    struct DevSvcManagerClnt *serviceManager = DevSvcManagerClntGetInstance();
    if (serviceManager == NULL) {
        return HDF_FAILURE;
    }
    DevSvcManagerClntRemoveService(deviceNode->servName);
    struct DeviceServiceStub *instance = (struct DeviceServiceStub *)deviceNode;
    HdfRemoteServiceRecycle(instance->remote);
    instance->remote = NULL;
    return HDF_SUCCESS;
}

void DeviceServiceStubConstruct(struct DeviceServiceStub *inst)
{
    HdfDeviceNodeConstruct(&inst->super);
    struct IDeviceNode *serviceIf = (struct IDeviceNode *)inst;
    if (serviceIf != NULL) {
        serviceIf->PublishService = DeviceServiceStubPublishService;
        serviceIf->RemoveService = DeviceServiceStubRemoveService;
    }
}

struct HdfObject *DeviceServiceStubCreate(void)
{
    struct DeviceServiceStub *instance =
        (struct DeviceServiceStub *)OsalMemCalloc(sizeof(struct DeviceServiceStub));
    if (instance != NULL) {
        DeviceServiceStubConstruct(instance);
    }
    return (struct HdfObject *)instance;
}

void DeviceServiceStubRelease(struct HdfObject *object)
{
    struct DeviceServiceStub *instance = (struct DeviceServiceStub *)object;
    if (instance != NULL) {
        if (instance->remote != NULL) {
            HdfRemoteServiceRecycle(instance->remote);
            instance->remote = NULL;
        }
        HdfDeviceNodeDestruct(&instance->super);
        OsalMemFree(instance);
    }
}

bool HdfDeviceObjectCheckInterfaceDesc(struct HdfDeviceObject *dev, struct HdfSBuf *data)
{
    if (dev == NULL || data == NULL) {
        return false;
    }
    struct HdfDeviceNode *devNode = CONTAINER_OF(dev, struct HdfDeviceNode, deviceObject);
    struct DeviceServiceStub *instance = (struct DeviceServiceStub *)devNode;
    return HdfRemoteServiceCheckInterfaceToken(instance->remote, data);
}