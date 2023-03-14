/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdmi_core.h"
#include "device_resource_if.h"
#include "hdf_log.h"
#include "hdmi_dispatch.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"

#define HDF_LOG_TAG hdmi_core_c

int32_t HdmiCntlrAllocDev(struct HdmiCntlr *cntlr)
{
    struct HdmiDevice *dev = (struct HdmiDevice *)OsalMemCalloc(sizeof(struct HdmiDevice));

    if (dev == NULL) {
        HDF_LOGE("Hdmi alloc dev fail");
        return HDF_ERR_MALLOC_FAIL;
    }

    dev->cntlr = cntlr;
    cntlr->hdmi = dev;
    return HDF_SUCCESS;
}

void HdmiCntlrFreeDev(struct HdmiCntlr *cntlr)
{
    if (cntlr == NULL || cntlr->hdmi == NULL) {
        return;
    }

    OsalMemFree(cntlr->hdmi);
    cntlr->hdmi = NULL;
}

static void HdmiCecInit(struct HdmiCntlr *cntlr)
{
    if (cntlr == NULL) {
        return;
    }
    if (cntlr->cap.baseCap.bits.cec == 0) {
        HDF_LOGD("not support cec");
        return;
    }

    if (cntlr->cec == NULL) {
        cntlr->cec = (struct HdmiCec *)OsalMemCalloc(sizeof(struct HdmiCec));
        if (cntlr->cec == NULL) {
            HDF_LOGE("cec malloc fail");
            return;
        }
    }
    HDF_LOGE("HdmiCecInit, success.");
    cntlr->cec->priv = (void *)cntlr;
}

static void HdmiCecDeinit(struct HdmiCntlr *cntlr)
{
    if (cntlr == NULL) {
        return;
    }
    if (cntlr->cec != NULL) {
        OsalMemFree(cntlr->cec);
        cntlr->cec = NULL;
    }
}

static void HdmiDdcInit(struct HdmiCntlr *cntlr)
{
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("ddc init: param is invalid");
        return;
    }
    cntlr->ddc.priv = (void *)cntlr;
    if (cntlr->ddc.init == true) {
        return;
    }
    ret = OsalMutexInit(&(cntlr->ddc.ddcMutex));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiDdcInit: mutex init fail!");
        return;
    }
    HDF_LOGE("HdmiDdcInit, success.");
    cntlr->ddc.init = true;
}

static void HdmiDdcDeinit(struct HdmiCntlr *cntlr)
{
    if (cntlr == NULL) {
        HDF_LOGE("ddc init: param is invalid");
        return;
    }

    (void)OsalMutexDestroy(&(cntlr->ddc.ddcMutex));
    if (memset_s(&(cntlr->ddc), sizeof(struct HdmiDdc), 0, sizeof(struct HdmiDdc)) != EOK) {
        HDF_LOGE("deinit ddc, memset_s fail.");
    }
    cntlr->ddc.priv = NULL;
    cntlr->ddc.init = false;
}

static void HdmiFrlInit(struct HdmiCntlr *cntlr)
{
    if (cntlr == NULL) {
        return;
    }
    if (cntlr->cap.baseCap.bits.frl == 0) {
        HDF_LOGD("not support frl");
        return;
    }

    if (cntlr->frl == NULL) {
        cntlr->frl = (struct HdmiFrl *)OsalMemCalloc(sizeof(struct HdmiFrl));
        if (cntlr->frl == NULL) {
            HDF_LOGE("frl malloc fail");
            return;
        }
    }
    HDF_LOGE("HdmiFrlInit, success.");
    cntlr->frl->priv = (void *)cntlr;
}

static void HdmiFrlDeinit(struct HdmiCntlr *cntlr)
{
    if (cntlr == NULL) {
        return;
    }
    if (cntlr->frl != NULL) {
        OsalMemFree(cntlr->frl);
        cntlr->frl = NULL;
    }
}

static void HdmiHdcpInit(struct HdmiCntlr *cntlr)
{
    int32_t ret;

    if (cntlr == NULL) {
        return;
    }
    if (cntlr->cap.baseCap.bits.hdcp == 0) {
        HDF_LOGD("not support hdcp");
        return;
    }

    if (cntlr->hdcp == NULL) {
        cntlr->hdcp = (struct HdmiHdcp *)OsalMemCalloc(sizeof(struct HdmiHdcp));
        if (cntlr->hdcp == NULL) {
            HDF_LOGE("hdcp malloc fail");
            return;
        }
        ret = OsalMutexInit(&(cntlr->hdcp->hdcpMutex));
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("HdmiHdcpInit: mutex init fail!");
            return;
        }
    }
    HDF_LOGE("HdmiHdcpInit, success.");
    cntlr->hdcp->priv = (void *)cntlr;
}

static void HdmiHdcpDeinit(struct HdmiCntlr *cntlr)
{
    if (cntlr == NULL) {
        return;
    }
    if (cntlr->hdcp != NULL) {
        HdmiHdcpClose(cntlr->hdcp);
        OsalMemFree(cntlr->hdcp);
        cntlr->hdcp = NULL;
    }
}

static void HdmiHdrInit(struct HdmiCntlr *cntlr)
{
    if (cntlr == NULL) {
        return;
    }
    if (cntlr->cap.baseCap.bits.hdr == 0) {
        HDF_LOGD("not support hdr");
        return;
    }

    if (cntlr->hdr == NULL) {
        cntlr->hdr = (struct HdmiHdr *)OsalMemCalloc(sizeof(struct HdmiHdr));
        if (cntlr->hdr == NULL) {
            HDF_LOGE("hdr malloc fail");
            return;
        }
    }
    HDF_LOGE("HdmiHdrInit, success.");
    cntlr->hdr->priv = (void *)cntlr;
}

static void HdmiHdrDeinit(struct HdmiCntlr *cntlr)
{
    if (cntlr == NULL) {
        return;
    }
    if (cntlr->hdr != NULL) {
        OsalMemFree(cntlr->hdr);
        cntlr->hdr = NULL;
    }
}

static void HdmiInfoFrameInit(struct HdmiCntlr *cntlr)
{
    if (cntlr == NULL) {
        return;
    }
    HDF_LOGE("HdmiInfoFrameInit, success.");
    cntlr->infoFrame.priv = (void *)cntlr;
}

static void HdmiInfoFrameDeInit(struct HdmiCntlr *cntlr)
{
    if (cntlr == NULL) {
        return;
    }

    if (memset_s(&(cntlr->infoFrame), sizeof(struct HdmiInfoFrame), 0, sizeof(struct HdmiInfoFrame)) != EOK) {
        HDF_LOGE("deinit infoFrame, memset_s fail.");
    }
    cntlr->infoFrame.priv = NULL;
}

static void HdmiScdcInit(struct HdmiCntlr *cntlr)
{
    if (cntlr == NULL) {
        return;
    }
    if (cntlr->cap.baseCap.bits.scdc == 0) {
        HDF_LOGD("not support scdc");
        return;
    }

    if (cntlr->scdc == NULL) {
        cntlr->scdc = (struct HdmiScdc *)OsalMemCalloc(sizeof(struct HdmiScdc));
        if (cntlr->scdc == NULL) {
            HDF_LOGE("scdc malloc fail");
            return;
        }
    }
    HDF_LOGE("HdmiScdcInit, success.");
    cntlr->scdc->priv = (void *)cntlr;
}

