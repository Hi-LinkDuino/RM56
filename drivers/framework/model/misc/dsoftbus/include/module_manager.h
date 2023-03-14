/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SOFTBUS_MODULE_MANAGER_H
#define SOFTBUS_MODULE_MANAGER_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_sbuf.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SOFTBUS_MODULE_WLAN_PARAM_MONITOR = 0,
    SOFTBUS_MODULE_LWIP_MONITOR,
    SOFTBUS_MODULE_MAX_INDEX,
} SoftbusDriverModuleId;

typedef int32_t (*SoftbusDriverModuleInit)(struct HdfDeviceObject *device);
typedef void (*SoftbusDriverModuleDeinit)(void);
typedef void (*SoftbusDriverModuleProcess)(const struct HdfSBuf *reqData, struct HdfSBuf *rspData);

typedef struct {
    int32_t moduleId;
    SoftbusDriverModuleInit init;
    SoftbusDriverModuleDeinit deinit;
    SoftbusDriverModuleProcess process;
} SoftbusDriverModule;

int32_t SoftbusModuleManagerInit(struct HdfDeviceObject *device);
void SoftbusModuleManagerDeinit(void);
void SoftbusDispatchModuleCommand(int32_t moduleId, const struct HdfSBuf *reqData, struct HdfSBuf *rspData);

#ifdef __cplusplus
}
#endif

#endif // SOFTBUS_MODULE_MANAGER_H