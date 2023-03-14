/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDMI_INFOFRAME_H
#define HDMI_INFOFRAME_H

#include "hdmi_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*
 * An InfoFrame packet carries one InfoFrame. The InfoFrame provided by HDMI is limited to 30
 * bytes plus a checksum byte. HDMI Sources are required, in some cases, to use the AVI InfoFrame and Audio InfoFrame
 * and recommended in other cases. Other InfoFrames specified in CEA-861 Spec are optional.
 *
 * TMDS is used to carry all audio and video data as well as auxiliary data, including AVI(Auxiliary Video information)
 * and Audio InfoFrames that describe the active audio and video streams.
 *
 */
#define HDMI_INFOFRAME_LEN 32
#define HDMI_INFOFRAME_PACKET_HEADER_LEN 4
#define HDMI_AVI_INFOFRAME_LEN 13
#define HDMI_AUDIO_INFOFRAME_LEN 10
#define HDMI_DRM_INFOFRAME_LEN 26
#define HDMI_SPD_INFOFRAME_LEN 25
#define HDMI_INFOFRAME_CHECKSUM 256

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
#define UINT8_ARRAY_TElEMENT_21 21
#define UINT8_ARRAY_TElEMENT_22 22
#define UINT8_ARRAY_TElEMENT_23 23
#define UINT8_ARRAY_TElEMENT_24 24
#define UINT8_ARRAY_TElEMENT_25 25

enum HdmiPacketType {
    HDMI_PACKET_TYPE_NULL = 0x00,                      /* Null Packet */
    HDMI_PACKET_TYPE_AUDIO_CLOCK_REGENERATION = 0x01,  /* Audio Clock Regeneration(N/CTS) */
    HDMI_PACKET_TYPE_AUDIO_SAMPLE = 0x02,              /* Audio Samlpe(L-PCM and IEC 61937 compressed formats) */
    HDMI_PACKET_TYPE_GENERAL_CONTROL = 0x03,           /* General Control Packet */
    HDMI_PACKET_TYPE_ACP = 0x04,                       /* ACP(Audio Content Protection Packet) */
    HDMI_PACKET_TYPE_ISRC1 = 0x05,                     /* ISRC(International Standard Recording Code) */
    HDMI_PACKET_TYPE_ISRC2 = 0x06,
    HDMI_PACKET_TYPE_ONE_BIT_AUDIO_SAMPLE = 0x07,      /* One Bit Audio Sample Packet */
    HDMI_PACKET_TYPE_DST_AUDIO = 0x08,                 /* DST(Diret Stream Transport) Audio Packet */
    HDMI_PACKET_TYPE_HBR_AUDIO_STREAM = 0x09,          /* HBR(High Bitrate) Audio Stream Packet */
    HDMI_PACKET_TYPE_GAMUT_METADATA = 0x0a,            /* gamut metadata packet */
    /* infoFrame type */
    HDMI_INFOFRAME_PACKET_TYPE_VS = 0x81,              /* Verdor-Specific */
    HDMI_INFOFRAME_PACKET_TYPE_AVI = 0x82,             /* Auxiliary Video Information */
    HDMI_INFOFRAME_PACKET_TYPE_SPD = 0x83,             /* Source Product Description */
    HDMI_INFOFRAME_PACKET_TYPE_AUDIO = 0x84,           /* Audio */
    HDMI_INFOFRAME_PACKET_TYPE_MPEG = 0x85,            /* MPEG Source */
    HDMI_INFOFRAME_PACKET_TYPE_GBD = 0x86,             /* gamut boundary description */
    HDMI_INFOFRAME_PACKET_TYPE_DRM = 0x87,             /* Dynamic Range and Mastering */
    HDMI_PACKET_TYPE_BUTT,
};

struct HdmiInfoFrameHeader {
    enum HdmiPacketType type;
    uint8_t verNum;
    uint8_t len;
};

