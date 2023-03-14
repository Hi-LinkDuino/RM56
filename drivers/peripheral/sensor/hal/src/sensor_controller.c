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

#include "sensor_controller.h"
#include <fcntl.h>
#include <inttypes.h>
#include <securec.h>
#include "osal_mem.h"
#include "osal_mutex.h"
#include "sensor_channel.h"
#include "sensor_common.h"
#include "sensor_if.h"
#include "sensor_manager.h"

#define HDF_LOG_TAG    hdf_sensor_dal

#define HDF_SENSOR_INFO_MAX_SIZE (4 * 1024) // 4kB

void ReleaseAllSensorInfo(void)
{
    struct SensorDevManager *manager = GetSensorDevManager();
    struct SensorIdListNode *pos = NULL;
    struct SensorIdListNode *tmp = NULL;

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->sensorIdListHead, struct SensorIdListNode, node) {
        DListRemove(&(pos->node));
        OsalMemFree(pos);
    }
    if (manager->sensorInfoEntry != NULL) {
        OsalMemFree(manager->sensorInfoEntry);
        manager->sensorInfoEntry = NULL;
    }
    manager->sensorSum = 0;
}

static int32_t SetSensorIdClassification(void)
{
    struct SensorDevManager *manager = GetSensorDevManager();
    struct SensorManagerNode *pos = NULL;
    int32_t begin = 0;
    int32_t end;
    struct SensorIdListNode *sensorIdNode = NULL;
    CHECK_NULL_PTR_RETURN_VALUE(manager->sensorInfoEntry, SENSOR_NULL_PTR);

    DLIST_FOR_EACH_ENTRY(pos, &manager->managerHead, struct SensorManagerNode, node) {
        end = begin + pos->sensorCount;
        if (end > manager->sensorSum) {
            break;
        }

        for (int i = begin; i < end; i++) {
            sensorIdNode = (struct SensorIdListNode*)OsalMemCalloc(sizeof(*sensorIdNode));
            CHECK_NULL_PTR_RETURN_VALUE(sensorIdNode, SENSOR_NULL_PTR);
            sensorIdNode->ioService = pos->ioService;
            sensorIdNode->sensorId = manager->sensorInfoEntry[i].sensorId;
            SetSensorIdBySensorType(manager->sensorInfoEntry[i].sensorTypeId, manager->sensorInfoEntry[i].sensorId);
            if (sensorIdNode->sensorId == SENSOR_TYPE_ACCELEROMETER) {
                manager->sensorInfoEntry[i].maxRange = manager->sensorInfoEntry[i].maxRange * HDI_SENSOR_GRAVITY;
                manager->sensorInfoEntry[i].accuracy = HDI_SENSOR_GRAVITY / HDI_SENSOR_ACCEL_LSB / HDI_SENSOR_UNITS;
                manager->sensorInfoEntry[i].power = manager->sensorInfoEntry[i].power / HDI_SENSOR_UNITS;
            }
            DListInsertTail(&sensorIdNode->node, &manager->sensorIdListHead);
        }
        begin = end;
    }

    return HDF_SUCCESS;
}

