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

#include "include/a2dp_aac_param_ctrl.h"
#include "include/a2dp_codec_config.h"
#include "include/a2dp_sbc_param_ctrl.h"
#include "log.h"
#include "memory.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
const int PRIORITY_SBC_BASIC = 1000;
const int PRIORITY_NUM_BASIC_VALUE = 1;
static void InitCodecConfig(
    A2dpCodecCapability &codecConfig, const A2dpCodecIndex codecIndex, const A2dpCodecPriority codecPriority)
{
    (void)memset_s(&codecConfig, sizeof(A2dpCodecCapability), 0, sizeof(A2dpCodecCapability));
    codecConfig.codecIndex_ = codecIndex;
    codecConfig.codecPriority_ = codecPriority;
}

A2dpCodecConfig::A2dpCodecConfig(A2dpCodecIndex index, A2dpCodecPriority priority)
    : codecIndex_(index), defaultPriority_(priority)
{
    LOG_INFO("[CodecConfig] %{public}s\n", __func__);
    SetCodecPriority(priority);
    InitCodecConfig(codecConfig_, codecIndex_, GetCodecPriority());
    InitCodecConfig(userCodecConfig_, codecIndex_, GetCodecPriority());
    InitCodecConfig(savedCodecUserConfig_, codecIndex_, GetCodecPriority());
    InitCodecConfig(audioCodecConfig_, codecIndex_, GetCodecPriority());
    InitCodecConfig(codecSelectableCap_, codecIndex_, GetCodecPriority());
    InitCodecConfig(codecLocalCap_, codecIndex_, GetCodecPriority());
    InitCodecConfig(codecCap_, codecIndex_, GetCodecPriority());
    InitCodecConfig(savedCodecCapability_, codecIndex_, GetCodecPriority());
    InitCodecConfig(savedCodecConfig_, codecIndex_, GetCodecPriority());
    InitCodecConfig(savedCodecSelectableCapability, codecIndex_, GetCodecPriority());
    (void)memset_s(otaCodecConfig_, A2DP_CODEC_SIZE, 0, sizeof(otaCodecConfig_));
    (void)memset_s(otaCodecPeerCapability_, A2DP_CODEC_SIZE, 0, sizeof(otaCodecPeerCapability_));
    (void)memset_s(savedOtaCodecPeerCapability, A2DP_CODEC_SIZE, 0, sizeof(savedOtaCodecPeerCapability));
    (void)memset_s(otaCodecPeerConfig_, A2DP_CODEC_SIZE, 0, sizeof(otaCodecPeerConfig_));
    (void)memset_s(savedOtaCodecConfig_, A2DP_CODEC_SIZE, 0, sizeof(savedOtaCodecConfig_));
}

A2dpCodecIndex A2dpCodecConfig::GetCodecIndex() const
{
    LOG_INFO("[CodecConfig] %{public}s codecIndex:0x%x\n", __func__, codecCap_.codecIndex_);
    return codecIndex_;
}

A2dpCodecPriority A2dpCodecConfig::GetCodecPriority() const
{
    LOG_INFO("[CodecConfig] %{public}s codecPriority_:0x%x\n", __func__, codecPriority_);
    return codecPriority_;
}

const std::string &A2dpCodecConfig::Name() const
{
    LOG_INFO("[CodecConfig] %{public}s\n", __func__);

    return name_;
}

A2dpCodecCapability A2dpCodecConfig::GetCodecLocalCapability() const
{
    LOG_INFO("[CodecConfig] %{public}s", __func__);
    return codecLocalCap_;
}

uint32_t A2dpCodecConfig::GetAudioBitsPerSample() const
{
    LOG_INFO("[CodecConfig] %{public}s\n", __func__);
    switch (codecConfig_.bitsPerSample) {
        case A2DP_SAMPLE_BITS_16:
            return A2DP_SAMPLE_BITS_16;
        case A2DP_SAMPLE_BITS_24:
            return A2DP_SAMPLE_BITS_24;
        case A2DP_SAMPLE_BITS_32:
            return A2DP_SAMPLE_BITS_32;
        default:
            LOG_INFO("Use default bitsPerSample: 16");
            return A2DP_SAMPLE_BITS_16;
    }
}

