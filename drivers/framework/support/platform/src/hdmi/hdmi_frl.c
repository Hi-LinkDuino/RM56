/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_log.h"
#include "hdmi_common.h"
#include "hdmi_core.h"
#include "hdmi_dfm.h"
#include "hdmi_ncts.h"

#define HDMI_FRL_TRAIN_STEP_RESULT_HANDLE_MAX_TRIES 3
#define HDMI_FRL_TRAIN_STEP_READR_CHECK_MAX_TRIES 10
#define HDMI_FRL_CHECK_SINK_STATUS_DELAY_TIME 2000

#define HDF_LOG_TAG hdmi_frl_c

static uint8_t HdmiFrlGetSinkVersion(struct HdmiScdc *scdc)
{
    uint8_t version = 0;

    if (HdmiScdcOptMsgHandle(scdc, HDMI_SCDC_OPT_GET_SINK_VER, &version, sizeof(version)) != HDF_SUCCESS) {
        HDF_LOGD("frl get sink version fail");
        return 0;
    }
    return version;
}

static bool HdmiFrlGetFltUpdate(struct HdmiScdc *scdc)
{
    bool fltUpdate = false;
    uint8_t flag = 0;

    if (HdmiScdcOptMsgHandle(scdc, HDMI_SCDC_OPT_GET_FLT_UPDATE, &flag, sizeof(flag)) != HDF_SUCCESS) {
        HDF_LOGD("frl get flt update fail");
        return fltUpdate;
    }

    fltUpdate = (flag & HDMI_SCDC_FLT_UPDATE_MARK) ? true : false;
    return fltUpdate;
}

static bool HdmiFrlGetFrlStart(struct HdmiScdc *scdc)
{
    bool frlStart = false;
    uint8_t flag = 0;

    if (HdmiScdcOptMsgHandle(scdc, HDMI_SCDC_OPT_GET_FLT_UPDATE, &flag, sizeof(flag)) != HDF_SUCCESS) {
        HDF_LOGD("frl get frl start fail");
        return frlStart;
    }

    frlStart = (flag & HDMI_SCDC_FRL_START_MARK) ? true : false;
    return frlStart;
}

static bool HdmiFrlGetFltReady(struct HdmiScdc *scdc)
{
    bool fltReady = false;
    uint8_t flag = 0;

    if (HdmiScdcOptMsgHandle(scdc, HDMI_SCDC_OPT_GET_FLT_READY, &flag, sizeof(flag)) != HDF_SUCCESS) {
        HDF_LOGD("frl get flt ready fail");
        return fltReady;
    }

    fltReady = (flag & HDMI_SCDC_FLT_READY_MARK) ? true : false;
    return fltReady;
}

static uint8_t HdmiFrlGetTestConfig1(struct HdmiScdc *scdc)
{
    uint8_t cfg = 0;

    if (HdmiScdcOptMsgHandle(scdc, HDMI_SCDC_OPT_GET_TEST_CONFIG_1, &cfg, sizeof(cfg)) != HDF_SUCCESS) {
        HDF_LOGD("frl get test cfg1 fail");
        return 0;
    }
    return cfg;
}

static void HdmiFrlSetFltUpdate(struct HdmiScdc *scdc, bool fltUpdate)
{
    uint8_t flag = 0;

    if (HdmiScdcOptMsgHandle(scdc, HDMI_SCDC_OPT_GET_FLT_UPDATE, &flag, sizeof(flag)) != HDF_SUCCESS) {
        HDF_LOGD("frl get flt update fail");
        return;
    }

    if (fltUpdate == true) {
        flag |= HDMI_SCDC_FLT_UPDATE_MARK;
    } else {
        flag &= (~HDMI_SCDC_FLT_UPDATE_MARK);
    }
    if (HdmiScdcOptMsgHandle(scdc, HDMI_SCDC_OPT_SET_FLT_UPDATE, &flag, sizeof(flag)) != HDF_SUCCESS) {
        HDF_LOGD("frl set flt update fail");
    }
}

static void HdmiFrlSetFrlStart(struct HdmiScdc *scdc, bool frlStart)
{
    uint8_t flag = 0;

    if (HdmiScdcOptMsgHandle(scdc, HDMI_SCDC_OPT_GET_FLT_UPDATE, &flag, sizeof(flag)) != HDF_SUCCESS) {
        HDF_LOGD("frl get frl start fail");
        return;
    }

    if (frlStart == true) {
        flag |= HDMI_SCDC_FRL_START_MARK;
    } else {
        flag &= (~HDMI_SCDC_FRL_START_MARK);
    }

    if (HdmiScdcOptMsgHandle(scdc, HDMI_SCDC_OPT_GET_FLT_UPDATE, &flag, sizeof(flag)) != HDF_SUCCESS) {
        HDF_LOGD("frl set frl start fail");
    }
}

