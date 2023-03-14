/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SPI_CORE_H
#define SPI_CORE_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_dlist.h"
#include "spi_if.h"
#include "osal_mutex.h"

struct SpiCntlr;
struct SpiCntlrMethod;

struct SpiCntlrMethod {
    int32_t (*GetCfg)(struct SpiCntlr *, struct SpiCfg *);
    int32_t (*SetCfg)(struct SpiCntlr *, struct SpiCfg *);
    int32_t (*Transfer)(struct SpiCntlr *, struct SpiMsg *, uint32_t);
    int32_t (*Open)(struct SpiCntlr *);
    int32_t (*Close)(struct SpiCntlr *);
};

struct SpiCntlr {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    uint32_t busNum;
    uint32_t numCs;
    uint32_t curCs;
    struct OsalMutex lock;
    struct SpiCntlrMethod *method;
    struct DListHead list;
    void *priv;
};

struct SpiDev {
    struct SpiCntlr *cntlr;
    struct DListHead list;
    struct SpiCfg cfg;
    uint32_t csNum;
    void *priv;
};

struct SpiCntlr *SpiCntlrCreate(struct HdfDeviceObject *device);
void SpiCntlrDestroy(struct SpiCntlr *cntlr);

/**
 * @brief Turn SpiCntlr to a HdfDeviceObject.
 *
 * @param cntlr Indicates the SPI cntlr device.
 *
 * @return Retrns the pointer of the HdfDeviceObject on success; returns NULL otherwise.
 * @since 1.0
 */
static inline struct HdfDeviceObject *SpiCntlrToDevice(struct SpiCntlr *cntlr)
{
    return (cntlr == NULL) ? NULL : cntlr->device;
}

/**
 * @brief Turn HdfDeviceObject to an SpiCntlr.
 *
 * @param device Indicates a HdfDeviceObject.
 *
 * @return Retrns the pointer of the SpiCntlr on success; returns NULL otherwise.
 * @since 1.0
 */
static inline struct SpiCntlr *SpiCntlrFromDevice(struct HdfDeviceObject *device)
{
    return (device == NULL) ? NULL : (struct SpiCntlr *)device->service;
}

int32_t SpiCntlrTransfer(struct SpiCntlr *, uint32_t, struct SpiMsg *, uint32_t);
int32_t SpiCntlrSetCfg(struct SpiCntlr *, uint32_t, struct SpiCfg *);
int32_t SpiCntlrGetCfg(struct SpiCntlr *, uint32_t, struct SpiCfg *);
int32_t SpiCntlrOpen(struct SpiCntlr *, uint32_t);
int32_t SpiCntlrClose(struct SpiCntlr *, uint32_t);

#endif /* SPI_CORE_H */
