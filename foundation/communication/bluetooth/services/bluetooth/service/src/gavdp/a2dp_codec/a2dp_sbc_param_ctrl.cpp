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

#include "include/a2dp_codec_constant.h"
#include "include/a2dp_codec_errors.h"
#include "include/a2dp_sbc_param_ctrl.h"
#include "log.h"
#include "memory.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
const int BIT_MOVE_LEFT4 = 4;
const int BIT_MOVE_RIGHT4 = 4;
A2dpCodecConfigSbcSource::A2dpCodecConfigSbcSource(A2dpCodecPriority codecPriority)
    : A2dpCodecConfigSbcBase(A2DP_SOURCE_CODEC_INDEX_SBC, codecPriority, true)
{
    LOG_INFO("[CodecConfigSbcSource] %{public}s", __func__);

    // Compute the local capability
    if (A2DP_SBC_SOURCE_CAPS.sampleFreq & A2DP_SBC_SAMPLE_RATE_44100) {
        codecLocalCap_.sampleRate_ |= A2DP_SBC_SAMPLE_RATE_44100;
    }
    if (A2DP_SBC_SOURCE_CAPS.sampleFreq & A2DP_SBC_SAMPLE_RATE_48000) {
        codecLocalCap_.sampleRate_ |= A2DP_SBC_SAMPLE_RATE_48000;
    }

    if (A2DP_SBC_SOURCE_CAPS.bitsPerSample & A2DP_SAMPLE_BITS_16) {
        codecLocalCap_.bitsPerSample |= A2DP_SAMPLE_BITS_16;
    }

    if (A2DP_SBC_SOURCE_CAPS.channelMode & A2DP_SBC_CHANNEL_MODE_MONO) {
        codecLocalCap_.channelMode_ |= A2DP_SBC_CHANNEL_MODE_MONO;
    }
    if (A2DP_SBC_SOURCE_CAPS.channelMode & A2DP_SBC_CHANNEL_MODE_JOINT_STEREO) {
        codecLocalCap_.channelMode_ |= A2DP_SBC_CHANNEL_MODE_STEREO;
    }
    if (A2DP_SBC_SOURCE_CAPS.channelMode & A2DP_SBC_CHANNEL_MODE_STEREO) {
        codecLocalCap_.channelMode_ |= A2DP_SBC_CHANNEL_MODE_STEREO;
    }
    if (A2DP_SBC_SOURCE_CAPS.channelMode & A2DP_SBC_CHANNEL_MODE_DUAL) {
        codecLocalCap_.channelMode_ |= A2DP_SBC_CHANNEL_MODE_STEREO;
    }
}

A2dpCodecConfigSbcSink::A2dpCodecConfigSbcSink(A2dpCodecPriority codecPriority)
    : A2dpCodecConfigSbcBase(A2DP_SINK_CODEC_INDEX_SBC, codecPriority, false)
{
    LOG_INFO("[CodecConfigSbcSink] %{public}s", __func__);
    // Compute the local capability
    if (A2DP_SBC_SINK_CAPS.sampleFreq & A2DP_SBC_SAMPLE_RATE_44100) {
        codecLocalCap_.sampleRate_ |= A2DP_SBC_SAMPLE_RATE_44100;
    }
    if (A2DP_SBC_SINK_CAPS.sampleFreq & A2DP_SBC_SAMPLE_RATE_48000) {
        codecLocalCap_.sampleRate_ |= A2DP_SBC_SAMPLE_RATE_48000;
    }

    if (A2DP_SBC_SINK_CAPS.bitsPerSample & A2DP_SAMPLE_BITS_16) {
        codecLocalCap_.bitsPerSample |= A2DP_SAMPLE_BITS_16;
    }

    if (A2DP_SBC_SINK_CAPS.channelMode & A2DP_SBC_CHANNEL_MODE_MONO) {
        codecLocalCap_.channelMode_ |= A2DP_SBC_CHANNEL_MODE_MONO;
    }
    if (A2DP_SBC_SINK_CAPS.channelMode & A2DP_SBC_CHANNEL_MODE_JOINT_STEREO) {
        codecLocalCap_.channelMode_ |= A2DP_SBC_CHANNEL_MODE_STEREO;
    }
    if (A2DP_SBC_SINK_CAPS.channelMode & A2DP_SBC_CHANNEL_MODE_STEREO) {
        codecLocalCap_.channelMode_ |= A2DP_SBC_CHANNEL_MODE_STEREO;
    }
    if (A2DP_SBC_SINK_CAPS.channelMode & A2DP_SBC_CHANNEL_MODE_DUAL) {
        codecLocalCap_.channelMode_ |= A2DP_SBC_CHANNEL_MODE_STEREO;
    }
}

void A2dpCodecConfigSbcBase::SetSampleRateUser(uint32_t sampFreq, A2dpSBCCapability &resultCap)
{
    switch (userCodecConfig_.sampleRate_) {
        case A2DP_SBC_SAMPLE_RATE_44100:
            if (sampFreq & A2DP_SBC_SAMPLE_RATE_44100) {
                resultCap.sampleFreq = A2DP_SBC_SAMPLE_RATE_44100;
                codecCap_.sampleRate_ = userCodecConfig_.sampleRate_;
                codecConfig_.sampleRate_ = userCodecConfig_.sampleRate_;
            }
            break;
        case A2DP_SBC_SAMPLE_RATE_48000:
            if (sampFreq & A2DP_SBC_SAMPLE_RATE_48000) {
                resultCap.sampleFreq = A2DP_SBC_SAMPLE_RATE_48000;
                codecCap_.sampleRate_ = userCodecConfig_.sampleRate_;
                codecConfig_.sampleRate_ = userCodecConfig_.sampleRate_;
            }
            break;
        case A2DP_SBC_SAMPLE_RATE_32000:
        case A2DP_SBC_SAMPLE_RATE_16000:
        case A2DP_SAMPLE_RATE_NONE:
            resultCap.sampleFreq = A2DP_SAMPLE_RATE_NONE;
            codecCap_.sampleRate_ = A2DP_SAMPLE_RATE_NONE;
            codecConfig_.sampleRate_ = A2DP_SAMPLE_RATE_NONE;
            break;
        default:
            break;
    }
}

