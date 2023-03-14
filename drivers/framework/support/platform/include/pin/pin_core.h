/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#ifndef PIN_CORE_H
#define PIN_CORE_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_dlist.h"
#include "pin_if.h"
#include "osal_spinlock.h"
#include "osal_atomic.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct PinCntlr;
struct PinCntlrMethod;
struct PinDesc;

struct PinDesc {
    const char *pinName;
    void *priv;
};

struct PinCntlr {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    struct PinCntlrMethod *method;
    struct DListHead node;
    OsalSpinlock spin;
    uint16_t number;
    uint16_t pinCount;
    struct PinDesc *pins;
    void *priv;
};

struct PinCntlrMethod {
    int32_t (*SetPinPull)(struct PinCntlr *cntlr, uint32_t index, enum PinPullType pullType);
    int32_t (*GetPinPull)(struct PinCntlr *cntlr, uint32_t index, enum PinPullType *pullType);
    int32_t (*SetPinStrength)(struct PinCntlr *cntlr, uint32_t index, uint32_t strength);
    int32_t (*GetPinStrength)(struct PinCntlr *cntlr, uint32_t index, uint32_t *strength);
    int32_t (*SetPinFunc)(struct PinCntlr *cntlr, uint32_t index, const char *funcName);
    int32_t (*GetPinFunc)(struct PinCntlr *cntlr, uint32_t index, const char **funcName);
};

int32_t PinCntlrAdd(struct PinCntlr *cntlr);

void PinCntlrRemove(struct PinCntlr *cntlr);

struct PinDesc *PinCntlrGetPinDescByName(const char *pinName);

struct PinCntlr *PinCntlrGetByNumber(uint16_t number);

struct PinCntlr *PinCntlrGetByPin(struct PinDesc *desc);

void PinCntlrPutPin(struct PinDesc *desc);

int32_t PinCntlrSetPinPull(struct PinCntlr *cntlr, struct PinDesc *desc, enum PinPullType pullType);

int32_t PinCntlrGetPinPull(struct PinCntlr *cntlr, struct PinDesc *desc, enum PinPullType *pullType);

int32_t PinCntlrSetPinStrength(struct PinCntlr *cntlr, struct PinDesc *desc, uint32_t strength);

int32_t PinCntlrGetPinStrength(struct PinCntlr *cntlr, struct PinDesc *desc, uint32_t *strength);

int32_t PinCntlrSetPinFunc(struct PinCntlr *cntlr, struct PinDesc *desc, const char *funcName);

int32_t PinCntlrGetPinFunc(struct PinCntlr *cntlr, struct PinDesc *desc, const char **funcName);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* PIN_CORE_H */
