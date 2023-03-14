/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "devhost_service_clnt.h"
#include "device_token_clnt.h"
#include "devmgr_service_start.h"
#include "hdf_base.h"
#include "hdf_driver_installer.h"
#include "hdf_log.h"
#include "osal_mem.h"

#define HDF_LOG_TAG devhost_service_clnt

int DevHostServiceClntInstallDriver(struct DevHostServiceClnt *hostClnt)
{
    int ret;
    struct HdfSListIterator it;
    struct HdfDeviceInfo *deviceInfo = NULL;
    struct IDevHostService *devHostSvcIf = NULL;
    if (hostClnt == NULL) {
        HDF_LOGE("failed to install driver, hostClnt is null");
        return HDF_FAILURE;
    }

    devHostSvcIf = (struct IDevHostService *)hostClnt->hostService;
    if (devHostSvcIf == NULL || devHostSvcIf->AddDevice == NULL) {
        HDF_LOGE("devHostSvcIf or devHostSvcIf->AddDevice is null");
        return HDF_FAILURE;
    }
    HdfSListIteratorInit(&it, &hostClnt->unloadDevInfos);
    while (HdfSListIteratorHasNext(&it)) {
        deviceInfo = (struct HdfDeviceInfo *)HdfSListIteratorNext(&it);
        if ((deviceInfo == NULL) || (deviceInfo->preload == DEVICE_PRELOAD_DISABLE)) {
            continue;
        }
        /*
         * If quick start feature enable, the device which 'preload' attribute set as
         * DEVICE_PRELOAD_ENABLE_STEP2 will be loaded later
         */
        if (DeviceManagerIsQuickLoad() == DEV_MGR_QUICK_LOAD &&
            deviceInfo->preload == DEVICE_PRELOAD_ENABLE_STEP2) {
            continue;
        }
        ret = devHostSvcIf->AddDevice(devHostSvcIf, deviceInfo);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("failed to install driver %s, ret = %d", deviceInfo->svcName, ret);
            continue;
        }
#ifndef __USER__
        HdfSListIteratorRemove(&it);
#endif
    }
    return HDF_SUCCESS;
}

static void DevHostServiceClntConstruct(struct DevHostServiceClnt *hostClnt)
{
    HdfSListInit(&hostClnt->devices);
    hostClnt->deviceHashMap = (Map *)OsalMemCalloc(sizeof(Map));
    if (hostClnt->deviceHashMap == NULL) {
        HDF_LOGE("%s:failed to malloc deviceHashMap", __func__);
        return;
    }
    MapInit(hostClnt->deviceHashMap);
}

struct DevHostServiceClnt *DevHostServiceClntNewInstance(uint16_t hostId, const char *hostName)
{
    struct DevHostServiceClnt *hostClnt =
        (struct DevHostServiceClnt *)OsalMemCalloc(sizeof(struct DevHostServiceClnt));
    if (hostClnt != NULL) {
        hostClnt->hostId = hostId;
        hostClnt->hostName = hostName;
        hostClnt->devCount = 0;
        hostClnt->hostPid = -1;
        hostClnt->stopFlag = false;
        DevHostServiceClntConstruct(hostClnt);
    }
    return hostClnt;
}

void DevHostServiceClntFreeInstance(struct DevHostServiceClnt *hostClnt)
{
    if (hostClnt != NULL) {
        HdfSListFlush(&hostClnt->devices, DeviceTokenClntDelete);
        HdfSListFlush(&hostClnt->unloadDevInfos, HdfDeviceInfoDelete);
        HdfSListFlush(&hostClnt->dynamicDevInfos, HdfDeviceInfoDelete);
        OsalMemFree(hostClnt->deviceHashMap);
        OsalMemFree(hostClnt);
    }
}

void DevHostServiceClntDelete(struct DevHostServiceClnt *hostClnt)
{
    if (hostClnt != NULL) {
        DevHostServiceClntFreeInstance(hostClnt);
    }
}

