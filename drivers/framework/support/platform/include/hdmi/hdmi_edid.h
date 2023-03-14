/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDMI_EDID_H
#define HDMI_EDID_H

#include "hdf_base.h"
#include "hdmi_ddc.h"
#include "hdmi_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define VIDEO_LENGTH_4 4
#define VIDEO_LENGTH_5 5
#define VIDEO_LENGTH_16 16
#define VIDEO_WIDTH_3 3
#define VIDEO_WIDTH_4 4
#define VIDEO_WIDTH_9 9
#define VIDEO_WIDTH_10 10

#define UINT8_ARRAY_TElEMENT_0 0
#define UINT8_ARRAY_TElEMENT_1 1
#define UINT8_ARRAY_TElEMENT_2 2
#define UINT8_ARRAY_TElEMENT_3 3
#define UINT8_ARRAY_TElEMENT_4 4
#define UINT8_ARRAY_TElEMENT_5 5
#define UINT8_ARRAY_TElEMENT_6 6
#define UINT8_ARRAY_TElEMENT_7 7
#define UINT8_ARRAY_TElEMENT_8 8
#define UINT8_ARRAY_TElEMENT_9 9
#define UINT8_ARRAY_TElEMENT_10 10
#define UINT8_ARRAY_TElEMENT_11 11
#define UINT8_ARRAY_TElEMENT_12 12
#define UINT8_ARRAY_TElEMENT_13 13
#define UINT8_ARRAY_TElEMENT_14 14
#define UINT8_ARRAY_TElEMENT_15 15
#define UINT8_ARRAY_TElEMENT_16 16
#define UINT8_ARRAY_TElEMENT_17 17
#define UINT8_ARRAY_TElEMENT_18 18
#define UINT8_ARRAY_TElEMENT_19 19
#define UINT8_ARRAY_TElEMENT_20 20

/* (EDID)Extended Display Identification Data */
#define HDMI_EDID_SINGLE_BLOCK_SIZE 128
#define HDMI_EDID_MAX_BLOCK_NUM 4
#define HDMI_EDID_TOTAL_SIZE (HDMI_EDID_SINGLE_BLOCK_SIZE * HDMI_EDID_MAX_BLOCK_NUM)
#define HDMI_EDID_MAX_VIC_COUNT 128
#define HDMI_EDID_CHECK_SUM_MARK 0xff

/* edid header */
#define HDMI_EDID_BLOCK_HEADER_FIELD_LEN 8

/* edid vendor/product info */
#define HDMI_EDID_MANUFACRURER_NAME_FIELD_LEN 2
#define HDMI_EDID_MANUFACRURER_NAME_MAX_LEN 4
#define HDMI_EDID_MANUFACRURER_NAME_EFFECTIVE_LEN 3
#define HDMI_EDID_MANUFACRURER_NAME_CHAR_MARK 0x1F  /* 5 bits */
#define HDMI_EDID_MANUFACRURER_NAME_CHAR_SHIFT 5
#define HDMI_EDID_MANUFACRURER_NAME_LOW_INVALID 0
#define HDMI_EDID_MANUFACRURER_NAME_HIGH_INVALID 27
#define HDMI_EDID_MANUFACRURER_NAME_SHIFT 8
#define HDMI_EDID_PRODUCT_CODE_FIELD_LEN 2
#define HDMI_EDID_PRODUCT_CODE_SHIFT 8
#define HDMI_EDID_SERIAL_NUMBER_FIELD_LEN 4
#define HDMI_EDID_SERIAL_NUMBER_SHIFT 8
#define HDMI_EDID_YEAR_BASE 1990

struct HdmiEdidVendorInfo {
    char mfrName[HDMI_EDID_MANUFACRURER_NAME_MAX_LEN];
    uint16_t productCode;
    uint32_t serialNumber;
    uint8_t week;
    uint8_t year;
};

#define HDMI_EDID_VERSION_NUM 1
#define HDMI_EDID_REVISION_NUM 3

/* edid version info */
struct HdmiEdidVersionInfo {
    uint8_t version;
    uint8_t revision;
};

/* edid basic display parameters and features */
struct HdmiEdidBasicDispParamInfo {
    uint8_t width;  /* unit: cm */
    uint8_t height; /* unit: cm */
};

/* edid color characteristics */
#define HDMI_EDID_COLOR_LOW_BITS_MARK 0x03
#define HDMI_EDID_COLOR_RED_X_LOW_OFFSET 6
#define HDMI_EDID_COLOR_RED_Y_LOW_OFFSET 4
#define HDMI_EDID_COLOR_GREEN_X_LOW_OFFSET 2
#define HDMI_EDID_COLOR_BLUE_X_LOW_OFFSET 6
#define HDMI_EDID_COLOR_BLUE_Y_LOW_OFFSET 4
#define HDMI_EDID_COLOR_WHITE_X_LOW_OFFSET 2
#define HDMI_EDID_COLOR_HIGH_OFFSET 2

