/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SENSOR_DEVICE_MANAGER_H
#define SENSOR_DEVICE_MANAGER_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_workqueue.h"
#include "osal_mutex.h"
#include "sensor_device_type.h"
#include "sensor_device_if.h"

#define HDF_SENSOR_EVENT_QUEUE_NAME    "hdf_sensor_event_queue"

enum SensorCmd {
    SENSOR_CMD_GET_INFO_LIST = 0,
    SENSOR_CMD_OPS           = 1,
    SENSOR_CMD_END,
};
enum SensorOpsCmd {
    SENSOR_OPS_CMD_ENABLE        = 0,
    SENSOR_OPS_CMD_DISABLE       = 1,
    SENSOR_OPS_CMD_SET_BATCH     = 2,
    SENSOR_OPS_CMD_SET_MODE      = 3,
    SENSOR_OPS_CMD_SET_OPTION    = 4,
    SENSOR_OPS_CMD_BUTT,
};

struct SensorDevInfoNode {
    struct SensorDeviceInfo devInfo;
    struct DListHead node;
};

typedef int32_t (*SensorCmdHandle)(struct SensorDeviceInfo *info, struct HdfSBuf *data, struct HdfSBuf *reply);

struct SensorCmdHandleList {
    enum SensorOpsCmd cmd;
    SensorCmdHandle func;
};

struct SensorDevMgrData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    struct DListHead sensorDevInfoHead;
    struct OsalMutex mutex;
    struct OsalMutex eventMutex;
};

#endif /* SENSOR_DEVICE_MANAGER_H */