static int32_t GetSensorInfoFromReply(struct HdfSBuf *reply)
{
    struct SensorDevManager *manager = GetSensorDevManager();
    struct SensorInformation *pos = NULL;
    struct SensorBasicInformation *buf = NULL;
    int32_t count = manager->sensorSum;
    uint32_t len;

    if (manager->sensorInfoEntry != NULL) {
        OsalMemFree(manager->sensorInfoEntry);
        manager->sensorInfoEntry = NULL;
    }

    manager->sensorInfoEntry = (struct SensorInformation *)OsalMemCalloc(sizeof(*manager->sensorInfoEntry) * count);
    if (manager->sensorInfoEntry == NULL) {
        HDF_LOGE("%{public}s: Sensor info malloc failed", __func__);
        return SENSOR_FAILURE;
    }

    pos = manager->sensorInfoEntry;
    size_t preLen = sizeof(*manager->sensorInfoEntry) -
        (sizeof(pos->maxRange) + sizeof(pos->accuracy) + sizeof(pos->power));

    for (int32_t i = 0; i < count; i++) {
        if (!HdfSbufReadBuffer(reply, (const void **)&buf, &len) || buf == NULL) {
            HDF_LOGE("%{public}s: Sensor read reply info failed", __func__);
            break;
        }

        if (memcpy_s(pos, sizeof(*pos), (void *)buf, preLen) != EOK) {
            HDF_LOGE("%{public}s: Sensor copy reply info failed", __func__);
            goto ERROR;
        }
        pos->maxRange = (float)(buf->maxRange);
        pos->accuracy = (float)(buf->accuracy);
        pos->power = (float)(buf->power);
        pos++;
    }

    if (SetSensorIdClassification() != SENSOR_SUCCESS) {
        HDF_LOGE("%{public}s: Sensor id Classification failed", __func__);
        goto ERROR;
    }
    return SENSOR_SUCCESS;

ERROR:
    ReleaseAllSensorInfo();
    return SENSOR_FAILURE;
}

static int32_t GetSensorNumByManagerType(struct HdfSBuf *reply)
{
    struct SensorDevManager *manager = GetSensorDevManager();
    int32_t count = (int32_t)(HdfSbufGetDataSize(reply) / sizeof(struct SensorBasicInformation));

    return ((count > manager->sensorSum) ? (count - manager->sensorSum) : 0);
}

static int32_t GetSensorInfo(struct SensorInformation **sensor, int32_t *count)
{
    struct SensorDevManager *manager = GetSensorDevManager();
    CHECK_NULL_PTR_RETURN_VALUE(sensor, SENSOR_NULL_PTR);
    CHECK_NULL_PTR_RETURN_VALUE(count, SENSOR_NULL_PTR);

    if (manager->sensorSum > 0) {
        *count = manager->sensorSum;
        *sensor = manager->sensorInfoEntry;
        return SENSOR_SUCCESS;
    }

    struct HdfSBuf *reply = HdfSbufObtain(HDF_SENSOR_INFO_MAX_SIZE);
    CHECK_NULL_PTR_RETURN_VALUE(reply, SENSOR_NULL_PTR);

    (void)OsalMutexLock(&manager->mutex);
    manager->sensorSum = 0;
    struct SensorManagerNode *pos = NULL;
    DLIST_FOR_EACH_ENTRY(pos, &manager->managerHead, struct SensorManagerNode, node) {
        if (manager->sensorSum >= SENSOR_TYPE_MAX) {
            break;
        }
        pos->sensorCount = 0;
        if ((pos->ioService == NULL) || (pos->ioService->dispatcher == NULL) ||
            (pos->ioService->dispatcher->Dispatch == NULL)) {
            HDF_LOGE("%{public}s: Sensor pos para failed", __func__);
            continue;
        }

        int32_t ret = pos->ioService->dispatcher->Dispatch(&pos->ioService->object,
            SENSOR_IO_CMD_GET_INFO_LIST, NULL, reply);
        if (ret != SENSOR_SUCCESS) {
            HDF_LOGE("%{public}s: Sensor dispatch info failed[%{public}d]", __func__, ret);
            break;
        }

        pos->sensorCount = GetSensorNumByManagerType(reply);
        manager->sensorSum += pos->sensorCount;
    }

    if (manager->sensorSum == 0) {
        HdfSbufRecycle(reply);
        (void)OsalMutexUnlock(&manager->mutex);
        HDF_LOGE("%{public}s: Sensor get info count failed", __func__);
        return SENSOR_FAILURE;
    }

    if (GetSensorInfoFromReply(reply) != SENSOR_SUCCESS) {
        HdfSbufRecycle(reply);
        (void)OsalMutexUnlock(&manager->mutex);
        HDF_LOGE("%{public}s: Sensor get info from reply failed", __func__);
        return SENSOR_FAILURE;
    }

    HdfSbufRecycle(reply);
    (void)OsalMutexUnlock(&manager->mutex);

    *count = manager->sensorSum;
    *sensor = manager->sensorInfoEntry;
    return SENSOR_SUCCESS;
}

