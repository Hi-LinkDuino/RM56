 /*
  * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
  *
  * HDF is dual licensed: you can use it either under the terms of
  * the GPL, or the BSD license, at your option.
  * See the LICENSE file in the root of this repository for complete details.
  */

#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#include "hcs_macro.h"
#include "hdf_config_macro.h"
#else
#include "device_resource_if.h"
#endif
#include "mmc_block.h"
#include "mmc_dispatch.h"
#include "mmc_emmc.h"
#include "mmc_sdio.h"

#define HDF_LOG_TAG mmc_core_c

#define MMC_POWER_ON_DELAY_TIME 10 /* 10ms */
#define MMC_OCR_MAX_VOLTAGE_BIT 23
#define MMC_OCR_MAX_SHIFT_BITS  3
#define MMC_DETECET_RETRY 5
#define MMC_REQUEST_RETRY 3
#define MMC_MAX_SECTOR_NUM 2048
#define MMC_MAX_ERASE_SECTOR 0x100000 /* 512M */
#define SDIO_IRQ_TASK_STACK_SIZE 0x2000

int32_t MmcCntlrDoRequest(struct MmcCntlr *cntlr, struct MmcCmd *cmd)
{
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cmd == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->ops == NULL || cntlr->ops->request == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    return cntlr->ops->request(cntlr, cmd);
}

static int32_t MmcCntlrExecRequest(struct MmcCntlr *cntlr, struct MmcCmd *cmd)
{
    uint32_t i;
    int32_t ret;

    for (i = 0; i < MMC_REQUEST_RETRY; i++) {
        MmcCntlrLock(cntlr);
        ret = MmcCntlrDoRequest(cntlr, cmd);
        MmcCntlrUnlock(cntlr);
        if (ret != HDF_SUCCESS) {
            continue;
        }
        if (cmd->returnError != HDF_SUCCESS) {
            continue;
        }
        if (cmd->data != NULL && cmd->data->returnError != HDF_SUCCESS) {
            continue;
        }
        break;
    }
    return ret;
}

static int32_t MmcCntlrPlug(struct MmcCntlr *cntlr)
{
    uint32_t i;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (MmcCntlrDevPluged(cntlr) != true) {
        HDF_LOGD("MmcCntlrPlug: dev is not plugged!");
        return HDF_SUCCESS;
    }

    /* dev not delete. */
    if (cntlr->curDev != NULL) {
        MmcCntlrLock(cntlr);
        MmcDeleteDev(cntlr);
        MmcCntlrUnlock(cntlr);
    }

    if (cntlr->ops != NULL && cntlr->ops->systemInit != NULL) {
        if (cntlr->ops->systemInit(cntlr) != HDF_SUCCESS) {
            HDF_LOGE("MmcCntlrPlug: system init fail!");
            return HDF_FAILURE;
        }
    }

    MmcCntlrLock(cntlr);
    for (i = 0; i < MMC_DETECET_RETRY; i++) {
        cntlr->detecting = true;
        if (MmcDoDetect(cntlr) == HDF_SUCCESS) {
            MmcDeviceAddOps(cntlr->curDev, NULL);
            cntlr->detecting = false;
            MmcCntlrUnlock(cntlr);
            return HDF_SUCCESS;
        }
    }
    cntlr->detecting = false;
    MmcCntlrUnlock(cntlr);

    return HDF_FAILURE;
}

static int32_t MmcCntlrUnplug(struct MmcCntlr *cntlr)
{
    if (cntlr == NULL) {
        HDF_LOGE("MmcCntlrUnplug: cntlr is null!");
        return HDF_ERR_INVALID_OBJECT;
    }

    MmcCntlrLock(cntlr);
    MmcDeleteDev(cntlr);
    MmcCntlrUnlock(cntlr);
    HDF_LOGD("host%d: a dev is removed!", cntlr->index);
    return HDF_SUCCESS;
}

static int32_t MmcCntlrSdioRescanHandle(struct MmcCntlr *cntlr)
{
    uint8_t val;
    int32_t error;

    if (cntlr == NULL) {
        HDF_LOGE("MmcCntlrSdioRescanHandle: cntlr is null.");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->curDev != NULL && cntlr->curDev->state.bits.present > 0) {
        if (cntlr->curDev->type == MMC_DEV_SDIO || cntlr->curDev->type == MMC_DEV_COMBO) {
            error = SdioReadCccrIoEnable(cntlr, &val);
            if (error != HDF_SUCCESS) {
                HDF_LOGD("re-detect sdio.");
                MmcCntlrPlug(cntlr);
            } else {
                HDF_LOGD("sdio no need to rescan.");
                return HDF_SUCCESS;
            }
        } else {
            HDF_LOGD("sd/emmc rescan does not support.");
        }
    } else {
        HDF_LOGD("init detect fail, re-detect sdio.");
        MmcCntlrPlug(cntlr);
    }

    if (cntlr->curDev == NULL) {
        HDF_LOGE("sdio rescan fail, please reset card!");
        return HDF_ERR_DEVICE_BUSY;
    }
    return HDF_SUCCESS;
}

