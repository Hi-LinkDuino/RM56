/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDMI_COMMON_H
#define HDMI_COMMON_H

#include "hdf_base.h"
#include "hdmi_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define HDMI_CEA_VIDEO_CODE_MAX 44
#define HDMI_VESA_VIDEO_CODE_MAX 31
#define HDMI_VIDEO_4K_CODES_MAX 4

#define HDMI_BIT0_MARK 0x01
#define HDMI_BIT1_MARK 0x02
#define HDMI_BIT2_MARK 0x04
#define HDMI_BIT3_MARK 0x08
#define HDMI_BIT4_MARK 0x10
#define HDMI_BIT5_MARK 0x20
#define HDMI_BIT6_MARK 0x40
#define HDMI_BIT7_MARK 0x80
#define HDMI_UPPER_NIBBLE_MARK 0xf0
#define HDMI_LOWER_NIBBLE_MARK 0x0f
#define HDMI_BITS_OF_ONE_BYTE 8
#define HDMI_ONE_BYTE_SHIFT 8
#define HDMI_6_BITS_SHIFT 6
#define HDMI_NIBBLE_SHIFT 4
#define HDMI_2_BITS_SHIFT 2
#define HDMI_ONE_BYTE_MARK 0xFF
#define HDMI_TWO_BYTES_SHIFT 16

#define HDMI_HDMI14_MAX_TMDS_RATE 340000  /* unit: KHz */
#define HDMI_HDMI20_MAX_TMDS_RATE 600000  /* unit: KHz */
#define HDMI_MULTIPLE_2P0(x)    (2 * (x))
#define HDMI_MULTIPLE_1P5(x)    (((x) * 3) >> 1)
#define HDMI_MULTIPLE_1P25(x)   (((x) * 5) >> 2)
#define HDMI_MULTIPLE_0P5(x)    ((x) >> 1)

enum HdmiTmdsModeType {
    HDMI_TMDS_MODE_NULL = 0,
    HDMI_TMDS_MODE_DVI = 1,
    HDMI_TMDS_MODE_HDMI_1_4 = 2,
    HDMI_TMDS_MODE_HDMI_2_0 = 3,
    HDMI_TMDS_MODE_AUTO = 4,
    HDMI_TMDS_MODE_HDMI_2_1 = 5,
    HDMI_TMDS_MODE_BUTT,
};

/*
 * Video Identification Code (VIC).
 * see CTA-861-G, table 3.
 */
