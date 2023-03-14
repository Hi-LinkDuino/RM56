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

#ifndef HDF_SAMPLE_DRIVER_H
#define HDF_SAMPLE_DRIVER_H

#include "hdf_object.h"

#define SAMPLE_SERVICE "sample_service"

typedef enum {
    SAMPLE_DRIVER_REGISTE_DEVICE = 0,
    SAMPLE_DRIVER_UNREGISTE_DEVICE,
    SAMPLE_DRIVER_UPDATE_SERVICE_INFO,
    SAMPLE_DRIVER_SENDEVENT_SINGLE_DEVICE,
    SAMPLE_DRIVER_SENDEVENT_BROADCAST_DEVICE,
} SAMPLE_DRIVER_CMDID;

struct HdfDeviceObject *GetDeviceObject(void);

#endif // HDF_MAIN_TEST_H

