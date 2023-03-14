/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "devsvc_manager.h"
#include "devmgr_service.h"
#include "hdf_base.h"
#include "hdf_cstring.h"
#include "hdf_log.h"
#include "hdf_object_manager.h"
#include "hdf_service_record.h"
#include "hdf_device_node.h"
#include "osal_mem.h"

#define HDF_LOG_TAG devsvc_manager

static struct DevSvcRecord *DevSvcManagerSearchService(struct IDevSvcManager *inst, uint32_t serviceKey)
{
    struct DevSvcRecord *record = NULL;
    struct DevSvcRecord *searchResult = NULL;
    struct DevSvcManager *devSvcManager = (struct DevSvcManager *)inst;
    if (devSvcManager == NULL) {
        HDF_LOGE("failed to search service, devSvcManager is null");
        return NULL;
    }

    OsalMutexLock(&devSvcManager->mutex);
    DLIST_FOR_EACH_ENTRY(record, &devSvcManager->services, struct DevSvcRecord, entry) {
        if (record->key == serviceKey) {
            searchResult = record;
            break;
        }
    }
    OsalMutexUnlock(&devSvcManager->mutex);
    return searchResult;
}

static void NotifyServiceStatusLocked(struct DevSvcManager *devSvcManager,
    struct DevSvcRecord *record, uint32_t status)
{
    struct ServStatListenerHolder *holder = NULL;
    struct ServStatListenerHolder *tmp = NULL;
    struct ServiceStatus svcstat = {
        .deviceClass = record->devClass,
        .serviceName = record->servName,
        .status = status,
        .info = record->servInfo,
    };
    DLIST_FOR_EACH_ENTRY_SAFE(holder, tmp, &devSvcManager->svcstatListeners, struct ServStatListenerHolder, node) {
        if ((holder->listenClass & record->devClass) && holder->NotifyStatus != NULL) {
            if (holder->NotifyStatus(holder, &svcstat) == HDF_FAILURE) {
                DListRemove(&holder->node);
                if (holder->Recycle != NULL) {
                    holder->Recycle(holder);
                }
            }
        }
    }
}

static void NotifyServiceStatusOnRegisterLocked(struct DevSvcManager *devSvcManager,
    struct ServStatListenerHolder *listenerHolder)
{
    struct DevSvcRecord *record = NULL;
    DLIST_FOR_EACH_ENTRY(record, &devSvcManager->services, struct DevSvcRecord, entry) {
        if ((listenerHolder->listenClass & record->devClass) == 0) {
            continue;
        }
        struct ServiceStatus svcstat = {
            .deviceClass = record->devClass,
            .serviceName = record->servName,
            .status = SERVIE_STATUS_START,
            .info = record->servInfo,
        };
        if (listenerHolder->NotifyStatus != NULL) {
            listenerHolder->NotifyStatus(listenerHolder, &svcstat);
        }
    }
}

int DevSvcManagerAddService(struct IDevSvcManager *inst, const char *servName,
    uint16_t devClass, struct HdfDeviceObject *service, const char *servInfo)
{
    struct DevSvcManager *devSvcManager = (struct DevSvcManager *)inst;
    struct DevSvcRecord *record = NULL;
    if (devSvcManager == NULL || service == NULL || servName == NULL) {
        HDF_LOGE("failed to add service, input param is null");
        return HDF_FAILURE;
    }
    record = DevSvcManagerSearchService(inst, HdfStringMakeHashKey(servName, 0));
    if (record != NULL) {
        HDF_LOGI("%s:add service %s exist, only update value", __func__, servName);
        // on service died will release old service object
        record->value = service;
        return HDF_SUCCESS;
    }
    record = DevSvcRecordNewInstance();
    if (record == NULL) {
        HDF_LOGE("failed to add service , record is null");
        return HDF_FAILURE;
    }

    record->key = HdfStringMakeHashKey(servName, 0);
    record->value = service;
    record->devClass = devClass;
    record->servName = HdfStringCopy(servName);
    record->servInfo = HdfStringCopy(servInfo);
    if (record->servName == NULL) {
        DevSvcRecordFreeInstance(record);
        return HDF_ERR_MALLOC_FAIL;
    }
    OsalMutexLock(&devSvcManager->mutex);
    DListInsertTail(&record->entry, &devSvcManager->services);
    NotifyServiceStatusLocked(devSvcManager, record, SERVIE_STATUS_START);
    OsalMutexUnlock(&devSvcManager->mutex);
    return HDF_SUCCESS;
}