enum HdmiVic {
    HDMI_VIC_NONE = 0,    /* No Video Identification Code Available(Used with AVI InfoFrame only) */
    HDMI_VIC_640X480P60_4_3 = 1,
    HDMI_VIC_720X480P60_4_3 = 2,
    HDMI_VIC_720X480P60_16_9 = 3,
    HDMI_VIC_1280X720P60_16_9 = 4,
    HDMI_VIC_1920X1080I60_16_9 = 5,
    HDMI_VIC_1440X480I60_4_3 = 6,
    HDMI_VIC_1440X480I60_16_9 = 7,
    HDMI_VIC_1440X240P60_4_3 = 8,
    HDMI_VIC_1440X240P60_16_9 = 9,
    HDMI_VIC_2880X480I60_4_3 = 10,
    HDMI_VIC_2880X480I60_16_9 = 11,
    HDMI_VIC_2880X240P60_4_3 = 12,
    HDMI_VIC_2880X240P60_16_9 = 13,
    HDMI_VIC_1440X480P60_4_3 = 14,
    HDMI_VIC_1440X480P60_16_9 = 15,
    HDMI_VIC_1920X1080P60_16_9 = 16,
    HDMI_VIC_720X576P50_4_3 = 17,
    HDMI_VIC_720X576P50_16_9 = 18,
    HDMI_VIC_1280X720P50_16_9 = 19,
    HDMI_VIC_1920X1080I50_16_9 = 20,
    HDMI_VIC_1440X576I50_4_3 = 21,
    HDMI_VIC_1440X576I50_16_9 = 22,
    HDMI_VIC_1440X288P50_4_3 = 23,
    HDMI_VIC_1440X288P50_16_9 = 24,
    HDMI_VIC_2880X576I50_4_3 = 25,
    HDMI_VIC_2880X576I50_16_9 = 26,
    HDMI_VIC_2880X288P50_4_3 = 27,
    HDMI_VIC_2880X288P50_16_9 = 28,
    HDMI_VIC_1440X576P50_4_3 = 29,
    HDMI_VIC_1440X576P50_16_9 = 30,
    HDMI_VIC_1920X1080P50_16_9 = 31,
    HDMI_VIC_1920X1080P24_16_9 = 32,
    HDMI_VIC_1920X1080P25_16_9 = 33,
    HDMI_VIC_1920X1080P30_16_9 = 34,
    HDMI_VIC_2880X480P60_4_3 = 35,
    HDMI_VIC_2880X480P60_16_9 = 36,
    HDMI_VIC_2880X576P50_4_3 = 37,
    HDMI_VIC_2880X576P50_16_9 = 38,
    HDMI_VIC_1920X1080I50_1250_16_9 = 39,
    HDMI_VIC_1920X1080I100_16_9 = 40,
    HDMI_VIC_1280X720P100_16_9 = 41,
    HDMI_VIC_720X576P100_4_3 = 42,
    HDMI_VIC_720X576P100_16_9 = 43,
    HDMI_VIC_1440X576I100_4_3 = 44,
    HDMI_VIC_1440X576I100_16_9 = 45,
    HDMI_VIC_1920X1080I120_16_9 = 46,
    HDMI_VIC_1280X720P120_16_9 = 47,
    HDMI_VIC_720X480P120_4_3 = 48,
    HDMI_VIC_720X480P120_16_9 = 49,
    HDMI_VIC_1440X480I120_4_3 = 50,
    HDMI_VIC_1440X480I120_16_9 = 51,
    HDMI_VIC_720X576P200_4_3 = 52,
    HDMI_VIC_720X576P200_16_9 = 53,
    HDMI_VIC_1440X576I120_4_3 = 54,
    HDMI_VIC_1440X576I120_16_9 = 55,
    HDMI_VIC_720X480P240_4_3 = 56,
    HDMI_VIC_720X480P240_16_9 = 57,
    HDMI_VIC_1440X480I240_4_3 = 58,
    HDMI_VIC_1440X480I240_16_9 = 59,
    HDMI_VIC_1280X720P24_16_9 = 60,
    HDMI_VIC_1280X720P25_16_9 = 61,
    HDMI_VIC_1280X720P30_16_9 = 62,
    HDMI_VIC_1920X1080P120_16_9 = 63,
    HDMI_VIC_1920X1080P100_16_9 = 64,
    HDMI_VIC_1280X720P24_64_27 = 65,
    HDMI_VIC_1280X720P25_64_27 = 66,
    HDMI_VIC_1280X720P30_64_27 = 67,
    HDMI_VIC_1280X720P50_64_27 = 68,
    HDMI_VIC_1280X720P60_64_27 = 69,
    HDMI_VIC_1280X720P100_64_27 = 70,
    HDMI_VIC_1280X720P120_64_27 = 71,
    HDMI_VIC_1920X1080P24_64_27 = 72,
    HDMI_VIC_1920X1080P25_64_27 = 73,
    HDMI_VIC_1920X1080P30_64_27 = 74,
    HDMI_VIC_1920X1080P50_64_27 = 75,
    HDMI_VIC_1920X1080P60_64_27 = 76,
    HDMI_VIC_1920X1080P100_64_27 = 77,
    HDMI_VIC_1920X1080P120_64_27 = 78,
    HDMI_VIC_1680X720P24_64_27 = 79,
    HDMI_VIC_1680X720P25_64_27 = 80,
    HDMI_VIC_1680X720P30_64_27 = 81,
    HDMI_VIC_1680X720P50_64_27 = 82,
    HDMI_VIC_1680X720P60_64_27 = 83,
    HDMI_VIC_1680X720P100_64_27 = 84,
    HDMI_VIC_1680X720P120_64_27 = 85,
    HDMI_VIC_2560X1080P24_64_27 = 86,
    HDMI_VIC_2560X1080P25_64_27 = 87,
    HDMI_VIC_2560X1080P30_64_27 = 88,
    HDMI_VIC_2560X1080P50_64_27 = 89,
    HDMI_VIC_2560X1080P60_64_27 = 90,
    HDMI_VIC_2560X1080P100_64_27 = 91,
    HDMI_VIC_2560X1080P120_64_27 = 92,
    HDMI_VIC_3840X2160P24_16_9 = 93,
    HDMI_VIC_3840X2160P25_16_9 = 94,
    HDMI_VIC_3840X2160P30_16_9 = 95,
    HDMI_VIC_3840X2160P50_16_9 = 96,
    HDMI_VIC_3840X2160P60_16_9 = 97,
    HDMI_VIC_4096X2160P24_256_135 = 98,
    HDMI_VIC_4096X2160P25_256_135 = 99,
    HDMI_VIC_4096X2160P30_256_135 = 100,
    HDMI_VIC_4096X2160P50_256_135 = 101,
    HDMI_VIC_4096X2160P60_256_135 = 102,
    HDMI_VIC_3840X2160P24_64_27 = 103,
    HDMI_VIC_3840X2160P25_64_27 = 104,
    HDMI_VIC_3840X2160P30_64_27 = 105,
    HDMI_VIC_3840X2160P50_64_27 = 106,
    HDMI_VIC_3840X2160P60_64_27 = 107,
    HDMI_VIC_1280X720P48_16_9 = 108,
    HDMI_VIC_1280X720P48_64_27 = 109,
    HDMI_VIC_1680X720P48_64_27 = 110,
    HDMI_VIC_1920X1080P48_16_9 = 111,
    HDMI_VIC_1920X1080P48_64_27 = 112,
    HDMI_VIC_2560X1080P48_64_27 = 113,
    HDMI_VIC_3840X2160P48_16_9 = 114,
    HDMI_VIC_4096X2160P48_256_135 = 115,
    HDMI_VIC_3840X2160P48_64_27 = 116,
    HDMI_VIC_3840X2160P100_16_9 = 117,
    HDMI_VIC_3840X2160P120_16_9 = 118,
    HDMI_VIC_3840X2160P100_64_27 = 119,
    HDMI_VIC_3840X2160P120_64_27 = 120,
    HDMI_VIC_5120X2160P24_64_27 = 121,
    HDMI_VIC_5120X2160P25_64_27 = 122,
    HDMI_VIC_5120X2160P30_64_27 = 123,
    HDMI_VIC_5120X2160P48_64_27 = 124,
    HDMI_VIC_5120X2160P50_64_27 = 125,
    HDMI_VIC_5120X2160P60_64_27 = 126,
    HDMI_VIC_5120X2160P100_64_27 = 127,
    HDMI_VIC_5120X2160P120_64_27 = 193,
    HDMI_VIC_7680X4320P24_16_9 = 194,
    HDMI_VIC_7680X4320P25_16_9 = 195,
    HDMI_VIC_7680X4320P30_16_9 = 196,
    HDMI_VIC_7680X4320P48_16_9 = 197,
    HDMI_VIC_7680X4320P50_16_9 = 198,
    HDMI_VIC_7680X4320P60_16_9 = 199,
    HDMI_VIC_7680X4320P100_16_9 = 200,
    HDMI_VIC_7680X4320P120_16_9 = 201,
    HDMI_VIC_7680X4320P24_64_27 = 202,
    HDMI_VIC_7680X4320P25_164_27 = 203,
    HDMI_VIC_7680X4320P30_64_27 = 204,
    HDMI_VIC_7680X4320P48_64_27 = 205,
    HDMI_VIC_7680X4320P50_64_27 = 206,
    HDMI_VIC_7680X4320P60_64_27 = 207,
    HDMI_VIC_7680X4320P100_64_27 = 208,
    HDMI_VIC_7680X4320P120_64_27 = 209,
    HDMI_VIC_10240X4320P24_64_27 = 210,
    HDMI_VIC_10240X4320P25_64_27 = 211,
    HDMI_VIC_10240X4320P30_64_27 = 212,
    HDMI_VIC_10240X4320P48_64_27 = 213,
    HDMI_VIC_10240X4320P50_64_27 = 214,
    HDMI_VIC_10240X4320P60_64_27 = 215,
    HDMI_VIC_10240X4320P100_64_27 = 216,
    HDMI_VIC_10240X4320P120_64_27 = 217,
    HDMI_VIC_4096X2160P100_256_135 = 218,
    HDMI_VIC_4096X2160P120_256_135 = 219,
};

