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

#define HDF_LOG_TAG hdmi_edid_c

/*
 * Address locations 0x00 and 0x07 contain data values 0x00 and locations 0x01 through 0x06 contain 0xFF as data
 * values. CTA-861 requires this data. This header is used to determine the beginning of an EDID structure  in a Sink.
 */
uint8_t g_edidHeader[HDMI_EDID_BLOCK_HEADER_FIELD_LEN] = { 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00 };

int32_t HdmiEdidReset(struct HdmiEdid *edid)
{
    if (edid == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (memset_s(edid, sizeof(struct HdmiEdid), 0, sizeof(struct HdmiEdid)) != EOK) {
        HDF_LOGE("memset_s fail.");
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

int32_t HdmiEdidGetRaw(struct HdmiEdid *edid, uint8_t *raw, uint32_t len)
{
    uint32_t length;

    if (edid == NULL || raw == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    length = ((len < edid->rawLen) ? len : edid->rawLen);
    if (length == 0) {
        HDF_LOGE("edid data not get.");
        return (int32_t)length;
    }
    if (memcpy_s(raw, len, edid->raw, length) != EOK) {
        HDF_LOGE("memcpy_s fail.");
        return HDF_ERR_IO;
    }
    return (int32_t)length;
}

static int32_t HdmiEdidHeaderPhase(struct HdmiEdid *edid)
{
    struct HdmiEdidFirstBlockInfo *block = (struct HdmiEdidFirstBlockInfo *)edid->raw;
    uint32_t i;

    for (i = 0; i < HDMI_EDID_BLOCK_HEADER_FIELD_LEN; i++) {
        if (block->header[i] != g_edidHeader[i]) {
            HDF_LOGE("header[%d] = 0x%x, is error.", i, block->header[i]);
            return HDF_ERR_INVALID_PARAM;
        }
    }
    return HDF_SUCCESS;
}

static int32_t HdmiEdidBlockCheckSum(uint8_t *data)
{
    uint32_t i;
    uint32_t checkSum = 0;

    HDF_LOGD("check sum is 0x%x", data[HDMI_EDID_CHECKSUM_ADDR]);
    for (i = 0; i < HDMI_EDID_SINGLE_BLOCK_SIZE; i++) {
        checkSum += data[i];
    }
    if ((checkSum & HDMI_EDID_CHECK_SUM_MARK) == 0) {
        return HDF_SUCCESS;
    }
    HDF_LOGE("checksum = 0x%x, is invalid.", checkSum);
    return HDF_FAILURE;
}

static int32_t HdmiEdidVendorInfoPhase(struct HdmiEdid *edid)
{
    uint16_t data, i, tmpData;
    struct HdmiSinkDeviceCapability *sinkCap = &(edid->sinkCap);
    struct HdmiEdidFirstBlockInfo *block = (struct HdmiEdidFirstBlockInfo *)edid->raw;

    /* Manufacturer Name */
    data = (block->vendorName[UINT8_ARRAY_TElEMENT_0] <<
            HDMI_EDID_MANUFACRURER_NAME_SHIFT) |
            (block->vendorName[UINT8_ARRAY_TElEMENT_1]);
    for (i = 0; i < HDMI_EDID_MANUFACRURER_NAME_MAX_LEN - 1; i++) {
        tmpData = (data & (HDMI_EDID_MANUFACRURER_NAME_CHAR_MARK << (HDMI_EDID_MANUFACRURER_NAME_CHAR_SHIFT * i)));
        tmpData = (tmpData >> (HDMI_EDID_MANUFACRURER_NAME_CHAR_SHIFT * i));
        if (tmpData > HDMI_EDID_MANUFACRURER_NAME_LOW_INVALID &&
            tmpData < HDMI_EDID_MANUFACRURER_NAME_HIGH_INVALID) {
            sinkCap->vendorInfo.mfrName[HDMI_EDID_MANUFACRURER_NAME_EFFECTIVE_LEN - 1 - i] =
                (char)('A' + tmpData - 1);
        }
    }

    /* Product Code */
    data = (block->productCode[UINT8_ARRAY_TElEMENT_1] <<
            HDMI_EDID_PRODUCT_CODE_SHIFT) |
            (block->productCode[UINT8_ARRAY_TElEMENT_0]);
    sinkCap->vendorInfo.productCode = data;

    /* Serial Number */
    for (i = 0; i < HDMI_EDID_SERIAL_NUMBER_FIELD_LEN; i++) {
        sinkCap->vendorInfo.serialNumber |= (uint32_t)(block->serialNumber[i] <<
                                                       (uint16_t)(HDMI_EDID_SERIAL_NUMBER_SHIFT * i));
    }
    sinkCap->vendorInfo.week = block->week;
    sinkCap->vendorInfo.year = block->year + HDMI_EDID_YEAR_BASE;
    return HDF_SUCCESS;
}

static int32_t HdmiEdidVersionInfoPhase(struct HdmiEdid *edid)
{
    struct HdmiSinkDeviceCapability *sinkCap = &(edid->sinkCap);
    struct HdmiEdidFirstBlockInfo *block = (struct HdmiEdidFirstBlockInfo *)edid->raw;

    sinkCap->verInfo.version = block->version;
    sinkCap->verInfo.revision = block->revision;
    if (sinkCap->verInfo.version != HDMI_EDID_VERSION_NUM) {
        HDF_LOGW("edid version is %d", sinkCap->verInfo.version);
    }
    if (sinkCap->verInfo.revision != HDMI_EDID_REVISION_NUM) {
        HDF_LOGW("edid revision is %d", sinkCap->verInfo.revision);
    }
    return HDF_SUCCESS;
}

static int32_t HdmiEdidBasicDispPhase(struct HdmiEdid *edid)
{
    struct HdmiSinkDeviceCapability *sinkCap = &(edid->sinkCap);
    struct HdmiEdidFirstBlockInfo *block = (struct HdmiEdidFirstBlockInfo *)edid->raw;

    sinkCap->disp.width = block->width;
    sinkCap->disp.height = block->height;
    HDF_LOGI("width = %d, height = %d", block->width, block->height);
    return HDF_SUCCESS;
}

static int32_t HdmiEdidColorFeaturePhase(struct HdmiEdid *edid)
{
    struct HdmiSinkDeviceCapability *sinkCap = &(edid->sinkCap);
    struct HdmiEdidFirstBlockInfo *block = (struct HdmiEdidFirstBlockInfo *)edid->raw;

    /* red color phase */
    sinkCap->colorInfo.redX = (block->redX << HDMI_EDID_COLOR_HIGH_OFFSET) |
        ((block->redGreenLow >> HDMI_EDID_COLOR_RED_X_LOW_OFFSET) & HDMI_EDID_COLOR_LOW_BITS_MARK);
    sinkCap->colorInfo.redY = (block->redY << HDMI_EDID_COLOR_HIGH_OFFSET) |
        ((block->redGreenLow >> HDMI_EDID_COLOR_RED_Y_LOW_OFFSET) & HDMI_EDID_COLOR_LOW_BITS_MARK);
    /* green color phase */
    sinkCap->colorInfo.greenX = (block->greenX << HDMI_EDID_COLOR_HIGH_OFFSET) |
        ((block->redGreenLow >> HDMI_EDID_COLOR_GREEN_X_LOW_OFFSET) & HDMI_EDID_COLOR_LOW_BITS_MARK);
    sinkCap->colorInfo.greenY = (block->greenY << HDMI_EDID_COLOR_HIGH_OFFSET) |
        (block->redGreenLow & HDMI_EDID_COLOR_LOW_BITS_MARK);
    /* blue color phase */
    sinkCap->colorInfo.blueX = (block->blueX << HDMI_EDID_COLOR_HIGH_OFFSET) |
        ((block->blueWhiteLow >> HDMI_EDID_COLOR_BLUE_X_LOW_OFFSET) & HDMI_EDID_COLOR_LOW_BITS_MARK);
    sinkCap->colorInfo.blueY = (block->blueY << HDMI_EDID_COLOR_HIGH_OFFSET) |
        ((block->blueWhiteLow >> HDMI_EDID_COLOR_BLUE_Y_LOW_OFFSET) & HDMI_EDID_COLOR_LOW_BITS_MARK);
    /* white color phase */
    sinkCap->colorInfo.whiteX = (block->whiteX << HDMI_EDID_COLOR_HIGH_OFFSET) |
        ((block->blueWhiteLow >> HDMI_EDID_COLOR_WHITE_X_LOW_OFFSET) & HDMI_EDID_COLOR_LOW_BITS_MARK);
    sinkCap->colorInfo.whiteY = (block->whiteY << HDMI_EDID_COLOR_HIGH_OFFSET) |
        (block->blueWhiteLow & HDMI_EDID_COLOR_LOW_BITS_MARK);
    return HDF_SUCCESS;
}

static int32_t HdmiEdidEstablisedTimingPhase(struct HdmiEdid *edid)
{
    uint32_t i;
    uint32_t data = 0;
    struct HdmiSinkDeviceCapability *sinkCap = &(edid->sinkCap);
    struct HdmiEdidFirstBlockInfo *block = (struct HdmiEdidFirstBlockInfo *)edid->raw;

    for (i = 0; i < HDMI_EDID_ESTABLISHED_TIMINGS_FIELD_LEN; i++) {
        data |= (uint32_t)(block->estTimings[i] << (uint32_t)(i * HDMI_ONE_BYTE_SHIFT));
    }

    /*
     * The established timing block is a field of one-bit flags, which are used to inducate support for established
     * VESA and other common timing in a very compact form. A bit set to "1" indicate support for that timing.
     */
    for (i = 0; i < HDMI_EDID_ESTABLISHED_TIMING_BUTT; i++) {
        if ((data & (1 << i)) > 0) {
            sinkCap->establishedTimingsInfo.estTimings[i] = i;
            sinkCap->establishedTimingsInfo.estTimingsNum++;
        }
    }
    return HDF_SUCCESS;
}

static uint32_t HdmiEdidGetStandardTimingVertPixel(uint32_t aspectRatio, uint32_t horizPixel)
{
    uint32_t vertPixel = 0;

    switch (aspectRatio) {
        case HDMI_EDID_STANDARD_TIMING_ASPECT_RATE_16_10:
            vertPixel = horizPixel * VIDEO_WIDTH_10 / VIDEO_LENGTH_16;
            break;
        case HDMI_EDID_STANDARD_TIMING_ASPECT_RATE_5_4:
            vertPixel = horizPixel * VIDEO_WIDTH_4 / VIDEO_LENGTH_5;
            break;
        case HDMI_EDID_STANDARD_TIMING_ASPECT_RATE_4_3:
            vertPixel = horizPixel * VIDEO_WIDTH_3 / VIDEO_LENGTH_4;
            break;
        case HDMI_EDID_STANDARD_TIMING_ASPECT_RATE_16_9:
            vertPixel = horizPixel * VIDEO_WIDTH_9 / VIDEO_LENGTH_16;
            break;
        default:
            HDF_LOGE("aspectRatio %d is invalid.", aspectRatio);
            break;
    }
    return vertPixel;
}

static int32_t HdmiEdidStandardTimingPhase(struct HdmiEdid *edid)
{
    uint32_t i, aspectRatio;
    struct HdmiSinkDeviceCapability *sinkCap = &(edid->sinkCap);
    struct HdmiEdidFirstBlockInfo *block = (struct HdmiEdidFirstBlockInfo *)edid->raw;
    uint8_t *data = block->stdTiming;

    /*
     * A 2-byte timing identifier identifies each timing mode.
     * Unused fields in this section shall be set to 01h, 01h.
     */
    for (i = 0; i < HDMI_EDID_STANDARD_TIMING_COUNT;  i++) {
        if (data[UINT8_ARRAY_TElEMENT_0] != HDMI_EDID_STANDARD_TIMING_UNUSED_FLAG ||
            data[UINT8_ARRAY_TElEMENT_1] != HDMI_EDID_STANDARD_TIMING_UNUSED_FLAG) {
            sinkCap->stdTimingsInfo[i].horizPixel = (data[UINT8_ARRAY_TElEMENT_0] +
                                                     HDMI_EDID_STANDARD_TIMING_HORIZ_PIXEL_BASE) *
                                                     HDMI_EDID_STANDARD_TIMING_HORIZ_PIXEL_FACTOR;
            sinkCap->stdTimingsInfo[i].refreshRate = (data[UINT8_ARRAY_TElEMENT_1] &
                                                      HDMI_EDID_STANDARD_TIMING_REFRESH_RATE_MARK) +
                                                      HDMI_EDID_STANDARD_TIMING_REFRESH_RATE_BASE;
            aspectRatio =  (data[UINT8_ARRAY_TElEMENT_1] & HDMI_EDID_STANDARD_TIMING_ASPECT_RATE_MARK) >>
                HDMI_EDID_STANDARD_TIMING_ASPECT_RATE_SHIFT;
            sinkCap->stdTimingsInfo[i].vertPixel =
                HdmiEdidGetStandardTimingVertPixel(aspectRatio, sinkCap->stdTimingsInfo[i].horizPixel);
        }
        data += HDMI_EDID_PER_STANDARD_TIMING_BYTE_NUM;
    }
    return HDF_SUCCESS;
}

static void HdmiEdidMonitorDescriptorPhase(struct HdmiSinkDeviceCapability *sinkCap, uint8_t *data, uint32_t len)
{
    struct HdmiEdidMonitorBlockInfo *block = (struct HdmiEdidMonitorBlockInfo *)data;

    if (block->dataTag != HDMI_EDID_MONITOR_DATA_NAME) {
        return;
    }
    if (memcpy_s(sinkCap->sinkDeviceName, HDMI_EDID_MAX_SINK_NAME_COUNT,
        block->data, HDMI_EDID_MONITOR_DATA_FIELD_LEN) != EOK) {
        HDF_LOGE("memcpy_s sink name fail");
    }
}

static void HdmiEdidDigitalSyncSignal(struct HdmiEdidPreferredTimingInfo *perTiming, uint8_t flags)
{
    /* bit3/bit2/bit1 */
    switch ((flags & HDMI_EDID_DETAILED_TIMING_SYNC_SIGNAL_TYPE_MARK) >> 1) {
        case HDMI_EDID_DETAILED_TIMING_SYNC_DCS_WS_0:
        case HDMI_EDID_DETAILED_TIMING_SYNC_DCS_WS_1:
        case HDMI_EDID_DETAILED_TIMING_SYNC_DCS_DS_2:
        case HDMI_EDID_DETAILED_TIMING_SYNC_DCS_DS_3:
            perTiming->ihs = false;
            perTiming->ivs = false;
            break;
        case HDMI_EDID_DETAILED_TIMING_SYNC_DSS_VN_HN_4:
            perTiming->ihs = false;
            perTiming->ivs = false;
            break;
        case HDMI_EDID_DETAILED_TIMING_SYNC_DSS_VN_HP_5:
            perTiming->ihs = true;
            perTiming->ivs = false;
            break;
        case HDMI_EDID_DETAILED_TIMING_SYNC_DSS_VP_HN_6:
            perTiming->ihs = false;
            perTiming->ivs = true;
            break;
        case HDMI_EDID_DETAILED_TIMING_SYNC_DSS_VP_HP_7:
            perTiming->ihs = true;
            perTiming->ivs = true;
            break;
        default:
            break;
    }
}

static void HdmiEdidDetailedTimingDescriptorPhase(struct HdmiSinkDeviceCapability *cap, uint8_t *data, uint32_t len)
{
    struct HdmiEdidDetailedTimingBlockInfo *block = (struct HdmiEdidDetailedTimingBlockInfo *)data;
    struct HdmiEdidPreferredTimingInfo *perTiming = NULL;
    uint32_t pixelClock;

    if (cap->preTimingCnt >= HDMI_EDID_MAX_DETAILED_TIMING_COUNT) {
        HDF_LOGE("preTimingCnt reach the maximum");
        return;
    }

    perTiming = &cap->preTimingInfo[cap->preTimingCnt];
    pixelClock = (block->pixelClk[UINT8_ARRAY_TElEMENT_1] <<
                  HDMI_ONE_BYTE_SHIFT) |
                  (block->pixelClk[UINT8_ARRAY_TElEMENT_0]);
    if (pixelClock == 0) {
        HDF_LOGD(" pixel clock is 0. preTimingCnt = %d", cap->preTimingCnt);
        return;
    }

    perTiming->pixelClk = pixelClock * HDMI_EDID_DETAILED_TIMING_PIXEL_CLK_KHZ_FACTOR;
    perTiming->hActive = (block->hActive) |
        ((block->hActiveBlanking & HDMI_EDID_DETAILED_TIMING_UPPER_4BITS_MARK) << HDMI_NIBBLE_SHIFT);
    perTiming->hBackBlank = (block->hBlanking) |
        ((block->hActiveBlanking & HDMI_EDID_DETAILED_TIMING_LOWER_4BITS_MARK) << HDMI_ONE_BYTE_SHIFT);
    perTiming->hFrontBlank = (block->hSyncOffset) |
        ((block->hsOffsetVsOffset & HDMI_EDID_DETAILED_TIMING_HS_OFFSET_MARK) << HDMI_2_BITS_SHIFT);
    perTiming->hSyncPluseWidth = (block->hSyncPulseWidth) |
        ((block->hsOffsetVsOffset & HDMI_EDID_DETAILED_TIMING_HS_PULSE_WIDTH_MARK) << HDMI_NIBBLE_SHIFT);

    perTiming->vActive = (block->vActive) |
        ((block->vActiveBlanking & HDMI_EDID_DETAILED_TIMING_UPPER_4BITS_MARK) << HDMI_NIBBLE_SHIFT);
    perTiming->vBackBlank = (block->vBlanking) |
        ((block->vActiveBlanking & HDMI_EDID_DETAILED_TIMING_LOWER_4BITS_MARK) << HDMI_ONE_BYTE_SHIFT);
    perTiming->vFrontBlank =
        ((block->hsOffsetVsOffset & HDMI_EDID_DETAILED_TIMING_VS_OFFSET_MARK) << HDMI_2_BITS_SHIFT) |
        ((block->vsOffesetPulseWidth & HDMI_EDID_DETAILED_TIMING_UPPER_4BITS_MARK) >> HDMI_NIBBLE_SHIFT);
    perTiming->vSyncPluseWidth =
        ((block->hsOffsetVsOffset & HDMI_EDID_DETAILED_TIMING_VS_PULSE_WIDTH_MARK) << HDMI_NIBBLE_SHIFT) |
        (block->vsOffesetPulseWidth & HDMI_EDID_DETAILED_TIMING_LOWER_4BITS_MARK);

    perTiming->imageWidth = (block->hImageSize) |
        ((block->hvImageSize & HDMI_EDID_DETAILED_TIMING_UPPER_4BITS_MARK) << HDMI_NIBBLE_SHIFT);
    perTiming->imageHeight = (block->vImageSize) |
        ((block->hvImageSize & HDMI_EDID_DETAILED_TIMING_LOWER_4BITS_MARK) << HDMI_ONE_BYTE_SHIFT);
    perTiming->interlaceFlag = (block->flags & HDMI_BIT7_MARK) ? true : false;
    /* Digital composite/separate */
    if ((block->flags & HDMI_BIT4_MARK) != 0) {
        HdmiEdidDigitalSyncSignal(perTiming, block->flags);
    }
    cap->preTimingCnt++;
}

static void HdmiEdidDetailedTiming(struct HdmiSinkDeviceCapability *sinkCap, uint8_t *data, uint32_t len)
{
    /*
     * Monitor Descriptor flag: data0/data1/data2 is 0x00.
     */
    if (data[UINT8_ARRAY_TElEMENT_0] == 0x00 &&
        data[UINT8_ARRAY_TElEMENT_1] == 0x00 &&
        data[UINT8_ARRAY_TElEMENT_2] == 0x00) {
        HdmiEdidMonitorDescriptorPhase(sinkCap, data, len);
        return;
    }

    /*
     * Those 18-byte blocks not used for Monitor Descriptors shall be used for detailed timings.
     */
    HdmiEdidDetailedTimingDescriptorPhase(sinkCap, data, len);
}

static int32_t HdmiEdidDetailedTimingPhase(struct HdmiEdid *edid)
{
    struct HdmiSinkDeviceCapability *sinkCap = &(edid->sinkCap);
    struct HdmiEdidFirstBlockInfo *block = (struct HdmiEdidFirstBlockInfo *)edid->raw;

    /*
     * The detailed timing section is divided into four descriptor blocks, which are 18 bytes each.
     * The first descriptor block shall be used to indicate the display's preferred timing mode.
     * A Monitor Name Descriptor must be provided.
     */
    HdmiEdidDetailedTiming(sinkCap, block->detailedTiming1, HDMI_EDID_DETAILED_TIMING_DESCRIPTOR_FIELD_LEN);
    HdmiEdidDetailedTiming(sinkCap, block->detailedTiming2, HDMI_EDID_DETAILED_TIMING_DESCRIPTOR_FIELD_LEN);
    HdmiEdidDetailedTiming(sinkCap, block->detailedTiming3, HDMI_EDID_DETAILED_TIMING_DESCRIPTOR_FIELD_LEN);
    HdmiEdidDetailedTiming(sinkCap, block->detailedTiming4, HDMI_EDID_DETAILED_TIMING_DESCRIPTOR_FIELD_LEN);
    return HDF_SUCCESS;
}

static int32_t HdmiEdidExtBlockNumPhase(struct HdmiEdid *edid)
{
    struct HdmiSinkDeviceCapability *sinkCap = &(edid->sinkCap);

    sinkCap->extBlockNum = edid->raw[HDMI_EDID_EXTENSION_BLOCK_ADDR];
    HDF_LOGD("edid extBlockNum = %d.", sinkCap->extBlockNum);
    if (sinkCap->extBlockNum > (HDMI_EDID_MAX_BLOCK_NUM - 1)) {
        HDF_LOGW("ext block number %d is invallid.", sinkCap->extBlockNum);
        sinkCap->extBlockNum = HDMI_EDID_MAX_BLOCK_NUM - 1;
    }
    return HDF_SUCCESS;
}

static int32_t HdmiEdidFirstBlockPhase(struct HdmiEdid *edid)
{
    uint32_t i;
    uint32_t len;
    int32_t ret;

    HdmiEdidPhaseFunc func[] = {
        HdmiEdidHeaderPhase,
        HdmiEdidVendorInfoPhase,
        HdmiEdidVersionInfoPhase,
        HdmiEdidBasicDispPhase,
        HdmiEdidColorFeaturePhase,
        HdmiEdidEstablisedTimingPhase,
        HdmiEdidStandardTimingPhase,
        HdmiEdidDetailedTimingPhase,
        HdmiEdidExtBlockNumPhase
    };

    ret = HdmiEdidBlockCheckSum(edid->raw);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("edid block0 check sum fail.");
        return ret;
    }

    len = sizeof(func) / sizeof(func[UINT8_ARRAY_TElEMENT_0]);
    for (i = 0; i < len; i++) {
        if (func[i] == NULL) {
            continue;
        }
        ret = (func[i])(edid);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("func[%u] exe fail.", i);
            return ret;
        }
    }
    return  HDF_SUCCESS;
}

static void HdmiEdidExtAdbSampleRatePhase(struct HdmiEdidAudioInfo *audio, uint8_t data, uint8_t formatCode)
{
    if ((data & HDMI_BIT0_MARK) > 0 &&
        audio->sampleRateNum < HDMI_EDID_EXTENSION_AUDIO_MAX_SAMPLE_RATE_NUM) {
        audio->sampleRate[audio->sampleRateNum] = HDMI_SAMPLE_RATE_32K;
        audio->sampleRateNum++;
    }
    if ((data & HDMI_BIT1_MARK) > 0 &&
        audio->sampleRateNum < HDMI_EDID_EXTENSION_AUDIO_MAX_SAMPLE_RATE_NUM) {
        audio->sampleRate[audio->sampleRateNum] = HDMI_SAMPLE_RATE_44K;
        audio->sampleRateNum++;
    }
    if ((data & HDMI_BIT2_MARK) > 0 &&
        audio->sampleRateNum < HDMI_EDID_EXTENSION_AUDIO_MAX_SAMPLE_RATE_NUM) {
        audio->sampleRate[audio->sampleRateNum] = HDMI_SAMPLE_RATE_48K;
        audio->sampleRateNum++;
    }
    if ((data & HDMI_BIT3_MARK) > 0 &&
        audio->sampleRateNum < HDMI_EDID_EXTENSION_AUDIO_MAX_SAMPLE_RATE_NUM) {
        audio->sampleRate[audio->sampleRateNum] = HDMI_SAMPLE_RATE_88K;
        audio->sampleRateNum++;
    }
    if ((data & HDMI_BIT4_MARK) > 0 &&
        audio->sampleRateNum < HDMI_EDID_EXTENSION_AUDIO_MAX_SAMPLE_RATE_NUM) {
        audio->sampleRate[audio->sampleRateNum] = HDMI_SAMPLE_RATE_96K;
        audio->sampleRateNum++;
    }

    if (formatCode >= HDMI_AUDIO_CODING_TYPE_LPCM && formatCode <= HDMI_AUDIO_CODING_TYPE_WMA_PRO) {
        if ((data & HDMI_BIT5_MARK) > 0 &&
            audio->sampleRateNum < HDMI_EDID_EXTENSION_AUDIO_MAX_SAMPLE_RATE_NUM) {
            audio->sampleRate[audio->sampleRateNum] = HDMI_SAMPLE_RATE_176K;
            audio->sampleRateNum++;
        }
        if ((data & HDMI_BIT6_MARK) > 0 &&
            audio->sampleRateNum < HDMI_EDID_EXTENSION_AUDIO_MAX_SAMPLE_RATE_NUM) {
            audio->sampleRate[audio->sampleRateNum] = HDMI_SAMPLE_RATE_192K;
            audio->sampleRateNum++;
        }
    }
}

static void HdmiEdidExtAdbDepthAndMaxRatePhase(struct HdmiEdidAudioInfo *audio, uint8_t data, uint8_t formatCode)
{
    /*
     * Audio Format Code is 1, bit[2:0] bit depth;
     * Audio Format Codes 2 to 8, Maximum bit rate divided by 8 kHz.
     */
    if (formatCode == HDMI_AUDIO_CODING_TYPE_LPCM) {
        if ((data & HDMI_BIT0_MARK) > 0 &&
            audio->bitDepthNum < HDMI_EDID_EXTENSION_AUDIO_MAX_BIT_DEPTH_NUM) {
            audio->bitDepth[audio->bitDepthNum] = HDMI_ADIO_BIT_DEPTH_16;
            audio->bitDepthNum++;
        }
        if ((data & HDMI_BIT1_MARK) > 0 &&
            audio->bitDepthNum < HDMI_EDID_EXTENSION_AUDIO_MAX_BIT_DEPTH_NUM) {
            audio->bitDepth[audio->bitDepthNum] = HDMI_ADIO_BIT_DEPTH_20;
            audio->bitDepthNum++;
        }
        if ((data & HDMI_BIT2_MARK) > 0 &&
            audio->bitDepthNum < HDMI_EDID_EXTENSION_AUDIO_MAX_BIT_DEPTH_NUM) {
            audio->bitDepth[audio->bitDepthNum] = HDMI_ADIO_BIT_DEPTH_24;
            audio->bitDepthNum++;
        }
    } else if (formatCode >= HDMI_AUDIO_CODING_TYPE_AC3 && formatCode <= HDMI_AUDIO_CODING_TYPE_ATRAC) {
        audio->maxBitRate = data * HDMI_EDID_EXTENSION_AUDIO_BIT_RATE_FACTOR;
    } else {
        HDF_LOGI("formatCode %hhu reserved or not care", formatCode);
    }
}

static int32_t HdmiEdidExtAudioDataBlockPhase(struct HdmiSinkDeviceCapability *sinkCap, uint8_t *data, uint8_t len)
{
    uint8_t i, formatCode;

    /*
     * Each Short Audio Descriptor is 3-bytes long. There can be up to 31 bytes following any tag,
     * therefore there may be up to 10 Short Audio Descriptors in the Audio Data Block.
     */
    if (len > (HDMI_EDID_EXTENSION_SHORT_AUDIO_DESCRIPTOR_LEN * HDMI_EDID_EXTENSION_MAX_SHORT_AUDIO_DESCRIPTOR_NUM)) {
        HDF_LOGE("Audio Data Block: len %hhu is invalid", len);
        return HDF_ERR_INVALID_PARAM;
    }

    for (i = 0; i < (len / HDMI_EDID_EXTENSION_SHORT_AUDIO_DESCRIPTOR_LEN); i++) {
        if (sinkCap->audioInfoCnt >= HDMI_EDID_EXTENSION_AUDIO_CAP_COUNT) {
            HDF_LOGE("ADB: info cnt reach the maximum");
            break;
        }
        data += (i * HDMI_EDID_EXTENSION_SHORT_AUDIO_DESCRIPTOR_LEN);
        /* byte0: bit[6:3] Audio Format Code; bit[2:0] Max Number of channels - 1 */
        formatCode = (data[UINT8_ARRAY_TElEMENT_0] & HDMI_EDID_EXTENSION_AUDIO_FORMAT_CODE_MARK) >>
            HDMI_EDID_EXTENSION_AUDIO_FORMAT_CODE_SHIFT;
        sinkCap->audioInfo[sinkCap->audioInfoCnt].formatCode = (enum HdmiAudioCodingType)formatCode;
        sinkCap->audioInfo[sinkCap->audioInfoCnt].channels =
            (data[UINT8_ARRAY_TElEMENT_0] & HDMI_EDID_EXTENSION_AUDIO_MAX_CHANNEL_MARK) + 1;
        /* byte1: Sample Rate */
        HdmiEdidExtAdbSampleRatePhase(&(sinkCap->audioInfo[sinkCap->audioInfoCnt]),
                                      data[UINT8_ARRAY_TElEMENT_1],
                                      formatCode);
        /* byte2: bit depth or maximum bit rate */
        HdmiEdidExtAdbDepthAndMaxRatePhase(&(sinkCap->audioInfo[sinkCap->audioInfoCnt]),
                                           data[UINT8_ARRAY_TElEMENT_2],
                                           formatCode);
        sinkCap->audioInfoCnt++;
    }
    return HDF_SUCCESS;
}

static int32_t HdmiEdidExtVideoDataBlockPhase(struct HdmiSinkDeviceCapability *sinkCap, uint8_t *data, uint8_t len)
{
    uint8_t i;
    uint32_t vicAll, vicLower;
    uint32_t implicitNative = 0;

    for (i = 0; i < len; i++) {
        if (sinkCap->videoInfo.vicNum >= HDMI_EDID_EXTENSION_MAX_VIC_COUNT) {
            HDF_LOGD("VDB: vicNum reach the maximum");
            break;
        }
        vicAll = data[i];
        vicLower = (vicAll & HDMI_EDID_EXTENSION_VIC_LOWER7_MARK);
        if (vicAll == 0) {
            continue;
        }
        /*
         * For VICs 1 through 64, the lower 7-bits are an index associated with the Video Format supported.
         * The most significant bit declares whether the format is a Native Video Format of the
         * display (native =1, not native = 0). Typically, there is a single SVD, with its native bit set.
         */
        if ((vicAll & HDMI_BIT7_MARK) > 0 && vicLower < HDMI_EDID_EXTENSION_VIC_NATIVE_MAX) {
            if (sinkCap->videoInfo.nativeFormat == 0) {
                sinkCap->videoInfo.nativeFormat = vicLower;
            }
        }
        /* set the first valid vic as implicit native */
        if (implicitNative == 0) {
            implicitNative = vicAll;
        }
        if ((vicAll & HDMI_BIT7_MARK) > 0 && vicLower < HDMI_EDID_EXTENSION_VIC_NATIVE_MAX) {
            sinkCap->videoInfo.vic[sinkCap->videoInfo.vicNum] = vicLower;
        } else {
            sinkCap->videoInfo.vic[sinkCap->videoInfo.vicNum] = vicAll;
        }
        sinkCap->videoInfo.vicNum++;
    }

    if (sinkCap->videoInfo.nativeFormat == 0) {
        sinkCap->videoInfo.nativeFormat = implicitNative;
    }
    return HDF_SUCCESS;
}

static void HdmiEdidVsdbCecPhyAddrPhase(struct HdmiSinkDeviceCapability *sinkCap, uint8_t *data, uint8_t len)
{
    if (len >= HDMI_EDID_VSDB_MIN_LEN_FOR_CEC_PHY_ADDR) {
        sinkCap->vsdbInfo.cecAddr.phyAddrA = (data[UINT8_ARRAY_TElEMENT_3] & HDMI_UPPER_NIBBLE_MARK) >>
                                              HDMI_NIBBLE_SHIFT;
        sinkCap->vsdbInfo.cecAddr.phyAddrB = (data[UINT8_ARRAY_TElEMENT_3] & HDMI_LOWER_NIBBLE_MARK);
        sinkCap->vsdbInfo.cecAddr.phyAddrC = (data[UINT8_ARRAY_TElEMENT_4] & HDMI_UPPER_NIBBLE_MARK) >>
                                              HDMI_NIBBLE_SHIFT;
        sinkCap->vsdbInfo.cecAddr.phyAddrD = (data[UINT8_ARRAY_TElEMENT_4] & HDMI_LOWER_NIBBLE_MARK);
        sinkCap->vsdbInfo.cecAddr.addrValid =
            (sinkCap->vsdbInfo.cecAddr.phyAddrA != HDMI_EDID_EXTENSION_VSDB_CEC_INVALID_ADDR) &&
            (sinkCap->vsdbInfo.cecAddr.phyAddrB != HDMI_EDID_EXTENSION_VSDB_CEC_INVALID_ADDR) &&
            (sinkCap->vsdbInfo.cecAddr.phyAddrC != HDMI_EDID_EXTENSION_VSDB_CEC_INVALID_ADDR) &&
            (sinkCap->vsdbInfo.cecAddr.phyAddrD != HDMI_EDID_EXTENSION_VSDB_CEC_INVALID_ADDR);
    }
}

static void HdmiEdidVsdbColorDepthPhase(struct HdmiSinkDeviceCapability *sinkCap, uint8_t *data, uint8_t len)
{
    if (len >= HDMI_EDID_VSDB_MIN_LEN_FOR_COLOR_DEPTH) {
        sinkCap->vsdbInfo.supportAi = (data[UINT8_ARRAY_TElEMENT_5] & HDMI_BIT7_MARK) ? true : false;
        sinkCap->vsdbInfo.supportDviDual = (data[UINT8_ARRAY_TElEMENT_5] & HDMI_BIT0_MARK) ? true : false;
        sinkCap->vsdbInfo.deepColor.dcY444 = (data[UINT8_ARRAY_TElEMENT_5] & HDMI_BIT3_MARK) ? true : false;
        sinkCap->vsdbInfo.deepColor.dc30bit = (data[UINT8_ARRAY_TElEMENT_5] & HDMI_BIT4_MARK) ? true : false;
        sinkCap->vsdbInfo.deepColor.dc36bit = (data[UINT8_ARRAY_TElEMENT_5] & HDMI_BIT5_MARK) ? true : false;
        sinkCap->vsdbInfo.deepColor.dc48bit = (data[UINT8_ARRAY_TElEMENT_5] & HDMI_BIT6_MARK) ? true : false;
    }
}

static void HdmiEdidVsdbMaxTmdsClockPhase(struct HdmiSinkDeviceCapability *sinkCap, uint8_t *data, uint8_t len)
{
    /*
     * This field shall be set cprrectly and non-zero if the sink support TMDS clock frequencies above 165MHz or
     * supports ant Deep Color mode or supports DVI dual-link. A value of zeor means that no clock rate is indicated.
     */
    if (len >= HDMI_EDID_VSDB_MIN_LEN_FOR_MAX_TMDS_CLOCK) {
        sinkCap->maxTmdsClk = data[UINT8_ARRAY_TElEMENT_6] * HDMI_EDID_EXTENSION_TMDS_FACTOR;
        sinkCap->supportHdmi20 = (sinkCap->maxTmdsClk > HDMI_EDID_EXTENSION_MAX_HDMI14_TMDS_RATE) ? true : false;
    }
}

static void HdmiEdidVsdbSinkPresentPhase(struct HdmiSinkDeviceCapability *sinkCap, uint8_t *data, uint8_t len)
{
    if (len >= HDMI_EDID_VSDB_MIN_LEN_FOR_SINK_PRESENT) {
        sinkCap->vsdbInfo.latencyFieldsPresent = (data[UINT8_ARRAY_TElEMENT_7] & HDMI_BIT7_MARK) ? true : false;
        sinkCap->vsdbInfo.iLatencyFieldsPresent = (data[UINT8_ARRAY_TElEMENT_7] & HDMI_BIT6_MARK) ? true : false;
        sinkCap->vsdbInfo.hdmiVideoPresent = (data[UINT8_ARRAY_TElEMENT_7] & HDMI_BIT5_MARK) ? true : false;
    }
}

static void HdmiEdidVsdbSinkLatencyPhase(struct HdmiSinkDeviceCapability *sinkCap, uint8_t *data, uint8_t len)
{
    if (sinkCap->vsdbInfo.latencyFieldsPresent == true) {
        if (len >= HDMI_EDID_VSDB_MIN_LEN_FOR_VIDEO_LATENCY) {
            sinkCap->vsdbInfo.videoLatency = data[UINT8_ARRAY_TElEMENT_8];
        }
        if (len >= HDMI_EDID_VSDB_MIN_LEN_FOR_AUDIO_LATENCY) {
            sinkCap->vsdbInfo.audioLatency = data[UINT8_ARRAY_TElEMENT_9];
        }
    }
    if (sinkCap->vsdbInfo.iLatencyFieldsPresent == true) {
        if (len >= HDMI_EDID_VSDB_MIN_LEN_FOR_INTERLACED_VIDEO_LATENCY) {
            sinkCap->vsdbInfo.interlacedVideoLatency = data[UINT8_ARRAY_TElEMENT_10];
        }
        if (len >= HDMI_EDID_VSDB_MIN_LEN_FOR_INTERLACED_AUDIO_LATENCY) {
            sinkCap->vsdbInfo.interlacedAudioLatency = data[UINT8_ARRAY_TElEMENT_11];
        }
    }
}

static void HdmiEdidVsdbVicInfoPhase(struct HdmiSinkDeviceCapability *sinkCap,
    uint8_t *data, uint8_t vicLen, uint8_t *offset)
{
    uint8_t i, index;
    /* see hdmi spec 1.4 table 8-13. */
    uint32_t hdmi4kVic[] = {
        0,
        HDMI_VIC_3840X2160P30_16_9,
        HDMI_VIC_3840X2160P25_16_9,
        HDMI_VIC_3840X2160P24_16_9,
        HDMI_VIC_4096X2160P24_256_135
    };

    for (i = 0; i < vicLen; i++) {
        if (sinkCap->videoInfo.vicNum >= HDMI_EDID_EXTENSION_MAX_VIC_COUNT) {
            break;
        }
        index = data[(*offset)];
        if (index != 0 && index < sizeof(hdmi4kVic) / sizeof(hdmi4kVic[UINT8_ARRAY_TElEMENT_0])) {
            sinkCap->videoInfo.vic[sinkCap->videoInfo.vicNum] = hdmi4kVic[index];
            sinkCap->videoInfo.vicNum++;
        }
        (*offset)++;
    }
}

static void HdmiEdidVsdb3dStructureInfoPhase(struct HdmiSinkDeviceCapability *sinkCap,
    uint8_t *data, uint8_t len, uint8_t *offset)
{
    /*
     * If 3D_Multi_present is 1 or 2, 3D_Struct_ALL_15...0 is present and assigns 3D formats to
     * all of the VICs listed in the first 16 entries in the EDID.
     */
    if (((sinkCap->vsdbInfo._3dMultiPresent) & HDMI_EDID_EXTENSION_VSDB_3D_STR_INVALID_MARK) == 0) {
        return;
    }
    /* see hdmi spec 1.4 table H-8. */
    if ((*offset) <= len) {
        /*
         * bit[15:9]: reserved.
         * bit8: sinks support "Side-by-side(half) with all sub-sampling methods" 3D formats.
         */
        sinkCap->vsdbInfo.support3dType[HDMI_VS_VIDEO_3D_SIDE_BY_SIDE_HALF] =
            (data[(*offset)] & HDMI_BIT0_MARK) ? true : false;
        (*offset)++;
    }

    if ((*offset) <= len) {
        sinkCap->vsdbInfo.support3dType[HDMI_VS_VIDEO_3D_FRAME_PACKING] =
            (data[(*offset)] & HDMI_BIT0_MARK) ? true : false;
        sinkCap->vsdbInfo.support3dType[HDMI_VS_VIDEO_3D_FIELD_ALTERNATIVE] =
            (data[(*offset)] & HDMI_BIT1_MARK) ? true : false;
        sinkCap->vsdbInfo.support3dType[HDMI_VS_VIDEO_3D_LINE_ALTERNATIVE] =
            (data[(*offset)] & HDMI_BIT2_MARK) ? true : false;
        sinkCap->vsdbInfo.support3dType[HDMI_VS_VIDEO_3D_SIDE_BY_SIDE_FULL] =
            (data[(*offset)] & HDMI_BIT3_MARK) ? true : false;
        sinkCap->vsdbInfo.support3dType[HDMI_VS_VIDEO_3D_L_DEPTH] =
            (data[(*offset)] & HDMI_BIT4_MARK) ? true : false;
        sinkCap->vsdbInfo.support3dType[HDMI_VS_VIDEO_3D_L_DEPTH_GFX_GFX_DEPTH] =
            (data[(*offset)] & HDMI_BIT5_MARK) ? true : false;
        sinkCap->vsdbInfo.support3dType[HDMI_VS_VIDEO_3D_TOP_AND_BOTTOM] =
            (data[(*offset)] & HDMI_BIT6_MARK) ? true : false;
        (*offset)++;
    }
}

static void HdmiEdidVsdbVicAnd3dInfoPhase(struct HdmiSinkDeviceCapability *sinkCap, uint8_t *data, uint8_t len)
{
    uint8_t hdmiVicLen = 0;
    uint8_t hdmi3dLen = 0;
    uint8_t offset;

    if (len < HDMI_EDID_VSDB_MIN_LEN_FOR_3D_PRESENT_INFO) {
        HDF_LOGD("vsdb: these is no vic/3d field.");
        return;
    }
    sinkCap->vsdbInfo._3dPresent = (data[UINT8_ARRAY_TElEMENT_12] & HDMI_BIT7_MARK) ? true : false;
    sinkCap->vsdbInfo._3dMultiPresent = (data[UINT8_ARRAY_TElEMENT_12] &
                                        HDMI_EDID_EXTENSION_VSDB_3D_MULTI_PRESENT_MARK) >>
                                        HDMI_EDID_EXTENSION_VSDB_3D_MULTI_PRESENT_SHIFT;

    if (len >= HDMI_EDID_VSDB_MIN_LEN_FOR_VIC_INFO) {
        hdmiVicLen = (data[UINT8_ARRAY_TElEMENT_13] &
                     HDMI_EDID_EXTENSION_VSDB_VIC_LEN_MARK) >>
                     HDMI_EDID_EXTENSION_VSDB_VIC_LEN_SHIFT;
        hdmi3dLen = (data[UINT8_ARRAY_TElEMENT_13] & HDMI_EDID_EXTENSION_VSDB_3D_LEN_MARK);
    }

    /* byte14~byteN: Vic info/3D info */
    offset = HDMI_EDID_VSDB_MIN_LEN_FOR_VIC_INFO;
    if (hdmiVicLen > 0 && (hdmiVicLen + offset + 1) <= len) {
        HdmiEdidVsdbVicInfoPhase(sinkCap, data, hdmiVicLen, &offset);
    }

    if (hdmi3dLen > 0 &&
        sinkCap->vsdbInfo._3dPresent == true &&
        (hdmi3dLen + offset + 1) <= len) {
        HdmiEdidVsdb3dStructureInfoPhase(sinkCap, data, len, &offset);
    }
}

static int32_t HdmiEdidVsdbPhase(struct HdmiSinkDeviceCapability *sinkCap, uint8_t *data, uint8_t len)
{
    /* byte3 byte4: cec addr */
    if (len < HDMI_EDID_VSDB_MIN_LEN_FOR_CEC_PHY_ADDR) {
        HDF_LOGD("vsdb: len = %d, too short.", len);
        return HDF_SUCCESS;
    }
    HdmiEdidVsdbCecPhyAddrPhase(sinkCap, data, len);
    /* byte 5: color depth flags */
    HdmiEdidVsdbColorDepthPhase(sinkCap, data, len);
    /* byte 6: max tmds clock. */
    HdmiEdidVsdbMaxTmdsClockPhase(sinkCap, data, len);
    /* byte7: some sink present */
    HdmiEdidVsdbSinkPresentPhase(sinkCap, data, len);
    /*
     * byte8: Video_Latency
     * byte9: Audio_Latency
     * byte10: Interlaced_Video_Latency
     * byte11: Interlaced_Audio_Latency
     */
    HdmiEdidVsdbSinkLatencyPhase(sinkCap, data, len);
    /*
     * byte12: 3D_Present, 3D_Multi_present
     * byte13: HDMI_VIC_LEN/HDMI_3D_LEN
     * byte14~byteN: Vic info/3D info
     */
    HdmiEdidVsdbVicAnd3dInfoPhase(sinkCap, data, len);
    return HDF_SUCCESS;
}

static void HdmiEdidHfVsdb21Phase(struct HdmiSinkDeviceCapability *sinkCap, uint8_t *data, uint8_t len)
{
    sinkCap->hfVsdbInfo.maxFrlRate = (data[UINT8_ARRAY_TElEMENT_6] & HDMI_UPPER_NIBBLE_MARK) >> HDMI_NIBBLE_SHIFT;

    sinkCap->hfVsdbInfo.fapaStartLocation = (data[UINT8_ARRAY_TElEMENT_7] & HDMI_BIT0_MARK) ? true : false;
    sinkCap->hfVsdbInfo.allm = (data[UINT8_ARRAY_TElEMENT_7] & HDMI_BIT1_MARK) ? true : false;
    sinkCap->hfVsdbInfo.fva = (data[UINT8_ARRAY_TElEMENT_7] & HDMI_BIT2_MARK) ? true : false;
    sinkCap->hfVsdbInfo.cnmVrr = (data[UINT8_ARRAY_TElEMENT_7] & HDMI_BIT3_MARK) ? true : false;
    sinkCap->hfVsdbInfo.cinemaVrr = (data[UINT8_ARRAY_TElEMENT_7] & HDMI_BIT4_MARK) ? true : false;
    sinkCap->hfVsdbInfo.mDelta = (data[UINT8_ARRAY_TElEMENT_7] & HDMI_BIT5_MARK) ? true : false;
    sinkCap->hfVsdbInfo.vrrMin = (data[UINT8_ARRAY_TElEMENT_8] & HDMI_EDID_EXTENSION_HFVSDB_VRRMIN_MARK);
    sinkCap->hfVsdbInfo.vrrMax = ((data[UINT8_ARRAY_TElEMENT_8] & HDMI_EDID_EXTENSION_HFVSDB_VRRMAX_MARK) <<
        HDMI_EDID_EXTENSION_HFVSDB_VRRMAX_SHIFT) | data[UINT8_ARRAY_TElEMENT_9];

    sinkCap->hfVsdbInfo.dscInfo.dsc1p2 = (data[UINT8_ARRAY_TElEMENT_10] & HDMI_BIT7_MARK) ? true : false;
    sinkCap->hfVsdbInfo.dscInfo.dscNative420 = (data[UINT8_ARRAY_TElEMENT_10] & HDMI_BIT6_MARK) ? true : false;
    sinkCap->hfVsdbInfo.dscInfo.dscAllBpp = (data[UINT8_ARRAY_TElEMENT_10] & HDMI_BIT3_MARK) ? true : false;
    sinkCap->hfVsdbInfo.dscInfo.dsc10bpc = (data[UINT8_ARRAY_TElEMENT_10] & HDMI_BIT2_MARK) ? true : false;
    sinkCap->hfVsdbInfo.dscInfo.dsc20bpc = (data[UINT8_ARRAY_TElEMENT_10] & HDMI_BIT1_MARK) ? true : false;
    sinkCap->hfVsdbInfo.dscInfo.dsc16bpc = (data[UINT8_ARRAY_TElEMENT_10] & HDMI_BIT0_MARK) ? true : false;
    sinkCap->hfVsdbInfo.dscInfo.dscMaxSlices = (data[UINT8_ARRAY_TElEMENT_11] & HDMI_LOWER_NIBBLE_MARK);
    sinkCap->hfVsdbInfo.dscInfo.dscMaxFrlRate = (data[UINT8_ARRAY_TElEMENT_11] & HDMI_UPPER_NIBBLE_MARK) >>
                                                HDMI_NIBBLE_SHIFT;
    sinkCap->hfVsdbInfo.dscInfo.dscTotalChunkKBytes = (data[UINT8_ARRAY_TElEMENT_12] &
        HDMI_EDID_EXTENSION_HFVSDB_DSC_TOTAL_CHUNK_MARK);
}

static int32_t HdmiEdidHfVsdbPhase(struct HdmiSinkDeviceCapability *sinkCap, uint8_t *data, uint8_t len)
{
    if (len < HDMI_EDID_EXTENSION_HFVSDB_MIN_INVALID_LEN) {
        HDF_LOGD("vsdb: data len %d is too short.", len);
        return HDF_SUCCESS;
    }

    /* byte3: Version */
    if (data[UINT8_ARRAY_TElEMENT_3] != HDMI_EDID_EXTENSION_HFVSDB_VERSION) {
        HDF_LOGD("vsdb: verdion %d is invalid.", data[UINT8_ARRAY_TElEMENT_3]);
    }
    /* byte4: Max_TMDS_Character_Rate */
    sinkCap->maxTmdsClk = data[UINT8_ARRAY_TElEMENT_4] * HDMI_EDID_EXTENSION_TMDS_FACTOR;
    sinkCap->supportHdmi20 = (sinkCap->maxTmdsClk > HDMI_EDID_EXTENSION_MAX_HDMI14_TMDS_RATE) ? true : false;
    /* byte5: several sink present */
    sinkCap->hfVsdbInfo.scdcPresent = (data[UINT8_ARRAY_TElEMENT_5] & HDMI_BIT7_MARK) ? true : false;
    sinkCap->hfVsdbInfo.rrCapable = (data[UINT8_ARRAY_TElEMENT_5] & HDMI_BIT6_MARK) ? true : false;
    sinkCap->hfVsdbInfo.lte340McscScramble = (data[UINT8_ARRAY_TElEMENT_5] & HDMI_BIT3_MARK) ? true : false;
    sinkCap->hfVsdbInfo.independentView = (data[UINT8_ARRAY_TElEMENT_5] & HDMI_BIT2_MARK) ? true : false;
    sinkCap->hfVsdbInfo.dualView = (data[UINT8_ARRAY_TElEMENT_5] & HDMI_BIT1_MARK) ? true : false;
    sinkCap->hfVsdbInfo._3dOsdDisparity = (data[UINT8_ARRAY_TElEMENT_5] & HDMI_BIT0_MARK) ? true : false;
    /* byte6: deep color */
    sinkCap->hfVsdbInfo.dc.dc30bit = (data[UINT8_ARRAY_TElEMENT_6] & HDMI_BIT0_MARK) ? true : false;
    sinkCap->hfVsdbInfo.dc.dc36bit = (data[UINT8_ARRAY_TElEMENT_6] & HDMI_BIT1_MARK) ? true : false;
    sinkCap->hfVsdbInfo.dc.dc48bit = (data[UINT8_ARRAY_TElEMENT_6] & HDMI_BIT2_MARK) ? true : false;
    if (len > HDMI_EDID_EXTENSION_HFVSDB_MIN_INVALID_LEN &&
        len <= HDMI_EDID_EXTENSION_HFVSDB_MAX_INVALID_LEN) {
        HdmiEdidHfVsdb21Phase(sinkCap, data, len);
    }
    return HDF_SUCCESS;
}

static int32_t HdmiEdidExtVsDataBlockPhase(struct HdmiSinkDeviceCapability *sinkCap, uint8_t *data, uint8_t len)
{
    int32_t ret = HDF_SUCCESS;
    bool vsdb = false;
    bool hfVsdb = false;

    if (len >= HDMI_EDID_EXTENSION_VSDB_LEN &&
        data[UINT8_ARRAY_TElEMENT_0] == HDMI_EDID_EXTENSION_VSDB_IEEE_1ST &&
        data[UINT8_ARRAY_TElEMENT_1] == HDMI_EDID_EXTENSION_VSDB_IEEE_2ND &&
        data[UINT8_ARRAY_TElEMENT_2] == HDMI_EDID_EXTENSION_VSDB_IEEE_3RD) {
        vsdb = true;
    }

    if (len >= HDMI_EDID_EXTENSION_VSDB_LEN &&
        data[UINT8_ARRAY_TElEMENT_0] == HDMI_EDID_EXTENSION_HFVSDB_IEEE_1ST &&
        data[UINT8_ARRAY_TElEMENT_1] == HDMI_EDID_EXTENSION_HFVSDB_IEEE_2ND &&
        data[UINT8_ARRAY_TElEMENT_2] == HDMI_EDID_EXTENSION_HFVSDB_IEEE_3RD) {
        hfVsdb = true;
    }

    if (vsdb == true) {
        sinkCap->supportHdmi14 = true;
        ret = HdmiEdidVsdbPhase(sinkCap, data, len);
    } else if (hfVsdb == true) {
        ret = HdmiEdidHfVsdbPhase(sinkCap, data, len);
    }
    return ret;
}

static int32_t HdmiEdidExtSpeakerDataBlockPhase(struct HdmiSinkDeviceCapability *sinkCap, uint8_t *data, uint8_t len)
{
    if (len < HDMI_EDID_EXTENSION_SADB_MIN_INVALID_LEN) {
        HDF_LOGD("SADB: len %hhu is too short", len);
        return HDF_SUCCESS;
    }

    sinkCap->supportAudioSpeaker[HDMI_EDID_AUDIO_SPEAKER_FL_FR] =
        (data[UINT8_ARRAY_TElEMENT_0] & HDMI_BIT0_MARK) ? true : false;
    sinkCap->supportAudioSpeaker[HDMI_EDID_AUDIO_SPEAKER_LFE] =
        (data[UINT8_ARRAY_TElEMENT_0] & HDMI_BIT1_MARK) ? true : false;
    sinkCap->supportAudioSpeaker[HDMI_EDID_AUDIO_SPEAKER_FC] =
        (data[UINT8_ARRAY_TElEMENT_0] & HDMI_BIT2_MARK) ? true : false;
    sinkCap->supportAudioSpeaker[HDMI_EDID_AUDIO_SPEAKER_BL_BR] =
        (data[UINT8_ARRAY_TElEMENT_0] & HDMI_BIT3_MARK) ? true : false;
    sinkCap->supportAudioSpeaker[HDMI_EDID_AUDIO_SPEAKER_BC] =
        (data[UINT8_ARRAY_TElEMENT_0] & HDMI_BIT4_MARK) ? true : false;
    sinkCap->supportAudioSpeaker[HDMI_EDID_AUDIO_SPEAKER_FLC_FRC] =
        (data[UINT8_ARRAY_TElEMENT_0] & HDMI_BIT5_MARK) ? true : false;
    sinkCap->supportAudioSpeaker[HDMI_EDID_AUDIO_SPEAKER_RLC_RRC] =
        (data[UINT8_ARRAY_TElEMENT_0] & HDMI_BIT6_MARK) ? true : false;
    sinkCap->supportAudioSpeaker[HDMI_EDID_AUDIO_SPEAKER_FLW_FRW] =
        (data[UINT8_ARRAY_TElEMENT_0] & HDMI_BIT7_MARK) ? true : false;
    sinkCap->supportAudioSpeaker[HDMI_EDID_AUDIO_SPEAKER_TPFL_TPFH] =
        (data[UINT8_ARRAY_TElEMENT_1] & HDMI_BIT0_MARK) ? true : false;
    sinkCap->supportAudioSpeaker[HDMI_EDID_AUDIO_SPEAKER_TPC] =
        (data[UINT8_ARRAY_TElEMENT_1] & HDMI_BIT1_MARK) ? true : false;
    sinkCap->supportAudioSpeaker[HDMI_EDID_AUDIO_SPEAKER_TPFC] =
        (data[UINT8_ARRAY_TElEMENT_1] & HDMI_BIT2_MARK) ? true : false;
    return HDF_SUCCESS;
}

static void HdmiEdidExtUseExtDataBlockVcdbPhase(struct HdmiSinkDeviceCapability *sinkCap,
    uint8_t *data, uint8_t len)
{
    if (len < HDMI_EDID_VCDB_LEN) {
        HDF_LOGD("VCDB: len is too short");
        return;
    }

    sinkCap->videoCap.qy = (data[UINT8_ARRAY_TElEMENT_1] & HDMI_BIT7_MARK) ? true : false;
    sinkCap->videoCap.qs = (data[UINT8_ARRAY_TElEMENT_1] & HDMI_BIT6_MARK) ? true : false;
}

static void HdmiEdidExtUseExtDataBlockCdbPhase(struct HdmiSinkDeviceCapability *sinkCap,
    uint8_t *data, uint8_t len)
{
    if (len < HDMI_EDID_CDB_LEN) {
        HDF_LOGD("CDB: len is too short");
        return;
    }

    sinkCap->colorimetry.xvYcc601 = (data[UINT8_ARRAY_TElEMENT_1] & HDMI_BIT0_MARK) ? true : false;
    sinkCap->colorimetry.xvYcc709 = (data[UINT8_ARRAY_TElEMENT_1] & HDMI_BIT1_MARK) ? true : false;
    sinkCap->colorimetry.sYcc601 = (data[UINT8_ARRAY_TElEMENT_1] & HDMI_BIT2_MARK) ? true : false;
    sinkCap->colorimetry.opYcc601 = (data[UINT8_ARRAY_TElEMENT_1] & HDMI_BIT3_MARK) ? true : false;
    sinkCap->colorimetry.opRgb = (data[UINT8_ARRAY_TElEMENT_1] & HDMI_BIT4_MARK) ? true : false;
    sinkCap->colorimetry.bt2020cYcc = (data[UINT8_ARRAY_TElEMENT_1] & HDMI_BIT5_MARK) ? true : false;
    sinkCap->colorimetry.bt2020Ycc = (data[UINT8_ARRAY_TElEMENT_1] & HDMI_BIT6_MARK) ? true : false;
    sinkCap->colorimetry.bt2020Rgb = (data[UINT8_ARRAY_TElEMENT_1] & HDMI_BIT7_MARK) ? true : false;

    sinkCap->colorimetry.dciP3 = (data[UINT8_ARRAY_TElEMENT_2] & HDMI_BIT7_MARK) ? true : false;
    sinkCap->colorimetry.md = (data[UINT8_ARRAY_TElEMENT_2] & HDMI_LOWER_NIBBLE_MARK);
}

static void HdmiEdidExtUseExtDataBlockY420VdbPhase(struct HdmiSinkDeviceCapability *sinkCap,
    uint8_t *data, uint8_t len)
{
    uint8_t i;
    uint32_t vic;

    for (i = 1; i < len; i++) {
        if (sinkCap->y420Cap.onlySupportY420VicNum >= HDMI_EDID_EXTENSION_MAX_VIC_COUNT) {
            HDF_LOGD("Y420Vdb: vic num reach to max.");
            break;
        }
        vic = data[i];
        if (vic == 0 ||
            (vic >= HDMI_EDID_EXTENSION_VIC_INVALID_LOW && vic <= HDMI_EDID_EXTENSION_VIC_INVALID_HIGH)) {
            continue;
        }
        sinkCap->y420Cap.onlySupportY420Format[sinkCap->y420Cap.onlySupportY420VicNum] = vic;
        sinkCap->y420Cap.onlySupportY420VicNum++;
        sinkCap->colorSpace.ycbcr420 = true;
    }
}

static void HdmiEdidExtUseExtDataBlockY420CmdbPhase(struct HdmiSinkDeviceCapability *sinkCap,
    uint8_t *data, uint8_t len)
{
    uint32_t i, loop;

    /*
     * When the Length field is set to 1, the Y420CMDB does not include a YCBCR 4:2:0 Capability Bit Map and
     * all the SVDs in the regular Video Data Block(s) support YCBCR 4:2:0 sampling mode.
     */
    if (len == 1) {
        for (i = 0; (i < sinkCap->videoInfo.vicNum) && (i < HDMI_EDID_EXTENSION_MAX_VIC_COUNT); i++) {
            if (sinkCap->y420Cap.SupportY420VicNum >= HDMI_EDID_EXTENSION_MAX_VIC_COUNT) {
                break;
            }
            sinkCap->y420Cap.SupportY420Format[sinkCap->y420Cap.SupportY420VicNum] = sinkCap->videoInfo.vic[i];
            sinkCap->y420Cap.SupportY420VicNum++;
            sinkCap->colorSpace.ycbcr420 = true;
        }
        return;
    }

    /*
     * Bit 0 of data byte 3 is associated with the first sequential SVD listed in the regular Video Data Block(s)
     * of the EDID, bit 1 the second SVD, bit 2 the third, and so on.
     */
    loop = len * HDMI_BITS_OF_ONE_BYTE;
    loop = (loop > HDMI_EDID_EXTENSION_MAX_VIC_COUNT) ? HDMI_EDID_EXTENSION_MAX_VIC_COUNT : loop;
    data++;
    for (i = 0; (i < loop) && (i < sinkCap->videoInfo.vicNum); i++) {
        if (sinkCap->y420Cap.SupportY420VicNum >= HDMI_EDID_EXTENSION_MAX_VIC_COUNT) {
            break;
        }
        if ((data[i / HDMI_BITS_OF_ONE_BYTE] & (0x01 << (i % HDMI_BITS_OF_ONE_BYTE))) > 0) {
            sinkCap->y420Cap.SupportY420Format[sinkCap->y420Cap.SupportY420VicNum] = sinkCap->videoInfo.vic[i];
            sinkCap->y420Cap.SupportY420VicNum++;
            sinkCap->colorSpace.ycbcr420 = true;
        }
    }
}

static void HdmiEdidExtUseExtDataBlockHdrSmdbPhase(struct HdmiSinkDeviceCapability *sinkCap,
    uint8_t *data, uint8_t len)
{
    if (len < HDMI_EDID_HDR_SMDB_MIN_LEN) {
        HDF_LOGD("Hdr SMDB: len is too short");
        return;
    }

    sinkCap->hdrCap.eotf.sdr = (data[UINT8_ARRAY_TElEMENT_1] & HDMI_BIT0_MARK) ? true : false;
    sinkCap->hdrCap.eotf.hdr = (data[UINT8_ARRAY_TElEMENT_1] & HDMI_BIT1_MARK) ? true : false;
    sinkCap->hdrCap.eotf.smpteSt2048 = (data[UINT8_ARRAY_TElEMENT_1] & HDMI_BIT2_MARK) ? true : false;
    sinkCap->hdrCap.eotf.hlg = (data[UINT8_ARRAY_TElEMENT_1] & HDMI_BIT2_MARK) ? true : false;
    sinkCap->hdrCap.smType1 = (data[UINT8_ARRAY_TElEMENT_2] & HDMI_BIT0_MARK) ? true : false;

    /*
     * The length of the data block, n, in Byte 1 indicates which of the Bytes 5 to 7 are present. Bytes 5 to 7 are
     * optional to declare. When n is 3, Bytes 5 to 7 are not present. When n is 4, Byte 5 is present; when n is 5,
     * Bytes 5 and 6 are present; and when n is 6, Bytes 5 to 7 are present.
     */
    if (len >= HDMI_EDID_HDR_SMDB_MIN_LEN_FOR_MAX_LUMINANCE_DATA) {
        sinkCap->hdrCap.maxLuminancedata = data[UINT8_ARRAY_TElEMENT_3];
    }
    if (len >= HDMI_EDID_HDR_SMDB_MIN_LEN_FOR_MAX_FRAME_AVE_LUMINANCE_DATA) {
        sinkCap->hdrCap.maxFrameAverageLuminanceData = data[UINT8_ARRAY_TElEMENT_4];
    }
    if (len >= HDMI_EDID_HDR_SMDB_MIN_LEN_FOR_MIN_LUMINANCE_DATA) {
        sinkCap->hdrCap.minLuminanceData = data[UINT8_ARRAY_TElEMENT_5];
    }
}

static void HdmiEdidDolbyCapVersionZeroPhase(struct HdmiSinkDeviceCapability *sinkCap, uint8_t *data)
{
    sinkCap->dolbyCap.globalDimming = (data[UINT8_ARRAY_TElEMENT_4] & HDMI_BIT2_MARK) ? true : false;
    sinkCap->dolbyCap.redX = ((data[UINT8_ARRAY_TElEMENT_5] &
                                HDMI_UPPER_NIBBLE_MARK) >>
                                HDMI_NIBBLE_SHIFT) |
                                (data[UINT8_ARRAY_TElEMENT_6] << HDMI_NIBBLE_SHIFT);
    sinkCap->dolbyCap.redY = (data[UINT8_ARRAY_TElEMENT_5] &
                                HDMI_LOWER_NIBBLE_MARK) |
                                (data[UINT8_ARRAY_TElEMENT_7] << HDMI_NIBBLE_SHIFT);
    sinkCap->dolbyCap.greenX = ((data[UINT8_ARRAY_TElEMENT_8] & HDMI_UPPER_NIBBLE_MARK) >>
                                    HDMI_NIBBLE_SHIFT) |
                                    (data[UINT8_ARRAY_TElEMENT_9] << HDMI_NIBBLE_SHIFT);
    sinkCap->dolbyCap.greenY = (data[UINT8_ARRAY_TElEMENT_8] &
                                HDMI_LOWER_NIBBLE_MARK) |
                                (data[UINT8_ARRAY_TElEMENT_10] << HDMI_NIBBLE_SHIFT);
    sinkCap->dolbyCap.blueX = ((data[UINT8_ARRAY_TElEMENT_11] & HDMI_UPPER_NIBBLE_MARK) >>
                                HDMI_NIBBLE_SHIFT) |
                                (data[UINT8_ARRAY_TElEMENT_12] << HDMI_NIBBLE_SHIFT);
    sinkCap->dolbyCap.blueY = (data[UINT8_ARRAY_TElEMENT_11] &
                                HDMI_LOWER_NIBBLE_MARK) |
                                (data[UINT8_ARRAY_TElEMENT_13] << HDMI_NIBBLE_SHIFT);
    sinkCap->dolbyCap.whiteX = ((data[UINT8_ARRAY_TElEMENT_14] & HDMI_UPPER_NIBBLE_MARK) >>
                                    HDMI_NIBBLE_SHIFT) |
                                    (data[UINT8_ARRAY_TElEMENT_15] << HDMI_NIBBLE_SHIFT);
    sinkCap->dolbyCap.whiteY = (data[UINT8_ARRAY_TElEMENT_14] & HDMI_LOWER_NIBBLE_MARK) |
                                (data[UINT8_ARRAY_TElEMENT_16] << HDMI_NIBBLE_SHIFT);
    sinkCap->dolbyCap.minLuminance = ((data[UINT8_ARRAY_TElEMENT_17] & HDMI_UPPER_NIBBLE_MARK) >>
                                        HDMI_NIBBLE_SHIFT) |
                                        (data[UINT8_ARRAY_TElEMENT_18] << HDMI_NIBBLE_SHIFT);
    sinkCap->dolbyCap.maxLuminance = (data[UINT8_ARRAY_TElEMENT_17] & HDMI_LOWER_NIBBLE_MARK) |
                                        (data[UINT8_ARRAY_TElEMENT_19] << HDMI_NIBBLE_SHIFT);
    sinkCap->dolbyCap.dMajorVer = (data[UINT8_ARRAY_TElEMENT_20] & HDMI_UPPER_NIBBLE_MARK) >>
                                    HDMI_NIBBLE_SHIFT;
    sinkCap->dolbyCap.dMinorVer = (data[UINT8_ARRAY_TElEMENT_20] & HDMI_LOWER_NIBBLE_MARK);
}

static void HdmiEdidDolbyCapVersionOnePhase(struct HdmiSinkDeviceCapability *sinkCap, uint8_t *data)
{
    sinkCap->dolbyCap.dmVer = (data[UINT8_ARRAY_TElEMENT_4] & HDMI_EDID_VSVDB_DOLBY_DM_VER_MARK) >>
                                HDMI_EDID_VSVDB_DOLBY_DM_VER_SHIFT;
    sinkCap->dolbyCap.globalDimming = (data[UINT8_ARRAY_TElEMENT_5] & HDMI_BIT0_MARK) ? true : false;
    sinkCap->dolbyCap.maxLuminance = ((data[UINT8_ARRAY_TElEMENT_5] >> 1) & HDMI_EDID_VSVDB_DOLBY_LOWER_7BIT_MARK);
    sinkCap->dolbyCap.colorimetry = (data[UINT8_ARRAY_TElEMENT_6] & HDMI_BIT0_MARK) ? true : false;
    sinkCap->dolbyCap.minLuminance = ((data[UINT8_ARRAY_TElEMENT_6] >> 1) & HDMI_EDID_VSVDB_DOLBY_LOWER_7BIT_MARK);
    sinkCap->dolbyCap.redX = data[UINT8_ARRAY_TElEMENT_8];
    sinkCap->dolbyCap.redY = data[UINT8_ARRAY_TElEMENT_9];
    sinkCap->dolbyCap.greenX = data[UINT8_ARRAY_TElEMENT_10];
    sinkCap->dolbyCap.greenY = data[UINT8_ARRAY_TElEMENT_11];
    sinkCap->dolbyCap.blueX = data[UINT8_ARRAY_TElEMENT_12];
    sinkCap->dolbyCap.blueY = data[UINT8_ARRAY_TElEMENT_13];
}

static void HdmiEdidExtUseExtDataBlockVsvdbPhase(struct HdmiSinkDeviceCapability *sinkCap,
                                                 uint8_t *data,
                                                 uint8_t len)
{
    uint32_t oui;

    if (len != HDMI_EDID_VSVDB_DOLBY_VERSION_0_LEN &&
        len != HDMI_EDID_VSVDB_DOLBY_VERSION_1_LEN) {
        HDF_LOGD("Vsvdb: invalid dolby len");
        return;
    }

    oui = (data[UINT8_ARRAY_TElEMENT_1]) | (data[UINT8_ARRAY_TElEMENT_2] << 8) | (data[UINT8_ARRAY_TElEMENT_3] << 16);
    if (oui != HDMI_EDID_VSVDB_DOLBY_OUI) {
        return;
    }
    sinkCap->dolbyCap.oui = oui;
    sinkCap->dolbyCap.version = (data[UINT8_ARRAY_TElEMENT_4] &
                                 HDMI_EDID_VSVDB_DOLBY_VERSION_MARK) >> HDMI_EDID_VSVDB_DOLBY_VERSION_SHIFT;
    sinkCap->dolbyCap.yuv422 = (data[UINT8_ARRAY_TElEMENT_4] & HDMI_BIT0_MARK) ? true : false;
    sinkCap->dolbyCap.b2160p60 = (data[UINT8_ARRAY_TElEMENT_4] & HDMI_BIT1_MARK) ? true : false;
    if (sinkCap->dolbyCap.version == HDMI_EDID_VSVDB_DOLBY_VERSION_0) {
        HdmiEdidDolbyCapVersionZeroPhase(sinkCap, data);
        return;
    }
    if (sinkCap->dolbyCap.version == HDMI_EDID_VSVDB_DOLBY_VERSION_1) {
        HdmiEdidDolbyCapVersionOnePhase(sinkCap, data);
    }
}

static int32_t HdmiEdidExtUseExtDataBlockPhase(struct HdmiSinkDeviceCapability *sinkCap, uint8_t *data, uint8_t len)
{
    uint8_t extTagCode = data[UINT8_ARRAY_TElEMENT_0];

    switch (extTagCode) {
        case HDMI_EDID_EXT_VCDB:
            HdmiEdidExtUseExtDataBlockVcdbPhase(sinkCap, data, len);
            break;
        case HDMI_EDID_EXT_VSVDB:
            HdmiEdidExtUseExtDataBlockVsvdbPhase(sinkCap, data, len);
            break;
        case HDMI_EDID_EXT_CDB:
            HdmiEdidExtUseExtDataBlockCdbPhase(sinkCap, data, len);
            break;
        case HDMI_EDID_EXT_HDR_SMDB:
            HdmiEdidExtUseExtDataBlockHdrSmdbPhase(sinkCap, data, len);
            break;
        case HDMI_EDID_EXT_YCBCR420_VDB:
            HdmiEdidExtUseExtDataBlockY420VdbPhase(sinkCap, data, len);
            break;
        case HDMI_EDID_EXT_YCBCR420_CMDB:
            HdmiEdidExtUseExtDataBlockY420CmdbPhase(sinkCap, data, len);
            break;
        default:
            HDF_LOGD("ext use ext DB: tag code %hhu unphase", extTagCode);
            break;
    }
    return HDF_SUCCESS;
}

static int32_t HdmiEdidExtDataBlockPhase(struct HdmiSinkDeviceCapability *sinkCap,
    uint8_t *data, uint8_t len, uint8_t tag)
{
    int32_t ret = HDF_SUCCESS;

    if (len == 0) {
        HDF_LOGD("ext DB: len is 0");
        return ret;
    }

    switch (tag) {
        case HDMI_EDID_AUDIO_DATA_BLOCK:
            /* Audio Data Block (includes one or more Short Audio Descriptors) */
            ret = HdmiEdidExtAudioDataBlockPhase(sinkCap, data, len);
            break;
        case HDMI_EDID_VIDEO_DATA_BLOCK:
            /* Video Data Block (includes one or more Short Video Descriptors) */
            ret = HdmiEdidExtVideoDataBlockPhase(sinkCap, data, len);
            break;
        case HDMI_EDID_VENDOR_SPECIFIC_DATA_BLOCK:
            ret = HdmiEdidExtVsDataBlockPhase(sinkCap, data, len);
            break;
        case HDMI_EDID_SPEAKER_ALLOCATION_DATA_BLOCK:
            ret = HdmiEdidExtSpeakerDataBlockPhase(sinkCap, data, len);
            break;
        case HDMI_EDID_USE_EXT_DATA_BLOCK:
            ret = HdmiEdidExtUseExtDataBlockPhase(sinkCap, data, len);
            break;
        default:
            HDF_LOGD("tag = %d is reserved or unphase block", tag);
            break;
    }
    return ret;
}

static void HdmiEdidExtSeveralDataBlockPhase(struct HdmiEdid *edid, uint8_t blockNum)
{
    uint8_t *data = edid->raw + (blockNum * HDMI_EDID_SINGLE_BLOCK_SIZE);
    struct HdmiSinkDeviceCapability *sinkCap = &(edid->sinkCap);
    uint8_t blkOffset = HDMI_EDID_EXTENSION_BLOCK_OFFSET;
    uint8_t dtdOffset = data[UINT8_ARRAY_TElEMENT_2];
    uint8_t dbTagCode, blkLen;
    int32_t ret;

    data += blkOffset;
    /* phase data block */
    for (blkLen = 0; (blkOffset < dtdOffset) && (data != NULL); blkOffset += (blkLen + 1)) {
        data += blkLen;
        blkLen = (data[UINT8_ARRAY_TElEMENT_0] & HDMI_EDID_EXTENSION_DATA_BLOCK_LEN_MARK);
        dbTagCode = (data[UINT8_ARRAY_TElEMENT_0] & HDMI_EDID_EXTENSION_DATA_BLOCK_TAG_CODE_MARK) >>
            HDMI_EDID_EXTENSION_DATA_BLOCK_TAG_CODE_SHIFT;
        data++;
        ret = HdmiEdidExtDataBlockPhase(sinkCap, data, blkLen, dbTagCode);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("data block %hhu phase fail", dbTagCode);
            return;
        }
    }

    data += blkLen;
    /* phase detialed timing descriptors */
    while ((HDMI_EDID_SINGLE_BLOCK_SIZE - 1 - blkOffset) >= HDMI_EDID_DETAILED_TIMING_DESCRIPTOR_FIELD_LEN) {
        HdmiEdidDetailedTiming(sinkCap, data, HDMI_EDID_DETAILED_TIMING_DESCRIPTOR_FIELD_LEN);
        blkOffset += HDMI_EDID_DETAILED_TIMING_DESCRIPTOR_FIELD_LEN;
        data += HDMI_EDID_DETAILED_TIMING_DESCRIPTOR_FIELD_LEN;
    }
}

static int32_t HdmiEdidExtBlockPhase(struct HdmiEdid *edid, uint8_t blockNum)
{
    uint8_t *data = edid->raw;
    struct HdmiSinkDeviceCapability *sinkCap = &(edid->sinkCap);
    int32_t ret;

    if (blockNum >= HDMI_EDID_MAX_BLOCK_NUM) {
        HDF_LOGE("blockNum %d is invalid", blockNum);
        return HDF_ERR_INVALID_PARAM;
    }

    data += (blockNum * HDMI_EDID_SINGLE_BLOCK_SIZE);
    ret = HdmiEdidBlockCheckSum(data);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("edid block%d check sum fail.", blockNum);
        return ret;
    }

    /* byte0: Extension Tag */
    if (data[UINT8_ARRAY_TElEMENT_0] != HDMI_EDID_CTA_EXTENSION_TAG) {
        HDF_LOGD("ext tag is %hhu", data[UINT8_ARRAY_TElEMENT_0]);
    }
    /* byte1: Extension Revision Number */
    if (data[UINT8_ARRAY_TElEMENT_1] != HDMI_EDID_CTA_EXTENSION3_REVISION) {
        HDF_LOGD("revision number is %d", data[UINT8_ARRAY_TElEMENT_1]);
    }
    /*
     * byte2: Byte number offset d where 18-byte descriptors begin (typically Detailed Timing Descriptors).
     * If no data is provided in the reserved data block, then d is 4. If d is 0, then no detailed timing
     * descriptors are provided and no data is provided in the reserved data block collection.
     */
    if (data[UINT8_ARRAY_TElEMENT_2] < HDMI_EDID_EXTENSION_D_INVALID_MIN_VAL) {
        HDF_LOGD("ext block%d no dtd", blockNum);
        return HDF_SUCCESS;
    }
    /* byte3: indication of underscan support, audio support, support of YCBCR and total number of native DTDs. */
    sinkCap->colorSpace.rgb444 = true;
    sinkCap->colorSpace.ycbcr422 = (data[UINT8_ARRAY_TElEMENT_3] & HDMI_BIT4_MARK) ? true : false;
    sinkCap->colorSpace.ycbcr444 = (data[UINT8_ARRAY_TElEMENT_3] & HDMI_BIT5_MARK) ? true : false;
    sinkCap->supportAudio = (data[UINT8_ARRAY_TElEMENT_3] & HDMI_BIT6_MARK) ? true : false;
    /*
     * Video Data Block, Audio Data Block, Speaker Allocation Data Block,
     * Vendor Specific Data Block and Video Capability Data Block phase.
     */
    HdmiEdidExtSeveralDataBlockPhase(edid, blockNum);
    return HDF_SUCCESS;
}

