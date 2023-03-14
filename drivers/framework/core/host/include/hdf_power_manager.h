/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_POWER_MANAGER_H
#define HDF_POWER_MANAGER_H

#include "hdf_task_queue.h"
#include "power_state_token.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    HDF_PM_REQUEST_ACQUIRE,
    HDF_PM_REQUEST_RELEASE,
} HDF_PM_REQUEST_TYPE;

struct HdfPmRequest {
    struct PowerStateToken *token;
    HDF_PM_REQUEST_TYPE pmType;
    struct HdfTaskType task;
};

struct PmTaskQueue *HdfPmTaskQueueInit(HdfTaskFunc func);

int32_t HdfPowerManagerInit(void);
void HdfPmTaskPut(struct PowerStateToken *powerToken, HDF_PM_REQUEST_TYPE type);
void HdfPowerManagerExit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HDF_POWER_MANAGER_H */
