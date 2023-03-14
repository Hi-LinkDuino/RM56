/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HDI_DEVICE_MANAGER_INF_H
#define HDI_DEVICE_MANAGER_INF_H

#include "hdf_base.h"
#include "hdf_dlist.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct HdfRemoteService;

enum HDF_DEVICE_TYPE {
    HDF_LOCAL_SERVICE,
    HDF_REMOTE_SERVICE,
};

struct DeviceInfoNode {
    char *svcName;
    enum HDF_DEVICE_TYPE deviceType;
    struct DListHead node;
};

struct DeviceInfoList {
    uint32_t deviceCnt;
    struct DListHead list;
};

struct HDIDeviceManager {
    struct HdfRemoteService *remote;

    void (*FreeQueryDeviceList)(struct HDIDeviceManager *self, struct DeviceInfoList *list);
    int32_t (*QueryUsableDeviceInfo)(struct HDIDeviceManager *self, struct DeviceInfoList *list);
    int32_t (*QueryUnusableDeviceInfo)(struct HDIDeviceManager *self, struct DeviceInfoList *list);
    int32_t (*LoadDevice)(struct HDIDeviceManager *self, const char *serviceName);
    int32_t (*UnloadDevice)(struct HDIDeviceManager *self, const char *serviceName);
};

struct HDIDeviceManager *HDIDeviceManagerGet(void);
void HDIDeviceManagerRelease(struct HDIDeviceManager *devmgr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HDI_DEVICE_MANAGER_INF_H */