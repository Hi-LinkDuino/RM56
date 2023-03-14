/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef A2DP_CODEC_CONSTANT_H
#define A2DP_CODEC_CONSTANT_H

#include <cstddef>
#include <cstdint>

#define A2DP_CODEC_SIZE 20
#define A2DP_MEDIA_TYPE_AUDIO 0
#define A2DP_MEDIA_TYPE_VIDEO 1
#define MAX_BUFFER_SIZE 19200

#define A2DP_CODEC_TYPE_INDEX 2
#define A2DP_CODEC_SBC_INFO_LEN 6
#define A2DP_CODEC_AAC_INFO_LEN 8

#define A2DP_SBC_MIN_BITPOOL 2
#define A2DP_SBC_MAX_BITPOOL 250

#define A2DP_AAC_DEFAULT_BITRATE 320000  // 320 kbps
#define A2DP_AAC_MIN_BITRATE 64000       // 64 kbps

#define A2DP_SUCCESS 0
#define A2DP_BADPARAM (-1)

enum A2dpCodecType : uint8_t {
    A2DP_CODEC_TYPE_SBC = 0,
    A2DP_CODEC_TYPE_MPEG1 = 0x01,
    A2DP_CODEC_TYPE_AAC = 0x01 << 1,
    A2DP_CODEC_TYPE_ATRAC = 0x01 << 2,
    A2DP_CODEC_TYPE_NONA2DP = 0xFF
};

enum A2dpCodecIndex : uint8_t {
    A2DP_SOURCE_CODEC_INDEX_SBC = 1,
    A2DP_SOURCE_CODEC_INDEX_AAC,
    A2DP_SOURCE_CODEC_INDEX_MAX,
    A2DP_SINK_CODEC_INDEX_MIN = A2DP_SOURCE_CODEC_INDEX_MAX,
    A2DP_SINK_CODEC_INDEX_SBC = A2DP_SINK_CODEC_INDEX_MIN,
    A2DP_SINK_CODEC_INDEX_AAC,
    A2DP_SINK_CODEC_INDEX_MAX,
    A2DP_CODEC_INDEX_MIN = A2DP_SOURCE_CODEC_INDEX_SBC,
    A2DP_CODEC_INDEX_MAX = A2DP_SINK_CODEC_INDEX_MAX
};

enum A2dpCodecPriority : uint32_t {
    A2DP_CODEC_PRIORITY_DISABLED = 0,
    A2DP_CODEC_PRIORITY_DEFAULT,
    A2DP_CODEC_PRIORITY_HIGHEST = 1000 * 1000,
};

enum A2dpCodecSampeRate : uint32_t {
    A2DP_SAMPLE_RATE_NONE = 0x0,
    A2DP_SBC_SAMPLE_RATE_48000 = 0x1 << 4, /* octet0 b4 */
    A2DP_SBC_SAMPLE_RATE_44100 = 0x1 << 5, /* octet0 b5 */
    A2DP_SBC_SAMPLE_RATE_32000 = 0x1 << 6, /* octet0 b6 */
    A2DP_SBC_SAMPLE_RATE_16000 = 0x1 << 7, /* octet0 b7 */
    A2DP_SBC_SAMPLE_RATE_MSK = 0xF0,
    A2DP_AAC_SAMPLE_RATE_OCTET1_44100 = 0x01,
    A2DP_AAC_SAMPLE_RATE_OCTET1_32000 = 0x01 << 1,
    A2DP_AAC_SAMPLE_RATE_OCTET1_24000 = 0x01 << 2,
    A2DP_AAC_SAMPLE_RATE_OCTET1_22050 = 0x01 << 3,
    A2DP_AAC_SAMPLE_RATE_OCTET1_16000 = 0x01 << 4,
    A2DP_AAC_SAMPLE_RATE_OCTET1_12000 = 0x01 << 5,
    A2DP_AAC_SAMPLE_RATE_OCTET1_11025 = 0x01 << 6,
    A2DP_AAC_SAMPLE_RATE_OCTET1_8000 = 0x01 << 7,
    A2DP_AAC_SAMPLE_RATE_OCTET1_MSK = 0xFF,
    A2DP_AAC_SAMPLE_RATE_OCTET2_96000 = 0x01 << 12,
    A2DP_AAC_SAMPLE_RATE_OCTET2_88200 = 0x01 << 13,
    A2DP_AAC_SAMPLE_RATE_OCTET2_64000 = 0x01 << 14,
    A2DP_AAC_SAMPLE_RATE_OCTET2_48000 = 0x01 << 15,
    A2DP_AAC_SAMPLE_RATE_OCTET2_MSK = 0xF0,
};