void A2dpCodecConfigSbcBase::SetSampleRateSelectableCapa(uint32_t sampFreq)
{
    if (sampFreq & A2DP_SBC_SAMPLE_RATE_44100) {
        codecSelectableCap_.sampleRate_ |= A2DP_SBC_SAMPLE_RATE_44100;
    }
    if (sampFreq & A2DP_SBC_SAMPLE_RATE_48000) {
        codecSelectableCap_.sampleRate_ |= A2DP_SBC_SAMPLE_RATE_48000;
    }
}

void A2dpCodecConfigSbcBase::SetSampleRateCommonCapa(uint32_t sampFreq)
{
    if (sampFreq & A2DP_SBC_SAMPLE_RATE_44100) {
        codecCap_.sampleRate_ = A2DP_SBC_SAMPLE_RATE_44100;
    }
    if (sampFreq & A2DP_SBC_SAMPLE_RATE_48000) {
        codecCap_.sampleRate_ = A2DP_SBC_SAMPLE_RATE_48000;
    }
}

bool A2dpCodecConfigSbcBase::SetSampleRateAudioConfig(uint32_t sampFreq, A2dpSBCCapability &resultCap)
{
    switch (audioCodecConfig_.sampleRate_) {
        case A2DP_SBC_SAMPLE_RATE_44100:
            if (sampFreq & A2DP_SBC_SAMPLE_RATE_44100) {
                resultCap.sampleFreq = A2DP_SBC_SAMPLE_RATE_44100;
                codecConfig_.sampleRate_ = A2DP_SBC_SAMPLE_RATE_44100;
                return true;
            }
            break;
        case A2DP_SBC_SAMPLE_RATE_48000:
            if (sampFreq & A2DP_SBC_SAMPLE_RATE_48000) {
                resultCap.sampleFreq = A2DP_SBC_SAMPLE_RATE_48000;
                codecConfig_.sampleRate_ = A2DP_SBC_SAMPLE_RATE_48000;
                return true;
            }
            break;
        default:
            break;
    }
    LOG_INFO("[CodecConfigSbcBase] %{public}s [audio config not suitable]", __func__);
    return false;
}

bool A2dpCodecConfigSbcBase::SetSampleRateBestMatch(uint32_t sampFreq, A2dpSBCCapability &resultCap)
{
    if (sampFreq & A2DP_SBC_SAMPLE_RATE_44100) {
        resultCap.sampleFreq = A2DP_SBC_SAMPLE_RATE_44100;
        codecConfig_.sampleRate_ = A2DP_SBC_SAMPLE_RATE_44100;
        return true;
    }

    if (sampFreq & A2DP_SBC_SAMPLE_RATE_48000) {
        resultCap.sampleFreq = A2DP_SBC_SAMPLE_RATE_48000;
        codecConfig_.sampleRate_ = A2DP_SBC_SAMPLE_RATE_48000;
        return true;
    }
    LOG_INFO("[CodecConfigSbcBase] %{public}s [audio config not suitable]", __func__);
    return false;
}

bool A2dpCodecConfigSbcBase::SetSampleRateDefaultConfig(uint32_t sampFreq, A2dpSBCCapability &resultCap)
{
    if (sampFreq & A2DP_SBC_SAMPLE_RATE_44100) {
        resultCap.sampleFreq = A2DP_SBC_SAMPLE_RATE_44100;
        codecConfig_.sampleRate_ = A2DP_SBC_SAMPLE_RATE_44100;
        return true;
    }
    if (sampFreq & A2DP_SBC_SAMPLE_RATE_48000) {
        resultCap.sampleFreq = A2DP_SBC_SAMPLE_RATE_48000;
        codecConfig_.sampleRate_ = A2DP_SBC_SAMPLE_RATE_48000;
        return true;
    }
    return false;
}

bool A2dpCodecConfigSbcBase::SetSampleRate(
    const A2dpSBCCapability &localCapability, A2dpSBCCapability peerCapability, A2dpSBCCapability &resultCap)
{
    codecConfig_.sampleRate_ = A2DP_SAMPLE_RATE_NONE;
    uint32_t sampFreq = localCapability.sampleFreq & peerCapability.sampleFreq;

    LOG_INFO("[CodecConfigSbcBase] %{public}s [sampFreq:0x%x][uFreq:0x%x][dFreq:0x%x][aFreq:0x%x][peer:0x%x]",
        __func__,
        sampFreq,
        userCodecConfig_.sampleRate_,
        A2DP_SBC_DEFAULT_CONFIG.sampleFreq,
        audioCodecConfig_.sampleRate_,
        peerCapability.sampleFreq);

    SetSampleRateUser(sampFreq, resultCap);

    do {
        SetSampleRateSelectableCapa(sampFreq);

        if (codecConfig_.sampleRate_ != A2DP_SAMPLE_RATE_NONE) {
            break;
        }

        SetSampleRateCommonCapa(sampFreq);

        if (SetSampleRateAudioConfig(sampFreq, resultCap)) {
            break;
        }

        if (SetSampleRateDefaultConfig(A2DP_SBC_DEFAULT_CONFIG.sampleFreq & peerCapability.sampleFreq, resultCap)) {
            break;
        }

        if (SetSampleRateBestMatch(sampFreq, resultCap)) {
            break;
        }
    } while (false);

    LOG_INFO("[CodecConfigSbcBase] %{public}s [sampFreq:0x%x]", __func__, codecConfig_.sampleRate_);
    if (codecConfig_.sampleRate_ == A2DP_SAMPLE_RATE_NONE) {
        LOG_ERROR("%{public}s: sampleRate_ had no value need reset config", __func__);
        return ResetCodecConfig();
    }
    return true;
}

