/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "eapol.h"
#include "securec.h"
#include "osal_time.h"
#include "osal_mem.h"
#include "hdf_log.h"

#define HDF_LOG_TAG             HDF_WIFI_EAPOL

#define RETURN_IF_INPUT_VALID(netDevice, buff) do {                 \
        if ((netDevice) == NULL || (buff) == NULL) {                        \
            HDF_LOGE("%s, netdevice = null or buf = null.", __func__); \
            return HDF_ERR_INVALID_PARAM;                               \
        }                                                               \
        if ((netDevice)->specialProcPriv == NULL) {                       \
            HDF_LOGE("%s : specialProcPriv = null", __func__);          \
            return HDF_ERR_INVALID_PARAM;                               \
        }                                                               \
    } while (0)                                                          \

static int32_t EnableEapol(const struct NetDevice *netDevice, struct EapolEnable *buff)
{
    struct EapolData *eapol = NULL;

    RETURN_IF_INPUT_VALID(netDevice, buff);
    eapol = (struct EapolData *)netDevice->specialProcPriv;
    eapol->regFlag = true;
    eapol->context = buff->context;
    eapol->notify = buff->notify;
    return HDF_SUCCESS;
}

static int32_t DisableEapol(const struct NetDevice *netDevice)
{
    struct EapolData *eapol = NULL;

    if (netDevice == NULL || netDevice->specialProcPriv == NULL) {
        HDF_LOGE("%s: netDevice = null or specialProcPriv = null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    eapol = (struct EapolData *)netDevice->specialProcPriv;
    eapol->regFlag = false;
    eapol->context = NULL;
    eapol->notify = NULL;
    return HDF_SUCCESS;
}

static int32_t GetEapol(const struct NetDevice *netDevice, struct EapolRx *buff)
{
    struct EapolData *eapol = NULL;
    struct EapolRx *eapolRx = buff;
    NetBuf *netBuff = NULL;
    uint8_t *p = NULL;
    uint32_t len;

    RETURN_IF_INPUT_VALID(netDevice, buff);
    eapol = (struct EapolData *)netDevice->specialProcPriv;
    if (NetBufQueueIsEmpty(&eapol->eapolQueue)) {
        HDF_LOGE("%s fail : eapolQueue empty!", __func__);
        return HDF_FAILURE;
    }
    netBuff = NetBufQueueDequeue(&eapol->eapolQueue);
    if (netBuff == NULL) {
        HDF_LOGE("%s fail : dequeue netBuff = null!", __func__);
        return HDF_FAILURE;
    }

    p = NetBufGetAddress(netBuff, E_DATA_BUF);
    len = NetBufGetDataLen(netBuff);
    if (len > eapolRx->len) {
        HDF_LOGE("%s fail : eapolRx->len too small! netBuff->len(%u) > eapolRx->len(%u).",
            __func__, netBuff->len, eapolRx->len);
        NetBufFree(netBuff);
        return HDF_FAILURE;
    }
    if (p != NULL) {
        if (memcpy_s(eapolRx->buff, eapolRx->len, p, len) != EOK) {
            HDF_LOGE("%s::mem safe function err!", __func__);
            NetBufFree(netBuff);
            return HDF_FAILURE;
        }
    }
    eapolRx->len = len;
    NetBufFree(netBuff);
    return HDF_SUCCESS;
}

static int32_t SendEapol(struct NetDevice *netDevice, struct EapolTx *buff)
{
    struct EapolTx *eapolTx = buff;
    NetBuf *netBuff = NULL;

    if (netDevice == NULL || buff == NULL) {
        HDF_LOGE("%s : netdevice = null or buff = null.", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if ((netDevice->netDeviceIf == NULL) || (netDevice->netDeviceIf->xmit == NULL)) {
        HDF_LOGE("%s : netDeviceIf NULL.", __func__);
        return HDF_FAILURE;
    }

    if (eapolTx->buff == NULL) {
        HDF_LOGE("%s::eapolTx->buff = null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    netBuff = NetBufDevAlloc(netDevice, eapolTx->len);
    if (netBuff == NULL) {
        HDF_LOGE("%s : netBuff alloc fail!", __func__);
        return HDF_FAILURE;
    }
    NetBufPush(netBuff, E_DATA_BUF, eapolTx->len);
    if (memcpy_s(NetBufGetAddress(netBuff, E_DATA_BUF), eapolTx->len,
        eapolTx->buff, eapolTx->len) != EOK) {
        NetBufFree(netBuff);
        HDF_LOGE("%s::mem safe function err!", __func__);
        return HDF_FAILURE;
    }
    return netDevice->netDeviceIf->xmit(netDevice, netBuff);
}

static void HandleEapolQueue(struct EapolData *eapol)
{
    int64_t currentTime = 0;
    int64_t intervalTime = 0;
    uint16_t maxCount;
    OsalTimespec timeSpec;
    NetBuf *netBuf = NULL;

    if (eapol == NULL) {
        return;
    }

    if (OsalGetTime(&timeSpec) == HDF_SUCCESS) {
        currentTime = timeSpec.sec;
    }
    maxCount = (eapol->maxCount != 0) ? eapol->maxCount : EAPOL_MAX_COUNT;
    /* The queue is empty to update enqueueTime and count */
    if (NetBufQueueIsEmpty(&eapol->eapolQueue)) {
        eapol->enqueueTime = currentTime;
        eapol->count = 0;
        return;
    }

    /* prevent a large amount of netbuff resources being used when WPA is abnormal */
    if (currentTime > eapol->enqueueTime) {
        intervalTime = currentTime - eapol->enqueueTime;
    }
    if (intervalTime > EAPOL_MAX_ENQUEUE_TIME || eapol->count >= maxCount) {
        /* output log to solve problem */
        if (intervalTime > EAPOL_MAX_ENQUEUE_TIME) {
            HDF_LOGE("%s discard pre netbuf : intervalTime(%lld) > EAPOL_MAX_ENQUEUE_TIME.", __func__,
                intervalTime);
        } else {
            HDF_LOGE("%s discard pre netbuf : eapol->count(%u) = maxCount(%hu).", __func__, eapol->count, maxCount);
        }
        eapol->count--;
        eapol->enqueueTime = currentTime;
        netBuf = NetBufQueueDequeue(&eapol->eapolQueue);
        if (netBuf == NULL) {
            HDF_LOGE("%s error : netbuff = null!", __func__);
            return;
        }
        NetBufFree(netBuf);
    }
    return;
}

static int32_t WriteEapolToQueue(const struct NetDevice *netDevice, NetBuf *buff)
{
    struct EapolData *eapol = NULL;

    RETURN_IF_INPUT_VALID(netDevice, buff);
    eapol = (struct EapolData *)netDevice->specialProcPriv;
    if (eapol->regFlag && (eapol->notify != NULL)) {
        HandleEapolQueue(eapol);
        /* eapol data enqueue and notify wap processing */
        eapol->count++;
        NetBufQueueEnqueue(&eapol->eapolQueue, buff);
        eapol->notify(netDevice->name, eapol->context);
        return HDF_SUCCESS;
    } else {
        /* free netbuff when not register notify. */
        NetBufFree(buff);
        HDF_LOGE("%s fail : eapol process is not register.", __func__);
        return HDF_SUCCESS;
    }
}

static struct EapolInterface g_eapolInterface = {
    .enableEapol = EnableEapol,
    .disableEapol = DisableEapol,
    .getEapol = GetEapol,
    .sendEapol = SendEapol,
    .writeEapolToQueue = WriteEapolToQueue,
};

struct Eapol g_eapol = {
    .eapolOp = &g_eapolInterface,
};

int32_t CreateEapolData(struct NetDevice *netDevice)
{
    struct EapolData *eapolData = NULL;

    if (netDevice == NULL) {
        HDF_LOGE("%s fail : netdev = null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (netDevice->specialProcPriv != NULL) {
        HDF_LOGI("%s already create!", __func__);
        return HDF_SUCCESS;
    }
    eapolData = (struct EapolData *)OsalMemCalloc(sizeof(struct EapolData));
    if (eapolData == NULL) {
        HDF_LOGE("%s fail : malloc fail!", __func__);
        return HDF_FAILURE;
    }
    (void)memset_s(eapolData, sizeof(struct EapolData), 0, sizeof(struct EapolData));
    NetBufQueueInit(&eapolData->eapolQueue);
    netDevice->specialProcPriv = eapolData;
    HDF_LOGI("%s success!", __func__);
    return HDF_SUCCESS;
}

void DestroyEapolData(struct NetDevice *netDevice)
{
    struct EapolData *eapolData = NULL;

    if (netDevice == NULL || netDevice->specialProcPriv == NULL) {
        HDF_LOGE("%s already free!", __func__);
        return;
    }
    eapolData = (struct EapolData *)netDevice->specialProcPriv;
    NetBufQueueClear(&eapolData->eapolQueue);
    OsalMemFree(eapolData);
    netDevice->specialProcPriv = NULL;
    HDF_LOGE("%s success!", __func__);
    return;
}

const struct Eapol *EapolGetInstance(void)
{
    return &g_eapol;
}
