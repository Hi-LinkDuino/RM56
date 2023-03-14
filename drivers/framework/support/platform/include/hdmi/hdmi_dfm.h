/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDMI_DFM_H
#define HDMI_DFM_H

#include "hdmi_common.h"
#include "hdmi_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*
 * DFM(Data Flow Metering).
 * During data transmission in FRL mode, there is a lot of bandwidth redundancy (blanking period) or slight bandwidth
 * insufficiency (active video). In order for the link to tolerate these fluctuations, source and sink need to use
 * buffers. To limit the size of the buffer required, the protocol adds the Data Flow Metering requirement.
 */
#define HDMI_DFM_FRL_CHAR_NUM_PER_CB 502
#define HDMI_DFM_RS_NUM_PER_CB 8
/* A FRL Character Block(CB) contains 502 FRL Characters and 8 RS check characters. */
#define HDMI_DFM_CHAR_NUM_PER_CB (HDMI_DFM_FRL_CHAR_NUM_PER_CB + HDMI_DFM_RS_NUM_PER_CB)

/*
 * A FRL Super Block(SB) contains four FRL Character Blocks.
 * Each FRL Super Block is preceded by a group of three or four Start Super Blocks (SSB) or a group of three or
 * four Scrambler Reset (SR) characters.
 */
#define HDMI_DFM_FRL_CB_NUM_PER_SB 4
#define HDMI_DFM_FRL_SB_LEN(lane) (HDMI_DFM_FRL_CB_NUM_PER_SB * HDMI_DFM_CHAR_NUM_PER_CB + (lane))

#define HDMI_DFM_FRL_PIXELCLK_TOLERANCE 5 /* 0.50% */
#define HDMI_DFM_FRL_BITRATE_TOLERANCE 3  /* 300 */
#define HDMI_DFM_FRL_AUDIOCLK_TOLERANCE 1
#define HDMI_DFM_MAGNIFICATION_8 8
#define HDMI_DFM_FRL_MAX_TB_BORROW 400
#define HDMI_DFM_RATE_MAGNIFICATION 100000
#define HDMI_DFM_OVERHEAD_SIZE 300

#define HDMI_AUDIO_CHANNEL_ALLOC_TYPE1 1
#define HDMI_AUDIO_CHANNEL_ALLOC_TYPE2 2
#define HDMI_AUDIO_CHANNEL_ALLOC_TYPE3 3

/*
 * Packets per sample.
 * This is the number of audio packets required to carry each sample.
 * x100.
 */
#define HDMI_AUDIO_AP_SIZE_25  25
#define HDMI_AUDIO_AP_SIZE_100 100
#define HDMI_AUDIO_AP_SIZE_200 200
#define HDMI_AUDIO_AP_SIZE_300 300
#define HDMI_AUDIO_AP_SIZE_400 400

enum HdmiAudioPacketType {
    HDMI_AUDIO_SAMPLE_PACKET = 0x02,
    HDMI_ONE_BIT_AUDIO_SAMPLE_PACKET = 0x07,
    HDMI_DTS_AUDIO_PACKET = 0x08,
    HDMI_HBR_AUDIO_PACKET = 0x09,
    HDMI_AUDIO_3D_SAMPLE_PACKET = 0x0B,
    HDMI_ONE_BIT_AUDIO_3D_SAMPLE_PACKET = 0x0C,
    HDMI_MULTI_STREAM_AUDIO_SAMPLE_PACKET = 0x0E,
    HDMI_ONE_BIT_MULTI_STREAM_AUDIO_SAMPLE_PACKET = 0x0F,
};

enum HdmiDfmPixelFormatMode {
    HDMI_DFM_PIXEL_FORMAT_MODE_0 = 0,
    HDMI_DFM_PIXEL_FORMAT_MODE_1,
    HDMI_DFM_PIXEL_FORMAT_MODE_2,
    HDMI_DFM_PIXEL_FORMAT_MODE_3
};

struct HdmiDfmInfo {
    bool audioSupport;
    bool videoSupport;
    bool uncompressSupport;
    bool isExtraMode;
    bool canbeTrans;
    uint32_t htotal;
    uint32_t vtotal;
    uint32_t bpp;     /* Bits per Pixel */
    uint32_t cFrlSb;  /* Total FRL Characters Per SuperBlock(include SSB/SR) */
    uint32_t overheadSb;
    uint32_t overheadRs;
    uint32_t overheadMap;
    uint32_t overheadMin;
    uint32_t overheadMax;
    uint32_t lineMinTime;  /* minimum Video Line period */
    uint32_t lineMaxTime;  /* maximum Video Line period */
    uint32_t audioAp;      /* the number of audio packets required to carry each sample */
    uint32_t audioRap;     /* Average Audio Related Packet Rate */
    uint32_t avgAudioPacketsPerLine;  /* Average Required Packets per line */
    uint32_t audioPacketsLine;        /* Packets per Hblank that must be supportable. */
    uint32_t hblankAudioMin;          /*  */
    uint32_t cFrlFree;
    uint32_t cFrlRcMargin;
    uint32_t cFrlRcSavings;
    uint32_t activeBytesPerLine;  /* Video Bytes per line */
    uint32_t activeTbPerLine;     /* Required Characters to carry Active Video per line */
    uint32_t hblankTbPerLine;      /* Required Characters to carry H-Blank Video per line */
    uint32_t activeTimeRef;
    uint32_t blankTimeRef;
    uint32_t activeTimeMin;
    uint32_t blankTimeMin;
    int32_t tBorrow;
    int32_t tbBorrow;
    uint32_t cFrlActualPayload;
    uint32_t utilization;
    int32_t margin;
    uint32_t avgTbRate;
    uint64_t pixelClk;
    uint64_t maxPixelClk;        /* maximum legal pixel rate */
    uint64_t minPixelClk;        /* minimum legal pixel rate */
    uint64_t maxBitRate;         /* FRL max bit rate */
    uint64_t minBitRate;         /* FRL min bit rate */
    uint64_t maxFrlCharRate;     /* max FRL Character Rate */
    uint64_t minFrlCharRate;     /* min FRL Character Rate */
    uint32_t maxFrlCharsPerLine; /* max FRL Characters per line Period */
    uint32_t minFrlCharsPerLine; /* min FRL Characters per line Period */
};

struct HdmiDfmParam {
    uint32_t hactive;
    uint32_t vactive;
    uint32_t hblank;
    uint32_t vblank;
    uint32_t hsync;
    uint32_t hback;
    uint32_t hfront;
    uint32_t vsync;
    uint32_t vback;
    uint32_t vfront;
    uint32_t vfreq;
    uint32_t colorDepth;
    uint32_t pixelFormat; /* 1:420, 2:422, 3:RGB444 */
    uint32_t laneNum;
    uint32_t bitRate;
    uint32_t audioRate;
    uint32_t packetType;
    uint32_t layout;
    uint32_t acat;
};

uint32_t HdmiDfmGetPixelFormat(enum HdmiColorSpace colorSpace);
void HdmiDfmFillParam(struct HdmiDfmParam *param, struct HdmiVideoDefInfo *videoInfo,
    struct HdmiAudioAttr *audioAttr, enum HdmiColorSpace colorSpace, enum HdmiDeepColor deepColor);
bool HdmiDfmFormatSupport(struct HdmiDfmParam *param);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HDMI_DFM_H */
