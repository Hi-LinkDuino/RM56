/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_SENSOR_DRIVER_TEST_H
#define HDF_SENSOR_DRIVER_TEST_H

#include "hdf_workqueue.h"
#include "osal_timer.h"

#define SENSOR_TEST_SAMPLING_200_MS    200000000

struct SensorTestDrvData {
    uint8_t initStatus;
    int64_t interval;
    HdfWorkQueue workQueue;
    HdfWork work;
    OsalTimer timer;
    bool enable;
};

#endif // HDF_SENSOR_DRIVER_TEST_H