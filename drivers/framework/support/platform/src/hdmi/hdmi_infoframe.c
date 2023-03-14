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

#define HDF_LOG_TAG hdmi_infoframe_c

#define HDMI_IEEE_OUI_1_4_1ST 0x03
#define HDMI_IEEE_OUI_1_4_2ND 0x0C
#define HDMI_IEEE_OUI_1_4_3RD 0x00

static void HdmiInfoFrameFillCheckSum(uint8_t *data, uint32_t len)
{
    uint32_t i;
    uint8_t checkSum = 0;

    for (i = 0; i < len; i++) {
        checkSum += data[i];
    }
    if (checkSum > 0) {
        /*
         * The checksum shall be calculated such that a byte-wide sum of all three bytes of the Packet Header and
         * all valid bytes of the InfoFrame Packet contents(determined by InfoFrame_length), plus the checksum itself,
         * equals zero.
         */
        data[UINT8_ARRAY_TElEMENT_3] = HDMI_INFOFRAME_CHECKSUM - checkSum;
    }
}

static void HdmiInfoFrameFillHeader(struct HdmiInfoFrameHeader *header, uint8_t *data, uint32_t len)
{
    if (len < HDMI_INFOFRAME_PACKET_HEADER_LEN) {
        HDF_LOGE("len = %u, val is too small.", len);
        return;
    }
    data[UINT8_ARRAY_TElEMENT_0] = header->type;
    data[UINT8_ARRAY_TElEMENT_1] = header->verNum;
    data[UINT8_ARRAY_TElEMENT_2] = header->len;
}

static int32_t HdmiInfoFramePacketVsEncoding(union HdmiInfoFrameInfo *infoFrame, uint8_t *data, uint32_t len)
{
    uint32_t length;
    struct HdmiVs14VsifContent *vsifContent = NULL;
    struct HdmiVsUserVsifContent *userContent = NULL;
    struct HdmiVsInfoFrame *vs = &(infoFrame->vs);

    length = HDMI_INFOFRAME_PACKET_HEADER_LEN + vs->len;
    if (len < length) {
        HDF_LOGE("len = %u, val is too small.", len);
        return HDF_ERR_INVALID_PARAM;
    }
    if (memset_s(data, len, 0, len) != EOK) {
        HDF_LOGE("memset_s fail.");
        return HDF_ERR_IO;
    }

    HdmiInfoFrameFillHeader(&(infoFrame->header), data, len);
    if (vs->vsifContent.vsif.oui == HDMI_IEEE_OUI_1_4) {
        data[UINT8_ARRAY_TElEMENT_4] = HDMI_IEEE_OUI_1_4_1ST;
        data[UINT8_ARRAY_TElEMENT_5] = HDMI_IEEE_OUI_1_4_2ND;
        data[UINT8_ARRAY_TElEMENT_6] = HDMI_IEEE_OUI_1_4_3RD;
        vsifContent = &(vs->vsifContent.vsif);
        userContent = &(vs->vsifContent.userVsif);
        data[UINT8_ARRAY_TElEMENT_7] = ((uint8_t)vsifContent->format & HDMI_VENDOR_1_4_FORMAT_MARK) <<
                                        HDMI_VENDOR_1_4_FORMAT_SHIFT;
        if (vsifContent->format == HDMI_VS_VIDEO_FORMAT_4K) {
            data[UINT8_ARRAY_TElEMENT_8] = vsifContent->vic;
            return HDF_SUCCESS;
        } else if (vsifContent->format == HDMI_VS_VIDEO_FORMAT_3D) {
            data[UINT8_ARRAY_TElEMENT_8] = ((uint8_t)vsifContent->_3dStruct & HDMI_VENDOR_3D_STRUCTURE_MARK) <<
                                            HDMI_VENDOR_3D_STRUCTURE_SHIFT;
        }
        data[UINT8_ARRAY_TElEMENT_9] = ((uint8_t)vsifContent->_3dExtData & HDMI_VENDOR_3D_EXT_DATA_MARK) <<
                                        HDMI_VENDOR_3D_EXT_DATA_SHIFT;
        if (vsifContent->_3dMetaPresent == false) {
            if (userContent->len == 0 || (userContent->len + length) > len) {
                return HDF_SUCCESS;
            }
            if (memcpy_s(&data[length], (len - length), userContent->data, userContent->len) != EOK) {
                HDF_LOGE("memcpy_s fail.");
                return HDF_ERR_IO;
            }
            length += userContent->len;
        }
    }
    HdmiInfoFrameFillCheckSum(data, length);
    return HDF_SUCCESS;
}

