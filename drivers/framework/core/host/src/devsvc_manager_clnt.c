/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "devsvc_manager_clnt.h"
#include "hdf_log.h"
#include "hdf_object_manager.h"

#define HDF_LOG_TAG devsvc_manager_clnt

int DevSvcManagerClntAddService(const char *svcName, uint16_t devClass,
    struct HdfDeviceObject *service, const char *servinfo)
{
    struct DevSvcManagerClnt *devSvcMgrClnt = DevSvcManagerClntGetInstance();
    struct IDevSvcManager *serviceManager = NULL;
    if (devSvcMgrClnt == NULL) {
        HDF_LOGE("failed to add service, client is null");
        return HDF_FAILURE;
    }
    if (devClass >= DEVICE_CLASS_MAX) {
        HDF_LOGE("failed to add service, invalid class");
        return HDF_FAILURE;
    }

    serviceManager = devSvcMgrClnt->devSvcMgrIf;
    if (serviceManager == NULL || serviceManager->AddService == NULL) {
        HDF_LOGE("serviceManager AddService function is null");
        return HDF_FAILURE;
    }
    return serviceManager->AddService(serviceManager, svcName, devClass, service, servinfo);
}

int DevSvcManagerClntUpdateService(const char *svcName, uint16_t devClass,
    struct HdfDeviceObject *service, const char *servinfo)
{
    struct DevSvcManagerClnt *devSvcMgrClnt = DevSvcManagerClntGetInstance();
    struct IDevSvcManager *serviceManager = NULL;
    if (devSvcMgrClnt == NULL) {
        HDF_LOGE("failed to add service, client is null");
        return HDF_FAILURE;
    }
    if (devClass >= DEVICE_CLASS_MAX) {
        HDF_LOGE("failed to add service, invalid class");
        return HDF_FAILURE;
    }

    serviceManager = devSvcMgrClnt->devSvcMgrIf;
    if (serviceManager == NULL || serviceManager->UpdateService == NULL) {
        HDF_LOGE("serviceManager UpdateService function is null");
        return HDF_FAILURE;
    }
    return serviceManager->UpdateService(serviceManager, svcName, devClass, service, servinfo);
}

const struct HdfObject *DevSvcManagerClntGetService(const char *svcName)
{
    struct DevSvcManagerClnt *devSvcMgrClnt = DevSvcManagerClntGetInstance();
    struct IDevSvcManager *serviceManager = NULL;
    if (devSvcMgrClnt == NULL) {
        HDF_LOGE("failed to get service, client is null");
        return NULL;
    }

    serviceManager = devSvcMgrClnt->devSvcMgrIf;
    if (serviceManager == NULL || serviceManager->GetService == NULL) {
        HDF_LOGE("serviceManager GetService function is null");
        return NULL;
    }
    return serviceManager->GetService(serviceManager, svcName);
}

struct HdfDeviceObject *DevSvcManagerClntGetDeviceObject(const char *svcName)
{
    struct DevSvcManagerClnt *devSvcMgrClnt = DevSvcManagerClntGetInstance();
    struct IDevSvcManager *serviceManager = NULL;
    if (devSvcMgrClnt == NULL) {
        HDF_LOGE("failed to get device object, client is null");
        return NULL;
    }

    serviceManager = devSvcMgrClnt->devSvcMgrIf;
    if (serviceManager == NULL || serviceManager->GetObject == NULL) {
        HDF_LOGE("failed to get device object, method not implement");
        return NULL;
    }
    return serviceManager->GetObject(serviceManager, svcName);
}

int DevSvcManagerClntSubscribeService(const char *svcName, struct SubscriberCallback callback)
{
    struct DevSvcManagerClnt *devSvcMgrClnt = DevSvcManagerClntGetInstance();
    struct IDevSvcManager *serviceManager = NULL;
    if (devSvcMgrClnt == NULL) {
        HDF_LOGE("failed to subscribe service, client is null");
        return HDF_FAILURE;
    }

    serviceManager = devSvcMgrClnt->devSvcMgrIf;
    if (serviceManager == NULL || serviceManager->SubscribeService == NULL) {
        HDF_LOGE("failed to subscribe service, method not implement");
        return HDF_FAILURE;
    }
    return serviceManager->SubscribeService(serviceManager, svcName, callback);
}

int DevSvcManagerClntUnsubscribeService(const char *svcName)
{
    struct DevSvcManagerClnt *devSvcMgrClnt = DevSvcManagerClntGetInstance();
    struct IDevSvcManager *serviceManager = NULL;
    if (devSvcMgrClnt == NULL) {
        HDF_LOGE("failed to unsubscribe service, client is null");
        return HDF_FAILURE;
    }

    serviceManager = devSvcMgrClnt->devSvcMgrIf;
    if (serviceManager == NULL || serviceManager->UnsubscribeService == NULL) {
        HDF_LOGE("failed to unsubscribe service, method not implement");
        return HDF_FAILURE;
    }
    return serviceManager->UnsubscribeService(serviceManager, svcName);
}

void DevSvcManagerClntRemoveService(const char *svcName)
{
    struct DevSvcManagerClnt *devSvcMgrClnt = DevSvcManagerClntGetInstance();
    struct IDevSvcManager *serviceManager = NULL;
    if (devSvcMgrClnt == NULL) {
        HDF_LOGE("failed to remove service, devSvcMgrClnt is null");
        return;
    }

    serviceManager = devSvcMgrClnt->devSvcMgrIf;
    if (serviceManager == NULL || serviceManager->RemoveService == NULL) {
        return;
    }
    serviceManager->RemoveService(serviceManager, svcName);
}

static void DevSvcManagerClntConstruct(struct DevSvcManagerClnt *inst)
{
    inst->devSvcMgrIf = (struct IDevSvcManager *)HdfObjectManagerGetObject(HDF_OBJECT_ID_DEVSVC_MANAGER);
}

struct DevSvcManagerClnt *DevSvcManagerClntGetInstance()
{
    static struct DevSvcManagerClnt *instance = NULL;
    if (instance == NULL) {
        static struct DevSvcManagerClnt singletonInstance;
        DevSvcManagerClntConstruct(&singletonInstance);
        instance = &singletonInstance;
    }
    return instance;
}

void DevSvcManagerClntFreeInstance(struct DevSvcManagerClnt *instance)
{
    if (instance != NULL) {
        HdfObjectManagerFreeObject((struct HdfObject *)instance->devSvcMgrIf);
    }
}