static int32_t MmcMsgHandleDefault(struct PlatformQueue *queue, struct PlatformMsg *msg)
{
    int32_t ret;
    struct MmcCntlr *cntlr = NULL;
    struct MmcMsg *mmcMsg = NULL;

    if (queue == NULL || msg == NULL) {
        HDF_LOGE("MmcMsgHandleDefault: msg or queue is null!");
        return HDF_ERR_INVALID_OBJECT;
    }

    cntlr = (struct MmcCntlr *)queue->data;
    if (cntlr == NULL) {
        HDF_LOGE("MmcMsgHandleDefault: cntlr is null!");
        return HDF_ERR_INVALID_OBJECT;
    }
    mmcMsg = (struct MmcMsg *)msg;
    switch (msg->code) {
        case MMC_MSG_PLUG:
            ret = MmcCntlrPlug(cntlr);
            break;
        case MMC_MSG_UNPLUG:
            ret = MmcCntlrUnplug(cntlr);
            break;
        case MMC_MSG_REQUEST:
            ret = MmcCntlrExecRequest(cntlr, mmcMsg->mmcCmd);
            break;
        case MMC_MSG_SDIO_RESCAN:
            ret = MmcCntlrSdioRescanHandle(cntlr);
            break;
        default:
            ret = HDF_ERR_NOT_SUPPORT;
            break;
    }

    if (!mmcMsg->block) {
        OsalMemFree(mmcMsg);
    } else {
        (void)OsalSemPost(&mmcMsg->sem);
    }
    return ret;
}

static int32_t MmcCntlrQueueCreate(struct MmcCntlr *cntlr, bool needQueue)
{
    int32_t ret;
    const char *threadName = NULL;

    if (!needQueue) {
        return HDF_SUCCESS;
    }

    if (cntlr->devType == MMC_DEV_SDIO) {
        threadName = "SdioWorkerThread";
    } else if (cntlr->devType == MMC_DEV_SD) {
        threadName = "SdWorkerThread";
    } else if (cntlr->devType == MMC_DEV_EMMC) {
        threadName = "EmmcWorkerThread";
    }
    cntlr->msgQueue = PlatformQueueCreate(MmcMsgHandleDefault, threadName, cntlr);
    if (cntlr->msgQueue == NULL) {
        HDF_LOGE("MmcCntlrQueueCreate: failed to create msg queue!");
        return HDF_PLT_ERR_OS_API;
    }
    ret = PlatformQueueStart(cntlr->msgQueue);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("MmcCntlrQueueCreate: failed to start msg queue!");
        PlatformQueueDestroy(cntlr->msgQueue);
    }
    return ret;
}

static int32_t MmcCntlrInit(struct MmcCntlr *cntlr, bool needQueue)
{
    int32_t ret;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->index >= MMC_CNTLR_NR_MAX) {
        HDF_LOGE("MmcCntlrInit: invalid cntlr index:%u", cntlr->index);
        return HDF_ERR_INVALID_PARAM;
    }

    if (cntlr->hdfDevObj == NULL) {
        HDF_LOGE("MmcCntlrInit: no HdfDeviceObject attached!");
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = OsalMutexInit(&cntlr->mutex);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("MmcCntlrInit: mutex init fail!");
        return ret;
    }

    ret = MmcCntlrQueueCreate(cntlr, needQueue);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("MmcCntlrInit: queue create fail!");
        return ret;
    }

    cntlr->service.Dispatch = MmcIoDispatch;
    cntlr->hdfDevObj->service = &(cntlr->service);
    cntlr->device.number = cntlr->index;
    cntlr->device.hdfDev = cntlr->hdfDevObj;
    return HDF_SUCCESS;
}

static void MmcCntlrUninit(struct MmcCntlr *cntlr)
{
    if (cntlr != NULL) {
        (void)OsalMutexDestroy(&cntlr->mutex);
        PlatformQueueDestroy(cntlr->msgQueue);
    }
}

int32_t MmcCntlrAdd(struct MmcCntlr *cntlr, bool needQueue)
{
    int32_t ret;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = MmcCntlrInit(cntlr, needQueue);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    cntlr->device.manager = PlatformManagerGet(PLATFORM_MODULE_MMC);
    ret = PlatformDeviceAdd(&cntlr->device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("MmcCntlrAdd: device add fail!");
        MmcCntlrUninit(cntlr);
        return ret;
    }
    return HDF_SUCCESS;
}

void MmcCntlrRemove(struct MmcCntlr *cntlr)
{
    if (cntlr != NULL) {
        MmcCntlrUninit(cntlr);
        PlatformDeviceDel(&cntlr->device);
    }
}

void MmcCntlrSetDevice(struct MmcCntlr *cntlr, struct MmcDevice *mmc)
{
    if (cntlr == NULL || mmc == NULL) {
        return;
    }
    cntlr->curDev = mmc;
}

struct MmcDevice *MmcCntlrGetDevice(struct MmcCntlr *cntlr)
{
    struct MmcDevice *mmc = NULL;

    if (cntlr != NULL) {
        mmc = cntlr->curDev;
    }
    return MmcDeviceGet(mmc);
}

static int32_t MmcCntlrPostMsg(struct MmcCntlr *cntlr, struct MmcMsg *mmcMsg)
{
    int32_t ret;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (mmcMsg == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (mmcMsg->block) {
        (void)OsalSemInit(&mmcMsg->sem, 0);
    }
    (void)PlatformQueueAddMsg(cntlr->msgQueue, &mmcMsg->msg);
    if (!mmcMsg->block) {
        return HDF_SUCCESS;
    }

    ret = OsalSemWait(&mmcMsg->sem, HDF_WAIT_FOREVER);
    (void)OsalSemDestroy(&mmcMsg->sem);
    OsalMemFree(mmcMsg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("MmcCntlrPostMsg: wait msg failed:%d", ret);
    }
    return ret;
}

void MmcCntlrSetClock(struct MmcCntlr *cntlr, uint32_t clock)
{
    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->setClock == NULL) {
        return;
    }
    cntlr->curDev->workPara.clock = clock;
    cntlr->ops->setClock(cntlr, clock);
}

