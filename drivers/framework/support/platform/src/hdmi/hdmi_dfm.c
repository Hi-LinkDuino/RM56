/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdmi_dfm.h"
#include "hdf_log.h"
#include "hdmi_common.h"

#define HDF_LOG_TAG hdmi_dfm_c

#define HDMI_DFM_THOUSAND 1000
#define HDMI_DFM_INVALID_VAL (-1)

uint32_t HdmiDfmGetPixelFormat(enum HdmiColorSpace colorSpace)
{
    uint32_t pixelFormat;

    switch (colorSpace) {
        case HDMI_COLOR_SPACE_RGB:
            pixelFormat = HDMI_DFM_PIXEL_FORMAT_MODE_0;
            break;
        case HDMI_COLOR_SPACE_YCBCR420:
            pixelFormat = HDMI_DFM_PIXEL_FORMAT_MODE_1;
            break;
        case HDMI_COLOR_SPACE_YCBCR422:
            pixelFormat = HDMI_DFM_PIXEL_FORMAT_MODE_2;
            break;
        case HDMI_COLOR_SPACE_YCBCR444:
            pixelFormat = HDMI_DFM_PIXEL_FORMAT_MODE_3;
            break;
        default:
            pixelFormat = HDMI_DFM_PIXEL_FORMAT_MODE_0;
            break;
    }
    return pixelFormat;
}

void HdmiDfmFillParam(struct HdmiDfmParam *param, struct HdmiVideoDefInfo *videoInfo,
    struct HdmiAudioAttr *audioAttr, enum HdmiColorSpace colorSpace, enum HdmiDeepColor deepColor)
{
    if (param == NULL || videoInfo == NULL || audioAttr == NULL) {
        return;
    }
    param->hactive = videoInfo->hactive;
    param->vactive = videoInfo->vactive;
    param->hblank = videoInfo->hblank;
    param->vblank = videoInfo->vblank;
    param->hsync = videoInfo->hsync;
    param->hback = videoInfo->hback;
    param->hfront = videoInfo->hfront;
    param->vsync = videoInfo->vsync;
    param->vback = videoInfo->vback;
    param->vfront = videoInfo->vfront;
    param->vfreq = videoInfo->rate;
    param->colorDepth = HdmiCommonDeepClolorConvertToColorDepth(deepColor);
    param->pixelFormat = HdmiDfmGetPixelFormat(colorSpace);
    param->audioRate = (uint32_t)audioAttr->sampleRate;
    param->layout = (audioAttr->channels > HDMI_AUDIO_FORMAT_CHANNEL_2) ? true : false;
    /* ACAT packet_type */
    param->acat = HDMI_AUDIO_CHANNEL_ALLOC_TYPE3;
    param->packetType = HDMI_AUDIO_SAMPLE_PACKET;
}

static void HdmiDfmBaseInfoInit(struct HdmiDfmInfo *info, struct HdmiDfmParam *param)
{
    info->htotal = param->hactive + param->hblank;
    info->vtotal = param->vactive + param->vblank;
    info->pixelClk = (uint64_t)(info->htotal) * info->vtotal * param->vfreq / HDMI_DFM_THOUSAND;

    /* 1. Determine the maximum legal pixel rate. */
    info->maxPixelClk = info->pixelClk * (HDMI_DFM_THOUSAND + HDMI_DFM_FRL_PIXELCLK_TOLERANCE) / HDMI_DFM_THOUSAND;
    info->minPixelClk = info->pixelClk * (HDMI_DFM_THOUSAND - HDMI_DFM_FRL_PIXELCLK_TOLERANCE) / HDMI_DFM_THOUSAND;
    if (info->maxPixelClk == 0 || info->minPixelClk == 0) {
        HDF_LOGE("max or min pixel clock is 0!");
        return;
    }

    /* 2. Determine the minimum Video Line period. */
    info->lineMinTime = (uint64_t)info->htotal * 1000000000000 / info->maxPixelClk;
    info->lineMaxTime = (uint64_t)info->htotal * 1000000000000 / info->minPixelClk;

    /* 3. Determine the Worst Case Slow Bit Rate. x10000 */
    info->minBitRate = (uint64_t)param->bitRate * 1000000000 * (10000 - HDMI_DFM_FRL_BITRATE_TOLERANCE) / 10000;
    info->maxBitRate = (uint64_t)param->bitRate * 1000000000 * (10000 + HDMI_DFM_FRL_BITRATE_TOLERANCE) / 10000;

    /* 4. Determine the FRL Character Rate */
    info->minFrlCharRate = info->minBitRate / 18;
    info->maxFrlCharRate = info->maxBitRate / 18;

    /* 5. Determine the Total FRL Characters per line Period.  */
    info->minFrlCharsPerLine = (uint32_t)(info->lineMinTime * info->minFrlCharRate * param->laneNum / 1000000000000);
    info->maxFrlCharsPerLine = (uint32_t)(info->lineMaxTime * info->maxFrlCharRate * param->laneNum / 1000000000000);

    info->cFrlSb = HDMI_DFM_FRL_SB_LEN(param->laneNum);
    info->overheadSb = param->laneNum * 100000 / info->cFrlSb;
    info->overheadRs = HDMI_DFM_RS_NUM_PER_CB * HDMI_DFM_FRL_CB_NUM_PER_SB * 100000 / info->cFrlSb;
    info->overheadMap = 25 * 10000 / info->cFrlSb; /* FRL map chars per blk. */
    info->overheadMin = info->overheadSb + info->overheadRs + info->overheadMap;
    info->overheadMax = info->overheadMin + HDMI_DFM_OVERHEAD_SIZE;
}