static void HdmiScdcDeinit(struct HdmiCntlr *cntlr)
{
    if (cntlr == NULL) {
        return;
    }
    if (cntlr->scdc != NULL) {
        OsalMemFree(cntlr->scdc);
        cntlr->scdc = NULL;
    }
}

static int32_t HdmiCntlrInit(struct HdmiCntlr *cntlr)
{
    int32_t ret;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->hdfDevObj == NULL) {
        HDF_LOGE("HdmiCntlrInit: no HdfDeviceObject attached!");
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = OsalMutexInit(&cntlr->mutex);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrInit: mutex init fail!");
        return ret;
    }

    cntlr->msgQueue = PlatformQueueCreate(HdmiEventMsgHandleDefault, "PlatformHdmiWorkerThread", cntlr);
    if (cntlr->msgQueue == NULL) {
        HDF_LOGE("HdmiCntlrInit: failed to create msg queue!");
        return HDF_PLT_ERR_OS_API;
    }
    ret = PlatformQueueStart(cntlr->msgQueue);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrInit: failed to start msg queue!");
        PlatformQueueDestroy(cntlr->msgQueue);
        return ret;
    }

    cntlr->service.Dispatch = HdmiIoDispatch;
    cntlr->hdfDevObj->service = &(cntlr->service);
    cntlr->device.number = (int32_t)cntlr->deviceIndex;
    cntlr->device.hdfDev = cntlr->hdfDevObj;
    HdmiInfoFrameInit(cntlr);
    HdmiDdcInit(cntlr);
    return HDF_SUCCESS;
}

static void HdmiCntlrUninit(struct HdmiCntlr *cntlr)
{
    if (cntlr != NULL) {
        HdmiInfoFrameDeInit(cntlr);
        HdmiScdcDeinit(cntlr);
        HdmiDdcDeinit(cntlr);
        HdmiCecDeinit(cntlr);
        HdmiFrlDeinit(cntlr);
        HdmiHdcpDeinit(cntlr);
        HdmiCntlrFreeDev(cntlr);
        (void)OsalMutexDestroy(&cntlr->mutex);
    }
}

int32_t HdmiCntlrAdd(struct HdmiCntlr *cntlr)
{
    int32_t ret;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = HdmiCntlrInit(cntlr);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    cntlr->device.manager = PlatformManagerGet(PLATFORM_MODULE_HDMI);
    ret = PlatformDeviceAdd(&cntlr->device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrAdd: device add fail!");
        HdmiCntlrUninit(cntlr);
        return ret;
    }
    return HDF_SUCCESS;
}

void HdmiCntlrRemove(struct HdmiCntlr *cntlr)
{
    if (cntlr != NULL) {
        HdmiCntlrUninit(cntlr);
        PlatformDeviceDel(&cntlr->device);
    }
}

static int32_t HdmiCntlrParseVideoCaps(struct HdmiCntlr *cntlr, struct DeviceResourceIface *drsOps,
    const struct DeviceResourceNode *node)
{
    int32_t ret;

    ret = drsOps->GetUint32(node, "maxTmdsClock", &(cntlr->cap.maxTmdsClock), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrParseVideoCaps: read maxTmdsClock fail!");
        return ret;
    }

    ret = drsOps->GetUint32(node, "defTmdsClock", &(cntlr->cap.defTmdsClock), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrParseVideoCaps: read defTmdsClock fail!");
        return ret;
    }

    ret = drsOps->GetUint32(node, "maxFrlRate", &(cntlr->cap.maxFrlRate), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrParseVideoCaps: read maxFrlRate fail!");
        return ret;
    }

    ret = drsOps->GetUint32(node, "videoTiming", &(cntlr->cap.videoTiming), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrParseVideoCaps: read videoTiming fail!");
        return ret;
    }

    ret = drsOps->GetUint32(node, "quantization", &(cntlr->cap.quantization), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrParseVideoCaps: read quantization fail!");
        return ret;
    }

    ret = drsOps->GetUint32(node, "colorSpace", &(cntlr->cap.colorSpace), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrParseVideoCaps: read colorSpace fail!");
        return ret;
    }

    ret = drsOps->GetUint32(node, "colorimetry", &(cntlr->cap.colorimetry), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrParseVideoCaps: read colorimetry fail!");
        return ret;
    }
    return HDF_SUCCESS;
}

static int32_t HdmiCntlrParseAudioCaps(struct HdmiCntlr *cntlr, struct DeviceResourceIface *drsOps,
    const struct DeviceResourceNode *node)
{
    int32_t ret;

    ret = drsOps->GetUint32(node, "audioIfType", &(cntlr->cap.audioIfType), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrParseAudioCaps: read audioIfType fail!");
        return ret;
    }
    HDF_LOGD("HdmiCntlrParseAudioCaps: audioIfType = %d", cntlr->cap.audioIfType);

    ret = drsOps->GetUint32(node, "audioBitDepth", &(cntlr->cap.audioBitDepth), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrParseAudioCaps: read audioBitDepth fail!");
        return ret;
    }
    HDF_LOGD("HdmiCntlrParseAudioCaps: audioBitDepth = %d", cntlr->cap.audioBitDepth);

    ret = drsOps->GetUint32(node, "audioSampleRate", &(cntlr->cap.audioSampleRate), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrParseAudioCaps: read audioSampleRate fail!");
        return ret;
    }
    HDF_LOGD("HdmiCntlrParseAudioCaps: audioSampleRate = %d", cntlr->cap.audioSampleRate);

    ret = drsOps->GetUint32(node, "audioChannels", &(cntlr->cap.audioChannels), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrParseAudioCaps: read audioChannels fail!");
        return ret;
    }
    HDF_LOGD("HdmiCntlrParseAudioCaps: audioChannels = %d", cntlr->cap.audioChannels);
    return HDF_SUCCESS;
}

static int32_t HdmiCntlrParseHdrCaps(struct HdmiCntlr *cntlr, struct DeviceResourceIface *drsOps,
    const struct DeviceResourceNode *node)
{
    int32_t ret;

    ret = drsOps->GetUint32(node, "hdrColorimetry", &(cntlr->cap.hdrColorimetry), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrParseAudioCaps: read hdrColorimetry fail!");
        return ret;
    }
    HDF_LOGD("HdmiCntlrParseAudioCaps: hdrColorimetry = %d", cntlr->cap.hdrColorimetry);

    ret = drsOps->GetUint32(node, "hdrUserMode", &(cntlr->cap.hdrUserMode), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrParseAudioCaps: read hdrUserMode fail!");
        return ret;
    }
    HDF_LOGD("HdmiCntlrParseAudioCaps: hdrUserMode = %d", cntlr->cap.hdrUserMode);
    return HDF_SUCCESS;
}