static struct HdfIoService *GetSensorServiceBySensorId(int32_t sensorId)
{
    struct SensorDevManager *manager = GetSensorDevManager();
    struct SensorIdListNode *sensorIdPos = NULL;

    DLIST_FOR_EACH_ENTRY(sensorIdPos, &manager->sensorIdListHead, struct SensorIdListNode, node) {
        if (sensorIdPos->sensorId == sensorId) {
            return sensorIdPos->ioService;
        }
    }
    return NULL;
}

static int32_t SendSensorMsg(int32_t sensorId, struct HdfSBuf *msg, struct HdfSBuf *reply)
{
    struct HdfIoService *ioService = NULL;

    ioService = GetSensorServiceBySensorId(sensorId);
    CHECK_NULL_PTR_RETURN_VALUE(ioService, SENSOR_NOT_SUPPORT);
    CHECK_NULL_PTR_RETURN_VALUE(ioService->dispatcher, SENSOR_NULL_PTR);
    CHECK_NULL_PTR_RETURN_VALUE(ioService->dispatcher->Dispatch, SENSOR_NULL_PTR);

    int32_t ret = ioService->dispatcher->Dispatch(&ioService->object, SENSOR_IO_CMD_OPS, msg, reply);
    if (ret != SENSOR_SUCCESS) {
        HDF_LOGE("%{public}s: Sensor dispatch failed", __func__);
        return ret;
    }
    return SENSOR_SUCCESS;
}

static int32_t EnableSensor(int32_t sensorId)
{
    struct HdfSBuf *msg = HdfSbufObtainDefaultSize();
    if (msg == NULL) {
        HDF_LOGE("%{public}s: Failed to obtain sBuf size", __func__);
        return SENSOR_FAILURE;
    }

    if (!HdfSbufWriteInt32(msg, sensorId)) {
        HDF_LOGE("%{public}s: Sensor write id failed", __func__);
        HdfSbufRecycle(msg);
        return SENSOR_FAILURE;
    }

    if (!HdfSbufWriteInt32(msg, SENSOR_OPS_IO_CMD_ENABLE)) {
        HDF_LOGE("%{public}s: Sensor write enable failed", __func__);
        HdfSbufRecycle(msg);
        return SENSOR_FAILURE;
    }

    int32_t ret = SendSensorMsg(sensorId, msg, NULL);
    if (ret != SENSOR_SUCCESS) {
        HDF_LOGE("%{public}s: Sensor enable failed, ret[%{public}d]", __func__, ret);
    }
    HdfSbufRecycle(msg);

    return ret;
}

static int32_t DisableSensor(int32_t sensorId)
{
    struct HdfSBuf *msg = HdfSbufObtainDefaultSize();
    if (msg == NULL) {
        HDF_LOGE("%{public}s: Failed to obtain sBuf", __func__);
        return SENSOR_FAILURE;
    }

    if (!HdfSbufWriteInt32(msg, sensorId)) {
        HDF_LOGE("%{public}s: Sensor write id failed", __func__);
        HdfSbufRecycle(msg);
        return SENSOR_FAILURE;
    }

    if (!HdfSbufWriteInt32(msg, SENSOR_OPS_IO_CMD_DISABLE)) {
        HDF_LOGE("%{public}s: Sensor write disable failed", __func__);
        HdfSbufRecycle(msg);
        return SENSOR_FAILURE;
    }

    int32_t ret = SendSensorMsg(sensorId, msg, NULL);
    if (ret != SENSOR_SUCCESS) {
        HDF_LOGE("%{public}s: Sensor disable failed, ret[%{public}d]", __func__, ret);
    }
    HdfSbufRecycle(msg);

    return ret;
}

