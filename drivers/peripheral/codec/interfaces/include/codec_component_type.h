/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

/**
 * @addtogroup Codec
 * @{
 *
 * @brief Defines APIs related to the Codec module.
 *
 * The Codec module provides APIs for initializing the custom data and audio and video codecs,
 * setting codec parameters, and controlling and transferring data.
 *
 * @since 3.1
 */

/**
 * @file codec_component_type.h
 *
 * @brief Declares custom data types used in the Codec module APIs, including the codec types,
 * audio and video parameters, and buffers.
 *
 * @since 3.1
 */

#ifndef CODEC_COMPONENT_TYPE_H
#define CODEC_COMPONENT_TYPE_H

#include <stdint.h>
#include <stdbool.h>
#include "OMX_Types.h"
#include "OMX_Index.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Defines the maximum value of the sampling format.
 */
#define SAMPLE_FMT_NUM 32

/**
 * @brief Enumerates the codec types.
 */
typedef enum {
    /** Video decoder */
    VIDEO_DECODER,
    /** Video encoder */
    VIDEO_ENCODER,
    /** Audio decoder */
    AUDIO_DECODER,
    /** Audio encoder */
    AUDIO_ENCODER,
    /** Invalid type */
    INVALID_TYPE
} CodecType;

/**
 * @brief Enumerates the types of audio and video encoding/decoding components.
 */
typedef enum {
    /** JPEG image */
    MEDIA_ROLETYPE_IMAGE_JPEG = 0,
    /** H.264 video */
    MEDIA_ROLETYPE_VIDEO_AVC,
    /** H.265 video */
    MEDIA_ROLETYPE_VIDEO_HEVC,
    /** Audio codec */
    MEDIA_ROLETYPE_AUDIO_FIRST = 0x10000,
    /** Advanced Audio Coding (AAC) */
    MEDIA_ROLETYPE_AUDIO_AAC = 0x10000,
    /** G711A audio */
    MEDIA_ROLETYPE_AUDIO_G711A,
    /** G711U audio */
    MEDIA_ROLETYPE_AUDIO_G711U,
    /** G726 audio */
    MEDIA_ROLETYPE_AUDIO_G726,
    /** Pulse-Code Modulation (PCM) audio */
    MEDIA_ROLETYPE_AUDIO_PCM,
    /** MP3 */
    MEDIA_ROLETYPE_AUDIO_MP3,
    /** Invalid type */
    MEDIA_ROLETYPE_INVALID,
} AvCodecRole;

/**
 * @brief Enumerates the codec profiles.
 */
typedef enum {
    /** Invalid profile */
    INVALID_PROFILE = 0,
    /** AAC-Low Complex */
    AAC_LC_PROFILE = 0x1000,
    /** AAC-Main */
    AAC_MAIN_PROFILE,
    /** HEAAC, AAC+, or AACPlusV1 */
    AAC_HE_V1_PROFILE,
    /** AAC++ or AACPlusV2 */
    AAC_HE_V2_PROFILE,
    /** AAC-Low Delay */
    AAC_LD_PROFILE,
    /** AAC-Enhanced Low Delay */
    AAC_ELD_PROFILE,
    /** H.264 Baseline */
    AVC_BASELINE_PROFILE = 0x2000,
    /** H.264 Main */
    AVC_MAIN_PROFILE,
    /** H.264 High */
    AVC_HIGH_PROFILE,
    /** H.265 Main */
    HEVC_MAIN_PROFILE = 0x3000,
    /** H.265 Main 10 */
    HEVC_MAIN_10_PROFILE,
} Profile;

/**
* @brief Defines the alignment.
 */
typedef struct {
    int32_t widthAlignment; /** Value to align with the width */
    int32_t heightAlignment; /** Value to align with the height */
} Alignment;

/**
 * @brief Defines a rectangle.
 */
typedef struct {
    int32_t width;  /** Width of the rectangle */
    int32_t height; /** Height of the rectangle */
} Rect;

/**
 * @brief Defines a value range.
 */
typedef struct {
    int32_t min; /** Minimum value */
    int32_t max; /** Maximum value */
} RangeValue;

/**
 * @brief Enumerates the playback capabilities.
 */
typedef enum {
    /** Adaptive playback */
    CODEC_CAP_ADAPTIVE_PLAYBACK = 0x1,
    /** Secure playback */
    CODEC_CAP_SECURE_PLAYBACK = 0x2,
    /** Tunnel playback */
    CODEC_CAP_TUNNEL_PLAYBACK = 0x4,
    /** Multi-plane (video image plane and audio tunnel plane) playback */
    CODEC_CAP_MULTI_PLANE = 0x10000,
} CodecCapsMask;

/**
 * @brief Enumerates the audio sampling rates.
 */