static void HdmiFrlSetConfig1(struct HdmiScdc *scdc, uint8_t *data)
{
    if (HdmiScdcOptMsgHandle(scdc, HDMI_SCDC_OPT_SET_CONFIG1, data, sizeof(*data)) != HDF_SUCCESS) {
        HDF_LOGD("frl set cfg1 fail");
    }
}

static void HdmiFrlSetTrainRate(struct HdmiFrl *frl)
{
    union HdmiScdcsConfig1 cfg = {0};
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)frl->priv;

    if (cntlr == NULL) {
        HDF_LOGE("frl set train rate fail");
        return;
    }

    if (frl->info.mode == HDMI_FRL_MODE_FRL) {
        cfg.bits.frlRate = frl->info.curFrlRate;
        cfg.bits.ffeLevels = HDMI_FRL_TXFFE_MODE_0;
    }
    HdmiFrlSetConfig1(cntlr->scdc, &(cfg.data));
}

static bool HdmiFrlCheckFrlCapability(struct HdmiFrl *frl)
{
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)frl->priv;
    uint8_t sinkScdcVerion;
    uint8_t sinkMaxFrlRate;

    sinkScdcVerion = HdmiFrlGetSinkVersion(cntlr->scdc);
    if (HdmiEdidSupportFrl(cntlr->hdmi) != true ||
        cntlr->cap.baseCap.bits.hdmi21 == 0 ||
        cntlr->cap.baseCap.bits.frl == 0 ||
        sinkScdcVerion == 0) {
        frl->info.maxFrlRate = 0;
        return HDF_ERR_NOT_SUPPORT;
    }
    sinkMaxFrlRate = HdmiEdidGetMaxFrlRate(cntlr->hdmi);
    frl->info.maxFrlRate = (sinkMaxFrlRate > cntlr->cap.maxFrlRate) ? cntlr->cap.maxFrlRate : sinkMaxFrlRate;
    return HDF_SUCCESS;
}

static bool HdmiFrlCheckFrlStrategy(enum HdmiFrlStrategyMode strategy, uint32_t pixelClock,
    uint32_t tmdsClock, enum HdmiVideoFormatType formatType)
{
    bool support = true;

    switch (strategy) {
        case HDMI_FRL_STRATEGY_MODE_1:
            if (pixelClock <= HDMI_HDMI20_MAX_TMDS_RATE && tmdsClock <= HDMI_HDMI20_MAX_TMDS_RATE) {
                support = false;
            }
            break;
        case HDMI_FRL_STRATEGY_MODE_2:
            if (tmdsClock <= HDMI_HDMI14_MAX_TMDS_RATE) {
                support = false;
            }
            break;
        case HDMI_FRL_STRATEGY_MODE_3:
            if (formatType == HDMI_VIDEO_FORMA_INTERLACE) {
                support = false;
            }
            break;
        default:
            HDF_LOGE("strategy %d is not support", strategy);
            break;
    }
    return support;
}

static void HdmiFrlFillRateInfo(uint32_t *frlBitRate, uint32_t *frlLaneNum, uint32_t frlRate)
{
    *frlLaneNum = HDMI_FRL_4_LANES;
    if (frlRate <= HDMI_FRL_WORK_MODE_3L6G) {
        *frlLaneNum = HDMI_FRL_3_LANES;
    }

    switch (frlRate) {
        case HDMI_FRL_WORK_MODE_3L3G:
            *frlBitRate = HDMI_FRL_BIT_RATE_3;
            break;
        case HDMI_FRL_WORK_MODE_3L6G:
        case HDMI_FRL_WORK_MODE_4L6G:
            *frlBitRate = HDMI_FRL_BIT_RATE_6;
            break;
        case HDMI_FRL_WORK_MODE_4L8G:
            *frlBitRate = HDMI_FRL_BIT_RATE_8;
            break;
        case HDMI_FRL_WORK_MODE_4L10G:
            *frlBitRate = HDMI_FRL_BIT_RATE_10;
            break;
        case HDMI_FRL_WORK_MODE_4L12G:
            *frlBitRate = HDMI_FRL_BIT_RATE_12;
            break;
        default:
            break;
    }
}