void A2dpCodecConfigSbcBase::SetBitsPerSampleUser()
{
    switch (userCodecConfig_.bitsPerSample) {
        case A2DP_SAMPLE_BITS_16:
            codecCap_.bitsPerSample = userCodecConfig_.bitsPerSample;
            codecConfig_.bitsPerSample = userCodecConfig_.bitsPerSample;
            break;
        case A2DP_SAMPLE_BITS_24:
        case A2DP_SAMPLE_BITS_32:
        case A2DP_SAMPLE_BITS_NONE:
            codecCap_.bitsPerSample = A2DP_SAMPLE_BITS_NONE;
            codecConfig_.bitsPerSample = A2DP_SAMPLE_BITS_NONE;
            break;
        default:
            break;
    }
}

void A2dpCodecConfigSbcBase::SetBitsPerSampleSelectableCapa(const A2dpSBCCapability &localCapability)
{
    codecSelectableCap_.bitsPerSample = localCapability.bitsPerSample;
    LOG_INFO("[CodecConfigSbcBase] %{public}s [CapBitsPerSample:0x%x]", __func__, codecSelectableCap_.bitsPerSample);
}

void A2dpCodecConfigSbcBase::SetBitsPerSampleCommonCapa()
{
    codecCap_.bitsPerSample = A2DP_SAMPLE_BITS_16;
    LOG_INFO("[CodecConfigSbcBase] %{public}s [bitsPerSample:0x%x]", __func__, codecCap_.bitsPerSample);
}

bool A2dpCodecConfigSbcBase::SetBitsPerSampleAudioConfig(A2dpSBCCapability &resultCap)
{
    switch (audioCodecConfig_.bitsPerSample) {
        case A2DP_SAMPLE_BITS_16:
            codecConfig_.bitsPerSample = A2DP_SAMPLE_BITS_16;
            resultCap.bitsPerSample = A2DP_SAMPLE_BITS_16;
            return true;
        case A2DP_SAMPLE_BITS_24:
        case A2DP_SAMPLE_BITS_32:
        case A2DP_SAMPLE_BITS_NONE:
            break;
        default:
            break;
    }
    LOG_INFO("[CodecConfigSbcBase] %{public}s [not bits16]", __func__);
    return false;
}

bool A2dpCodecConfigSbcBase::SetBitsPerSampleBestMatch(A2dpSBCCapability &resultCap)
{
    codecConfig_.bitsPerSample = A2DP_SAMPLE_BITS_16;
    resultCap.bitsPerSample = A2DP_SAMPLE_BITS_16;
    return true;
}

bool A2dpCodecConfigSbcBase::SetBitsPerSampleDefaultConfig(A2dpSBCCapability &resultCap)
{
    codecConfig_.bitsPerSample = A2DP_SAMPLE_BITS_16;
    resultCap.bitsPerSample = A2DP_SAMPLE_BITS_16;
    return true;
}

bool A2dpCodecConfigSbcBase::SetBitsPerSample(const A2dpSBCCapability &localCapability, A2dpSBCCapability &resultCap)
{
    LOG_INFO("[CodecConfigSbcBase] %{public}s [uBits:0x%x][aBits:0x%x]",
        __func__,
        userCodecConfig_.bitsPerSample,
        audioCodecConfig_.bitsPerSample);

    codecConfig_.bitsPerSample = A2DP_SAMPLE_BITS_NONE;

    SetBitsPerSampleUser();

    do {
        SetBitsPerSampleSelectableCapa(localCapability);

        if (codecConfig_.bitsPerSample != A2DP_SAMPLE_BITS_NONE) {
            break;
        }

        SetBitsPerSampleCommonCapa();

        if (SetBitsPerSampleAudioConfig(resultCap)) {
            break;
        }

        if (SetBitsPerSampleDefaultConfig(resultCap)) {
            break;
        }

        if (SetBitsPerSampleBestMatch(resultCap)) {
            break;
        }
    } while (false);

    LOG_INFO("[CodecConfigSbcBase] %{public}s [bitsPerSample:0x%x]", __func__, codecConfig_.bitsPerSample);

    if (codecConfig_.bitsPerSample == A2DP_SAMPLE_BITS_NONE) {
        LOG_ERROR("%{public}s: bitsPerSample had no value", __func__);
        return ResetCodecConfig();
    }
    return true;
}

void A2dpCodecConfigSbcBase::SetChannelModeUser(uint8_t chMode, A2dpSBCCapability &resultCap)
{
    switch (userCodecConfig_.channelMode_) {
        case A2DP_SBC_CHANNEL_MODE_MONO:
            if (chMode & A2DP_SBC_CHANNEL_MODE_MONO) {
                resultCap.channelMode = A2DP_SBC_CHANNEL_MODE_MONO;
                codecCap_.channelMode_ = userCodecConfig_.channelMode_;
                codecConfig_.channelMode_ = userCodecConfig_.channelMode_;
            }
            break;
        case A2DP_SBC_CHANNEL_MODE_JOINT_STEREO:
        case A2DP_SBC_CHANNEL_MODE_STEREO:
        case A2DP_SBC_CHANNEL_MODE_DUAL:
            if (chMode & A2DP_SBC_CHANNEL_MODE_JOINT_STEREO) {
                resultCap.channelMode = A2DP_SBC_CHANNEL_MODE_JOINT_STEREO;
                codecCap_.channelMode_ = userCodecConfig_.channelMode_;
                codecConfig_.channelMode_ = userCodecConfig_.channelMode_;
                break;
            }
            if (chMode & A2DP_SBC_CHANNEL_MODE_STEREO) {
                resultCap.channelMode = A2DP_SBC_CHANNEL_MODE_STEREO;
                codecCap_.channelMode_ = userCodecConfig_.channelMode_;
                codecConfig_.channelMode_ = userCodecConfig_.channelMode_;
                break;
            }
            if (chMode & A2DP_SBC_CHANNEL_MODE_DUAL) {
                resultCap.channelMode = A2DP_SBC_CHANNEL_MODE_DUAL;
                codecCap_.channelMode_ = userCodecConfig_.channelMode_;
                codecConfig_.channelMode_ = userCodecConfig_.channelMode_;
                break;
            }
            break;
        case A2DP_CHANNEL_MODE_NONE:
            resultCap.channelMode = A2DP_CHANNEL_MODE_NONE;
            codecCap_.channelMode_ = A2DP_CHANNEL_MODE_NONE;
            codecConfig_.channelMode_ = A2DP_CHANNEL_MODE_NONE;
            break;
        default:
            break;
    }
}

