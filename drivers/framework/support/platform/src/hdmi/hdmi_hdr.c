/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_log.h"
#include "hdmi_core.h"
#include "securec.h"

#define HDF_LOG_TAG hdmi_hdr_c

static bool HdmiHdrAttrChange(struct HdmiHdrAttr *curAttr, struct HdmiHdrAttr *oldAttr)
{
    if (memcmp(curAttr, oldAttr, sizeof(struct HdmiHdrAttr)) == 0) {
        return false;
    }
    return true;
}

static void HdmiHdrColorimetryUpdate(struct HdmiVideoAttr *videoAttr, enum HdmiHdrColormetry colorimetry)
{
    if (colorimetry >= HDMI_HDR_COLORIMETRY_EXTENDED) {
        videoAttr->colorimetry = HDMI_COLORIMETRY_EXTENDED;
        videoAttr->extColorimetry = (enum HdmiExtendedColorimetry)(colorimetry -
            HDMI_HDR_EXTENDED_COLORIMETRY_XV_YCC_601);
    } else {
        videoAttr->colorimetry = (enum HdmiColorimetry)colorimetry;
        videoAttr->extColorimetry = HDMI_EXTENDED_COLORIMETRY_XV_YCC_601;
    }
}

static void HdmiHdrZeroDrmInfoFrameTimerSet(struct HdmiCntlr *cntlr, bool start)
{
    struct HdmiHdrInfo *hdrInfo = &(cntlr->hdr->info);
    struct HdmiHdrAttr *hdrAttr = &(cntlr->attr.hdrAttr);

    hdrInfo->zeroDrmIfTimer.start = false;
    hdrInfo->zeroDrmIfTimer.timerType = HDMI_HDR_TIMER_ZERO_DRMIF;
    hdrInfo->zeroDrmIfTimer.time = HDMI_ZERO_DRMIF_SEND_TIME;
    HdmiCntlrHdrTimerSet(cntlr, &(hdrInfo->zeroDrmIfTimer));
    if (start == false) {
        return;
    }

    hdrAttr->eotfType = HDMI_EOTF_SDR_LUMIN;
    hdrAttr->metadataType = HDMI_DRM_STATIC_METADATA_TYPE_1;
    if (memset_s(&(hdrAttr->descriptor), sizeof(hdrAttr->descriptor), 0, sizeof(hdrAttr->descriptor)) != EOK) {
        HDF_LOGE("memset_s fail");
        return;
    }
    (void)HdmiDrmInfoFrameSend(&(cntlr->infoFrame), true);

    hdrInfo->zeroDrmIfTimer.start = true;
    HdmiCntlrHdrTimerSet(cntlr, &(hdrInfo->zeroDrmIfTimer));
}

static void HdmiHdrModeChangeTimerSet(struct HdmiCntlr *cntlr, bool start)
{
    struct HdmiHdrInfo *hdrInfo = &(cntlr->hdr->info);

    hdrInfo->stateChangeTimer.start = false;
    hdrInfo->stateChangeTimer.timerType = HDMI_HDR_TIMER_SDR_TO_HDR10;
    hdrInfo->stateChangeTimer.time = HDMI_HDR_STATE_CHANGE_TIME;
    HdmiCntlrHdrTimerSet(cntlr, &(hdrInfo->stateChangeTimer));
    if (start == false) {
        return;
    }

    hdrInfo->stateChangeTimer.start = true;
    HdmiCntlrHdrTimerSet(cntlr, &(hdrInfo->stateChangeTimer));
}