static int32_t SetSensorBatch(int32_t sensorId, int64_t samplingInterval, int64_t interval)
{
    if (samplingInterval < 0) {
        HDF_LOGE("%{public}s: invalid param , samplingInterval is [%{public}" PRId64 "]", __func__, samplingInterval);
        return SENSOR_INVALID_PARAM;
    }
    struct HdfSBuf *msg = HdfSbufObtainDefaultSize();
    if (msg == NULL) {
        HDF_LOGE("%{public}s: Msg failed to obtain sBuf", __func__);
        return SENSOR_FAILURE;
    }

    if (!HdfSbufWriteInt32(msg, sensorId)) {
        HDF_LOGE("%{public}s: Sensor write id failed", __func__);
        HdfSbufRecycle(msg);
        return SENSOR_FAILURE;
    }

    if (!HdfSbufWriteInt32(msg, SENSOR_OPS_IO_CMD_SET_BATCH) || !HdfSbufWriteInt64(msg, samplingInterval) ||
        !HdfSbufWriteInt64(msg, interval)) {
        HDF_LOGE("%{public}s: Sensor write failed", __func__);
        HdfSbufRecycle(msg);
        return SENSOR_FAILURE;
    }

    int32_t ret = SendSensorMsg(sensorId, msg, NULL);
    if (ret != SENSOR_SUCCESS) {
        HDF_LOGE("%{public}s: Sensor set batch failed, ret[%{public}d]", __func__, ret);
    }
    HdfSbufRecycle(msg);

    return ret;
}

static int32_t SetSensorMode(int32_t sensorId, int32_t mode)
{
    struct HdfSBuf *msg = HdfSbufObtainDefaultSize();
    if (msg == NULL) {
        HDF_LOGE("%{public}s: Msg failed to obtain sBuf", __func__);
        return SENSOR_FAILURE;
    }

    if (!HdfSbufWriteInt32(msg, sensorId)) {
        HDF_LOGE("%{public}s: Sensor write id failed", __func__);
        HdfSbufRecycle(msg);
        return SENSOR_FAILURE;
    }

    if (!HdfSbufWriteInt32(msg, SENSOR_OPS_IO_CMD_SET_MODE) || !HdfSbufWriteInt32(msg, mode)) {
        HDF_LOGE("%{public}s: Sensor write failed", __func__);
        HdfSbufRecycle(msg);
        return SENSOR_FAILURE;
    }

    int32_t ret = SendSensorMsg(sensorId, msg, NULL);
    if (ret != SENSOR_SUCCESS) {
        HDF_LOGE("%{public}s: Sensor set mode failed, ret[%{public}d]", __func__, ret);
    }
    HdfSbufRecycle(msg);

    return ret;
}

static int32_t SetSensorOption(int32_t sensorId, uint32_t option)
{
    struct HdfSBuf *msg = HdfSbufObtainDefaultSize();
    if (msg == NULL) {
        HDF_LOGE("%{public}s: Msg failed to obtain sBuf", __func__);
        return SENSOR_FAILURE;
    }

    if (!HdfSbufWriteInt32(msg, sensorId)) {
        HDF_LOGE("%{public}s: Sensor write id failed", __func__);
        HdfSbufRecycle(msg);
        return SENSOR_FAILURE;
    }

    if (!HdfSbufWriteInt32(msg, SENSOR_OPS_IO_CMD_SET_OPTION) || !HdfSbufWriteUint32(msg, option)) {
        HDF_LOGE("%{public}s: Sensor write failed", __func__);
        HdfSbufRecycle(msg);
        return SENSOR_FAILURE;
    }

    int32_t ret = SendSensorMsg(sensorId, msg, NULL);
    if (ret != SENSOR_SUCCESS) {
        HDF_LOGE("%{public}s: Sensor set option failed, ret[%{public}d]", __func__, ret);
    }
    HdfSbufRecycle(msg);

    return ret;
}

void GetSensorDeviceMethods(struct SensorInterface *device)
{
    CHECK_NULL_PTR_RETURN(device);
    device->GetAllSensors = GetSensorInfo;
    device->Enable = EnableSensor;
    device->Disable = DisableSensor;
    device->SetBatch = SetSensorBatch;
    device->SetMode = SetSensorMode;
    device->SetOption = SetSensorOption;
    device->Register = Register;
    device->Unregister = Unregister;
}