void MmcCntlrSetBusWidth(struct MmcCntlr *cntlr, enum MmcBusWidth width)
{
    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->setBusWidth == NULL) {
        return;
    }
    cntlr->curDev->workPara.width = width;
    cntlr->ops->setBusWidth(cntlr, width);
}

void MmcCntlrSetBusTiming(struct MmcCntlr *cntlr, enum MmcBusTiming timing)
{
    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->setBusTiming == NULL) {
        return;
    }
    cntlr->curDev->workPara.timing = timing;
    cntlr->ops->setBusTiming(cntlr, timing);
}

void MmcCntlrSetEnhanceSrobe(struct MmcCntlr *cntlr, bool enable)
{
    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->setEnhanceSrobe == NULL) {
        return;
    }
    cntlr->ops->setEnhanceSrobe(cntlr, enable);
}

int32_t MmcCntlrSwitchVoltage(struct MmcCntlr *cntlr, enum MmcVolt voltage)
{
    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->switchVoltage == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    return cntlr->ops->switchVoltage(cntlr, voltage);
}

bool MmcCntlrDevReadOnly(struct MmcCntlr *cntlr)
{
    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->devReadOnly == NULL) {
        return false;
    }
    return cntlr->ops->devReadOnly(cntlr);
}

bool MmcCntlrDevPluged(struct MmcCntlr *cntlr)
{
    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->devPluged == NULL) {
        return false;
    }

    if (cntlr->caps.bits.nonremovable > 0) {
        return true;
    }
    return cntlr->ops->devPluged(cntlr);
}

bool MmcCntlrDevBusy(struct MmcCntlr *cntlr)
{
    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->devBusy == NULL) {
        return true;
    }

    return cntlr->ops->devBusy(cntlr);
}

int32_t MmcCntlrTune(struct MmcCntlr *cntlr, uint32_t cmdCode)
{
    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->tune == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    return cntlr->ops->tune(cntlr, cmdCode);
}

void MmcCntlrSelectWorkVoltage(struct MmcCntlr *cntlr, union MmcOcr *ocr)
{
    uint32_t tmpOcr;
    uint32_t i;

    /* ignore reveserd. */
    if ((ocr->ocrData & 0x7F) > 0) {
        ocr->ocrData &= (~0x7F);
    }
    /* use low voltage shuould both host and dev support. */
    if (cntlr->ocrDef.bits.vdd1v65To1v95 == 0) {
        ocr->bits.vdd1v65To1v95 = 0;
    }

    /*
     * Based on the voltage range supported by the host and the voltage range read from the OCR register,
     * obtain the voltage range supported by both the host and the OCR register,
     * and then select the minimum voltage value.
     */
    tmpOcr = ((ocr->ocrData) & (cntlr->ocrDef.ocrData));
    for (i = 0; i <= MMC_OCR_MAX_VOLTAGE_BIT; i++) {
        if ((tmpOcr & (1 << i)) > 0) {
            break;
        }
    }

    if (i > 0 && i <= MMC_OCR_MAX_VOLTAGE_BIT) {
        tmpOcr &= (MMC_OCR_MAX_SHIFT_BITS << i);
        cntlr->vddBit = i;
    } else {
        tmpOcr = 0;
    }
    cntlr->curDev->reg.ocr.ocrData = tmpOcr;
}

void MmcCntlrPowerUp(struct MmcCntlr *cntlr)
{
    if (cntlr == NULL || cntlr->ops == NULL) {
        return;
    }

    if (cntlr->ops->setEnhanceSrobe != NULL) {
        cntlr->ops->setEnhanceSrobe(cntlr, false);
    }
    /* disable clock. */
    if (cntlr->ops->setClock != NULL) {
        cntlr->ops->setClock(cntlr, 0);
    }
    /* power up. */
    if (cntlr->ops->setPowerMode != NULL) {
        cntlr->ops->setPowerMode(cntlr, MMC_POWER_MODE_POWER_UP);
    }
    if (cntlr->ops->setBusWidth != NULL) {
        cntlr->ops->setBusWidth(cntlr, BUS_WIDTH1);
    }
    if (cntlr->ops->setBusTiming != NULL) {
        cntlr->ops->setBusTiming(cntlr, BUS_TIMING_MMC_DS);
    }
    OsalMDelay(MMC_POWER_ON_DELAY_TIME);

    /* enable clock. */
    if (cntlr->ops->setClock != NULL) {
        cntlr->ops->setClock(cntlr, cntlr->freqDef);
    }
    /* power on. */
    if (cntlr->ops->setPowerMode != NULL) {
        cntlr->ops->setPowerMode(cntlr, MMC_POWER_MODE_POWER_ON);
    }
    OsalMDelay(MMC_POWER_ON_DELAY_TIME);

    if (cntlr->ops->hardwareReset != NULL && cntlr->caps.bits.hardwareReset > 0) {
        cntlr->ops->hardwareReset(cntlr);
    }
    /* init voltage 3.3v. */
    if (cntlr->ops->switchVoltage != NULL) {
        cntlr->ops->switchVoltage(cntlr, cntlr->voltDef);
    }
}