typedef enum {
    /** 8 kHz */
    AUD_SAMPLE_RATE_8000   = 8000,
    /** 12 kHz */
    AUD_SAMPLE_RATE_12000  = 12000,
    /** 11.025 kHz */
    AUD_SAMPLE_RATE_11025  = 11025,
    /** 16 kHz */
    AUD_SAMPLE_RATE_16000  = 16000,
    /** 22.050 kHz */
    AUD_SAMPLE_RATE_22050  = 22050,
    /** 24 kHz */
    AUD_SAMPLE_RATE_24000  = 24000,
    /** 32 kHz */
    AUD_SAMPLE_RATE_32000  = 32000,
    /** 44.1 kHz */
    AUD_SAMPLE_RATE_44100  = 44100,
    /** 48 kHz */
    AUD_SAMPLE_RATE_48000  = 48000,
    /** 64 kHz */
    AUD_SAMPLE_RATE_64000  = 64000,
    /** 96 kHz */
    AUD_SAMPLE_RATE_96000  = 96000,
    /** Invalid sampling rate */
    AUD_SAMPLE_RATE_INVALID,
} AudioSampleRate;

/**
 * @brief Enumerate the audio sampling formats.
 *
 * For the planar sampling format, the data of each channel is independently stored in data.
 * For the packed sampling format, only the first data is used, and the data of each channel is interleaved.
 */
typedef enum {
    /** Unsigned 8 bits, packed */
    AUDIO_SAMPLE_FMT_U8,
    /** Signed 16 bits, packed */
    AUDIO_SAMPLE_FMT_S16,
    /** Signed 32 bits, packed */
    AUDIO_SAMPLE_FMT_S32,
    /** Float, packed */
    AUDIO_SAMPLE_FMT_FLOAT,
    /** Double, packed */
    AUDIO_SAMPLE_FMT_DOUBLE,
    /** Unsigned 8 bits, planar */
    AUDIO_SAMPLE_FMT_U8P,
    /** Signed 16 bits, planar */
    AUDIO_SAMPLE_FMT_S16P,
    /** Signed 32 bits, planar */
    AUDIO_SAMPLE_FMT_S32P,
    /** Float, planar */
    AUDIO_SAMPLE_FMT_FLOATP,
    /** Double, planar */
    AUDIO_SAMPLE_FMT_DOUBLEP,
    /** Invalid sampling format */
    AUDIO_SAMPLE_FMT_INVALID,
} AudioSampleFormat;

/**
 * @brief Defines the video encoding and decoding capabilities.
 */
#define PIX_FORMAT_NUM 16 /** Size of the supported pixel format array */
typedef struct {
    Rect minSize;                            /** Minimum resolution supported. */
    Rect maxSize;                            /** Maximum resolution supported. */
    Alignment whAlignment;                   /** Values to align with the width and height. */
    RangeValue blockCount;                   /** Number of blocks supported. */
    RangeValue blocksPerSecond;              /** Number of blocks processed per second. */
    Rect blockSize;                          /** Block size supported. */
    int32_t supportPixFmts[PIX_FORMAT_NUM];  /** Supported pixel format. For details,
                                                 see {@link OMX_COLOR_FORMATTYPE}. */
} VideoPortCap;

/**
 * @brief Defines the video encoding and decoding capabilities.
 */
#define SAMPLE_FORMAT_NUM 12 /** Size of the audio sampling format array supported. */
#define SAMPLE_RATE_NUM 16 /** Size of the audio sampling rate array supported. */
#define CHANNEL_NUM 16 /** Size of the audio channel array supported. */
typedef struct {
    int32_t sampleFormats[SAMPLE_FMT_NUM]; /** Supported audio sampling formats. For details,
                                               see {@link AudioSampleFormat}. */
    int32_t sampleRate[SAMPLE_RATE_NUM];   /** Supported audio sampling rates. For details,
                                               see {@link AudioSampleRate}. */
    int32_t channelLayouts[CHANNEL_NUM];   /** Supported audio channel layouts. */
    int32_t channelCount[CHANNEL_NUM];     /** Supported audio channels. */
} AudioPortCap;

/**
 * @brief Defines the audio and video encoding and decoding capabilities.
 */
typedef union {
    VideoPortCap video;               /** Video encoding and decoding capabilities */
    AudioPortCap audio;               /** Audio encoding and decoding capabilities */
} PortCap;

/**
 * @brief Enumerates the codec processing modes.
 */
typedef enum {
    /** Input buffer in sync mode */
    PROCESS_BLOCKING_INPUT_BUFFER     = 0X1,
    /** Output buffer in sync mode */
    PROCESS_BLOCKING_OUTPUT_BUFFER    = 0X2,
    /** Control flow in sync mode */
    PROCESS_BLOCKING_CONTROL_FLOW     = 0X4,
    /** Input buffer in async mode */
    PROCESS_NONBLOCKING_INPUT_BUFFER  = 0X100,
    /** Output buffer in async mode */
    PROCESS_NONBLOCKING_OUTPUT_BUFFER = 0X200,
    /** Control flow in async mode */
    PROCESS_NONBLOCKING_CONTROL_FLOW  = 0X400,
} CodecProcessMode;