struct HdmiEdidColorInfo {
    uint16_t redX;
    uint16_t redY;
    uint16_t greenX;
    uint16_t greenY;
    uint16_t blueX;
    uint16_t blueY;
    uint16_t whiteX;
    uint16_t whiteY;
};

/* edid established timings */
#define HDMI_EDID_ESTABLISHED_TIMINGS_FIELD_LEN 3

enum HdmiEdidEstablishTiming {
    HDMI_EDID_ESTABLISHED_TIMING_VESA_800X600_60 = 0,
    HDMI_EDID_ESTABLISHED_TIMING_VESA_800X600_56 = 1,
    HDMI_EDID_ESTABLISHED_TIMING_VESA_640X480_75 = 2,
    HDMI_EDID_ESTABLISHED_TIMING_VESA_640X480_72 = 3,
    HDMI_EDID_ESTABLISHED_TIMING_640X480_67 = 4,
    HDMI_EDID_ESTABLISHED_TIMING_VGA_640X480_60 = 5,
    HDMI_EDID_ESTABLISHED_TIMING_XGA_720X400_88 = 6,
    HDMI_EDID_ESTABLISHED_TIMING_VGA_720X400_70 = 7,
    HDMI_EDID_ESTABLISHED_TIMING_VESA_1280X1024_75 = 8,
    HDMI_EDID_ESTABLISHED_TIMING_VESA_1024X768_75 = 9,
    HDMI_EDID_ESTABLISHED_TIMING_VESA_1024X768_70 = 10,
    HDMI_EDID_ESTABLISHED_TIMING_VESA_1024X768_60 = 11,
    HDMI_EDID_ESTABLISHED_TIMING_1024X768_87 = 12,
    HDMI_EDID_ESTABLISHED_TIMING_832X624_75 = 13,
    HDMI_EDID_ESTABLISHED_TIMING_VESA_800X600_75 = 14,
    HDMI_EDID_ESTABLISHED_TIMING_VESA_800X600_72 = 15,
    HDMI_EDID_ESTABLISHED_TIMING_VESA_1152X870_75 = 16,
    HDMI_EDID_ESTABLISHED_TIMING_BUTT,
};

struct HdmiEdidEstablishedTimingsInfo {
    uint32_t estTimingsNum;
    uint32_t estTimings[HDMI_EDID_ESTABLISHED_TIMING_BUTT];
};

/* edid standard timing */
#define HDMI_EDID_STANDARD_TIMING_FIELD_LEN 16
#define HDMI_EDID_STANDARD_TIMING_COUNT 8
#define HDMI_EDID_PER_STANDARD_TIMING_BYTE_NUM 2
#define HDMI_EDID_STANDARD_TIMING_UNUSED_FLAG 0x01
#define HDMI_EDID_STANDARD_TIMING_HORIZ_PIXEL_BASE 31
#define HDMI_EDID_STANDARD_TIMING_HORIZ_PIXEL_FACTOR 8
#define HDMI_EDID_STANDARD_TIMING_REFRESH_RATE_BASE 60
#define HDMI_EDID_STANDARD_TIMING_REFRESH_RATE_MARK 0x3F

#define HDMI_EDID_STANDARD_TIMING_ASPECT_RATE_MARK 0xC0
#define HDMI_EDID_STANDARD_TIMING_ASPECT_RATE_SHIFT 6

enum HdmiEdidStdTimingAspectRate {
    HDMI_EDID_STANDARD_TIMING_ASPECT_RATE_16_10 = 0,
    HDMI_EDID_STANDARD_TIMING_ASPECT_RATE_5_4 = 1,
    HDMI_EDID_STANDARD_TIMING_ASPECT_RATE_4_3 = 2,
    HDMI_EDID_STANDARD_TIMING_ASPECT_RATE_16_9 = 3,
};

struct HdmiEdidStdTimingInfo {
    uint32_t horizPixel;
    uint32_t vertPixel;
    uint32_t refreshRate;
};