void MmcCntlrPowerOff(struct MmcCntlr *cntlr)
{
    if (cntlr == NULL || cntlr->ops == NULL) {
        return;
    }

    /* disable clock. */
    if (cntlr->ops->setClock != NULL) {
        cntlr->ops->setClock(cntlr, 0);
    }
    /* power up. */
    if (cntlr->ops->setPowerMode != NULL) {
        cntlr->ops->setPowerMode(cntlr, MMC_POWER_MODE_POWER_OFF);
    }
    if (cntlr->ops->setBusWidth != NULL) {
        cntlr->ops->setBusWidth(cntlr, BUS_WIDTH1);
    }
}

int32_t MmcCntlrAllocDev(struct MmcCntlr *cntlr, enum MmcDevType devType)
{
    uint32_t len = 0;
    struct MmcDevice *mmc = NULL;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (devType == MMC_DEV_SDIO || devType == MMC_DEV_COMBO) {
        len = (uint32_t)sizeof(struct SdioDevice);
    } else if (devType == MMC_DEV_SD) {
        len = (uint32_t)sizeof(struct SdDevice);
    } else if (devType == MMC_DEV_EMMC) {
        len = (uint32_t)sizeof(struct EmmcDevice);
    }

    if (len == 0) {
        HDF_LOGE("MmcCntlrAllocDev: len is 0, type = %d!", (uint32_t)devType);
        return HDF_ERR_INVALID_PARAM;
    }

    mmc = (struct MmcDevice *)OsalMemCalloc(len);
    if (mmc == NULL) {
        HDF_LOGE("MmcCntlrAllocDev: OsalMemCalloc fail!");
        return HDF_ERR_MALLOC_FAIL;
    }
    mmc->type = devType;
    mmc->cntlr = cntlr;
    MmcCntlrSetDevice(cntlr, mmc);
    return 0;
}

void MmcCntlrFreeDev(struct MmcCntlr *cntlr)
{
    if (cntlr == NULL || cntlr->curDev == NULL) {
        return;
    }

    OsalMemFree(cntlr->curDev);
    cntlr->curDev = NULL;
}

bool MmcCntlrSupportUhs(struct MmcCntlr *cntlr)
{
    if (cntlr == NULL) {
        return false;
    }

    if (cntlr->caps.bits.uhsSdr12 > 0 ||
        cntlr->caps.bits.uhsSdr25 > 0 ||
        cntlr->caps.bits.uhsSdr50 > 0 ||
        cntlr->caps.bits.uhsSdr104 > 0 ||
        cntlr->caps.bits.uhsDdr50 > 0) {
        return true;
    }
    return false;
}

bool MmcCntlrSupportHighSpeed400EnhancedStrobe(struct MmcCntlr *cntlr)
{
    if (cntlr == NULL) {
        return false;
    }

    if (cntlr->caps2.bits.hs400EnhancedStrobe > 0 &&
        cntlr->caps.bits.cap8Bit > 0 &&
        cntlr->caps2.bits.hs400Support1v2 > 0 &&
        cntlr->caps2.bits.hs400Support1v8 > 0) {
        return true;
    }
    return false;
}

bool MmcCntlrSupportHighSpeed400(struct MmcCntlr *cntlr)
{
    if (cntlr == NULL) {
        return false;
    }

    if (cntlr->caps.bits.cap8Bit > 0 &&
        cntlr->caps2.bits.hs400Support1v2 > 0 &&
        cntlr->caps2.bits.hs400Support1v8 > 0) {
        return true;
    }
    return false;
}

bool MmcCntlrSupportHighSpeed200(struct MmcCntlr *cntlr)
{
    if (cntlr == NULL) {
        return false;
    }

    if (cntlr->caps2.bits.hs200Sdr1v8 > 0 ||
        cntlr->caps2.bits.hs200Sdr1v2 > 0) {
        return true;
    }
    return false;
}

bool MmcCntlrSdSupportCmd23(struct MmcCntlr *cntlr)
{
    struct SdDevice *dev = NULL;

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return false;
    }
    if (cntlr->curDev->type != MMC_DEV_SD && cntlr->curDev->type != MMC_DEV_COMBO) {
        return false;
    }

    dev = (struct SdDevice *)cntlr->curDev;
    if ((dev->reg.scr.cmdSupport & SD_SCR_SET_BLOCK_COUNT_SUPPORT) > 0) {
        return true;
    }
    return false;
}

bool MmcCntlrEmmcSupportCmd23(struct MmcCntlr *cntlr)
{
    struct EmmcDevice *dev = NULL;

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return false;
    }
    if (cntlr->curDev->type != MMC_DEV_EMMC) {
        return false;
    }

    dev = (struct EmmcDevice *)cntlr->curDev;
    if ((dev->mmc.reg.csd.ccc & MMC_CSD_CCC_BLOCK_READ) > 0 ||
        (dev->mmc.reg.csd.ccc & MMC_CSD_CCC_BLOCK_WRITE) > 0) {
        return true;
    }
    return false;
}