static bool HdnmiFrlCheckFrlMode(struct HdmiFrl *frl)
{
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)frl->priv;
    struct HdmiVideoDefInfo *videoInfo = NULL;
    struct HdmiDfmParam param = {0};
    bool enable3d;
    uint32_t i;

    enable3d = (cntlr->attr.videoAttr._3dStruct >= HDMI_VS_VIDEO_3D_BUTT) ? false : true;
    videoInfo = HdmiCommonGetVideoDefInfo(cntlr->attr.videoAttr.timing, cntlr->attr.videoAttr.aspect, enable3d);
    if (videoInfo == NULL) {
        HDF_LOGD("have no this video param, use tmds to transmit.");
        return false;
    }
    HDF_LOGD("use frl to transmit, vic is %d.", videoInfo->vic);

    frl->info.tmdsClock = cntlr->attr.videoAttr.tmdsClock;
    if (HdmiFrlCheckFrlStrategy(frl->info.strategy, cntlr->attr.videoAttr.pixelClock,
        frl->info.tmdsClock, videoInfo->formatType) == false) {
        return false;
    }

    if (frl->info.maxFrlRate > HDMI_FRL_WORK_MODE_4L12G) {
        HDF_LOGD("max frl rate is wrong(%u)", frl->info.maxFrlRate);
        frl->info.maxFrlRate = HDMI_FRL_WORK_MODE_4L12G;
        return true;
    }

    /* Check whether a particular video format can be transmitted in a particular FRL configuration. */
    HdmiDfmFillParam(&param, videoInfo, &(cntlr->attr.audioAttr), cntlr->attr.commAttr.colorSpace,
        cntlr->attr.commAttr.deepColor);
    for (i = frl->info.maxFrlRate; i > 0; i--) {
        HdmiFrlFillRateInfo(&(param.bitRate), &(param.laneNum), i);
        if (HdmiDfmFormatSupport(&param) == true) {
            frl->info.minFrlRate = i;
            HDF_LOGD("min_rate: %d", i);
        } else {
            break;
        }
    }
    if (i == frl->info.maxFrlRate) {
        return false;
    }
    return true;
}

static bool HdmiFrlIsCtsMode(struct HdmiFrl *frl)
{
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)frl->priv;
    union HdmiScdcsTestConfig1 cfg = {0};

    cfg.data = HdmiFrlGetTestConfig1(cntlr->scdc);
    if ((cfg.bits.frlMax > 0 && cfg.bits.dscFrlMax == 0) ||
        (cfg.bits.frlMax == 0 && cfg.bits.dscFrlMax > 0)) {
        return true;
    }
    return false;
}

static int32_t HdmiFrlGetCurRate(struct HdmiFrl *frl)
{
    uint8_t curFrlrate = frl->info.curFrlRate;

    if (frl->info.ctsMode == true) {
        frl->info.preFrlRate = curFrlrate;
        curFrlrate = frl->info.maxFrlRate;
    } else {
        if (frl->info.preFrlRate != 0) {
            curFrlrate = frl->info.preFrlRate;
            frl->info.preFrlRate = 0;
        } else {
            if (frl->info.rateSelect == HDMI_FRL_RATE_BIG) {
                curFrlrate--;
            } else {
                curFrlrate++;
            }
        }
    }

    if ((curFrlrate > frl->info.maxFrlRate) ||
        (curFrlrate < frl->info.minFrlRate)) {
        return HDF_FAILURE;
    }
    frl->info.curFrlRate = curFrlrate;
    return HDF_SUCCESS;
}

static void HdmiFrlAudioNctsSet(struct HdmiFrl *frl)
{
    struct HdmiFrlAudioNctsConfig cfg = {0};
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)frl->priv;

    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->audioNctsSet == NULL) {
        HDF_LOGD("not support set frl audio ncts.");
        return;
    }

    cfg.mode = frl->info.mode;
    cfg.sampleRate = cntlr->attr.audioAttr.sampleRate;
    if (cfg.mode == HDMI_FRL_MODE_FRL) {
        cfg.frlRate = frl->info.curFrlRate;
    } else {
        cfg.pixelClk = cntlr->attr.videoAttr.pixelClock;
        cfg.n = HdmiGetN(cfg.sampleRate, cfg.pixelClk);
        cfg.cts = HdmiGetCts(cfg.sampleRate, cfg.pixelClk);
    }
    HdmiCntlrLock(cntlr);
    cntlr->ops->audioNctsSet(cntlr, &cfg);
    HdmiCntlrUnlock(cntlr);
}