void A2dpCodecConfigSbcBase::SetChannelModeSelectableCapa(uint8_t chMode)
{
    if (chMode & A2DP_SBC_CHANNEL_MODE_MONO) {
        codecSelectableCap_.channelMode_ |= A2DP_SBC_CHANNEL_MODE_MONO;
    }
    if (chMode & A2DP_SBC_CHANNEL_MODE_JOINT_STEREO) {
        codecSelectableCap_.channelMode_ |= A2DP_SBC_CHANNEL_MODE_STEREO;
    }
    if (chMode & A2DP_SBC_CHANNEL_MODE_STEREO) {
        codecSelectableCap_.channelMode_ |= A2DP_SBC_CHANNEL_MODE_STEREO;
    }
    if (chMode & A2DP_SBC_CHANNEL_MODE_DUAL) {
        codecSelectableCap_.channelMode_ |= A2DP_SBC_CHANNEL_MODE_STEREO;
    }
}

void A2dpCodecConfigSbcBase::SetChannelModeCommonCapa(uint8_t chMode)
{
    if (chMode & A2DP_SBC_CHANNEL_MODE_MONO) {
        codecCap_.channelMode_ |= A2DP_SBC_CHANNEL_MODE_MONO;
    }
    if (chMode & (A2DP_SBC_CHANNEL_MODE_JOINT_STEREO | A2DP_SBC_CHANNEL_MODE_STEREO | A2DP_SBC_CHANNEL_MODE_DUAL)) {
        codecCap_.channelMode_ |= A2DP_SBC_CHANNEL_MODE_STEREO;
    }
}

bool A2dpCodecConfigSbcBase::SetChannelModeAudioConfig(uint8_t chMode, A2dpSBCCapability &resultCap)
{
    switch (audioCodecConfig_.channelMode_) {
        case A2DP_SBC_CHANNEL_MODE_MONO:
            if (chMode & A2DP_SBC_CHANNEL_MODE_MONO) {
                resultCap.channelMode = A2DP_SBC_CHANNEL_MODE_MONO;
                codecConfig_.channelMode_ = A2DP_SBC_CHANNEL_MODE_MONO;
                return true;
            }
            break;
        case A2DP_SBC_CHANNEL_MODE_DUAL:
            if (chMode & A2DP_SBC_CHANNEL_MODE_DUAL) {
                resultCap.channelMode = A2DP_SBC_CHANNEL_MODE_STEREO;
                codecConfig_.channelMode_ = A2DP_SBC_CHANNEL_MODE_STEREO;
                return true;
            }
            break;
        case A2DP_SBC_CHANNEL_MODE_STEREO:
            if (chMode & A2DP_SBC_CHANNEL_MODE_STEREO) {
                resultCap.channelMode = A2DP_SBC_CHANNEL_MODE_STEREO;
                codecConfig_.channelMode_ = A2DP_SBC_CHANNEL_MODE_STEREO;
                return true;
            }
            break;
        case A2DP_SBC_CHANNEL_MODE_JOINT_STEREO:
            if (chMode & A2DP_SBC_CHANNEL_MODE_JOINT_STEREO) {
                resultCap.channelMode = A2DP_SBC_CHANNEL_MODE_JOINT_STEREO;
                codecConfig_.channelMode_ = A2DP_SBC_CHANNEL_MODE_JOINT_STEREO;
                return true;
            }
            break;
        default:
            LOG_ERROR("[CodecConfigSbcBase] %{public}s [Mode None]", __func__);
            break;
    }
    return false;
}

bool A2dpCodecConfigSbcBase::SetChannelModeBestMatch(uint8_t chMode, A2dpSBCCapability &resultCap)
{
    if (chMode & A2DP_SBC_CHANNEL_MODE_MONO) {
        resultCap.channelMode = A2DP_SBC_CHANNEL_MODE_MONO;
        codecConfig_.channelMode_ = A2DP_SBC_CHANNEL_MODE_MONO;
        return true;
    }

    if (chMode & A2DP_SBC_CHANNEL_MODE_DUAL) {
        resultCap.channelMode = A2DP_SBC_CHANNEL_MODE_STEREO;
        codecConfig_.channelMode_ = A2DP_SBC_CHANNEL_MODE_STEREO;
        return true;
    }

    if (chMode & A2DP_SBC_CHANNEL_MODE_STEREO) {
        resultCap.channelMode = A2DP_SBC_CHANNEL_MODE_STEREO;
        codecConfig_.channelMode_ = A2DP_SBC_CHANNEL_MODE_STEREO;
        return true;
    }

    if (chMode & A2DP_SBC_CHANNEL_MODE_JOINT_STEREO) {
        resultCap.channelMode = A2DP_SBC_CHANNEL_MODE_JOINT_STEREO;
        codecConfig_.channelMode_ = A2DP_SBC_CHANNEL_MODE_JOINT_STEREO;
        return true;
    }
    return false;
}