A2dpCodecCapability A2dpCodecConfig::GetCodecConfig() const
{
    LOG_INFO("[CodecConfig] %{public}s", __func__);
    return codecConfig_;
}

A2dpCodecCapability A2dpCodecConfig::GetUserConfig() const
{
    LOG_INFO("[CodecConfig] %{public}s", __func__);
    return userCodecConfig_;
}

A2dpCodecCapability A2dpCodecConfig::GetAudioCodecConfig() const
{
    LOG_INFO("[CodecConfig] %{public}s", __func__);
    return audioCodecConfig_;
}

bool A2dpCodecConfig::CopyOutOtaCodecConfig(uint8_t *codecInfo) const
{
    LOG_INFO("[CodecConfig] %{public}s", __func__);
    (void)memcpy_s(codecInfo, A2DP_CODEC_SIZE, otaCodecConfig_, A2DP_CODEC_SIZE);
    return true;
}

void A2dpCodecConfig::SetCodecPriority(A2dpCodecPriority codecPriority)
{
    LOG_INFO("[CodecConfig] %{public}s codecPriority:%u", __func__, codecPriority);
    if (codecPriority == A2DP_CODEC_PRIORITY_DEFAULT) {
        SetDefaultCodecPriority();
    } else {
        codecPriority_ = codecPriority;
    }
    codecConfig_.codecPriority_ = codecPriority_;
}

void A2dpCodecConfig::SetDefaultCodecPriority()
{
    if (defaultPriority_ != A2DP_CODEC_PRIORITY_DEFAULT) {
        codecPriority_ = defaultPriority_;
    } else {
        // Compute the default codec priority
        const uint32_t priority = PRIORITY_SBC_BASIC * (codecIndex_ + PRIORITY_NUM_BASIC_VALUE) +
            PRIORITY_NUM_BASIC_VALUE;
        codecPriority_ = static_cast<A2dpCodecPriority>(priority);
    }
    codecConfig_.codecPriority_ = codecPriority_;
}

bool IsConfigChanged(const uint8_t (&lastCodecInfo)[A2DP_CODEC_SIZE], const uint8_t *newCodecInfo)
{
    A2dpCodecType lastType = GetCodecType(lastCodecInfo);
    A2dpCodecType newType = GetCodecType(newCodecInfo);
    if (lastType != newType) {
        return false;
    }
    switch (lastType) {
        case A2DP_CODEC_TYPE_SBC:
            return IsSbcConfigChanged(lastCodecInfo, newCodecInfo);
        case A2DP_CODEC_TYPE_AAC:
            return IsAacConfigChanged(lastCodecInfo, newCodecInfo);
        default:
            return false;
    }
}

bool A2dpCodecConfig::SetCodecUserConfig(const A2dpCodecCapability &userConfig, const A2dpCodecCapability &audioConfig,
    const uint8_t *peerCodeInfo, uint8_t *resultCodecInfo, bool *restartConfig)
{
    LOG_INFO("[CodecConfig] %{public}s\n", __func__);
    bool ret = A2DP_SUCCESS;
    A2dpCodecCapability savedCodecUserConfig = userCodecConfig_;
    userCodecConfig_ = userConfig;
    A2dpCodecCapability savedAudioConfig = audioCodecConfig_;
    audioCodecConfig_ = audioConfig;
    uint8_t lastOtaCodecConfig[A2DP_CODEC_SIZE];
    (void)memcpy_s(lastOtaCodecConfig, A2DP_CODEC_SIZE, otaCodecConfig_, A2DP_CODEC_SIZE);
    ret = SetCodecConfig(peerCodeInfo, resultCodecInfo);
    LOG_INFO("[CodecConfig] %{public}s ret:%{public}d\n", __func__, ret);
    if (!ret) {
        userCodecConfig_ = savedCodecUserConfig;
        audioCodecConfig_ = savedAudioConfig;
        return ret;
    }

    if (!IsConfigChanged(lastOtaCodecConfig, resultCodecInfo)) {
        *restartConfig = true;
    }

    return ret;
}

