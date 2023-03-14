/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SENSOR_DEVICE_IF_H
#define SENSOR_DEVICE_IF_H

#include "sensor_device_type.h"

struct SensorOps {
    int32_t (*Enable)(void);
    int32_t (*Disable)(void);
    int32_t (*SetBatch)(int64_t samplingInterval, int64_t reportInterval);
    int32_t (*SetMode)(int32_t mode);
    int32_t (*SetOption)(uint32_t option);
};

struct SensorDeviceInfo {
    struct SensorBasicInfo sensorInfo;
    struct SensorOps ops;
};

int32_t AddSensorDevice(const struct SensorDeviceInfo *deviceInfo);
int32_t DeleteSensorDevice(const struct SensorBasicInfo *sensorBaseInfo);
int32_t ReportSensorEvent(const struct SensorReportEvent *events);

#endif /* SENSOR_DEVICE_IF_H */