bool A2dpCodecConfigSbcBase::SetChannelModeDefaultConfig(uint8_t chMode, A2dpSBCCapability &resultCap)
{
    if (chMode & A2DP_SBC_CHANNEL_MODE_MONO) {
        resultCap.channelMode = A2DP_SBC_CHANNEL_MODE_MONO;
        codecConfig_.channelMode_ = A2DP_SBC_CHANNEL_MODE_MONO;
        return true;
    }
    if (chMode & A2DP_SBC_CHANNEL_MODE_JOINT_STEREO) {
        resultCap.channelMode = A2DP_SBC_CHANNEL_MODE_JOINT_STEREO;
        codecConfig_.channelMode_ = A2DP_SBC_CHANNEL_MODE_JOINT_STEREO;
        return true;
    }
    if (chMode & A2DP_SBC_CHANNEL_MODE_STEREO) {
        resultCap.channelMode = A2DP_SBC_CHANNEL_MODE_STEREO;
        codecConfig_.channelMode_ = A2DP_SBC_CHANNEL_MODE_STEREO;
        return true;
    }
    if (chMode & A2DP_SBC_CHANNEL_MODE_DUAL) {
        resultCap.channelMode = A2DP_SBC_CHANNEL_MODE_DUAL;
        codecConfig_.channelMode_ = A2DP_SBC_CHANNEL_MODE_DUAL;
        return true;
    }
    return false;
}

bool A2dpCodecConfigSbcBase::SetChannelMode(
    const A2dpSBCCapability &localCapability, A2dpSBCCapability peerCapability, A2dpSBCCapability &resultCap)
{
    uint8_t chMode = localCapability.channelMode & peerCapability.channelMode;
    codecConfig_.channelMode_ = A2DP_CHANNEL_MODE_NONE;

    LOG_INFO("[CodecConfigSbcBase] %{public}s [chMode:0x%x][uMode:0x%x][dMode:0x%x][aMode:0x%x][peer:0x%x]",
        __func__,
        chMode,
        userCodecConfig_.channelMode_,
        A2DP_SBC_DEFAULT_CONFIG.channelMode,
        audioCodecConfig_.channelMode_,
        peerCapability.channelMode);

    SetChannelModeUser(chMode, resultCap);

    do {
        SetChannelModeSelectableCapa(chMode);

        if (codecConfig_.channelMode_ != A2DP_CHANNEL_MODE_NONE) {
            break;
        }

        SetChannelModeCommonCapa(chMode);

        if (SetChannelModeAudioConfig(chMode, resultCap)) {
            break;
        }

        if (SetChannelModeDefaultConfig(A2DP_SBC_DEFAULT_CONFIG.channelMode & peerCapability.channelMode, resultCap)) {
            break;
        }

        if (SetChannelModeBestMatch(chMode, resultCap)) {
            break;
        }
    } while (false);

    LOG_INFO("[CodecConfigSbcBase] %{public}s [channelMode_:0x%x]", __func__, codecConfig_.channelMode_);

    if (codecConfig_.channelMode_ == A2DP_CHANNEL_MODE_NONE) {
        LOG_ERROR("%{public}s: channelMode had no value", __func__);
        return ResetCodecConfig();
    }
    return true;
}

bool A2dpCodecConfigSbcBase::SetBlockLength(
    const A2dpSBCCapability &localCapability, A2dpSBCCapability peerCapability, A2dpSBCCapability &resultCap)
{
    uint8_t blockLen = localCapability.blockLen & peerCapability.blockLen;

    LOG_INFO("[CodecConfigSbcBase] %{public}s [blockLen:0x%x]", __func__, blockLen);

    if (blockLen & A2DP_SBC_BLOCKS_16) {
        resultCap.blockLen = A2DP_SBC_BLOCKS_16;
    } else if (blockLen & A2DP_SBC_BLOCKS_12) {
        resultCap.blockLen = A2DP_SBC_BLOCKS_12;
    } else if (blockLen & A2DP_SBC_BLOCKS_8) {
        resultCap.blockLen = A2DP_SBC_BLOCKS_8;
    } else if (blockLen & A2DP_SBC_BLOCKS_4) {
        resultCap.blockLen = A2DP_SBC_BLOCKS_4;
    } else {
        LOG_ERROR("%{public}s: BlockLength had no value", __func__);
        return ResetCodecConfig();
    }
    return true;
}

bool A2dpCodecConfigSbcBase::SetSubBands(
    const A2dpSBCCapability &localCapability, A2dpSBCCapability peerCapability, A2dpSBCCapability &resultCap)
{
    uint8_t numSubbands = localCapability.numSubbands & peerCapability.numSubbands;

    LOG_INFO("[CodecConfigSbcBase] %{public}s [numSubbands:0x%x]", __func__, numSubbands);

    if (numSubbands & A2DP_SBC_SUBBAND_8) {
        resultCap.numSubbands = A2DP_SBC_SUBBAND_8;
    } else if (numSubbands & A2DP_SBC_SUBBAND_4) {
        resultCap.numSubbands = A2DP_SBC_SUBBAND_4;
    } else {
        LOG_ERROR("%{public}s: subBands had no value", __func__);
        return ResetCodecConfig();
    }
    return true;
}

bool A2dpCodecConfigSbcBase::SetAllocationMethod(
    const A2dpSBCCapability &localCapability, A2dpSBCCapability peerCapability, A2dpSBCCapability &resultCap)
{
    uint8_t allocMethod = localCapability.allocatedMethod & peerCapability.allocatedMethod;

    LOG_INFO("[CodecConfigSbcBase] %{public}s [allocMethod:0x%x]", __func__, allocMethod);

    if (allocMethod & A2DP_SBC_ALLOC_MODE_L) {
        resultCap.allocatedMethod = A2DP_SBC_ALLOC_MODE_L;
    } else if (allocMethod & A2DP_SBC_ALLOC_MODE_S) {
        resultCap.allocatedMethod = A2DP_SBC_ALLOC_MODE_S;
    } else {
        LOG_ERROR("%{public}s: subBands had no value", __func__);
        return ResetCodecConfig();
    }
    return true;
}