int32_t HdmiCntlrParse(struct HdmiCntlr *cntlr, struct HdfDeviceObject *obj)
{
    const struct DeviceResourceNode *node = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;

    if (obj == NULL || cntlr == NULL) {
        HDF_LOGE("HdmiCntlrParse: input param is NULL.");
        return HDF_FAILURE;
    }

    node = obj->property;
    if (node == NULL) {
        HDF_LOGE("HdmiCntlrParse: drs node is NULL.");
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("HdmiCntlrParse: invalid drs ops fail!");
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint32(node, "index", &(cntlr->deviceIndex), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrParse: read hostId fail!");
        return ret;
    }

    ret = drsOps->GetUint32(node, "cap", &(cntlr->cap.baseCap.data), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrParse: read baseCap fail!");
        return ret;
    }

    ret = HdmiCntlrParseVideoCaps(cntlr, drsOps, node);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrParse: read video caps fail!");
        return ret;
    }

    ret = HdmiCntlrParseAudioCaps(cntlr, drsOps, node);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrParse: read audio caps fail!");
        return ret;
    }

    ret = HdmiCntlrParseHdrCaps(cntlr, drsOps, node);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiCntlrParse: read hdr caps fail!");
        return ret;
    }
    return HDF_SUCCESS;
}

static struct HdmiSinkDeviceCapability *HdmiGetSinkCapability(struct HdmiDevice *hdmi)
{
    if (hdmi == NULL) {
        return NULL;
    }

    if (hdmi->edid.edidPhase == false) {
        return NULL;
    }
    return &(hdmi->edid.sinkCap);
}

int32_t HdmiCntlrGetSinkEdid(struct HdmiCntlr *cntlr, uint8_t *buffer, uint32_t len)
{
    int32_t ret;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (HdmiHpdStatusDelayGet(cntlr) == false) {
        HDF_LOGE("no hdmi sink device");
        HdmiCntlrFreeDev(cntlr);
        return HDF_DEV_ERR_NO_DEVICE;
    }
    if (cntlr->hdmi != NULL) {
        return HdmiEdidGetRaw(&(cntlr->hdmi->edid), buffer, len);
    }

    ret  = HdmiCntlrAllocDev(cntlr);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    ret = HdmiEdidRawDataRead(&(cntlr->hdmi->edid), &(cntlr->ddc));
    if (ret != HDF_SUCCESS) {
        HdmiCntlrFreeDev(cntlr);
        return ret;
    }

    if (cntlr->hdmi->edid.edidPhase != true) {
        (void)HdmiEdidPhase(&(cntlr->hdmi->edid));
        cntlr->hdmi->edid.edidPhase = true;
    }
    return HdmiEdidGetRaw(&(cntlr->hdmi->edid), buffer, len);
}

static void HdmiCntlrModeSelect(struct HdmiCntlr *cntlr)
{
    enum HdmiTmdsModeType tmdsMode;
    struct HdmiCommonAttr *commAttr = &(cntlr->attr.commAttr);
    struct HdmiScdcScrambleCap scramble = {0};
    struct HdmiScdcScrambleCap curScramble = {0};
    int32_t ret;

    /* support FRL. */
    if (HdmiFrlSupport(cntlr->frl) == true &&
        cntlr->cap.baseCap.bits.frl > 0) {
        return;
    }

    if (cntlr->cap.baseCap.bits.scdc > 0) {
        ret = HdmiScdcFillScrambleCap(cntlr->scdc, &scramble, &tmdsMode);
        if (ret != HDF_SUCCESS) {
            return;
        }
    } else {
        tmdsMode = (commAttr->enableHdmi == true) ? HDMI_TMDS_MODE_HDMI_1_4 : HDMI_TMDS_MODE_DVI;
    }

    cntlr->tmdsMode = tmdsMode;
    HdmiCntlrTmdsModeSet(cntlr, tmdsMode);
    HdmiCntlrReset(cntlr);

    if (cntlr->cap.baseCap.bits.scdc > 0 &&
        HdmiEdidScdcSupport(cntlr->hdmi) == true) {
        ret = HdmiScdcScrambleGet(cntlr->scdc, &curScramble);
        if (ret != HDF_SUCCESS) {
            return;
        }
        if (scramble.sinkScramble != curScramble.sinkScramble ||
            scramble.sourceScramble != curScramble.sourceScramble ||
            scramble.tmdsBitClockRatio40 != curScramble.tmdsBitClockRatio40) {
            (void)HdmiScdcScrambleSet(cntlr->scdc, &scramble);
        }
    }
}

static void HdmiCommonAttrInit(struct HdmiCntlr *cntlr)
{
    struct HdmiCommonAttr *commAttr = &(cntlr->attr.commAttr);

    commAttr->enableHdmi = true;
    commAttr->enableVideo = true;
    commAttr->enableAudio = true;
    commAttr->audio = true;
    commAttr->avi = true;
    commAttr->hdcpMode = HDMI_HDCP_MODE_1_4;
    commAttr->colorSpace = (enum HdmiColorSpace)cntlr->cap.colorSpace;
    commAttr->quantization = (enum HdmiQuantizationRange)cntlr->cap.quantization;
}

static void HdmiVideoAttrInit(struct HdmiCntlr *cntlr)
{
    struct HdmiVideoAttr *videoAttr = &(cntlr->attr.videoAttr);
    bool colorimetry601;

    videoAttr->colorSpace = (enum HdmiColorSpace)cntlr->cap.colorSpace;
    videoAttr->colorimetry = (enum HdmiColorimetry)cntlr->cap.colorimetry;
    videoAttr->timing = (enum HdmiVideoTiming)cntlr->cap.videoTiming;
    colorimetry601 = ((videoAttr->timing == HDMI_VIDEO_TIMING_720X480P60) ||
        (videoAttr->timing == HDMI_VIDEO_TIMING_720X576P50) ||
        (videoAttr->timing == HDMI_VIDEO_TIMING_1440X240P60) ||
        (videoAttr->timing == HDMI_VIDEO_TIMING_1440X480I60) ||
        (videoAttr->timing == HDMI_VIDEO_TIMING_1440X576I50) ||
        (videoAttr->timing == HDMI_VIDEO_TIMING_1440X576P50) ||
        (videoAttr->timing == HDMI_VIDEO_TIMING_1440X576I60));

    if (videoAttr->timing == HDMI_VIDEO_TIMING_640X480P60) {
        videoAttr->colorimetry = HDMI_COLORIMETRY_ITU709;
        videoAttr->aspect = HDMI_PICTURE_ASPECT_4_3;
    } else if (colorimetry601 == true) {
        videoAttr->colorimetry = HDMI_COLORIMETRY_ITU601;
        videoAttr->aspect = HDMI_PICTURE_ASPECT_4_3;
    } else if (videoAttr->timing <= HDMI_VIDEO_TIMING_4096X2160P120 &&
        videoAttr->timing >= HDMI_VIDEO_TIMING_4096X2160P24) {
        videoAttr->colorimetry = HDMI_COLORIMETRY_ITU709;
        videoAttr->aspect = HDMI_PICTURE_ASPECT_256_135;
    } else {
        videoAttr->colorimetry = HDMI_COLORIMETRY_ITU709;
        videoAttr->aspect = HDMI_PICTURE_ASPECT_16_9;
    }

    videoAttr->pixelRepeat = 1;
    if (videoAttr->timing == HDMI_VIDEO_TIMING_1440X480I60 ||
        videoAttr->timing == HDMI_VIDEO_TIMING_1440X576I50) {
        videoAttr->pixelRepeat = 2;
    }

    if (videoAttr->timing <= HDMI_VIDEO_TIMING_640X480P60) {
        videoAttr->quantization = HDMI_QUANTIZATION_RANGE_DEFAULT;
    } else {
        videoAttr->yccQuantization = HDMI_YCC_QUANTIZATION_RANGE_LIMITED;
    }

    if (videoAttr->timing != HDMI_VIDEO_TIMING_NONE &&
        videoAttr->timing != HDMI_VIDEO_TIMING_640X480P60) {
        videoAttr->colorSpace = HDMI_COLOR_SPACE_YCBCR444;
    } else {
        videoAttr->colorSpace = HDMI_COLOR_SPACE_RGB;
    }

    videoAttr->_3dStruct = HDMI_VS_VIDEO_3D_BUTT;
    videoAttr->activeAspect = HDMI_ACTIVE_FORMAT_ASPECT_PICTURE;
}