static int32_t HdmiInfoFramePacketAviEncoding(union HdmiInfoFrameInfo *infoFrame, uint8_t *data, uint32_t len)
{
    uint32_t length;
    uint8_t *buff = data;
    struct HdmiAviInfoFrame *avi = &(infoFrame->avi);

    length = HDMI_INFOFRAME_PACKET_HEADER_LEN + avi->len;
    if (len < length) {
        HDF_LOGE("len = %u, val is too small.", len);
        return HDF_ERR_INVALID_PARAM;
    }
    if (memset_s(buff, len, 0, len) != EOK) {
        HDF_LOGE("memset_s fail.");
        return HDF_ERR_IO;
    }

    HdmiInfoFrameFillHeader(&(infoFrame->header), data, len);
    buff += HDMI_INFOFRAME_PACKET_HEADER_LEN;
    /* PB1 */
    buff[UINT8_ARRAY_TElEMENT_0] |= ((uint8_t)avi->colorSpace & HDMI_AVI_COLOR_SPACE_MARK) <<
                                     HDMI_AVI_COLOR_SPACE_SHIFT;
    buff[UINT8_ARRAY_TElEMENT_0] |= ((uint8_t)avi->scanMode & HDMI_AVI_SCAN_MODE_MARK);
    if (avi->activeFormatInformationPresent == true) {
        buff[UINT8_ARRAY_TElEMENT_0] |= (1 << HDMI_AVI_ACTIVE_INFORMATION_SHIFT);
    }
    if (avi->horizBarInfoPresent == true) {
        buff[UINT8_ARRAY_TElEMENT_0] |= (1 << HDMI_AVI_HORIZONTAL_BAR_SHIFT);
    }
    if (avi->vertBarInfoPresent == true) {
        buff[UINT8_ARRAY_TElEMENT_0] |= (1 << HDMI_AVI_VERTICAL_BAR_SHIFT);
    }
    /* PB2 */
    buff[UINT8_ARRAY_TElEMENT_1] |= ((uint8_t)avi->colorimetry & HDMI_AVI_COLORIMETRY_MARK) <<
                                     HDMI_AVI_COLORIMETRY_SHIFT;
    buff[UINT8_ARRAY_TElEMENT_1] |= ((uint8_t)avi->pictureAspect & HDMI_AVI_PICTURE_ASPECT_RATE_MARK) <<
                                     HDMI_AVI_PICTURE_ASPECT_RATE_SHIFT;
    buff[UINT8_ARRAY_TElEMENT_1] |= ((uint8_t)avi->activeAspect & HDMI_AVI_ACTIVE_FORMAT_ASPECT_RATE_MARK);
    /* PB3 */
    buff[UINT8_ARRAY_TElEMENT_2] |= ((uint8_t)avi->extColorimetry & HDMI_AVI_EXT_COLORIMETRY_MARK) <<
                                     HDMI_AVI_EXT_COLORIMETRY_SHIFT;
    buff[UINT8_ARRAY_TElEMENT_2] |= ((uint8_t)avi->range & HDMI_AVI_EXT_QUANTIZATION_RANGE_MARK) <<
                                     HDMI_AVI_EXT_QUANTIZATION_RANGE_SHIFT;
    buff[UINT8_ARRAY_TElEMENT_2] |= ((uint8_t)avi->nups & HDMI_AVI_NUPS_RANGE_MARK);
    if (avi->itc == true) {
        buff[UINT8_ARRAY_TElEMENT_2] |= (1 << HDMI_AVI_IT_CONTENT_SHIFT);
    }
    /* PB4 */
    buff[UINT8_ARRAY_TElEMENT_3] = avi->vic;
    /* PB5 */
    buff[UINT8_ARRAY_TElEMENT_4] |= ((uint8_t)avi->yccRange & HDMI_AVI_YCC_QUANTIZATION_RANGE_MARK) <<
                                     HDMI_AVI_YCC_QUANTIZATION_RANGE_SHIFT;
    buff[UINT8_ARRAY_TElEMENT_4] |= ((uint8_t)avi->itcType & HDMI_AVI_IT_CONTENT_TYPE_MARK) <<
                                     HDMI_AVI_IT_CONTENT_TYPE_SHIFT;
    buff[UINT8_ARRAY_TElEMENT_4] |= (avi->pixelRepetitionFactor & HDMI_AVI_PIXEL_REPETION_FACTOR_MARK);
    /* PB6 */
    buff[UINT8_ARRAY_TElEMENT_5] = (uint8_t)(avi->topBar & HDMI_AVI_BAR_MODE_MARK);
    /* PB7 */
    buff[UINT8_ARRAY_TElEMENT_6] = (uint8_t)((avi->topBar >> HDMI_AVI_BAR_MODE_SHIFT) & HDMI_AVI_BAR_MODE_MARK);
    /* PB8 */
    buff[UINT8_ARRAY_TElEMENT_7] = (uint8_t)(avi->bottomBar & HDMI_AVI_BAR_MODE_MARK);
    /* PB9 */
    buff[UINT8_ARRAY_TElEMENT_8] = (uint8_t)((avi->bottomBar >> HDMI_AVI_BAR_MODE_SHIFT) & HDMI_AVI_BAR_MODE_MARK);
    /* PB10 */
    buff[UINT8_ARRAY_TElEMENT_9] = (uint8_t)(avi->leftBar & HDMI_AVI_BAR_MODE_MARK);
    /* PB11 */
    buff[UINT8_ARRAY_TElEMENT_10] = (uint8_t)((avi->leftBar >> HDMI_AVI_BAR_MODE_SHIFT) & HDMI_AVI_BAR_MODE_MARK);
    /* PB12 */
    buff[UINT8_ARRAY_TElEMENT_11] = (uint8_t)(avi->rightBar & HDMI_AVI_BAR_MODE_MARK);
    /* PB13 */
    buff[UINT8_ARRAY_TElEMENT_12] = (uint8_t)((avi->rightBar >> HDMI_AVI_BAR_MODE_SHIFT) & HDMI_AVI_BAR_MODE_MARK);
    HdmiInfoFrameFillCheckSum(data, length);
    return HDF_SUCCESS;
}