bool A2dpCodecConfigSbcBase::SetBitPool(
    const A2dpSBCCapability &localCapability, A2dpSBCCapability peerCapability, A2dpSBCCapability &resultCap)
{
    resultCap.minBitpool = localCapability.minBitpool;
    if (resultCap.minBitpool < peerCapability.minBitpool) {
        resultCap.minBitpool = peerCapability.minBitpool;
    }

    resultCap.maxBitpool = localCapability.maxBitpool;
    if (resultCap.maxBitpool > peerCapability.maxBitpool) {
        resultCap.maxBitpool = peerCapability.maxBitpool;
    }

    if (resultCap.minBitpool > localCapability.maxBitpool) {
        LOG_ERROR("%{public}s: bitpool set err", __func__);
        return ResetCodecConfig();
    }
    return true;
}

bool A2dpCodecConfigSbcBase::SetCodecSpecific(void)
{
    if (userCodecConfig_.codecSpecific1 != 0) {
        codecConfig_.codecSpecific1 = userCodecConfig_.codecSpecific1;
    }
    if (userCodecConfig_.codecSpecific2 != 0) {
        codecConfig_.codecSpecific2 = userCodecConfig_.codecSpecific2;
    }
    if (userCodecConfig_.codecSpecific3 != 0) {
        codecConfig_.codecSpecific3 = userCodecConfig_.codecSpecific3;
    }
    if (userCodecConfig_.codecSpecific4 != 0) {
        codecConfig_.codecSpecific4 = userCodecConfig_.codecSpecific4;
    }
    return true;
}

bool A2dpCodecConfigSbcBase::ResetCodecConfig(void)
{
    LOG_ERROR("[CodecConfigSbcBase] %{public}s", __func__);
    codecConfig_ = savedCodecConfig_;
    codecCap_ = savedCodecCapability_;
    userCodecConfig_ = savedCodecUserConfig_;
    (void)memcpy_s(otaCodecConfig_, A2DP_CODEC_SIZE, savedOtaCodecConfig_, A2DP_CODEC_SIZE);
    return false;
}

bool A2dpCodecConfigSbcBase::SetCodecConfig(const uint8_t *peerCodeInfo, uint8_t *resultCodecInfo)
{
    LOG_INFO("[CodecConfigSbcBase] %{public}s", __func__);
    const A2dpSBCCapability *a2dpSbcCaps = (isSource_) ? &A2DP_SBC_SOURCE_CAPS : &A2DP_SBC_SINK_CAPS;
    A2dpSBCCapability cfgCap = {};
    A2dpSBCCapability cfgCapResult = {};
    savedCodecConfig_ = codecConfig_;
    savedCodecCapability_ = codecCap_;
    savedCodecUserConfig_ = userCodecConfig_;
    (void)memcpy_s(savedOtaCodecConfig_, A2DP_CODEC_SIZE, otaCodecConfig_, A2DP_CODEC_SIZE);

    if (ParseInfoSbc(&cfgCap, peerCodeInfo) != A2DP_SUCCESS) {
        LOG_INFO("[CodecConfigSbcBase] %{public}s: can't parse peer's capabilities", __func__);
        return ResetCodecConfig();
    }

    if (SetSampleRate(*a2dpSbcCaps, cfgCap, cfgCapResult) != true) {
        LOG_ERROR("%{public}s: SetSampleRate failed", __func__);
    }
    if (SetBitsPerSample(*a2dpSbcCaps, cfgCapResult) != true) {
        LOG_ERROR("%{public}s: SetBitsPerSample failed", __func__);
    }
    if (SetChannelMode(*a2dpSbcCaps, cfgCap, cfgCapResult) != true) {
        LOG_ERROR("%{public}s: SetChannelMode failed", __func__);
    }
    if (SetBlockLength(*a2dpSbcCaps, cfgCap, cfgCapResult) != true) {
        LOG_ERROR("%{public}s: SetBlockLength failed", __func__);
    }
    if (SetSubBands(*a2dpSbcCaps, cfgCap, cfgCapResult) != true) {
        LOG_ERROR("%{public}s: SetSubBands failed", __func__);
    }
    if (SetAllocationMethod(*a2dpSbcCaps, cfgCap, cfgCapResult) != true) {
        LOG_ERROR("%{public}s: SetAllocationMethod failed", __func__);
    }
    if (SetBitPool(*a2dpSbcCaps, cfgCap, cfgCapResult) != true) {
        LOG_ERROR("%{public}s: SetBitPool failed", __func__);
    }

    if (BuildSbcInfo(&cfgCapResult, resultCodecInfo) != A2DP_SUCCESS) {
        LOG_INFO("[CodecConfigSbcBase] %{public}s: BuildSbcInfo for return config failed", __func__);
        return ResetCodecConfig();
    }

    SetCodecSpecific();

    if (BuildSbcInfo(&cfgCapResult, otaCodecConfig_) != A2DP_SUCCESS) {
        LOG_INFO("[CodecConfigSbcBase] %{public}s: Build Sbc Info failed", __func__);
        return ResetCodecConfig();
    }
    return true;
}

void A2dpCodecConfigSbcBase::SetSampleFrameCapa(
    const A2dpSBCCapability &localCapability, A2dpSBCCapability peerCapability)
{
    uint32_t sampFreq = localCapability.sampleFreq & peerCapability.sampleFreq;
    if (sampFreq & A2DP_SBC_SAMPLE_RATE_44100) {
        codecSelectableCap_.sampleRate_ |= A2DP_SBC_SAMPLE_RATE_44100;
    }
    if (sampFreq & A2DP_SBC_SAMPLE_RATE_48000) {
        codecSelectableCap_.sampleRate_ |= A2DP_SBC_SAMPLE_RATE_48000;
    }
}

void A2dpCodecConfigSbcBase::SetBitsPerSampleCapa(const A2dpSBCCapability &localCapability)
{
    codecSelectableCap_.bitsPerSample = localCapability.bitsPerSample;
}