/* edid detailed timing descriptor */
#define HDMI_EDID_DETAILED_TIMING_DESCRIPTOR_FIELD_LEN 18
#define HDMI_EDID_DETAILED_TIMING_PIXEL_CLK_FIELD_LEN 2
#define HDMI_EDID_DETAILED_TIMING_PIXEL_CLK_KHZ_FACTOR 10
#define HDMI_EDID_DETAILED_TIMING_UPPER_4BITS_MARK 0xF0
#define HDMI_EDID_DETAILED_TIMING_LOWER_4BITS_MARK 0x0F
#define HDMI_EDID_DETAILED_TIMING_HS_OFFSET_MARK 0xC0
#define HDMI_EDID_DETAILED_TIMING_HS_PULSE_WIDTH_MARK 0x30
#define HDMI_EDID_DETAILED_TIMING_VS_OFFSET_MARK 0x0C
#define HDMI_EDID_DETAILED_TIMING_VS_PULSE_WIDTH_MARK 0x03
#define HDMI_EDID_DETAILED_TIMING_STEREO_MARK 0x60
#define HDMI_EDID_DETAILED_TIMING_STEREO_SEQUENTIAL_R 0x02
#define HDMI_EDID_DETAILED_TIMING_STEREO_SEQUENTIAL_L 0x04
#define HDMI_EDID_DETAILED_TIMING_STEREO_INTERLEAVED_2R 0x03
#define HDMI_EDID_DETAILED_TIMING_STEREO_INTERLEAVED_2L 0x05
#define HDMI_EDID_DETAILED_TIMING_STEREO_INTERLEAVED_4 0x06
#define HDMI_EDID_DETAILED_TIMING_STEREO_INTERLEAVED_SBS 0x07
#define HDMI_EDID_DETAILED_TIMING_SYNC_SIGNAL_TYPE_MARK 0x0E
#define HDMI_EDID_DETAILED_TIMING_SYNC_DCS_WS_0 0x00
#define HDMI_EDID_DETAILED_TIMING_SYNC_DCS_WS_1 0x01
#define HDMI_EDID_DETAILED_TIMING_SYNC_DCS_DS_2 0x02
#define HDMI_EDID_DETAILED_TIMING_SYNC_DCS_DS_3 0x03
#define HDMI_EDID_DETAILED_TIMING_SYNC_DSS_VN_HN_4 0x04
#define HDMI_EDID_DETAILED_TIMING_SYNC_DSS_VN_HP_5 0x05
#define HDMI_EDID_DETAILED_TIMING_SYNC_DSS_VP_HN_6 0x06
#define HDMI_EDID_DETAILED_TIMING_SYNC_DSS_VP_HP_7 0x07
#define HDMI_EDID_MAX_DETAILED_TIMING_COUNT 10

struct HdmiEdidDetailedTimingBlockInfo {
    uint8_t pixelClk[HDMI_EDID_DETAILED_TIMING_PIXEL_CLK_FIELD_LEN];
    uint8_t hActive;
    uint8_t hBlanking;
    uint8_t hActiveBlanking;
    uint8_t vActive;
    uint8_t vBlanking;
    uint8_t vActiveBlanking;
    uint8_t hSyncOffset;
    uint8_t hSyncPulseWidth;
    uint8_t vsOffesetPulseWidth;
    uint8_t hsOffsetVsOffset;
    uint8_t hImageSize;
    uint8_t vImageSize;
    uint8_t hvImageSize;
    uint8_t hBorder;
    uint8_t vBorder;
    uint8_t flags;
};

struct HdmiEdidPreferredTimingInfo {
    uint32_t pixelClk;        /* unit: KHz */
    uint32_t hActive;         /* horizontal active area */
    uint32_t hBackBlank;      /* horizontal back blank */
    uint32_t hFrontBlank;     /* horizontal front blank */
    uint32_t hSyncPluseWidth; /* horizontal sync pluse width */
    uint32_t vActive;         /* vertical active area */
    uint32_t vBackBlank;      /* vertical back blank */
    uint32_t vFrontBlank;     /* vertical front blank */
    uint32_t vSyncPluseWidth; /* vertical sync pluse width */
    uint32_t imageWidth;      /* image width */
    uint32_t imageHeight;     /* image height */
    bool interlaceFlag;       /* interlace flag */
    bool idv;                 /* flag of data valid signal is needed flip */
    bool ihs;                 /* flag of horizontal sync pulse is needed flip */
    bool ivs;                 /* flag of vertical sync pulse is needed flip */
};


/* edid monitor descriptor */
#define HDMI_EDID_MONITOR_FLAGS_FIELD_LEN 3
#define HDMI_EDID_MONITOR_DATA_FIELD_LEN 13
#define HDMI_EDID_MAX_SINK_NAME_COUNT 14

