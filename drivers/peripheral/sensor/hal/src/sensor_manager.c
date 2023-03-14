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

#include "sensor_manager.h"
#include <fcntl.h>
#include "hdf_dlist.h"
#include "hdf_io_service_if.h"
#include "osal_mem.h"
#include "sensor_channel.h"
#include "sensor_common.h"
#include "sensor_controller.h"
#include "sensor_if.h"

#define HDF_LOG_TAG    hdf_sensor_dal

struct SensorDevManager *GetSensorDevManager(void)
{
    static struct SensorDevManager devManager = {
        .initState = false,
        .hasSensorListener = false,
        .sensorSum = 0,
        .sensorInfoEntry = NULL,
    };

    return &devManager;
}

static int32_t GetSensorServiceList(void)
{
    struct SensorManagerNode *managerNode = NULL;
    struct SensorDevManager *manager = GetSensorDevManager();
    struct HdfSBuf *data = HdfSbufObtainDefaultSize();
    const char *svcName = NULL;

    CHECK_NULL_PTR_RETURN_VALUE(data, SENSOR_NULL_PTR);
    int32_t ret = HdfGetServiceNameByDeviceClass(DEVICE_CLASS_SENSOR, data);
    if (ret != SENSOR_SUCCESS) {
        HDF_LOGE("%{public}s :sensor manager get service class failed", __func__);
        HdfSbufRecycle(data);
        return SENSOR_INVALID_SERVICE;
    }

    (void)OsalMutexLock(&manager->mutex);
    while (true) {
        svcName = HdfSbufReadString(data);
        if (svcName == NULL) {
            break;
        }

        managerNode = (struct SensorManagerNode*)OsalMemCalloc(sizeof(*managerNode));
        if (managerNode == NULL) {
            break;
        }

        managerNode->sensorCount = 0;
        managerNode->ioService = HdfIoServiceBind(svcName);
        if (managerNode->ioService == NULL) {
            HDF_LOGE("%{public}s: Sensor manager get manager service name[%{public}s] failed", __func__, svcName);
            OsalMemFree(managerNode);
            managerNode = NULL;
            continue;
        }

        DListInsertTail(&managerNode->node, &manager->managerHead);
    }
    (void)OsalMutexUnlock(&manager->mutex);

    HdfSbufRecycle(data);
    data = NULL;

    if (DListIsEmpty(&manager->managerHead)) {
        HDF_LOGE("%{public}s: Sensor get service failed", __func__);
        return SENSOR_INVALID_SERVICE;
    }

    return SENSOR_SUCCESS;
}

static void ReleaseSensorServiceList()
{
    struct SensorManagerNode *pos = NULL;
    struct SensorManagerNode *tmp = NULL;
    struct SensorDevManager *manager = GetSensorDevManager();

    (void)OsalMutexLock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->managerHead, struct SensorManagerNode, node) {
        if (pos->ioService != NULL) {
            HdfIoServiceRecycle(pos->ioService);
            pos->ioService = NULL;
        };

        DListRemove(&(pos->node));
        OsalMemFree(pos);
        pos = NULL;
    }

    (void)OsalMutexLock(&manager->eventMutex);
    if (manager->serviceGroup != NULL) {
        if (manager->hasSensorListener) {
            struct HdfDevEventlistener *listener = GetSensorListener();
            if (listener != NULL) {
                HdfIoServiceGroupUnregisterListener(manager->serviceGroup, listener);
                manager->hasSensorListener = false;
            }
        }
        HdfIoServiceGroupRecycle(manager->serviceGroup);
    }
    (void)OsalMutexUnlock(&manager->eventMutex);
    (void)OsalMutexUnlock(&manager->mutex);
}

static int32_t InitSensorManager(void)
{
    struct SensorDevManager *manager = GetSensorDevManager();
    manager->recordDataCb[TRADITIONAL_SENSOR_TYPE] = NULL;
    manager->recordDataCb[MEDICAL_SENSOR_TYPE] = NULL;
    DListHeadInit(&manager->managerHead);
    DListHeadInit(&manager->sensorIdListHead);
    OsalMutexInit(&manager->mutex);
    OsalMutexInit(&manager->eventMutex);

    int32_t ret = GetSensorServiceList();
    if (ret != SENSOR_SUCCESS) {
        HDF_LOGE("%{public}s: Sensor get service failed", __func__);
        ReleaseSensorServiceList();
        OsalMutexDestroy(&manager->mutex);
        OsalMutexDestroy(&manager->eventMutex);
        return SENSOR_INVALID_SERVICE;
    }

    return SENSOR_SUCCESS;
}

const struct SensorInterface *NewSensorInterfaceInstance(void)
{
    static struct SensorInterface sensorDevInstance;
    struct SensorDevManager *manager = GetSensorDevManager();

    if (manager->initState) {
        return &sensorDevInstance;
    }

    // Construct device interface instance
    GetSensorDeviceMethods(&sensorDevInstance);

    if (InitSensorManager() != SENSOR_SUCCESS) {
        HDF_LOGE("%{public}s: Sensor init manager failed", __func__);
        return NULL;
    }
    manager->initState = true;
    HDF_LOGD("%{public}s: Get sensor device instance success", __func__);
    return &sensorDevInstance;
}

int32_t FreeSensorInterfaceInstance(void)
{
    struct SensorDevManager *manager = GetSensorDevManager();

    if (!manager->initState) {
        return SENSOR_SUCCESS;
    }

    ReleaseAllSensorInfo();
    ReleaseSensorServiceList();
    manager->recordDataCb[TRADITIONAL_SENSOR_TYPE] = NULL;
    manager->recordDataCb[MEDICAL_SENSOR_TYPE] = NULL;

    OsalMutexDestroy(&manager->mutex);
    OsalMutexDestroy(&manager->eventMutex);

    manager->initState = false;

    return SENSOR_SUCCESS;
}
