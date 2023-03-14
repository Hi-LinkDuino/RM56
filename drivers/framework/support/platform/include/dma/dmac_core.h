/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef DMAC_CORE_H
#define DMAC_CORE_H

#include "hdf_base.h"
#include "hdf_device.h"
#include "hdf_device_desc.h"
#include "hdf_object.h"
#include "los_event.h"
#include "osal_mutex.h"
#include "osal_spinlock.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define PERIPH_ADDR_INVALID               0xfff
#define DMAC_CHAN_NUM_MAX                 100

#define DmaEventInit(event)               LOS_EventInit(event)
#define DmaEventSignal(event, bit)        LOS_EventWrite(event, bit)
#define DmaEventWait(event, bit, timeout) LOS_EventRead(event, bit, LOS_WAITMODE_OR + LOS_WAITMODE_CLR, timeout)

typedef void DmacCallback(void *callbackData, int status);
typedef EVENT_CB_S DmacEvent;

/* definition for the return value */
enum DmacErrorNumber {
    DMAC_ERROR_BASE                       = 0x100,
    DMAC_CHANNEL_INVALID                  = DMAC_ERROR_BASE + 1,
    DMAC_TRXFERSIZE_INVALID               = DMAC_ERROR_BASE + 2,
    DMAC_SOURCE_ADDRESS_INVALID           = DMAC_ERROR_BASE + 3,
    DMAC_DESTINATION_ADDRESS_INVALID      = DMAC_ERROR_BASE + 4,
    DMAC_MEMORY_ADDRESS_INVALID           = DMAC_ERROR_BASE + 5,
    DMAC_PERIPHERAL_ID_INVALID            = DMAC_ERROR_BASE + 6,
    DMAC_DIRECTION_ERROR                  = DMAC_ERROR_BASE + 7,
    DMAC_TRXFER_ERROR                     = DMAC_ERROR_BASE + 8,
    DMAC_LLIHEAD_ERROR                    = DMAC_ERROR_BASE + 9,
    DMAC_SWIDTH_ERROR                     = DMAC_ERROR_BASE + 0xa,
    DMAC_LLI_ADDRESS_INVALID              = DMAC_ERROR_BASE + 0xb,
    DMAC_TRANS_CONTROL_INVALID            = DMAC_ERROR_BASE + 0xc,
    DMAC_MEMORY_ALLOCATE_ERROR            = DMAC_ERROR_BASE + 0xd,
    DMAC_NOT_FINISHED                     = DMAC_ERROR_BASE + 0xe,
    DMAC_TIMEOUT                          = DMAC_ERROR_BASE + 0xf,
    DMAC_CHN_SUCCESS                      = DMAC_ERROR_BASE + 0x10,
    DMAC_CHN_ERROR                        = DMAC_ERROR_BASE + 0x11,
    DMAC_CHN_TIMEOUT                      = DMAC_ERROR_BASE + 0x12,
    DMAC_CHN_ALLOCAT                      = DMAC_ERROR_BASE + 0x13,
    DMAC_CHN_VACANCY                      = DMAC_ERROR_BASE + 0x14,
};

enum DmacEvent {
    DMAC_EVENT_DONE                       = 0x1,
    DMAC_EVENT_ERROR                      = 0x2,
};
#define DMA_EVENT_WAIT_DEF_TIME           ((LOSCFG_BASE_CORE_TICK_PER_SECOND) * 5)

enum DmacTransferType {
    TRASFER_TYPE_M2M = 0x0,
    TRASFER_TYPE_P2M = 0x1,
    TRASFER_TYPE_M2P = 0x2,
};

struct DmacMsg {
    uintptr_t srcAddr;
    uintptr_t destAddr;
    uint8_t srcWidth;         // src data width in bytes
    uint8_t destWidth;        // dest data width in bytes
    uint8_t transType;        // 0: mem to mem; 1: periph to mem; 2:mem to periph
    size_t transLen;
    DmacCallback *cb;
    void *para;
};

static inline uintptr_t DmacMsgGetPeriphAddr(struct DmacMsg *msg)
{
    return (msg->transType == TRASFER_TYPE_M2P) ? msg->destAddr :
        (msg->transType == TRASFER_TYPE_P2M) ? msg->srcAddr : PERIPH_ADDR_INVALID;
}