enum A2dpCodecChannelMode : uint8_t {
    A2DP_CHANNEL_MODE_NONE = 0x0,
    A2DP_SBC_CHANNEL_MODE_JOINT_STEREO = 0x1, /* octet0 b0 */
    A2DP_SBC_CHANNEL_MODE_STEREO = 0x1 << 1,  /* octet0 b1 */
    A2DP_SBC_CHANNEL_MODE_DUAL = 0x1 << 2,    /* octet0 b2 */
    A2DP_SBC_CHANNEL_MODE_MONO = 0x1 << 3,    /* octet0 b3 */
    A2DP_SBC_CHANNEL_MODE_MSK = 0x0F,
    A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE = 0x01 << 2, /* octet2 b2 */
    A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE = 0x01 << 3, /* octet2 b3 */
    A2DP_AAC_CHANNEL_MODE_OCTET2_MSK = 0x0C,
};

enum A2dpCodecBlock : uint8_t {
    A2DP_SBC_BLOCKS_16 = 0x1 << 4, /* octet1 b4: 16 */
    A2DP_SBC_BLOCKS_12 = 0x1 << 5, /* octet1 b5: 12 */
    A2DP_SBC_BLOCKS_8 = 0x1 << 6,  /* octet1 b6: 8 */
    A2DP_SBC_BLOCKS_4 = 0x1 << 7,  /* octet1 b7: 4 */
    A2DP_SBC_BLOCKS_MSK = 0xF0
};

enum A2dpCodecSubBannds : uint8_t {
    A2DP_SBC_SUBBAND_8 = 0x1 << 2, /* octet1 b2: 8 */
    A2DP_SBC_SUBBAND_4 = 0x1 << 3, /* octet1 b3: 4 */
    A2DP_SBC_SUBBAND_MSK = 0x0C
};

enum A2dpCodecAllocMode : uint8_t {
    A2DP_SBC_ALLOC_MODE_L = 0x1,      /* octet1 b0: loudness */
    A2DP_SBC_ALLOC_MODE_S = 0x1 << 1, /* octet1 b1: SNR */
    A2DP_SBC_ALLOC_MODE_MSK = 0x03
};