static void HdmiFrlTxffeSet(struct HdmiFrl *frl)
{
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)frl->priv;
    struct HdmiPhyCfg cfg = {0};

    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->phyOutputSet == NULL) {
        HDF_LOGD("not support phy output set.");
        return;
    }

    cfg.modeCfg = HDMI_PHY_MODE_TXFFE;
    cfg.rate = (enum HdmiFrlWorkMode)frl->info.curFrlRate;
    HdmiCntlrLock(cntlr);
    cntlr->ops->phyOutputSet(cntlr, &cfg);
    HdmiCntlrUnlock(cntlr);
}

static void HdmiFrlphyConfigSet(struct HdmiFrl *frl)
{
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)frl->priv;
    struct HdmiPhyCfg cfg = {0};

    if (cntlr == NULL || cntlr->ops == NULL) {
        return;
    }

    cfg.tmdsClk = cntlr->attr.videoAttr.tmdsClock;
    cfg.pixelClk = cntlr->attr.videoAttr.pixelClock;
    cfg.deepColor = cntlr->attr.videoAttr.deepColor;
    cfg.colorSpace = cntlr->attr.commAttr.colorSpace;
    if (frl->info.mode == HDMI_FRL_MODE_FRL) {
        cfg.modeCfg = HDMI_PHY_MODE_FRL;
        cfg.rate = (enum HdmiFrlWorkMode)frl->info.curFrlRate;
    }

    if (cntlr->ops->phyOutputEnable != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->phyOutputEnable(cntlr, false);
        HdmiCntlrUnlock(cntlr);
    }
    if (cntlr->ops->phyOutputSet != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->phyOutputSet(cntlr, &cfg);
        HdmiCntlrUnlock(cntlr);
    }
    if (cntlr->ops->phyOutputEnable != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->phyOutputEnable(cntlr, true);
        HdmiCntlrUnlock(cntlr);
    }
}

static void HdmiFrlStateMachineChangeState(struct HdmiFrl *frl, enum HdmiFrlTrainStep state)
{
    /* change state machine's state */
    frl->info.machineInfo.trainingState = state;
    /*  restart timer */
    frl->info.machineInfo.startTime = OsalGetSysTimeMs();
}

static void HdmiFrlTrainingReadyCheck(struct HdmiFrl *frl)
{
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)frl->priv;

    if (HdmiFrlGetFltReady(cntlr->scdc) == true) {
        HDF_LOGD("step, ready chack pass, start training.");
        HdmiFrlphyConfigSet(frl);
        HdmiFrlSetTrainRate(frl);
        HdmiFrlStateMachineChangeState(frl, HDMI_FRL_TRAIN_STEP_TRAIN_START);
    }
}

static void HdmiFrlConfigAndStartTraining(struct HdmiFrl *frl)
{
    struct HdmiFrlTrainConfig cfg = {0};
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)frl->priv;
    union HdmiScdcsTestConfig1 testCfg = {0};

    if (cntlr == NULL || cntlr->ops == NULL) {
        return;
    }

    cfg.frlRate = frl->info.curFrlRate;
    cfg.txffe = HDMI_FRL_TXFFE_MODE_0;
    testCfg.data = HdmiFrlGetTestConfig1(cntlr->scdc);
    if (testCfg.bits.fltNoTimeout > 0) {
        cfg.frlNoTimeout = true;
    }
    cfg.trainTimeout = frl->info.machineInfo.trainTimeout;

    if (cntlr->ops->frlTrainingConfigSet != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->frlTrainingConfigSet(cntlr, &cfg);
        HdmiCntlrUnlock(cntlr);
    }
    if (cntlr->ops->frlEnable != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->frlEnable(cntlr, false);
        HdmiCntlrUnlock(cntlr);
    }
    if (cntlr->ops->frlTrainingStart != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->frlTrainingStart(cntlr);
        HdmiCntlrUnlock(cntlr);
    }
    HdmiFrlStateMachineChangeState(frl, HDMI_FRL_TRAIN_STEP_RESULT_CHECK);
    return;
}

