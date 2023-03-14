/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "spi_core.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "spi_if.h"

#define HDF_LOG_TAG spi_core

int32_t SpiCntlrOpen(struct SpiCntlr *cntlr, uint32_t csNum)
{
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->method == NULL || cntlr->method->Open == NULL) {
        HDF_LOGE("%s: Open not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    (void)OsalMutexLock(&(cntlr->lock));
    cntlr->curCs = csNum;
    ret = cntlr->method->Open(cntlr);
    (void)OsalMutexUnlock(&(cntlr->lock));
    return ret;
}

int32_t SpiCntlrClose(struct SpiCntlr *cntlr, uint32_t csNum)
{
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->method == NULL || cntlr->method->Close == NULL) {
        HDF_LOGE("%s: Close not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    (void)OsalMutexLock(&(cntlr->lock));
    cntlr->curCs = csNum;
    ret = cntlr->method->Close(cntlr);
    (void)OsalMutexUnlock(&(cntlr->lock));
    return ret;
}

int32_t SpiCntlrTransfer(struct SpiCntlr *cntlr, uint32_t csNum, struct SpiMsg *msg, uint32_t count)
{
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->method == NULL || cntlr->method->Transfer == NULL) {
        HDF_LOGE("%s: transfer not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    cntlr->curCs = csNum;
    ret = cntlr->method->Transfer(cntlr, msg, count);
    (void)OsalMutexUnlock(&(cntlr->lock));
    return ret;
}

int32_t SpiCntlrSetCfg(struct SpiCntlr *cntlr, uint32_t csNum, struct SpiCfg *cfg)
{
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (cntlr->method == NULL || cntlr->method->SetCfg == NULL) {
        HDF_LOGE("%s: not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    cntlr->curCs = csNum;
    ret = cntlr->method->SetCfg(cntlr, cfg);
    (void)OsalMutexUnlock(&(cntlr->lock));
    return ret;
}

int32_t SpiCntlrGetCfg(struct SpiCntlr *cntlr, uint32_t csNum, struct SpiCfg *cfg)
{
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (cntlr->method == NULL || cntlr->method->GetCfg == NULL) {
        HDF_LOGE("%s: not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    cntlr->curCs = csNum;
    ret = cntlr->method->GetCfg(cntlr, cfg);
    (void)OsalMutexUnlock(&(cntlr->lock));
    return ret;
}

static int32_t SpiTransferRebuildMsgs(struct HdfSBuf *data, struct SpiMsg **ppmsgs, uint32_t *pcount, uint8_t **ppbuf)
{
    uint32_t count;
    int32_t i;
    uint32_t len;
    uint32_t lenReply = 0;
    uint8_t *buf = NULL;
    uint8_t *bufReply = NULL;
    struct SpiMsg *msgs = NULL;

    if (!HdfSbufReadBuffer(data, (const void **)&msgs, &len) || msgs == NULL || len == 0) {
        HDF_LOGE("%s: read msgs fail!", __func__);
        return HDF_ERR_IO;
    }

    count = (uint32_t)len / (uint32_t)sizeof(struct SpiMsg);
    for (i = 0; i < count; i++) {
        if (msgs[i].rbuf != NULL) {
            lenReply += msgs[i].len;
        }
        if (msgs[i].wbuf == NULL) {
            continue;
        }
        if (!HdfSbufReadBuffer(data, (const void **)&buf, &len)) {
            HDF_LOGE("%s: read msg[%d] buf fail!", __func__, i);
        } else {
            msgs[i].wbuf = buf;
        }
    }

    if (lenReply > 0) {
        bufReply = OsalMemCalloc(lenReply);
        if (bufReply == NULL) {
            return HDF_ERR_MALLOC_FAIL;
        }
        for (i = 0, buf = bufReply; i < count && buf < (bufReply + lenReply); i++) {
            if (msgs[i].rbuf != NULL) {
                msgs[i].rbuf = buf;
                buf += msgs[i].len;
            }
        }
    }

    *ppmsgs = msgs;
    *pcount = count;
    *ppbuf = bufReply;

    return HDF_SUCCESS;
}

static int32_t SpiTransferWriteBackMsgs(struct HdfSBuf *reply, struct SpiMsg *msgs, uint32_t count)
{
    uint32_t i;

    for (i = 0; i < count; i++) {
        if (msgs[i].rbuf == NULL) {
            continue;
        }

        if (!HdfSbufWriteBuffer(reply, msgs[i].rbuf, msgs[i].len)) {
            HDF_LOGE("%s: write msg[%u] reply failed!", __func__, i);
            return HDF_ERR_IO;
        }
    }

    return HDF_SUCCESS;
}

static int32_t SpiIoTransfer(struct SpiCntlr *cntlr, uint32_t csNum, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint32_t count;
    struct SpiMsg *msgs = NULL;
    uint8_t *bufReply = NULL;

    if (data == NULL || reply == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    ret = SpiTransferRebuildMsgs(data, &msgs, &count, &bufReply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: rebuild msgs fail:%d", __func__, ret);
        goto EXIT;
    }

    ret = SpiCntlrTransfer(cntlr, csNum, msgs, count);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: do transfer failed:%d", __func__, ret);
        goto EXIT;
    }

    ret =  SpiTransferWriteBackMsgs(reply, msgs, count);

EXIT:
    if (bufReply != NULL) {
        OsalMemFree(bufReply);
    }
    return ret;
}

static int32_t SpiIoOpen(struct SpiCntlr *cntlr, uint32_t csNum)
{
    return SpiCntlrOpen(cntlr, csNum);
}

static int32_t SpiIoClose(struct SpiCntlr *cntlr, uint32_t csNum)
{
    return SpiCntlrClose(cntlr, csNum);
}

static int32_t SpiIoSetConfig(struct SpiCntlr *cntlr, uint32_t csNum, struct HdfSBuf *data)
{
    uint32_t len;
    struct SpiCfg *cfg = NULL;

    if (!HdfSbufReadBuffer(data, (const void **)&cfg, &len) || sizeof(*cfg) != len) {
        HDF_LOGE("%s: read buffer failed!", __func__);
        return HDF_ERR_IO;
    }
    return SpiCntlrSetCfg(cntlr, csNum, cfg);
}

static int32_t SpiIoGetConfig(struct SpiCntlr *cntlr, uint32_t csNum, struct HdfSBuf *reply)
{
    int32_t ret;
    struct SpiCfg cfg;

    ret = SpiCntlrGetCfg(cntlr, csNum, &cfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get cfg failed!", __func__);
        return ret;
    }

    if (!HdfSbufWriteBuffer(reply, &cfg, sizeof(cfg))) {
        HDF_LOGE("%s: write buffer failed!", __func__);
        return ret;
    }
    return HDF_SUCCESS;
}

static int32_t SpiIoDispatch(struct HdfDeviceIoClient *client, int cmd,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint32_t csNum;
    struct SpiCntlr *cntlr = NULL;

    if (client == NULL || client->device == NULL || client->device->service == NULL) {
        HDF_LOGE("%s: invalid client", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    cntlr = (struct SpiCntlr *)client->device->service;
    if (data == NULL) {
        HDF_LOGE("%s: data is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &csNum)) {
        HDF_LOGE("%s: read csNum failed!", __func__);
        return HDF_ERR_IO;
    }

    switch (cmd) {
        case SPI_IO_OPEN:
            return SpiIoOpen(cntlr, csNum);
        case SPI_IO_CLOSE:
            return SpiIoClose(cntlr, csNum);
        case SPI_IO_SET_CONFIG:
            return SpiIoSetConfig(cntlr, csNum, data);
        case SPI_IO_GET_CONFIG:
            return SpiIoGetConfig(cntlr, csNum, reply);
        case SPI_IO_TRANSFER:
            return SpiIoTransfer(cntlr, csNum, data, reply);
        default:
            ret = HDF_ERR_NOT_SUPPORT;
            break;
    }
    return ret;
}

void SpiCntlrDestroy(struct SpiCntlr *cntlr)
{
    if (cntlr == NULL) {
        return;
    }
    (void)OsalMutexDestroy(&(cntlr->lock));
    OsalMemFree(cntlr);
}

struct SpiCntlr *SpiCntlrCreate(struct HdfDeviceObject *device)
{
    struct SpiCntlr *cntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return NULL;
    }

    cntlr = (struct SpiCntlr *)OsalMemCalloc(sizeof(*cntlr));
    if (cntlr == NULL) {
        HDF_LOGE("%s: OsalMemCalloc error", __func__);
        return NULL;
    }
    cntlr->device = device;
    device->service = &(cntlr->service);
    device->service->Dispatch = SpiIoDispatch;
    (void)OsalMutexInit(&cntlr->lock);
    DListHeadInit(&cntlr->list);
    cntlr->priv = NULL;
    return cntlr;
}
