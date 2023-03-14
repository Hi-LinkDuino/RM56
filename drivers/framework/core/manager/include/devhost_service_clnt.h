/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef DEVHOST_SERVICE_CLNT_H
#define DEVHOST_SERVICE_CLNT_H

#include "devhost_service_if.h"
#include "hdf_slist.h"
#include "hdf_map.h"

struct DevHostServiceClnt {
    struct DListHead node;
    struct HdfSList devices;
    struct HdfSList unloadDevInfos;
    struct HdfSList dynamicDevInfos;
    Map *deviceHashMap;
    struct IDevHostService *hostService;
    uint16_t devCount;
    uint16_t hostId;
    int hostPid;
    const char *hostName;
    bool stopFlag;
};

int DevHostServiceClntInstallDriver(struct DevHostServiceClnt *hostClnt);
struct DevHostServiceClnt *DevHostServiceClntNewInstance(uint16_t hostId, const char *hostName);
void DevHostServiceClntFreeInstance(struct DevHostServiceClnt *hostClnt);
void DevHostServiceClntDelete(struct DevHostServiceClnt *hostClnt);

#endif /* DEVHOST_SERVICE_CLNT_H */