static int32_t HdmiDisableHdr(struct HdmiCntlr *cntlr, struct HdmiHdrAttr *curAttr)
{
    int32_t ret = HDF_SUCCESS;
    struct HdmiHdrAttr *oldAttr = &(cntlr->attr.hdrAttr);
    struct HdmiVideoAttr *videoAttr = &(cntlr->attr.videoAttr);
    struct HdmiCommonAttr *commAttr = &(cntlr->attr.commAttr);
    struct HdmiHdrInfo *hdrInfo = &(cntlr->hdr->info);

    switch (oldAttr->mode) {
        case HDMI_HDR_MODE_DISABLE:
            break;
        case HDMI_HDR_MODE_CEA_861_3:
            *oldAttr = *curAttr;
            if (hdrInfo->state != HDMI_HDR_STATE_NONE) {
                /* to solve the problem of some TV can not switch smoothly from HDR10 to non-HDR10. */
                if (hdrInfo->state == HDMI_HDR_STATE_OE) {
                    HdmiCntlrAudioMuteEnable(cntlr, false);
                    HdmiCntlrBlackDataSet(cntlr, true);
                }
                HdmiCntlrAvmuteSet(cntlr, true);
                if (hdrInfo->state == HDMI_HDR_STATE_OE) {
                    HdmiCntlrPhyOutputEnablet(cntlr, false);
                }
                HdmiHdrModeChangeTimerSet(cntlr,  true);
                HdmiCntlrHdrTimerSet(cntlr, &(hdrInfo->stateChangeTimer));
                (void)HdmiDrmInfoFrameSend(&(cntlr->infoFrame), false);
            } else {
                /* start timer, send zero DRMIF and stop after 2 seconds. */
                HdmiHdrZeroDrmInfoFrameTimerSet(cntlr,  true);
            }
            break;
        case HDMI_HDR_MODE_CEA_861_3_AUTHEN:
        case HDMI_HDR_MODE_DOLBY_NORMAL:
        case HDMI_HDR_MODE_DOLBY_TUNNELING:
            if (oldAttr->mode != HDMI_HDR_MODE_DOLBY_NORMAL) {
                commAttr->colorSpace = HDMI_COLOR_SPACE_YCBCR444;
            }
            videoAttr->quantization = HDMI_QUANTIZATION_RANGE_DEFAULT;
            videoAttr->yccQuantization = HDMI_YCC_QUANTIZATION_RANGE_LIMITED;
            break;
        default:
            ret = HDF_FAILURE;
            break;
    }
    return ret;
}

static void HdmiChangeToHdr(struct HdmiCntlr *cntlr, struct HdmiCommonAttr *commAttr)
{
    struct HdmiHdrAttr *hdrAttr = &(cntlr->attr.hdrAttr);
    struct HdmiHdrInfo *hdrInfo = &(cntlr->hdr->info);

    HdmiHdrZeroDrmInfoFrameTimerSet(cntlr, false);
    if (hdrAttr->userMode != HDMI_HDR_USERMODE_HDR10) {
        HDF_LOGE("usermode is not HDR10/HLG.");
        return;
    }
    commAttr->drm = true;

    if (hdrInfo->state != HDMI_HDR_STATE_NONE) {
        /* to solve the problem of some TV can not switch smoothly from non-HDR10 to HDR10. */
        if (hdrInfo->state == HDMI_HDR_STATE_OE) {
            HdmiCntlrAudioMuteEnable(cntlr, false);
            HdmiCntlrBlackDataSet(cntlr, true);
        }
        HdmiCntlrAvmuteSet(cntlr, true);
        if (hdrInfo->state == HDMI_HDR_STATE_OE) {
            HdmiCntlrPhyOutputEnablet(cntlr, false);
        }
        hdrInfo->stateChangeTimer.start = true;
        hdrInfo->stateChangeTimer.timerType = HDMI_HDR_TIMER_SDR_TO_HDR10;
        hdrInfo->stateChangeTimer.time = HDMI_HDR_STATE_CHANGE_TIME;
        HdmiCntlrHdrTimerSet(cntlr, &(hdrInfo->stateChangeTimer));
    }
}

