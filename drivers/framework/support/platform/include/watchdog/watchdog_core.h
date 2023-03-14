/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef WATCHDOG_CORE_H
#define WATCHDOG_CORE_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_spinlock.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct WatchdogCntlr;
struct WatchdogMethod;

struct WatchdogCntlr {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    OsalSpinlock lock;
    struct WatchdogMethod *ops;
    int16_t wdtId;
    void *priv;
};

struct WatchdogMethod {
    int32_t (*getStatus)(struct WatchdogCntlr *wdt, int32_t *status);
    int32_t (*setTimeout)(struct WatchdogCntlr *wdt, uint32_t seconds);
    int32_t (*getTimeout)(struct WatchdogCntlr *wdt, uint32_t *seconds);
    int32_t (*start)(struct WatchdogCntlr *wdt);
    int32_t (*stop)(struct WatchdogCntlr *wdt);
    int32_t (*feed)(struct WatchdogCntlr *wdt);
    int32_t (*getPriv)(struct WatchdogCntlr *wdt);
    void (*releasePriv)(struct WatchdogCntlr *wdt);
};

/**
 * @brief Add a new WatchdogCntlr to HDF.
 *
 * @param cntlr The watchdog controller to be added.
 *
 * @return Returns 0 on success; returns a negative value otherwise.
 * @since 1.0
 */
int32_t WatchdogCntlrAdd(struct WatchdogCntlr *cntlr);

/**
 * @brief Remove the watchdog controller from HDF.
 *
 * @param cntlr The watchdog controller to be removed.
 *
 * @since 1.0
 */
void WatchdogCntlrRemove(struct WatchdogCntlr *cntlr);


/**
 * @brief Turn HdfDeviceObject to an Watchdog.
 *
 * @param device Indicates a HdfDeviceObject.
 *
 * @return Retrns the pointer of the WatchdogCntlr on success; returns NULL otherwise.
 * @since 1.0
 */
static inline struct WatchdogCntlr *WatchdogCntlrFromDevice(struct HdfDeviceObject *device)
{
    return (device == NULL) ? NULL : (struct WatchdogCntlr *)device->service;
}

/**
 * @brief Turn WatchdogCntlr to a HdfDeviceObject.
 *
 * @param wdt Indicates the WATCHDOG wdt device.
 *
 * @return Retrns the pointer of the HdfDeviceObject on success; returns NULL otherwise.
 * @since 1.0
 */
static inline struct HdfDeviceObject *WatchdogCntlrToDevice(struct WatchdogCntlr *wdt)
{
    return (wdt == NULL) ? NULL : wdt->device;
}

int32_t WatchdogCntlrGetStatus(struct WatchdogCntlr *cntlr, int32_t *status);

int32_t WatchdogCntlrStart(struct WatchdogCntlr *cntlr);

int32_t WatchdogCntlrStop(struct WatchdogCntlr *cntlr);

int32_t WatchdogCntlrSetTimeout(struct WatchdogCntlr *cntlr, uint32_t seconds);

int32_t WatchdogCntlrGetTimeout(struct WatchdogCntlr *cntlr, uint32_t *seconds);

int32_t WatchdogCntlrFeed(struct WatchdogCntlr *cntlr);

int32_t WatchdogGetPrivData(struct WatchdogCntlr *cntlr);

int32_t WatchdogReleasePriv(struct WatchdogCntlr *cntlr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* WATCHDOG_CORE_H */
