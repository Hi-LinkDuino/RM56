/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef DAC_CORE_H
#define DAC_CORE_H

#include "osal_spinlock.h"
#include "hdf_base.h"
#include "dac_if.h"
#include "platform_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define DAC_DEVICES_MAX 15

struct DacDevice;
struct DacMethod;
struct DacLockMethod;

struct DacDevice {
    const struct DacMethod *ops;
    OsalSpinlock spin;
    uint32_t devNum;
    uint32_t chanNum;
    const struct DacLockMethod *lockOps;
    void *priv;
};

struct DacMethod {
    int32_t (*write)(struct DacDevice *device, uint32_t channel, uint32_t val);
    int32_t (*start)(struct DacDevice *device);
    int32_t (*stop)(struct DacDevice *device);
};

struct DacLockMethod {
    int32_t (*lock)(struct DacDevice *device);
    void (*unlock)(struct DacDevice *device);
};

int32_t DacDeviceAdd(struct DacDevice *device);

void DacDeviceRemove(struct DacDevice *device);

struct DacDevice *DacDeviceGet(uint32_t number);

void DacDevicePut(struct DacDevice *device);

int32_t DacDeviceWrite(struct DacDevice *device, uint32_t channel, uint32_t val);

int32_t DacDeviceStart(struct DacDevice *device);

int32_t DacDeviceStop(struct DacDevice *device);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* DAC_CORE_H */
