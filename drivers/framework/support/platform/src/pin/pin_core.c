/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "pin_core.h"
#include "hdf_log.h"

#define HDF_LOG_TAG pin_core

#define MAX_CNT_PER_CNTLR    20

static struct DListHead g_cntlrListHead;
static OsalSpinlock g_listLock;
static uint32_t g_irqSave;

static struct DListHead *PinCntlrListGet(void)
{
    static struct DListHead *head = NULL;
    uint32_t irqSave;
    if (head == NULL) {
        head = &g_cntlrListHead;
        DListHeadInit(head);
        OsalSpinInit(&g_listLock);
    }
    while (OsalSpinLockIrqSave(&g_listLock, &irqSave) != HDF_SUCCESS);
    g_irqSave = irqSave;
    return head;
}

static void PinCntlrListPut(void)
{
    (void)OsalSpinUnlockIrqRestore(&g_listLock, &g_irqSave);
}

int32_t PinCntlrAdd(struct PinCntlr *cntlr)
{
    struct DListHead *head = NULL;

    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid object cntlr is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    DListHeadInit(&cntlr->node);

    if (cntlr->method == NULL) {
        HDF_LOGE("%s: no method supplied!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->pinCount >= MAX_CNT_PER_CNTLR) {
        HDF_LOGE("%s: invalid pinCount:%u", __func__, cntlr->pinCount);
        return HDF_ERR_INVALID_PARAM;
    }

    OsalSpinInit(&cntlr->spin);

    head = PinCntlrListGet();
    DListInsertTail(&cntlr->node, head);
    PinCntlrListPut();
    return HDF_SUCCESS;
}

void PinCntlrRemove(struct PinCntlr *cntlr)
{
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL!", __func__);
        return;
    }

    (void)PinCntlrListGet();
    DListRemove(&cntlr->node);
    PinCntlrListPut();
    (void)OsalSpinDestroy(&cntlr->spin);
}

struct PinDesc *PinCntlrGetPinDescByName(const char *pinName)
{
    struct DListHead *head = NULL;
    struct PinCntlr *cntlr = NULL;
    struct PinCntlr *tmp = NULL;
    uint16_t num;

    if (pinName == NULL) {
        HDF_LOGE("%s: pinName is NULL!", __func__);
        return NULL;
    }

    head = PinCntlrListGet();

    DLIST_FOR_EACH_ENTRY_SAFE(cntlr, tmp, head, struct PinCntlr, node) {
        for (num = 0; num < cntlr->pinCount; num++) {
            if (cntlr->pins[num].pinName == NULL) {
                continue;
            }
            if (!strcmp(cntlr->pins[num].pinName, pinName)) {
                PinCntlrListPut();
                HDF_LOGI("%s: cntlr->pins[%d].pinName is %s!", __func__, num, cntlr->pins[num].pinName);
                return &cntlr->pins[num];
            }
        }
    }
    PinCntlrListPut();
    HDF_LOGE("%s: pinName:%s doesn't matching!", __func__, pinName);
    return NULL;
}

struct PinCntlr *PinCntlrGetByNumber(uint16_t number)
{
    struct DListHead *head = NULL;
    struct PinCntlr *cntlr = NULL;
    struct PinCntlr *tmp = NULL;

    head = PinCntlrListGet();

    DLIST_FOR_EACH_ENTRY_SAFE(cntlr, tmp, head, struct PinCntlr, node) {
        if (cntlr->number == number) {
            PinCntlrListPut();
            HDF_LOGI("%s: get cntlr by number success!", __func__);
            return cntlr;
        }
    }
    PinCntlrListPut();
    HDF_LOGE("%s: get cntlr by number error!", __func__);
    return NULL;
}

struct PinCntlr *PinCntlrGetByPin(struct PinDesc *desc)
{
    struct DListHead *head = NULL;
    struct PinCntlr *cntlr = NULL;
    struct PinCntlr *tmp = NULL;
    int32_t num;

    head = PinCntlrListGet();

    DLIST_FOR_EACH_ENTRY_SAFE(cntlr, tmp, head, struct PinCntlr, node) {
        for (num = 0; num <cntlr->pinCount; num++) {
            if (desc == &cntlr->pins[num]) {
            PinCntlrListPut();
            HDF_LOGI("%s: get cntlr by desc success!", __func__);
            return cntlr;
            }
        }
    }
    PinCntlrListPut();
    HDF_LOGE("%s: pinCtrl:%s not in any controllers!", __func__, desc->pinName);
    return NULL;
}

static int32_t GetPinIndex(struct PinCntlr *cntlr, struct PinDesc *desc)
{
    uint16_t index;
    int32_t ret;

    for (index = 0; index < cntlr->pinCount; index++) {
        if (cntlr->pins[index].pinName == NULL) {
            HDF_LOGE("%s: cntlr->pin[index].pinName is NULL!", __func__);
            break;
        }
        ret = strcmp(cntlr->pins[index].pinName, desc->pinName);
        if (ret == 0) {
            HDF_LOGI("%s: get pin index:%hu success!", __func__, index);
            return (int32_t)index;
        }
    }
    HDF_LOGE("%s:  get pin index failed!", __func__);
    return HDF_ERR_INVALID_PARAM;
}

void PinCntlrPutPin(struct PinDesc *desc)
{
    (void)desc;
}

