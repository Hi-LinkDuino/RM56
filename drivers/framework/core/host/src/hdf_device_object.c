/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_device_object.h"
#include "devhost_service.h"
#include "devsvc_manager_clnt.h"
#include "hdf_base.h"
#include "hdf_cstring.h"
#include "hdf_device_node.h"
#include "hdf_driver_loader.h"
#include "hdf_log.h"
#include "hdf_object_manager.h"
#include "hdf_observer_record.h"
#include "hdf_power_manager.h"
#include "hdf_service_observer.h"
#include "osal_mem.h"
#include "power_state_token.h"

#define HDF_LOG_TAG device_object

#define SERVICE_INFO_LEN_MAX 128

int32_t HdfDeviceSubscribeService(
    struct HdfDeviceObject *deviceObject, const char *serviceName, struct SubscriberCallback callback)
{
    struct DevHostService *hostService = NULL;
    struct HdfDeviceNode *devNode = NULL;
    if (deviceObject == NULL || serviceName == NULL) {
        HDF_LOGE("failed to subscribe service, serviceName is null");
        return HDF_FAILURE;
    }
    devNode = (struct HdfDeviceNode *)HDF_SLIST_CONTAINER_OF(
        struct HdfDeviceObject, deviceObject, struct HdfDeviceNode, deviceObject);
    hostService = devNode->hostService;
    if (hostService == NULL) {
        HDF_LOGE("failed to subscribe service, hostService is null");
        return HDF_FAILURE;
    }

    return HdfServiceObserverSubscribeService(&hostService->observer, serviceName, devNode->devId, callback);
}

const char *HdfDeviceGetServiceName(const struct HdfDeviceObject *deviceObject)
{
    struct HdfDeviceNode *devNode = NULL;
    if (deviceObject == NULL) {
        HDF_LOGE("failed to get service name, deviceObject is invalid");
        return NULL;
    }
    devNode = (struct HdfDeviceNode *)HDF_SLIST_CONTAINER_OF(
        struct HdfDeviceObject, deviceObject, struct HdfDeviceNode, deviceObject);
    return devNode->servName;
}

