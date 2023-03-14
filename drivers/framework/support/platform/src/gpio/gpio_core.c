/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "gpio/gpio_core.h"
#include "osal_mem.h"
#include "platform_core.h"

#define HDF_LOG_TAG gpio_core

#define GPIO_IRQ_STACK_SIZE        10000

static inline void GpioInfoLock(struct GpioInfo *ginfo)
{
    (void)OsalSpinLockIrqSave(&ginfo->spin, &ginfo->irqSave);
}

static inline void GpioInfoUnlock(struct GpioInfo *ginfo)
{
    (void)OsalSpinUnlockIrqRestore(&ginfo->spin, &ginfo->irqSave);
}

int32_t GpioCntlrWrite(struct GpioCntlr *cntlr, uint16_t local, uint16_t val)
{
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops == NULL || cntlr->ops->write == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    return cntlr->ops->write(cntlr, local, val);
}

int32_t GpioCntlrRead(struct GpioCntlr *cntlr, uint16_t local, uint16_t *val)
{
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops == NULL || cntlr->ops->read == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    if (val == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    return cntlr->ops->read(cntlr, local, val);
}

int32_t GpioCntlrSetDir(struct GpioCntlr *cntlr, uint16_t local, uint16_t dir)
{
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops == NULL || cntlr->ops->setDir == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    return cntlr->ops->setDir(cntlr, local, dir);
}

int32_t GpioCntlrGetDir(struct GpioCntlr *cntlr, uint16_t local, uint16_t *dir)
{
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops == NULL || cntlr->ops->getDir == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    if (dir == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    return cntlr->ops->getDir(cntlr, local, dir);
}

int32_t GpioCntlrToIrq(struct GpioCntlr *cntlr, uint16_t local, uint16_t *irq)
{
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops == NULL || cntlr->ops->toIrq == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    return cntlr->ops->toIrq(cntlr, local, irq);
}

void GpioCntlrIrqCallback(struct GpioCntlr *cntlr, uint16_t local)
{
    struct GpioInfo *ginfo = NULL;
    struct GpioIrqRecord *irqRecord = NULL;

    if (cntlr == NULL || cntlr->ginfos == NULL || local >= cntlr->count) {
        PLAT_LOGW("GpioCntlrIrqCallback: invalid cntlr(ginfos) or loal num:%hu!", local);
        return;
    }
    ginfo = &cntlr->ginfos[local];
    if (ginfo == NULL) {
        PLAT_LOGW("GpioCntlrIrqCallback: ginfo null(start:%u, local:%u)", cntlr->start, local);
        return;
    }

    GpioInfoLock(ginfo);
    irqRecord = ginfo->irqRecord;

    if (irqRecord == NULL) {
        PLAT_LOGW("GpioCntlrIrqCallback: irq not set (start:%u, local:%u)", cntlr->start, local);
        GpioInfoUnlock(ginfo);
        return;
    }
    GpioIrqRecordTrigger(irqRecord);
    GpioInfoUnlock(ginfo);
}

static int32_t GpioCntlrIrqThreadHandler(void *data)
{
    int32_t ret;
    uint32_t irqSave;
    struct GpioIrqRecord *irqRecord = (struct GpioIrqRecord *)data;

    while (true) {
        ret = OsalSemWait(&irqRecord->sem, HDF_WAIT_FOREVER);
        if (irqRecord->removed) {
            break;
        }
        if (ret != HDF_SUCCESS) {
            continue;
        }
        if (irqRecord->btmFunc != NULL) {
            (void)irqRecord->btmFunc(irqRecord->global, irqRecord->irqData);
        }
    }
    (void)OsalSpinLockIrqSave(&irqRecord->spin, &irqSave);
    (void)OsalSpinUnlockIrqRestore(&irqRecord->spin, &irqSave); // last post done
    PLAT_LOGI("GpioCntlrIrqThreadHandler: gpio(%u) thread exit", irqRecord->global);
    (void)OsalSemDestroy(&irqRecord->sem);
    (void)OsalSpinDestroy(&irqRecord->spin);
    (void)OsalThreadDestroy(&irqRecord->thread);
    OsalMemFree(irqRecord);
    return HDF_SUCCESS;
}

static int32_t GpioCntlrSetIrqInner(struct GpioInfo *ginfo, struct GpioIrqRecord *irqRecord)
{
    int32_t ret;
    uint16_t local = GpioInfoToLocal(ginfo);
    struct GpioCntlr *cntlr = ginfo->cntlr;

    if (cntlr == NULL) {
        PLAT_LOGE("GpioCntlrSetIrqInner: cntlr is NULL");
        return HDF_ERR_INVALID_PARAM;
    }
    GpioInfoLock(ginfo);

    if (ginfo->irqRecord != NULL) {
        GpioInfoUnlock(ginfo);
        PLAT_LOGE("GpioCntlrSetIrqInner: gpio(%u+%u) irq already set", cntlr->start, local);
        return HDF_ERR_NOT_SUPPORT;
    }

    ginfo->irqRecord = irqRecord;
    ret = cntlr->ops->setIrq(cntlr, local, irqRecord->mode);
    if (ret != HDF_SUCCESS) {
        ginfo->irqRecord = NULL;
    }

    GpioInfoUnlock(ginfo);
    return ret;
}

static int32_t GpioIrqRecordCreate(struct GpioInfo *ginfo, uint16_t mode, GpioIrqFunc func, void *arg,
    struct GpioIrqRecord **new)
{
    int32_t ret;
    struct GpioIrqRecord *irqRecord = NULL;
    struct OsalThreadParam cfg;

    irqRecord = (struct GpioIrqRecord *)OsalMemCalloc(sizeof(*irqRecord));
    if (irqRecord == NULL) {
        PLAT_LOGE("GpioIrqRecordCreate: alloc irq record failed");
        return HDF_ERR_MALLOC_FAIL;
    }
    irqRecord->removed = false;
    irqRecord->mode = mode;
    irqRecord->irqFunc = ((mode & GPIO_IRQ_USING_THREAD) == 0) ? func : NULL;
    irqRecord->btmFunc = ((mode & GPIO_IRQ_USING_THREAD) != 0) ? func : NULL;
    irqRecord->irqData = arg;
    irqRecord->global = GpioInfoToGlobal(ginfo);
    if (irqRecord->btmFunc != NULL) {
        ret = OsalThreadCreate(&irqRecord->thread, GpioCntlrIrqThreadHandler, irqRecord);
        if (ret != HDF_SUCCESS) {
            OsalMemFree(irqRecord);
            PLAT_LOGE("GpioIrqRecordCreate: create irq thread failed:%d", ret);
            return ret;
        }
        (void)OsalSpinInit(&irqRecord->spin);
        (void)OsalSemInit(&irqRecord->sem, 0);
        cfg.name = (char *)ginfo->name;
        cfg.priority = OSAL_THREAD_PRI_HIGHEST;
        cfg.stackSize = GPIO_IRQ_STACK_SIZE;
        ret = OsalThreadStart(&irqRecord->thread, &cfg);
        if (ret != HDF_SUCCESS) {
            (void)OsalSemDestroy(&irqRecord->sem);
            (void)OsalSpinDestroy(&irqRecord->spin);
            (void)OsalThreadDestroy(&irqRecord->thread);
            OsalMemFree(irqRecord);
            PLAT_LOGE("GpioIrqRecordCreate: start irq thread failed:%d", ret);
            return ret;
        }
        PLAT_LOGI("GpioIrqRecordCreate: gpio(%u) thread started", GpioInfoToGlobal(ginfo));
    }

    *new = irqRecord;
    return HDF_SUCCESS;
}

int32_t GpioCntlrSetIrq(struct GpioCntlr *cntlr, uint16_t local, uint16_t mode, GpioIrqFunc func, void *arg)
{
    int32_t ret;
    struct GpioInfo *ginfo = NULL;
    struct GpioIrqRecord *irqRecord = NULL;

    if (cntlr == NULL || cntlr->ginfos == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (local >= cntlr->count || func == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->ops == NULL || cntlr->ops->setIrq == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    ginfo = &cntlr->ginfos[local];
    ret = GpioIrqRecordCreate(ginfo, mode, func, arg, &irqRecord);
    if (ret != HDF_SUCCESS) {
        PLAT_LOGE("GpioCntlrSetIrq: create irq record failed:%d", ret);
        return ret;
    }

    ret = GpioCntlrSetIrqInner(ginfo, irqRecord);
    if (ret != HDF_SUCCESS) {
        GpioIrqRecordDestroy(irqRecord);
    }
    return ret;
}

int32_t GpioCntlrUnsetIrq(struct GpioCntlr *cntlr, uint16_t local, void *arg)
{
    int32_t ret;
    struct GpioInfo *ginfo = NULL;
    struct GpioIrqRecord *irqRecord = NULL;

    if (cntlr == NULL || cntlr->ginfos == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (local >= cntlr->count) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->ops == NULL || cntlr->ops->unsetIrq == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    ginfo = &cntlr->ginfos[local];
    GpioInfoLock(ginfo);
    if (ginfo->irqRecord == NULL) {
        GpioInfoUnlock(ginfo);
        PLAT_LOGE("GpioCntlrUnsetIrq: gpio(%u+%u) irq not set yet", cntlr->start, local);
        return HDF_ERR_NOT_SUPPORT;
    }
    irqRecord = ginfo->irqRecord;
    if (arg != irqRecord->irqData) {
        GpioInfoUnlock(ginfo);
        PLAT_LOGE("GpioCntlrUnsetIrq: gpio(%u+%u) arg not match", cntlr->start, local);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = cntlr->ops->unsetIrq(cntlr, local);
    if (ret == HDF_SUCCESS) {
        ginfo->irqRecord = NULL;
    }
    GpioInfoUnlock(ginfo);

    if (ret == HDF_SUCCESS) {
        GpioIrqRecordDestroy(irqRecord);
    }
    return ret;
}

int32_t GpioCntlrEnableIrq(struct GpioCntlr *cntlr, uint16_t local)
{
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops == NULL || cntlr->ops->enableIrq == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    return cntlr->ops->enableIrq(cntlr, local);
}

int32_t GpioCntlrDisableIrq(struct GpioCntlr *cntlr, uint16_t local)
{
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops == NULL || cntlr->ops->disableIrq == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    return cntlr->ops->disableIrq(cntlr, local);
}
