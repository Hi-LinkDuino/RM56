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

#ifndef A2DP_CODEC_CONFIG_H
#define A2DP_CODEC_CONFIG_H

#include <cstdint>
#include <string>

#include "a2dp_codec_constant.h"

namespace OHOS {
namespace bluetooth {
struct A2dpCodecCapability {
    A2dpCodecIndex codecIndex_;
    A2dpCodecPriority codecPriority_;
    A2dpCodecPriority defaultPriority_;
    uint32_t sampleRate_;
    uint32_t bitsPerSample;
    uint8_t channelMode_;
    uint64_t codecSpecific1;
    uint64_t codecSpecific2;
    uint64_t codecSpecific3;
    uint64_t codecSpecific4;

    std::string ToString() const
    {
        std::string codecNameStr;

        switch (codecIndex_) {
            case A2DP_SOURCE_CODEC_INDEX_SBC:
                codecNameStr = "SBC";
                break;
            case A2DP_SOURCE_CODEC_INDEX_AAC:
                codecNameStr = "AAC";
                break;
            case A2DP_SINK_CODEC_INDEX_SBC:
                codecNameStr = "SBC (Sink)";
                break;
            case A2DP_SINK_CODEC_INDEX_AAC:
                codecNameStr = "AAC (Sink)";
                break;
            default:
                codecNameStr = "invalid";
                break;
        }

        std::string sampleRateStr;
        AppendCapability(sampleRateStr, (sampleRate_ == A2DP_SAMPLE_RATE_NONE), "NONE");
        AppendCapability(sampleRateStr, (sampleRate_ & A2DP_SBC_SAMPLE_RATE_44100), "44100");
        AppendCapability(sampleRateStr, (sampleRate_ & A2DP_SBC_SAMPLE_RATE_48000), "48000");
        AppendCapability(sampleRateStr, (sampleRate_ & A2DP_SBC_SAMPLE_RATE_32000), "32000");
        AppendCapability(sampleRateStr, (sampleRate_ & A2DP_SBC_SAMPLE_RATE_16000), "16000");

        std::string bitsPerSampleStr;
        AppendCapability(bitsPerSampleStr, (bitsPerSample == A2DP_SAMPLE_BITS_NONE), "NONE");
        AppendCapability(bitsPerSampleStr, (bitsPerSample & A2DP_SAMPLE_BITS_16), "16");
        AppendCapability(bitsPerSampleStr, (bitsPerSample & A2DP_SAMPLE_BITS_24), "24");
        AppendCapability(bitsPerSampleStr, (bitsPerSample & A2DP_SAMPLE_BITS_32), "32");

        std::string channelModeStr;
        AppendCapability(channelModeStr, (channelMode_ == A2DP_CHANNEL_MODE_NONE), "NONE");
        AppendCapability(channelModeStr, (channelMode_ & A2DP_SBC_CHANNEL_MODE_MONO), "A2DP_SBC_CHANNEL_MODE_MONO");
        AppendCapability(channelModeStr, (channelMode_ & A2DP_SBC_CHANNEL_MODE_STEREO), "A2DP_SBC_CHANNEL_MODE_STEREO");

        return "codec: " + codecNameStr + " priority: " + std::to_string(codecPriority_) +
               " sampleRate_: " + sampleRateStr + " bitsPerSample: " + bitsPerSampleStr +
               " channelMode_: " + channelModeStr + " codec_specific_1: " + std::to_string(codecSpecific1) +
               " codec_specific_2: " + std::to_string(codecSpecific2) +
               " codec_specific_3: " + std::to_string(codecSpecific3) +
               " codec_specific_4: " + std::to_string(codecSpecific4);
    }

private:
    static std::string AppendCapability(std::string &result, bool append, const std::string &name)
    {
        if (!append) {
            return result;
        }

        if (!result.empty()) {
            result += "|";
        }
        result += name;
        return result;
    }
};

class A2dpCodecConfig {
public:
    explicit A2dpCodecConfig(A2dpCodecIndex index = A2dpCodecIndex::A2DP_SOURCE_CODEC_INDEX_SBC,
        A2dpCodecPriority priority = A2dpCodecPriority::A2DP_CODEC_PRIORITY_DEFAULT);
    virtual ~A2dpCodecConfig() = default;