enum HdmiEdidMonitorDataType {
    HDMI_EDID_MONITOR_DATA_NAME = 0xFC,
    HDMI_EDID_MONITOR_DATA_RANGE_LIMIT = 0xFD,
    HDMI_EDID_MONITOR_DATA_STRING = 0xFE,
    HDMI_EDID_MONITOR_DATA_SERIAL_NUMBER = 0xFF,
};

struct HdmiEdidMonitorBlockInfo {
    uint8_t flags[HDMI_EDID_MONITOR_FLAGS_FIELD_LEN];
    uint8_t dataTag;
    uint8_t flag;
    uint8_t data[HDMI_EDID_MONITOR_DATA_FIELD_LEN];
};

/* edid extension flag and checksum */
#define HDMI_EDID_EXTENSION_BLOCK_ADDR 0x7E
#define HDMI_EDID_CHECKSUM_ADDR 0x7F

struct HdmiEdidFirstBlockInfo {
    uint8_t header[HDMI_EDID_BLOCK_HEADER_FIELD_LEN];
    uint8_t vendorName[HDMI_EDID_MANUFACRURER_NAME_FIELD_LEN];
    uint8_t productCode[HDMI_EDID_PRODUCT_CODE_FIELD_LEN];
    uint8_t serialNumber[HDMI_EDID_SERIAL_NUMBER_FIELD_LEN];
    uint8_t week;  /*
                    * The designated values for this field range from 1 to 53. If this field is unused, the value
                    * should be set to 0. If  the next field is used for Model Year, then 0xFF should be set.
                    */
    uint8_t year;  /* This value is determined by the actual year of production minus 1990. */
    uint8_t version;
    uint8_t revision;
    uint8_t videoInput;
    uint8_t width;  /* unit: cm */
    uint8_t height; /* unit: cm */
    uint8_t gamma;  /* Display Transfer Characteristics */
    uint8_t features;
    uint8_t redGreenLow;
    uint8_t blueWhiteLow;
    uint8_t redX;
    uint8_t redY;
    uint8_t greenX;
    uint8_t greenY;
    uint8_t blueX;
    uint8_t blueY;
    uint8_t whiteX;
    uint8_t whiteY;
    uint8_t estTimings[HDMI_EDID_ESTABLISHED_TIMINGS_FIELD_LEN];
    uint8_t stdTiming[HDMI_EDID_STANDARD_TIMING_FIELD_LEN];
    uint8_t detailedTiming1[HDMI_EDID_DETAILED_TIMING_DESCRIPTOR_FIELD_LEN];
    uint8_t detailedTiming2[HDMI_EDID_DETAILED_TIMING_DESCRIPTOR_FIELD_LEN];
    uint8_t detailedTiming3[HDMI_EDID_DETAILED_TIMING_DESCRIPTOR_FIELD_LEN];
    uint8_t detailedTiming4[HDMI_EDID_DETAILED_TIMING_DESCRIPTOR_FIELD_LEN];
    uint8_t extensionFlag;
    uint8_t checksum;
};

/* edid extension block */
#define HDMI_EDID_CTA_EXTENSION_TAG 0x02
#define HDMI_EDID_CTA_EXTENSION3_REVISION 0x03
#define HDMI_EDID_EXTENSION_DATA_BLOCK_LEN_MARK 0x1F
#define HDMI_EDID_EXTENSION_DATA_BLOCK_TAG_CODE_MARK 0xE0
#define HDMI_EDID_EXTENSION_DATA_BLOCK_TAG_CODE_SHIFT 5
#define HDMI_EDID_EXTENSION_BLOCK_OFFSET 4
#define HDMI_EDID_EXTENSION_D_INVALID_MIN_VAL 4

enum HdmiEdidExtDataBlockType {
    HDMI_EDID_NULL_DATA_BLOCK = 0,
    HDMI_EDID_AUDIO_DATA_BLOCK = 1,
    HDMI_EDID_VIDEO_DATA_BLOCK = 2,
    HDMI_EDID_VENDOR_SPECIFIC_DATA_BLOCK = 3,
    HDMI_EDID_SPEAKER_ALLOCATION_DATA_BLOCK = 4,
    HDMI_EDID_VESA_DTC_DATA_BLOCK = 5, /* VESA Display Transfer Characteristic Data Block */
    HDMI_EDID_USE_EXT_DATA_BLOCK = 7,  /* Use Extended Tag */
};

struct HdmiEdidColorSpace {
    bool rgb444;
    bool ycbcr422;
    bool ycbcr444;
    bool ycbcr420;
};

