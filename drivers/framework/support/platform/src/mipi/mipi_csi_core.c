/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "mipi_csi_core.h"
#include "hdf_log.h"

#define HDF_LOG_TAG mipi_csi_core

struct MipiCsiHandle {
    struct MipiCsiCntlr *cntlr;
    struct OsalMutex lock;
    void *priv;
};

static struct MipiCsiHandle g_mipiCsihandle[MAX_CNTLR_CNT];

int32_t MipiCsiRegisterCntlr(struct MipiCsiCntlr *cntlr, struct HdfDeviceObject *device)
{
    HDF_LOGI("%s: enter!", __func__);
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->devNo >= MAX_CNTLR_CNT) {
        HDF_LOGE("%s: cntlr->devNo is error.", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (device == NULL) {
        HDF_LOGE("%s: device is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (g_mipiCsihandle[cntlr->devNo].cntlr == NULL) {
        (void)OsalMutexInit(&g_mipiCsihandle[cntlr->devNo].lock);
        (void)OsalMutexInit(&(cntlr->lock));

        g_mipiCsihandle[cntlr->devNo].cntlr = cntlr;
        g_mipiCsihandle[cntlr->devNo].priv = NULL;
        cntlr->device = device;
        device->service = &(cntlr->service);
        cntlr->priv = NULL;
        HDF_LOGI("%s: success.", __func__);

        return HDF_SUCCESS;
    }

    HDF_LOGE("%s: cntlr already exists.", __func__);
    return HDF_FAILURE;
}

void MipiCsiUnregisterCntlr(struct MipiCsiCntlr *cntlr)
{
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL.", __func__);
        return;
    }

    (void)OsalMutexDestroy(&(cntlr->lock));
    (void)OsalMutexDestroy(&(g_mipiCsihandle[cntlr->devNo].lock));

    HDF_LOGI("%s: success.", __func__);
    return;
}

struct MipiCsiCntlr *MipiCsiCntlrFromDevice(const struct HdfDeviceObject *device)
{
    return (device == NULL) ? NULL : (struct MipiCsiCntlr *)device->service;
}

struct MipiCsiCntlr *MipiCsiCntlrGet(uint8_t number)
{
    struct MipiCsiCntlr *cntlr = NULL;
    HDF_LOGI("%s: enter!", __func__);

    if (number >= MAX_CNTLR_CNT) {
        HDF_LOGE("%s: invalid number.", __func__);
        return NULL;
    }
    if (g_mipiCsihandle[number].cntlr == NULL) {
        HDF_LOGE("%s: g_mipiCsihandle[number].cntlr is NULL.", __func__);
        return NULL;
    }

    (void)OsalMutexLock(&(g_mipiCsihandle[number].lock));
    g_mipiCsihandle[number].cntlr->devNo = number;
    cntlr = g_mipiCsihandle[number].cntlr;
    (void)OsalMutexUnlock(&(g_mipiCsihandle[number].lock));

    return cntlr;
}

void MipiCsiCntlrPut(const struct MipiCsiCntlr *cntlr)
{
    uint8_t number;

    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL.", __func__);
        return;
    }

    number = cntlr->devNo;
    if (number >= MAX_CNTLR_CNT) {
        HDF_LOGE("%s: invalid number.", __func__);
        return;
    }

    HDF_LOGI("%s: success!", __func__);
}

