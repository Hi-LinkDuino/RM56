/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_device.h"
#include "hdf_base.h"
#include "hdf_cstring.h"
#include "hdf_device_node.h"
#include "hdf_device_token.h"
#include "hdf_log.h"
#include "hdf_object_manager.h"
#include "hdf_service_observer.h"
#include "osal_mem.h"

#define HDF_LOG_TAG hdf_device

static void UpdateDeivceNodeIdIndex(struct HdfDevice *device, devid_t nodeDevid)
{
    if (device->devidIndex < DEVICEID(nodeDevid)) {
        device->devidIndex = DEVICEID(nodeDevid);
    }
}

static int AcquireNodeDeivceId(struct HdfDevice *device, devid_t *devid)
{
    if (device->devidIndex >= DEVICEID_MASK) {
        return HDF_FAILURE;
    }
    device->devidIndex++;
    *devid = MK_DEVID(HOSTID(device->deviceId), DEVICEID(device->deviceId), device->devidIndex);

    return HDF_SUCCESS;
}

static int HdfDeviceAttach(struct IHdfDevice *devInst, struct HdfDeviceNode *devNode)
{
    int ret;
    struct HdfDevice *device = (struct HdfDevice *)devInst;
    struct IDeviceNode *nodeIf = (struct IDeviceNode *)devNode;

    if (device == NULL || nodeIf == NULL || nodeIf->LaunchNode == NULL) {
        HDF_LOGE("failed to attach device, input params invalid");
        return HDF_ERR_INVALID_PARAM;
    }

    // for dynamic added device node, assign device id here
    if (devNode->devId == 0 && AcquireNodeDeivceId(device, &devNode->devId) != HDF_SUCCESS) {
        HDF_LOGE("failed to attach device, invalid device id");
        return HDF_ERR_INVALID_PARAM;
    }
    devNode->token->devid = devNode->devId;
    ret = nodeIf->LaunchNode(devNode);
    if (ret == HDF_SUCCESS) {
        DListInsertTail(&devNode->entry, &device->devNodes);
        UpdateDeivceNodeIdIndex(device, devNode->devId);
    }

    return ret;
}

int HdfDeviceDetach(struct IHdfDevice *devInst, struct HdfDeviceNode *devNode)
{
    struct HdfDevice *device = NULL;
    if (devInst == NULL || devNode == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    device = CONTAINER_OF(devInst, struct HdfDevice, super);
    if (DEVICEID(device->deviceId) != DEVICEID(devNode->devId)) {
        HDF_LOGE("%s: device %x detach unknown devnode %x", __func__, device->deviceId, devNode->devId);
        return HDF_DEV_ERR_NO_DEVICE;
    }

    if (devNode->entry.next != NULL) {
        DListRemove(&devNode->entry);
    }
    if (devNode->super.UnlaunchNode != NULL) {
        devNode->super.UnlaunchNode(devNode);
    }

    return HDF_SUCCESS;
}

static struct HdfDeviceNode *HdfDeviceGetDeviceNode(struct IHdfDevice *device, devid_t devid)
{
    struct HdfDeviceNode *devNode = NULL;
    struct HdfDevice *dev = CONTAINER_OF(device, struct HdfDevice, super);
    DLIST_FOR_EACH_ENTRY(devNode, &dev->devNodes, struct HdfDeviceNode, entry) {
        if (devNode->devId == devid) {
            return devNode;
        };
    }
    return NULL;
}

static int HdfDeviceDetachWithDevid(struct IHdfDevice *device, devid_t devid)
{
    struct HdfDevice *dev = CONTAINER_OF(device, struct HdfDevice, super);
    struct HdfDeviceNode *devNode = HdfDeviceGetDeviceNode(device, devid);
    if (devNode == NULL) {
        HDF_LOGE("detach device node %x not in device %x", devid, dev->deviceId);
        return HDF_DEV_ERR_NO_DEVICE;
    }

    return HdfDeviceDetach(device, devNode);
}

void HdfDeviceConstruct(struct HdfDevice *device)
{
    device->super.Attach = HdfDeviceAttach;
    device->super.Detach = HdfDeviceDetach;
    device->super.DetachWithDevid = HdfDeviceDetachWithDevid;
    device->super.GetDeviceNode = HdfDeviceGetDeviceNode;

    DListHeadInit(&device->devNodes);
}

void HdfDeviceDestruct(struct HdfDevice *device)
{
    struct HdfDeviceNode *devNode = NULL;
    struct HdfDeviceNode *tmp = NULL;
    DLIST_FOR_EACH_ENTRY_SAFE(devNode, tmp, &device->devNodes, struct HdfDeviceNode, entry) {
        HdfDeviceNodeFreeInstance(devNode);
    }
    DListHeadInit(&device->devNodes);
}

struct HdfObject *HdfDeviceCreate(void)
{
    struct HdfDevice *device = (struct HdfDevice *)OsalMemCalloc(sizeof(struct HdfDevice));
    if (device != NULL) {
        HdfDeviceConstruct(device);
    }
    return (struct HdfObject *)device;
}

void HdfDeviceRelease(struct HdfObject *object)
{
    struct HdfDevice *device = (struct HdfDevice *)object;
    if (device != NULL) {
        HdfDeviceDestruct(device);
        OsalMemFree(device);
    }
}

struct HdfDevice *HdfDeviceNewInstance(void)
{
    return (struct HdfDevice *)HdfObjectManagerGetObject(HDF_OBJECT_ID_DEVICE);
}

void HdfDeviceFreeInstance(struct HdfDevice *device)
{
    if (device != NULL) {
        HdfObjectManagerFreeObject(&device->super.object);
    }
}
