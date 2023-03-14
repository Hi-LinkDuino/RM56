/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_SAMPLE_DRIVER_H
#define HDF_SAMPLE_DRIVER_H

#include "hdf_object.h"

#define SAMPLE_SERVICE "sample_service"

typedef enum {
    SAMPLE_DRIVER_REGISTER_DEVICE = 0,
    SAMPLE_DRIVER_UNREGISTER_DEVICE,
    SAMPLE_DRIVER_UPDATE_SERVICE_INFO,
    SAMPLE_DRIVER_SENDEVENT_SINGLE_DEVICE,
    SAMPLE_DRIVER_SENDEVENT_BROADCAST_DEVICE,
    SAMPLE_DRIVER_PM_STATE_INJECT,
} SAMPLE_DRIVER_CMDID;

struct HdfDeviceObject *GetDeviceObject(void);

#endif // HDF_MAIN_TEST_H

