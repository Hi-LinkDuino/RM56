/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_device_node.h"
#include "devhost_service.h"
#include "devmgr_service_clnt.h"
#include "devsvc_manager_clnt.h"
#include "hcs_tree_if.h"
#include "hdf_attribute_manager.h"
#include "hdf_base.h"
#include "hdf_cstring.h"
#include "hdf_device_object.h"
#include "hdf_device_token.h"
#include "hdf_driver_loader.h"
#include "hdf_log.h"
#include "hdf_object_manager.h"
#include "hdf_observer_record.h"
#include "osal_mem.h"
#include "power_state_token.h"

#define HDF_LOG_TAG device_node

static int HdfDeviceNodePublishLocalService(struct HdfDeviceNode *devNode)
{
    if (devNode == NULL) {
        HDF_LOGE("failed to publish local service, device is null");
        return HDF_FAILURE;
    }
    if (devNode->hostService == NULL) {
        HDF_LOGE("failed to publish local service, host service is null");
        return HDF_FAILURE;
    }
    return HdfServiceObserverPublishService(&(devNode->hostService->observer), devNode->servName,
        devNode->devId, devNode->policy, (struct HdfObject *)devNode->deviceObject.service);
}

static int HdfDeviceNodePublishService(struct HdfDeviceNode *devNode)
{
    int status = HDF_SUCCESS;
    struct IDeviceNode *nodeIf = NULL;
    if (devNode->policy == SERVICE_POLICY_NONE ||
        (devNode->servName != NULL && strlen(devNode->servName) == 0)) {
        return status;
    }

    nodeIf = &devNode->super;
    if (devNode->policy == SERVICE_POLICY_PUBLIC || devNode->policy == SERVICE_POLICY_CAPACITY) {
        if (nodeIf->PublishService != NULL) {
            status = nodeIf->PublishService(devNode);
        }
    }
    if (status == HDF_SUCCESS) {
        status = HdfDeviceNodePublishLocalService(devNode);
    }
    return status;
}

int DeviveDriverBind(struct HdfDeviceNode *devNode)
{
    int ret;
    const struct HdfDriverEntry *driverEntry = NULL;
    if (devNode == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    driverEntry = devNode->driver->entry;
    if (devNode->policy == SERVICE_POLICY_PUBLIC || devNode->policy == SERVICE_POLICY_CAPACITY) {
        if (driverEntry->Bind == NULL) {
            HDF_LOGE("driver %s bind method not implement", driverEntry->moduleName);
            devNode->devStatus = DEVNODE_NONE;
            return HDF_ERR_INVALID_OBJECT;
        }
        ret = driverEntry->Bind(&devNode->deviceObject);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("bind driver %s failed", driverEntry->moduleName);
            return HDF_DEV_ERR_DEV_INIT_FAIL;
        }
    }

    return HDF_SUCCESS;
}

int HdfDeviceLaunchNode(struct HdfDeviceNode *devNode)
{
    const struct HdfDriverEntry *driverEntry = NULL;
    int ret;
    if (devNode == NULL) {
        HDF_LOGE("failed to launch service, device or service is null");
        return HDF_ERR_INVALID_PARAM;
    }

    HDF_LOGI("launch devnode %s", devNode->servName ? devNode->servName : "");
    driverEntry = devNode->driver->entry;
    if (driverEntry == NULL || driverEntry->Init == NULL) {
        HDF_LOGE("failed to launch service, deviceEntry invalid");
        return HDF_ERR_INVALID_PARAM;
    }
    devNode->devStatus = DEVNODE_LAUNCHED;

    ret = DeviveDriverBind(devNode);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    ret = driverEntry->Init(&devNode->deviceObject);
    if (ret != HDF_SUCCESS) {
        return HDF_DEV_ERR_DEV_INIT_FAIL;
    }

    ret = HdfDeviceNodePublishService(devNode);
    if (ret != HDF_SUCCESS) {
        return HDF_DEV_ERR_PUBLISH_FAIL;
    }

    ret = DevmgrServiceClntAttachDevice(devNode->token);
    if (ret != HDF_SUCCESS) {
        return HDF_DEV_ERR_ATTACHDEV_FAIL;
    }
    return ret;
}

int HdfDeviceNodeAddPowerStateListener(
    struct HdfDeviceNode *devNode, const struct IPowerEventListener *listener)
{
    if (devNode->powerToken != NULL) {
        return HDF_FAILURE;
    }

    devNode->powerToken = PowerStateTokenNewInstance(&devNode->deviceObject, listener);
    return (devNode->powerToken != NULL) ? HDF_SUCCESS : HDF_FAILURE;
}

void HdfDeviceNodeRemovePowerStateListener(
    struct HdfDeviceNode *devNode, const struct IPowerEventListener *listener)
{
    (void)listener;
    if (devNode == NULL || devNode->powerToken == NULL) {
        return;
    }

    PowerStateTokenFreeInstance(devNode->powerToken);
    devNode->powerToken = NULL;
}