int32_t MmcCntlrAddMsgToQueue(struct MmcCntlr *cntlr, struct MmcCmd *cmd,
    int32_t code, bool block)
{
    struct MmcMsg *msg = NULL;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    msg = (struct MmcMsg *)OsalMemCalloc(sizeof(struct MmcMsg));
    if (msg == NULL) {
        HDF_LOGE("MmcCntlrAddMsgToQueue: OsalMemCalloc fail!n");
        return HDF_ERR_MALLOC_FAIL;
    }
    msg->msg.code = code;
    msg->msg.data = (void *)cntlr;
    msg->block = block;
    msg->mmcCmd = cmd;
    return MmcCntlrPostMsg(cntlr, msg);
}

int32_t MmcCntlrAddRequestMsgToQueue(struct MmcCntlr *cntlr, struct MmcCmd *cmd)
{
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops == NULL || cntlr->ops->devPluged == NULL) {
        return HDF_PLT_ERR_NO_DEV;
    }
    if (cntlr->ops->devPluged(cntlr) == false) {
        HDF_LOGE("MmcCntlrAddRequestMsgToQueue: host%d dev is unplug!", cntlr->index);
        return HDF_PLT_ERR_NO_DEV;
    }

    return MmcCntlrAddMsgToQueue(cntlr, cmd, MMC_MSG_REQUEST, true);
}

int32_t MmcCntlrAddDetectMsgToQueue(struct MmcCntlr *cntlr)
{
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops == NULL || cntlr->ops->devPluged == NULL) {
        return HDF_PLT_ERR_NO_DEV;
    }

    if (cntlr->ops->devPluged(cntlr) == false) {
        HDF_LOGD("MmcCntlrAddDetectMsgToQueue: host%d dev is not plugged!", cntlr->index);
        return HDF_PLT_ERR_NO_DEV;
    }
    cntlr->devPluged = true;
    return MmcCntlrAddMsgToQueue(cntlr, NULL, MMC_MSG_PLUG, false);
}

int32_t MmcCntlrAddPlugMsgToQueue(struct MmcCntlr *cntlr)
{
    bool curStatus = false;
    int32_t code;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops == NULL || cntlr->ops->devPluged == NULL) {
        return HDF_PLT_ERR_NO_DEV;
    }

    curStatus = cntlr->ops->devPluged(cntlr);
    if (curStatus == cntlr->devPluged) {
        HDF_LOGD("MmcCntlrAddPlugMsgToQueue: dev plug srtatus not change.");
        return HDF_SUCCESS;
    }

    if (curStatus == true) {
        cntlr->devPluged = true;
        code = MMC_MSG_PLUG;
        HDF_LOGD("host%d: a dev is plugged!", cntlr->index);
    } else {
        cntlr->devPluged = false;
        code = MMC_MSG_UNPLUG;
        HDF_LOGD("host%d: a dev is unplugged!", cntlr->index);
    }
    return MmcCntlrAddMsgToQueue(cntlr, NULL, code, false);
}

int32_t MmcCntlrAddSdioRescanMsgToQueue(struct MmcCntlr *cntlr)
{
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->devType != MMC_DEV_SDIO) {
        HDF_LOGD("MmcCntlrAddSdioRescanMsgToQueue: not sdio card, do not handle rescan!");
        return HDF_FAILURE;
    }

    return MmcCntlrAddMsgToQueue(cntlr, NULL, MMC_MSG_SDIO_RESCAN, true);
}

static void SdioHandlePendingIrq(struct MmcCntlr *cntlr, struct SdioDevice *dev)
{
    uint8_t val;
    int32_t ret;
    uint32_t i;
    struct SdioFunction *func = dev->curFunction;

    if (func == NULL) {
        HDF_LOGE("MmcCntlrHandleSdioPendingIrq: cur func is NULL.");
        return;
    }
    if (dev->irqPending == true && func->irqHandler != NULL) {
        func->irqHandler(func);
        return;
    }

    ret = SdioReadCccrIntPending(cntlr, &val);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("MmcCntlrHandleSdioPendingIrq: read cccr int pending fail! ret = %d.", ret);
        return;
    }

    for (i = 0; i < SDIO_MAX_FUNCTION_NUMBER; i++) {
        if ((val & (1 << (i + 1))) == 0) {
            continue;
        }
        func = dev->sdioFunc[i];
        if (func == NULL) {
            HDF_LOGD("MmcCntlrHandleSdioPendingIrq: function%d not exist.", i + 1);
            continue;
        }
        if (func->irqHandler == NULL) {
            HDF_LOGD("MmcCntlrHandleSdioPendingIrq: function%d irq handler not exist.", i + 1);
            continue;
        }
        func->irqHandler(func);
    }
}