static int32_t HdmiInfoFramePacketSpdEncoding(union HdmiInfoFrameInfo *infoFrame, uint8_t *data, uint32_t len)
{
    uint32_t length;
    uint8_t *buff = data;
    struct HdmiSpdInfoFrame *spd = &(infoFrame->spd);

    length = HDMI_INFOFRAME_PACKET_HEADER_LEN + spd->len;
    if (len < length) {
        HDF_LOGE("len = %u, val is too small.", len);
        return HDF_ERR_INVALID_PARAM;
    }
    if (memset_s(buff, len, 0, len) != EOK) {
        HDF_LOGE("memset_s fail.");
        return HDF_ERR_IO;
    }

    buff += HDMI_INFOFRAME_PACKET_HEADER_LEN;
    /* PB1~PB8 */
    if (memcpy_s(buff, (len - HDMI_INFOFRAME_PACKET_HEADER_LEN), spd->vendorName, sizeof(spd->vendorName)) != EOK) {
        HDF_LOGE("memcpy_s fail.");
        return HDF_ERR_IO;
    }
    buff += HDMI_SPD_VENDOR_NAME_LEN;
    /* PB9~PB24 */
    if (memcpy_s(buff, (len - HDMI_INFOFRAME_PACKET_HEADER_LEN - HDMI_SPD_VENDOR_NAME_LEN),
        spd->productDescription, sizeof(spd->productDescription)) != EOK) {
        HDF_LOGE("memcpy_s fail.");
        return HDF_ERR_IO;
    }
    buff += HDMI_SPD_PRODUCT_DESCRIPTION_LEN;
    /* PB25 */
    buff[UINT8_ARRAY_TElEMENT_0] = spd->sdi;
    HdmiInfoFrameFillCheckSum(data, length);
    return HDF_SUCCESS;
}

static int32_t HdmiInfoFramePacketAudioEncoding(union HdmiInfoFrameInfo *infoFrame, uint8_t *data, uint32_t len)
{
    uint32_t length;
    uint8_t *buff = data;
    struct HdmiAudioInfoFrame *audio = &(infoFrame->audio);

    length = HDMI_INFOFRAME_PACKET_HEADER_LEN + audio->len;
    if (len < length) {
        HDF_LOGE("len = %u, val is too small.", len);
        return HDF_ERR_INVALID_PARAM;
    }
    if (memset_s(buff, len, 0, len) != EOK) {
        HDF_LOGE("memset_s fail.");
        return HDF_ERR_IO;
    }

    HdmiInfoFrameFillHeader(&(infoFrame->header), data, len);
    buff += HDMI_INFOFRAME_PACKET_HEADER_LEN;
    /* PB1 */
    buff[UINT8_ARRAY_TElEMENT_0] |= ((uint8_t)audio->codingType & HDMI_AUDIO_CODING_TYPE_MARK) <<
                                     HDMI_AUDIO_CODING_TYPE_SHIFT;
    buff[UINT8_ARRAY_TElEMENT_0] |= ((uint8_t)audio->channelCount & HDMI_AUDIO_CHANNEL_COUNT_MARK);
    /* PB2 */
    buff[UINT8_ARRAY_TElEMENT_1] |= ((uint8_t)audio->sampleFreq & HDMI_AUDIO_SAMPLE_FREQUENCY_MARK) <<
                                     HDMI_AUDIO_SAMPLE_FREQUENCY_SHIFT;
    buff[UINT8_ARRAY_TElEMENT_1] |= ((uint8_t)audio->sampleSize & HDMI_AUDIO_SAMPLE_SIZE_MARK);
    /* PB3 */
    buff[UINT8_ARRAY_TElEMENT_2] |= ((uint8_t)audio->codingExtType & HDMI_AUDIO_CXT_MARK);
    /* PB4 */
    buff[UINT8_ARRAY_TElEMENT_3] |= audio->channelAllocation;
    /* PB5 */
    buff[UINT8_ARRAY_TElEMENT_4] |= ((uint8_t)audio->levelShiftValue & HDMI_AUDIO_LEVEL_SHIFT_VALUE_MARK) <<
                                     HDMI_AUDIO_LEVEL_SHIFT_VALUE_SHIFT;
    buff[UINT8_ARRAY_TElEMENT_4] |= ((uint8_t)audio->playBackLevel & HDMI_AUDIO_LEF_PLAYBACK_LEVEL_MARK);
    if (audio->dmInh == true) {
        buff[UINT8_ARRAY_TElEMENT_4] |= (1 << HDMI_AUDIO_DM_INH_SHIFT);
    }
    HdmiInfoFrameFillCheckSum(data, length);
    return HDF_SUCCESS;
}