/**
 * @brief Defines the codec capabilities.
 */
#define NAME_LENGTH 32  /** Size of the component name. */
#define PROFILE_NUM 256  /** Size of the profile array supported. */
typedef struct {
    AvCodecRole role;                     /** Media type. */
    CodecType type;                       /** Codec type. */
    char compName[NAME_LENGTH];           /** Codec component name. */
    int32_t supportProfiles[PROFILE_NUM]; /** Supported profiles. For details, see {@link Profile}. */
    int32_t maxInst;                      /** Maximum instance. */
    bool isSoftwareCodec;                 /** Whether it is software codec or hardware codec. */
    int32_t processModeMask;              /** Codec processing mode mask. For details,
                                              see {@link CodecProcessMode}. */
    uint32_t capsMask;                    /** Codec playback capability mask. For details,
                                              see {@link CodecCapsMask}. */
    RangeValue bitRate;                   /** Supported bit rate range. */
    PortCap port;                         /** Supported audio and video encoding/decoding capabilities. */
} CodecCompCapability;

/**
 * @brief Enumerates the buffer types.
 */
enum BufferType {
    /** Invalid buffer type. */
    BUFFER_TYPE_INVALID = 0,
    /** Virtual address type. */
    BUFFER_TYPE_VIRTUAL_ADDR = 0x1,
    /** Shared memory. */
    BUFFER_TYPE_AVSHARE_MEM_FD = 0x2,
    /** Handle. */
    BUFFER_TYPE_HANDLE = 0x4,
    /** Dynamic handle. */
    BUFFER_TYPE_DYNAMIC_HANDLE = 0x8,
};

/**
 * @brief Enumerate the shared memory types.
 */
enum ShareMemTypes {
    /** Readable and writable shared memory */
    READ_WRITE_TYPE = 0x1,
    /** Readable shared memory */
    READ_ONLY_TYPE = 0x2,
};

/**
 * @brief Defines the codec buffer information.
 */
struct OmxCodecBuffer {
    uint32_t bufferId;               /** Buffer ID. */
    uint32_t size;                   /** Size of the structure. */
    union OMX_VERSIONTYPE version;   /** Component version. */
    enum BufferType bufferType;      /** Buffer type. */
    uint8_t *buffer;                 /** Buffer used for encoding or decoding. */
    uint32_t bufferLen;              /** Size of the buffer. */
    uint32_t allocLen;               /** Size of the buffer allocated. */
    uint32_t filledLen;              /** Size of the buffer filled. */
    uint32_t offset;                 /** Offset to the start position of the valid data in the buffer. */
    int32_t fenceFd;                 /** Fence file descriptor used to signal when the input or
                                         output buffer is ready to consume. */
    enum ShareMemTypes type;         /** Shared memory type. */
    int64_t pts;                     /** Timestamp. */
    uint32_t flag;                   /** Flag. */
};

/**
 * @brief Enumerates the extended codec indexes.
 */
enum OmxIndexCodecExType {
    /** Extended BufferType index */
    OMX_IndexExtBufferTypeStartUnused = OMX_IndexKhronosExtensions + 0x00a00000,
    /** SupportBuffer */
    OMX_IndexParamSupportBufferType,
    /** UseBuffer */
    OMX_IndexParamUseBufferType,
    /** GetBufferHandleUsage */
    OMX_IndexParamGetBufferHandleUsage,
};

/**
 * @brief Defines the <b>SupportBuffer</b>.
 */
struct SupportBufferType {
    uint32_t size;                                          /** Size of the structure */
    union OMX_VERSIONTYPE version;                          /** Component version */
    uint32_t portIndex;                                     /** Port index */
    uint32_t bufferTypes;                                   /** Supported buffer types */
};

/**
 * @brief Define the <b>UseBuffer</b>.
 */
struct UseBufferType {
    uint32_t size;                                         /** Size of the structure */
    union OMX_VERSIONTYPE version;                         /** Component version */
    uint32_t portIndex;                                    /** Port index */
    uint32_t bufferType;                                   /** Buffer type */
};

/**
 * @brief Defines the <b>BufferHandleUsage</b>.
 */
struct GetBufferHandleUsageParams {
    uint32_t size;                                         /** Size of the structure */
    union OMX_VERSIONTYPE version;                         /** Component version */
    uint32_t portIndex;                                    /** Port index */
    uint32_t usage;                                        /** Usage */
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* CODEC_COMPONENT_TYPE_H */
/** @} */