static int32_t SdioIrqThreadWorker(void *data)
{
    int32_t ret;
    struct SdioDevice *dev = (struct SdioDevice *)data;
    struct MmcCntlr *cntlr = NULL;

    if (dev == NULL) {
        HDF_LOGE("SdioIrqThreadWorker: data is NULL.");
        return HDF_FAILURE;
    }

    cntlr = dev->sd.mmc.cntlr;
    if (cntlr == NULL) {
        HDF_LOGE("SdioIrqThreadWorker: cntlr is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    while (true) {
        /* wait envent */
        ret = OsalSemWait(&dev->sem, HDF_WAIT_FOREVER);
        if (ret != HDF_SUCCESS) {
            continue;
        }
        SdioHandlePendingIrq(cntlr, dev);
        dev->irqPending = false;
        if (cntlr->caps.bits.sdioIrq > 0 && cntlr->ops != NULL && cntlr->ops->setSdioIrq != NULL) {
            (void)cntlr->ops->setSdioIrq(cntlr, true);
        }
    }

    return HDF_SUCCESS;
}

int32_t MmcCntlrCreatSdioIrqThread(struct MmcCntlr *cntlr)
{
    int32_t ret;
    struct OsalThreadParam config = {0};
    struct SdioDevice *dev = NULL;

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    dev = (struct SdioDevice *)cntlr->curDev;
    ret = OsalSemInit(&dev->sem, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("MmcCntlrCreatSdioIrqThread: sem init fail.");
        return ret;
    }

    ret = OsalThreadCreate(&dev->thread, (OsalThreadEntry)SdioIrqThreadWorker, dev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("MmcCntlrCreatSdioIrqThread: thread create fail.");
        (void)OsalSemDestroy(&dev->sem);
        return ret;
    }

    config.name = "SdioIrqTask";
    config.priority = OSAL_THREAD_PRI_HIGHEST;
    config.stackSize = SDIO_IRQ_TASK_STACK_SIZE;
    ret = OsalThreadStart(&dev->thread, &config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("MmcCntlrCreatSdioIrqThread: thread start fail.");
        OsalThreadDestroy(&dev->thread);
        (void)OsalSemDestroy(&dev->sem);
        return ret;
    }
    dev->threadRunning = true;
    if (cntlr->caps.bits.sdioIrq > 0 && cntlr->ops != NULL && cntlr->ops->setSdioIrq != NULL) {
        (void)cntlr->ops->setSdioIrq(cntlr, true);
    }
    return HDF_SUCCESS;
}

void MmcCntlrDestroySdioIrqThread(struct MmcCntlr *cntlr)
{
    struct SdioDevice *dev = NULL;

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return;
    }

    dev = (struct SdioDevice *)cntlr->curDev;
    (void)OsalThreadDestroy(&dev->thread);
    (void)OsalSemDestroy(&dev->sem);
    dev->threadRunning = true;
}

void MmcCntlrNotifySdioIrqThread(struct MmcCntlr *cntlr)
{
    struct SdioDevice *dev = NULL;

    if (cntlr == NULL || cntlr->curDev == NULL) {
        HDF_LOGD("MmcCntlrNotifySdioIrqThread: cntlr or curDev is null!");
        return;
    }
    if (cntlr->curDev->type != MMC_DEV_SDIO && cntlr->curDev->type != MMC_DEV_COMBO) {
        return;
    }
    dev = (struct SdioDevice *)cntlr->curDev;
    if (dev->threadRunning == true) {
        dev->irqPending = true;
        /* notify the worker thread */
        (void)OsalSemPost(&dev->sem);
    }
}

#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#define MMC_PARSE_DEVICE_INFO(node, cntlr) \
    do { \
        cntlr->index = HCS_PROP(node, hostId); \
        cntlr->devType = HCS_PROP(node, devType); \
    } while (0)

#define MMC_PARSE_CNTLR_FREQ_INFO(node, cntlr) \
    do { \
        cntlr->freqMin = HCS_PROP(node, freqMin); \
        cntlr->freqMax = HCS_PROP(node, freqMax); \
        cntlr->freqDef = HCS_PROP(node, freqDef); \
    } while (0)

#define MMC_PARSE_CNTLR_CAPS_INFO(node, cntlr) \
    do { \
        cntlr->voltDef = HCS_PROP(node, voltDef); \
        cntlr->ocrDef.ocrData = HCS_PROP(node, ocrDef); \
        cntlr->caps.capsData = HCS_PROP(node, caps); \
        cntlr->caps2.caps2Data = HCS_PROP(node, caps2); \
        cntlr->maxBlkNum = HCS_PROP(node, maxBlkNum); \
        cntlr->maxBlkSize = HCS_PROP(node, maxBlkSize); \
        cntlr->maxReqSize = cntlr->maxBlkNum * cntlr->maxBlkSize; \
    } while (0)

#define MMC_FIND_CONFIG(node, name, cntlr) \
    do { \
        if (strcmp(HCS_PROP(node, match_attr), name) == 0) { \
            MMC_PARSE_DEVICE_INFO(node, cntlr); \
            MMC_PARSE_CNTLR_FREQ_INFO(node, cntlr); \
            MMC_PARSE_CNTLR_CAPS_INFO(node, cntlr); \
            break; \
        } \
    } while (0)

#define PLATFORM_CONFIG HCS_NODE(HCS_ROOT, platform)
#define MMC_CNTLR_PARSE_CONFIG HCS_NODE(HCS_NODE(HCS_ROOT, platform), mmc_config)

int32_t MmcCntlrParse(struct MmcCntlr *cntlr, struct HdfDeviceObject *obj)
{
    if (cntlr == NULL || obj == NULL) {
        HDF_LOGE("MmcCntlrParse: input param is NULL.");
        return HDF_FAILURE;
    }

    /* parse hcs config. */
#if HCS_NODE_HAS_PROP(PLATFORM_CONFIG, mmc_config)
    HCS_FOREACH_CHILD_VARGS(MMC_CNTLR_PARSE_CONFIG, MMC_FIND_CONFIG, obj->deviceMatchAttr, cntlr);
#endif

    return HDF_SUCCESS;
}
#else
static void MmcCntlrParseCapability(const struct DeviceResourceNode *node,
    struct DeviceResourceIface *drsOps, struct MmcCntlr *cntlr)
{
    int32_t ret;