static void HdmiAudioAttrInit(struct HdmiCntlr *cntlr)
{
    struct HdmiAudioAttr *audioAttr = &(cntlr->attr.audioAttr);

    audioAttr->ifType = (enum HdmiAudioInterfaceType)cntlr->cap.audioIfType;
    audioAttr->bitDepth = (enum HdmiAudioBitDepth)cntlr->cap.audioBitDepth;
    audioAttr->sampleRate = (enum HdmiSampleRate)cntlr->cap.audioSampleRate;
    audioAttr->channels = (enum HdmiAudioFormatChannel)cntlr->cap.audioChannels;
}

static void HdmiHdrAttrInit(struct HdmiCntlr *cntlr)
{
    struct HdmiHdrAttr *hdrAttr = &(cntlr->attr.hdrAttr);

    hdrAttr->userMode = (enum HdmiHdrUserMode)cntlr->cap.hdrUserMode;
    hdrAttr->colorimetry = (enum HdmiHdrColormetry)cntlr->cap.hdrColorimetry;
}

static void HdmiAttrInit(struct HdmiCntlr *cntlr)
{
    HdmiCommonAttrInit(cntlr);
    HdmiVideoAttrInit(cntlr);
    HdmiAudioAttrInit(cntlr);
    HdmiHdrAttrInit(cntlr);
}

int32_t HdmiCntlrOpen(struct HdmiCntlr *cntlr)
{
    int32_t ret;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    HdmiInfoFrameInit(cntlr);
    HdmiDdcInit(cntlr);
    HdmiScdcInit(cntlr);
    HdmiFrlInit(cntlr);
    HdmiHdcpInit(cntlr);
    HdmiCecInit(cntlr);
    HdmiHdrInit(cntlr);
    HdmiAttrInit(cntlr);
    HdmiCntlrHardWareInit(cntlr);
     /* HDCP on. */
    if ((cntlr->cap.baseCap.data & HDMI_CAP_HDCP_MARK) > 0) {
        ret = HdmiHdcpOpen(cntlr->hdcp);
        if (ret != HDF_SUCCESS) {
            HdmiHdcpClose(cntlr->hdcp);
        }
    }
    cntlr->state |= HDMI_CNTLR_STATE_OPEN;
    return HDF_SUCCESS;
}

