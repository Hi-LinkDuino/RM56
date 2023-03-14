/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @addtogroup HDMI
 * @{
 *
 * @brief Declares standard APIs of basic High-Definition Multimedia Interface (HDMI) capabilities.
 *
 * You can use this module to access the HDMI and enable the driver to operate an HDMI sink device.
 * These capabilities include start and stop tramnsfer on HDMI, setting some attributes,
 * and read sink device's EDID data.
 *
 * @since 1.0
 */

/**
 * @file hdmi_if.h
 *
 * @brief Declares the standard HDMI interface functions.
 *
 * @since 1.0
 */

#ifndef HDMI_IF_H
#define HDMI_IF_H

#include "platform_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Indicates that maximum length of an HDMI sink device's EDID is 512 bytes.
 *
 * @since 1.0
 */
#define HDMI_EDID_MAX_LEN 512

/**
 * @brief Enumerates deep color,
 * see the section 6.2.4 of HDMI Specification 1.4 for details.
 *
 * @since 1.0
 */
enum HdmiDeepColor {
    HDMI_DEEP_COLOR_24BITS = 0,
    HDMI_DEEP_COLOR_30BITS = 1,
    HDMI_DEEP_COLOR_36BITS = 2,
    HDMI_DEEP_COLOR_48BITS = 3,
    HDMI_DEEP_COLOR_OFF = 0xff,
    HDMI_DEEP_COLOR_BUTT,
};

/**
 * @brief Enumerates video bit depth
 * see the section 6.6 of HDMI Specification 1.4 for details.
 *
 * @since 1.0
 */
enum HdmiVideoBitDepth {
    HDMI_VIDEO_BIT_DEPTH_8 = 0,
    HDMI_VIDEO_BIT_DEPTH_10 = 1,
    HDMI_VIDEO_BIT_DEPTH_12 = 2,
    HDMI_VIDEO_BIT_DEPTH_16 = 3,
    HDMI_VIDEO_BIT_DEPTH_OFF,
};

/**
 * @brief Enumerates color space,
 * see the section 6 of HDMI Specification 1.4 for details.
 *
 * @since 1.0
 */
enum HdmiColorSpace {
    HDMI_COLOR_SPACE_RGB = 0,
    HDMI_COLOR_SPACE_YCBCR422 = 1,
    HDMI_COLOR_SPACE_YCBCR444 = 2,
    HDMI_COLOR_SPACE_YCBCR420 = 3,
    HDMI_COLOR_SPACE_BUTT,
};

/**
 * @brief Enumerates colorimetry, see the section 6.7 of HDMI Specification 1.4 and
 * the section 7.2 of HDMI Specification 2.0 for details.
 *
 * @since 1.0
 */
enum HdmiColorimetry {
    HDMI_COLORIMETRY_NO_DATA = 0,
    HDMI_COLORIMETRY_ITU601 = 1,
    HDMI_COLORIMETRY_ITU709 = 2,
    HDMI_COLORIMETRY_EXTENDED = 3,
};

/**
 * @brief Enumerates extended colorimetry, see the section 6.7 of HDMI Specification 1.4 and
 * the section 7.2 of HDMI Specification 2.0 for details.
 *
 * @since 1.0
 */
enum HdmiExtendedColorimetry {
    HDMI_EXTENDED_COLORIMETRY_XV_YCC_601 = 0,
    HDMI_EXTENDED_COLORIMETRY_XV_YCC_709 = 1,
    HDMI_EXTENDED_COLORIMETRY_S_YCC_601 = 2,
    HDMI_EXTENDED_COLORIMETRY_OPYCC_601 = 3,
    HDMI_EXTENDED_COLORIMETRY_OPRGB = 4,
    HDMI_EXTENDED_COLORIMETRY_BT2020_CONST_LUM = 5,
    HDMI_EXTENDED_COLORIMETRY_BT2020 = 6,
    HDMI_EXTENDED_COLORIMETRY_ADDITIONAL = 7,
    HDMI_EXTENDED_COLORIMETRY_BUTT,
};

/**
 * @brief Enumerates quantization range, see the section 6.6 of HDMI Specification 1.4 and
 * the section 7.3 of HDMI Specification 2.0 for details.
 *
 * @since 1.0
 */
enum HdmiQuantizationRange {
    HDMI_QUANTIZATION_RANGE_DEFAULT = 0,
    HDMI_QUANTIZATION_RANGE_LIMITED = 1,
    HDMI_QUANTIZATION_RANGE_FULL = 2,
    HDMI_QUANTIZATION_RANGE_BUTT,
};

/**
 * @brief Enumerates YCC quantization range, see the section 6.6 of HDMI Specification 1.4 and
 * the section 7.3 of HDMI Specification 2.0 for details.
 *
 * @since 1.0
 */
enum HdmiYccQuantizationRange {
    HDMI_YCC_QUANTIZATION_RANGE_LIMITED = 0,
    HDMI_YCC_QUANTIZATION_RANGE_FULL = 1,
    HDMI_YCC_QUANTIZATION_RANGE_BUTT,
};

/**
 * @brief Enumerates video 3d structure,
 * see the section 8.2.3 of HDMI Specification 1.4 for details.
 *
 * @since 1.0
 */
enum HdmiVideo3dStructure {
    HDMI_VS_VIDEO_3D_FRAME_PACKING = 0,
    HDMI_VS_VIDEO_3D_FIELD_ALTERNATIVE = 1,
    HDMI_VS_VIDEO_3D_LINE_ALTERNATIVE = 2,
    HDMI_VS_VIDEO_3D_SIDE_BY_SIDE_FULL = 3,
    HDMI_VS_VIDEO_3D_L_DEPTH = 4,
    HDMI_VS_VIDEO_3D_L_DEPTH_GFX_GFX_DEPTH = 5,
    HDMI_VS_VIDEO_3D_TOP_AND_BOTTOM = 6,
    HDMI_VS_VIDEO_3D_SIDE_BY_SIDE_HALF = 8,
    HDMI_VS_VIDEO_3D_BUTT,
};

/**
 * @brief Enumerates video timing,
 * see the section 8.2.3 of HDMI Specification 1.4 for details.
 *
 * @since 1.0
 */
enum HdmiVideoTiming {
    HDMI_VIDEO_TIMING_NONE = 0,
    HDMI_VIDEO_TIMING_640X480P60 = 1,
    HDMI_VIDEO_TIMING_720X480P60 = 2,
    HDMI_VIDEO_TIMING_720X480P120 = 3,
    HDMI_VIDEO_TIMING_720X480P240 = 4,
    HDMI_VIDEO_TIMING_720X576P50 = 5,
    HDMI_VIDEO_TIMING_720X576P100 = 6,
    HDMI_VIDEO_TIMING_720X576P200 = 7,
    HDMI_VIDEO_TIMING_1280X720P24 = 8,
    HDMI_VIDEO_TIMING_1280X720P25 = 9,
    HDMI_VIDEO_TIMING_1280X720P30 = 10,
    HDMI_VIDEO_TIMING_1280X720P48 = 11,
    HDMI_VIDEO_TIMING_1280X720P50 = 12,
    HDMI_VIDEO_TIMING_1280X720P60 = 13,
    HDMI_VIDEO_TIMING_1280X720P100 = 14,
    HDMI_VIDEO_TIMING_1280X720P120 = 15,
    HDMI_VIDEO_TIMING_1440X240P60 = 16,
    HDMI_VIDEO_TIMING_1440X288P50 = 17,
    HDMI_VIDEO_TIMING_1440X480I60 = 18,
    HDMI_VIDEO_TIMING_1440X480P60 = 19,
    HDMI_VIDEO_TIMING_1440X480I120 = 20,
    HDMI_VIDEO_TIMING_1440X480I240 = 21,
    HDMI_VIDEO_TIMING_1440X576I50 = 22,
    HDMI_VIDEO_TIMING_1440X576P50 = 23,
    HDMI_VIDEO_TIMING_1440X576I60 = 24,
    HDMI_VIDEO_TIMING_1440X576I100 = 25,
    HDMI_VIDEO_TIMING_1440X576I200 = 26,
    HDMI_VIDEO_TIMING_2880X288P50 = 27,
    HDMI_VIDEO_TIMING_2880X480I60 = 28,
    HDMI_VIDEO_TIMING_2880X480P60 = 29,
    HDMI_VIDEO_TIMING_2880X240I60 = 30,
    HDMI_VIDEO_TIMING_2880X576I50 = 31,
    HDMI_VIDEO_TIMING_2880X576P50 = 32,
    HDMI_VIDEO_TIMING_1680X720P24 = 33,
    HDMI_VIDEO_TIMING_1680X720P25 = 34,
    HDMI_VIDEO_TIMING_1680X720P30 = 35,
    HDMI_VIDEO_TIMING_1680X720P48 = 36,
    HDMI_VIDEO_TIMING_1680X720P50 = 37,
    HDMI_VIDEO_TIMING_1680X720P60 = 38,
    HDMI_VIDEO_TIMING_1680X720P100 = 39,
    HDMI_VIDEO_TIMING_1680X720P120 = 40,
    HDMI_VIDEO_TIMING_2560X1080P24 = 41,
    HDMI_VIDEO_TIMING_2560X1080P25 = 42,
    HDMI_VIDEO_TIMING_2560X1080P30 = 43,
    HDMI_VIDEO_TIMING_2560X1080P48 = 44,
    HDMI_VIDEO_TIMING_2560X1080P50 = 45,
    HDMI_VIDEO_TIMING_2560X1080P60 = 46,
    HDMI_VIDEO_TIMING_2560X1080P100 = 47,
    HDMI_VIDEO_TIMING_2560X1080P120 = 48,
    HDMI_VIDEO_TIMING_1920X1080I60 = 49,
    HDMI_VIDEO_TIMING_1920X1080P60 = 50,
    HDMI_VIDEO_TIMING_1920X1080I50 = 51,
    HDMI_VIDEO_TIMING_1920X1080P50 = 52,
    HDMI_VIDEO_TIMING_1920X1080P24 = 53,
    HDMI_VIDEO_TIMING_1920X1080P25 = 54,
    HDMI_VIDEO_TIMING_1920X1080P30 = 55,
    HDMI_VIDEO_TIMING_1920X1080P48 = 56,
    HDMI_VIDEO_TIMING_1920X1080I100 = 57,
    HDMI_VIDEO_TIMING_1920X1080I120 = 58,
    HDMI_VIDEO_TIMING_1920X1080P120 = 59,
    HDMI_VIDEO_TIMING_1920X1080P100 = 60,
    HDMI_VIDEO_TIMING_3840X2160P24 = 61,
    HDMI_VIDEO_TIMING_3840X2160P25 = 62,
    HDMI_VIDEO_TIMING_3840X2160P30 = 63,
    HDMI_VIDEO_TIMING_3840X2160P48 = 64,
    HDMI_VIDEO_TIMING_3840X2160P50 = 65,
    HDMI_VIDEO_TIMING_3840X2160P60 = 66,
    HDMI_VIDEO_TIMING_3840X2160P100 = 67,
    HDMI_VIDEO_TIMING_3840X2160P120 = 68,
    HDMI_VIDEO_TIMING_4096X2160P24 = 69,
    HDMI_VIDEO_TIMING_4096X2160P25 = 70,
    HDMI_VIDEO_TIMING_4096X2160P30 = 71,
    HDMI_VIDEO_TIMING_4096X2160P48 = 72,
    HDMI_VIDEO_TIMING_4096X2160P50 = 73,
    HDMI_VIDEO_TIMING_4096X2160P60 = 74,
    HDMI_VIDEO_TIMING_4096X2160P100 = 75,
    HDMI_VIDEO_TIMING_4096X2160P120 = 76,
    HDMI_VIDEO_TIMING_5120X2160P24 = 77,
    HDMI_VIDEO_TIMING_5120X2160P25 = 78,
    HDMI_VIDEO_TIMING_5120X2160P30 = 79,
    HDMI_VIDEO_TIMING_5120X2160P48 = 80,
    HDMI_VIDEO_TIMING_5120X2160P50 = 81,
    HDMI_VIDEO_TIMING_5120X2160P60 = 82,
    HDMI_VIDEO_TIMING_5120X2160P100 = 83,
    HDMI_VIDEO_TIMING_5120X2160P120 = 84,
    HDMI_VIDEO_TIMING_7680X4320P24 = 85,
    HDMI_VIDEO_TIMING_7680X4320P25 = 86,
    HDMI_VIDEO_TIMING_7680X4320P30 = 87,
    HDMI_VIDEO_TIMING_7680X4320P48 = 88,
    HDMI_VIDEO_TIMING_7680X4320P50 = 89,
    HDMI_VIDEO_TIMING_7680X4320P60 = 90,
    HDMI_VIDEO_TIMING_7680X4320P100 = 91,
    HDMI_VIDEO_TIMING_7680X4320P120 = 92,
    HDMI_VIDEO_TIMING_10240X4320P24 = 93,
    HDMI_VIDEO_TIMING_10240X4320P25 = 94,
    HDMI_VIDEO_TIMING_10240X4320P30 = 95,
    HDMI_VIDEO_TIMING_10240X4320P48 = 96,
    HDMI_VIDEO_TIMING_10240X4320P50 = 97,
    HDMI_VIDEO_TIMING_10240X4320P60 = 98,
    HDMI_VIDEO_TIMING_10240X4320P100 = 99,
    HDMI_VIDEO_TIMING_10240X4320P120 = 100,
    HDMI_VIDEO_TIMING_VESA_DEFINE = 101,
    HDMI_VIDEO_TIMING_VESA_800X600_60 = 102,
    HDMI_VIDEO_TIMING_VESA_848X480_60 = 103,
    HDMI_VIDEO_TIMING_VESA_1024X768_60 = 104,
    HDMI_VIDEO_TIMING_VESA_1280X720_60 = 105,
    HDMI_VIDEO_TIMING_VESA_1280X768_60 = 106,
    HDMI_VIDEO_TIMING_VESA_1280X768_60_RB = 107,
    HDMI_VIDEO_TIMING_VESA_1280X800_60 = 108,
    HDMI_VIDEO_TIMING_VESA_1280X800_60_RB = 109,
    HDMI_VIDEO_TIMING_VESA_1280X960_60 = 110,
    HDMI_VIDEO_TIMING_VESA_1280X1024_60 = 111,
    HDMI_VIDEO_TIMING_VESA_1360X768_60 = 112,
    HDMI_VIDEO_TIMING_VESA_1366X768_60 = 113,
    HDMI_VIDEO_TIMING_VESA_1400X1050_60 = 114,
    HDMI_VIDEO_TIMING_VESA_1440X900_60 = 115,
    HDMI_VIDEO_TIMING_VESA_1440X900_60_RB = 116,
    HDMI_VIDEO_TIMING_VESA_1440X1050_60 = 117,
    HDMI_VIDEO_TIMING_VESA_1440X1050_60_RB = 118,
    HDMI_VIDEO_TIMING_VESA_1600X900_60_RB = 119,
    HDMI_VIDEO_TIMING_VESA_1600X1200_60 = 120,
    HDMI_VIDEO_TIMING_VESA_1680X1050_60 = 113,
    HDMI_VIDEO_TIMING_VESA_1680X1050_60_RB = 114,
    HDMI_VIDEO_TIMING_VESA_1792X1344_60 = 115,
    HDMI_VIDEO_TIMING_VESA_1856X1392_60 = 116,
    HDMI_VIDEO_TIMING_VESA_1920X1080_60 = 117,
    HDMI_VIDEO_TIMING_VESA_1920X1200_60 = 118,
    HDMI_VIDEO_TIMING_VESA_1920X1200_60_RB = 119,
    HDMI_VIDEO_TIMING_VESA_1920X1440_60 = 120,
    HDMI_VIDEO_TIMING_VESA_2048X1152_60 = 121,
    HDMI_VIDEO_TIMING_VESA_2560X1440_60_RB = 122,
    HDMI_VIDEO_TIMING_VESA_2560X1600_60 = 123,
    HDMI_VIDEO_TIMING_VESA_2560X1600_60_RB = 124,
    HDMI_VIDEO_TIMING_USER_DEFINE = 125,
    HDMI_VIDEO_TIMING_USER_1920X2160_30 = 126,
    HDMI_VIDEO_TIMING_USER_2560X1440_30 = 127,
    HDMI_VIDEO_TIMING_USER_2560X1440_60 = 128,
    HDMI_VIDEO_TIMING_USER_1280X720_60 = 129,
    HDMI_VIDEO_TIMING_USER_1366X768_60 = 130,
    HDMI_VIDEO_TIMING_USER_1600X900_60_RB = 131,
    HDMI_VIDEO_TIMING_USER_1920X1080_60 = 132,
    HDMI_VIDEO_TIMING_USER_2048X1152_60 = 133,
    HDMI_VIDEO_TIMING_BUTT,
};

/**
 * @brief Enumerates picture aspect ratio,
 * see the section 8.2.1 of HDMI Specification 1.4 for details.
 *
 * @since 1.0
 */
enum HdmiPictureAspectRatio {
    HDMI_PICTURE_ASPECT_NO_DATA = 0,
    HDMI_PICTURE_ASPECT_4_3 = 1,
    HDMI_PICTURE_ASPECT_16_9 = 2,
    HDMI_PICTURE_ASPECT_64_27 = 3,
    HDMI_PICTURE_ASPECT_256_135 = 4,
    HDMI_PICTURE_ASPECT_BUTT = 5,
};

/**
 * @brief Enumerates active format aspect ratio,
 * see the section 8.2.1 of HDMI Specification 1.4 for details.
 *
 * @since 1.0
 */
enum HdmiActiveFormatAspectRatio {
    HDMI_ACTIVE_FORMAT_ASPECT_16_9_TOP = 2,
    HDMI_ACTIVE_FORMAT_ASPECT_14_9_TOP = 3,
    HDMI_ACTIVE_FORMAT_ASPECT_16_9_CENTER = 4,
    HDMI_ACTIVE_FORMAT_ASPECT_PICTURE = 8,
    HDMI_ACTIVE_FORMAT_ASPECT_4_3 = 9,
    HDMI_ACTIVE_FORMAT_ASPECT_16_9 = 10,
    HDMI_ACTIVE_FORMAT_ASPECT_14_9 = 11,
    HDMI_ACTIVE_FORMAT_ASPECT_4_3_SP_14_9 = 13,
    HDMI_ACTIVE_FORMAT_ASPECT_16_9_SP_14_9 = 14,
    HDMI_ACTIVE_FORMAT_ASPECT_16_9_SP_4_3 = 15,
    HDMI_ACTIVE_FORMAT_ASPECT_BUTT,
};

/**
 * @brief Enumerates Nups(non-uniform picture scaling),
 * see the section 8.2.1 of HDMI Specification 1.4 for details.
 *
 * @since 1.0
 */
enum HdmiNups {
    HDMI_NUPS_UNKNOWN = 0,      /* No Known non-uniform scaling */
    HDMI_NUPS_HORIZONTAL = 1,   /* Picture has been scaled horizontally */
    HDMI_NUPS_VERTICAL = 2,     /* Picture has been scaled vertically */
    HDMI_NUPS_BOTH = 3,         /* Picture has been scaled horizontally and vertically */
};

/**
 * @brief Defines the video attribute struct.
 *
 * @since 1.0
 */
struct HdmiVideoAttr {
    uint32_t tmdsClock;  /* unit: KHz */
    uint32_t pixelClock; /* unit: KHz */
    uint32_t pixelRepeat;
    enum HdmiColorSpace colorSpace;
    enum HdmiColorimetry colorimetry;
    enum HdmiExtendedColorimetry extColorimetry;
    enum HdmiQuantizationRange quantization;
    enum HdmiYccQuantizationRange yccQuantization;
    enum HdmiDeepColor deepColor;
    enum HdmiVideo3dStructure _3dStruct;
    enum HdmiVideoTiming timing;
    enum HdmiPictureAspectRatio aspect;
    enum HdmiActiveFormatAspectRatio activeAspect;
    enum HdmiNups nups;
    bool xvycc;
};

/**
 * @brief Enumerates audio coding type,
 * see the section 7 of HDMI Specification 1.4 for details.
 *
 * @since 1.0
 */
enum HdmiAudioCodingType {
    HDMI_AUDIO_CODING_TYPE_STREAM = 0,
    HDMI_AUDIO_CODING_TYPE_LPCM = 1,
    HDMI_AUDIO_CODING_TYPE_AC3 = 2,
    HDMI_AUDIO_CODING_TYPE_MPEG1 = 3,
    HDMI_AUDIO_CODING_TYPE_MP3 = 4,
    HDMI_AUDIO_CODING_TYPE_MPEG2 = 5,
    HDMI_AUDIO_CODING_TYPE_AAC_LC = 6,
    HDMI_AUDIO_CODING_TYPE_DTS = 7,
    HDMI_AUDIO_CODING_TYPE_ATRAC = 8,
    HDMI_AUDIO_CODING_TYPE_OBA = 9,
    HDMI_AUDIO_CODING_TYPE_EAC3 = 10,
    HDMI_AUDIO_CODING_TYPE_DTS_HD = 11,
    HDMI_AUDIO_CODING_TYPE_MLP = 12,
    HDMI_AUDIO_CODING_TYPE_DST = 13,
    HDMI_AUDIO_CODING_TYPE_WMA_PRO = 14,
    HDMI_AUDIO_CODING_TYPE_CXT = 15,
};

/**
 * @brief Enumerates audio interface type.
 *
 * @since 1.0
 */
enum HdmiAudioInterfaceType {
    HDMI_AUDIO_IF_TYPE_I2S = 0,    /* Inter-IC Sound */
    HDMI_AUDIO_IF_TYPE_SPDIF = 1,  /* Sony/Philips Digital Interface */
    HDMI_AUDIO_IF_TYPE_OTHER,
};

/**
 * @brief Enumerates audio bit depth,
 * see the section 7 of HDMI Specification 1.4 for details.
 *
 * @since 1.0
 */
enum HdmiAudioBitDepth {
    HDMI_ADIO_BIT_DEPTH_UNKNOWN,
    HDMI_ADIO_BIT_DEPTH_8 = 8,
    HDMI_ADIO_BIT_DEPTH_16 = 16,
    HDMI_ADIO_BIT_DEPTH_18 = 18,
    HDMI_ADIO_BIT_DEPTH_20 = 20,
    HDMI_ADIO_BIT_DEPTH_24 = 24,
    HDMI_ADIO_BIT_DEPTH_32 = 32,
    HDMI_ADIO_BIT_DEPTH_BUTT,
};

/**
 * @brief Enumerates sample rate used for audio,
 * see the section 7.3 of HDMI Specification 1.4 for details.
 *
 * @since 1.0
 */
enum HdmiSampleRate {
    HDMI_SAMPLE_RATE_UNKNOWN,
    HDMI_SAMPLE_RATE_8K = 8000,
    HDMI_SAMPLE_RATE_11K = 11025,
    HDMI_SAMPLE_RATE_12K = 12000,
    HDMI_SAMPLE_RATE_16K = 16000,
    HDMI_SAMPLE_RATE_22K = 22050,
    HDMI_SAMPLE_RATE_24K = 24000,
    HDMI_SAMPLE_RATE_32K = 32000,
    HDMI_SAMPLE_RATE_44K = 44100,
    HDMI_SAMPLE_RATE_48K = 48000,
    HDMI_SAMPLE_RATE_88K = 88200,
    HDMI_SAMPLE_RATE_96K = 96000,
    HDMI_SAMPLE_RATE_176K = 176400,
    HDMI_SAMPLE_RATE_192K = 192000,
    HDMI_SAMPLE_RATE_768K = 768000,
    HDMI_SAMPLE_RATE_BUTT,
};

/**
 * @brief Enumerates audio format channel,
 * see the section 7 of HDMI Specification 1.4 for details.
 *
 * @since 1.0
 */
enum HdmiAudioFormatChannel {
    HDMI_AUDIO_FORMAT_CHANNEL_2 = 2,
    HDMI_AUDIO_FORMAT_CHANNEL_3,
    HDMI_AUDIO_FORMAT_CHANNEL_4,
    HDMI_AUDIO_FORMAT_CHANNEL_5,
    HDMI_AUDIO_FORMAT_CHANNEL_6,
    HDMI_AUDIO_FORMAT_CHANNEL_7,
    HDMI_AUDIO_FORMAT_CHANNEL_8,
    HDMI_AUDIO_FORMAT_CHANNEL_BUTT,
};

/**
 * @brief Defines the audio attribute struct.
 *
 * @since 1.0
 */
struct HdmiAudioAttr {
    enum HdmiAudioCodingType codingType;
    enum HdmiAudioInterfaceType ifType;
    enum HdmiAudioBitDepth bitDepth;
    enum HdmiSampleRate sampleRate;
    bool downSample;
    enum HdmiAudioFormatChannel channels;
};

/**
 * @brief Enumerates Electro-Optical Transfer Function (EOTF),
 * see the section 6.9 of CTA-861-G for details.
 *
 * @since 1.0
 */
enum HdmiEotfType {
    HDMI_EOTF_SDR_LUMIN = 0,        /* Traditional gamma - SDR Luminance Range */
    HDMI_EOTF_HDR_LUMIN = 1,        /* Traditional gamma - HDR Luminance Range */
    HDMI_EOTF_SMPTE_ST_2048 = 2,    /* SMPTE ST 2048 */
    HDMI_EOTF_HLG = 3,              /* Hybrid Log-Gamma (HLG) based on ITU-R BT.2100-0 */
    HDMI_EOTF_BUTT,
};

/**
 * @brief Enumerates hdr colormetry,
 * see HDMI Specification 2.1 for details.
 *
 * @since 1.0
 */
enum HdmiHdrColormetry {
    HDMI_HDR_COLORIMETRY_NONE,
    HDMI_HDR_COLORIMETRY_ITU_601,
    HDMI_HDR_COLORIMETRY_ITU_709,
    HDMI_HDR_COLORIMETRY_EXTENDED,
    HDMI_HDR_EXTENDED_COLORIMETRY_XV_YCC_601,
    HDMI_HDR_EXTENDED_COLORIMETRY_XV_YCC_709,
    HDMI_HDR_EXTENDED_COLORIMETRY_S_YCC_601,
    HDMI_HDR_EXTENDED_COLORIMETRY_ADOBE_YCC_601,
    HDMI_HDR_EXTENDED_COLORIMETRY_ADOBE_RGB,
    HDMI_HDR_EXTENDED_COLORIMETRY_2020_CONST_LUMINOUS, /* BT2020 c_ycc */
    HDMI_HDR_EXTENDED_COLORIMETRY_2020_NON_CONST_LUMINOUW
};

/**
 * @brief Enumerates hdr mode,
 * see HDMI Specification 2.1 for details.
 *
 * @since 1.0
 */
enum HdmiHdrMode {
    HDMI_HDR_MODE_DISABLE,          /* HDR & dolby mode disable */
    HDMI_HDR_MODE_DOLBY_NORMAL,     /* dolby normal(ycbcr422-12bit) mode enable */
    HDMI_HDR_MODE_DOLBY_TUNNELING,  /* dolby tunneling(RGB-8bit) mode enable */
    HDMI_HDR_MODE_CEA_861_3,        /* HDR standard mode enable(according to <CEA-861-3.2015>) */
    HDMI_HDR_MODE_CEA_861_3_AUTHEN, /* HDR authen mode */
    HDMI_HDR_MODE_BUTT
};

/**
 * @brief Enumerates hdr user mode,
 * see HDMI Specification 2.1 for details.
 *
 * @since 1.0
 */
enum HdmiHdrUserMode {
    HDMI_HDR_USERMODE_SDR,
    HDMI_HDR_USERMODE_HDR10,
    HDMI_HDR_USERMODE_DOLBY,
    HDMI_HDR_USERMODE_BUTT
};

/**
 * @brief Enumerates static metadata type,
 * see the section 6.9 of CTA-861-G for details.
 *
 * @since 1.0
 */
enum HdmiStaticMetadataType {
    HDMI_DRM_STATIC_METADATA_TYPE_1 = 0, /* Static Metadata Type 1 */
    HDMI_DRM_STATIC_METADATA_BUTT,
};

/**
 * @brief Enumerates static metadata descriptor1st,
 * see the section 6.9 of CTA-861-G for details.
 *
 * @since 1.0
 */
struct HdmiStaticMetadataDescriptor1st {
    uint16_t displayPrimaries0X;            /* display_primaries_x[0], in units of 0.00002 */
    uint16_t displayPrimaries0Y;            /* display_primaries_y[0], in units of 0.00002 */
    uint16_t displayPrimaries1X;            /* display_primaries_x[1], in units of 0.00002 */
    uint16_t displayPrimaries1Y;            /* display_primaries_y[1], in units of 0.00002 */
    uint16_t displayPrimaries2X;            /* display_primaries_x[2], in units of 0.00002 */
    uint16_t displayPrimaries2Y;            /* display_primaries_y[2], in units of 0.00002 */
    uint16_t whitePointX;                   /* white_point_x, in units of 0.00002 */
    uint16_t whitePointY;                   /* white_point_y, in units of 0.00002 */
    uint16_t maxDisplayMasteringLuminance;  /* max_display_mastering_luminance, in units of 1 cd/m^2 */
    uint16_t minDisplayMasteringLuminance;  /* min_display_mastering_luminance, in units of 0.0001 cd/m^2 */
    uint16_t maxContentLightLevel;          /* Maximum Content Light Level, in units of 1 cd/m^2 */
    uint16_t maxFrameAverageLightLevel;     /* Maximum Frame-average Light Level, in units of 1 cd/m^2 */
};

/**
 * @brief Defines the static metadata descriptor,
 * see the section 6.9 of CTA-861-G for details.
 *
 * @since 1.0
 */
union HdmiStaticMetadataDescriptor {
    struct HdmiStaticMetadataDescriptor1st type1;
};

/**
 * @brief Defines the hdr attribute struct.
 *
 * @since 1.0
 */
struct HdmiHdrAttr {
    enum HdmiHdrMode mode;
    enum HdmiHdrUserMode userMode;
    /* The following members are valid, when mode is HDMI_HDR_MODE_CEA_861_3. */
    enum HdmiEotfType eotfType;
    enum HdmiStaticMetadataType metadataType;
    union HdmiStaticMetadataDescriptor descriptor;
    enum HdmiHdrColormetry colorimetry;
};

/**
 * @brief Defines the hdmi HPD(Hot Plug Detect) callback info.
 *
 * @since 1.0
 */
struct HdmiHpdCallbackInfo {
    void *data;
    void (*callbackFunc)(void *data, bool hdp);
};

/**
 * @brief Opens an HDMI controller with a specified bus number.
 *
 * Before using the HDMI interface, you can obtain the device handle of the HDMI controller
 * by calling {@link HdmiOpen}. This function is used in pair with {@link HdmiClose}.
 *
 * @param busNum Indicates the bus number.
 *
 * @return Returns the device handle {@link DevHandle} of the HDMI controller if the operation is successful;
 * returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 */
DevHandle HdmiOpen(uint16_t busNum);

/**
 * @brief HDMI transmission start.
 *
 * This function is used in pair with {@link HdmiStop}.
 *
 * @param handle Indicates the pointer to the device handle of the HDMI controller.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value if the operation fails.
 *
 * @since 1.0
 */
int32_t HdmiStart(DevHandle handle);

/**
 * @brief HDMI transmission stop.
 *
 * This function is used in pair with {@link HdmiStart}.
 *
 * @param handle Indicates the pointer to the device handle of the HDMI controller.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value if the operation fails.
 *
 * @since 1.0
 */
int32_t HdmiStop(DevHandle handle);

/**
 * @brief HDMI Avmute set.
 *
 * First put the display device into the black screen mute state, waitting for the HDMI output device switch,
 * and then send a Clear AVMute signal, so that the display device to boot, so that the switching process of
 * the screen will be shielded.
 *
 * @param handle Indicates the pointer to the device handle of the HDMI controller.
 * @param enable Indicates whether to enable avmute.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value if the operation fails.
 *
 * @since 1.0
 */
int32_t HdmiAvmuteSet(DevHandle handle, bool enable);


/**
 * @brief HDMI deep color set.
 *
 * @param handle Indicates the pointer to the device handle of the HDMI controller.
 * @param color Indicates the deep color to be set, see {@link HdmiDeepColor}.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value if the operation fails.
 *
 * @since 1.0
 */
int32_t HdmiDeepColorSet(DevHandle handle, enum HdmiDeepColor color);

/**
 * @brief HDMI deep color get.
 *
 * @param handle Indicates the pointer to the device handle of the HDMI controller.
 * @param color Indicates the pointer to the deep color to read, see {@link HdmiDeepColor}.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value if the operation fails.
 *
 * @since 1.0
 */
int32_t HdmiDeepColorGet(DevHandle handle, enum HdmiDeepColor *color);

/**
 * @brief HDMI set video attribute.
 *
 * @param handle Indicates the pointer to the device handle of the HDMI controller.
 * @param attr Indicates the pointer to the video attribute to set, see {@link HdmiVideoAttr}.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value if the operation fails.
 *
 * @since 1.0
 */
int32_t HdmiSetVideoAttribute(DevHandle handle, struct HdmiVideoAttr *attr);

/**
 * @brief HDMI set audio attribute.
 *
 * @param handle Indicates the pointer to the device handle of the HDMI controller.
 * @param attr Indicates the pointer to the audio attribute to set, see {@link HdmiAudioAttr}.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value if the operation fails.
 *
 * @since 1.0
 */
int32_t HdmiSetAudioAttribute(DevHandle handle, struct HdmiAudioAttr *attr);

/**
 * @brief HDMI set hdr attribute.
 *
 * @param handle Indicates the pointer to the device handle of the HDMI controller.
 * @param attr Indicates the pointer to the hdr attribute to set, see {@link HdmiHdrAttr}.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value if the operation fails.
 *
 * @since 1.0
 */
int32_t HdmiSetHdrAttribute(DevHandle handle, struct HdmiHdrAttr *attr);

/**
 * @brief HDMI read sink device's raw EDID data.
 *
 * @param handle Indicates the pointer to the device handle of the HDMI controller.
 * @param buffer Indicates the pointer to the data to read.
 * @param len Indicates the length of the data to read.
 *
 * @return Returns the length of the data read if the operation is successful;
 * returns a negative value or 0 if the operation fails.
 *
 * @since 1.0
 */
int32_t HdmiReadSinkEdid(DevHandle handle, uint8_t *buffer, uint32_t len);

/**
 * @brief HDMI register HPD(Hot Plug Detect) callback function.
 *
 * @param handle Indicates the pointer to the device handle of the HDMI controller.
 * @param callback Indicates the pointer to the callback info.
 *
 * @return Returns the length of the data read if the operation is successful;
 * returns a negative value or 0 if the operation fails.
 *
 * @since 1.0
 */
int32_t HdmiRegisterHpdCallbackFunc(DevHandle handle, struct HdmiHpdCallbackInfo *callback);

/**
 * @brief HDMI unregister HPD(Hot Plug Detect) callback function.
 *
 * @param handle Indicates the pointer to the device handle of the HDMI controller.
 *
 * @return Returns the length of the data read if the operation is successful;
 * returns a negative value or 0 if the operation fails.
 *
 * @since 1.0
 */
int32_t HdmiUnregisterHpdCallbackFunc(DevHandle handle);

/**
 * @brief Closes an HDMI controller.
 *
 * After the HDMI interface is used, you can close the HDMI controller by calling {@link HdmiClose}.
 * This function is used in pair with {@link HdmiOpen}.
 *
 * @param handle Indicates the pointer to the device handle of the HDMI controller.
 *
 * @since 1.0
 */
void HdmiClose(DevHandle handle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HDMI_IF_H */