/* edid extension block: Audio Data Block */
#define HDMI_EDID_EXTENSION_SHORT_AUDIO_DESCRIPTOR_LEN 3
#define HDMI_EDID_EXTENSION_MAX_SHORT_AUDIO_DESCRIPTOR_NUM 10
#define HDMI_EDID_EXTENSION_AUDIO_FORMAT_CODE_MARK 0x78
#define HDMI_EDID_EXTENSION_AUDIO_FORMAT_CODE_SHIFT 3
#define HDMI_EDID_EXTENSION_AUDIO_MAX_CHANNEL_MARK 0x07
#define HDMI_EDID_EXTENSION_AUDIO_MAX_SAMPLE_RATE_NUM 8
#define HDMI_EDID_EXTENSION_AUDIO_MAX_BIT_DEPTH_NUM 6
#define HDMI_EDID_EXTENSION_AUDIO_BIT_RATE_FACTOR 8
#define HDMI_EDID_EXTENSION_AUDIO_CAP_COUNT 16

struct HdmiEdidAudioInfo {
    enum HdmiAudioCodingType formatCode;
    uint32_t sampleRateNum;
    enum HdmiSampleRate sampleRate[HDMI_EDID_EXTENSION_AUDIO_MAX_SAMPLE_RATE_NUM];
    uint32_t bitDepthNum;
    enum HdmiAudioBitDepth bitDepth[HDMI_EDID_EXTENSION_AUDIO_MAX_BIT_DEPTH_NUM];
    uint8_t channels;
    uint32_t maxBitRate;  /* unit: KHz */
};

/* edid extension block: Video Data Block */
#define HDMI_EDID_EXTENSION_VIC_NATIVE_MAX 64
#define HDMI_EDID_EXTENSION_VIC_LOWER7_MARK 0x7F
#define HDMI_EDID_EXTENSION_MAX_VIC_COUNT 128
#define HDMI_EDID_EXTENSION_VIC_INVALID_LOW 128
#define HDMI_EDID_EXTENSION_VIC_INVALID_HIGH 192

/*
 * Each supported Video Format is represented by a Short Video Descriptor (SVD) containing
 * a Video Identification Code (VIC) and, in the case of VICs 1 through 64, a Native Video Format indicator.
 */
struct HdmiEdidVideoInfo {
    uint32_t vicNum;
    uint32_t vic[HDMI_EDID_EXTENSION_MAX_VIC_COUNT];
    uint32_t nativeFormat;
};

/* edid extension block: (VSDB)Vendor-Specific Data Block */
#define HDMI_EDID_EXTENSION_VSDB_LEN 3
#define HDMI_EDID_EXTENSION_VSDB_IEEE_1ST 0x03
#define HDMI_EDID_EXTENSION_VSDB_IEEE_2ND 0x0C
#define HDMI_EDID_EXTENSION_VSDB_IEEE_3RD 0x00
#define HDMI_EDID_EXTENSION_VSDB_CEC_INVALID_ADDR 0xF
#define HDMI_EDID_EXTENSION_MAX_HDMI14_TMDS_RATE 340 /* unit: Mcsc */
#define HDMI_EDID_EXTENSION_TMDS_FACTOR 5  /* unit: MHz */
#define HDMI_EDID_EXTENSION_VSDB_3D_LEN_MARK 0x1F
#define HDMI_EDID_EXTENSION_VSDB_VIC_LEN_MARK 0xE0
#define HDMI_EDID_EXTENSION_VSDB_VIC_LEN_SHIFT 5
#define HDMI_EDID_EXTENSION_VSDB_3D_MULTI_PRESENT_MARK 0x60
#define HDMI_EDID_EXTENSION_VSDB_3D_MULTI_PRESENT_SHIFT 5
#define HDMI_EDID_EXTENSION_VSDB_3D_STR_INVALID_MARK 0x03

#define HDMI_EDID_VSDB_MIN_LEN_FOR_CEC_PHY_ADDR 5
#define HDMI_EDID_VSDB_MIN_LEN_FOR_COLOR_DEPTH 6
#define HDMI_EDID_VSDB_MIN_LEN_FOR_MAX_TMDS_CLOCK 7
#define HDMI_EDID_VSDB_MIN_LEN_FOR_SINK_PRESENT 8
#define HDMI_EDID_VSDB_MIN_LEN_FOR_VIDEO_LATENCY 9
#define HDMI_EDID_VSDB_MIN_LEN_FOR_AUDIO_LATENCY 10
#define HDMI_EDID_VSDB_MIN_LEN_FOR_INTERLACED_VIDEO_LATENCY 11
#define HDMI_EDID_VSDB_MIN_LEN_FOR_INTERLACED_AUDIO_LATENCY 12
#define HDMI_EDID_VSDB_MIN_LEN_FOR_3D_PRESENT_INFO 13
#define HDMI_EDID_VSDB_MIN_LEN_FOR_VIC_INFO 14