/* Verdor-Specific infoFrame details. */
#define HDMI_VENDOR_1_4_MAX_3D_METADAT_LEN 20
#define HDMI_VENDOR_USER_DATA_MAX_LEN 22
#define HDMI_VENDOR_1_4_FORMAT_MARK 0x07
#define HDMI_VENDOR_1_4_FORMAT_SHIFT 5
#define HDMI_VENDOR_3D_STRUCTURE_MARK 0x0f
#define HDMI_VENDOR_3D_STRUCTURE_SHIFT 4
#define HDMI_VENDOR_3D_EXT_DATA_MARK 0x0f
#define HDMI_VENDOR_3D_EXT_DATA_SHIFT 4
#define HDMI_VSIF_VERSION 1

enum HdmiIeeeOui {
    HDMI_IEEE_OUI_1_4 = 0x000c03,
    HDMI_IEEE_OUI_2_0 = 0x045dd8,
};

enum HdmiVsVideoFormat {
    HDMI_VS_VIDEO_FORMAT_NULL = 0,
    HDMI_VS_VIDEO_FORMAT_4K = 1,
    HDMI_VS_VIDEO_FORMAT_3D = 2,
    HDMI_VS_VIDEO_FORMAT_BUTT,
};

enum HdmiVs3dExtData {
    HDMI_3D_PICTURE_HORIZONTAL_ODD_LEFT_ODD_RIGHT = 0,
    HDMI_3D_PICTURE_HORIZONTAL_ODD_LEFT_EVEN_RIGHT = 1,
    HDMI_3D_PICTURE_HORIZONTAL_EVEN_LEFT_ODD_RIGHT = 2,
    HDMI_3D_PICTURE_HORIZONTAL_EVEN_LEFT_EVEN_RIGHT = 3,
    HDMI_3D_PICTURE_MATRIX_ODD_LEFT_ODD_RIGHT = 4,
    HDMI_3D_PICTURE_MATRIX_ODD_LEFT_EVEN_RIGHT = 5,
    HDMI_3D_PICTURE_MATRIX_EVEN_LEFT_ODD_RIGHT = 6,
    HDMI_3D_PICTURE_MATRIX_EVEN_LEFT_EVEN_RIGHT = 7,
    HDMI_3D_PICTURE_BUTT,
};

struct HdmiVs14VsifContent {
    enum HdmiIeeeOui oui;
    enum HdmiVsVideoFormat format;
    enum Hdmi4kVic vic;
    bool _3dMetaPresent;
    enum HdmiVideo3dStructure _3dStruct;
    enum HdmiVs3dExtData _3dExtData;
    uint8_t _3dMetadataType;
    uint8_t _3dMetadataLen;
    uint8_t _3dMetadata[HDMI_VENDOR_1_4_MAX_3D_METADAT_LEN];
};

struct HdmiVsUserVsifContent {
    enum HdmiIeeeOui oui;
    enum HdmiVsVideoFormat format;
    enum Hdmi4kVic vic;
    bool _3dMetaPresent;
    enum HdmiVideo3dStructure _3dStruct;
    uint8_t len;
    uint8_t data[HDMI_VENDOR_USER_DATA_MAX_LEN];
};

struct HdmiForumVsifContent {
    enum HdmiIeeeOui oui;
    uint8_t version;
    bool _3dValid;
    uint8_t _3dFStructure;
    bool _3dAdditionalInfoPresent;
    bool _3dDisparityDataPresent;
    bool _3dMetaPresent;
    uint8_t _3dFExtData;
    uint8_t _3dDualView;
    uint8_t _3dViewDependency;
    uint8_t _3dPreferred2dView;
    uint8_t _3dDisparityDataVersion;
    uint8_t _3dDisparityDataLen;
    uint8_t _3dDisparityData[10];
    uint8_t _3dMetadataType;
    uint8_t _3dMetadataLen;
    uint8_t _3dMetadata[10];
};