static uint32_t HdmiDfmGetAudioPackets(struct HdmiDfmParam *param)
{
    uint32_t ap = 0;

    switch (param->packetType) {
        case HDMI_AUDIO_SAMPLE_PACKET:
        case HDMI_ONE_BIT_AUDIO_SAMPLE_PACKET:
            ap = HDMI_AUDIO_AP_SIZE_100;
            if (param->layout == false) {
                ap = HDMI_AUDIO_AP_SIZE_25;
            }
            break;
        case HDMI_DTS_AUDIO_PACKET:
        case HDMI_HBR_AUDIO_PACKET:
        case HDMI_MULTI_STREAM_AUDIO_SAMPLE_PACKET:
        case HDMI_ONE_BIT_MULTI_STREAM_AUDIO_SAMPLE_PACKET:
            ap = HDMI_AUDIO_AP_SIZE_100;
            break;
        case HDMI_AUDIO_3D_SAMPLE_PACKET:
        case HDMI_ONE_BIT_AUDIO_3D_SAMPLE_PACKET:
            if (param->acat == HDMI_AUDIO_CHANNEL_ALLOC_TYPE1) {
                ap = HDMI_AUDIO_AP_SIZE_200;
            } else if (param->acat == HDMI_AUDIO_CHANNEL_ALLOC_TYPE2) {
                ap = HDMI_AUDIO_AP_SIZE_300;
            } else if (param->acat == HDMI_AUDIO_CHANNEL_ALLOC_TYPE3) {
                ap = HDMI_AUDIO_AP_SIZE_400;
            }
            break;
        default:
            HDF_LOGE("audio packet type 0x%x, is not support.", param->packetType);
            break;
    }

    return ap;
}

static void HdmiDfmCaculateAudioInfo(struct HdmiDfmInfo *info, struct HdmiDfmParam *param)
{
    /* 1. Determine the number of audio packets required to carry each sample. */
    info->audioAp = HdmiDfmGetAudioPackets(param);

    /* 2. Determine Average Audio Related Packet Rate. */
    info->audioRap = (uint32_t)((uint64_t)param->audioRate * (1000 + HDMI_DFM_FRL_AUDIOCLK_TOLERANCE) *
        info->audioAp / 100000);

    /* 3. Determine Average Required Packets per line. */
    info->avgAudioPacketsPerLine = (uint32_t)((uint64_t)info->audioRap * info->lineMinTime / 1000000000);

    /* 4. Determine the Packets per Hblank that must be supportable. */
    info->audioPacketsLine = (info->avgAudioPacketsPerLine + HDMI_DFM_THOUSAND - 1) / HDMI_DFM_THOUSAND;

    /*
     * 5. Determine The c_frl_blank_min(64(guard bands, two 12-character control periods, c_frl_active_extra)
     * and 32 audio_packets).
     */
    info->hblankAudioMin = 64 + 32 * info->audioPacketsLine;
}

static void HdmiDfmCaculateVideoBorrowInfo(struct HdmiDfmInfo *info)
{
    if (info->activeTimeRef >= info->activeTimeMin && info->blankTimeRef >= info->blankTimeMin) {
        info->tBorrow = 0;
    } else if (info->activeTimeRef < info->activeTimeMin && info->blankTimeRef >= info->blankTimeMin) {
        info->tBorrow = (int32_t)(info->activeTimeMin - info->activeTimeRef);
    } else {
        info->tBorrow = HDMI_DFM_INVALID_VAL;
    }
    if (info->tBorrow == HDMI_DFM_INVALID_VAL) {
        info->tbBorrow = HDMI_DFM_INVALID_VAL;
    } else {
        info->tbBorrow = ((int32_t)(info->tBorrow * info->avgTbRate / 100000000000) + 10 - 1) / 10;
    }
}

