/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef ADC_CORE_H
#define ADC_CORE_H

#include "osal_spinlock.h"
#include "hdf_base.h"
#include "adc_if.h"
#include "platform_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define ADC_DEVICES_MAX 15

struct AdcDevice;
struct AdcMethod;
struct AdcLockMethod;

struct AdcDevice {
    const struct AdcMethod *ops;
    OsalSpinlock spin;
    uint32_t devNum;
    uint32_t chanNum;
    const struct AdcLockMethod *lockOps;
    void *priv;
};

struct AdcMethod {
    int32_t (*read)(struct AdcDevice *device, uint32_t channel, uint32_t *val);
    int32_t (*start)(struct AdcDevice *device);
    int32_t (*stop)(struct AdcDevice *device);
};

struct AdcLockMethod {
    int32_t (*lock)(struct AdcDevice *device);
    void (*unlock)(struct AdcDevice *device);
};

int32_t AdcDeviceAdd(struct AdcDevice *device);

void AdcDeviceRemove(struct AdcDevice *device);

struct AdcDevice *AdcDeviceGet(uint32_t number);

void AdcDevicePut(struct AdcDevice *device);

int32_t AdcDeviceRead(struct AdcDevice *device, uint32_t channel, uint32_t *val);

int32_t AdcDeviceStart(struct AdcDevice *device);

int32_t AdcDeviceStop(struct AdcDevice *device);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* ADC_CORE_H */
