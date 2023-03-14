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

#ifndef HDF_DEVICE_FULL_H
#define HDF_DEVICE_FULL_H

#include "hdf_device.h"
#include "hdf_device_thread.h"

struct HdfDeviceFull {
    struct HdfDevice super;
    struct DeviceThread *deviceThread;
};

struct HdfObject *HdfDeviceFullCreate(void);
void HdfDeviceFullRelease(struct HdfObject *object);

#endif /* HDF_DEVICE_FULL_H */
