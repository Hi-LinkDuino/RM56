/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SOFTBUS_WLAN_PARAM_MONITOR_H
#define SOFTBUS_WLAN_PARAM_MONITOR_H

#include "hdf_base.h"
#include "hdf_device_desc.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t SoftbusWlanParamMonitorInit(struct HdfDeviceObject *device);
void SoftbusWlanParamMonitorDeinit(void);
void SoftbusWlanParamMonitorProcess(const struct HdfSBuf *reqData, struct HdfSBuf *rspData);

#ifdef __cplusplus
}
#endif

#endif // SOFTBUS_WLAN_PARAM_MONITOR_H