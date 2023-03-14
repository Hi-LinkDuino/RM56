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

#ifndef HDI_SENSOR_MANAGER_H
#define HDI_SENSOR_MANAGER_H

#include "hdf_dlist.h"
#include "hdf_io_service_if.h"
#include "osal_mutex.h"
#include "sensor_type.h"

#define CALLBACK_TYPE_COUNT 2

struct SensorIdListNode {
    int32_t sensorId;
    struct HdfIoService *ioService;
    struct DListHead node;
};

struct SensorManagerNode {
    struct HdfIoService *ioService;
    int32_t sensorCount;
    struct DListHead node;
};

struct SensorDevManager {
    bool initState;
    bool hasSensorListener;
    RecordDataCallback recordDataCb[CALLBACK_TYPE_COUNT];
    struct DListHead managerHead;
    struct DListHead sensorIdListHead;
    struct HdfIoServiceGroup *serviceGroup;
    struct SensorInformation *sensorInfoEntry;
    int32_t sensorSum;
    struct OsalMutex mutex;
    struct OsalMutex eventMutex;
};

struct SensorDevManager *GetSensorDevManager(void);

#endif /* HDI_SENSOR_MANAGER_H */