struct HdmiEdidVsdbCecAddr {
    bool addrValid;
    uint8_t phyAddrA;
    uint8_t phyAddrB;
    uint8_t phyAddrC;
    uint8_t phyAddrD;
};

struct HdmiEdidDeepColor {
    bool dcY444;
    bool dc30bit;
    bool dc36bit;
    bool dc48bit;
};

struct HdmiEdidExtVsdbInfo {
    struct HdmiEdidVsdbCecAddr cecAddr;
    struct HdmiEdidDeepColor deepColor;
    bool supportAi;
    bool supportDviDual;
    bool latencyFieldsPresent;
    bool iLatencyFieldsPresent;
    bool hdmiVideoPresent;
    uint8_t videoLatency;
    uint8_t audioLatency;
    uint8_t interlacedVideoLatency;
    uint8_t interlacedAudioLatency;
    bool _3dPresent;
    uint8_t _3dMultiPresent;
    bool support3dType[HDMI_VS_VIDEO_3D_BUTT];
};

/* edid extension block: (HF-VSDB)HDMI Forum Vendor-Specific Data Block */
#define HDMI_EDID_EXTENSION_HFVSDB_IEEE_1ST 0xD8
#define HDMI_EDID_EXTENSION_HFVSDB_IEEE_2ND 0x5D
#define HDMI_EDID_EXTENSION_HFVSDB_IEEE_3RD 0xC4
#define HDMI_EDID_EXTENSION_HFVSDB_VERSION 0x01
#define HDMI_EDID_EXTENSION_HFVSDB_MIN_INVALID_LEN 7
#define HDMI_EDID_EXTENSION_HFVSDB_MAX_INVALID_LEN 31

/* deep color Y420(4:2:0) */
struct HdmiEdidExtHfVsdbDeepColor {
    bool dc30bit;
    bool dc36bit;
    bool dc48bit;
};

#define HDMI_EDID_EXTENSION_HFVSDB_DSC_TOTAL_CHUNK_MARK 0x3F

/* dsc(Display Stream Compression ) */
struct HdmiEdidExtHfVsdbDscInfo {
    bool dsc1p2;
    bool dscNative420;
    bool dscAllBpp;
    bool dsc10bpc;
    bool dsc20bpc;
    bool dsc16bpc;
    uint8_t dscMaxSlices;
    uint8_t dscMaxFrlRate;
    uint8_t dscTotalChunkKBytes;
};

#define HDMI_EDID_EXTENSION_HFVSDB_VRRMIN_MARK 0x3F
#define HDMI_EDID_EXTENSION_HFVSDB_VRRMAX_MARK 0xC0
#define HDMI_EDID_EXTENSION_HFVSDB_VRRMAX_SHIFT 2

struct HdmiEdidExtHfVsdbInfo {
    bool scdcPresent;
    bool rrCapable;           /* read request cap. */
    bool lte340McscScramble;
    bool independentView;
    bool dualView;
    bool _3dOsdDisparity;
    struct HdmiEdidExtHfVsdbDeepColor dc;
    /* hdmi 2.1 */
    uint8_t maxFrlRate;
    bool fapaStartLocation;
    bool allm;
    bool fva;
    bool cnmVrr;
    bool cinemaVrr;
    bool mDelta;
    uint8_t vrrMin;
    uint16_t vrrMax;
    struct HdmiEdidExtHfVsdbDscInfo dscInfo;
};

/* edid extension block: Speaker Allocation Data Block */
#define HDMI_EDID_EXTENSION_SADB_MIN_INVALID_LEN 2

enum HdmiEdidAudioSpeaker {
    HDMI_EDID_AUDIO_SPEAKER_FL_FR = 0,
    HDMI_EDID_AUDIO_SPEAKER_LFE = 1,
    HDMI_EDID_AUDIO_SPEAKER_FC = 2,
    HDMI_EDID_AUDIO_SPEAKER_BL_BR = 3,
    HDMI_EDID_AUDIO_SPEAKER_BC = 4,
    HDMI_EDID_AUDIO_SPEAKER_FLC_FRC = 5,
    HDMI_EDID_AUDIO_SPEAKER_RLC_RRC = 6,
    HDMI_EDID_AUDIO_SPEAKER_FLW_FRW = 7,
    HDMI_EDID_AUDIO_SPEAKER_TPFL_TPFH = 8,
    HDMI_EDID_AUDIO_SPEAKER_TPC = 9,
    HDMI_EDID_AUDIO_SPEAKER_TPFC = 10,
    HDMI_EDID_AUDIO_SPEAKER_BUTT,
};