static void HdmiFrlTrainingResultCheck(struct HdmiFrl *frl)
{
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)frl->priv;
    struct HdmiFrlTrainRslt rslt = {0};

    if (cntlr != NULL && cntlr->ops != NULL && cntlr->ops->frlGetTriningRslt != NULL) {
        cntlr->ops->frlGetTriningRslt(cntlr, &rslt);
    }

    if (rslt.status == HDMI_FRL_TRAIN_STATUS_SUCC) {
        frl->info.trainingFailCnt = 0;
        HdmiFrlStateMachineChangeState(frl, HDMI_FRL_TRAIN_STEP_RESULT_HANDLE);
    } else if (rslt.failReason == HDMI_FRL_TRAIN_FAIL_FFE_CHANGE) {
        frl->info.trainingFailCnt = 0;
        HdmiFrlTxffeSet(frl);
        HdmiFrlStateMachineChangeState(frl, HDMI_FRL_TRAIN_STEP_TRAIN_START);
    } else if (rslt.failReason == HDMI_FRL_TRAIN_FAIL_RATE_CHANGE) {
        frl->info.trainingFailCnt = 0;
        HdmiFrlStateMachineChangeState(frl, HDMI_FRL_TRAIN_STEP_RATE_CHANGE);
    } else {
        (frl->info.trainingFailCnt)++;
    }
}

static void HdmiFrlTrainingException(struct HdmiFrl *frl)
{
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)frl->priv;
    struct HdmiPhyCfg cfg = {0};

    if (cntlr == NULL || cntlr->ops == NULL) {
        return;
    }

    if (cntlr->ops->phyOutputEnable != NULL) {
        cntlr->ops->phyOutputEnable(cntlr, false);
    }

    HdmiFrlStateMachineChangeState(frl, HDMI_FRL_TRAIN_STEP_TRAIN_STOP);
    frl->info.start = false;
    /* TMDS config. */
    cfg.pixelClk = cntlr->attr.videoAttr.pixelClock;
    cfg.tmdsClk = cntlr->attr.videoAttr.tmdsClock;
    cfg.deepColor = cntlr->attr.videoAttr.deepColor;
    cfg.modeCfg = HDMI_PHY_MODE_TMDS;
    cfg.colorSpace = cntlr->attr.commAttr.colorSpace;
    HDF_LOGD("FRL change to TMDS.");
    if (cfg.tmdsClk > HDMI_HDMI20_MAX_TMDS_RATE) {
        HDF_LOGE("TMDS clk > 600M. can't work TMDS mode.");
        return;
    }
    frl->info.mode = HDMI_FRL_MODE_TMDS;
    HdmiFrlSetTrainRate(frl);
    HdmiFrlAudioNctsSet(frl);

    if (cntlr->ops->phyOutputSet != NULL) {
        cntlr->ops->phyOutputSet(cntlr, &cfg);
    }
    if (cntlr->ops->phyOutputEnable != NULL) {
        cntlr->ops->phyOutputEnable(cntlr, true);
    }
}

static void HdmiFrlTrainingRateChange(struct HdmiFrl *frl)
{
    frl->info.ctsMode = HdmiFrlIsCtsMode(frl);
    if (HdmiFrlGetCurRate(frl) != HDF_SUCCESS) {
        HDF_LOGE("FRL rate %u get failed!\n", frl->info.curFrlRate);
        HdmiFrlTrainingException(frl);
        return;
    }
    /* FRL rate change */
    HdmiFrlphyConfigSet(frl);
    HdmiFrlSetTrainRate(frl);
    HdmiFrlStateMachineChangeState(frl, HDMI_FRL_TRAIN_STEP_TRAIN_START);
}

static void HdmiFrlTrainingPassedCheck(struct HdmiFrl *frl)
{
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)frl->priv;

    if (cntlr == NULL || cntlr->ops == NULL) {
        return;
    }

    if (frl->info.start == false ||
        HdmiFrlGetFrlStart(cntlr->scdc) == false) {
        return;
    }

    HDF_LOGD("FRL training passed.");
    frl->info.work = true;
    /* n/cts config */
    HdmiFrlAudioNctsSet(frl);
    OsalMSleep(1);
    if (cntlr->ops->phyOutputEnable != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->phyOutputEnable(cntlr, frl->info.work);
        HdmiCntlrUnlock(cntlr);
    }
    OsalMSleep(1);
    HdmiFrlSetFrlStart(cntlr->scdc, true);
    HdmiFrlStateMachineChangeState(frl, HDMI_FRL_TRAIN_STEP_RETRAIN_CHECK);
}