    ret = drsOps->GetUint16(node, "voltDef", &(cntlr->voltDef), 0);
    if (ret != HDF_SUCCESS) {
        cntlr->voltDef = 0;
    }
    ret = drsOps->GetUint32(node, "freqMin", &(cntlr->freqMin), 0);
    if (ret != HDF_SUCCESS) {
        cntlr->freqMin = 0;
    }
    ret = drsOps->GetUint32(node, "freqMax", &(cntlr->freqMax), 0);
    if (ret != HDF_SUCCESS) {
        cntlr->freqMax = 0;
    }
    ret = drsOps->GetUint32(node, "freqDef", &(cntlr->freqDef), 0);
    if (ret != HDF_SUCCESS) {
        cntlr->freqDef = 0;
    }
    ret = drsOps->GetUint32(node, "ocrDef", &(cntlr->ocrDef.ocrData), 0);
    if (ret != HDF_SUCCESS) {
        cntlr->ocrDef.ocrData = 0;
    }

    ret = drsOps->GetUint32(node, "caps", &(cntlr->caps.capsData), 0);
    if (ret != HDF_SUCCESS) {
        cntlr->caps.capsData = 0;
    }
    ret = drsOps->GetUint32(node, "caps2", &(cntlr->caps2.caps2Data), 0);
    if (ret != HDF_SUCCESS) {
        cntlr->caps2.caps2Data = 0;
    }

    ret = drsOps->GetUint32(node, "maxBlkNum", &(cntlr->maxBlkNum), 0);
    if (ret != HDF_SUCCESS) {
        cntlr->maxBlkNum = 0;
    }
    ret = drsOps->GetUint32(node, "maxBlkSize", &(cntlr->maxBlkSize), 0);
    if (ret != HDF_SUCCESS) {
        cntlr->maxBlkSize = 0;
    }
    cntlr->maxReqSize = cntlr->maxBlkNum * cntlr->maxBlkSize;
}