int32_t HdmiCntlrStart(struct HdmiCntlr *cntlr)
{
    if (cntlr == NULL || cntlr->ops == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    HdmiCntlrLowPowerSet(cntlr, false);
    /* select mode */
    HdmiCntlrModeSelect(cntlr);
    HdmiCntlrAvmuteSet(cntlr, false);

    if (cntlr->cap.baseCap.bits.hdr > 0 &&
        cntlr->cap.baseCap.bits.frl > 0) {
        HdmiFrlEnable(cntlr->frl, true);
    } else {
        /* TMDS Transfer. */
        if (cntlr->ops->phyOutputEnable != NULL) {
            HdmiCntlrLock(cntlr);
            cntlr->ops->phyOutputEnable(cntlr, true);
            HdmiCntlrUnlock(cntlr);
        }
    }

    HdmiCntlrAudioPathEnable(cntlr, true);
    HdmiCntlrBlackDataSet(cntlr, false);

    cntlr->state |= HDMI_CNTLR_STATE_START;
    return HDF_SUCCESS;
}

int32_t HdmiCntlrStop(struct HdmiCntlr *cntlr)
{
    if (cntlr == NULL || cntlr->ops == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    HdmiCntlrLock(cntlr);
    if (cntlr->ops->audioPathEnable != NULL) {
        cntlr->ops->audioPathEnable(cntlr, false);
    }
    if (cntlr->ops->avmuteSet != NULL) {
        cntlr->ops->avmuteSet(cntlr, true);
    }
    if (cntlr->ops->blackDataSet != NULL) {
        cntlr->ops->blackDataSet(cntlr, true);
    }
    HdmiFrlEnable(cntlr->frl, false);
    if (cntlr->ops->phyOutputEnable != NULL) {
        cntlr->ops->phyOutputEnable(cntlr, false);
    }
    HdmiCntlrUnlock(cntlr);

    HdmiCntlrLowPowerSet(cntlr, true);
    cntlr->state = HDMI_CNTLR_STATE_STOP;
    return HDF_SUCCESS;
}

static void HdmiTmdsClockUpdate(struct HdmiVideoAttr *videoAttr, struct HdmiCommonAttr *commAttr)
{
    /*
     * Video at the default 24-bit color depth is carried at a TMDS clock rate equal to the pixel clock rate.
     * YCBCB 4:2:0 video is carried at a TMDS clock rate equal to the 1/2 pixel clock rate.
     */
    if (commAttr->colorSpace == HDMI_COLOR_SPACE_YCBCR420) {
        videoAttr->tmdsClock = HDMI_MULTIPLE_0P5(videoAttr->pixelClock);
        return;
    }

    /*
     * The TMDS clock rate is ibcreased by the ratio of the pixel size to 24-bits:
     * 24-bit mode: TMDS clock is 1.0 X pixel clock;
     * 30-bit mode: TMDS clock is 1.25 X pixel clock;
     * 36-bit mode: TMDS clock is 1.5 X pixel clock;
     * 48-bit mode: TMDS clock is 2.0 X pixel clock;
     */
    videoAttr->tmdsClock = videoAttr->pixelClock;
    if (commAttr->colorSpace != HDMI_COLOR_SPACE_YCBCR422) {
        if (commAttr->deepColor == HDMI_DEEP_COLOR_30BITS) {
            videoAttr->tmdsClock = HDMI_MULTIPLE_1P25(videoAttr->pixelClock);
        } else if (commAttr->deepColor == HDMI_DEEP_COLOR_36BITS) {
            videoAttr->tmdsClock = HDMI_MULTIPLE_1P5(videoAttr->pixelClock);
        } else if (commAttr->deepColor == HDMI_DEEP_COLOR_48BITS) {
            videoAttr->tmdsClock = HDMI_MULTIPLE_2P0(videoAttr->pixelClock);
        }
    } else {
        if ((commAttr->deepColor != HDMI_DEEP_COLOR_OFF) &&
            (commAttr->deepColor != HDMI_DEEP_COLOR_24BITS)) {
            HDF_LOGD("Y422 foce deepcolor 8bit");
            commAttr->deepColor = HDMI_DEEP_COLOR_24BITS;
        }
        if (videoAttr->timing == HDMI_VIDEO_TIMING_1440X480I60 ||
            videoAttr->timing == HDMI_VIDEO_TIMING_1440X576I50) {
            HDF_LOGD("Y422 is not support at pal and ntsc, force adapt to rgb!");
            commAttr->colorSpace = HDMI_COLOR_SPACE_RGB;
        }
    }
}

static bool HdmiYcbcr420FormatCheck(struct HdmiSinkDeviceCapability *sinkCap, struct HdmiVideoAttr *videoAttr)
{
    uint32_t i;
    enum HdmiVic vic;

    vic =  HdmiCommonGetVic(videoAttr->timing, videoAttr->aspect, false);
    if (vic == HDMI_VIC_NONE) {
        HDF_LOGD("Y420 not have supported vic.");
        return false;
    }

    for (i = 0; i < sinkCap->y420Cap.SupportY420VicNum; i++) {
        if (vic == sinkCap->y420Cap.SupportY420Format[i]) {
            HDF_LOGD("Y420 supported vic is %d.", vic);
            return true;
        }
    }

    for (i = 0; i < sinkCap->y420Cap.onlySupportY420VicNum; i++) {
        if (vic == sinkCap->y420Cap.onlySupportY420Format[i]) {
            HDF_LOGD("Y420 supported vic is %d.", vic);
            return true;
        }
    }

    HDF_LOGD("Y420 have no supported vic.");
    return false;
}

static int32_t HdmiColorSpaceCheck(struct HdmiCntlr *cntlr, struct HdmiSinkDeviceCapability *sinkCap,
    uint32_t maxTmdsClk)
{
    struct HdmiVideoAttr *videoAttr = &(cntlr->attr.videoAttr);
    struct HdmiCommonAttr *commAttr = &(cntlr->attr.commAttr);
    union HdmiCap *cap = &(cntlr->cap.baseCap);
    bool supportColorSpace = false;

    if (videoAttr->pixelClock > maxTmdsClk &&
        commAttr->colorSpace != HDMI_COLOR_SPACE_YCBCR420) {
        return HDF_ERR_INVALID_PARAM;
    }

    switch (commAttr->colorSpace) {
        case HDMI_COLOR_SPACE_RGB:
            supportColorSpace = true;
            if ((sinkCap->colorSpace.rgb444 && cap->bits.rgb444) == false) {
                HDF_LOGD("sink or source not support RGB!");
            }
            break;
        case HDMI_COLOR_SPACE_YCBCR422:
            supportColorSpace = sinkCap->colorSpace.ycbcr422 && cap->bits.ycbcr422;
            break;
        case HDMI_COLOR_SPACE_YCBCR444:
            supportColorSpace = sinkCap->colorSpace.ycbcr444 && cap->bits.ycbcr444;
            break;
        case HDMI_COLOR_SPACE_YCBCR420:
            supportColorSpace = cap->bits.ycbcr420 && HdmiYcbcr420FormatCheck(sinkCap, videoAttr);
            break;
        default:
            HDF_LOGE("unknow color_space = %u!\n", commAttr->colorSpace);
            break;
    }

    if (supportColorSpace != true) {
        return HDF_ERR_INVALID_PARAM;
    }
    return HDF_SUCCESS;
}

static void HdmiDeepColor10bitsCheck(struct HdmiSinkDeviceCapability *sinkCap, struct HdmiCommonAttr *commAttr,
    union HdmiCap *cap, uint32_t *tmdsClock, bool *supportDeepColor)
{
    uint32_t tmpTmdsClk = *tmdsClock;

    switch (commAttr->colorSpace) {
        case HDMI_COLOR_SPACE_RGB:
            tmpTmdsClk = HDMI_MULTIPLE_1P25(tmpTmdsClk);
            *supportDeepColor = (sinkCap->vsdbInfo.deepColor.dc30bit && cap->bits.deepColor10bits) ? true : false;
            break;
        case HDMI_COLOR_SPACE_YCBCR444:
            tmpTmdsClk = HDMI_MULTIPLE_1P25(tmpTmdsClk);
            *supportDeepColor = (sinkCap->vsdbInfo.deepColor.dc30bit && cap->bits.deepColor10bits &&
                sinkCap->vsdbInfo.deepColor.dcY444) ? true : false;
            break;
        case HDMI_COLOR_SPACE_YCBCR420:
            tmpTmdsClk = HDMI_MULTIPLE_1P25(tmpTmdsClk);
            *supportDeepColor = (sinkCap->hfVsdbInfo.dc.dc30bit && cap->bits.deepColor10bits) ? true : false;
            break;
        default:
            /* Y422, ignore deepclr */
            *supportDeepColor = true;
            break;
    }
    *tmdsClock = tmpTmdsClk;
}

static void HdmiDeepColor12bitsCheck(struct HdmiSinkDeviceCapability *sinkCap, struct HdmiCommonAttr *commAttr,
    union HdmiCap *cap, uint32_t *tmdsClock, bool *supportDeepColor)
{
    uint32_t tmpTmdsClk = *tmdsClock;

    switch (commAttr->colorSpace) {
        case HDMI_COLOR_SPACE_RGB:
            tmpTmdsClk = HDMI_MULTIPLE_1P5(tmpTmdsClk);
            *supportDeepColor = (sinkCap->vsdbInfo.deepColor.dc36bit && cap->bits.deepColor12bits) ? true : false;
            break;
        case HDMI_COLOR_SPACE_YCBCR444:
            tmpTmdsClk = HDMI_MULTIPLE_1P5(tmpTmdsClk);
            *supportDeepColor = (sinkCap->vsdbInfo.deepColor.dc36bit && cap->bits.deepColor12bits &&
                sinkCap->vsdbInfo.deepColor.dcY444) ? true : false;
            break;
        case HDMI_COLOR_SPACE_YCBCR420:
            tmpTmdsClk = HDMI_MULTIPLE_1P5(tmpTmdsClk);
            *supportDeepColor = (sinkCap->hfVsdbInfo.dc.dc36bit && cap->bits.deepColor12bits) ? true : false;
            break;
        default:
            /* Y422, ignore deepclr */
            *supportDeepColor = true;
            break;
    }
    *tmdsClock = tmpTmdsClk;
}

static int32_t HdmiDeepColorCheck(struct HdmiCntlr *cntlr, struct HdmiSinkDeviceCapability *sinkCap,
    uint32_t maxTmdsClk)
{
    struct HdmiVideoAttr *videoAttr = &(cntlr->attr.videoAttr);
    struct HdmiCommonAttr *commAttr = &(cntlr->attr.commAttr);
    union HdmiCap *cap = &(cntlr->cap.baseCap);
    uint32_t tmdsClock = videoAttr->pixelClock;
    bool supportDeepColor = false;

    if (commAttr->colorSpace == HDMI_COLOR_SPACE_YCBCR420) {
        tmdsClock = HDMI_MULTIPLE_0P5(tmdsClock);
    }

    switch (commAttr->deepColor) {
        case HDMI_DEEP_COLOR_30BITS:
            HdmiDeepColor10bitsCheck(sinkCap, commAttr, cap, &tmdsClock, &supportDeepColor);
            break;
        case HDMI_DEEP_COLOR_36BITS:
            HdmiDeepColor12bitsCheck(sinkCap, commAttr, cap, &tmdsClock, &supportDeepColor);
            break;
        default:
            commAttr->deepColor = HDMI_DEEP_COLOR_24BITS;
            supportDeepColor = true;
            break;
    }

    if (supportDeepColor == false || tmdsClock > maxTmdsClk) {
        return HDF_ERR_INVALID_PARAM;
    }
    videoAttr->tmdsClock = tmdsClock;
    /* Y422 default 12bit output, deep_color force adapt to 8bit(24bit). */
    if (commAttr->colorSpace == HDMI_COLOR_SPACE_YCBCR422) {
        if (videoAttr->timing == HDMI_VIDEO_TIMING_1440X480I60 ||
            videoAttr->timing == HDMI_VIDEO_TIMING_1440X576I50) {
            HDF_LOGD("force change colorspace to RGB!");
            commAttr->colorSpace = HDMI_COLOR_SPACE_RGB;
        }
        if ((videoAttr->deepColor != HDMI_DEEP_COLOR_24BITS) &&
            (videoAttr->deepColor != HDMI_DEEP_COLOR_OFF)) {
            videoAttr->deepColor = HDMI_DEEP_COLOR_OFF;
            HDF_LOGD("when Y422, deep_color not support 10/12bit!");
        }
    }
    return HDF_SUCCESS;
}

static int32_t HdmiColorBitSelect(struct HdmiCntlr *cntlr)
{
    struct HdmiSinkDeviceCapability *sinkCap = NULL;
    uint32_t maxTmdsClk = 0;

    /* DVI mode, must set RGB and DEEP_COLOR_OFF */
    if (cntlr->attr.commAttr.enableHdmi == false) {
        if (cntlr->attr.commAttr.colorSpace != HDMI_COLOR_SPACE_RGB) {
            HDF_LOGE("DVI mode, the color space is not RGB!");
            return HDF_ERR_INVALID_PARAM;
        }
        cntlr->attr.commAttr.deepColor = HDMI_DEEP_COLOR_OFF;
        cntlr->attr.videoAttr.tmdsClock = cntlr->attr.videoAttr.pixelClock;
        return HDF_SUCCESS;
    }

    /* FRL mode. */
    if (cntlr->cap.baseCap.bits.hdr > 0) {
        HdmiTmdsClockUpdate(&(cntlr->attr.videoAttr), &(cntlr->attr.commAttr));
        if (cntlr->attr.videoAttr.tmdsClock > HDMI_HDMI20_MAX_TMDS_RATE) {
            HDF_LOGD("HDMI2.1, tmds clock exceed max.");
            return HDF_SUCCESS;
        }
    }

    sinkCap = HdmiGetSinkCapability(cntlr->hdmi);
    if (sinkCap == NULL) {
        HdmiTmdsClockUpdate(&(cntlr->attr.videoAttr), &(cntlr->attr.commAttr));
        return HDF_SUCCESS;
    }

    if (sinkCap->maxTmdsClk == 0) {
        /* use default clock. */
        maxTmdsClk = cntlr->cap.defTmdsClock;
    } else {
        maxTmdsClk = (sinkCap->maxTmdsClk < cntlr->cap.maxTmdsClock) ?
            sinkCap->maxTmdsClk : cntlr->cap.maxTmdsClock;
    }
    /* MHz is converted to KHz */
    maxTmdsClk *= 1000;

    if (HdmiColorSpaceCheck(cntlr, sinkCap, maxTmdsClk) != HDF_SUCCESS) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (HdmiDeepColorCheck(cntlr, sinkCap, maxTmdsClk) != HDF_SUCCESS) {
        return HDF_ERR_INVALID_PARAM;
    }
    return HDF_SUCCESS;
}

static int32_t HdmiAudioAttrHandle(struct HdmiCntlr *cntlr)
{
    struct HdmiAudioConfigInfo audioCfg  = {0};
    struct HdmiCommonAttr *commAttr = &(cntlr->attr.commAttr);
    struct HdmiVideoAttr *videoAttr = &(cntlr->attr.videoAttr);
    struct HdmiAudioAttr *audioAttr = &(cntlr->attr.audioAttr);
    int32_t ret;

    audioCfg.downSample = audioAttr->downSample;
    audioCfg.channels = audioAttr->channels;
    audioCfg.bitDepth = audioAttr->bitDepth;
    audioCfg.sampleRate = audioAttr->sampleRate;
    audioCfg.ifType = audioAttr->ifType;
    audioCfg.enable = (commAttr->enableAudio) && (commAttr->enableHdmi);
    audioCfg.tmdsClock = videoAttr->tmdsClock;
    audioCfg.pixelRepeat = videoAttr->pixelRepeat;

    HdmiCntlrAudioPathEnable(cntlr, false);
    HdmiCntlrAudioPathSet(cntlr, &audioCfg);
    ret = HdmiAudioInfoFrameSend(&(cntlr->infoFrame), ((commAttr->enableAudio) && (commAttr->audio)));
    HdmiCntlrAudioPathEnable(cntlr, true);
    return ret;
}

static void HdmiHdrModeCheck(struct HdmiCommonAttr *commAttr,
    struct HdmiVideoAttr *videoAttr, struct HdmiHdrAttr *hdrAttr)
{
    switch (hdrAttr->userMode) {
        case HDMI_HDR_USERMODE_DOLBY:
            videoAttr->yccQuantization = HDMI_YCC_QUANTIZATION_RANGE_LIMITED;
            commAttr->colorSpace = HDMI_COLOR_SPACE_YCBCR444;
            videoAttr->tmdsClock = videoAttr->pixelClock;
            commAttr->deepColor = HDMI_DEEP_COLOR_24BITS;
            videoAttr->quantization = HDMI_QUANTIZATION_RANGE_DEFAULT;
            break;
        case HDMI_HDR_USERMODE_HDR10:
            if (hdrAttr->mode == HDMI_HDR_MODE_CEA_861_3) {
                if (hdrAttr->colorimetry > HDMI_HDR_COLORIMETRY_EXTENDED) {
                    videoAttr->colorimetry = HDMI_COLORIMETRY_EXTENDED;
                    videoAttr->extColorimetry = (enum HdmiExtendedColorimetry)(hdrAttr->colorimetry -
                        HDMI_HDR_EXTENDED_COLORIMETRY_XV_YCC_601);
                } else {
                    videoAttr->colorimetry = (enum HdmiColorimetry)(hdrAttr->colorimetry);
                    videoAttr->extColorimetry = HDMI_EXTENDED_COLORIMETRY_BUTT;
                }
                commAttr->xvyccMode = false;
            }
            break;
        default:
            break;
    }
}

static void HdmiFillVideoAttrFromHardwareStatus(struct HdmiVideoAttr *videoAttr,
    struct HdmiHardwareStatus *hwStatus, struct HdmiCommonAttr *commAttr)
{
    bool rgb = false;
    bool aspectIs256 = false;
    uint8_t vic;
    enum HdmiVsVideoFormat format;
    enum Hdmi4kVic vic4k;

    rgb = ((hwStatus->videoStatus.rgb2Ycbcr) ||
        ((!hwStatus->videoStatus.ycbcr2Rgb) && (hwStatus->videoStatus.outColorSpace == HDMI_COLOR_SPACE_RGB)));
    videoAttr->colorSpace = (rgb == true) ? HDMI_COLOR_SPACE_RGB : HDMI_COLOR_SPACE_YCBCR444;

    if (hwStatus->infoFrameStatus.aviEnable) {
        vic = hwStatus->infoFrameStatus.avi[7];
        /*
         * when the timing is 4096*2160, the aspect ratio in AVI infoFrame is 0
         * (but the real aspect ratio is 256:135<0x04>, the video_code is 0)
         */
        aspectIs256 = (((vic == 0) && (hwStatus->infoFrameStatus.vsif[8] == 0x04)) ||
            ((vic >= HDMI_VIC_4096X2160P25_256_135) && (vic <= HDMI_VIC_4096X2160P60_256_135)));
        videoAttr->aspect = (aspectIs256 == true) ? HDMI_PICTURE_ASPECT_256_135 :
            ((hwStatus->infoFrameStatus.avi[5] >> 4) & 0x3);  /* 4'b, BIT[2:1] */
        videoAttr->activeAspect = hwStatus->infoFrameStatus.avi[5] & 0xf;
        videoAttr->colorimetry = (hwStatus->infoFrameStatus.avi[5] >> 6) & 0x3; /* 6'b, BIT[2:1] */
        videoAttr->quantization = (hwStatus->infoFrameStatus.avi[6] >> 2) & 0x3; /* 2'b, BIT[2:1] */
        videoAttr->extColorimetry = (hwStatus->infoFrameStatus.avi[6] >> 4) & 0x07; /* 4'b, BIT[3:1] */
        videoAttr->yccQuantization = (hwStatus->infoFrameStatus.avi[8] >> 6) & 0x3; /* 6'b, BIT[2:1] */
        videoAttr->pixelRepeat = (hwStatus->infoFrameStatus.avi[8] & 0xf) + 1;
        videoAttr->timing = HdmiCommonGetVideoTiming(vic, videoAttr->aspect);
        if ((!hwStatus->infoFrameStatus.vsifEnable) && (!vic)) {
            videoAttr->timing = HDMI_VIDEO_TIMING_NONE;
        }
        commAttr->quantization = (commAttr->colorSpace == HDMI_COLOR_SPACE_RGB) ?
            videoAttr->quantization : (videoAttr->yccQuantization + 1);
    }

    videoAttr->_3dStruct = HDMI_VS_VIDEO_3D_BUTT;
    if (hwStatus->infoFrameStatus.vsifEnable) {
        format = (hwStatus->infoFrameStatus.vsif[7] >> 5) & 0x3; /* 5'b, BIT[2:1] */
        if (format == HDMI_VS_VIDEO_FORMAT_4K) {
            vic4k = hwStatus->infoFrameStatus.vsif[8];
            videoAttr->timing = HdmiCommonGetVideo4kTiming(vic4k);
        } else if (format == HDMI_VS_VIDEO_FORMAT_3D) {
            videoAttr->_3dStruct = (hwStatus->infoFrameStatus.vsif[8] >> 4) & 0xf; /* 4'b, BIT[4:1] */
        }
    }
}

static void HdmiFillCommonAttrFromHardwareStatus(struct HdmiCommonAttr *commAttr,
    struct HdmiHardwareStatus *hwStatus, struct HdmiAttr *attr)
{
    bool dolby = false;

    switch (hwStatus->commonStatus.tmdsMode) {
        case HDMI_TMDS_MODE_HDMI_1_4:
        case HDMI_TMDS_MODE_HDMI_2_0:
            commAttr->enableHdmi = true;
            commAttr->enableVideo = true;
            break;
        case HDMI_TMDS_MODE_DVI:
            commAttr->enableHdmi = false;
            commAttr->enableVideo = true;
            break;
        default:
            commAttr->enableHdmi = false;
            commAttr->enableVideo = false;
            break;
    }

    commAttr->enableAudio = (hwStatus->audioStatus.enable && hwStatus->infoFrameStatus.audioEnable);
    commAttr->avi = hwStatus->infoFrameStatus.aviEnable;
    commAttr->audio = hwStatus->infoFrameStatus.audioEnable;
    commAttr->xvyccMode = hwStatus->infoFrameStatus.gdbEnable;
    commAttr->deepColor = HdmiCommonColorDepthConvertToDeepClolor(hwStatus->videoStatus.outBitDepth);

    /* color space is ycbcr444 when the hdr mode is dolby. */
    dolby = ((attr->hdrAttr.userMode == HDMI_HDR_USERMODE_DOLBY) &&
        (!hwStatus->videoStatus.rgb2Ycbcr) &&
        (!hwStatus->videoStatus.ycbcr2Rgb) &&
        (!hwStatus->videoStatus.ycbcr444422) &&
        (!hwStatus->videoStatus.ycbcr422420) &&
        (!hwStatus->videoStatus.ycbcr422444) &&
        (!hwStatus->videoStatus.ycbcr420422));
    commAttr->colorSpace = (dolby == true) ? HDMI_COLOR_SPACE_YCBCR444 : hwStatus->videoStatus.outColorSpace;
}

static bool HdmiCommonAttrChanged(struct HdmiCommonAttr *curCommAttr, struct HdmiCommonAttr *commAttr)
{
    bool change;

    change = (commAttr->enableHdmi != curCommAttr->enableHdmi) ||
        (commAttr->colorSpace != curCommAttr->colorSpace) ||
        (commAttr->xvyccMode != curCommAttr->xvyccMode) ||
        (commAttr->avi != curCommAttr->avi) ||
        (commAttr->quantization != curCommAttr->quantization);

    if (change == true) {
        return true;
    }

    if (((commAttr->deepColor == HDMI_DEEP_COLOR_OFF) ||
        (commAttr->deepColor == HDMI_DEEP_COLOR_24BITS)) &&
        ((curCommAttr->deepColor == HDMI_DEEP_COLOR_OFF) ||
        (curCommAttr->deepColor == HDMI_DEEP_COLOR_24BITS))) {
        HDF_LOGI("deep color not change: %u", curCommAttr->deepColor);
    } else if (commAttr->deepColor != curCommAttr->deepColor) {
        return true;
    }
    return false;
}

static bool HdmiVideoAttrChanged(struct HdmiVideoAttr *curVideoAttr, struct HdmiVideoAttr *videoAttr)
{
    bool change;

    change = (videoAttr->timing != curVideoAttr->timing) ||
        (videoAttr->colorSpace != curVideoAttr->colorSpace) ||
        (videoAttr->_3dStruct != curVideoAttr->_3dStruct) ||
        (videoAttr->pixelRepeat != curVideoAttr->pixelRepeat) ||
        (videoAttr->colorimetry != curVideoAttr->colorimetry) ||
        (videoAttr->extColorimetry != curVideoAttr->extColorimetry) ||
        (videoAttr->quantization != curVideoAttr->quantization) ||
        (videoAttr->yccQuantization != curVideoAttr->yccQuantization) ||
        (videoAttr->aspect != curVideoAttr->aspect);
    return change;
}

static int32_t HdmiVedioAttrHandle(struct HdmiCntlr *cntlr, struct HdmiHardwareStatus *hwStatus)
{
    struct HdmiCommonAttr *commAttr = &(cntlr->attr.commAttr);
    struct HdmiVideoAttr *videoAttr = &(cntlr->attr.videoAttr);
    struct HdmiCommonAttr curCommAttr = {0};
    struct HdmiVideoAttr curVideoAttr = {0};
    int32_t ret;

    /* support HDR. */
    if (cntlr->cap.baseCap.bits.hdr > 0) {
        HdmiHdrModeCheck(commAttr, videoAttr, &(cntlr->attr.hdrAttr));
    }

    /* support FRL. */
    if (cntlr->cap.baseCap.bits.frl > 0) {
        ret = HdmiFrlModeSelect(cntlr->frl);
        if (ret != HDF_SUCCESS) {
            return ret;
        }
    }

    HdmiFillCommonAttrFromHardwareStatus(&curCommAttr, hwStatus, &(cntlr->attr));
    HdmiFillVideoAttrFromHardwareStatus(&curVideoAttr, hwStatus, &curCommAttr);
    if (HdmiCommonAttrChanged(&curCommAttr, commAttr) == false &&
        HdmiVideoAttrChanged(&curVideoAttr, videoAttr) == false &&
        HdmiFrlModeChanged(cntlr->frl) == false) {
        HDF_LOGI("video and common attr is not changed.");
        return HDF_SUCCESS;
    }

    if (commAttr->enableVideo == true) {
        HdmiCntlrVideoPathSet(cntlr, videoAttr);
    }

    (void)HdmiAviInfoFrameSend(&(cntlr->infoFrame), (commAttr->enableHdmi && commAttr->avi));
    (void)HdmiVsInfoFrameSend(&(cntlr->infoFrame), commAttr->enableHdmi, commAttr->vsifDolby);
    /* the drm infoFrame is stop because software was reset in videopath, so re-enable it if the mode is HDR10. */
    if (cntlr->cap.baseCap.bits.hdr > 0) {
        if (cntlr->attr.hdrAttr.mode == HDMI_HDR_MODE_CEA_861_3) {
            (void)HdmiDrmInfoFrameSend(&(cntlr->infoFrame), false);
            (void)HdmiDrmInfoFrameSend(&(cntlr->infoFrame), true);
        }
    }
    return HDF_SUCCESS;
}

static void HdmiAttrUpdate(struct HdmiCntlr *cntlr)
{
    struct HdmiHardwareStatus hwStatus = {0};

    if (HdmiColorBitSelect(cntlr) != HDF_SUCCESS) {
        HDF_LOGD("attr update: color bit select fail.");
        return;
    }

    if (cntlr->ops->hardWareStatusGet == NULL) {
        return;
    }

    HdmiCntlrLock(cntlr);
    cntlr->ops->hardWareStatusGet(cntlr, &hwStatus);
    HdmiCntlrUnlock(cntlr);

    if (HdmiAudioAttrHandle(cntlr) != HDF_SUCCESS) {
        HDF_LOGD("audio attr check fail");
        return;
    }
    if (HdmiVedioAttrHandle(cntlr, &hwStatus) != HDF_SUCCESS) {
        HDF_LOGD("vedio attr check fail");
    }
}

int32_t HdmiCntlrDeepColorSet(struct HdmiCntlr *cntlr, enum HdmiDeepColor color)
{
    if (cntlr == NULL || cntlr->ops == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->attr.commAttr.colorSpace == HDMI_COLOR_SPACE_YCBCR422) {
        if (color != HDMI_DEEP_COLOR_24BITS && color != HDMI_DEEP_COLOR_OFF) {
            HDF_LOGE("not support deepcolor %u when colorspace is ycbcr422.", color);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    /* if the current hdmi mode is dolby, deep color set is invalid. */
    if (cntlr->attr.hdrAttr.mode == HDMI_HDR_MODE_DOLBY_NORMAL ||
        cntlr->attr.hdrAttr.mode == HDMI_HDR_MODE_DOLBY_TUNNELING) {
        HDF_LOGD("don't change deepcolor when dolby mode is %u.", cntlr->attr.hdrAttr.mode);
        return HDF_ERR_NOT_SUPPORT;
    }

    cntlr->attr.commAttr.deepColor = color;
    HdmiCntlrAvmuteSet(cntlr, true);
    HdmiAttrUpdate(cntlr);
    HdmiCntlrAvmuteSet(cntlr, false);
    return HDF_SUCCESS;
}

int32_t HdmiCntlrDeepColorGet(struct HdmiCntlr *cntlr, enum HdmiDeepColor *color)
{
    if (cntlr == NULL || cntlr->ops == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (color == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    *color = cntlr->attr.commAttr.deepColor;
    return HDF_SUCCESS;
}

int32_t HdmiCntlrSetVideoAttribute(struct HdmiCntlr *cntlr, struct HdmiVideoAttr *attr)
{
    struct HdmiHardwareStatus hwStatus = {0};

    if (cntlr == NULL || cntlr->ops == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (attr == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    cntlr->attr.videoAttr = *attr;

    if (cntlr->ops->hardWareStatusGet == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    HdmiCntlrLock(cntlr);
    cntlr->ops->hardWareStatusGet(cntlr, &hwStatus);
    HdmiCntlrUnlock(cntlr);
    return HdmiVedioAttrHandle(cntlr, &hwStatus);
}

int32_t HdmiCntlrSetAudioAttribute(struct HdmiCntlr *cntlr, struct HdmiAudioAttr *attr)
{
    if (cntlr == NULL || cntlr->ops == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (attr == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    cntlr->attr.audioAttr = *attr;
    return HdmiAudioAttrHandle(cntlr);
}

int32_t HdmiCntlrSetHdrAttribute(struct HdmiCntlr *cntlr, struct HdmiHdrAttr *attr)
{
    if (cntlr == NULL || cntlr->ops == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (attr == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    return HdmiHdrAttrHandle(cntlr->hdr, attr);
}

int32_t HdmiCntlrInfoFrameGet(struct HdmiCntlr *cntlr, enum HdmiPacketType type, union HdmiInfoFrameInfo *frame)
{
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return HdmiInfoFrameGetInfo(&(cntlr->infoFrame), type, frame);
}

int32_t HdmiCntlrInfoFrameSet(struct HdmiCntlr *cntlr, enum HdmiPacketType type, union HdmiInfoFrameInfo *frame)
{
    if (cntlr == NULL || cntlr->ops == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (frame == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    return HdmiInfoFrameSetInfo(&(cntlr->infoFrame), type, frame);
}

int32_t HdmiCntlrRegisterHpdCallbackFunc(struct HdmiCntlr *cntlr, struct HdmiHpdCallbackInfo *callback)
{
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (callback == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    cntlr->event.callback.data = callback->data;
    cntlr->event.callback.callbackFunc = callback->callbackFunc;
    return HDF_SUCCESS;
}

int32_t HdmiCntlrUnregisterHpdCallbackFunc(struct HdmiCntlr *cntlr)
{
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    cntlr->event.callback.data = NULL;
    cntlr->event.callback.callbackFunc = NULL;
    return HDF_SUCCESS;
}

void HdmiCntlrClose(struct HdmiCntlr *cntlr)
{
    if (cntlr == NULL || cntlr->ops == NULL) {
        return;
    }

    if ((cntlr->state & HDMI_CNTLR_STATE_START) > 0) {
        HdmiCntlrStop(cntlr);
    }

    HdmiInfoFrameDeInit(cntlr);
    HdmiDdcDeinit(cntlr);
    HdmiScdcDeinit(cntlr);
    HdmiFrlDeinit(cntlr);
    HdmiHdcpDeinit(cntlr);
    HdmiCecDeinit(cntlr);
    HdmiHdrDeinit(cntlr);
    cntlr->state = HDMI_CNTLR_STATE_CLOSE;
}