struct HdmiVsInfoFrame {
    enum HdmiPacketType type;
    uint8_t verNum;
    uint8_t len;
    union {
        struct HdmiVs14VsifContent vsif;
        struct HdmiVsUserVsifContent userVsif;
        struct HdmiForumVsifContent forumVsif;
    } vsifContent;
};

/* AVI infoFrame details. */
#define HDMI_AVI_COLOR_SPACE_MARK 0x03
#define HDMI_AVI_SCAN_MODE_MARK 0x03
#define HDMI_AVI_COLOR_SPACE_SHIFT 5
#define HDMI_AVI_ACTIVE_INFORMATION_SHIFT 4
#define HDMI_AVI_HORIZONTAL_BAR_SHIFT 3
#define HDMI_AVI_VERTICAL_BAR_SHIFT 2

#define HDMI_AVI_COLORIMETRY_MARK 0x03
#define HDMI_AVI_PICTURE_ASPECT_RATE_MARK 0x03
#define HDMI_AVI_ACTIVE_FORMAT_ASPECT_RATE_MARK 0x0f
#define HDMI_AVI_COLORIMETRY_SHIFT 6
#define HDMI_AVI_PICTURE_ASPECT_RATE_SHIFT 4

#define HDMI_AVI_EXT_COLORIMETRY_MARK 0x07
#define HDMI_AVI_EXT_QUANTIZATION_RANGE_MARK 0x03
#define HDMI_AVI_NUPS_RANGE_MARK 0x03
#define HDMI_AVI_IT_CONTENT_SHIFT 7
#define HDMI_AVI_EXT_COLORIMETRY_SHIFT 4
#define HDMI_AVI_EXT_QUANTIZATION_RANGE_SHIFT 2

#define HDMI_AVI_YCC_QUANTIZATION_RANGE_MARK 0x03
#define HDMI_AVI_IT_CONTENT_TYPE_MARK 0x03
#define HDMI_AVI_PIXEL_REPETION_FACTOR_MARK 0x0f
#define HDMI_AVI_YCC_QUANTIZATION_RANGE_SHIFT 6
#define HDMI_AVI_IT_CONTENT_TYPE_SHIFT 4

#define HDMI_AVI_BAR_MODE_MARK 0xff
#define HDMI_AVI_BAR_MODE_SHIFT 8

#define HDMI_AVI_VERSION2 2
#define HDMI_AVI_VERSION3 3
#define HDMI_AVI_VERSION4 4
#define HDMI_AVI_Y2_MASK (0x1 << 2)

enum HdmiScanMode {
    HDMI_SCAN_MODE_NO_DATA = 0,
    HDMI_SCAN_MODE_OVERSCAN = 1,
    HDMI_SCAN_MODE_UNDERSCAN = 2,
    HDMI_SCAN_MODE_FUTURE = 3,
};

enum HdmiItContentType {
    HDMI_IT_CONTENT_TYPE_GRAPHICS = 0,
    HDMI_IT_CONTENT_TYPE_PHOTO = 1,
    HDMI_IT_CONTENT_TYPE_CINEMA = 2,
    HDMI_IT_CONTENT_TYPE_GAME = 3,
};

struct HdmiAviInfoFrame {
    enum HdmiPacketType type;
    uint8_t verNum;
    uint8_t len;
    enum HdmiScanMode scanMode;
    bool vertBarInfoPresent;
    bool horizBarInfoPresent;
    bool activeFormatInformationPresent;
    enum HdmiColorSpace colorSpace;
    enum HdmiActiveFormatAspectRatio activeAspect;
    enum HdmiPictureAspectRatio pictureAspect;
    enum HdmiColorimetry colorimetry;
    enum HdmiNups nups;
    enum HdmiQuantizationRange range;
    enum HdmiExtendedColorimetry extColorimetry;
    bool itc; /* IT content */
    uint8_t vic;
    uint8_t pixelRepetitionFactor;
    enum HdmiItContentType itcType;
    enum HdmiYccQuantizationRange yccRange;
    uint16_t topBar;
    uint16_t bottomBar;
    uint16_t leftBar;
    uint16_t rightBar;
};

