/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_device_node_ext.h"
#include "devsvc_manager_clnt.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_io_service.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"
#include "osal_mem.h"

#define HDF_LOG_TAG devnode_ext

static int DeviceNodeExtDispatch(struct HdfObject *stub, int code, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct IDeviceIoService *deviceMethod = NULL;
    struct HdfDeviceNode *devNode = NULL;
    uint64_t ioClientPtr = 0;

    if (stub == NULL) {
        HDF_LOGE("device ext dispatch: stub is null");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint64(reply, &ioClientPtr) || ioClientPtr == 0) {
        HDF_LOGE("device ext dispatch: input ioClient is null");
        return HDF_FAILURE;
    }
    HdfSbufFlush(reply);
    devNode = CONTAINER_OF(stub, struct HdfDeviceNode, deviceObject);
    deviceMethod = devNode->deviceObject.service;
    if (deviceMethod == NULL) {
        HDF_LOGE("device ext dispatch: device service interface is null");
        return HDF_FAILURE;
    }

    if (devNode->policy == SERVICE_POLICY_CAPACITY) {
        if (deviceMethod->Dispatch == NULL) {
            HDF_LOGE("device ext dispatch: remote service dispatch method is null");
            return HDF_FAILURE;
        }
        return deviceMethod->Dispatch((struct HdfDeviceIoClient *)((uintptr_t)ioClientPtr), code, data, reply);
    }
    return HDF_FAILURE;
}

static int DeviceNodeExtPublishService(struct HdfDeviceNode *devNode)
{
    struct HdfDeviceObject *deviceObject = NULL;
    struct DeviceNodeExt *devNodeExt = (struct DeviceNodeExt *)devNode;
    int ret;
    static struct HdfIoDispatcher dispatcher = { .Dispatch = DeviceNodeExtDispatch };

    if (devNodeExt == NULL) {
        return HDF_FAILURE;
    }

    deviceObject = &devNodeExt->super.deviceObject;
    if (deviceObject->service == NULL) {
        HDF_LOGE("device service interface is null");
        return HDF_FAILURE;
    }
    if (devNode->policy == SERVICE_POLICY_CAPACITY) {
        devNodeExt->ioService = HdfIoServicePublish(devNode->servName, devNode->permission);
        if (devNodeExt->ioService != NULL) {
            devNodeExt->ioService->target = (struct HdfObject *)(deviceObject);
            devNodeExt->ioService->dispatcher = &dispatcher;
        } else {
            HDF_LOGE("device remote service publish failed");
            return HDF_DEV_ERR_NO_DEVICE_SERVICE;
        }
    }

    // base(device node) publish inner service
    ret = HdfDeviceNodePublishPublicService(devNode);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("failed to publish device service, ret is %d", ret);
        HdfIoServiceRemove(devNodeExt->ioService);
        devNodeExt->ioService = NULL;
        return ret;
    }

    return HDF_SUCCESS;
}

int DeviceNodeExtRemoveService(struct HdfDeviceNode *devNode)
{
    struct DeviceNodeExt *devNodeExt = CONTAINER_OF(devNode, struct DeviceNodeExt, super);
    if (devNode == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (devNode->policy == SERVICE_POLICY_CAPACITY) {
        HdfIoServiceRemove(devNodeExt->ioService);
        devNodeExt->ioService = NULL;
    }

    // remove inner service published by base
    return HdfDeviceNodeRemoveService(devNode);
}

static void DeviceNodeExtConstruct(struct DeviceNodeExt *inst)
{
    struct IDeviceNode *nodeIf = (struct IDeviceNode *)inst;
    if (nodeIf != NULL) {
        HdfDeviceNodeConstruct(&inst->super);
        nodeIf->PublishService = DeviceNodeExtPublishService;
        nodeIf->RemoveService = DeviceNodeExtRemoveService;
    }
}

static void DeviceNodeExtDestruct(struct DeviceNodeExt *devnode)
{
    if (DeviceNodeExtRemoveService(&devnode->super) != HDF_SUCCESS) {
        HDF_LOGE("failed to remove service");
    }
    HdfDeviceNodeDestruct(&devnode->super);
}

struct HdfObject *DeviceNodeExtCreate(void)
{
    struct DeviceNodeExt *instance = (struct DeviceNodeExt *)OsalMemCalloc(sizeof(struct DeviceNodeExt));
    if (instance != NULL) {
        DeviceNodeExtConstruct(instance);
        instance->ioService = NULL;
    }
    return (struct HdfObject *)instance;
}

void DeviceNodeExtRelease(struct HdfObject *object)
{
    struct DeviceNodeExt *devnode = (struct DeviceNodeExt *)object;
    if (devnode != NULL) {
        DeviceNodeExtDestruct(devnode);
        OsalMemFree(devnode);
    }
}