static int32_t HdmiInfoFramePacketDrmEncoding(union HdmiInfoFrameInfo *infoFrame, uint8_t *data, uint32_t len)
{
    uint32_t length;
    uint8_t *buff = data;
    struct HdmiDrmInfoFrame *drm = &(infoFrame->drm);
    struct HdmiStaticMetadataDescriptor1st *des = &(drm->des.type1);

    length = HDMI_INFOFRAME_PACKET_HEADER_LEN + drm->len;
    if (len < length) {
        HDF_LOGE("len = %u, val is too small.", len);
        return HDF_ERR_INVALID_PARAM;
    }
    if (memset_s(buff, len, 0, len) != EOK) {
        HDF_LOGE("memset_s fail.");
        return HDF_ERR_IO;
    }

    HdmiInfoFrameFillHeader(&(infoFrame->header), data, len);
    buff += HDMI_INFOFRAME_PACKET_HEADER_LEN;
    /* PB1 */
    buff[UINT8_ARRAY_TElEMENT_0] = drm->eotfType;
    /* PB2 */
    buff[UINT8_ARRAY_TElEMENT_1] = drm->metadataType;
    /* PB3 */
    buff[UINT8_ARRAY_TElEMENT_2] = (uint8_t)(des->displayPrimaries0X & HDMI_DRM_METADATA_MARK);
    /* PB4 */
    buff[UINT8_ARRAY_TElEMENT_3] = (uint8_t)((des->displayPrimaries0X >> HDMI_DRM_METADATA_SHIFT) &
                                             HDMI_DRM_METADATA_MARK);
    /* PB5 */
    buff[UINT8_ARRAY_TElEMENT_4] = (uint8_t)(des->displayPrimaries0Y & HDMI_DRM_METADATA_MARK);
    /* PB6 */
    buff[UINT8_ARRAY_TElEMENT_5] = (uint8_t)((des->displayPrimaries0Y & HDMI_DRM_METADATA_MARK) &
                                             HDMI_DRM_METADATA_MARK);
    /* PB7 */
    buff[UINT8_ARRAY_TElEMENT_6] = (uint8_t)(des->displayPrimaries1X & HDMI_DRM_METADATA_MARK);
    /* PB8 */
    buff[UINT8_ARRAY_TElEMENT_7] = (uint8_t)((des->displayPrimaries1X & HDMI_DRM_METADATA_MARK) &
                                             HDMI_DRM_METADATA_MARK);
    /* PB9 */
    buff[UINT8_ARRAY_TElEMENT_8] = (uint8_t)(des->displayPrimaries1Y & HDMI_DRM_METADATA_MARK);
    /* PB10 */
    buff[UINT8_ARRAY_TElEMENT_9] = (uint8_t)((des->displayPrimaries1Y & HDMI_DRM_METADATA_MARK) &
                                             HDMI_DRM_METADATA_MARK);
    /* PB11 */
    buff[UINT8_ARRAY_TElEMENT_10] = (uint8_t)(des->displayPrimaries2X & HDMI_DRM_METADATA_MARK);
    /* PB12 */
    buff[UINT8_ARRAY_TElEMENT_11] = (uint8_t)((des->displayPrimaries2X & HDMI_DRM_METADATA_MARK) &
                                              HDMI_DRM_METADATA_MARK);
    /* PB13 */
    buff[UINT8_ARRAY_TElEMENT_12] = (uint8_t)(des->displayPrimaries2Y & HDMI_DRM_METADATA_MARK);
    /* PB14 */
    buff[UINT8_ARRAY_TElEMENT_13] = (uint8_t)((des->displayPrimaries2Y & HDMI_DRM_METADATA_MARK) &
                                              HDMI_DRM_METADATA_MARK);
    /* PB15 */
    buff[UINT8_ARRAY_TElEMENT_14] = (uint8_t)(des->whitePointX & HDMI_DRM_METADATA_MARK);
    /* PB16 */
    buff[UINT8_ARRAY_TElEMENT_15] = (uint8_t)((des->whitePointX & HDMI_DRM_METADATA_MARK) & HDMI_DRM_METADATA_MARK);
    /* PB17 */
    buff[UINT8_ARRAY_TElEMENT_16] = (uint8_t)(des->whitePointY & HDMI_DRM_METADATA_MARK);
    /* PB18 */
    buff[UINT8_ARRAY_TElEMENT_17] = (uint8_t)((des->whitePointY & HDMI_DRM_METADATA_MARK) & HDMI_DRM_METADATA_MARK);
    /* PB19 */
    buff[UINT8_ARRAY_TElEMENT_18] = (uint8_t)(des->maxDisplayMasteringLuminance & HDMI_DRM_METADATA_MARK);
    /* PB20 */
    buff[UINT8_ARRAY_TElEMENT_19] = (uint8_t)((des->maxDisplayMasteringLuminance & HDMI_DRM_METADATA_MARK) &
                                              HDMI_DRM_METADATA_MARK);
    /* PB21 */
    buff[UINT8_ARRAY_TElEMENT_20] = (uint8_t)(des->minDisplayMasteringLuminance & HDMI_DRM_METADATA_MARK);
    /* PB22 */
    buff[UINT8_ARRAY_TElEMENT_21] = (uint8_t)((des->minDisplayMasteringLuminance & HDMI_DRM_METADATA_MARK) &
                                              HDMI_DRM_METADATA_MARK);
    /* PB23 */
    buff[UINT8_ARRAY_TElEMENT_22] = (uint8_t)(des->maxContentLightLevel & HDMI_DRM_METADATA_MARK);
    /* PB24 */
    buff[UINT8_ARRAY_TElEMENT_23] = (uint8_t)((des->maxContentLightLevel & HDMI_DRM_METADATA_MARK) &
                                              HDMI_DRM_METADATA_MARK);
    /* PB25 */
    buff[UINT8_ARRAY_TElEMENT_24] = (uint8_t)(des->maxFrameAverageLightLevel & HDMI_DRM_METADATA_MARK);
    /* PB26 */
    buff[UINT8_ARRAY_TElEMENT_25] = (uint8_t)((des->maxFrameAverageLightLevel & HDMI_DRM_METADATA_MARK) &
                                              HDMI_DRM_METADATA_MARK);
    HdmiInfoFrameFillCheckSum(data, length);
    return HDF_SUCCESS;
}

static int32_t HdmiInfoFramePacketEncoding(union HdmiInfoFrameInfo *infoFrame,
    enum HdmiPacketType type, uint8_t *data, uint32_t len)
{
    int32_t ret;