int32_t MipiCsiCntlrSetComboDevAttr(struct MipiCsiCntlr *cntlr, ComboDevAttr *pAttr)
{
    int32_t ret;
    HDF_LOGI("%s: enter!", __func__);

    if ((cntlr == NULL) || (cntlr->ops == NULL)) {
        HDF_LOGE("%s: cntlr or ops is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (pAttr == NULL) {
        HDF_LOGE("%s: pAttr is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops->setComboDevAttr == NULL) {
        HDF_LOGE("%s: setComboDevAttr is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->ops->setComboDevAttr(cntlr, pAttr);
    (void)OsalMutexUnlock(&(cntlr->lock));

    if (ret == HDF_SUCCESS) {
        HDF_LOGI("%s: success!", __func__);
    } else {
        HDF_LOGE("%s: failed!", __func__);
    }

    return ret;
}

int32_t MipiCsiCntlrSetPhyCmvmode(struct MipiCsiCntlr *cntlr, uint8_t devno, PhyCmvMode cmvMode)
{
    int32_t ret;
    HDF_LOGI("%s: enter!", __func__);

    if ((cntlr == NULL) || (cntlr->ops == NULL)) {
        HDF_LOGE("%s: cntlr or ops is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops->setPhyCmvmode == NULL) {
        HDF_LOGE("%s: setPhyCmvmode is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->ops->setPhyCmvmode(cntlr, devno, cmvMode);
    (void)OsalMutexUnlock(&(cntlr->lock));

    if (ret == HDF_SUCCESS) {
        HDF_LOGI("%s: success!", __func__);
    } else {
        HDF_LOGE("%s: failed!", __func__);
    }

    return ret;
}

int32_t MipiCsiCntlrSetExtDataType(struct MipiCsiCntlr *cntlr, ExtDataType* dataType)
{
    int32_t ret;
    HDF_LOGI("%s: enter!", __func__);

    if ((cntlr == NULL) || (cntlr->ops == NULL)) {
        HDF_LOGE("%s: cntlr or ops is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (dataType == NULL) {
        HDF_LOGE("%s: dataType is NULL.", __func__);
        return HDF_FAILURE;
    }
    if (cntlr->ops->setExtDataType == NULL) {
        HDF_LOGE("%s: setExtDataType is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->ops->setExtDataType(cntlr, dataType);
    (void)OsalMutexUnlock(&(cntlr->lock));

    if (ret == HDF_SUCCESS) {
        HDF_LOGI("%s: success!", __func__);
    } else {
        HDF_LOGE("%s: failed!", __func__);
    }

    return ret;
}

int32_t MipiCsiCntlrSetHsMode(struct MipiCsiCntlr *cntlr, LaneDivideMode laneDivideMode)
{
    int32_t ret;
    HDF_LOGI("%s: enter!", __func__);

    if ((cntlr == NULL) || (cntlr->ops == NULL)) {
        HDF_LOGE("%s: cntlr or ops is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops->setHsMode == NULL) {
        HDF_LOGE("%s: setHsMode is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->ops->setHsMode(cntlr, laneDivideMode);
    (void)OsalMutexUnlock(&(cntlr->lock));

    if (ret == HDF_SUCCESS) {
        HDF_LOGI("%s: success!", __func__);
    } else {
        HDF_LOGE("%s: failed!", __func__);
    }

    return ret;
}

int32_t MipiCsiCntlrEnableClock(struct MipiCsiCntlr *cntlr, uint8_t comboDev)
{
    int32_t ret;
    HDF_LOGI("%s: enter!", __func__);

    if ((cntlr == NULL) || (cntlr->ops == NULL)) {
        HDF_LOGE("%s: cntlr or ops is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops->enableClock == NULL) {
        HDF_LOGE("%s: enableClock is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->ops->enableClock(cntlr, comboDev);
    (void)OsalMutexUnlock(&(cntlr->lock));

    if (ret == HDF_SUCCESS) {
        HDF_LOGI("%s: success!", __func__);
    } else {
        HDF_LOGE("%s: failed!", __func__);
    }

    return ret;
}

int32_t MipiCsiCntlrDisableClock(struct MipiCsiCntlr *cntlr, uint8_t comboDev)
{
    int32_t ret;
    HDF_LOGI("%s: enter!", __func__);

    if ((cntlr == NULL) || (cntlr->ops == NULL)) {
        HDF_LOGE("%s: cntlr or ops is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops->disableClock == NULL) {
        HDF_LOGE("%s: disableClock is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->ops->disableClock(cntlr, comboDev);
    (void)OsalMutexUnlock(&(cntlr->lock));

    if (ret == HDF_SUCCESS) {
        HDF_LOGI("%s: success!", __func__);
    } else {
        HDF_LOGE("%s: failed!", __func__);
    }

    return ret;
}

int32_t MipiCsiCntlrResetRx(struct MipiCsiCntlr *cntlr, uint8_t comboDev)
{
    int32_t ret;
    HDF_LOGI("%s: enter!", __func__);

    if ((cntlr == NULL) || (cntlr->ops == NULL)) {
        HDF_LOGE("%s: cntlr or ops is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops->resetRx == NULL) {
        HDF_LOGE("%s: resetRx is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->ops->resetRx(cntlr, comboDev);
    (void)OsalMutexUnlock(&(cntlr->lock));

    if (ret == HDF_SUCCESS) {
        HDF_LOGI("%s: success!", __func__);
    } else {
        HDF_LOGE("%s: failed!", __func__);
    }

    return ret;
}

int32_t MipiCsiCntlrUnresetRx(struct MipiCsiCntlr *cntlr, uint8_t comboDev)
{
    int32_t ret;
    HDF_LOGI("%s: enter!", __func__);

    if ((cntlr == NULL) || (cntlr->ops == NULL)) {
        HDF_LOGE("%s: cntlr or ops is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops->unresetRx == NULL) {
        HDF_LOGE("%s: unresetRx is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->ops->unresetRx(cntlr, comboDev);
    (void)OsalMutexUnlock(&(cntlr->lock));

    if (ret == HDF_SUCCESS) {
        HDF_LOGI("%s: success!", __func__);
    } else {
        HDF_LOGE("%s: failed!", __func__);
    }

    return ret;
}

int32_t MipiCsiCntlrEnableSensorClock(struct MipiCsiCntlr *cntlr, uint8_t snsClkSource)
{
    int32_t ret;
    HDF_LOGI("%s: enter!", __func__);

    if ((cntlr == NULL) || (cntlr->ops == NULL)) {
        HDF_LOGE("%s: cntlr or ops is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops->enableSensorClock == NULL) {
        HDF_LOGE("%s: enableSensorClock is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->ops->enableSensorClock(cntlr, snsClkSource);
    (void)OsalMutexUnlock(&(cntlr->lock));

    if (ret == HDF_SUCCESS) {
        HDF_LOGI("%s: success!", __func__);
    } else {
        HDF_LOGE("%s: failed!", __func__);
    }

    return ret;
}

int32_t MipiCsiCntlrDisableSensorClock(struct MipiCsiCntlr *cntlr, uint8_t snsClkSource)
{
    int32_t ret;
    HDF_LOGI("%s: enter!", __func__);

    if ((cntlr == NULL) || (cntlr->ops == NULL)) {
        HDF_LOGE("%s: cntlr or ops is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops->disableSensorClock == NULL) {
        HDF_LOGE("%s: disableSensorClock is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->ops->disableSensorClock(cntlr, snsClkSource);
    (void)OsalMutexUnlock(&(cntlr->lock));

    if (ret == HDF_SUCCESS) {
        HDF_LOGI("%s: success!", __func__);
    } else {
        HDF_LOGE("%s: failed!", __func__);
    }

    return ret;
}

int32_t MipiCsiCntlrResetSensor(struct MipiCsiCntlr *cntlr, uint8_t snsResetSource)
{
    int32_t ret;
    HDF_LOGI("%s: enter!", __func__);

    if ((cntlr == NULL) || (cntlr->ops == NULL)) {
        HDF_LOGE("%s: cntlr or ops is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops->resetSensor == NULL) {
        HDF_LOGE("%s: resetSensor is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->ops->resetSensor(cntlr, snsResetSource);
    (void)OsalMutexUnlock(&(cntlr->lock));

    if (ret == HDF_SUCCESS) {
        HDF_LOGI("%s: success!", __func__);
    } else {
        HDF_LOGE("%s: failed!", __func__);
    }

    return ret;
}

int32_t MipiCsiCntlrUnresetSensor(struct MipiCsiCntlr *cntlr, uint8_t snsResetSource)
{
    int32_t ret;
    HDF_LOGI("%s: enter!", __func__);

    if ((cntlr == NULL) || (cntlr->ops == NULL)) {
        HDF_LOGE("%s: cntlr or ops is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops->unresetSensor == NULL) {
        HDF_LOGE("%s: unresetSensor is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->ops->unresetSensor(cntlr, snsResetSource);
    (void)OsalMutexUnlock(&(cntlr->lock));

    if (ret == HDF_SUCCESS) {
        HDF_LOGI("%s: success!", __func__);
    } else {
        HDF_LOGE("%s: failed!", __func__);
    }

    return ret;
}

int32_t MipiCsiCntlrSetDrvData(struct MipiCsiCntlr *cntlr, void *drvData)
{
    int32_t ret;

    if ((cntlr == NULL) || (cntlr->ops == NULL)) {
        HDF_LOGE("%s: cntlr or ops is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->ops->setDrvData == NULL) {
        HDF_LOGE("%s: setDrvData is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->ops->setDrvData(cntlr, drvData);
    (void)OsalMutexUnlock(&(cntlr->lock));

    if (ret == HDF_SUCCESS) {
        HDF_LOGI("%s: success!", __func__);
    } else {
        HDF_LOGE("%s: failed!", __func__);
    }

    return ret;
}

int32_t MipiCsiDebugGetMipiDevCtx(struct MipiCsiCntlr *cntlr, MipiDevCtx *ctx)
{
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->debugs == NULL) {
        HDF_LOGE("%s: debugs is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (cntlr->debugs->getMipiDevCtx == NULL) {
        HDF_LOGE("%s: getMipiDevCtx is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (ctx == NULL) {
        HDF_LOGE("%s: ctx is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    cntlr->debugs->getMipiDevCtx(cntlr, ctx);
    (void)OsalMutexUnlock(&(cntlr->lock));
    HDF_LOGI("%s: success!", __func__);

    return HDF_SUCCESS;
}

int32_t MipiCsiDebugGetPhyErrIntCnt(struct MipiCsiCntlr *cntlr, unsigned int phyId, PhyErrIntCnt *errInfo)
{
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->debugs == NULL) {
        HDF_LOGE("%s: debugs is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (cntlr->debugs->getPhyErrIntCnt == NULL) {
        HDF_LOGE("%s: getPhyErrIntCnt is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (errInfo == NULL) {
        HDF_LOGE("%s: errInfo is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    cntlr->debugs->getPhyErrIntCnt(cntlr, phyId, errInfo);
    (void)OsalMutexUnlock(&(cntlr->lock));
    HDF_LOGI("%s: success!", __func__);

    return HDF_SUCCESS;
}

int32_t MipiCsiDebugGetMipiErrInt(struct MipiCsiCntlr *cntlr, unsigned int phyId, MipiErrIntCnt *errInfo)
{
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->debugs == NULL) {
        HDF_LOGE("%s: debugs is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (cntlr->debugs->getMipiErrInt == NULL) {
        HDF_LOGE("%s: getMipiErrInt is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (errInfo == NULL) {
        HDF_LOGE("%s: errInfo is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    cntlr->debugs->getMipiErrInt(cntlr, phyId, errInfo);
    (void)OsalMutexUnlock(&(cntlr->lock));
    HDF_LOGI("%s: success!", __func__);

    return HDF_SUCCESS;
}

int32_t MipiCsiDebugGetLvdsErrIntCnt(struct MipiCsiCntlr *cntlr, unsigned int phyId, LvdsErrIntCnt *errInfo)
{
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->debugs == NULL) {
        HDF_LOGE("%s: debugs is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (cntlr->debugs->getLvdsErrIntCnt == NULL) {
        HDF_LOGE("%s: getLvdsErrIntCnt is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (errInfo == NULL) {
        HDF_LOGE("%s: errInfo is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    cntlr->debugs->getLvdsErrIntCnt(cntlr, phyId, errInfo);
    (void)OsalMutexUnlock(&(cntlr->lock));
    HDF_LOGI("%s: success!", __func__);

    return HDF_SUCCESS;
}

int32_t MipiCsiDebugGetAlignErrIntCnt(struct MipiCsiCntlr *cntlr, unsigned int phyId, AlignErrIntCnt *errInfo)
{
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->debugs == NULL) {
        HDF_LOGE("%s: debugs is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (cntlr->debugs->getAlignErrIntCnt == NULL) {
        HDF_LOGE("%s: getAlignErrIntCnt is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (errInfo == NULL) {
        HDF_LOGE("%s: errInfo is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    cntlr->debugs->getAlignErrIntCnt(cntlr, phyId, errInfo);
    (void)OsalMutexUnlock(&(cntlr->lock));
    HDF_LOGI("%s: success!", __func__);

    return HDF_SUCCESS;
}

int32_t MipiCsiDebugGetPhyData(struct MipiCsiCntlr *cntlr, int phyId, int laneId, unsigned int *laneData)
{
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->debugs == NULL) {
        HDF_LOGE("%s: debugs is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (cntlr->debugs->getPhyData == NULL) {
        HDF_LOGE("%s: getPhyData is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (laneData == NULL) {
        HDF_LOGE("%s: laneData is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    cntlr->debugs->getPhyData(cntlr, phyId, laneId, laneData);
    (void)OsalMutexUnlock(&(cntlr->lock));
    HDF_LOGI("%s: success!", __func__);

    return HDF_SUCCESS;
}

int32_t MipiCsiDebugGetPhyMipiLinkData(struct MipiCsiCntlr *cntlr, int phyId, int laneId, unsigned int *laneData)
{
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->debugs == NULL) {
        HDF_LOGE("%s: debugs is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (cntlr->debugs->getPhyMipiLinkData == NULL) {
        HDF_LOGE("%s: getPhyMipiLinkData is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (laneData == NULL) {
        HDF_LOGE("%s: laneData is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    cntlr->debugs->getPhyMipiLinkData(cntlr, phyId, laneId, laneData);
    (void)OsalMutexUnlock(&(cntlr->lock));
    HDF_LOGI("%s: success!", __func__);

    return HDF_SUCCESS;
}

int32_t MipiCsiDebugGetPhyLvdsLinkData(struct MipiCsiCntlr *cntlr, int phyId, int laneId, unsigned int *laneData)
{
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->debugs == NULL) {
        HDF_LOGE("%s: debugs is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (cntlr->debugs->getPhyLvdsLinkData == NULL) {
        HDF_LOGE("%s: getPhyLvdsLinkData is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (laneData == NULL) {
        HDF_LOGE("%s: laneData is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    cntlr->debugs->getPhyLvdsLinkData(cntlr, phyId, laneId, laneData);
    (void)OsalMutexUnlock(&(cntlr->lock));
    HDF_LOGI("%s: success!", __func__);

    return HDF_SUCCESS;
}

int32_t MipiCsiDebugGetMipiImgsizeStatis(struct MipiCsiCntlr *cntlr, uint8_t devno, short vc, ImgSize *pSize)
{
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->debugs == NULL) {
        HDF_LOGE("%s: debugs is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (cntlr->debugs->getMipiImgsizeStatis == NULL) {
        HDF_LOGE("%s: getMipiImgsizeStatis is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (pSize == NULL) {
        HDF_LOGE("%s: pSize is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    cntlr->debugs->getMipiImgsizeStatis(cntlr, devno, vc, pSize);
    (void)OsalMutexUnlock(&(cntlr->lock));
    HDF_LOGI("%s: success!", __func__);

    return HDF_SUCCESS;
}

int32_t MipiCsiDebugGetLvdsImgsizeStatis(struct MipiCsiCntlr *cntlr, uint8_t devno, short vc, ImgSize *pSize)
{
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->debugs == NULL) {
        HDF_LOGE("%s: debugs is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (cntlr->debugs->getLvdsImgsizeStatis == NULL) {
        HDF_LOGE("%s: getLvdsImgsizeStatis is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (pSize == NULL) {
        HDF_LOGE("%s: pSize is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    cntlr->debugs->getLvdsImgsizeStatis(cntlr, devno, vc, pSize);
    (void)OsalMutexUnlock(&(cntlr->lock));
    HDF_LOGI("%s: success!", __func__);

    return HDF_SUCCESS;
}

int32_t MipiCsiDebugGetLvdsLaneImgsizeStatis(struct MipiCsiCntlr *cntlr, uint8_t devno, short lane, ImgSize *pSize)
{
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->debugs == NULL) {
        HDF_LOGE("%s: debugs is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (cntlr->debugs->getLvdsLaneImgsizeStatis == NULL) {
        HDF_LOGE("%s: getLvdsLaneImgsizeStatis is NULL.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (pSize == NULL) {
        HDF_LOGE("%s: pSize is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    cntlr->debugs->getLvdsLaneImgsizeStatis(cntlr, devno, lane, pSize);
    (void)OsalMutexUnlock(&(cntlr->lock));
    HDF_LOGI("%s: success!", __func__);

    return HDF_SUCCESS;
}