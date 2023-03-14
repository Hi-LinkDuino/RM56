/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef DEVICE_MANAGER_SERVICE_H
#define DEVICE_MANAGER_SERVICE_H

#include "devmgr_service_if.h"
#include "hdf_dlist.h"
#include "osal_mutex.h"

struct DevmgrService {
    struct IDevmgrService super;
    struct DListHead hosts;
    struct OsalMutex devMgrMutex;
};

int DevmgrServiceStartService(struct IDevmgrService *inst);
bool DevmgrServiceConstruct(struct DevmgrService *inst);
struct HdfObject *DevmgrServiceCreate(void);
void DevmgrServiceRelease(struct HdfObject *object);
struct IDevmgrService *DevmgrServiceGetInstance(void);
int32_t DevmgrServiceLoadLeftDriver(struct DevmgrService *devMgrSvc);

#endif /* DEVICE_MANAGER_SERVICE_H */