/* Audio infoFrame details. */
#define HDMI_AUDIO_INFOFRAME_VERSION 1
#define HDMI_AUDIO_CODING_TYPE_MARK 0x0f
#define HDMI_AUDIO_CHANNEL_COUNT_MARK 0x07
#define HDMI_AUDIO_CODING_TYPE_SHIFT 4

#define HDMI_AUDIO_SAMPLE_FREQUENCY_MARK 0x07
#define HDMI_AUDIO_SAMPLE_SIZE_MARK 0x03
#define HDMI_AUDIO_SAMPLE_FREQUENCY_SHIFT 2

#define HDMI_AUDIO_CXT_MARK 0x1f

#define HDMI_AUDIO_LEVEL_SHIFT_VALUE_MARK 0x0f
#define HDMI_AUDIO_LEF_PLAYBACK_LEVEL_MARK 0x03
#define HDMI_AUDIO_DM_INH_SHIFT 7
#define HDMI_AUDIO_LEVEL_SHIFT_VALUE_SHIFT 3

enum HdmiAudioSampleSize {
    HDMI_AUDIO_SAMPLE_SIZE_STREAM = 0,
    HDMI_AUDIO_SAMPLE_SIZE_16_BIT = 1,
    HDMI_AUDIO_SAMPLE_SIZE_20_BIT = 2,
    HDMI_AUDIO_SAMPLE_SIZE_24_BIT = 3,
};

enum HdmiAudioSampleFrequency {
    HDMI_AUDIO_SAMPLE_FREQUENCY_STREAM = 0,
    HDMI_AUDIO_SAMPLE_FREQUENCY_32K = 1,
    HDMI_AUDIO_SAMPLE_FREQUENCY_44_1K = 2,
    HDMI_AUDIO_SAMPLE_FREQUENCY_48K = 3,
    HDMI_AUDIO_SAMPLE_FREQUENCY_88_2K = 4,
    HDMI_AUDIO_SAMPLE_FREQUENCY_96K = 5,
    HDMI_AUDIO_SAMPLE_FREQUENCY_176_4K = 6,
    HDMI_AUDIO_SAMPLE_FREQUENCY_192K = 7,
};

enum HdmiAudioCodingExtType {
    /* Refer to Audio Coding Type (CT) field in Data Byte 1 */
    HDMI_AUDIO_CODING_EXT_TYPET_CT = 0,

    /*
     * The next three CXT values are defined in CEA-861-E only.
     * They do not exist in older versions, and in CEA-861-F they are
     * defined as 'Not in use'.
     */
    HDMI_AUDIO_CODING_EXT_TYPET_HE_AAC = 1,
    HDMI_AUDIO_CODING_EXT_TYPET_HE_AAC_V2 = 2,
    HDMI_AUDIO_CODING_EXT_TYPET_MPEG_SURROUND = 3,

    HDMI_AUDIO_CODING_EXT_TYPET_MPEG4_HE_AAC = 4,
    HDMI_AUDIO_CODING_EXT_TYPET_MPEG4_HE_AAC_V2 = 5,
    HDMI_AUDIO_CODING_EXT_TYPET_MPEG4_AAC_LC = 6,
    HDMI_AUDIO_CODING_EXT_TYPET_DRA = 7,
    HDMI_AUDIO_CODING_EXT_TYPET_MPEG4_HE_AAC_SURROUND = 8,
    HDMI_AUDIO_CODING_EXT_TYPET_RESERVED = 9,
    HDMI_AUDIO_CODING_EXT_TYPET_MPEG4_AAC_LC_SURROUND = 10,

