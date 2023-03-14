/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_device_info.h"
#include "hdf_log.h"
#include "osal_mem.h"

#define HDF_LOG_TAG device_info

void HdfDeviceInfoConstruct(struct HdfDeviceInfo *deviceInfo)
{
    if (deviceInfo == NULL) {
        return;
    }
    deviceInfo->isDynamic = false;
    deviceInfo->status = HDF_SERVICE_UNUSABLE;
    deviceInfo->deviceType = HDF_DEV_LOCAL_SERVICE;
    deviceInfo->deviceId = 0;
    deviceInfo->policy = SERVICE_POLICY_INVALID;
    deviceInfo->priority = 0;
    deviceInfo->preload = DEVICE_PRELOAD_ENABLE;
    deviceInfo->permission = 0;
    deviceInfo->svcName = NULL;
    deviceInfo->moduleName = NULL;
    deviceInfo->deviceMatchAttr = NULL;
}

struct HdfDeviceInfo *HdfDeviceInfoNewInstance(void)
{
    struct HdfDeviceInfo *deviceInfo =
            (struct HdfDeviceInfo*)OsalMemCalloc(sizeof(struct HdfDeviceInfo));
    if (deviceInfo != NULL) {
        HdfDeviceInfoConstruct(deviceInfo);
        return deviceInfo;
    }
    HDF_LOGE("failed to create deviceInfo, oom");
    return NULL;
}

void HdfDeviceInfoFreeInstance(struct HdfDeviceInfo *deviceInfo)
{
    if (deviceInfo != NULL) {
        OsalMemFree(deviceInfo);
    }
}

void HdfDeviceInfoDelete(struct HdfSListNode *listEntry)
{
    struct HdfDeviceInfo *deviceInfo = (struct HdfDeviceInfo *)listEntry;
    HdfDeviceInfoFreeInstance(deviceInfo);
}

