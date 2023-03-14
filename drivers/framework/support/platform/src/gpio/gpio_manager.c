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
#include "securec.h"

#define HDF_LOG_TAG gpio_manager

#define MAX_CNT_PER_CNTLR          1024

static int32_t GpioCntlrCheckStart(struct GpioCntlr *cntlr, struct DListHead *list)
{
    uint16_t freeStart;
    uint16_t freeCount;
    struct PlatformDevice *iterLast = NULL;
    struct PlatformDevice *iterCur = NULL;
    struct PlatformDevice *tmp = NULL;
    struct GpioCntlr *cntlrCur = NULL;
    struct GpioCntlr *cntlrLast = NULL;

    DLIST_FOR_EACH_ENTRY_SAFE(iterCur, tmp, list, struct PlatformDevice, node) {
        cntlrCur = CONTAINER_OF(iterCur, struct GpioCntlr, device);
        cntlrLast = CONTAINER_OF(iterLast, struct GpioCntlr, device);
        if (cntlrLast == NULL) {
            freeStart = 0;
            freeCount = cntlrCur->start;
        } else {
            freeStart = cntlrLast->start + cntlrLast->count;
            freeCount = cntlrCur->start - freeStart;
        }

        if (cntlr->start < freeStart) {
            PLAT_LOGE("GpioCntlrCheckStart: start:%hu not available(freeStart:%hu, freeCount:%hu)",
                cntlr->start, freeStart, freeCount);
            return HDF_PLT_RSC_NOT_AVL;
        }

        if ((cntlr->start + cntlr->count) <= (freeStart + freeCount)) {
            return HDF_SUCCESS;
        }
        cntlrLast = cntlrCur;
    }
    if (cntlrLast == NULL) { // empty list
        return HDF_SUCCESS;
    }
    if (cntlr->start >= (cntlrLast->start + cntlrLast->count)) {
        return HDF_SUCCESS;
    }
    PLAT_LOGE("GpioCntlrCheckStart: start:%u(%u) not available(lastStart:%u, lastCount:%u)",
        cntlr->start, cntlr->count, cntlrLast->start, cntlrLast->count);
    return GPIO_NUM_MAX;
}

static int32_t GpioManagerAdd(struct PlatformManager *manager, struct PlatformDevice *device)
{
    int32_t ret;
    struct GpioCntlr *cntlr = CONTAINER_OF(device, struct GpioCntlr, device);

    ret = GpioCntlrCheckStart(cntlr, &manager->devices);
    if (ret != HDF_SUCCESS) {
        PLAT_LOGE("GpioManagerAdd: start:%u(%u) invalid:%d", cntlr->start, cntlr->count, ret);
        return HDF_ERR_INVALID_PARAM;
    }

    DListInsertTail(&device->node, &manager->devices);
    PLAT_LOGI("GpioManagerAdd: start:%u count:%u added success", cntlr->start, cntlr->count);
    return HDF_SUCCESS;
}

static int32_t GpioManagerDel(struct PlatformManager *manager, struct PlatformDevice *device)
{
    if (!DListIsEmpty(&device->node)) {
        DListRemove(&device->node);
    }
    return HDF_SUCCESS;
}

struct PlatformManager *GpioManagerGet(void)
{
    static struct PlatformManager *manager = NULL;

    if (manager == NULL) {
        manager = PlatformManagerGet(PLATFORM_MODULE_GPIO);
        if (manager != NULL) {
            manager->add = GpioManagerAdd;
            manager->del = GpioManagerDel;
        }
    }
    return manager;
}

static int32_t GpioCntlrCreateGpioInfos(struct GpioCntlr *cntlr)
{
    int32_t ret;
    uint16_t i;

    cntlr->ginfos = OsalMemCalloc(sizeof(*cntlr->ginfos) * cntlr->count);
    if (cntlr->ginfos == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }
    for (i = 0; i < cntlr->count; i++) {
        cntlr->ginfos[i].cntlr = cntlr;
        if (snprintf_s(cntlr->ginfos[i].name, GPIO_NAME_LEN, GPIO_NAME_LEN - 1,
            "GPIO%u+%u", cntlr->start, i) < 0) {
            PLAT_LOGE("GpioCntlrCreateGpioInfos: format gpio name fail");
            ret = HDF_PLT_ERR_OS_API;
            goto ERROR_EXIT;
        }
        (void)OsalSpinInit(&cntlr->ginfos[i].spin);
    }
    return HDF_SUCCESS;

ERROR_EXIT:
    while (i-- > 0) {
        (void)OsalSpinDestroy(&cntlr->ginfos[i].spin);
    }
    OsalMemFree(cntlr->ginfos);
    return ret;
}

static void GpioCntlrDestroyGpioInfos(struct GpioCntlr *cntlr)
{
    uint16_t i;
    struct GpioIrqRecord *irqRecord = NULL;

    for (i = 0; i < cntlr->count; i++) {
        irqRecord = cntlr->ginfos[i].irqRecord;
        if (irqRecord != NULL) {
            GpioIrqRecordDestroy(irqRecord);
        }
        (void)OsalSpinDestroy(&cntlr->ginfos[i].spin);
    }
    OsalMemFree(cntlr->ginfos);
    cntlr->ginfos = NULL;
}

int32_t GpioCntlrAdd(struct GpioCntlr *cntlr)
{
    int32_t ret;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->ops == NULL) {
        PLAT_LOGE("GpioCntlrAdd: no ops supplied!");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->count == 0 || cntlr->count >= MAX_CNT_PER_CNTLR) {
        PLAT_LOGE("GpioCntlrAdd: invalid gpio count:%u", cntlr->count);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = GpioCntlrCreateGpioInfos(cntlr);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    cntlr->device.manager = GpioManagerGet();
    ret = PlatformDeviceAdd(&cntlr->device);
    if (ret != HDF_SUCCESS) {
        GpioCntlrDestroyGpioInfos(cntlr);
        return ret;
    }

    return HDF_SUCCESS;
}

void GpioCntlrRemove(struct GpioCntlr *cntlr)
{
    if (cntlr == NULL) {
        return;
    }

    PlatformDeviceDel(&cntlr->device);

    if (cntlr->ginfos == NULL) {
        return;
    }

    GpioCntlrDestroyGpioInfos(cntlr);
}

static bool GpioCntlrFindMatch(struct PlatformDevice *device, void *data)
{
    uint16_t gpio = (uint16_t)(uintptr_t)data;
    struct GpioCntlr *cntlr = CONTAINER_OF(device, struct GpioCntlr, device);

    if (gpio >= cntlr->start && gpio < (cntlr->start + cntlr->count)) {
        return true;
    }
    return false;
}

struct GpioCntlr *GpioCntlrGetByGpio(uint16_t gpio)
{
    struct PlatformManager *gpioMgr = NULL;
    struct PlatformDevice *device = NULL;

    gpioMgr = GpioManagerGet();
    if (gpioMgr == NULL) {
        PLAT_LOGE("GpioCntlrGetByGpio: get gpio manager failed");
        return NULL;
    }

    device = PlatformManagerFindDevice(gpioMgr, (void *)(uintptr_t)gpio, GpioCntlrFindMatch);
    if (device == NULL) {
        PLAT_LOGE("GpioCntlrGetByGpio: gpio %u not in any controllers!", gpio);
        return NULL;
    }
    return CONTAINER_OF(device, struct GpioCntlr, device);
}
