/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef DEVICE_HOST_SERVICE_H
#define DEVICE_HOST_SERVICE_H

#include "devhost_service_if.h"
#include "hdf_service_observer.h"
#include "hdf_dlist.h"
#include "osal_mutex.h"
#include "osal_sysevent.h"

struct DevHostService {
    struct IDevHostService super;
    uint16_t hostId;
    const char *hostName;
    struct DListHead devices;
    struct HdfServiceObserver observer;
    struct HdfSysEventNotifyNode sysEventNotifyNode;
};

void DevHostServiceConstruct(struct DevHostService *service);
void DevHostServiceDestruct(struct DevHostService *service);
int DevHostServiceAddDevice(struct IDevHostService *inst, const struct HdfDeviceInfo *deviceInfo);
int DevHostServiceDelDevice(struct IDevHostService *inst, devid_t devId);
struct IDevHostService *DevHostServiceNewInstance(uint16_t hostId, const char *hostName);
void DevHostServiceFreeInstance(struct IDevHostService *service);
struct HdfObject *DevHostServiceCreate(void);
void DevHostServiceRelease(struct HdfObject *object);

#endif /* DEVICE_HOST_SERVICE_H */
