/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "dmac_core.h"
#include <string.h>
#include "hdf_log.h"
#include "osal_io.h"
#include "osal_irq.h"
#include "osal_mem.h"

#define HDF_LOG_TAG dmac_core

#define DMA_ALIGN_SIZE 256
#define DMA_MAX_TRANS_SIZE_DEFAULT 256

static int32_t DmacCntlrCheckOps(struct DmaCntlr *cntlr)
{
    if (cntlr->getChanInfo == NULL) {
        HDF_LOGE("%s: getChanInfo is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->dmaChanEnable == NULL) {
        HDF_LOGE("%s: dmaChanEnable is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->dmaM2mChanEnable == NULL) {
        HDF_LOGE("%s: dmaM2mChanEnable is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->dmacChanDisable == NULL) {
        HDF_LOGE("%s: dmacChanDisable is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->dmacCacheInv == NULL) {
        HDF_LOGE("%s: dmacCacheInv null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->dmacCacheFlush == NULL) {
        HDF_LOGE("%s: dmacCacheFlush null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->dmacPaddrToVaddr == NULL) {
        HDF_LOGE("%s: dmacPaddrToVaddr null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->dmacVaddrToPaddr == NULL) {
        HDF_LOGE("%s: dmacVaddrToPaddr null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->dmacGetChanStatus == NULL) {
        HDF_LOGE("%s: dmacGetChanStatus null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->dmacGetCurrDestAddr == NULL) {
        HDF_LOGE("%s: dmacGetCurrDestAddr null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    return HDF_SUCCESS;
}

static int32_t DmacCntlrCheckParam(struct DmaCntlr *cntlr)
{
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->maxTransSize == 0) {
        HDF_LOGE("%s: cntlr is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->channelNum == 0 || cntlr->channelNum > DMAC_CHAN_NUM_MAX) {
        HDF_LOGE("%s: invalid channelNum:%u", __func__, cntlr->channelNum);
        return HDF_ERR_INVALID_OBJECT;
    }
    return HDF_SUCCESS;
}

static int32_t DmacCntlrCheckInit(struct DmaCntlr *cntlr)
{
    int32_t ret;

    ret = DmacCntlrCheckParam(cntlr);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    ret = DmacCntlrCheckOps(cntlr);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    return HDF_SUCCESS;
}

static int32_t DmacCntlrCheck(struct DmaCntlr *cntlr)
{
    int32_t ret;

    ret = DmacCntlrCheckInit(cntlr);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (cntlr->channelList == NULL) {
        HDF_LOGE("%s: channelList is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    return HDF_SUCCESS;
}

struct DmaCntlr *DmaCntlrCreate(struct HdfDeviceObject *device)
{
    struct DmaCntlr *cntlr = NULL;

    if (device == NULL) {
        return NULL;
    }
    cntlr = (struct DmaCntlr *)OsalMemCalloc(sizeof(struct DmaCntlr));
    if (cntlr == NULL) {
        return NULL;
    }
    cntlr->device = device;
    return cntlr;
}

static void DmacFreeLli(struct DmacChanInfo *chanInfo)
{
    if (chanInfo != NULL && chanInfo->lli != NULL) {
        OsalMemFree(chanInfo->lli);
        chanInfo->lli = NULL;
        chanInfo->lliCnt = 0;
    }
}

/*
 * If private is allocated, release private before calling this function
 */
void DmaCntlrDestroy(struct DmaCntlr *cntlr)
{
    uint16_t i;

    if (cntlr == NULL || cntlr->channelNum > DMAC_CHAN_NUM_MAX) {
        HDF_LOGE("dma cntlr null or channel invalid!");
        return;
    }
    if (cntlr->channelList != NULL) {
        for (i = 0; i < cntlr->channelNum; i++) {
            DmacFreeLli(&(cntlr->channelList[i]));
        }
        OsalMemFree(cntlr->channelList);
        cntlr->channelList = NULL;
        cntlr->channelNum = 0;
    }
    /* Private is released by the caller */
    cntlr->private = NULL;
    OsalMemFree(cntlr);
}

static void DmacEventCallback(struct DmacChanInfo *chanInfo)
{
    if (chanInfo->status == DMAC_CHN_ERROR) {
        DmaEventSignal(&chanInfo->waitEvent, DMAC_EVENT_ERROR);
    } else {
        DmaEventSignal(&chanInfo->waitEvent, DMAC_EVENT_DONE);
    }
}

static void DmacCallbackHandle(struct DmacChanInfo *chanInfo)
{
    if (chanInfo->transType == TRASFER_TYPE_M2M) {
        DmacEventCallback(chanInfo);
        return;
    }
    if (chanInfo->callback != NULL) {
        chanInfo->callback(chanInfo->callbackData, chanInfo->status);
    }
}

static int32_t DmacWaitM2mSendComplete(struct DmaCntlr *cntlr, struct DmacChanInfo *chanInfo)
{
    uint32_t ret;

    if (DmacCntlrCheck(cntlr) != HDF_SUCCESS) {
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = DmaEventWait(&chanInfo->waitEvent, DMAC_EVENT_DONE | DMAC_EVENT_ERROR, DMA_EVENT_WAIT_DEF_TIME);
    if (ret == DMAC_EVENT_ERROR) {
        HDF_LOGE("%s: wait event error", __func__);
        return DMAC_CHN_ERROR;
    } else if (ret == LOS_ERRNO_EVENT_READ_TIMEOUT) {
        HDF_LOGE("%s: wait event timeout", __func__);
        return DMAC_CHN_TIMEOUT;
    }

    cntlr->dmacChanDisable(cntlr, chanInfo->channel);
    return DMAC_CHN_SUCCESS;
}

static uint16_t DmacAllocateChannel(struct DmaCntlr *cntlr)
{
    int i;
    uint32_t flags;

    if (DmacCntlrCheck(cntlr) != HDF_SUCCESS) {
        return HDF_ERR_INVALID_OBJECT;
    }

    OsalSpinLockIrqSave(&cntlr->lock, &flags);
    for (i = 0; i < cntlr->channelNum; i++) {
        if (cntlr->channelList[i].useStatus ==  DMAC_CHN_VACANCY) {
            cntlr->channelList[i].useStatus = DMAC_CHN_ALLOCAT;
            OsalSpinUnlockIrqRestore(&cntlr->lock, &flags);
            return i;
        }
    }
    OsalSpinUnlockIrqRestore(&cntlr->lock, &flags);
    return HDF_FAILURE;
}

static void DmacFreeChannel(struct DmaCntlr *cntlr, uint16_t channel)
{
    uint32_t flags;

    if (DmacCntlrCheck(cntlr) != HDF_SUCCESS) {
        return;
    }

    OsalSpinLockIrqSave(&cntlr->lock, &flags);
    DmacFreeLli(&cntlr->channelList[channel]);
    cntlr->channelList[channel].useStatus = DMAC_CHN_VACANCY;
    OsalSpinUnlockIrqRestore(&cntlr->lock, &flags);
}

static struct DmacChanInfo *DmacRequestChannel(struct DmaCntlr *cntlr, struct DmacMsg *msg)
{
    int32_t ret;
    int chan;
    struct DmacChanInfo *chanInfo = NULL;

    if (DmacCntlrCheck(cntlr) != HDF_SUCCESS || msg == NULL) {
        HDF_LOGE("%s: cntlr check failed or msg invalid", __func__);
        return NULL;
    }

    chan = DmacAllocateChannel(cntlr);
    if (chan < 0) {
        HDF_LOGE("%s: allocate channel failed", __func__);
        return NULL;
    }
    chanInfo = &(cntlr->channelList[chan]);
    chanInfo->channel = (unsigned int)chan;
    chanInfo->transType = msg->transType;
    ret = cntlr->getChanInfo(cntlr, chanInfo, msg);
    if (ret < 0) {
        DmacFreeChannel(cntlr, chan);
        HDF_LOGE("%s: get channel info failed ret = %d", __func__, ret);
        return NULL;
    }
#ifdef DMA_CORE_DEBUG
    HDF_LOGD("chan = %d, type = %d srcWidth = %u, destWidth = %u, config = 0x%x, lliEnflag = 0x%x",
        chan, chanInfo->transType, chanInfo->srcWidth, chanInfo->destWidth, chanInfo->config, chanInfo->lliEnFlag);
#endif
    return chanInfo;
}

static uintptr_t DmacGetDummyBuf(struct DmaCntlr *cntlr, struct DmacChanInfo *chan)
{
    if (chan->dummyPage == NULL) {
        chan->dummyPage = OsalMemCalloc(sizeof(cntlr->maxTransSize));
    }

    return (chan->dummyPage == NULL) ? 0 : (uintptr_t)LOS_PaddrQuery(chan->dummyPage);
}

static inline size_t DmacAlignedTransMax(size_t maxSize, uint8_t srcWidth, uint8_t destWidth)
{
    size_t ret;
    uint8_t maxWidth = (srcWidth >= destWidth) ? srcWidth : destWidth;

    ret = ((maxWidth == 0) ? maxSize : (maxSize - (maxSize % maxWidth)));
#ifdef DMA_CORE_DEBUG
    HDF_LOGD("%s: max:%zu, srcwidth:%u, dstwidth:%hhu, alignedmax:%zu", __func__, maxSize, srcWidth, destWidth, ret);
#endif
    return ret;
}

static int32_t DmacFillLli(struct DmaCntlr *cntlr, struct DmacChanInfo *chanInfo,
    uintptr_t srcaddr, uintptr_t dstaddr, size_t length)
{
    int32_t ret = HDF_SUCCESS;
    uint16_t i;
    uint16_t lliNum;
    struct DmacLli  *plli = NULL;
    size_t alignedMax;
    uintptr_t srcDummy = 0;
    uintptr_t dstDummy = 0;

    if (DmacCntlrCheck(cntlr) != HDF_SUCCESS) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (chanInfo == NULL || chanInfo->lli == NULL) {
        HDF_LOGE("%s: chanInfo or lli is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    alignedMax = DmacAlignedTransMax(cntlr->maxTransSize, chanInfo->srcWidth, chanInfo->destWidth);
    if (alignedMax == 0) {
        HDF_LOGE("%s: maxTransSize:%zu srcWidth:%u dstWidth:%u", __func__,
            cntlr->maxTransSize, chanInfo->srcWidth, chanInfo->destWidth);
        return HDF_ERR_INVALID_PARAM;
    }
    plli = chanInfo->lli;

    if (srcaddr == 0) {
        srcaddr = srcDummy = DmacGetDummyBuf(cntlr, chanInfo);
    }
    if (dstaddr == 0) {
        dstaddr = dstDummy = DmacGetDummyBuf(cntlr, chanInfo);
    }
    if (srcaddr == 0 || dstaddr == 0) {
        return HDF_ERR_MALLOC_FAIL;
    }

    for (i = 0, lliNum = chanInfo->lliCnt; i < lliNum; i++, plli++) {
        plli->nextLli = (uintptr_t)cntlr->dmacVaddrToPaddr((void *)plli) + (uintptr_t)sizeof(struct DmacLli);
        plli->nextLli = (i < lliNum - 1) ? (plli->nextLli + chanInfo->lliEnFlag) : 0;
        plli->count = ((i < lliNum - 1) || ((length % alignedMax) == 0)) ? alignedMax : (length % alignedMax);

        plli->srcAddr = srcaddr;
        plli->destAddr = dstaddr;
        plli->config = chanInfo->config;

#ifdef DMA_CORE_DEBUG
        HDF_LOGD("plli=0x%lx, next=0x%lx, count=0x%lx, src=0x%lx, dst=0x%lx, cfg=0x%lx",
            (uintptr_t)cntlr->dmacVaddrToPaddr(plli), plli->nextLli,
            plli->count, plli->srcAddr, plli->destAddr, plli->config);
#endif
        dstaddr += (chanInfo->transType == TRASFER_TYPE_P2M && dstDummy == 0) ? plli->count : 0;
        srcaddr += (chanInfo->transType == TRASFER_TYPE_M2P && srcDummy == 0) ? plli->count : 0;
    }
    plli = chanInfo->lli;
    cntlr->dmacCacheFlush((uintptr_t)plli, (uintptr_t)plli + (uintptr_t)(sizeof(struct DmacLli) * lliNum));
    return ret;
}

static int32_t DmacAllocLli(struct DmacChanInfo *chanInfo, size_t length, size_t maxSize)
{
    size_t lliNum;
    size_t allocLength;
    void *allocAddr = NULL;

    if (chanInfo == NULL || maxSize == 0) {
        return HDF_ERR_INVALID_PARAM;
    }
    lliNum = (length / maxSize) + ((length % maxSize) > 0 ? 1 : 0);
    if (lliNum > 2048) {  /* 2048: lliNum is not more than 2048 */
        HDF_LOGE("%s: lliNum %zu is bigger than 2048", __func__, lliNum);
        return HDF_ERR_INVALID_PARAM;
    }

    allocLength = lliNum * sizeof(struct DmacLli);
    allocLength = ALIGN(allocLength, CACHE_ALIGNED_SIZE);
    allocAddr = OsalMemAllocAlign(DMA_ALIGN_SIZE, allocLength);
    if (allocAddr == NULL) {
        HDF_LOGE("%s: alloc lli mem failed", __func__);
        return HDF_FAILURE;
    }
    if (memset_s(allocAddr, allocLength, 0, allocLength) != EOK) {
        OsalMemFree(allocAddr);
        return HDF_FAILURE;
    }

    chanInfo->lliCnt = (uint16_t)lliNum;
    chanInfo->lli = (struct DmacLli *)allocAddr;
    return HDF_SUCCESS;
}

static int32_t DmacPeriphTransfer(struct DmaCntlr *cntlr, struct DmacMsg *msg)
{
    int32_t ret;
    struct DmacChanInfo *chanInfo = NULL;

    chanInfo = DmacRequestChannel(cntlr, msg);
    if (chanInfo == NULL) {
        HDF_LOGE("%s: request channel failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (msg->srcAddr == 0 && msg->destAddr == 0) {
        HDF_LOGE("%s: src addr & dest addr both null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    chanInfo->callbackData = msg->para;
    chanInfo->callback = (DmacCallback *)msg->cb;
    ret = DmacAllocLli(chanInfo, msg->transLen,
        DmacAlignedTransMax(cntlr->maxTransSize, chanInfo->srcWidth, chanInfo->destWidth));
    if (ret != HDF_SUCCESS) {
        DmacFreeChannel(cntlr, chanInfo->channel);
        return ret;
    }
    ret = DmacFillLli(cntlr, chanInfo, msg->srcAddr, msg->destAddr, msg->transLen);
    if (ret != HDF_SUCCESS) {
        DmacFreeLli(chanInfo);
        DmacFreeChannel(cntlr, chanInfo->channel);
        return ret;
    }
    ret = cntlr->dmaChanEnable(cntlr, chanInfo);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: enable channel failed", __func__);
        DmacFreeLli(chanInfo);
        DmacFreeChannel(cntlr, chanInfo->channel);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t DmacM2mTransfer(struct DmaCntlr *cntlr, struct DmacMsg *msg)
{
    int32_t ret;
    size_t leftSize;
    size_t dmaSize;
    size_t dmaCount = 0;
    struct DmacChanInfo *chanInfo = NULL;

    if (DmacCntlrCheck(cntlr) != HDF_SUCCESS) {
        return HDF_ERR_INVALID_OBJECT;
    }

    chanInfo = DmacRequestChannel(cntlr, msg);
    if (chanInfo == NULL) {
        HDF_LOGE("%s: request channel failed", __func__);
        return HDF_FAILURE;
    }
    chanInfo->callback = msg->cb;
    chanInfo->callbackData = msg->para;
    cntlr->dmacCacheFlush((uintptr_t)msg->srcAddr, (uintptr_t)(msg->srcAddr + msg->transLen));
    cntlr->dmacCacheInv((uintptr_t)msg->destAddr, (uintptr_t)(msg->destAddr + msg->transLen));
    leftSize = msg->transLen;
    while (leftSize > 0) {
        dmaSize = (leftSize >= cntlr->maxTransSize) ? cntlr->maxTransSize : leftSize;
        ret = cntlr->dmaM2mChanEnable(cntlr, chanInfo, msg->srcAddr + dmaCount * cntlr->maxTransSize,
            msg->destAddr + dmaCount * cntlr->maxTransSize, dmaSize);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: enable channel m2m failed", __func__);
            DmacFreeChannel(cntlr, chanInfo->channel);
            if (chanInfo->callback != NULL) {
                chanInfo->callback(chanInfo->callbackData, DMAC_CHN_ERROR);
            }
            return ret;
        }
        ret = DmacWaitM2mSendComplete(cntlr, chanInfo);
        if (ret != DMAC_CHN_SUCCESS) {
            HDF_LOGE("%s: m2m transfer failed, ret = %d", __func__, ret);
            DmacFreeChannel(cntlr, chanInfo->channel);
            if (chanInfo->callback != NULL) {
                chanInfo->callback(chanInfo->callbackData, ret);
            }
            return HDF_FAILURE;
        }
        leftSize -= dmaSize;
        dmaCount++;
    }
    DmacFreeChannel(cntlr, chanInfo->channel);
    if (chanInfo->callback != NULL) {
        chanInfo->callback(chanInfo->callbackData, DMAC_CHN_SUCCESS);
    }
    return HDF_SUCCESS;
}

int32_t DmaCntlrTransfer(struct DmaCntlr *cntlr, struct DmacMsg *msg)
{
    uintptr_t phyAddr;

    if (DmacCntlrCheck(cntlr) != HDF_SUCCESS) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (msg == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (msg->transType == TRASFER_TYPE_P2M) {
        if (msg->destAddr != 0) {
            phyAddr = (uintptr_t)cntlr->dmacPaddrToVaddr((paddr_t)msg->destAddr);
            cntlr->dmacCacheInv(phyAddr, (uintptr_t)(phyAddr + msg->transLen));
        }
    } else if (msg->transType == TRASFER_TYPE_M2P) {
        if (msg->srcAddr != 0) {
            phyAddr = (uintptr_t)cntlr->dmacPaddrToVaddr((paddr_t)msg->srcAddr);
            cntlr->dmacCacheFlush(phyAddr, (uintptr_t)(phyAddr + msg->transLen));
        }
    } else if (msg->transType == TRASFER_TYPE_M2M) {
        return DmacM2mTransfer(cntlr, msg);
    } else {
        HDF_LOGE("%s: invalid transType %d", __func__, msg->transType);
        return HDF_FAILURE;
    }
    return DmacPeriphTransfer(cntlr, msg);
}

uintptr_t DmaGetCurrChanDestAddr(struct DmaCntlr *cntlr, uint16_t chan)
{
    if (DmacCntlrCheck(cntlr) != HDF_SUCCESS) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return cntlr->dmacGetCurrDestAddr(cntlr, chan);
}

static uint32_t DmacIsr(uint32_t irq, void *dev)
{
    uint16_t i;
    int channelStatus;
    struct DmaCntlr *cntlr = (struct DmaCntlr *)dev;

    if (DmacCntlrCheck(cntlr) != HDF_SUCCESS) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (irq != cntlr->irq || cntlr->channelNum > DMAC_CHAN_NUM_MAX) {
        HDF_LOGE("%s: cntlr parm err! irq:%d, channel:%u", __func__, cntlr->irq, cntlr->channelNum);
        return HDF_ERR_INVALID_OBJECT;
    }
    for (i = 0; i < cntlr->channelNum; i++) {
        channelStatus = cntlr->dmacGetChanStatus(cntlr, i);
        if (channelStatus == DMAC_CHN_SUCCESS || channelStatus == DMAC_CHN_ERROR) {
            cntlr->channelList[i].status = channelStatus;
            DmacCallbackHandle(&(cntlr->channelList[i]));
            DmacFreeChannel(cntlr, cntlr->channelList[i].channel);
        }
    }
    return HDF_SUCCESS;
}

int32_t DmacCntlrAdd(struct DmaCntlr *cntlr)
{
    int32_t ret;
    uint16_t i;

    ret = DmacCntlrCheckInit(cntlr);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    (void)OsalSpinInit(&cntlr->lock);
    ret = OsalRegisterIrq(cntlr->irq, 0, (OsalIRQHandle)DmacIsr, "PlatDmac", cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: request irq %u failed, ret = %d", __func__, cntlr->irq, ret);
        (void)OsalSpinDestroy(&cntlr->lock);
        return ret;
    }
    cntlr->channelList = (struct DmacChanInfo *)OsalMemCalloc(sizeof(struct DmacChanInfo) * cntlr->channelNum);
    if (cntlr->channelList == NULL) {
        HDF_LOGE("%s: alloc channel list failed", __func__);
        (void)OsalUnregisterIrq(cntlr->irq, cntlr);
        (void)OsalSpinDestroy(&cntlr->lock);
        return HDF_ERR_MALLOC_FAIL;
    }
    for (i = 0; i < cntlr->channelNum; i++) {
        cntlr->dmacChanDisable(cntlr, i);
        DmaEventInit(&(cntlr->channelList[i].waitEvent));
        cntlr->channelList[i].useStatus = DMAC_CHN_VACANCY;
    }
    return HDF_SUCCESS;
}

void DmacCntlrRemove(struct DmaCntlr *cntlr)
{
    (void)cntlr;
}