int32_t MmcCntlrParse(struct MmcCntlr *cntlr, struct HdfDeviceObject *obj)
{
    const struct DeviceResourceNode *node = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;

    if (obj == NULL || cntlr == NULL) {
        HDF_LOGE("MmcCntlrParse: input param is NULL.");
        return HDF_FAILURE;
    }

    node = obj->property;
    if (node == NULL) {
        HDF_LOGE("MmcCntlrParse: drs node is NULL.");
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint16 == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("MmcCntlrParse: invalid drs ops fail!");
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint16(node, "hostId", &(cntlr->index), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("MmcCntlrParse: read hostId fail!");
        return ret;
    }
    HDF_LOGD("MmcCntlrParse: hostId = %d", cntlr->index);

    ret = drsOps->GetUint32(node, "devType", &(cntlr->devType), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("MmcCntlrParse: read devType fail!");
        return ret;
    }
    HDF_LOGD("MmcCntlrParse: devType = %d", cntlr->devType);

    MmcCntlrParseCapability(node, drsOps, cntlr);
    return HDF_SUCCESS;
}
#endif

int32_t MmcDeviceAdd(struct MmcDevice *mmc)
{
    int32_t ret;

    if (mmc == NULL || mmc->cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (mmc->type >= MMC_DEV_INVALID) {
        return HDF_PLT_ERR_DEV_TYPE;
    }
    if (mmc->secSize == 0) {
        mmc->secSize = MMC_SEC_SIZE;
    } else {
        HDF_LOGE("MmcDeviceAdd: invalid sector size:%u", mmc->secSize);
        return HDF_ERR_INVALID_PARAM;
    }
    if (mmc->type != MMC_DEV_SDIO && mmc->capacity == 0) {
        //HDF_LOGE("MmcDeviceAdd: invalid capacity:%u", mmc->capacity);
        //return HDF_ERR_INVALID_PARAM;
    }
    if (mmc->type != MMC_DEV_SDIO && mmc->eraseSize == 0) {
        HDF_LOGE("MmcDeviceAdd: invalid erase size:%u", mmc->eraseSize);
        return HDF_ERR_INVALID_PARAM;
    }
    mmc->device.number = mmc->cntlr->device.number + MMC_CNTLR_NR_MAX;

    if (MmcCntlrGet(mmc->cntlr) == NULL) {
        return HDF_PLT_ERR_DEV_GET;
    }

    mmc->device.manager = PlatformManagerGet(PLATFORM_MODULE_MMC);
    ret = PlatformDeviceAdd(&mmc->device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("MmcDeviceAdd: mmc device add fail");
        MmcCntlrPut(mmc->cntlr);
        return ret;
    }

    if (mmc->type == MMC_DEV_EMMC || mmc->type == MMC_DEV_SD || mmc->type == MMC_DEV_COMBO) {
        ret = MmcBlockInit(mmc);
        if (ret != HDF_SUCCESS) {
            MmcCntlrPut(mmc->cntlr);
            PlatformDeviceDel(&mmc->device);
            return ret;
        }
    }
    return HDF_SUCCESS;
}

void MmcDeviceRemove(struct MmcDevice *mmc)
{
    if (mmc == NULL) {
        return;
    }
    if (mmc->type == MMC_DEV_EMMC || mmc->type == MMC_DEV_SD || mmc->type == MMC_DEV_COMBO) {
        MmcBlockUninit(mmc);
    }
    PlatformDeviceDel(&mmc->device);
    MmcCntlrPut(mmc->cntlr);
}

struct MmcDevice *MmcDeviceGet(struct MmcDevice *mmc)
{
    if (mmc == NULL) {
        return NULL;
    }

    if (PlatformDeviceGet(&mmc->device) != HDF_SUCCESS) {
        HDF_LOGE("MmcDeviceAdd: get device ref fail!");
        return NULL;
    }
    return mmc;
}

void MmcDevicePut(struct MmcDevice *mmc)
{
    if (mmc == NULL) {
        return;
    }
    PlatformDevicePut(&mmc->device);
}

void MmcDeviceAddOps(struct MmcDevice *mmc, void *ops)
{
    if (mmc->type >= MMC_DEV_INVALID) {
        HDF_LOGE("MmcDeviceAddOps: dev type error!");
        return;
    }

    if (mmc->type == MMC_DEV_SDIO || mmc->type == MMC_DEV_COMBO) {
        SdioDeviceAddOps((struct SdioDevice *)mmc, ops);
        return;
    }

    if (mmc->type == MMC_DEV_EMMC) {
        EmmcDeviceAddOps((struct EmmcDevice *)mmc, (struct EmmcDeviceOps *)ops);
    }
}

static void MmcDeviceFillRwInfo(struct MmcRwData *info, uint8_t *buf,
    bool writeFlag, size_t startSec, size_t nSec)
{
    info->buf = buf;
    info->writeFlag = writeFlag;
    info->startSector = startSec;
    info->sectors = nSec;
}

ssize_t MmcDeviceRead(struct MmcDevice *mmc, uint8_t *buf, size_t startSec, size_t nSec)
{
    struct MmcCmd cmd = {0};
    struct MmcData data = {0};
    struct MmcRwData info = {0};
    size_t curSec = nSec;
    size_t curStartSec = startSec;
    size_t readSec;
    ssize_t ret;

    if (mmc == NULL) {
        HDF_LOGE("MmcDeviceRead: mmc is null!");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (buf == NULL) {
        HDF_LOGE("MmcDeviceRead: buf is null!");
        return HDF_ERR_INVALID_PARAM;
    }

    do {
        if (curSec >= MMC_MAX_SECTOR_NUM) {
            readSec = MMC_MAX_SECTOR_NUM;
        } else {
            readSec = curSec;
        }
        MmcDeviceFillRwInfo(&info, buf, false, curStartSec, readSec);
        if (mmc->cntlr->detecting == true) {
            /* In SD device detecting, VFS will read/write SD. */
            ret = MmcSendReadWriteBlocks(mmc->cntlr, &info);
        } else {
            cmd.data = &data;
            MmcSetupReadWriteBlocksCmd(mmc, &cmd, &info);
            ret = MmcCntlrAddRequestMsgToQueue(mmc->cntlr, &cmd);
        }
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("MmcDeviceRead: fail!");
            return ret;
        }
        curSec -= readSec;
        curStartSec += readSec;
        buf += (readSec * BYTES_PER_BLOCK);
    } while (curSec > 0);

    return nSec;
}

ssize_t MmcDeviceWrite(struct MmcDevice *mmc, uint8_t *buf, size_t startSec, size_t nSec)
{
    struct MmcCmd cmd = {0};
    struct MmcData data = {0};
    struct MmcRwData info = {0};
    size_t curSec = nSec;
    size_t curStartSec = startSec;
    size_t writeSec;
    ssize_t ret;

    if (mmc == NULL) {
        HDF_LOGE("MmcDeviceWrite: mmc is null!");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (buf == NULL) {
        HDF_LOGE("MmcDeviceWrite: buf is null!");
        return HDF_ERR_INVALID_PARAM;
    }

    do {
        if (curSec >= MMC_MAX_SECTOR_NUM) {
            writeSec = MMC_MAX_SECTOR_NUM;
        } else {
            writeSec = curSec;
        }
        MmcDeviceFillRwInfo(&info, buf, true, curStartSec, writeSec);
        if (mmc->cntlr->detecting == true) {
            /* In SD device detecting, VFS will read/write SD. */
            ret = MmcSendReadWriteBlocks(mmc->cntlr, &info);
        } else {
            cmd.data = &data;
            MmcSetupReadWriteBlocksCmd(mmc, &cmd, &info);
            ret = MmcCntlrAddRequestMsgToQueue(mmc->cntlr, &cmd);
        }
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("MmcDeviceWrite: fail!");
            return ret;
        }
        curSec -= writeSec;
        curStartSec += writeSec;
        buf += (writeSec * BYTES_PER_BLOCK);
    } while (curSec > 0);

    return nSec;
}

ssize_t MmcDeviceErase(struct MmcDevice *mmc, size_t startSec, size_t nSec)
{
    size_t curSec = nSec;
    size_t curStartSec = startSec;
    size_t eraseSec;
    ssize_t ret;

    if (mmc == NULL) {
        HDF_LOGE("MmcDeviceErase: mmc is null!");
        return HDF_ERR_INVALID_OBJECT;
    }

    do {
        if (curSec > MMC_MAX_ERASE_SECTOR) {
            eraseSec = MMC_MAX_ERASE_SECTOR;
        } else {
            eraseSec = curSec;
        }
        ret = MmcSendErase(mmc->cntlr, curStartSec, eraseSec);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("MmcDeviceErase: fail!");
            return ret;
        }
        curSec -= eraseSec;
        curStartSec += eraseSec;
    } while (curSec > 0);

    return HDF_SUCCESS;
}