void A2dpCodecConfigSbcBase::SetChannelModeCapa(
    const A2dpSBCCapability &localCapability, A2dpSBCCapability peerCapability)
{
    uint32_t chMode = localCapability.channelMode & peerCapability.channelMode;
    if (chMode & A2DP_SBC_CHANNEL_MODE_MONO) {
        codecSelectableCap_.channelMode_ |= A2DP_SBC_CHANNEL_MODE_MONO;
    }
    if (chMode & A2DP_SBC_CHANNEL_MODE_JOINT_STEREO) {
        codecSelectableCap_.channelMode_ |= A2DP_SBC_CHANNEL_MODE_JOINT_STEREO;
    }
    if (chMode & A2DP_SBC_CHANNEL_MODE_STEREO) {
        codecSelectableCap_.channelMode_ |= A2DP_SBC_CHANNEL_MODE_STEREO;
    }
    if (chMode & A2DP_SBC_CHANNEL_MODE_DUAL) {
        codecSelectableCap_.channelMode_ |= A2DP_SBC_CHANNEL_MODE_DUAL;
    }
}

bool A2dpCodecConfigSbcBase::ResetCodecCapability(void)
{
    LOG_ERROR("[CodecConfigSbcBase] %{public}s", __func__);
    codecSelectableCap_ = savedCodecSelectableCapability;
    (void)memcpy_s(otaCodecPeerCapability_, A2DP_CODEC_SIZE, savedOtaCodecPeerCapability, A2DP_CODEC_SIZE);
    return false;
}

bool A2dpCodecConfigSbcBase::SetPeerCodecCapabilities(const uint8_t *peerCapabilities)
{
    const A2dpSBCCapability *a2dpSbcCaps = (isSource_) ? &A2DP_SBC_SOURCE_CAPS : &A2DP_SBC_SINK_CAPS;

    A2dpSBCCapability cfgCap = {};

    if (ParseInfoSbc(&cfgCap, peerCapabilities) != A2DP_SUCCESS) {
        LOG_ERROR("[CodecConfigSbcBase]%{public}s: can't parse peer's capabilities", __func__);
        return ResetCodecCapability();
    }
    SetSampleFrameCapa(*a2dpSbcCaps, cfgCap);
    SetBitsPerSampleCapa(*a2dpSbcCaps);
    SetChannelModeCapa(*a2dpSbcCaps, cfgCap);

    if (BuildSbcInfo(&cfgCap, otaCodecPeerCapability_) != A2DP_SUCCESS) {
        LOG_ERROR("[CodecConfigSbcBase]%{public}s: BuildSbcInfo Error", __func__);
        return ResetCodecCapability();
    }
    return true;
}

int GetMinBitpoolSbc(const uint8_t *codecInfo)
{
    A2dpSBCCapability cfgCap = {};
    if (ParseInfoSbc(&cfgCap, codecInfo) != A2DP_SUCCESS) {
        LOG_ERROR("%{public}s: cannot get Minpool information", __func__);
        return -1;
    }
    LOG_INFO("[CodecConfigSbcBase] %{public}s [minBitpool:0x%x]", __func__, cfgCap.minBitpool);
    return cfgCap.minBitpool;
}

int GetMaxBitpoolSbc(const uint8_t *codecInfo)
{
    A2dpSBCCapability cfgCap = {};
    if (ParseInfoSbc(&cfgCap, codecInfo) != A2DP_SUCCESS) {
        LOG_ERROR("%{public}s: cannot Maxpool information", __func__);
        return -1;
    }
    LOG_INFO("[CodecConfigSbcBase] %{public}s [maxBitpool:0x%x]", __func__, cfgCap.maxBitpool);
    return cfgCap.maxBitpool;
}

int GetSampleRateSbc(const uint8_t *codecInfo)
{
    A2dpSBCCapability cfgCap = {};
    if (ParseInfoSbc(&cfgCap, codecInfo) != A2DP_SUCCESS) {
        LOG_ERROR("%{public}s: cannot get SampleRate Sbc information", __func__);
        return -1;
    }
    LOG_INFO("[CodecConfigSbcBase] %{public}s [sampleFreq:0x%x]", __func__, cfgCap.sampleFreq);
    return cfgCap.sampleFreq;
}

int GetTrackChannelModeSbc(const uint8_t *codecInfo)
{
    A2dpSBCCapability cfgCap = {};
    if (ParseInfoSbc(&cfgCap, codecInfo) != A2DP_SUCCESS) {
        LOG_ERROR("%{public}s: cannot get Track ChannelMode information", __func__);
        return -1;
    }
    LOG_INFO("[CodecConfigSbcBase] %{public}s [channelMode:0x%x]", __func__, cfgCap.channelMode);
    return cfgCap.channelMode;
}

int GetTrackChannelCountSbc(const uint8_t *codecInfo)
{
    A2dpSBCCapability cfgCap = {};
    if (ParseInfoSbc(&cfgCap, codecInfo) != A2DP_SUCCESS) {
        LOG_ERROR("%{public}s: cannot get TrackChannel information", __func__);
        return -1;
    }
    LOG_INFO("[CodecConfigSbcBase] %{public}s [channelMode:0x%x]", __func__, cfgCap.channelMode);
    return cfgCap.channelMode;
}

int GetNumberOfSubbandsSbc(const uint8_t *codecInfo)
{
    A2dpSBCCapability cfgCap = {};
    if (ParseInfoSbc(&cfgCap, codecInfo) != A2DP_SUCCESS) {
        LOG_ERROR("%{public}s: cannot get NumberOfSubbands information", __func__);
        return -1;
    }
    LOG_INFO("[CodecConfigSbcBase] %{public}s [numSubbands:0x%x]", __func__, cfgCap.numSubbands);
    return cfgCap.numSubbands;
}