enum HdmiVideoFormatType {
    HDMI_VIDEO_FORMAT_NULL = 0,
    HDMI_VIDEO_FORMAT_PROGRESSIVE = 1,
    HDMI_VIDEO_FORMA_INTERLACE = 2,
    HDMI_VIDEO_FORMA_BUTT,
};

enum HdmiColorDeep {
    HDMI_COLOR_DEEP_8BITS = 8,
    HDMI_COLOR_DEEP_10BITS = 10,
    HDMI_COLOR_DEEP_12BITS = 12,
    HDMI_COLOR_DEEP_16BITS = 16,
    HDMI_COLOR_DEEP_BUTT,
};

enum HdmiPhyModeCfg {
    HDMI_PHY_MODE_TMDS = 0,
    HDMI_PHY_MODE_FRL = 1,
    HDMI_PHY_MODE_TXFFE = 2,
    HDMI_PHY_MODE_BUTT,
};

enum HdmiHdcpMode {
    HDMI_HDCP_MODE_AUTO = 0,
    HDMI_HDCP_MODE_1_4 = 1,
    HDMI_HDCP_MODE_2_2 = 2,
    HDMI_HDCP_MODE_BUTT
};

struct HdmiVideoDefInfo {
    enum HdmiVic vic;
    uint32_t pixclk;
    uint32_t rate;
    uint32_t hactive;
    uint32_t vactive;
    uint32_t hblank;
    uint32_t vblank;
    uint32_t hfront;
    uint32_t hsync;
    uint32_t hback;
    uint32_t vfront;
    uint32_t vsync;
    uint32_t vback;
    enum HdmiPictureAspectRatio aspect;
    enum HdmiVideoTiming timing;
    enum HdmiVideoFormatType formatType;
};