A2dpCodecConfig *A2dpCodecConfig::CreateCodec(A2dpCodecIndex codecIndex, A2dpCodecPriority codecPriority)
{
    LOG_INFO("[CodecConfig] %{public}s [codecIndex:%u][codecPriority:%u]\n", __func__, codecIndex, codecPriority);
    std::unique_ptr<A2dpCodecConfig>  codecConfig = nullptr;
    switch (codecIndex) {
        case A2DP_SOURCE_CODEC_INDEX_SBC:
            codecConfig =  std::make_unique<A2dpCodecConfigSbcSource>(codecPriority);
            break;
        case A2DP_SINK_CODEC_INDEX_SBC:
            codecConfig =  std::make_unique<A2dpCodecConfigSbcSink>(codecPriority);
            break;
        case A2DP_SOURCE_CODEC_INDEX_AAC:
            codecConfig =  std::make_unique<A2dpCodecConfigAacSource>(codecPriority);
            break;
        case A2DP_SINK_CODEC_INDEX_AAC:
            codecConfig =  std::make_unique<A2dpCodecConfigAacSink>(codecPriority);
            break;
        default:
            break;
    }

    return codecConfig.release();
}

A2dpCodecType GetCodecType(const uint8_t *codecInfo)
{
    LOG_INFO("[CodecConfig] %{public}s codectype(%u)\n", __func__, *(codecInfo + A2DP_CODEC_TYPE_INDEX));
    return static_cast<A2dpCodecType>(*(codecInfo + A2DP_CODEC_TYPE_INDEX));
}

A2dpCodecIndex GetSourceCodecIndex(const uint8_t *codecInfo)
{
    LOG_INFO("[CodecConfig] %{public}s\n", __func__);
    A2dpCodecType codecType = GetCodecType(codecInfo);
    LOG_INFO("[CodecConfig] %{public}s codecType:%u\n", __func__, codecType);
    switch (codecType) {
        case A2DP_CODEC_TYPE_SBC:
            return A2DP_SOURCE_CODEC_INDEX_SBC;
        case A2DP_CODEC_TYPE_AAC:
            return A2DP_SOURCE_CODEC_INDEX_AAC;
        default:
            return A2DP_SOURCE_CODEC_INDEX_MAX;
    }
}

A2dpCodecIndex GetSinkCodecIndex(const uint8_t *codecInfo)
{
    LOG_INFO("%{public}s", __func__);
    A2dpCodecType codecType = GetCodecType(codecInfo);
    LOG_INFO("[CodecConfig] %{public}s codecType:%u\n", __func__, codecType);

    switch (codecType) {
        case A2DP_CODEC_TYPE_SBC:
            return A2DP_SINK_CODEC_INDEX_SBC;
            break;
        case A2DP_CODEC_TYPE_AAC:
            return A2DP_SINK_CODEC_INDEX_AAC;
            break;
        default:
            return A2DP_SINK_CODEC_INDEX_MAX;
    }
}

bool IsPeerSinkCodecValid(const uint8_t *codecInfo)
{
    bool ret = false;
    A2dpCodecType codecType = GetCodecType(codecInfo);
    A2dpSBCCapability sbcCap = {};
    A2dpAACCapability aacCap = {};
    LOG_INFO("[CodecConfig] %{public}s codecType:%u\n", __func__, codecType);
    switch (codecType) {
        case A2DP_CODEC_TYPE_SBC:
            if (ParseInfoSbc(&sbcCap, codecInfo) == A2DP_SUCCESS) {
                ret = true;
            }
            break;
        case A2DP_CODEC_TYPE_AAC:
            if (ParseInfoAac(&aacCap, codecInfo) == A2DP_SUCCESS) {
                ret = true;
            }
            break;
        default:
            break;
    }

    return ret;
}

bool IsPeerSourceCodecValid(const uint8_t *codecInfo)
{
    bool ret = false;
    A2dpCodecType codecType = GetCodecType(codecInfo);
    A2dpSBCCapability sbcCap = {};
    A2dpAACCapability aacCap = {};
    LOG_INFO("[CodecConfig] %{public}s codecType:%u\n", __func__, codecType);
    switch (codecType) {
        case A2DP_CODEC_TYPE_SBC:
            if (ParseInfoSbc(&sbcCap, codecInfo) == A2DP_SUCCESS) {
                ret = true;
            }
            break;
        case A2DP_CODEC_TYPE_AAC:
            if (ParseInfoAac(&aacCap, codecInfo) == A2DP_SUCCESS) {
                ret = true;
            }
            break;
        default:
            break;
    }

    return ret;
}
}  // namespace bluetooth
}  // namespace OHOS
