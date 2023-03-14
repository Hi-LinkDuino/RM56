/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef DEVICE_SERVICE_MANAGER_H
#define DEVICE_SERVICE_MANAGER_H

#include "devsvc_manager_if.h"
#include "hdf_service_observer.h"
#include "hdf_dlist.h"
#include "osal_mutex.h"

struct DevSvcManager {
    struct IDevSvcManager super;
    struct DListHead services;
    struct HdfServiceObserver observer;
    struct DListHead svcstatListeners;
    struct OsalMutex mutex;
};

struct HdfObject *DevSvcManagerCreate(void);
bool DevSvcManagerConstruct(struct DevSvcManager *inst);
void DevSvcManagerRelease(struct IDevSvcManager *inst);
struct IDevSvcManager *DevSvcManagerGetInstance(void);

int DevSvcManagerStartService(void);

int DevSvcManagerAddService(struct IDevSvcManager *manager, const char *svcName,
                            uint16_t devClass, struct HdfDeviceObject *service, const char *servInfo);
struct HdfObject *DevSvcManagerGetService(struct IDevSvcManager *manager, const char *svcName);
void DevSvcManagerRemoveService(struct IDevSvcManager *manager, const char *svcName);
void DevSvcManagerListService(struct HdfSBuf *serviecNameSet, DeviceClass deviceClass);

int DevSvcManagerClntSubscribeService(const char *svcName, struct SubscriberCallback callback);
int DevSvcManagerClntUnsubscribeService(const char *svcName);

#endif /* DEVICE_SERVICE_MANAGER_H */