static int32_t HdmiHdrModeSelect(struct HdmiCntlr *cntlr)
{
    struct HdmiHdrAttr *hdrAttr = &(cntlr->attr.hdrAttr);
    struct HdmiCommonAttr *commAttr = &(cntlr->attr.commAttr);

    switch (hdrAttr->mode) {
        case HDMI_HDR_MODE_CEA_861_3:
            HdmiChangeToHdr(cntlr, commAttr);
            commAttr->vsifDolby = false;
            break;
        case HDMI_HDR_MODE_CEA_861_3_AUTHEN:
            /* HDR10 authen mode, HDMI must bypass. */
            if (hdrAttr->userMode != HDMI_HDR_USERMODE_DOLBY) {
                HDF_LOGE("user mode is not dolby");
                return HDF_FAILURE;
            }
            commAttr->drm = true;
            commAttr->vsifDolby = false;
            break;
        case HDMI_HDR_MODE_DOLBY_NORMAL:
        case HDMI_HDR_MODE_DOLBY_TUNNELING:
            if (hdrAttr->userMode != HDMI_HDR_USERMODE_DOLBY) {
                HDF_LOGE("user mode is not dolby");
                return HDF_FAILURE;
            }
            commAttr->drm = false;
            commAttr->vsifDolby = true;
            break;
        default:
            return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t HdmiHdrAttrHandle(struct HdmiHdr *hdr, struct HdmiHdrAttr *curAttr)
{
    struct HdmiHdrAttr *oldAttr = NULL;
    struct HdmiCommonAttr *commAttr = NULL;
    struct HdmiCntlr *cntlr = NULL;
    int32_t ret = HDF_FAILURE;

    if (hdr == NULL || hdr->priv == NULL) {
        HDF_LOGE("hdr attr handle, input param invalid.");
        return HDF_ERR_INVALID_PARAM;
    }

    cntlr = (struct HdmiCntlr *)hdr->priv;
    oldAttr = &(cntlr->attr.hdrAttr);
    commAttr = &(cntlr->attr.commAttr);
    if (HdmiHdrAttrChange(curAttr, oldAttr) == false) {
        HDF_LOGD("hdr attr not change");
        return HDF_SUCCESS;
    }
    if (oldAttr->colorimetry != curAttr->colorimetry) {
        HdmiHdrColorimetryUpdate(&(cntlr->attr.videoAttr), curAttr->colorimetry);
    }

    /* change to SDR */
    if (curAttr->mode == HDMI_HDR_MODE_DISABLE) {
        if (HdmiDisableHdr(cntlr, curAttr) == HDF_SUCCESS) {
            commAttr->vsifDolby = false;
            cntlr->attr.hdrAttr = *curAttr;
            (void)HdmiAviInfoFrameSend(&(cntlr->infoFrame), (commAttr->enableHdmi && commAttr->avi));
            (void)HdmiVsInfoFrameSend(&(cntlr->infoFrame), commAttr->enableHdmi, commAttr->vsifDolby);
            HdmiCntlrVideoPathSet(cntlr, &(cntlr->attr.videoAttr));
            return ret;
        } else {
            return HDF_FAILURE;
        }
    }

    /* change to HDR */
    cntlr->attr.hdrAttr = *curAttr;
    ret = HdmiHdrModeSelect(cntlr);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    (void)HdmiDrmInfoFrameSend(&(cntlr->infoFrame), (commAttr->enableHdmi && commAttr->drm));
    (void)HdmiAviInfoFrameSend(&(cntlr->infoFrame), (commAttr->enableHdmi && commAttr->avi));
    (void)HdmiVsInfoFrameSend(&(cntlr->infoFrame), commAttr->enableHdmi, commAttr->vsifDolby);
    HdmiCntlrVideoPathSet(cntlr, &(cntlr->attr.videoAttr));
    return ret;
}

int32_t HdmiHdrDrmInfoFrameStop(struct HdmiHdr *hdr)
{
    struct HdmiCntlr *cntlr = NULL;

    HDF_LOGD("zero DRM infoFrame send timeout.");
    if (hdr == NULL || hdr->priv == NULL) {
        HDF_LOGE("hdr stop drm, input param invalid.");
        return HDF_ERR_INVALID_PARAM;
    }
    cntlr = (struct HdmiCntlr *)hdr->priv;

    if (cntlr->attr.hdrAttr.mode != HDMI_HDR_MODE_CEA_861_3) {
        return HdmiDrmInfoFrameSend(&(cntlr->infoFrame), false);
    }
    return HDF_SUCCESS;
}

int32_t HdmiHdrModeChangeTimeout(struct HdmiHdr *hdr)
{
    struct HdmiCntlr *cntlr = NULL;

    HDF_LOGD("hdr mode change timeout.");
    if (hdr == NULL || hdr->priv == NULL) {
        HDF_LOGE("hdr mode change timeout, input param invalid.");
        return HDF_ERR_INVALID_PARAM;
    }
    cntlr = (struct HdmiCntlr *)hdr->priv;

    if (hdr->info.state == HDMI_HDR_STATE_OE) {
        HdmiCntlrPhyOutputEnablet(cntlr, true);
    }
    HdmiCntlrAvmuteSet(cntlr, false);
    if (hdr->info.state == HDMI_HDR_STATE_OE) {
        HdmiCntlrBlackDataSet(cntlr, false);
        HdmiCntlrAudioMuteEnable(cntlr, true);
    }
    return HDF_SUCCESS;
}
