/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef GPIO_CORE_H
#define GPIO_CORE_H

#include "gpio_if.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_dlist.h"
#include "osal_mem.h"
#include "osal_spinlock.h"
#include "platform_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct GpioCntlr;
struct GpioMethod;
struct GpioInfo;
struct GpioIrqRecord;

#define GPIO_NUM_MAX     0xFFFF
#define GPIO_NAME_LEN    16

/**
 * @brief Defines the struct which represent a hardware GPIO controller.
 *
 * @since 1.0
 */
struct GpioCntlr {
    struct PlatformDevice device;
    struct GpioMethod *ops;
    uint16_t start;
    uint16_t count;
    struct GpioInfo *ginfos;
    void *priv;
};

/**
 * @brief Defines the struct which represent a hardware GPIO pin.
 *
 * @since 1.0
 */
struct GpioInfo {
    struct GpioCntlr *cntlr;
    char name[GPIO_NAME_LEN];
    OsalSpinlock spin;
    uint32_t irqSave;
    struct GpioIrqRecord *irqRecord;
};

static inline uint16_t GpioInfoToLocal(struct GpioInfo *ginfo)
{
    return ginfo - &(ginfo->cntlr->ginfos[0]);
}

static inline uint16_t GpioInfoToGlobal(struct GpioInfo *ginfo)
{
    return ginfo->cntlr->start + GpioInfoToLocal(ginfo);
}

/**
 * @brief Defines the struct which represent a GPIO irq action.
 *
 * @since 1.0
 */
struct GpioIrqRecord {
    uint16_t mode;
    GpioIrqFunc irqFunc;
    GpioIrqFunc btmFunc;
    void *irqData;
    uint16_t global;
    OsalSpinlock spin;
    struct OsalSem sem;
    struct OsalThread thread;
    bool removed;
};

static inline void GpioIrqRecordTrigger(struct GpioIrqRecord *irqRecord)
{
    if (irqRecord->irqFunc != NULL) {
        (void)irqRecord->irqFunc(irqRecord->global, irqRecord->irqData);
    }
    if (irqRecord->btmFunc != NULL) {
        (void)OsalSemPost(&irqRecord->sem);
    }
}

static inline void GpioIrqRecordDestroy(struct GpioIrqRecord *irqRecord)
{
    uint32_t irqSave;

    if (irqRecord->btmFunc == NULL) {
        OsalMemFree(irqRecord);  // the last access to this record
    } else {
        (void)OsalSpinLockIrqSave(&irqRecord->spin, &irqSave);
        irqRecord->removed = true;
        (void)OsalSemPost(&irqRecord->sem); // this is the last post ...
        (void)OsalSpinUnlockIrqRestore(&irqRecord->spin, &irqSave);
    }
}

/**
 * @brief Defines the struct which contains the hooks which a GPIO driver need to implement.
 *
 * @since 1.0
 */
struct GpioMethod {
    /** request exclusive access to an GPIO pin, optional */
    int32_t (*request)(struct GpioCntlr *cntlr, uint16_t local);
    /** release exclusive access to an GPIO pin, optional */
    int32_t (*release)(struct GpioCntlr *cntlr, uint16_t local);
    /** write the level value into a GPIO pin */
    int32_t (*write)(struct GpioCntlr *cntlr, uint16_t local, uint16_t val);
    /** read the level value of a GPIO pin */
    int32_t (*read)(struct GpioCntlr *cntlr, uint16_t local, uint16_t *val);
    /** set the direction for a GPIO pin */
    int32_t (*setDir)(struct GpioCntlr *cntlr, uint16_t local, uint16_t dir);
    /** get the direction of a GPIO pin */
    int32_t (*getDir)(struct GpioCntlr *cntlr, uint16_t local, uint16_t *dir);
    /** get the irq number of a GPIO pin, optional */
    int32_t (*toIrq)(struct GpioCntlr *cntlr, uint16_t local, uint16_t *irq);
    /** set the ISR function for a GPIO pin */
    int32_t (*setIrq)(struct GpioCntlr *cntlr, uint16_t local, uint16_t mode);
    /** unset the ISR function for a GPIO pin */
    int32_t (*unsetIrq)(struct GpioCntlr *cntlr, uint16_t local);
    /** enable a GPIO pin interrupt */
    int32_t (*enableIrq)(struct GpioCntlr *cntlr, uint16_t local);
    /** disable a GPIO pin interrupt */
    int32_t (*disableIrq)(struct GpioCntlr *cntlr, uint16_t local);
};

/**
 * @brief Add the Gpio Controller to HDF, and do some checking.
 *
 * @param cntlr Indicates the GPIO controller device.
 *
 * @return Returns 0 on success; returns a negative value otherwise.
 * @since 1.0
 */
int32_t GpioCntlrAdd(struct GpioCntlr *cntlr);

/**
 * @brief Remove the Gpio Controller from HDF.
 *
 * @param cntlr Indicates the I2C controller device.
 *
 * @since 1.0
 */
void GpioCntlrRemove(struct GpioCntlr *cntlr);

/**
 * @brief Turn HdfDeviceObject to an GpioCntlr.
 *
 * @param device Indicates a HdfDeviceObject.
 *
 * @return Retrns the pointer of the GpioCntlr on success; returns NULL otherwise.
 * @since 1.0
 */
static inline struct GpioCntlr *GpioCntlrFromHdfDev(struct HdfDeviceObject *device)
{
    return (struct GpioCntlr *)PlatformDeviceFromHdfDev(device);
}

int32_t GpioCntlrWrite(struct GpioCntlr *cntlr, uint16_t local, uint16_t val);

int32_t GpioCntlrRead(struct GpioCntlr *cntlr, uint16_t local, uint16_t *val);

int32_t GpioCntlrSetDir(struct GpioCntlr *cntlr, uint16_t local, uint16_t dir);

int32_t GpioCntlrGetDir(struct GpioCntlr *cntlr, uint16_t local, uint16_t *dir);

int32_t GpioCntlrToIrq(struct GpioCntlr *cntlr, uint16_t local, uint16_t *irq);

int32_t GpioCntlrSetIrq(struct GpioCntlr *cntlr, uint16_t local, uint16_t mode, GpioIrqFunc func, void *arg);

int32_t GpioCntlrUnsetIrq(struct GpioCntlr *cntlr, uint16_t local, void *arg);

int32_t GpioCntlrEnableIrq(struct GpioCntlr *cntlr, uint16_t local);

int32_t GpioCntlrDisableIrq(struct GpioCntlr *cntlr, uint16_t local);

void GpioCntlrIrqCallback(struct GpioCntlr *cntlr, uint16_t local);

struct PlatformManager *GpioManagerGet(void);

struct GpioCntlr *GpioCntlrGetByGpio(uint16_t gpio);

static inline void GpioCntlrPut(struct GpioCntlr *cntlr)
{
    if (cntlr != NULL) {
        PlatformDevicePut(&cntlr->device);
    }
}

static inline uint16_t GpioCntlrGetLocal(struct GpioCntlr *cntlr, uint16_t gpio)
{
    return (cntlr == NULL) ? gpio : (gpio - cntlr->start);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* GPIO_CORE_H */
