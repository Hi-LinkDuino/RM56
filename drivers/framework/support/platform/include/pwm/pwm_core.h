/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PWM_CORE_H
#define PWM_CORE_H

#include "pwm_if.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_spinlock.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct PwmMethod;
struct PwmDev;

struct PwmMethod {
    int32_t (*setConfig)(struct PwmDev *pwm, struct PwmConfig *config);
    int32_t (*open)(struct PwmDev *pwm);
    int32_t (*close)(struct PwmDev *pwm);
};

struct PwmDev {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    struct PwmConfig cfg;
    struct PwmMethod *method;
    bool busy;
    uint32_t num;
    OsalSpinlock lock;
    void *priv;
};

int32_t PwmDeviceGet(struct PwmDev *pwm);
int32_t PwmDevicePut(struct PwmDev *pwm);
int32_t PwmDeviceSetConfig(struct PwmDev *pwm, struct PwmConfig *config);
int32_t PwmDeviceGetConfig(struct PwmDev *pwm, struct PwmConfig *config);
void *PwmGetPriv(struct PwmDev *pwm);
int32_t PwmSetPriv(struct PwmDev *pwm, void *priv);
int32_t PwmDeviceAdd(struct HdfDeviceObject *obj, struct PwmDev *pwm);
void PwmDeviceRemove(struct HdfDeviceObject *obj, struct PwmDev *pwm);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* PWM_CORE_H */