    HDMI_AUDIO_CODING_EXT_TYPET_MPEGH_3D_AUDIO = 11,
    HDMI_AUDIO_CODING_EXT_TYPET_AC4 = 12,
    HDMI_AUDIO_CODING_EXT_TYPET_LPCM_3D_AUDIO = 13,
    HDMI_AUDIO_CODING_EXT_TYPET_BUTT,
};

/* LEF(Low Frequency Effects) playback level. */
enum HdmiAudioLfePlaybackLevel {
    HDMI_AUDIO_LFE_PLAYBACK_NULL = 0,
    HDMI_AUDIO_LFE_PLAYBACK_0DB = 1,
    HDMI_AUDIO_LFE_PLAYBACK_10DB = 2,
    HDMI_AUDIO_LFE_PLAYBAC_BUTT,
};

struct HdmiAudioInfoFrame {
    enum HdmiPacketType type;
    uint8_t verNum;
    uint8_t len;
    uint8_t channelCount;
    enum HdmiAudioCodingType codingType;
    enum HdmiAudioSampleSize sampleSize;
    enum HdmiAudioSampleFrequency sampleFreq;
    enum HdmiAudioCodingExtType codingExtType;
    uint8_t channelAllocation;
    enum HdmiAudioLfePlaybackLevel playBackLevel;
    uint8_t levelShiftValue;
    bool dmInh; /* Down-mix Inhibit Flag */
};

/* Source Product Description infoFrame details. */
#define HDMI_SPD_VENDOR_NAME_LEN 8
#define HDMI_SPD_PRODUCT_DESCRIPTION_LEN 16
#define HDMI_SPD_VERSION 1

/* SDI(Source Device Information) */
enum HdmiSpdSdi {
    HDMI_SPD_SDI_NULL = 0,
    HDMI_SPD_SDI_DIGITAL_STB = 1,
    HDMI_SPD_SDI_DVD_PLAYER = 2,
    HDMI_SPD_SDI_D_VHS = 3,
    HDMI_SPD_SDI_HDD_VIDEORECORDER = 4,
    HDMI_SPD_SDI_DVC = 5,
    HDMI_SPD_SDI_DSC = 6,
    HDMI_SPD_SDI_VIDEO_CD = 7,
    HDMI_SPD_SDI_GAME = 8,
    HDMI_SPD_SDI_PC_GENERAL = 9,
    HDMI_SPD_SDI_BLU_RAY_DIS = 10,
    HDMI_SPD_SDI_SUPER_AUDIO_CD = 11,
    HDMI_SPD_SDI_HDDVD = 12,
    HDMI_SPD_SDI_PMP = 13,
    HDMI_SPD_SDI_BUTT,
};

struct HdmiSpdInfoFrame {
    enum HdmiPacketType type;
    uint8_t verNum;
    uint8_t len;
    char vendorName[HDMI_SPD_VENDOR_NAME_LEN];
    char productDescription[HDMI_SPD_PRODUCT_DESCRIPTION_LEN];
    enum HdmiSpdSdi sdi;
};

/* Dynamic Range and Mastering infoFrame details. */
#define HDMI_DRM_METADATA_MARK 0xff
#define HDMI_DRM_METADATA_SHIFT 8
#define HDMI_DRM_INFOFRAME_VERSION 1

struct HdmiDrmInfoFrame {
    enum HdmiPacketType type;
    uint8_t verNum;
    uint8_t len;
    enum HdmiEotfType eotfType;
    enum HdmiStaticMetadataType metadataType;
    union HdmiStaticMetadataDescriptor des;
};

/* GBD(gamut boundary description) */
union HdmiGbdHb1 {
    struct {
        uint8_t affectedGamutSeqNum : 4;  /* [0:3] */
        uint8_t gbdProfil : 3;  /* [4:5] */
        uint8_t reserved : 1;   /* [6] */
        uint8_t noCrntGbd : 1;  /* [7] */
    } bits;
    uint8_t val;
};

union HdmiGbdHb2 {
    struct {
        uint8_t currentGamutSeqNum : 4;  /* [0:3] */
        uint8_t packetSeq : 2;  /* [4:5] */
        uint8_t nextField : 1;  /* [7] */
    } bits;
    uint8_t val;
};