int32_t HdmiEdidPhase(struct HdmiEdid *edid)
{
    uint8_t blockNum;
    int32_t ret;
    struct HdmiSinkDeviceCapability *sinkCap = NULL;

    if (edid == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    ret = HdmiEdidFirstBlockPhase(edid);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("edid first block phase fail.");
        return ret;
    }

    sinkCap = &(edid->sinkCap);
    for (blockNum = 1; blockNum <= sinkCap->extBlockNum; blockNum++) {
        ret = HdmiEdidExtBlockPhase(edid, blockNum);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("edid ext block%hhu phase fail.", blockNum);
            return ret;
        }
    }
    return HDF_SUCCESS;
}

int32_t HdmiEdidRawDataRead(struct HdmiEdid *edid, struct HdmiDdc *ddc)
{
    struct HdmiDdcCfg cfg = {0};
    int32_t ret;
    uint8_t extBlkNum;

    if (edid == NULL || ddc == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    /* read block0 */
    cfg.type = HDMI_DDC_DEV_EDID;
    cfg.mode = HDMI_DDC_MODE_READ_MUTIL_NO_ACK;
    cfg.data = edid->raw;
    cfg.dataLen = HDMI_EDID_SINGLE_BLOCK_SIZE;
    cfg.readFlag = true;
    cfg.devAddr = HDMI_DDC_EDID_DEV_ADDRESS;
    ret = HdmiDdcTransfer(ddc, &cfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("edid block0 read fail");
        return ret;
    }
    edid->rawLen += HDMI_EDID_SINGLE_BLOCK_SIZE;

    extBlkNum = edid->raw[HDMI_EDID_EXTENSION_BLOCK_ADDR];
    if (extBlkNum > (HDMI_EDID_MAX_BLOCK_NUM - 1)) {
        extBlkNum = (HDMI_EDID_MAX_BLOCK_NUM - 1);
        HDF_LOGD("extBlkNum > max, use max.");
    }
    if (extBlkNum == 0) {
        HDF_LOGD("edid only has block0");
        return HDF_SUCCESS;
    }

    /* read block1 */
    cfg.data += HDMI_EDID_SINGLE_BLOCK_SIZE;
    ret = HdmiDdcTransfer(ddc, &cfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("edid block1 read fail");
        return ret;
    }
    edid->rawLen += HDMI_EDID_SINGLE_BLOCK_SIZE;

    if (extBlkNum == 1) {
        HDF_LOGD("edid only has block0~1");
        return HDF_SUCCESS;
    }
    /* read block2~3 */
    cfg.data += HDMI_EDID_SINGLE_BLOCK_SIZE;
    cfg.dataLen = (extBlkNum - 1) * HDMI_EDID_SINGLE_BLOCK_SIZE;
    cfg.mode = HDMI_DDC_MODE_READ_SEGMENT_NO_ACK;
    cfg.segment = 1;
    ret = HdmiDdcTransfer(ddc, &cfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("edid block2~3 read fail");
        return ret;
    }
    edid->rawLen += (extBlkNum - 1) * HDMI_EDID_SINGLE_BLOCK_SIZE;
    return HDF_SUCCESS;
}

bool HdmiEdidSupportFrl(struct HdmiDevice *hdmi)
{
    if (hdmi == NULL) {
        HDF_LOGD("no hdmi sink.");
        return false;
    }

    if (hdmi->edid.sinkCap.hfVsdbInfo.scdcPresent == true &&
        hdmi->edid.sinkCap.hfVsdbInfo.maxFrlRate > 0) {
        return true;
    }
    return false;
}

uint8_t HdmiEdidGetMaxFrlRate(struct HdmiDevice *hdmi)
{
    if (hdmi == NULL) {
        HDF_LOGD("no hdmi sink.");
        return 0;
    }
    return hdmi->edid.sinkCap.hfVsdbInfo.maxFrlRate;
}

bool HdmiEdidScdcSupport(struct HdmiDevice *hdmi)
{
    if (hdmi == NULL) {
        HDF_LOGD("no hdmi sink.");
        return false;
    }
    return hdmi->edid.sinkCap.hfVsdbInfo.scdcPresent;
}