static void HdmiDfmCaculateVideoInfo(struct HdmiDfmInfo *info, struct HdmiDfmParam *param)
{
    uint32_t kcd, k420;

    /*
     * 1. if 4:2:2 pixels, kcd is 1. Otherwise, kcd is CD / 8.
     * if 4:2:0 pixels, k420 is 2. Otherwise, k420 is 1.
     */
    kcd = (param->pixelFormat == 2) ? HDMI_DFM_MAGNIFICATION_8 : param->colorDepth;
    k420 = (param->pixelFormat == 1) ? 2 : 1;

    /* 2. Determine Bits per Pixel */
    info->bpp = ((24 * kcd) / k420) / HDMI_DFM_MAGNIFICATION_8;

    /* 3. Determine Video Bytes per line. */
    info->activeBytesPerLine = info->bpp * param->hactive / HDMI_DFM_MAGNIFICATION_8;

    /*
     * 4. Determine Required Characters to carry Active Video per line.
     * 3 is means active_bytes_per_line need 3 characters.
     */
    info->activeTbPerLine = (info->activeBytesPerLine + 3 - 1) / 3;

    /* 5. Determine Required Characters to carry H-Blank Video per line. */
    info->hblankTbPerLine = (param->hblank * kcd / k420 + HDMI_DFM_MAGNIFICATION_8 - 1) / HDMI_DFM_MAGNIFICATION_8;

    /* 6. 32 is FRL characters each packet, 7 is guard bands island(4 FRL char) + video(3 FRL char) */
    if (((param->hblank * kcd) / k420) / HDMI_DFM_MAGNIFICATION_8 > 32 * (1 + info->audioPacketsLine) + 7) {
        info->cFrlFree = ((param->hblank * kcd) / k420) / HDMI_DFM_MAGNIFICATION_8 -
            32 * (1 + info->audioPacketsLine) - 7;
    }

    /* 7. add 1 character each for RC break caused by 4 */
    info->cFrlRcMargin = 4;
    /* 8. RC compression transmit control characters reduce 7/8th total characters. */
    if (7 * info->cFrlFree / HDMI_DFM_MAGNIFICATION_8 > info->cFrlRcMargin) {
        info->cFrlRcSavings = 7 * info->cFrlFree / HDMI_DFM_MAGNIFICATION_8 - info->cFrlRcMargin;
    }

    if (info->htotal == 0) {
        HDF_LOGE("htotal is 0!");
        return;
    }
    info->avgTbRate = info->maxPixelClk * (info->activeTbPerLine + info->hblankTbPerLine) / info->htotal;
    info->activeTimeRef = (uint64_t)info->lineMinTime * param->hactive / info->htotal;
    info->blankTimeRef = (uint64_t)info->lineMinTime * param->hblank / info->htotal;

    /* 9. (3 / 2) is active_tb coefficient in protocol */
    info->activeTimeMin = (uint64_t)info->activeTbPerLine * 3 * 100000000000000 /
        (2 * param->laneNum * info->minFrlCharRate * (HDMI_DFM_RATE_MAGNIFICATION - info->overheadMax) /
        HDMI_DFM_THOUSAND);
    info->blankTimeMin = (uint64_t)info->hblankTbPerLine * 100000000000000 /
        (param->laneNum * info->minFrlCharRate * (HDMI_DFM_RATE_MAGNIFICATION - info->overheadMax) /
        HDMI_DFM_THOUSAND);

    /* 10. Determine Borrow Info */
    HdmiDfmCaculateVideoBorrowInfo(info);

    /* 11. (3 / 2) is active_tb coefficient in protocol */
    info->cFrlActualPayload =
        (3 * info->activeTbPerLine + 2 - 1) / 2 + info->hblankTbPerLine - info->cFrlRcSavings;

    if (info->minFrlCharsPerLine == 0) {
        HDF_LOGE("min FRL Characters per line is 0!");
        return;
    }
    info->utilization = (uint64_t)info->cFrlActualPayload * 100000 / (uint64_t)info->minFrlCharsPerLine;
    info->margin = (int32_t)(100000 - (info->utilization + info->overheadMax));
}

static void HdmiDfmFillInfo(struct HdmiDfmInfo *info, struct HdmiDfmParam *param)
{
    HdmiDfmBaseInfoInit(info, param);
    HdmiDfmCaculateAudioInfo(info, param);
    HdmiDfmCaculateVideoInfo(info, param);

    /* Check Audio Cap. */
    info->audioSupport = (info->hblankTbPerLine >= info->hblankAudioMin) ? true : false;
    /* Check Video Cap. */
    if (info->tbBorrow == HDMI_DFM_INVALID_VAL) {
        info->videoSupport = false;
    } else {
        info->videoSupport = (info->tbBorrow <= HDMI_DFM_FRL_MAX_TB_BORROW) ? true : false;
    }

    info->uncompressSupport = (info->margin >= 0) ? true : false;
    info->canbeTrans = (info->audioSupport && info->videoSupport && info->uncompressSupport) ? true : false;
    info->isExtraMode = (info->canbeTrans && (info->tbBorrow > 0)) ? true : false;
}

bool HdmiDfmFormatSupport(struct HdmiDfmParam *param)
{
    struct HdmiDfmInfo info = {0};

    HdmiDfmFillInfo(&info, param);
    if (info.audioSupport == true && info.videoSupport == true && info.uncompressSupport == true) {
        return true;
    }
    return false;
}