int DevSvcManagerUpdateService(struct IDevSvcManager *inst, const char *servName,
    uint16_t devClass, struct HdfDeviceObject *service, const char *servInfo)
{
    struct DevSvcManager *devSvcManager = (struct DevSvcManager *)inst;
    struct DevSvcRecord *record = NULL;
    char *servInfoStr = NULL;
    if (devSvcManager == NULL || service == NULL || servName == NULL) {
        HDF_LOGE("failed to update service, invalid param");
        return HDF_FAILURE;
    }

    record = DevSvcManagerSearchService(inst, HdfStringMakeHashKey(servName, 0));
    if (record == NULL) {
        return HDF_DEV_ERR_NO_DEVICE;
    }

    if (servInfo != NULL) {
        servInfoStr = HdfStringCopy(servInfo);
        if (servInfoStr == NULL) {
            return HDF_ERR_MALLOC_FAIL;
        }
        OsalMemFree((char *)record->servInfo);
        record->servInfo = servInfoStr;
    }

    record->value = service;
    record->devClass = devClass;
    OsalMutexLock(&devSvcManager->mutex);
    NotifyServiceStatusLocked(devSvcManager, record, SERVIE_STATUS_CHANGE);
    OsalMutexUnlock(&devSvcManager->mutex);
    return HDF_SUCCESS;
}

int DevSvcManagerSubscribeService(struct IDevSvcManager *inst, const char *svcName, struct SubscriberCallback callBack)
{
    struct DevmgrService *devMgrSvc = (struct DevmgrService *)DevmgrServiceGetInstance();
    struct HdfObject *deviceService = NULL;
    if (inst == NULL || svcName == NULL || devMgrSvc == NULL) {
        return HDF_FAILURE;
    }

    deviceService = DevSvcManagerGetService(inst, svcName);
    if (deviceService != NULL) {
        if (callBack.OnServiceConnected != NULL) {
            callBack.OnServiceConnected(callBack.deviceObject, deviceService);
        }
        return HDF_SUCCESS;
    }

    return devMgrSvc->super.LoadDevice(&devMgrSvc->super, svcName);
}

void DevSvcManagerRemoveService(struct IDevSvcManager *inst, const char *svcName)
{
    struct DevSvcManager *devSvcManager = (struct DevSvcManager *)inst;
    struct DevSvcRecord *serviceRecord = NULL;
    uint32_t serviceKey = HdfStringMakeHashKey(svcName, 0);

    if (svcName == NULL || devSvcManager == NULL) {
        return;
    }
    serviceRecord = DevSvcManagerSearchService(inst, serviceKey);
    if (serviceRecord == NULL) {
        return;
    }
    OsalMutexLock(&devSvcManager->mutex);
    NotifyServiceStatusLocked(devSvcManager, serviceRecord, SERVIE_STATUS_STOP);
    DListRemove(&serviceRecord->entry);
    OsalMutexUnlock(&devSvcManager->mutex);

    DevSvcRecordFreeInstance(serviceRecord);
}

struct HdfDeviceObject *DevSvcManagerGetObject(struct IDevSvcManager *inst, const char *svcName)
{
    uint32_t serviceKey = HdfStringMakeHashKey(svcName, 0);
    struct DevSvcRecord *serviceRecord = NULL;
    if (svcName == NULL) {
        HDF_LOGE("Get service failed, svcName is null");
        return NULL;
    }
    serviceRecord = DevSvcManagerSearchService(inst, serviceKey);
    if (serviceRecord != NULL) {
        return serviceRecord->value;
    }
    return NULL;
}

// only use for kernel space
void DevSvcManagerListService(struct HdfSBuf *serviecNameSet, DeviceClass deviceClass)
{
    struct DevSvcRecord *record = NULL;
    struct DevSvcManager *devSvcManager = (struct DevSvcManager *)DevSvcManagerGetInstance();
    if (devSvcManager == NULL) {
        HDF_LOGE("failed to list service, devSvcManager is null");
        return;
    }

    OsalMutexLock(&devSvcManager->mutex);
    DLIST_FOR_EACH_ENTRY(record, &devSvcManager->services, struct DevSvcRecord, entry) {
        if (record->devClass == deviceClass) {
            HdfSbufWriteString(serviecNameSet, record->servName);
        }
    }
    OsalMutexUnlock(&devSvcManager->mutex);
}

