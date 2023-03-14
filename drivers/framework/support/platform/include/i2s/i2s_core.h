/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef I2S_CORE_H
#define I2S_CORE_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_dlist.h"
#include "i2s_if.h"
#include "osal_mutex.h"


struct I2sCntlr;

struct I2sCntlrMethod {
    int32_t (*GetCfg)(struct I2sCntlr *, struct I2sCfg *);
    int32_t (*SetCfg)(struct I2sCntlr *, struct I2sCfg *);
    int32_t (*Transfer)(struct I2sCntlr *, struct I2sMsg *);
    int32_t (*Open)(struct I2sCntlr *);
    int32_t (*Close)(struct I2sCntlr *);
    int32_t (*Enable)(struct I2sCntlr *);
    int32_t (*Disable)(struct I2sCntlr *);
    int32_t (*StartWrite)(struct I2sCntlr *);
    int32_t (*StopWrite)(struct I2sCntlr *);
    int32_t (*StartRead)(struct I2sCntlr *);
    int32_t (*StopRead)(struct I2sCntlr *);
};

struct I2sCntlr {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    uint32_t busNum;
    uint32_t irqNum;
    struct OsalMutex lock;
    struct I2sCntlrMethod *method;
    void *priv; // private data
};

/**
 * @brief Turn HdfDeviceObject to an I2sCntlr.
 *
 * @param device Indicates a HdfDeviceObject.
 *
 * @return Retrns the pointer of the I2sCntlr on success; returns NULL otherwise.
 * @since 1.0
 */
static inline struct I2sCntlr *I2sCntlrFromDevice(struct HdfDeviceObject *device)
{
    return (device == NULL) ? NULL : (struct I2sCntlr *)device->service;
}

struct I2sCntlr *I2sCntlrCreate(struct HdfDeviceObject *device);
void I2sCntlrDestroy(struct I2sCntlr *cntlr);
int32_t I2sCntlrStartRead(struct I2sCntlr *cntlr);
int32_t I2sCntlrStopRead(struct I2sCntlr *cntlr);
int32_t I2sCntlrStartWrite(struct I2sCntlr *cntlr);
int32_t I2sCntlrStopWrite(struct I2sCntlr *cntlr);
int32_t I2sCntlrOpen(struct I2sCntlr *cntlr);
int32_t I2sCntlrClose(struct I2sCntlr *cntlr);
int32_t I2sCntlrEnable(struct I2sCntlr *cntlr);
int32_t I2sCntlrDisable(struct I2sCntlr *cntlr);
int32_t I2sCntlrSetCfg(struct I2sCntlr *cntlr, struct I2sCfg *cfg);
int32_t I2sCntlrGetCfg(struct I2sCntlr *cntlr, struct I2sCfg *cfg);
int32_t I2sCntlrTransfer(struct I2sCntlr *cntlr, struct I2sMsg *msg);
#endif /* I2S_CORE_H */