#define DMAC_LLI_HEAD         \
    uintptr_t nextLli;        \
    uintptr_t reserved0[6];   \
    unsigned long count;      \
    uintptr_t srcAddr;        \
    uintptr_t reserved1;      \
    uintptr_t destAddr;       \
    uintptr_t reserved2;      \
    unsigned long config

struct DmacLliHead {
    DMAC_LLI_HEAD;
};
#define DMAC_LLI_HEAD_SIZE    (sizeof(struct DmacLliHead))

#define DMAC_LLI_SIZE         64 // must be 64 Bytes aligned
struct DmacLli {
    DMAC_LLI_HEAD;
    uint8_t pad[DMAC_LLI_SIZE - DMAC_LLI_HEAD_SIZE];
};

struct DmacChanInfo {
    uint16_t channel;
    int status;
    int useStatus;
    int transType;
    uint8_t srcWidth;         // src data width in bytes
    uint8_t destWidth;        // dest data width in bytes
    unsigned long config;     // cpu width expected
    uintptr_t lliEnFlag;
    DmacEvent waitEvent;
    DmacCallback *callback;
    void *callbackData;
    uint16_t lliCnt;
    struct DmacLli *lli;
    void *dummyPage;
};

struct DmaCntlr {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    uint16_t index;
    uint32_t irq;
    uintptr_t phyBase;
    volatile unsigned char *remapBase;
    size_t regSize;
    size_t maxTransSize;
    uint16_t channelNum;
    OsalSpinlock lock;
    struct DmacChanInfo *channelList;
    int32_t (*getChanInfo)(struct DmaCntlr *cntlr, struct DmacChanInfo *chanInfo, struct DmacMsg *msg);
    int32_t (*dmaChanEnable)(struct DmaCntlr *cntlr, struct DmacChanInfo *chanInfo);
    int32_t (*dmaM2mChanEnable)(struct DmaCntlr *cntlr, struct DmacChanInfo *chanInfo,
        uintptr_t src, uintptr_t dest, size_t length);
    void (*dmacChanDisable)(struct DmaCntlr *cntlr, uint16_t channel);
    void (*dmacCacheInv)(uintptr_t vaddr, uintptr_t vend);
    void (*dmacCacheFlush)(uintptr_t vaddr, uintptr_t vend);
    void *(*dmacPaddrToVaddr)(uintptr_t paddr);
    uintptr_t (*dmacVaddrToPaddr)(void *vaddr);
    int (*dmacGetChanStatus)(struct DmaCntlr *cntlr, uint16_t chan);
    uintptr_t (*dmacGetCurrDestAddr)(struct DmaCntlr *cntlr, uint16_t chan);
    void *private;
};

#ifdef LOSCFG_DRIVERS_HDF_PLATFORM_DMAC
struct DmaCntlr *DmaCntlrCreate(struct HdfDeviceObject *dev);

void DmaCntlrDestroy(struct DmaCntlr *cntlr);

int32_t DmacCntlrAdd(struct DmaCntlr *cntlr);

void DmacCntlrRemove(struct DmaCntlr *cntlr);

int32_t DmaCntlrTransfer(struct DmaCntlr *cntlr, struct DmacMsg *msg);

uintptr_t DmaGetCurrChanDestAddr(struct DmaCntlr *cntlr, uint16_t chan);
#else
static inline struct DmaCntlr *DmaCntlrCreate(struct HdfDeviceObject *dev)
{
    (void)dev;
    return NULL;
}

static inline void DmaCntlrDestroy(struct DmaCntlr *cntlr)
{
    (void)cntlr;
    return;
}

static inline int32_t DmacCntlrAdd(struct DmaCntlr *cntlr)
{
    (void)cntlr;
    return HDF_ERR_NOT_SUPPORT;
}

static inline void DmacCntlrRemove(struct DmaCntlr *cntlr)
{
    (void)cntlr;
    return;
}

static inline int32_t DmaCntlrTransfer(struct DmaCntlr *cntlr, struct DmacMsg *msg)
{
    (void)cntlr;
    (void)msg;
    return HDF_ERR_NOT_SUPPORT;
}

static inline uintptr_t DmaGetCurrChanDestAddr(struct DmaCntlr *cntlr, uint16_t chan)
{
    (void)cntlr;
    (void)chan;
    return 0;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* DMAC_CORE_H */
