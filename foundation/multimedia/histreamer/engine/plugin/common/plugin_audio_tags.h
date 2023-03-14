/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_PLUGIN_COMMON_AUDIO_TAGS_H
#define HISTREAMER_PLUGIN_COMMON_AUDIO_TAGS_H

#include <cstdint>  // NOLINT: used it

namespace OHOS {
namespace Media {
namespace Plugin {
/**
 * @enum Audio Channel Masks
 *
 * A 64-bit integer with bits set for each channel.
 *
 * @since 1.0
 * @version 1.0
 */
enum AudioChannelMasks : uint64_t {
    FRONT_LEFT              = 1ULL << 0U,
    FRONT_RIGHT             = 1ULL << 1U,
    FRONT_CENTER            = 1ULL << 2U,
    LOW_FREQUENCY           = 1ULL << 3U,
    BACK_LEFT               = 1ULL << 4U,
    BACK_RIGHT              = 1ULL << 5U,
    FRONT_LEFT_OF_CENTER    = 1ULL << 6U,
    FRONT_RIGHT_OF_CENTER   = 1ULL << 7U,
    BACK_CENTER             = 1ULL << 8U,
    SIDE_LEFT               = 1ULL << 9U,
    SIDE_RIGHT              = 1ULL << 10U,
    TOP_CENTER              = 1ULL << 11U,
    TOP_FRONT_LEFT          = 1ULL << 12U,
    TOP_FRONT_CENTER        = 1ULL << 13U,
    TOP_FRONT_RIGHT         = 1ULL << 14U,
    TOP_BACK_LEFT           = 1ULL << 15U,
    TOP_BACK_CENTER         = 1ULL << 16U,
    TOP_BACK_RIGHT          = 1ULL << 17U,
    STEREO_LEFT             = 1ULL << 29U,
    STEREO_RIGHT            = 1ULL << 30U,
    WIDE_LEFT               = 1ULL << 31U,
    WIDE_RIGHT              = 1ULL << 32U,
    SURROUND_DIRECT_LEFT    = 1ULL << 33U,
    SURROUND_DIRECT_RIGHT   = 1ULL << 34U,
    LOW_FREQUENCY_2         = 1ULL << 35U,
    TOP_SIDE_LEFT           = 1ULL << 36U,
    TOP_SIDE_RIGHT          = 1ULL << 37U,
    BOTTOM_FRONT_CENTER     = 1ULL << 38U,
    BOTTOM_FRONT_LEFT       = 1ULL << 39U,
    BOTTOM_FRONT_RIGHT      = 1ULL << 40U,
};


/**
 * @enum Audio Channel Layout
 *
 * Indicates that the channel order in which the user requests decoder output
 * is the native codec channel order.
 *
 * @since 1.0
 * @version 1.0
 */
enum struct AudioChannelLayout : uint64_t {
    UNKNOWN                 = 0,
    MONO                    = (AudioChannelMasks::FRONT_CENTER),
    STEREO                  = (AudioChannelMasks::FRONT_LEFT | AudioChannelMasks::FRONT_RIGHT),
    CH_2POINT1              = (STEREO | AudioChannelMasks::LOW_FREQUENCY),
    CH_2_1                  = (STEREO | AudioChannelMasks::BACK_CENTER),
    SURROUND                = (STEREO | AudioChannelMasks::FRONT_CENTER),
    CH_3POINT1              = (SURROUND | AudioChannelMasks::LOW_FREQUENCY),
    CH_4POINT0              = (SURROUND | AudioChannelMasks::BACK_CENTER),
    CH_4POINT1              = (CH_4POINT0 | AudioChannelMasks::LOW_FREQUENCY),
    CH_2_2                  = (STEREO | AudioChannelMasks::SIDE_LEFT | AudioChannelMasks::SIDE_RIGHT),
    QUAD                    = (STEREO | AudioChannelMasks::BACK_LEFT | AudioChannelMasks::BACK_RIGHT),
    CH_5POINT0              = (SURROUND | AudioChannelMasks::SIDE_LEFT | AudioChannelMasks::SIDE_RIGHT),
    CH_5POINT1              = (CH_5POINT0 | AudioChannelMasks::LOW_FREQUENCY),
    CH_5POINT0_BACK         = (SURROUND | AudioChannelMasks::BACK_LEFT | AudioChannelMasks::BACK_RIGHT),
    CH_5POINT1_BACK         = (CH_5POINT0_BACK | AudioChannelMasks::LOW_FREQUENCY),
    CH_6POINT0              = (CH_5POINT0 | AudioChannelMasks::BACK_CENTER),
    CH_6POINT0_FRONT        = (CH_2_2 | AudioChannelMasks::FRONT_LEFT_OF_CENTER |
                                AudioChannelMasks::FRONT_RIGHT_OF_CENTER),
    HEXAGONAL               = (CH_5POINT0_BACK | AudioChannelMasks::BACK_CENTER),
    CH_6POINT1              = (CH_5POINT1 | AudioChannelMasks::BACK_CENTER),
    CH_6POINT1_BACK         = (CH_5POINT1_BACK | AudioChannelMasks::BACK_CENTER),
    CH_6POINT1_FRONT        = (CH_6POINT0_FRONT | AudioChannelMasks::LOW_FREQUENCY),
    CH_7POINT0              = (CH_5POINT0 | AudioChannelMasks::BACK_LEFT | AudioChannelMasks::BACK_RIGHT),
    CH_7POINT0_FRONT        = (CH_5POINT0 | AudioChannelMasks::FRONT_LEFT_OF_CENTER |
                                AudioChannelMasks::FRONT_RIGHT_OF_CENTER),
    CH_7POINT1              = (CH_5POINT1 | AudioChannelMasks::BACK_LEFT | AudioChannelMasks::BACK_RIGHT),
    CH_7POINT1_WIDE         = (CH_5POINT1 | AudioChannelMasks::FRONT_LEFT_OF_CENTER |
                                AudioChannelMasks::FRONT_RIGHT_OF_CENTER),
    CH_7POINT1_WIDE_BACK    = (CH_5POINT1_BACK | AudioChannelMasks::FRONT_LEFT_OF_CENTER |
                                AudioChannelMasks::FRONT_RIGHT_OF_CENTER),
    CH_3POINT1POINT2        = (CH_3POINT1 | AudioChannelMasks::TOP_FRONT_LEFT | AudioChannelMasks::TOP_FRONT_RIGHT),
    CH_5POINT1POINT2        = (CH_5POINT1 | AudioChannelMasks::TOP_SIDE_LEFT | AudioChannelMasks::TOP_SIDE_RIGHT),
    CH_5POINT1POINT4        = (CH_5POINT1 | AudioChannelMasks::TOP_FRONT_LEFT | AudioChannelMasks::TOP_FRONT_RIGHT |
                                AudioChannelMasks::TOP_BACK_LEFT | AudioChannelMasks::TOP_BACK_RIGHT),
    CH_7POINT1POINT2        = (CH_7POINT1 | AudioChannelMasks::TOP_SIDE_LEFT | AudioChannelMasks::TOP_SIDE_RIGHT),
    CH_7POINT1POINT4        = (CH_7POINT1 | AudioChannelMasks::TOP_FRONT_LEFT | AudioChannelMasks::TOP_FRONT_RIGHT |
                                AudioChannelMasks::TOP_BACK_LEFT | AudioChannelMasks::TOP_BACK_RIGHT),
    CH_9POINT1POINT4        = (CH_7POINT1POINT4 | AudioChannelMasks::WIDE_LEFT |
                                AudioChannelMasks::WIDE_RIGHT),
    CH_9POINT1POINT6        = (CH_9POINT1POINT4 | AudioChannelMasks::TOP_SIDE_LEFT | AudioChannelMasks::TOP_SIDE_RIGHT),
    CH_10POINT2             = (AudioChannelMasks::FRONT_LEFT | AudioChannelMasks::FRONT_RIGHT |
                                AudioChannelMasks::FRONT_CENTER | AudioChannelMasks::TOP_FRONT_LEFT |
                                AudioChannelMasks::TOP_FRONT_RIGHT | AudioChannelMasks::BACK_LEFT |
                                AudioChannelMasks::BACK_RIGHT | AudioChannelMasks::BACK_CENTER |
                                AudioChannelMasks::SIDE_LEFT | AudioChannelMasks::SIDE_RIGHT |
                                AudioChannelMasks::WIDE_LEFT | AudioChannelMasks::WIDE_RIGHT),
    CH_22POINT2             = (CH_7POINT1POINT4 | AudioChannelMasks::FRONT_LEFT_OF_CENTER |
                                AudioChannelMasks::FRONT_RIGHT_OF_CENTER | AudioChannelMasks::BACK_CENTER |
                                AudioChannelMasks::TOP_CENTER | AudioChannelMasks::TOP_FRONT_CENTER |
                                AudioChannelMasks::TOP_BACK_CENTER | AudioChannelMasks::TOP_SIDE_LEFT |
                                AudioChannelMasks::TOP_SIDE_RIGHT | AudioChannelMasks::BOTTOM_FRONT_LEFT |
                                AudioChannelMasks::BOTTOM_FRONT_RIGHT | AudioChannelMasks::BOTTOM_FRONT_CENTER |
                                AudioChannelMasks::LOW_FREQUENCY_2),
    OCTAGONAL               = (CH_5POINT0 | AudioChannelMasks::BACK_LEFT | AudioChannelMasks::BACK_CENTER |
                                AudioChannelMasks::BACK_RIGHT),
    HEXADECAGONAL           = (OCTAGONAL | AudioChannelMasks::WIDE_LEFT | AudioChannelMasks::WIDE_RIGHT |
                                AudioChannelMasks::TOP_BACK_LEFT | AudioChannelMasks::TOP_BACK_RIGHT |
                                AudioChannelMasks::TOP_BACK_CENTER | AudioChannelMasks::TOP_FRONT_CENTER |
                                AudioChannelMasks::TOP_FRONT_LEFT | AudioChannelMasks::TOP_FRONT_RIGHT),
    STEREO_DOWNMIX          = (AudioChannelMasks::STEREO_LEFT | AudioChannelMasks::STEREO_RIGHT),
};

/**
 * @enum Audio sample formats
 *
 * 'S' is signed, 'U' is unsigned, and 'F' is a floating-point number.
 * 'P' is planes， default is interleaved.
 *
 * @since 1.0
 * @version 1.0
 */
enum struct AudioSampleFormat : uint8_t {
    NONE,
    /* 8 bit */
    S8, U8, S8P, U8P,
    /* 16 bit */
    S16, U16, S16P, U16P,
    /* 24 bit */
    S24, U24, S24P, U24P,
    /* 32 bit */
    S32, U32, S32P, U32P,
    /* 64 bit */
    S64, U64, S64P, U64P,
    /* float double */
    F32, F32P, F64, F64P,
};

/**
 * @enum Audio AAC Profile。
 *
 * AAC mode type.  Note that the term profile is used with the MPEG-2
 * standard and the term object type and profile is used with MPEG-4
 *
 * @since 1.0
 * @version 1.0
 */
enum struct AudioAacProfile : uint8_t {
    NONE = 0,           ///< Null, not used
    MAIN = 1,           ///< AAC Main object
    LC,                 ///< AAC Low Complexity object (AAC profile)
    SSR,                ///< AAC Scalable Sample Rate object
    LTP,                ///< AAC Long Term Prediction object
    HE,                 ///< AAC High Efficiency (object type SBR, HE-AAC profile)
    SCALABLE,           ///< AAC Scalable object
    ERLC = 17,          ///< ER AAC Low Complexity object (Error Resilient AAC-LC)
    ER_SCALABLE = 20,   ///< ER AAC scalable object
    LD = 23,            ///< AAC Low Delay object (Error Resilient)
    HE_PS = 29,         ///< AAC High Efficiency with Parametric Stereo coding (HE-AAC v2, object type PS)
    ELD = 39,           ///< AAC Enhanced Low Delay. NOTE: Pending Khronos standardization
    XHE = 42,           ///< extended High Efficiency AAC. NOTE: Pending Khronos standardization
};

/**
 * @enum Audio AAC Stream Format
 *
 * @since 1.0
 * @version 1.0
 */
enum struct AudioAacStreamFormat : uint8_t {
    MP2ADTS = 0,        ///< AAC Audio Data Transport Stream 2 format
    MP4ADTS,            ///< AAC Audio Data Transport Stream 4 format
    MP4LOAS,            ///< AAC Low Overhead Audio Stream format
    MP4LATM,            ///< AAC Low overhead Audio Transport Multiplex
    ADIF,               ///< AAC Audio Data Interchange Format
    MP4FF,              ///< AAC inside MPEG-4/ISO File Format
    RAW,                ///< AAC Raw Format
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_COMMON_AUDIO_TAGS_H