int HdfPmRegisterPowerListener(struct HdfDeviceObject *deviceObject, const struct IPowerEventListener *listener)
{
    struct HdfDeviceNode *devNode = NULL;
    if (deviceObject == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    devNode = (struct HdfDeviceNode *)HDF_SLIST_CONTAINER_OF(
        struct HdfDeviceObject, deviceObject, struct HdfDeviceNode, deviceObject);
    return HdfDeviceNodeAddPowerStateListener(devNode, listener);
}

void HdfPmUnregisterPowerListener(struct HdfDeviceObject *deviceObject, const struct IPowerEventListener *listener)
{
    struct HdfDeviceNode *devNode = NULL;
    if (deviceObject == NULL) {
        return;
    }
    devNode = (struct HdfDeviceNode *)HDF_SLIST_CONTAINER_OF(
        struct HdfDeviceObject, deviceObject, struct HdfDeviceNode, deviceObject);
    HdfDeviceNodeRemovePowerStateListener(devNode, listener);
}

void HdfPmAcquireDevice(struct HdfDeviceObject *deviceObject)
{
    struct HdfDeviceNode *devNode = NULL;
    struct IPowerStateToken *tokenIf = NULL;
    if (deviceObject == NULL) {
        HDF_LOGE("%s: input param is invalid", __func__);
        return;
    }
    devNode = (struct HdfDeviceNode *)HDF_SLIST_CONTAINER_OF(
        struct HdfDeviceObject, deviceObject, struct HdfDeviceNode, deviceObject);
    tokenIf = (struct IPowerStateToken *)devNode->powerToken;
    if ((tokenIf != NULL) && (tokenIf->AcquireWakeLock != NULL)) {
        tokenIf->AcquireWakeLock(tokenIf);
    }
}

void HdfPmReleaseDevice(struct HdfDeviceObject *deviceObject)
{
    struct HdfDeviceNode *devNode = NULL;
    struct IPowerStateToken *tokenIf = NULL;
    if (deviceObject == NULL) {
        HDF_LOGE("%s: input param is invalid", __func__);
        return;
    }
    devNode = (struct HdfDeviceNode *)HDF_SLIST_CONTAINER_OF(
        struct HdfDeviceObject, deviceObject, struct HdfDeviceNode, deviceObject);
    tokenIf = (struct IPowerStateToken *)devNode->powerToken;
    if ((tokenIf != NULL) && (tokenIf->ReleaseWakeLock != NULL)) {
        tokenIf->ReleaseWakeLock(tokenIf);
    }
}

void HdfPmAcquireDeviceAsync(struct HdfDeviceObject *deviceObject)
{
    struct HdfDeviceNode *devNode = NULL;

    if (deviceObject == NULL) {
        HDF_LOGE("%s: input param is invalid", __func__);
        return;
    }

    devNode = (struct HdfDeviceNode *)HDF_SLIST_CONTAINER_OF(
        struct HdfDeviceObject, deviceObject, struct HdfDeviceNode, deviceObject);
    HdfPmTaskPut(devNode->powerToken, HDF_PM_REQUEST_ACQUIRE);
}

void HdfPmReleaseDeviceAsync(struct HdfDeviceObject *deviceObject)
{
    struct HdfDeviceNode *devNode = NULL;

    if (deviceObject == NULL) {
        HDF_LOGE("%s: input param is invalid", __func__);
        return;
    }

    devNode = (struct HdfDeviceNode *)HDF_SLIST_CONTAINER_OF(
        struct HdfDeviceObject, deviceObject, struct HdfDeviceNode, deviceObject);
    HdfPmTaskPut(devNode->powerToken, HDF_PM_REQUEST_RELEASE);
}

void HdfPmSetMode(struct HdfDeviceObject *deviceObject, uint32_t mode)
{
    struct HdfDeviceNode *devNode = NULL;
    struct PowerStateToken *token = NULL;
    if (deviceObject == NULL || mode > HDF_POWER_MODE_MAX) {
        HDF_LOGE("%s: input param is invalid", __func__);
        return;
    }
    devNode = (struct HdfDeviceNode *)HDF_SLIST_CONTAINER_OF(
        struct HdfDeviceObject, deviceObject, struct HdfDeviceNode, deviceObject);
    token = devNode->powerToken;
    if (token != NULL) {
        token->mode = mode;
    }
}

bool HdfDeviceSetClass(struct HdfDeviceObject *deviceObject, DeviceClass deviceClass)
{
    if ((deviceObject == NULL) || (deviceClass >= DEVICE_CLASS_MAX)) {
        return false;
    }
    deviceObject->deviceClass = deviceClass;
    return true;
}

void HdfDeviceObjectConstruct(struct HdfDeviceObject *deviceObject)
{
    if (deviceObject != NULL) {
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
        deviceObject->deviceMatchAttr = NULL;
#else
        deviceObject->property = NULL;
#endif
        deviceObject->service = NULL;
        deviceObject->deviceClass = DEVICE_CLASS_DEFAULT;
    }
}

struct HdfDeviceObject *HdfDeviceObjectAlloc(struct HdfDeviceObject *parent, const char *driverName)
{
    struct HdfDeviceNode *newNode = NULL;
    struct HdfDeviceNode *parentDevNode = CONTAINER_OF(parent, struct HdfDeviceNode, deviceObject);

    if (parent == NULL) {
        HDF_LOGE("failed to alloc device, parent invalid");
        return NULL;
    }

    if (parentDevNode->devStatus != DEVNODE_LAUNCHED) {
        HDF_LOGE("failed to alloc device, parent status invalid %u", parentDevNode->devStatus);
        return NULL;
    }

    newNode = (struct HdfDeviceNode *)HdfObjectManagerGetObject(HDF_OBJECT_ID_DEVICE_SERVICE);
    if (newNode == NULL) {
        return NULL;
    }
    newNode->driverName = HdfStringCopy(driverName);
    if (newNode->driverName == NULL) {
        HdfDeviceNodeFreeInstance(newNode);
        return NULL;
    }

    newNode->hostService = parentDevNode->hostService;
    newNode->device = parentDevNode->device;

    return &newNode->deviceObject;
}

void HdfDeviceObjectRelease(struct HdfDeviceObject *dev)
{
    struct HdfDeviceNode *devNode = CONTAINER_OF(dev, struct HdfDeviceNode, deviceObject);
    if (dev == NULL) {
        return;
    }

    if (devNode->device != NULL && devNode->device->super.Detach != NULL) {
        devNode->device->super.Detach(&devNode->device->super, devNode);
    }
    HdfDeviceNodeFreeInstance(devNode);
}

int HdfDeviceObjectRegister(struct HdfDeviceObject *dev)
{
    int ret = HDF_FAILURE;
    struct HdfDeviceNode *devNode = CONTAINER_OF(dev, struct HdfDeviceNode, deviceObject);
    struct IDriverLoader *driverLoader = HdfDriverLoaderGetInstance();

    if (dev == NULL || devNode->driverName == NULL || devNode->device == NULL || driverLoader == NULL ||
        driverLoader->GetDriver == NULL) {
        HDF_LOGE("failed to add device, param invalid");
        return HDF_ERR_INVALID_PARAM;
    }

    devNode->driver = driverLoader->GetDriver(devNode->driverName);
    if (devNode->driver == NULL) {
        HDF_LOGE("can not found driver %s", devNode->driverName);
        return HDF_DEV_ERR_NO_DEVICE;
    }

    ret = devNode->device->super.Attach(&devNode->device->super, devNode);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("failed to attach device %s", devNode->driverName);
        return HDF_DEV_ERR_ATTACHDEV_FAIL;
    }

    return ret;
}