enum A2dpCodecBitsPerSample : uint32_t {
    A2DP_SAMPLE_BITS_NONE = 0x0,
    A2DP_SAMPLE_BITS_16 = 0x1 << 0,
    A2DP_SAMPLE_BITS_24 = 0x1 << 1,
    A2DP_SAMPLE_BITS_32 = 0x1 << 2,
    A2DP_SAMPLE_BITS_MSK = 0x06,
    A2DP_AAC_SAMPLE_BITS_OCTET3_BIT0 = 0x01 << 16, /* octet3 b0 */
    A2DP_AAC_SAMPLE_BITS_OCTET3_BIT1 = 0x01 << 17, /* octet3 b1 */
    A2DP_AAC_SAMPLE_BITS_OCTET3_BIT2 = 0x01 << 18, /* octet3 b2 */
    A2DP_AAC_SAMPLE_BITS_OCTET3_BIT3 = 0x01 << 19, /* octet3 b3 */
    A2DP_AAC_SAMPLE_BITS_OCTET3_BIT4 = 0x01 << 20, /* octet3 b4 */
    A2DP_AAC_SAMPLE_BITS_OCTET3_BIT5 = 0x01 << 21, /* octet3 b5 */
    A2DP_AAC_SAMPLE_BITS_OCTET3_BIT6 = 0x01 << 22, /* octet3 b6 */
    A2DP_AAC_SAMPLE_BITS_OCTET4_BIT0 = 0x01 << 8, /* octet4 b0 */
    A2DP_AAC_SAMPLE_BITS_OCTET4_BIT1 = 0x01 << 9, /* octet4 b1 */
    A2DP_AAC_SAMPLE_BITS_OCTET4_BIT2 = 0x01 << 10, /* octet4 b2 */
    A2DP_AAC_SAMPLE_BITS_OCTET4_BIT3 = 0x01 << 11, /* octet4 b3 */
    A2DP_AAC_SAMPLE_BITS_OCTET4_BIT4 = 0x01 << 12, /* octet4 b4 */
    A2DP_AAC_SAMPLE_BITS_OCTET4_BIT5 = 0x01 << 13, /* octet4 b5 */
    A2DP_AAC_SAMPLE_BITS_OCTET4_BIT6 = 0x01 << 14, /* octet4 b6 */
    A2DP_AAC_SAMPLE_BITS_OCTET4_BIT7 = 0x01 << 15, /* octet4 b6 */
    A2DP_AAC_SAMPLE_BITS_OCTET5_BIT0 = 0x01,      /* octet5 b0 */
    A2DP_AAC_SAMPLE_BITS_OCTET5_BIT1 = 0x01 << 1, /* octet5 b1 */
    A2DP_AAC_SAMPLE_BITS_OCTET5_BIT2 = 0x01 << 2, /* octet5 b2 */
    A2DP_AAC_SAMPLE_BITS_OCTET5_BIT3 = 0x01 << 3, /* octet5 b3 */
    A2DP_AAC_SAMPLE_BITS_OCTET5_BIT4 = 0x01 << 4, /* octet5 b4 */
    A2DP_AAC_SAMPLE_BITS_OCTET5_BIT5 = 0x01 << 5, /* octet5 b5 */
    A2DP_AAC_SAMPLE_BITS_OCTET5_BIT6 = 0x01 << 6, /* octet5 b6 */
    A2DP_AAC_SAMPLE_BITS_OCTET5_BIT7 = 0x01 << 7, /* octet5 b6 */
};

enum A2dpCodecObjectType : uint8_t {
    A2DP_AAC_MPEG4_SCALABLE_OBJECT_TYPE = 0x01 << 4, /* octet0 b4 */
    A2DP_AAC_MPEG4_LTP_OBJECT_TYPE = 0x01 << 5,      /* octet0 b5 */
    A2DP_AAC_MPEG4_LC_OBJECT_TYPE = 0x01 << 6,       /* octet0 b6 */
    A2DP_AAC_MPEG2_OBJECT_TYPE = 0x01 << 7,          /* octet0 b7s */
};

enum A2dpCodecVBR : uint8_t {
    A2DP_AAC_VARIABLE_BIT_RATE_DISABLED = 0,
    A2DP_AAC_VARIABLE_BIT_RATE_ENABLED = 0x80, /* octet3 b7 */
};

/* data type for the SBC Codec Information Element */
struct A2dpSBCCapability {
    uint32_t sampleFreq;     /* Sampling frequency */
    uint8_t channelMode;     /* Channel mode */
    uint8_t blockLen;        /* Block length */
    uint8_t numSubbands;     /* Number of subbands */
    uint8_t allocatedMethod; /* Allocation method */
    uint8_t minBitpool;      /* Minimum bitpool */
    uint8_t maxBitpool;      /* Maximum bitpool */
    uint8_t bitsPerSample;
};

/* SBC Source codec capabilities */
static const A2dpSBCCapability A2DP_SBC_SOURCE_CAPS = {
    (A2DP_SBC_SAMPLE_RATE_48000 | A2DP_SBC_SAMPLE_RATE_44100),                         /* sampleFreq */
    (A2DP_SBC_CHANNEL_MODE_MONO | A2DP_SBC_CHANNEL_MODE_STEREO),                       /* channelMode */
    (A2DP_SBC_BLOCKS_16 | A2DP_SBC_BLOCKS_12 | A2DP_SBC_BLOCKS_8 | A2DP_SBC_BLOCKS_4), /* blockLen */
    (A2DP_SBC_SUBBAND_4 | A2DP_SBC_SUBBAND_8),                                         /* numSubbands */
    (A2DP_SBC_ALLOC_MODE_L | A2DP_SBC_ALLOC_MODE_S),                                   /* allocatedMethod */
    A2DP_SBC_MIN_BITPOOL,                                                              /* minBitpool */
    A2DP_SBC_MAX_BITPOOL,                                                              /* maxBitpool */
    A2DP_SAMPLE_BITS_16                                                                /* bitsPerSample */
};