/* see hdmi spec2.0 table 10-2. */
enum Hdmi4kVic {
    HDMI_4K_VIC_3840X2160_30 = 1,
    HDMI_4K_VIC_3840X2160_25,
    HDMI_4K_VIC_3840X2160_24,
    HDMI_4K_VIC_4096X2160_24
};

struct HdmiVideo4kInfo {
    enum Hdmi4kVic _4kVic;
    enum HdmiVic vic;
    uint32_t pixclk;
    uint32_t rate;
    uint32_t hactive;
    uint32_t vactive;
    enum HdmiPictureAspectRatio aspect;
    enum HdmiVideoTiming timing;
    enum HdmiVideoFormatType formatType;
};

struct HdmiCommonStatus {
    bool hotplug;
    bool avmute;
    bool sinkPowerOn;
    enum HdmiTmdsModeType tmdsMode;
};

struct HdmiAudioStatus {
    bool mute;
    bool enable;
    bool downSample;
    enum HdmiSampleRate sampleRate;
    enum HdmiAudioInterfaceType interface;
    enum HdmiAudioBitDepth bitDepth;
    enum HdmiAudioFormatChannel channel;
    uint32_t refN;
    uint32_t regN;
    uint32_t refCts;
    uint32_t regCts;
};

struct HdmiVideoStatus {
    bool mute;
    bool ycbcr2Rgb;
    bool rgb2Ycbcr;
    bool ycbcr444422;
    bool ycbcr422420;
    bool ycbcr420422;
    bool ycbcr422444;
    bool in420Ydemux;
    bool out420Ydemux;
    bool vSyncPol;
    bool hSyncPol;
    bool syncPol;
    bool dePol;
    bool swapHsCs;
    enum HdmiColorSpace inColorSpace;
    enum HdmiColorSpace outColorSpace;
    enum HdmiVideoBitDepth outBitDepth;
    enum HdmiQuantizationRange quantization;
};

struct HdmiCommonAttr {
    bool enableHdmi;
    bool enableVideo;
    bool enableAudio;
    bool xvyccMode;
    bool avi;
    bool spd;
    bool mpeg;
    bool audio;
    bool drm;
    bool hdcpEnable;
    bool vsifDolby;
    enum HdmiColorSpace  colorSpace;
    enum HdmiQuantizationRange quantization;
    enum HdmiDeepColor deepColor;
    enum HdmiHdcpMode hdcpMode;
};

enum HdmiVideoBitDepth HdmiCommonDeepClolorConvertToColorDepth(enum HdmiDeepColor deepColor);
enum HdmiDeepColor HdmiCommonColorDepthConvertToDeepClolor(enum HdmiVideoBitDepth colorDepth);
enum HdmiVic HdmiCommonGetVic(enum HdmiVideoTiming timing,
    enum HdmiPictureAspectRatio aspect, bool enable3d);
struct HdmiVideoDefInfo *HdmiCommonGetVideoDefInfo(enum HdmiVideoTiming timing,
    enum HdmiPictureAspectRatio aspect, bool enable3d);
struct HdmiVideo4kInfo *HdmiCommonGetVideo4kInfo(uint32_t _4kVic);
enum HdmiVideoTiming HdmiCommonGetVideoTiming(enum HdmiVic vic, enum HdmiPictureAspectRatio aspect);
enum HdmiVideoTiming HdmiCommonGetVideo4kTiming(uint32_t _4kVic);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HDMI_COMMON_H */