union HdmiGbdRangeData0 {
    struct {
        uint8_t gbdColorSpace : 3; /* [0:2] */
        uint8_t gbdColorPercision : 2; /* [3:4] */
        uint8_t rsvd : 2; /* [5:6] */
        uint8_t formatFlag : 1; /* [7] */
    } bits;
    uint8_t val;
};

struct HdmiGbdPacket {
    uint8_t hb0;
    union HdmiGbdHb1 hb1;
    union HdmiGbdHb2 hb2;
    union HdmiGbdRangeData0 data0;
    uint8_t minRedDataH;
    uint8_t midRedData;  /* Min_Red_Data_L | Max_Red_Data_H */
    uint8_t maxRedDataL;
    uint8_t minGreenDataH;
    uint8_t midGreenData;  /* Min_Green_Data_L | Max_Green_Data_H */
    uint8_t maxGreenDataL;
    uint8_t minBlueDataH;
    uint8_t midBlueData;  /* Min_Blue_Data_L | Max_Blue_Data_H */
    uint8_t maxBlueDataL;
};

union HdmiInfoFrameInfo {
    struct HdmiInfoFrameHeader header;
    struct HdmiVsInfoFrame vs;
    struct HdmiAviInfoFrame avi;
    struct HdmiAudioInfoFrame audio;
    struct HdmiSpdInfoFrame spd;
    struct HdmiDrmInfoFrame drm;
    struct HdmiGbdPacket gbd;
};

/*
 * InfoFrame: A data structure defined in CEA-861-D that is designed to carry a variety of auxiliary data items
 * regarding the audio or video streams or the source device and is carried from Source to Sink across HDMI.
 */
struct HdmiInfoFrame {
    enum HdmiPacketType infoFrameType;
    bool hdrSupport;
    struct HdmiVsInfoFrame vs;
    struct HdmiAviInfoFrame avi;
    struct HdmiAudioInfoFrame audio;
    struct HdmiSpdInfoFrame spd;
    struct HdmiDrmInfoFrame drm;
    struct HdmiGbdPacket gbd;
    struct HdmiVsUserVsifContent userVsif;
    void *priv;
};

struct HdmiInfoFrameStatus {
    bool vsifEnable;
    bool aviEnable;
    bool audioEnable;
    bool spdEnable;
    bool drmEnable;
    bool gdbEnable;
    uint8_t avi[HDMI_INFOFRAME_LEN];
    uint8_t audio[HDMI_INFOFRAME_LEN];
    uint8_t vsif[HDMI_INFOFRAME_LEN];
    uint8_t spd[HDMI_INFOFRAME_LEN];
    uint8_t drm[HDMI_INFOFRAME_LEN];
    uint8_t gdb[HDMI_INFOFRAME_LEN];
};

int32_t HdmiAudioInfoFrameSend(struct HdmiInfoFrame *frame, bool enable);
int32_t HdmiAviInfoFrameSend(struct HdmiInfoFrame *frame, bool enable);
int32_t HdmiDrmInfoFrameSend(struct HdmiInfoFrame *frame, bool enable);
int32_t HdmiVsInfoFrameSend(struct HdmiInfoFrame *frame, bool enable, bool dolbyEnable);
int32_t HdmiSpdInfoFrameSend(struct HdmiInfoFrame *frame, bool enable,
    const char *vendorName, const char *productName, enum HdmiSpdSdi sdi);
int32_t HdmiInfoFrameGetInfo(struct HdmiInfoFrame *frame, enum HdmiPacketType type,
    union HdmiInfoFrameInfo *infoFrame);
int32_t HdmiInfoFrameSetInfo(struct HdmiInfoFrame *frame, enum HdmiPacketType type,
    union HdmiInfoFrameInfo *infoFrame);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HDMI_INFOFRAME_H */