/* SBC Sink codec capabilities */
static const A2dpSBCCapability A2DP_SBC_SINK_CAPS = {
    (A2DP_SBC_SAMPLE_RATE_48000 | A2DP_SBC_SAMPLE_RATE_44100),                         /* sampleFreq */
    (A2DP_SBC_CHANNEL_MODE_MONO | A2DP_SBC_CHANNEL_MODE_STEREO),                       /* channelMode */
    (A2DP_SBC_BLOCKS_16 | A2DP_SBC_BLOCKS_12 | A2DP_SBC_BLOCKS_8 | A2DP_SBC_BLOCKS_4), /* blockLen */
    (A2DP_SBC_SUBBAND_4 | A2DP_SBC_SUBBAND_8),                                         /* numSubbands */
    (A2DP_SBC_ALLOC_MODE_L | A2DP_SBC_ALLOC_MODE_S),                                   /* allocatedMethod */
    A2DP_SBC_MIN_BITPOOL,                                                              /* minBitpool */
    A2DP_SBC_MAX_BITPOOL,                                                              /* maxBitpool */
    A2DP_SAMPLE_BITS_16                                                                /* bitsPerSample */
};

/* Default SBC codec configuration */
const A2dpSBCCapability A2DP_SBC_DEFAULT_CONFIG = {
    A2DP_SBC_SAMPLE_RATE_44100,   /* sampleFreq */
    A2DP_SBC_CHANNEL_MODE_MONO, /* channelMode */
    A2DP_SBC_BLOCKS_16,           /* blockLen */
    A2DP_SBC_SUBBAND_8,           /* numSubbands */
    A2DP_SBC_ALLOC_MODE_L,        /* allocatedMethod */
    A2DP_SBC_MIN_BITPOOL,         /* minBitpool */
    A2DP_SBC_MAX_BITPOOL,         /* maxBitpool */
    A2DP_SAMPLE_BITS_16           /* bitsPerSample */
};

struct A2dpEncoderInitPeerParams {
    bool isPeerEdr;
    bool peerSupports3mbps;
    uint16_t peermtu;
};

/* data type for the AAC Codec Information Element */
struct A2dpAACCapability {
    uint8_t objectType;
    uint32_t sampleFreq;
    uint8_t channelMode;
    uint8_t variableBitRateSupport;
    uint32_t bitRate;
    uint8_t bitsPerSample;
};

/* AAC Source codec capabilities */
const A2dpAACCapability A2DP_AAC_SOURCE_CAPS = {
    A2DP_AAC_MPEG2_OBJECT_TYPE,
    A2DP_AAC_SAMPLE_RATE_OCTET1_44100 | A2DP_AAC_SAMPLE_RATE_OCTET2_48000,
    A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE | A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE,
    A2DP_AAC_VARIABLE_BIT_RATE_DISABLED,
    A2DP_AAC_DEFAULT_BITRATE,
    A2DP_SAMPLE_BITS_16
};

/* SBC Sink codec capabilities */
const A2dpAACCapability A2DP_AAC_SINK_CAPS = {
    A2DP_AAC_MPEG2_OBJECT_TYPE,
    (A2DP_AAC_SAMPLE_RATE_OCTET1_44100 | A2DP_AAC_SAMPLE_RATE_OCTET2_48000),
    (A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE | A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE),
    A2DP_AAC_VARIABLE_BIT_RATE_ENABLED,
    A2DP_AAC_DEFAULT_BITRATE,
    A2DP_SAMPLE_BITS_16
};

/* Default SBC codec configuration */
const A2dpAACCapability A2DP_AAC_DEFAULT_CONFIG = {
    A2DP_AAC_MPEG2_OBJECT_TYPE,
    A2DP_AAC_SAMPLE_RATE_OCTET1_44100,
    A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE,
    A2DP_AAC_VARIABLE_BIT_RATE_DISABLED,
    A2DP_AAC_DEFAULT_BITRATE,
    A2DP_SAMPLE_BITS_16
};
#endif  // ! A2DP_CODEC_CONSTANT_H