/*
 * If the Tag Code is 7 (Use Extended Tag) then the second byte of the data block contains the Extended
 * Tag Code, which indicates the actual type of the data block.
 */
enum HdmiEdidExtExtenedDataBlockType {
    HDMI_EDID_EXT_VCDB = 0,            /* Video Capability Data Block */
    HDMI_EDID_EXT_VSVDB = 1,           /* Vendor-Specific Video Data Block */
    HDMI_EDID_EXT_VESA_DDDB = 2,       /* VESA Display Device Data Block */
    HDMI_EDID_EXT_VESA_VTBE = 3,       /* VESA Video Timing Block Extension */
    HDMI_EDID_EXT_CDB = 5,             /* Colorimetry Data Block */
    HDMI_EDID_EXT_HDR_SMDB = 6,        /* HDR Static Metadata Data Block */
    HDMI_EDID_EXT_HDR_DMDB = 7,        /* HDR Dynamic Metadata Data Block */
    HDMI_EDID_EXT_VFPDB = 13,          /* Video Format Preference Data Block */
    HDMI_EDID_EXT_YCBCR420_VDB = 14,   /* YCBCR 4:2:0 Video Data Block */
    HDMI_EDID_EXT_YCBCR420_CMDB = 15,  /* YCBCR 4:2:0 Capability Map Data Block */
    HDMI_EDID_EXT_VSADB = 17,          /* Vendor-Specific Audio Data Block */
    HDMI_EDID_EXT_RCDB = 19,           /* Room Configuration Data Block */
    HDMI_EDID_EXT_SLDB = 20,           /* Speaker Location Data Block */
    HDMI_EDID_EXT_INFOFRAME_DB = 32,   /* InfoFrame Data Block (includes one or more Short InfoFrame Descriptors) */
    HDMI_EDID_EXT_BUTT,
};

/* edid extension block: Colorimetry Data Block */
#define HDMI_EDID_CDB_LEN 3

struct HdmiEdidColorimetry {
    bool xvYcc601;
    bool xvYcc709;
    bool sYcc601;
    bool opYcc601;
    bool opRgb;
    bool bt2020cYcc;
    bool bt2020Ycc;
    bool bt2020Rgb;
    bool dciP3;
    uint8_t md;
};

/* edid extension block: Vendor-Specific Video Data Block */
#define HDMI_EDID_VSVDB_DOLBY_OUI 0x00D046
#define HDMI_EDID_VSVDB_DOLBY_VERSION_0 0x00
#define HDMI_EDID_VSVDB_DOLBY_VERSION_1 0x01
#define HDMI_EDID_VSVDB_DOLBY_VERSION_0_LEN 27
#define HDMI_EDID_VSVDB_DOLBY_VERSION_1_LEN 14
#define HDMI_EDID_VSVDB_DOLBY_LOWER_2BIT_MARK 0x03
#define HDMI_EDID_VSVDB_DOLBY_LOWER_3BIT_MARK 0x07
#define HDMI_EDID_VSVDB_DOLBY_LOWER_7BIT_MARK 0x7F
#define HDMI_EDID_VSVDB_DOLBY_VERSION_MARK 0xE0
#define HDMI_EDID_VSVDB_DOLBY_VERSION_SHIFT 5
#define HDMI_EDID_VSVDB_DOLBY_DM_VER_MARK 0x1C
#define HDMI_EDID_VSVDB_DOLBY_DM_VER_SHIFT 2

struct HdmiEdidVsvdbDolbyCap {
    uint32_t oui;
    uint8_t version;
    bool yuv422;
    bool b2160p60;       /* capable of processing a max timing 3840X2160p60. */
    bool globalDimming;
    uint16_t redX;
    uint16_t redY;
    uint16_t greenX;
    uint16_t greenY;
    uint16_t blueX;
    uint16_t blueY;
    uint16_t whiteX;          /* only version0 support */
    uint16_t whiteY;          /* only version0 support */
    uint16_t minLuminance;
    uint16_t maxLuminance;
    uint8_t dMajorVer;        /* only version0 support */
    uint8_t dMinorVer;        /* only version0 support */
    bool colorimetry;         /* only version1 support */
    uint8_t dmVer;            /* only version1 support */
};

/* edid extension block: Video Capability Data Block */
#define HDMI_EDID_VCDB_LEN 2

struct HdmiEdidVideoCapability {
    bool qy;     /* Quantization Range(Applies to YCC only) */
    bool qs;     /* Quantization Range Selectable(Applies to RGB only) */
};