int GetNumberOfBlocksSbc(const uint8_t *codecInfo)
{
    A2dpSBCCapability cfgCap = {};
    if (ParseInfoSbc(&cfgCap, codecInfo) != A2DP_SUCCESS) {
        LOG_ERROR("%{public}s: cannot get NumberOfBlocks information", __func__);
        return -1;
    }
    LOG_INFO("[CodecConfigSbcBase] %{public}s [blockLen:0x%x]", __func__, cfgCap.blockLen);
    return cfgCap.blockLen;
}

int GetAllocationMethodCodeSbc(const uint8_t *codecInfo)
{
    A2dpSBCCapability cfgCap = {};
    if (ParseInfoSbc(&cfgCap, codecInfo) != A2DP_SUCCESS) {
        LOG_ERROR("%{public}s: cannot get AllocationMethodCode information", __func__);
        return -1;
    }
    LOG_INFO("[CodecConfigSbcBase] %{public}s [allocatedMethod:0x%x]", __func__, cfgCap.allocatedMethod);
    return cfgCap.allocatedMethod;
}

uint8_t BuildSbcInfo(A2dpSBCCapability *sbcCap, uint8_t *codecInfo)
{
    LOG_INFO("[CodecConfig] %{public}s", __func__);
    /// SBC codec Info
    *codecInfo++ = A2DP_CODEC_SBC_INFO_LEN;  /// sbc information length
    *codecInfo++ = (A2DP_MEDIA_TYPE_AUDIO << BIT_MOVE_LEFT4);
    *codecInfo++ = 0;  /// SBC media codec type
    *codecInfo++ = sbcCap->sampleFreq | sbcCap->channelMode;
    *codecInfo++ = sbcCap->blockLen | sbcCap->numSubbands | sbcCap->allocatedMethod;
    *codecInfo++ = sbcCap->minBitpool;
    *codecInfo++ = sbcCap->maxBitpool;
    LOG_INFO("[CodecConfig] %{public}s End", __func__);
    return A2DP_SUCCESS;
}

uint8_t ParseInfoSbc(A2dpSBCCapability *sbcCap, const uint8_t *codecInfo)
{
    LOG_INFO("[CodecConfigSbcBase] %{public}s", __func__);
    uint8_t losc;
    uint8_t mediaType;
    uint8_t codecType;
    if (codecInfo == nullptr) {
        LOG_ERROR("[CodecConfigSbcBase]%{public}s codecInfo is NULL", __func__);
        return A2DP_CODEC_ERROR_INVALID_PARAMS;
    }

    losc = *codecInfo++;
    if (losc != A2DP_CODEC_SBC_INFO_LEN) {
        LOG_ERROR("[CodecConfigSbcBase]%{public}s wrong codec", __func__);
        return A2DP_CODEC_ERROR_WRONG_CODEC;
    }

    mediaType = (*codecInfo++) >> BIT_MOVE_RIGHT4;
    codecType = *codecInfo++;
    if ((mediaType != A2DP_MEDIA_TYPE_AUDIO) || (codecType != A2DP_CODEC_TYPE_SBC)) {
        LOG_ERROR("[CodecConfigSbcBase]%{public}s wrong media type", __func__);
        return A2DP_CODEC_ERROR_WRONG_CODEC;
    }
    sbcCap->sampleFreq = *codecInfo & A2DP_SBC_SAMPLE_RATE_MSK;
    sbcCap->channelMode = *codecInfo & A2DP_SBC_CHANNEL_MODE_MSK;
    codecInfo++;
    sbcCap->blockLen = *codecInfo & A2DP_SBC_BLOCKS_MSK;
    sbcCap->numSubbands = *codecInfo & A2DP_SBC_SUBBAND_MSK;
    sbcCap->allocatedMethod = *codecInfo & A2DP_SBC_ALLOC_MODE_MSK;
    codecInfo++;
    sbcCap->minBitpool = *codecInfo++;
    sbcCap->maxBitpool = *codecInfo++;
    if ((sbcCap->minBitpool < A2DP_SBC_MIN_BITPOOL) || (sbcCap->minBitpool > A2DP_SBC_MAX_BITPOOL)) {
        LOG_ERROR("[CodecConfigSbcBase]%{public}s wrong min pool", __func__);
        return A2DP_CODEC_ERROR_BAD_MIN_BITPOOL;
    }

    if ((sbcCap->maxBitpool < A2DP_SBC_MIN_BITPOOL) || (sbcCap->maxBitpool > A2DP_SBC_MAX_BITPOOL) ||
        (sbcCap->maxBitpool < sbcCap->minBitpool)) {
        LOG_ERROR("[CodecConfigSbcBase]%{public}s wrong max pool", __func__);
        return A2DP_CODEC_ERROR_BAD_MAX_BITPOOL;
    }

    return A2DP_SUCCESS;
}


bool IsSbcConfigChanged(const uint8_t *lastCodecInfo, const uint8_t *newCodecInfo)
{
    A2dpSBCCapability lastInfo = {};
    A2dpSBCCapability newInfo = {};

    if (ParseInfoSbc(&lastInfo, lastCodecInfo) != A2DP_SUCCESS) {
        LOG_ERROR("%{public}s: cannot parse lastCodecInfo", __func__);
        return false;
    }

    if (ParseInfoSbc(&newInfo, newCodecInfo) != A2DP_SUCCESS) {
        LOG_ERROR("%{public}s: cannot parse newCodecInfo", __func__);
        return false;
    }

    return (lastInfo.sampleFreq == newInfo.sampleFreq) && (lastInfo.channelMode == newInfo.channelMode) &&
           (lastInfo.blockLen == newInfo.blockLen) && (lastInfo.numSubbands == newInfo.numSubbands) &&
           (lastInfo.allocatedMethod == newInfo.allocatedMethod) && (lastInfo.minBitpool == newInfo.minBitpool) &&
           (lastInfo.maxBitpool == newInfo.maxBitpool);
}
}  // namespace bluetooth
}  // namespace OHOS