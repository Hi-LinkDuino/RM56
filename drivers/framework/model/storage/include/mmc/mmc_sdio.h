/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MMC_SDIO_H
#define MMC_SDIO_H

#include "mmc_sd.h"
#include "osal_thread.h"
#include "sdio_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct SdioFunction {
    struct SdioDevice *dev;
    uint32_t maxBlkSize;
    uint32_t curBlkSize;
    uint32_t funcNum;
    uint8_t funcClass;
    uint16_t vendorId;
    uint16_t deviceId;
    uint32_t timeOut;
    SdioIrqHandler *irqHandler;
};

struct SdioRegister {
    struct SdioCccr cccr;
    struct SdioCis cis;
};

struct SdioDevice {
    struct SdDevice sd;
    struct SdioDeviceOps *sdioOps;
    struct SdioRegister sdioReg;
    uint32_t functions;
    struct SdioFunction *sdioFunc[SDIO_MAX_FUNCTION_NUMBER];
    struct SdioFunction *curFunction;
    struct OsalThread thread;  /* irq thread */
    struct OsalSem sem;
    bool irqPending;
    bool threadRunning;
};

struct SdioDeviceOps {
    int32_t (*incrAddrReadBytes)(struct SdioDevice *, uint8_t *, uint32_t, uint32_t);
    int32_t (*incrAddrWriteBytes)(struct SdioDevice *, uint8_t *, uint32_t, uint32_t);
    int32_t (*fixedAddrReadBytes)(struct SdioDevice *, uint8_t *, uint32_t, uint32_t, uint32_t);
    int32_t (*fixedAddrWriteBytes)(struct SdioDevice *, uint8_t *, uint32_t, uint32_t, uint32_t);
    int32_t (*func0ReadBytes)(struct SdioDevice *, uint8_t *, uint32_t, uint32_t);
    int32_t (*func0WriteBytes)(struct SdioDevice *, uint8_t *, uint32_t, uint32_t);
    int32_t (*setBlockSize)(struct SdioDevice *, uint32_t);
    int32_t (*getCommonInfo)(struct SdioDevice *, SdioCommonInfo *, uint32_t);
    int32_t (*setCommonInfo)(struct SdioDevice *, SdioCommonInfo *, uint32_t);
    int32_t (*flushData)(struct SdioDevice *);
    int32_t (*enableFunc)(struct SdioDevice *);
    int32_t (*disableFunc)(struct SdioDevice *);
    int32_t (*claimIrq)(struct SdioDevice *, SdioIrqHandler *);
    int32_t (*releaseIrq)(struct SdioDevice *);
    int32_t (*findFunc)(struct SdioDevice *, struct SdioFunctionConfig *);
    int32_t (*claimHost)(struct SdioDevice *);
    int32_t (*releaseHost)(struct SdioDevice *);
};

int32_t SdioDeviceFindFunction(struct SdioDevice *sdio, struct SdioFunctionConfig *config);
int32_t SdioDeviceIncrAddrReadBytes(struct SdioDevice *sdio,
    uint8_t *data, uint32_t addr, uint32_t size);
int32_t SdioDeviceIncrAddrWriteBytes(struct SdioDevice *sdio,
    uint8_t *data, uint32_t addr, uint32_t size);
int32_t SdioDeviceFixedAddrReadBytes(struct SdioDevice *sdio,
    uint8_t *data, uint32_t addr, uint32_t size, uint32_t scatterLen);
int32_t SdioDeviceFixedAddrWriteBytes(struct SdioDevice *sdio,
    uint8_t *data, uint32_t addr, uint32_t size, uint32_t scatterLen);
int32_t SdioDeviceFunc0ReadBytes(struct SdioDevice *sdio,
    uint8_t *data, uint32_t addr, uint32_t size);
int32_t SdioDeviceFunc0WriteBytes(struct SdioDevice *sdio,
    uint8_t *data, uint32_t addr, uint32_t size);
int32_t SdioDeviceSetBlockSize(struct SdioDevice *sdio, uint32_t blockSize);
int32_t SdioDeviceGetCommonInfo(struct SdioDevice *sdio,
    SdioCommonInfo *info, SdioCommonInfoType infoType);
int32_t SdioDeviceSetCommonInfo(struct SdioDevice *sdio,
    SdioCommonInfo *info, SdioCommonInfoType infoType);
int32_t SdioDeviceFlushData(struct SdioDevice *sdio);
int32_t SdioDeviceClaimHost(struct SdioDevice *sdio);
int32_t SdioDeviceReleaseHost(struct SdioDevice *sdio);
int32_t SdioDeviceEnableFunc(struct SdioDevice *sdio);
int32_t SdioDeviceDisableFunc(struct SdioDevice *sdio);
int32_t SdioDeviceClaimIrq(struct SdioDevice *sdio, SdioIrqHandler *irqHandler);
int32_t SdioDeviceReleaseIrq(struct SdioDevice *sdio);

void SdioDeviceAddOps(struct SdioDevice *sdio, void *ops);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* MMC_SDIO_H */