int HdfDeviceObjectUnRegister(struct HdfDeviceObject *dev)
{
    struct HdfDeviceNode *devNode = CONTAINER_OF(dev, struct HdfDeviceNode, deviceObject);
    if (devNode == NULL || devNode->device == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    return devNode->device->super.Detach(&devNode->device->super, devNode);
}

int HdfDeviceObjectPublishService(struct HdfDeviceObject *dev, const char *servName, uint8_t policy, uint32_t perm)
{
    int ret;
    struct HdfDeviceNode *devNode = CONTAINER_OF(dev, struct HdfDeviceNode, deviceObject);
    if (dev == NULL || servName == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (policy <= SERVICE_POLICY_NONE || policy >= SERVICE_POLICY_INVALID) {
        return HDF_DEV_ERR_NO_DEVICE_SERVICE;
    }

    if (devNode->servStatus) {
        HDF_LOGE("failed to publish public service, repeat publish");
        return HDF_FAILURE;
    }

    devNode->servName = HdfStringCopy(servName);
    if (devNode->servName == NULL) {
        return HDF_DEV_ERR_NO_MEMORY;
    }

    devNode->policy = policy;
    devNode->permission = perm;

    ret = DeviveDriverBind(devNode);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    return devNode->super.PublishService(devNode);
}

int HdfDeviceObjectRemoveService(struct HdfDeviceObject *dev)
{
    struct HdfDeviceNode *devNode = CONTAINER_OF(dev, struct HdfDeviceNode, deviceObject);
    if (dev == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    return devNode->super.RemoveService(devNode);
}
int HdfDeviceObjectSetServInfo(struct HdfDeviceObject *dev, const char *info)
{
    if (dev == NULL || info == NULL || strlen(info) > SERVICE_INFO_LEN_MAX) {
        return HDF_ERR_INVALID_PARAM;
    }

    struct HdfDeviceNode *devNode = CONTAINER_OF(dev, struct HdfDeviceNode, deviceObject);
    if (devNode->servInfo != NULL) {
        OsalMemFree((char *)devNode->servInfo);
    }
    devNode->servInfo = HdfStringCopy(info);
    if (devNode->servInfo == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }
    return HDF_SUCCESS;
}

int HdfDeviceObjectUpdate(struct HdfDeviceObject *dev)
{
    struct HdfDeviceNode *devNode = CONTAINER_OF(dev, struct HdfDeviceNode, deviceObject);
    if (dev == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    return DevSvcManagerClntUpdateService(
        devNode->servName, devNode->deviceObject.deviceClass, &devNode->deviceObject, devNode->servInfo);
}

int HdfDeviceObjectSetInterfaceDesc(struct HdfDeviceObject *dev, const char *interfaceDesc)
{
    struct HdfDeviceNode *devNode = CONTAINER_OF(dev, struct HdfDeviceNode, deviceObject);
    if (dev == NULL || interfaceDesc == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    devNode->interfaceDesc = HdfStringCopy(interfaceDesc);
    return devNode->interfaceDesc != NULL ? HDF_SUCCESS : HDF_ERR_MALLOC_FAIL;
}

bool __attribute__((weak)) HdfDeviceObjectCheckInterfaceDesc(struct HdfDeviceObject *dev, struct HdfSBuf *data)
{
    (void)dev;
    (void)data;
    return true;
}