    if (infoFrame == NULL || data == NULL) {
        HDF_LOGE("input param is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }

    switch (type) {
        case HDMI_INFOFRAME_PACKET_TYPE_VS:
            ret = HdmiInfoFramePacketVsEncoding(infoFrame, data, len);
            break;
        case HDMI_INFOFRAME_PACKET_TYPE_AVI:
            ret = HdmiInfoFramePacketAviEncoding(infoFrame, data, len);
            break;
        case HDMI_INFOFRAME_PACKET_TYPE_SPD:
            ret = HdmiInfoFramePacketSpdEncoding(infoFrame, data, len);
            break;
        case HDMI_INFOFRAME_PACKET_TYPE_AUDIO:
            ret = HdmiInfoFramePacketAudioEncoding(infoFrame, data, len);
            break;
        case HDMI_INFOFRAME_PACKET_TYPE_DRM:
            ret = HdmiInfoFramePacketDrmEncoding(infoFrame, data, len);
            break;
        default:
            HDF_LOGD("type %d not support.", type);
            ret = HDF_ERR_NOT_SUPPORT;
    }
    return ret;
}

static int32_t HdmiInfoFrameSend(struct HdmiInfoFrame *frame, union HdmiInfoFrameInfo *infoFrame)
{
    uint8_t buffer[HDMI_INFOFRAME_LEN] = {0};
    struct HdmiCntlr *cntlr = NULL;
    int32_t ret;

    if (frame == NULL || frame->priv == NULL || infoFrame == NULL) {
        HDF_LOGE("HdmiInfoFrameSend: input param is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }
    cntlr = (struct HdmiCntlr *)frame->priv;
    if (cntlr->ops == NULL || cntlr->ops->infoFrameSend == NULL || cntlr->ops->infoFrameEnable == NULL) {
        HDF_LOGD("HdmiInfoFrameSend not support.");
        return HDF_ERR_NOT_SUPPORT;
    }

    ret = HdmiInfoFramePacketEncoding(infoFrame, infoFrame->header.type, buffer, HDMI_INFOFRAME_LEN);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("encding infoFrame %d fail", infoFrame->header.type);
        return ret;
    }

    HdmiCntlrLock(cntlr);
    cntlr->ops->infoFrameEnable(cntlr, infoFrame->header.type, false);
    ret = cntlr->ops->infoFrameSend(cntlr, infoFrame->header.type, buffer, HDMI_INFOFRAME_LEN);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("send infoFrame %d fail", infoFrame->header.type);
        HdmiCntlrUnlock(cntlr);
        return ret;
    }
    cntlr->ops->infoFrameEnable(cntlr, infoFrame->header.type, true);
    HdmiCntlrUnlock(cntlr);
    return HDF_SUCCESS;
}

static void HdmiFillAviHdrInfoFrame(struct HdmiAviInfoFrame *avi,
    struct HdmiVideoAttr *videoAttr, struct HdmiHdrAttr *hdrAttr, struct HdmiCommonAttr *commAttr)
{
    switch (hdrAttr->mode) {
        case HDMI_HDR_MODE_CEA_861_3:
        case HDMI_HDR_MODE_CEA_861_3_AUTHEN:
            avi->colorimetry = videoAttr->colorimetry;
            avi->extColorimetry = videoAttr->extColorimetry;
            avi->colorSpace = commAttr->colorSpace;
            if (hdrAttr->mode == HDMI_HDR_MODE_CEA_861_3_AUTHEN) {
                avi->colorSpace = HDMI_COLOR_SPACE_YCBCR422;
            }
            avi->range = videoAttr->quantization;
            avi->yccRange = videoAttr->yccQuantization;
            break;
        case HDMI_HDR_MODE_DOLBY_NORMAL:
            avi->colorSpace = HDMI_COLOR_SPACE_YCBCR422;
            avi->colorimetry = (videoAttr->xvycc == true) ? HDMI_COLORIMETRY_EXTENDED : videoAttr->colorimetry;
            avi->extColorimetry = videoAttr->extColorimetry;
            avi->range = videoAttr->quantization;
            avi->yccRange = HDMI_YCC_QUANTIZATION_RANGE_FULL;
            break;
        case HDMI_HDR_MODE_DOLBY_TUNNELING:
            avi->colorSpace = HDMI_COLOR_SPACE_RGB;
            avi->colorimetry = (videoAttr->xvycc == true) ? HDMI_COLORIMETRY_EXTENDED : videoAttr->colorimetry;
            avi->extColorimetry = videoAttr->extColorimetry;
            avi->range = HDMI_QUANTIZATION_RANGE_FULL;
            avi->yccRange = videoAttr->yccQuantization;
            break;
        default:
            avi->colorSpace = commAttr->colorSpace;
            avi->colorimetry = (videoAttr->xvycc == true) ? HDMI_COLORIMETRY_EXTENDED : videoAttr->colorimetry;
            avi->extColorimetry = videoAttr->extColorimetry;
            avi->range = videoAttr->quantization;
            avi->yccRange = videoAttr->yccQuantization;
            break;
    }
}

static void HdmiFillAviInfoFrameVersion(struct HdmiAviInfoFrame *avi)
{
    /*
     * see hdmi spec2.0 10.1.
     * The Y2 and VIC7 bits are simply set to zero in a Version 2 AVI InfoFrame and might not be decoded by
     * some Sinks. A Version 3 AVI InfoFrame shall be used and the Version field set to 0x03 (indicating that
     * the Sink shall decode the additional most-significant bits) whenever either of the most-significant bits
     * Y2 or VIC7 are set to '1'. If both Y2 and VIC7 are set to '0', then a Version 2 AVI InfoFrame shall be used
     * and the Version field shall be set to 0x02 (indicating that the Sink does not have to decode the additional
     * most-significant bits).
     */
    avi->verNum = HDMI_AVI_VERSION2;
    if (((uint32_t)avi->colorSpace & HDMI_AVI_Y2_MASK) > 0 ||
        (avi->vic > HDMI_VIC_5120X2160P100_64_27)) {
        avi->verNum = HDMI_AVI_VERSION3;
    } else if (avi->colorimetry == HDMI_COLORIMETRY_EXTENDED &&
               avi->extColorimetry == HDMI_EXTENDED_COLORIMETRY_ADDITIONAL) {
        /*
         * (C1,C0) is (1,1) and (EC2,EC1,EC0) is (1,1,1), version shall be 4.
         * All fields of the Version 4 AVI InfoFrame are the same as Version 3 AVI InfoFrame,
         * except for the InfoFrame Version Number, Length of AVI InfoFrame, and additional Data Byte 14.
         */
        avi->verNum = HDMI_AVI_VERSION4;
        (avi->len)++;
    }
}

static void HdmiFillAviInfoFrame(struct HdmiAviInfoFrame *avi,
    struct HdmiVideoAttr *videoAttr, struct HdmiHdrAttr *hdrAttr, struct HdmiCommonAttr *commAttr)
{
    bool enable3d = true;

    if (memset_s(avi, sizeof(struct HdmiAviInfoFrame), 0, sizeof(struct HdmiAviInfoFrame)) != EOK) {
        HDF_LOGE("fill vsif, memset_s fail.");
        return;
    }
    avi->type = HDMI_INFOFRAME_PACKET_TYPE_AVI;
    avi->len = HDMI_AVI_INFOFRAME_LEN;

    avi->activeFormatInformationPresent = true;
    avi->colorSpace = commAttr->colorSpace;
    avi->activeAspect = videoAttr->activeAspect;
    avi->pictureAspect = videoAttr->aspect;
    avi->colorimetry = (videoAttr->xvycc == true) ? HDMI_COLORIMETRY_EXTENDED : videoAttr->colorimetry;
    avi->nups = videoAttr->nups;

    avi->range = videoAttr->quantization;
    avi->extColorimetry = videoAttr->extColorimetry;
    if (videoAttr->_3dStruct >= HDMI_VS_VIDEO_3D_BUTT) {
        enable3d = false;
    }
    avi->vic = HdmiCommonGetVic(videoAttr->timing, videoAttr->aspect, enable3d);
    avi->pixelRepetitionFactor = (uint8_t)videoAttr->pixelRepeat;
    avi->yccRange = videoAttr->yccQuantization;
    HdmiFillAviHdrInfoFrame(avi, videoAttr, hdrAttr, commAttr);
    HdmiFillAviInfoFrameVersion(avi);
}

int32_t HdmiAviInfoFrameSend(struct HdmiInfoFrame *frame, bool enable)
{
    struct HdmiCntlr *cntlr = NULL;
    union HdmiInfoFrameInfo infoFrame = {0};

    if (frame == NULL || frame->priv == NULL) {
        HDF_LOGE("HdmiAviInfoFrameSend: input param is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }
    cntlr = (struct HdmiCntlr *)frame->priv;
    if (cntlr->ops == NULL || cntlr->ops->infoFrameEnable == NULL || cntlr->ops->infoFrameSend == NULL) {
        HDF_LOGD("HdmiAviInfoFrameSend not support.");
        return HDF_ERR_NOT_SUPPORT;
    }

    if (enable == false) {
        cntlr->ops->infoFrameEnable(cntlr, HDMI_INFOFRAME_PACKET_TYPE_AVI, false);
        return HDF_SUCCESS;
    }
    HdmiFillAviInfoFrame(&(frame->avi), &(cntlr->attr.videoAttr), &(cntlr->attr.hdrAttr), &(cntlr->attr.commAttr));
    infoFrame.avi = frame->avi;
    return HdmiInfoFrameSend(frame, &infoFrame);
}

void HdmiFillAudioInfoFrame(struct HdmiAudioInfoFrame *audio, struct HdmiAudioAttr *audioAttr)
{
    if (memset_s(audio, sizeof(struct HdmiAudioInfoFrame), 0, sizeof(struct HdmiAudioInfoFrame)) != EOK) {
        HDF_LOGE("fill vsif, memset_s fail.");
        return;
    }
    audio->type = HDMI_INFOFRAME_PACKET_TYPE_AUDIO;
    audio->len = HDMI_AUDIO_INFOFRAME_LEN;
    audio->verNum = HDMI_AUDIO_INFOFRAME_VERSION;

    /* fill channels. */
    if (audioAttr->ifType != HDMI_AUDIO_IF_TYPE_I2S) {
        HDF_LOGI("audio channel refer to stream.");
    } else {
        HDF_LOGI("audio channel %u \n", audioAttr->channels);
        audio->channelCount = (audioAttr->channels >= 2) ? (audioAttr->channels - 1) : 0;
    }

    /* fill coding type. */
    if (audioAttr->codingType == HDMI_AUDIO_CODING_TYPE_AC3 ||
        audioAttr->codingType == HDMI_AUDIO_CODING_TYPE_DTS ||
        audioAttr->codingType == HDMI_AUDIO_CODING_TYPE_EAC3 ||
        audioAttr->codingType == HDMI_AUDIO_CODING_TYPE_DTS_HD) {
        audio->codingType = audioAttr->codingType;
    } else {
        audio->codingType = HDMI_AUDIO_CODING_TYPE_STREAM;
    }

    /* fill CA field. see CEA-861-D table 20. */
    switch (audioAttr->channels) {
        case HDMI_AUDIO_FORMAT_CHANNEL_3:
            audio->channelAllocation = 0x01; /* 1 channel */
            break;
        case HDMI_AUDIO_FORMAT_CHANNEL_6:
            audio->channelAllocation = 0x0b; /* 3, 4 channel */
            break;
        case HDMI_AUDIO_FORMAT_CHANNEL_8:
            audio->channelAllocation = 0x13; /* 1, 2, 5 channel */
            break;
        default:
            audio->channelAllocation = 0x00; /* 0 channel */
            break;
    }
}

int32_t HdmiAudioInfoFrameSend(struct HdmiInfoFrame *frame, bool enable)
{
    struct HdmiCntlr *cntlr = NULL;
    union HdmiInfoFrameInfo infoFrame = {0};

    if (frame == NULL || frame->priv == NULL) {
        HDF_LOGE("HdmiAudioInfoFrameSend: input param is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }
    cntlr = (struct HdmiCntlr *)frame->priv;
    if (cntlr->ops == NULL || cntlr->ops->infoFrameEnable == NULL) {
        HDF_LOGD("HdmiAudioInfoFrameSend not support.");
        return HDF_ERR_NOT_SUPPORT;
    }

    if (enable == false) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->infoFrameEnable(cntlr, HDMI_INFOFRAME_PACKET_TYPE_AUDIO, false);
        HdmiCntlrUnlock(cntlr);
        return HDF_SUCCESS;
    }
    HdmiFillAudioInfoFrame(&(frame->audio), &(cntlr->attr.audioAttr));
    infoFrame.audio = frame->audio;
    return HdmiInfoFrameSend(frame, &infoFrame);
}

static void HdmiFillDrmInfoFrame(struct HdmiDrmInfoFrame *drm, struct HdmiHdrAttr *hdrAttr)
{
    if (memset_s(drm, sizeof(struct HdmiDrmInfoFrame), 0, sizeof(struct HdmiDrmInfoFrame)) != EOK) {
        HDF_LOGE("fill vsif, memset_s fail.");
        return;
    }
    drm->type = HDMI_INFOFRAME_PACKET_TYPE_DRM;
    drm->len = HDMI_DRM_INFOFRAME_LEN;
    drm->verNum = HDMI_DRM_INFOFRAME_VERSION;
    drm->eotfType = hdrAttr->eotfType;
    drm->metadataType = hdrAttr->metadataType;
    drm->des = hdrAttr->descriptor;
}

int32_t HdmiDrmInfoFrameSend(struct HdmiInfoFrame *frame, bool enable)
{
    struct HdmiCntlr *cntlr = NULL;
    union HdmiInfoFrameInfo infoFrame = {0};

    if (frame == NULL || frame->priv == NULL) {
        HDF_LOGE("HdmiDrmInfoFrameSend: input param is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }
    cntlr = (struct HdmiCntlr *)frame->priv;
    if (cntlr->ops == NULL || cntlr->ops->infoFrameEnable == NULL) {
        HDF_LOGD("HdmiDrmInfoFrameSend not support.");
        return HDF_ERR_NOT_SUPPORT;
    }

    if (enable == false) {
        cntlr->ops->infoFrameEnable(cntlr, HDMI_INFOFRAME_PACKET_TYPE_DRM, false);
        return HDF_SUCCESS;
    }
    HdmiFillDrmInfoFrame(&(frame->drm), &(cntlr->attr.hdrAttr));
    infoFrame.drm = frame->drm;
    return HdmiInfoFrameSend(frame, &infoFrame);
}

static uint8_t HdmiGetVsifLength(struct HdmiVs14VsifContent *_14Vsif, bool dolbyEnable, bool hdrSupport)
{
    uint8_t length = 0x07;

    if (hdrSupport == true) {
        if (dolbyEnable == true) {
            length = 0x18;
        } else if (_14Vsif->format == HDMI_VS_VIDEO_FORMAT_4K) {
            length = 0x05;
        } else if (_14Vsif->format == HDMI_VS_VIDEO_FORMAT_3D) {
            length = 0x07;
        } else {
            length = 0x04;
        }
    }
    return length;
}

static void HdmiFill14Vsif(struct HdmiVsInfoFrame *vs, struct HdmiVideoAttr *videoAttr)
{
    struct HdmiVideo4kInfo *info = NULL;
    struct HdmiVs14VsifContent *vsif = &(vs->vsifContent.vsif);
    enum HdmiVic vic;
    uint32_t cnt;

    vsif->oui = HDMI_IEEE_OUI_1_4;
    vic = HdmiCommonGetVic(videoAttr->timing, videoAttr->aspect, false);
    if ((vic == HDMI_VIC_3840X2160P24_16_9 || vic == HDMI_VIC_3840X2160P25_16_9 ||
        vic == HDMI_VIC_3840X2160P30_16_9 || vic == HDMI_VIC_4096X2160P24_256_135) &&
        videoAttr->_3dStruct == HDMI_VS_VIDEO_3D_BUTT) {
        vsif->format = HDMI_VS_VIDEO_FORMAT_4K;
        for (cnt = 0; cnt <= HDMI_VIDEO_4K_CODES_MAX; cnt++) {
            info = HdmiCommonGetVideo4kInfo(cnt);
            if (info != NULL && info->timing == videoAttr->timing) {
                vsif->vic = info->_4kVic;
                break;
            }
        }
    } else if (videoAttr->_3dStruct < HDMI_VS_VIDEO_3D_BUTT) {  // common 3D
        vsif->format = HDMI_VS_VIDEO_FORMAT_3D;
        vsif->_3dStruct = videoAttr->_3dStruct;
    } else {
        vsif->format = HDMI_VS_VIDEO_FORMAT_NULL;
        vsif->_3dStruct = videoAttr->_3dStruct;
    }
}

static void HdmiFillVsInfoFrame(struct HdmiInfoFrame *frame, struct HdmiVideoAttr *videoAttr,
    bool dolbyEnable, bool hdrSupport)
{
    struct HdmiVsInfoFrame *vs = &(frame->vs);
    int32_t ret;

    ret = memset_s(vs, sizeof(struct HdmiVsInfoFrame), 0, sizeof(struct HdmiVsInfoFrame));
    if (ret != EOK) {
        HDF_LOGE("fill vsif, memset_s fail.");
        return;
    }
    vs->type = HDMI_INFOFRAME_PACKET_TYPE_VS;
    vs->verNum = HDMI_VSIF_VERSION;
    HdmiFill14Vsif(vs, videoAttr);
    vs->len = HdmiGetVsifLength(&(vs->vsifContent.vsif), dolbyEnable, hdrSupport);
    /* fill user vendor data */
    vs->vsifContent.userVsif.len = frame->userVsif.len;
    ret = memcpy_s(vs->vsifContent.userVsif.data, HDMI_VENDOR_USER_DATA_MAX_LEN,
        frame->userVsif.data, frame->userVsif.len);
    if (ret != EOK) {
        HDF_LOGE("fill vsif, memcpy_s fail.");
    }
}

int32_t HdmiVsInfoFrameSend(struct HdmiInfoFrame *frame, bool enable, bool dolbyEnable)
{
    struct HdmiCntlr *cntlr = NULL;
    union HdmiInfoFrameInfo infoFrame = {0};

    if (frame == NULL || frame->priv == NULL) {
        HDF_LOGE("HdmiVsInfoFrameSend: input param is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }
    cntlr = (struct HdmiCntlr *)frame->priv;
    if (cntlr->ops == NULL || cntlr->ops->infoFrameEnable == NULL) {
        HDF_LOGD("HdmiVsInfoFrameSend not support.");
        return HDF_ERR_NOT_SUPPORT;
    }

    if (enable == false) {
        cntlr->ops->infoFrameEnable(cntlr, HDMI_INFOFRAME_PACKET_TYPE_VS, false);
        return HDF_SUCCESS;
    }
    HdmiFillVsInfoFrame(frame, &(cntlr->attr.videoAttr), dolbyEnable, frame->hdrSupport);
    infoFrame.vs = frame->vs;
    return HdmiInfoFrameSend(frame, &infoFrame);
}

static void HdmiFillSpdInfoFrame(struct HdmiSpdInfoFrame *spd,
                                 const char *vendorName, const char *productName,
                                 enum HdmiSpdSdi sdi)
{
    uint32_t len, length;

    if (memset_s(spd, sizeof(struct HdmiSpdInfoFrame), 0, sizeof(struct HdmiSpdInfoFrame)) != EOK) {
        HDF_LOGE("fill spd infoFrame, memset_s fail.");
        return;
    }
    spd->type = HDMI_INFOFRAME_PACKET_TYPE_SPD;
    spd->len = HDMI_SPD_INFOFRAME_LEN;
    spd->verNum = HDMI_SPD_VERSION;
    spd->sdi = sdi;

    len = (uint32_t)strlen(vendorName);
    length = (uint32_t)sizeof(spd->vendorName);
    length = (length > len) ? len : length;
    if (memcpy_s(spd->vendorName, length, vendorName, length) != EOK) {
        HDF_LOGE("fill spd infoFrame vendor name, memcpy_s fail.");
    }

    len = (uint32_t)strlen(productName);
    length = (uint32_t)sizeof(spd->productDescription);
    length = (length > len) ? len : length;
    if (memcpy_s(spd->productDescription, length, productName, length) != EOK) {
        HDF_LOGE("fill spd infoFrame product name, memcpy_s fail.");
    }
}

int32_t HdmiSpdInfoFrameSend(struct HdmiInfoFrame *frame, bool enable,
    const char *vendorName, const char *productName, enum HdmiSpdSdi sdi)
{
    struct HdmiCntlr *cntlr = NULL;
    union HdmiInfoFrameInfo infoFrame = {0};

    if (frame == NULL || frame->priv == NULL) {
        HDF_LOGE("HdmiSpdInfoFrameSend: input param is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }
    cntlr = (struct HdmiCntlr *)frame->priv;
    if (cntlr->ops == NULL || cntlr->ops->infoFrameEnable == NULL) {
        HDF_LOGD("HdmiSpdInfoFrameSend not support.");
        return HDF_ERR_NOT_SUPPORT;
    }

    if (enable == false) {
        cntlr->ops->infoFrameEnable(cntlr, HDMI_INFOFRAME_PACKET_TYPE_SPD, false);
        return HDF_SUCCESS;
    }
    HdmiFillSpdInfoFrame(&(frame->spd), vendorName, productName, sdi);
    infoFrame.spd = frame->spd;
    return HdmiInfoFrameSend(frame, &infoFrame);
}

int32_t HdmiInfoFrameGetInfo(struct HdmiInfoFrame *frame, enum HdmiPacketType type,
    union HdmiInfoFrameInfo *infoFrame)
{
    if (frame == NULL || infoFrame == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    switch (type) {
        case HDMI_INFOFRAME_PACKET_TYPE_VS:
            infoFrame->vs = frame->vs;
            break;
        case HDMI_INFOFRAME_PACKET_TYPE_AVI:
            infoFrame->avi = frame->avi;
            break;
        case HDMI_INFOFRAME_PACKET_TYPE_AUDIO:
            infoFrame->audio = frame->audio;
            break;
        case HDMI_INFOFRAME_PACKET_TYPE_DRM:
            infoFrame->drm = frame->drm;
            break;
        default:
            HDF_LOGD("infoFrame %d not support get", type);
            return HDF_ERR_INVALID_PARAM;
    }
    return HDF_SUCCESS;
}

int32_t HdmiInfoFrameSetInfo(struct HdmiInfoFrame *frame, enum HdmiPacketType type,
    union HdmiInfoFrameInfo *infoFrame)
{
    if (frame == NULL || infoFrame == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    switch (type) {
        case HDMI_INFOFRAME_PACKET_TYPE_VS:
            frame->vs = infoFrame->vs;
            break;
        case HDMI_INFOFRAME_PACKET_TYPE_AVI:
            frame->avi = infoFrame->avi;
            break;
        case HDMI_INFOFRAME_PACKET_TYPE_AUDIO:
            frame->audio = infoFrame->audio;
            break;
        case HDMI_INFOFRAME_PACKET_TYPE_DRM:
            frame->drm = infoFrame->drm;
            break;
        default:
            HDF_LOGD("infoFrame %d not support set", type);
            return HDF_ERR_INVALID_PARAM;
    }
    return HDF_SUCCESS;
}
