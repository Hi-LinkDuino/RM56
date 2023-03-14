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

#ifndef A2DP_PARCEL_CODEC_H
#define A2DP_PARCEL_CODEC_H

#include <string>
#include <vector>

namespace OHOS {
namespace bluetooth {
/**
 * @brief A2dp codec type enum.
 *
 * @since 6.0
 */
enum A2dpIpcCodecType : uint8_t {
    A2DP_CODEC_TYPE_SBC_IPC = 0,
    A2DP_CODEC_TYPE_MPEG1_IPC = 0x01,
    A2DP_CODEC_TYPE_AAC_IPC = 0x01 << 1,
    A2DP_CODEC_TYPE_ATRAC_IPC = 0x01 << 2,
    A2DP_CODEC_TYPE_NONA2DP_IPC = 0xFF
};

/**
 * @brief A2dp codec priority enum.
 *
 * @since 6.0
 */
enum A2dpIpcCodecPriority : uint8_t {
    A2DP_CODEC_PRIORITY_DISABLED_IPC = 0,
    A2DP_CODEC_PRIORITY_DEFAULT_IPC,
    A2DP_CODEC_PRIORITY_HIGHEST_IPC = 255,
};

/**
 * @brief A2dp codec sample rate enum.
 *
 * @since 6.0
 */
enum A2dpIpcCodecSampleRate : uint32_t {
    A2DP_SAMPLE_RATE_NONE_IPC = 0x0,
    A2DP_SBC_SAMPLE_RATE_48000_IPC = 0x1 << 4, /* octet0 b4 */
    A2DP_SBC_SAMPLE_RATE_44100_IPC = 0x1 << 5, /* octet0 b5 */
    A2DP_SBC_SAMPLE_RATE_32000_IPC = 0x1 << 6, /* octet0 b6 */
    A2DP_SBC_SAMPLE_RATE_16000_IPC = 0x1 << 7, /* octet0 b7 */
    A2DP_SBC_SAMPLE_RATE_MSK_IPC = 0xF0,
    A2DP_AAC_SAMPLE_RATE_OCTET1_44100_IPC = 0x01,
    A2DP_AAC_SAMPLE_RATE_OCTET1_32000_IPC = 0x01 << 1,
    A2DP_AAC_SAMPLE_RATE_OCTET1_24000_IPC = 0x01 << 2,
    A2DP_AAC_SAMPLE_RATE_OCTET1_22050_IPC = 0x01 << 3,
    A2DP_AAC_SAMPLE_RATE_OCTET1_16000_IPC = 0x01 << 4,
    A2DP_AAC_SAMPLE_RATE_OCTET1_12000_IPC = 0x01 << 5,
    A2DP_AAC_SAMPLE_RATE_OCTET1_11025_IPC = 0x01 << 6,
    A2DP_AAC_SAMPLE_RATE_OCTET1_8000_IPC = 0x01 << 7,
    A2DP_AAC_SAMPLE_RATE_OCTET1_MSK_IPC = 0xFF,
    A2DP_AAC_SAMPLE_RATE_OCTET2_96000_IPC = 0x01 << 4,
    A2DP_AAC_SAMPLE_RATE_OCTET2_88200_IPC = 0x01 << 5,
    A2DP_AAC_SAMPLE_RATE_OCTET2_64000_IPC = 0x01 << 6,
    A2DP_AAC_SAMPLE_RATE_OCTET2_48000_IPC = 0x01 << 7,
    A2DP_AAC_SAMPLE_RATE_OCTET2_MSK_IPC = 0xF0,
};

/**
 * @brief A2dp codec channel mode enum.
 *
 * @since 6.0
 */
enum A2dpIpcCodecChannelMode : uint8_t {
    A2DP_CHANNEL_MODE_NONE_IPC = 0x0,
    A2DP_SBC_CHANNEL_MODE_JOINT_STEREO_IPC = 0x1, /* octet0 b0 */
    A2DP_SBC_CHANNEL_MODE_STEREO_IPC = 0x1 << 1,  /* octet0 b1 */
    A2DP_SBC_CHANNEL_MODE_DUAL_IPC = 0x1 << 2,    /* octet0 b2 */
    A2DP_SBC_CHANNEL_MODE_MONO_IPC = 0x1 << 3,    /* octet0 b3 */
    A2DP_SBC_CHANNEL_MODE_MSK_IPC = 0x0F,
    A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE_IPC = 0x01 << 2, /* octet2 b2 */
    A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE_IPC = 0x01 << 3, /* octet2 b3 */
    A2DP_AAC_CHANNEL_MODE_OCTET2_MSK_IPC = 0x0C,
};

/**
 * @brief A2dp codec bits per sample enum.
 *
 * @since 6.0
 */
enum A2dpIpcCodecBitsPerSample : uint8_t {
    A2DP_SAMPLE_BITS_NONE_IPC = 0x0,
    A2DP_SAMPLE_BITS_16_IPC = 0x1 << 0,
    A2DP_SAMPLE_BITS_24_IPC = 0x1 << 1,
    A2DP_SAMPLE_BITS_32_IPC = 0x1 << 2,
    A2DP_SAMPLE_BITS_MSK_IPC = 0x06,
};

/**
 * @brief A2dp codec configuration information of a2dp source
 *
 * @since 6.0
 */
struct CodecInfo {
    // Codec priority
    uint8_t codecPriority;

    // Codec type
    uint8_t codecType;

    // Codec sample
    uint32_t sampleRate;

    // Codec bits per sample
    uint8_t bitsPerSample;

    // Codec channel mode
    uint8_t channelMode;

    // Codec specific value1
    uint64_t codecSpecific1;

    // Codec specific value2
    uint64_t codecSpecific2;

    // Codec specific value3
    uint64_t codecSpecific3;

    // Codec specific value4
    uint64_t codecSpecific4;
};

/**
 * @brief The codec configuration and capability information of a2dp source
 *
 * @since 6.0
 */
struct CodecStatus {
    // current codec information
    CodecInfo codecInfo;

    // local codec information
    std::vector<CodecInfo> codecInfoLocalCap;

    // Local device and peer confirmed codec information
    std::vector<CodecInfo> codecInfoConfirmCap;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // A2DP_PARCEL_CODEC_H