/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PCIE_CORE_H
#define PCIE_CORE_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_mutex.h"
#include "platform_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct PcieCntlr;

struct PcieCntlrOps {
    int32_t (*read)(struct PcieCntlr *cntlr, uint32_t pos, uint8_t *data, uint32_t len);
    int32_t (*write)(struct PcieCntlr *cntlr, uint32_t pos, uint8_t *data, uint32_t len);
};

struct PcieDevCfgInfo {
    uint32_t busNum;
    uint32_t vendorId;
    uint32_t devId;
};

struct PcieCntlr {
    struct IDeviceIoService service;
    struct HdfDeviceObject *hdfDevObj;
    struct PlatformDevice device;
    struct OsalMutex mutex;
    struct PcieCntlrOps *ops;
    struct PcieDevCfgInfo devInfo;
    void *priv;
};

static inline void PcieCntlrLock(struct PcieCntlr *cntlr)
{
    if (cntlr != NULL) {
        (void)OsalMutexLock(&cntlr->mutex);
    }
}

static inline void PcieCntlrUnlock(struct PcieCntlr *cntlr)
{
    if (cntlr != NULL) {
        (void)OsalMutexUnlock(&cntlr->mutex);
    }
}

static inline struct PcieCntlr *PcieCntlrGetByBusNum(uint16_t num)
{
    struct PlatformDevice *device = PlatformManagerGetDeviceByNumber(PlatformManagerGet(PLATFORM_MODULE_PCIE), num);

    if (device == NULL) {
        return NULL;
    }
    return CONTAINER_OF(device, struct PcieCntlr, device);
}

int32_t PcieCntlrParse(struct PcieCntlr *cntlr, struct HdfDeviceObject *obj);
int32_t PcieCntlrAdd(struct PcieCntlr *cntlr);
void PcieCntlrRemove(struct PcieCntlr *cntlr);

int32_t PcieCntlrRead(struct PcieCntlr *cntlr, uint32_t pos, uint8_t *data, uint32_t len);
int32_t PcieCntlrWrite(struct PcieCntlr *cntlr, uint32_t pos, uint8_t *data, uint32_t len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* PCIE_CORE_H */