static void HdmiFrlRetrainingCheck(struct HdmiFrl *frl)
{
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)frl->priv;

    if (cntlr == NULL || cntlr->ops == NULL) {
        return;
    }
    if (HdmiFrlGetFltUpdate(cntlr->scdc) == false) {
        return;
    }

    HDF_LOGD("FRL retraining flt_update get OK");
    HdmiFrlSetFltUpdate(cntlr->scdc, true);
    frl->info.work = false;
    if (cntlr->ops->phyOutputEnable != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->phyOutputEnable(cntlr, frl->info.work);
        HdmiCntlrUnlock(cntlr);
    }
    HdmiFrlStateMachineChangeState(frl, HDMI_FRL_TRAIN_STEP_TRAIN_START);
}

static void HdmiFrlTrainingStop(struct HdmiFrl *frl)
{
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)frl->priv;

    if (cntlr == NULL || cntlr->ops == NULL) {
        return;
    }

    frl->info.work = false;
    if (cntlr->ops->phyOutputEnable != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->phyOutputEnable(cntlr, frl->info.work);
        HdmiCntlrUnlock(cntlr);
    }
    HdmiFrlStateMachineChangeState(frl, HDMI_FRL_TRAIN_STEP_BUTT);
}

void HdmiFrlTrainingStateMachineHandle(struct HdmiFrl *frl)
{
    if (frl == NULL) {
        HDF_LOGE("FRL trining, not support FRL.");
        return;
    }

    switch (frl->info.machineInfo.trainingState) {
        case HDMI_FRL_TRAIN_STEP_READR_CHECK:
            HdmiFrlTrainingReadyCheck(frl);
            break;
        case HDMI_FRL_TRAIN_STEP_TRAIN_START:
            HdmiFrlConfigAndStartTraining(frl);
            break;
        case HDMI_FRL_TRAIN_STEP_RESULT_CHECK:
            HdmiFrlTrainingResultCheck(frl);
            break;
        case HDMI_FRL_TRAIN_STEP_RATE_CHANGE:
            HdmiFrlTrainingRateChange(frl);
            break;
        case HDMI_FRL_TRAIN_STEP_RESULT_HANDLE:
            HdmiFrlTrainingPassedCheck(frl);
            break;
        case HDMI_FRL_TRAIN_STEP_RETRAIN_CHECK:
            HdmiFrlRetrainingCheck(frl);
            break;
        case HDMI_FRL_TRAIN_STEP_TRAIN_STOP:
            HdmiFrlTrainingStop(frl);
            break;
        default:
            break;
    }
}

static void HdmiFrlTrainingReadyCheckTimeout(struct HdmiFrl *frl)
{
    uint64_t curTime = OsalGetSysTimeMs();
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)frl->priv;

    if ((curTime - frl->info.machineInfo.startTime) > frl->info.machineInfo.waitReadyTime) {
        if (HdmiFrlGetFltReady(cntlr->scdc) == true) {
            HDF_LOGD("step, ready chack pass, start training.");
            frl->info.machineInfo.timeoutCnt = 0;
            HdmiFrlphyConfigSet(frl);
            HdmiFrlSetTrainRate(frl);
            HdmiFrlStateMachineChangeState(frl, HDMI_FRL_TRAIN_STEP_TRAIN_START);
        } else {
            (frl->info.machineInfo.timeoutCnt)++;
            /*  */
            HdmiFrlStateMachineChangeState(frl, HDMI_FRL_TRAIN_STEP_READR_CHECK);
        }
    } else {
        /* wait for a maximum of 10 times, (wait_ready_ms * 10) ms */
        if (frl->info.machineInfo.timeoutCnt < HDMI_FRL_TRAIN_STEP_READR_CHECK_MAX_TRIES) {
            return;
        }
        HDF_LOGD("FRL training READY_CHECK exception.");
        frl->info.machineInfo.timeoutCnt = 0;
        HdmiFrlTrainingException(frl);
    }
}