int HdfDeviceNodePublishPublicService(struct HdfDeviceNode *devNode)
{
    int ret;
    if (devNode == NULL || devNode->deviceObject.service == NULL) {
        HDF_LOGE("failed to publish public service, devNode is NULL");
        return HDF_FAILURE;
    }

    ret = DevSvcManagerClntAddService(devNode->servName,
        devNode->deviceObject.deviceClass, &devNode->deviceObject, devNode->servInfo);
    if (ret == HDF_SUCCESS) {
        devNode->servStatus = true;
    }

    return ret;
}

int HdfDeviceNodeRemoveService(struct HdfDeviceNode *devNode)
{
    if (devNode != NULL && devNode->servStatus) {
        DevSvcManagerClntRemoveService(devNode->servName);
        devNode->servStatus = false;
    }

    return HDF_SUCCESS;
}

static void HdfDeviceUnlaunchNode(struct HdfDeviceNode *devNode)
{
    const struct HdfDriverEntry *driverEntry = NULL;
    struct IDriverLoader *driverLoader = NULL;
    if (devNode == NULL || devNode->devStatus != DEVNODE_LAUNCHED) {
        return;
    }

    if (devNode->driver != NULL) {
        driverEntry = devNode->driver->entry;
    }

    if (driverEntry != NULL && driverEntry->Release != NULL) {
        driverEntry->Release(&devNode->deviceObject);
    }

    if (devNode->servStatus) {
        devNode->super.RemoveService(devNode);
    }
    DevmgrServiceClntDetachDevice(devNode->devId);

    // release driver object or close driver library
    driverLoader = HdfDriverLoaderGetInstance();
    if (driverLoader != NULL) {
        driverLoader->ReclaimDriver(devNode->driver);
        devNode->driver = NULL;
    } else {
        HDF_LOGE("failed to get driver loader");
    }
    devNode->devStatus = DEVNODE_INITED;
}

void HdfDeviceNodeConstruct(struct HdfDeviceNode *devNode)
{
    if (devNode != NULL) {
        struct IDeviceNode *nodeIf = &devNode->super;
        HdfDeviceObjectConstruct(&devNode->deviceObject);
        devNode->token = HdfDeviceTokenNewInstance();
        nodeIf->LaunchNode = HdfDeviceLaunchNode;
        nodeIf->PublishService = HdfDeviceNodePublishPublicService;
        nodeIf->RemoveService = HdfDeviceNodeRemoveService;
        nodeIf->UnlaunchNode = HdfDeviceUnlaunchNode;
    }
}

void HdfDeviceNodeDestruct(struct HdfDeviceNode *devNode)
{
    if (devNode == NULL) {
        return;
    }
    HDF_LOGI("release devnode %s", devNode->servName);
    switch (devNode->devStatus) {
        case DEVNODE_LAUNCHED: // fall-through
            HdfDeviceUnlaunchNode(devNode);
        case DEVNODE_INITED:
            HdfDeviceTokenFreeInstance(devNode->token);
            devNode->token = NULL;
            PowerStateTokenFreeInstance(devNode->powerToken);
            devNode->powerToken = NULL;
            OsalMemFree(devNode->servName);
            OsalMemFree((char *)devNode->servInfo);
            OsalMemFree(devNode->driverName);
            devNode->servName = NULL;
            devNode->servInfo = NULL;
            break;
        case DEVNODE_NONE:
            break;
        default:
            break;
    }
}

struct HdfDeviceNode *HdfDeviceNodeNewInstance(const struct HdfDeviceInfo *deviceInfo, struct HdfDriver *driver)
{
    struct HdfDeviceNode *devNode = NULL;
    if (deviceInfo == NULL) {
        return NULL;
    }
    devNode = (struct HdfDeviceNode *)HdfObjectManagerGetObject(HDF_OBJECT_ID_DEVICE_SERVICE);
    if (devNode == NULL) {
        return NULL;
    }

    devNode->driver = driver;
    devNode->devId = deviceInfo->deviceId;
    devNode->permission = deviceInfo->permission;
    devNode->policy = deviceInfo->policy;
    devNode->token->devid = deviceInfo->deviceId;
    devNode->servName = HdfStringCopy(deviceInfo->svcName);
    if (devNode->servName == NULL) {
        HdfDeviceNodeFreeInstance(devNode);
        return NULL;
    }
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
    devNode->deviceObject.deviceMatchAttr = deviceInfo->deviceMatchAttr;
#else
    devNode->deviceObject.property = HcsGetNodeByMatchAttr(HdfGetHcsRootNode(), deviceInfo->deviceMatchAttr);
    if (devNode->deviceObject.property == NULL) {
        HDF_LOGD("node %s property empty, match attr: %s", deviceInfo->moduleName, deviceInfo->deviceMatchAttr);
    }
#endif

    devNode->devStatus = DEVNODE_INITED;
    return devNode;
}

void HdfDeviceNodeFreeInstance(struct HdfDeviceNode *devNode)
{
    HdfObjectManagerFreeObject((struct HdfObject *)devNode);
}