struct HdfObject *DevSvcManagerGetService(struct IDevSvcManager *inst, const char *svcName)
{
    struct HdfDeviceObject *deviceObject = DevSvcManagerGetObject(inst, svcName);
    if (deviceObject == NULL) {
        return NULL;
    }
    return (struct HdfObject *)deviceObject->service;
}

int DevSvcManagerRegsterServListener(struct IDevSvcManager *inst,
    struct ServStatListenerHolder *listenerHolder)
{
    struct DevSvcManager *devSvcManager = (struct DevSvcManager *)inst;
    if (devSvcManager == NULL || listenerHolder == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    OsalMutexLock(&devSvcManager->mutex);
    DListInsertTail(&listenerHolder->node, &devSvcManager->svcstatListeners);
    NotifyServiceStatusOnRegisterLocked(devSvcManager, listenerHolder);
    OsalMutexUnlock(&devSvcManager->mutex);

    return HDF_SUCCESS;
}

void DevSvcManagerUnregsterServListener(struct IDevSvcManager *inst,
    struct ServStatListenerHolder *listenerHolder)
{
    struct DevSvcManager *devSvcManager = (struct DevSvcManager *)inst;
    if (devSvcManager == NULL || listenerHolder == NULL) {
        return;
    }

    OsalMutexLock(&devSvcManager->mutex);
    DListRemove(&listenerHolder->node);
    OsalMutexUnlock(&devSvcManager->mutex);
}

bool DevSvcManagerConstruct(struct DevSvcManager *inst)
{
    struct IDevSvcManager *devSvcMgrIf = NULL;
    if (inst == NULL) {
        HDF_LOGE("%s: inst is null!", __func__);
        return false;
    }
    devSvcMgrIf = &inst->super;
    devSvcMgrIf->AddService = DevSvcManagerAddService;
    devSvcMgrIf->UpdateService = DevSvcManagerUpdateService;
    devSvcMgrIf->SubscribeService = DevSvcManagerSubscribeService;
    devSvcMgrIf->UnsubscribeService = NULL;
    devSvcMgrIf->RemoveService = DevSvcManagerRemoveService;
    devSvcMgrIf->GetService = DevSvcManagerGetService;
    devSvcMgrIf->GetObject = DevSvcManagerGetObject;
    devSvcMgrIf->RegsterServListener = DevSvcManagerRegsterServListener;
    devSvcMgrIf->UnregsterServListener = DevSvcManagerUnregsterServListener;
    if (OsalMutexInit(&inst->mutex) != HDF_SUCCESS) {
        HDF_LOGE("failed to create device service manager mutex");
        return false;
    }
    DListHeadInit(&inst->services);
    DListHeadInit(&inst->svcstatListeners);
    return true;
}

int DevSvcManagerStartService(void)
{
    int ret;
    struct IDevSvcManager *svcmgr = DevSvcManagerGetInstance();

    if (svcmgr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (svcmgr->StartService == NULL) {
        return HDF_SUCCESS;
    }

    ret = svcmgr->StartService(svcmgr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("failed to start service manager");
    }

    return ret;
}

struct HdfObject *DevSvcManagerCreate(void)
{
    static bool isDevSvcManagerInit = false;
    static struct DevSvcManager devSvcManagerInstance;
    if (!isDevSvcManagerInit) {
        if (!DevSvcManagerConstruct(&devSvcManagerInstance)) {
            return NULL;
        }
        isDevSvcManagerInit = true;
    }
    return (struct HdfObject *)&devSvcManagerInstance;
}

void DevSvcManagerRelease(struct IDevSvcManager *inst)
{
    struct DevSvcManager *devSvcManager = CONTAINER_OF(inst, struct DevSvcManager, super);
    if (inst == NULL) {
        return;
    }
    struct DevSvcRecord *record = NULL;
    struct DevSvcRecord *tmp = NULL;
    DLIST_FOR_EACH_ENTRY_SAFE(record, tmp, &devSvcManager->services, struct DevSvcRecord, entry) {
        DevSvcRecordFreeInstance(record);
    }
    OsalMutexDestroy(&devSvcManager->mutex);
}

struct IDevSvcManager *DevSvcManagerGetInstance()
{
    static struct IDevSvcManager *instance = NULL;
    if (instance == NULL) {
        instance = (struct IDevSvcManager *)HdfObjectManagerGetObject(HDF_OBJECT_ID_DEVSVC_MANAGER);
    }
    return instance;
}