static void HdmiFrlTrainingResultCheckTimeout(struct HdmiFrl *frl)
{
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)frl->priv;
    struct HdmiFrlTrainRslt rslt = {0};

    if (cntlr != NULL && cntlr->ops != NULL && cntlr->ops->frlGetTriningRslt != NULL) {
        cntlr->ops->frlGetTriningRslt(cntlr, &rslt);
    }

    if (rslt.status == HDMI_FRL_TRAIN_STATUS_SUCC) {
        frl->info.trainingFailCnt = 0;
        HdmiFrlStateMachineChangeState(frl, HDMI_FRL_TRAIN_STEP_RESULT_HANDLE);
    } else if (rslt.failReason == HDMI_FRL_TRAIN_FAIL_FFE_CHANGE) {
        frl->info.trainingFailCnt = 0;
        HdmiFrlTxffeSet(frl);
        HdmiFrlStateMachineChangeState(frl, HDMI_FRL_TRAIN_STEP_TRAIN_START);
    } else if (rslt.failReason == HDMI_FRL_TRAIN_FAIL_RATE_CHANGE) {
        frl->info.trainingFailCnt = 0;
        HdmiFrlStateMachineChangeState(frl, HDMI_FRL_TRAIN_STEP_RATE_CHANGE);
    } else {
        (frl->info.trainingFailCnt)++;
        if (frl->info.trainingFailCnt > frl->info.trainingMaxFailTimes) {
            frl->info.trainingFailCnt = 0;
            HdmiFrlTrainingException(frl);
        } else {
            /* not reach max fail times, re-training. */
            HdmiFrlStateMachineChangeState(frl, HDMI_FRL_TRAIN_STEP_TRAIN_START);
        }
    }
}

static void HdmiFrlTrainingResultHandleTimeout(struct HdmiFrl *frl)
{
    uint64_t curTime;
    bool frlStart = false;
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)frl->priv;

    if (cntlr == NULL || cntlr->ops == NULL) {
        return;
    }

    do {
        frlStart = HdmiFrlGetFrlStart(cntlr->scdc);
        if (frlStart == true && frl->info.start == true) {
            frl->info.work = true;
            /* n/cts config */
            HdmiFrlAudioNctsSet(frl);
            if (cntlr->ops->phyOutputEnable != NULL) {
                cntlr->ops->phyOutputEnable(cntlr, frl->info.work);
            }
            HdmiFrlSetFrlStart(cntlr->scdc, true);
            HdmiFrlStateMachineChangeState(frl, HDMI_FRL_TRAIN_STEP_RETRAIN_CHECK);
            break;
        } else if (frlStart == false && HdmiFrlGetFltUpdate(cntlr->scdc) == true) {
            HdmiFrlSetFltUpdate(cntlr->scdc, true);
            HdmiFrlStateMachineChangeState(frl, HDMI_FRL_TRAIN_STEP_TRAIN_START);
            break;
        }

        curTime = OsalGetSysTimeMs();
        if ((curTime - frl->info.machineInfo.startTime) > frl->info.machineInfo.waitHandleTime) {
            (frl->info.machineInfo.timeoutCnt)++;
            break;
        }
        /* check the sink's status every 2ms required by protocol */
        OsalUDelay(HDMI_FRL_CHECK_SINK_STATUS_DELAY_TIME);
    } while (true);

    if (frl->info.machineInfo.timeoutCnt >= HDMI_FRL_TRAIN_STEP_RESULT_HANDLE_MAX_TRIES) {
        HDF_LOGE("FRL training timeout.");
        frl->info.machineInfo.timeoutCnt = 0;
        HdmiFrlTrainingException(frl);
    }
}

static void HdmiFrlRetrainingCheckTimeout(struct HdmiFrl *frl)
{
    uint64_t curTime = OsalGetSysTimeMs();
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)frl->priv;

    if (cntlr == NULL || cntlr->ops == NULL) {
        return;
    }
    if ((curTime - frl->info.machineInfo.startTime) <= frl->info.machineInfo.waitRetrainTime) {
        return;
    }
    if (HdmiFrlGetFltUpdate(cntlr->scdc) != true) {
        return;
    }

    HdmiFrlSetFltUpdate(cntlr->scdc, true);
    frl->info.work = true;
    if (cntlr->ops->phyOutputEnable != NULL) {
        cntlr->ops->phyOutputEnable(cntlr, frl->info.work);
    }
    HdmiFrlStateMachineChangeState(frl, HDMI_FRL_TRAIN_STEP_TRAIN_START);
}