int32_t PinCntlrSetPinPull(struct PinCntlr *cntlr, struct PinDesc *desc, enum PinPullType pullType)
{
    int32_t ret;
    uint32_t index;
    uint32_t irqSave;

    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid object cntlr is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->method == NULL || cntlr->method->SetPinPull == NULL) {
        HDF_LOGE("%s: method or SetPinPull is NULL", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (desc == NULL) {
        HDF_LOGE("%s: desc is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    index = (uint32_t)GetPinIndex(cntlr, desc);
    if (index < HDF_SUCCESS) {
        HDF_LOGE("%s: get pin index fail!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    (void)OsalSpinLockIrqSave(&cntlr->spin, &irqSave);
    ret = cntlr->method->SetPinPull(cntlr, index, pullType);
    (void)OsalSpinUnlockIrqRestore(&cntlr->spin, &irqSave);
    return ret;
}

int32_t PinCntlrGetPinPull(struct PinCntlr *cntlr, struct PinDesc *desc, enum PinPullType *pullType)
{
    int32_t ret;
    uint32_t index;
    uint32_t irqSave;
    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid object cntlr is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->method == NULL || cntlr->method->GetPinPull == NULL) {
        HDF_LOGE("%s: method or GetPinPull is NULL", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (desc == NULL) {
        HDF_LOGE("%s: desc is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (pullType == NULL) {
        HDF_LOGE("%s: pullType is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    index = (uint32_t)GetPinIndex(cntlr, desc);
    if (index < HDF_SUCCESS) {
        HDF_LOGE("%s: get pin index failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    (void)OsalSpinLockIrqSave(&cntlr->spin, &irqSave);
    ret = cntlr->method->GetPinPull(cntlr, index, pullType);
    (void)OsalSpinUnlockIrqRestore(&cntlr->spin, &irqSave);

    return ret;
}

int32_t PinCntlrSetPinStrength(struct PinCntlr *cntlr, struct PinDesc *desc, uint32_t strength)
{
    int32_t ret;
    uint32_t index;
    uint32_t irqSave;

    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid object cntlr is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->method == NULL || cntlr->method->SetPinStrength == NULL) {
        HDF_LOGE("%s: method or SetStrength is NULL", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (desc == NULL) {
        HDF_LOGE("%s: desc is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    index = (uint32_t)GetPinIndex(cntlr, desc);
    if (index < HDF_SUCCESS) {
        HDF_LOGE("%s: get pin index fail!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    (void)OsalSpinLockIrqSave(&cntlr->spin, &irqSave);
    ret = cntlr->method->SetPinStrength(cntlr, index, strength);
    (void)OsalSpinUnlockIrqRestore(&cntlr->spin, &irqSave);
    return ret;
}

int32_t PinCntlrGetPinStrength(struct PinCntlr *cntlr, struct PinDesc *desc, uint32_t *strength)
{
    int32_t ret;
    uint32_t index;
    uint32_t irqSave;
    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid object cntlr is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->method == NULL || cntlr->method->GetPinStrength == NULL) {
        HDF_LOGE("%s: method or GetStrength is NULL", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (desc == NULL) {
        HDF_LOGE("%s: desc is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (strength == NULL) {
        HDF_LOGE("%s: strength is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    index = (uint32_t)GetPinIndex(cntlr, desc);
    if (index < HDF_SUCCESS) {
        HDF_LOGE("%s: get pin index failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    (void)OsalSpinLockIrqSave(&cntlr->spin, &irqSave);
    ret = cntlr->method->GetPinStrength(cntlr, index, strength);
    (void)OsalSpinUnlockIrqRestore(&cntlr->spin, &irqSave);

    return ret;
}

int32_t PinCntlrSetPinFunc(struct PinCntlr *cntlr, struct PinDesc *desc, const char *funcName)
{
    int32_t ret;
    uint32_t index;
    uint32_t irqSave;

    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid object cntlr is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->method == NULL || cntlr->method->SetPinFunc == NULL) {
        HDF_LOGE("%s: method or SetPinFunc is NULL", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (desc == NULL) {
        HDF_LOGE("%s: desc is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    index = (uint32_t)GetPinIndex(cntlr, desc);
    if (index < HDF_SUCCESS) {
        HDF_LOGE("%s: get pin index failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (funcName == NULL) {
        HDF_LOGE("%s: invalid funcName pointer", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    (void)OsalSpinLockIrqSave(&cntlr->spin, &irqSave);
    ret = cntlr->method->SetPinFunc(cntlr, index, funcName);
    (void)OsalSpinUnlockIrqRestore(&cntlr->spin, &irqSave);
    return ret;
}

int32_t PinCntlrGetPinFunc(struct PinCntlr *cntlr, struct PinDesc *desc, const char **funcName)
{
    int32_t ret;
    uint32_t index;
    uint32_t irqSave;

    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid object cntlr is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->method == NULL || cntlr->method->GetPinFunc == NULL) {
        HDF_LOGE("%s: method or SetPinFunc is NULL", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (desc == NULL) {
        HDF_LOGE("%s: desc is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    index = (uint32_t)GetPinIndex(cntlr, desc);
    if (index < HDF_SUCCESS) {
        HDF_LOGE("%s: get pin index failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    (void)OsalSpinLockIrqSave(&cntlr->spin, &irqSave);
    ret = cntlr->method->GetPinFunc(cntlr, index, funcName);
    (void)OsalSpinUnlockIrqRestore(&cntlr->spin, &irqSave);
    return ret;
}