/* edid extension block: HDR Static Metadata Data Block */
#define HDMI_EDID_HDR_SMDB_MIN_LEN 3
#define HDMI_EDID_HDR_SMDB_MIN_LEN_FOR_MAX_LUMINANCE_DATA 4
#define HDMI_EDID_HDR_SMDB_MIN_LEN_FOR_MAX_FRAME_AVE_LUMINANCE_DATA 5
#define HDMI_EDID_HDR_SMDB_MIN_LEN_FOR_MIN_LUMINANCE_DATA 6

/* EOTF(Electro-Optical Transfer Functions) */
struct HdmiEdidHdrSmdbEotf {
    bool sdr;                  /* Traditional gamma - SDR Luminance Range */
    bool hdr;                  /* Traditional gamma - HDR Luminance Range */
    bool smpteSt2048;          /* SMPTE ST 2084 */
    bool hlg;                  /* Hybrid Log-Gamma (HLG) based on Recommendation ITU-R BT.2100-0 */
};

struct HdmiEdidHdrCap {
    struct HdmiEdidHdrSmdbEotf eotf;
    bool smType1;                           /* Static Metadata Type 1 */
    uint8_t maxLuminancedata;               /* Desired Content Max Luminance data */
    uint8_t maxFrameAverageLuminanceData;   /* Desired Content Max Frame-average Luminance data */
    uint8_t minLuminanceData;               /* Desired Content Min Luminance data */
};

/* edid extension block: YCBCR 4:2:0 Capability Map Data Block */
struct HdmiEdidYcbcr420Cap {
    uint32_t onlySupportY420VicNum;
    uint32_t onlySupportY420Format[HDMI_EDID_EXTENSION_MAX_VIC_COUNT];
    uint32_t SupportY420VicNum;
    uint32_t SupportY420Format[HDMI_EDID_EXTENSION_MAX_VIC_COUNT];
};

/* sink device cap */
struct HdmiSinkDeviceCapability {
    uint8_t extBlockNum;
    struct HdmiEdidVendorInfo vendorInfo;
    struct HdmiEdidVersionInfo verInfo;
    char sinkDeviceName[HDMI_EDID_MAX_SINK_NAME_COUNT];
    struct HdmiEdidBasicDispParamInfo disp;
    struct HdmiEdidEstablishedTimingsInfo establishedTimingsInfo;
    struct HdmiEdidStdTimingInfo stdTimingsInfo[HDMI_EDID_STANDARD_TIMING_COUNT];
    struct HdmiEdidColorInfo colorInfo;
    struct HdmiEdidColorSpace colorSpace;
    uint32_t preTimingCnt;
    struct HdmiEdidPreferredTimingInfo preTimingInfo[HDMI_EDID_MAX_DETAILED_TIMING_COUNT];
    uint32_t audioInfoCnt;
    struct HdmiEdidAudioInfo audioInfo[HDMI_EDID_EXTENSION_AUDIO_CAP_COUNT];
    struct HdmiEdidVideoInfo videoInfo;
    uint32_t maxTmdsClk;
    bool supportHdmi14;
    bool supportHdmi20;
    bool supportAudio;
    struct HdmiEdidExtVsdbInfo vsdbInfo;
    struct HdmiEdidExtHfVsdbInfo hfVsdbInfo;
    bool supportAudioSpeaker[HDMI_EDID_AUDIO_SPEAKER_BUTT];
    struct HdmiEdidColorimetry colorimetry;
    struct HdmiEdidVideoCapability videoCap;
    struct HdmiEdidYcbcr420Cap y420Cap;
    struct HdmiEdidHdrCap hdrCap;
    struct HdmiEdidVsvdbDolbyCap dolbyCap;
};

/*
 * EDID(Extended Display Identification Data).
 * The Source read the Sink's EDID in order to discover the Sink's configuration and/or capabilities.
 */
struct HdmiEdid {
    bool edidPhase;
    uint32_t rawLen;
    uint8_t raw[HDMI_EDID_TOTAL_SIZE];
    struct HdmiSinkDeviceCapability sinkCap;
};

typedef int32_t (*HdmiEdidPhaseFunc)(struct HdmiEdid *edid);

int32_t HdmiEdidReset(struct HdmiEdid *edid);
int32_t HdmiEdidPhase(struct HdmiEdid *edid);
int32_t HdmiEdidGetRaw(struct HdmiEdid *edid, uint8_t *raw, uint32_t len);
int32_t HdmiEdidRawDataRead(struct HdmiEdid *edid, struct HdmiDdc *ddc);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HDMI_EDID_H */