void HdmiFrlTrainingStateMachineTimeoutHandle(struct HdmiFrl *frl)
{
    if (frl == NULL || frl->priv == NULL) {
        HDF_LOGE("FRL trining, not support FRL.");
        return;
    }

    switch (frl->info.machineInfo.trainingState) {
        case HDMI_FRL_TRAIN_STEP_READR_CHECK:
            HdmiFrlTrainingReadyCheckTimeout(frl);
            break;
        case HDMI_FRL_TRAIN_STEP_TRAIN_START:
            HdmiFrlConfigAndStartTraining(frl);
            break;
        case HDMI_FRL_TRAIN_STEP_RESULT_CHECK:
            HdmiFrlTrainingResultCheckTimeout(frl);
            break;
        case HDMI_FRL_TRAIN_STEP_RATE_CHANGE:
            HdmiFrlTrainingRateChange(frl);
            break;
        case HDMI_FRL_TRAIN_STEP_RESULT_HANDLE:
            HdmiFrlTrainingResultHandleTimeout(frl);
            break;
        case HDMI_FRL_TRAIN_STEP_RETRAIN_CHECK:
            HdmiFrlRetrainingCheckTimeout(frl);
            break;
        case HDMI_FRL_TRAIN_STEP_TRAIN_STOP:
            HdmiFrlTrainingStop(frl);
            break;
        default:
            break;
    }
}

void HdmiFrlEnable(struct HdmiFrl *frl, bool enable)
{
    if (frl == NULL || frl->priv == NULL) {
        return;
    }

    frl->info.start = enable;
    frl->info.machineInfo.start = enable;
    frl->info.machineInfo.machineState = (enable == true) ? HDMI_FRL_STATE_MACHINE_START : HDMI_FRL_STATE_MACHINE_STOP;
    frl->info.ctsMode = HdmiFrlIsCtsMode(frl);
    if (frl->info.ctsMode == true) {
        HdmiFrlGetCurRate(frl);
        HDF_LOGD("frl cur rate %d", frl->info.curFrlRate);
    }
    HdmiFrlStateMachineChangeState(frl, HDMI_FRL_TRAIN_STEP_READR_CHECK);
}

int32_t HdmiFrlModeSelect(struct HdmiFrl *frl)
{
    int32_t ret;
    struct HdmiCntlr *cntlr = NULL;
    struct HdmiVideoAttr *videoAttr = NULL;

    if (frl == NULL || frl->priv == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    ret = HdmiFrlCheckFrlCapability(frl);
    if (ret == HDF_FAILURE) {
        return ret;
    } else if (ret != HDF_SUCCESS) {
        HDF_LOGD("frl check capability fail, change to TMDS.");
        frl->info.mode = HDMI_FRL_MODE_TMDS;
    } else {
        if (HdnmiFrlCheckFrlMode(frl) == true) {
            HDF_LOGD("frl check frl mode succ.");
            frl->info.mode = HDMI_FRL_MODE_FRL;
        } else {
            HDF_LOGD("frl check frl mode fail, change to TMDS.");
            frl->info.mode = HDMI_FRL_MODE_TMDS;
        }
    }

    cntlr = (struct HdmiCntlr *)frl->priv;
    videoAttr = &(cntlr->attr.videoAttr);
    if (frl->info.mode == HDMI_FRL_MODE_TMDS) {
        if (videoAttr->tmdsClock > HDMI_HDMI20_MAX_TMDS_RATE) {
            HDF_LOGE("FRL mode select, tmds Clock exceed max.");
            return HDF_FAILURE;
        }
        if (HdmiEdidScdcSupport(cntlr->hdmi) == true) {
            HdmiFrlSetTrainRate(frl);
        }
        return HDF_SUCCESS;
    }

    /* FRL mode. */
    frl->info.curFrlRate = (frl->info.rateSelect == HDMI_FRL_RATE_BIG) ?
        frl->info.maxFrlRate : frl->info.minFrlRate;
    cntlr->tmdsMode = HDMI_TMDS_MODE_HDMI_2_0;
    if (cntlr->ops != NULL && cntlr->ops->tmdsModeSet != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->tmdsModeSet(cntlr, cntlr->tmdsMode);
        HdmiCntlrUnlock(cntlr);
    }
    frl->info.ctsMode = false;
    return HDF_SUCCESS;
}

bool HdmiFrlModeChanged(struct HdmiFrl *frl)
{
    if (frl == NULL) {
        return false;
    }

    if (frl->info.perMode != frl->info.mode) {
        frl->info.perMode = frl->info.mode;
        return true;
    }
    return false;
}

bool HdmiFrlSupport(struct HdmiFrl *frl)
{
    if (frl == NULL) {
        return false;
    }

    if (frl->info.mode == HDMI_FRL_MODE_FRL) {
        return true;
    }
    return false;
}