    /**
     * @brief A function to create codec elements.
     *
     * @param[in] codecIndex The index of the codec elements.
     * @param[in] codecPriority The priority of the codec elements.
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    static A2dpCodecConfig *CreateCodec(A2dpCodecIndex codecIndex, A2dpCodecPriority codecPriority);
    A2dpCodecIndex GetCodecIndex() const;
    A2dpCodecPriority GetCodecPriority() const;
    A2dpCodecCapability GetCodecLocalCapability() const;
    A2dpCodecCapability GetCodecConfig() const;
    A2dpCodecCapability GetUserConfig() const;
    A2dpCodecCapability GetAudioCodecConfig() const;
    uint32_t GetAudioBitsPerSample() const;
    bool CopyOutOtaCodecConfig(uint8_t *codecInfo) const;
    const std::string &Name() const;

    void SetCodecPriority(A2dpCodecPriority codecPriority);
    void SetDefaultCodecPriority();
    bool SetCodecUserConfig(const A2dpCodecCapability &userConfig, const A2dpCodecCapability &audioConfig,
        const uint8_t *peerCodeInfo, uint8_t *resultCodecInfo, bool *restartConfig);
    virtual bool SetCodecConfig(const uint8_t *peerCodeInfo, uint8_t *resultCodecInfo) = 0;
    virtual bool SetPeerCodecCapabilities(const uint8_t *peerCapabilities) = 0;

protected:
    A2dpCodecCapability codecConfig_ {};
    A2dpCodecCapability userCodecConfig_ {};
    A2dpCodecCapability savedCodecUserConfig_ {};
    A2dpCodecCapability audioCodecConfig_ {};
    A2dpCodecCapability codecSelectableCap_ {};
    A2dpCodecCapability codecLocalCap_ {};
    A2dpCodecCapability codecCap_ {};
    A2dpCodecCapability savedCodecCapability_ {};
    A2dpCodecCapability savedCodecConfig_ {};
    A2dpCodecCapability savedCodecSelectableCapability {};
    A2dpCodecIndex codecIndex_ {};
    A2dpCodecPriority codecPriority_ {};
    A2dpCodecPriority defaultPriority_ {};
    uint8_t otaCodecConfig_[A2DP_CODEC_SIZE] {};
    uint8_t otaCodecPeerCapability_[A2DP_CODEC_SIZE] {};
    uint8_t savedOtaCodecPeerCapability[A2DP_CODEC_SIZE] {};
    uint8_t otaCodecPeerConfig_[A2DP_CODEC_SIZE] {};
    uint8_t savedOtaCodecConfig_[A2DP_CODEC_SIZE] {};
    std::string name_ = "";
};

/**
 * @brief A function to get the codec type.
 *
 * @param[in] codecInfo The information about the codec Capabilities.
 * @return the codec type.
 * @since 6.0
 */
A2dpCodecType GetCodecType(const uint8_t *codecInfo);

/**
 * @brief A function to check codec capabilities conntain valid peer source codec.
 *
 * @param[in] codecInfo The information about the codec.
 * @return TRUE: is valid, otherwise not valid.
 * @since 6.0
 */
bool IsPeerSourceCodecValid(const uint8_t *codecInfo);

/**
 * @brief A function to check codec capabilities conntain valid peer sink codec.
 *
 * @param[in] codecInfo The information about the codec.
 * @return TRUE: is valid, otherwise not valid.
 * @since 6.0
 */
bool IsPeerSinkCodecValid(const uint8_t *codecInfo);

/**
 * @brief A function to get the source codec index.
 *
 * @param[in] codecInfo The information about the codec.
 * @return the index of the codec.
 * @since 6.0
 */
A2dpCodecIndex GetSourceCodecIndex(const uint8_t *codecInfo);

/**
 * @brief A function to get the sink codec index.
 *
 * @param[in] codecInfo The information about the codec.
 * @return the index of the codec.
 * @since 6.0
 */
A2dpCodecIndex GetSinkCodecIndex(const uint8_t *codecInfo);

/**
 * @brief A function to get the codec name
 *
 * @param[in] codecInfo The information about the codec Capabilities.
 * @return the codec type.
 * @since 6.0
 */
bool IsConfigChanged(const uint8_t (&lastCodecInfo)[A2DP_CODEC_SIZE], const uint8_t *newCodecInfo);
}  // namespace bluetooth
}  // namespace OHOS
#endif  // !A2DP_CODEC_CONFIG_H