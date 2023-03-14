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

#include "include/a2dp_codec_constant.h"
#include "include/a2dp_codec_errors.h"
#include "log.h"
#include "memory.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
const int PCM_CHANNEL_SAMPLES_PER_SAMPLE = 1024;
const int BIT_MOVE_RIGHT16 = 16;
const int BIT_MOVE_RIGHT8 = 8;
const int BIT_MOVE_RIGHT4 = 4;
const int BIT_MOVE_LEFT16 = 16;
const int BIT_MOVE_LEFT8 = 8;
const int BIT_MOVE_LEFT4 = 4;
const int CHANNEL_MODEL1 = 1;
const int CHANNEL_MODEL2 = 2;
const int SAMPLE_RATE8000 = 8000;
const int SAMPLE_RATE11025 = 11025;
const int SAMPLE_RATE12000 = 12000;
const int SAMPLE_RATE16000 = 16000;
const int SAMPLE_RATE22050 = 22050;
const int SAMPLE_RATE24000 = 24000;
const int SAMPLE_RATE32000 = 32000;
const int SAMPLE_RATE44100 = 44100;
const int SAMPLE_RATE48000 = 48000;
const int SAMPLE_RATE64000 = 64000;
const int SAMPLE_RATE88200 = 88200;
const int SAMPLE_RATE96000 = 96000;
const int VALUE_OF_EIGHT = 8;
A2dpCodecConfigAacSource::A2dpCodecConfigAacSource(A2dpCodecPriority codecPriority)
    : A2dpCodecConfigAacBase(A2DP_SOURCE_CODEC_INDEX_AAC, codecPriority, true)
{
    LOG_INFO("[A2dpCodecConfigAacSource] %{public}s", __func__);

    // Compute the local capability
    if (A2DP_AAC_SOURCE_CAPS.sampleFreq & A2DP_AAC_SAMPLE_RATE_OCTET1_44100) {
        codecLocalCap_.sampleRate_ |= A2DP_AAC_SAMPLE_RATE_OCTET1_44100;
    }
    if (A2DP_AAC_SOURCE_CAPS.sampleFreq & A2DP_AAC_SAMPLE_RATE_OCTET1_32000) {
        codecLocalCap_.sampleRate_ |= A2DP_AAC_SAMPLE_RATE_OCTET1_32000;
    }
    if (A2DP_AAC_SOURCE_CAPS.sampleFreq & A2DP_AAC_SAMPLE_RATE_OCTET1_24000) {
        codecLocalCap_.sampleRate_ |= A2DP_AAC_SAMPLE_RATE_OCTET1_24000;
    }
    if (A2DP_AAC_SOURCE_CAPS.sampleFreq & A2DP_AAC_SAMPLE_RATE_OCTET1_22050) {
        codecLocalCap_.sampleRate_ |= A2DP_AAC_SAMPLE_RATE_OCTET1_22050;
    }
    if (A2DP_AAC_SOURCE_CAPS.sampleFreq & A2DP_AAC_SAMPLE_RATE_OCTET1_16000) {
        codecLocalCap_.sampleRate_ |= A2DP_AAC_SAMPLE_RATE_OCTET1_16000;
    }
    if (A2DP_AAC_SOURCE_CAPS.sampleFreq & A2DP_AAC_SAMPLE_RATE_OCTET1_12000) {
        codecLocalCap_.sampleRate_ |= A2DP_AAC_SAMPLE_RATE_OCTET1_12000;
    }
    if (A2DP_AAC_SOURCE_CAPS.sampleFreq & A2DP_AAC_SAMPLE_RATE_OCTET1_11025) {
        codecLocalCap_.sampleRate_ |= A2DP_AAC_SAMPLE_RATE_OCTET1_11025;
    }
    if (A2DP_AAC_SOURCE_CAPS.sampleFreq & A2DP_AAC_SAMPLE_RATE_OCTET1_8000) {
        codecLocalCap_.sampleRate_ |= A2DP_AAC_SAMPLE_RATE_OCTET1_8000;
    }
    if (A2DP_AAC_SOURCE_CAPS.sampleFreq & A2DP_AAC_SAMPLE_RATE_OCTET2_96000) {
        codecLocalCap_.sampleRate_ |= A2DP_AAC_SAMPLE_RATE_OCTET2_96000;
    }
    if (A2DP_AAC_SOURCE_CAPS.sampleFreq & A2DP_AAC_SAMPLE_RATE_OCTET2_88200) {
        codecLocalCap_.sampleRate_ |= A2DP_AAC_SAMPLE_RATE_OCTET2_88200;
    }
    if (A2DP_AAC_SOURCE_CAPS.sampleFreq & A2DP_AAC_SAMPLE_RATE_OCTET2_64000) {
        codecLocalCap_.sampleRate_ |= A2DP_AAC_SAMPLE_RATE_OCTET2_64000;
    }
    if (A2DP_AAC_SOURCE_CAPS.sampleFreq & A2DP_AAC_SAMPLE_RATE_OCTET2_48000) {
        codecLocalCap_.sampleRate_ |= A2DP_AAC_SAMPLE_RATE_OCTET2_48000;
    }

    if (A2DP_AAC_SOURCE_CAPS.channelMode & A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE) {
        codecLocalCap_.channelMode_ |= A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE;
    }
    if (A2DP_AAC_SOURCE_CAPS.channelMode & A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE) {
        codecLocalCap_.channelMode_ |= A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE;
    }
}

A2dpCodecConfigAacSink::A2dpCodecConfigAacSink(A2dpCodecPriority codecPriority)
    : A2dpCodecConfigAacBase(A2DP_SINK_CODEC_INDEX_AAC, codecPriority, false)
{
    LOG_INFO("[A2dpCodecConfigAacSink] %{public}s", __func__);
    // Compute the local capability
    if (A2DP_AAC_SINK_CAPS.sampleFreq & A2DP_AAC_SAMPLE_RATE_OCTET1_44100) {
        codecLocalCap_.sampleRate_ |= A2DP_AAC_SAMPLE_RATE_OCTET1_44100;
    }
    if (A2DP_AAC_SINK_CAPS.sampleFreq & A2DP_AAC_SAMPLE_RATE_OCTET2_96000) {
        codecLocalCap_.sampleRate_ |= A2DP_AAC_SAMPLE_RATE_OCTET2_96000;
    }
    if (A2DP_AAC_SINK_CAPS.sampleFreq & A2DP_AAC_SAMPLE_RATE_OCTET2_88200) {
        codecLocalCap_.sampleRate_ |= A2DP_AAC_SAMPLE_RATE_OCTET2_88200;
    }
    if (A2DP_AAC_SINK_CAPS.sampleFreq & A2DP_AAC_SAMPLE_RATE_OCTET2_48000) {
        codecLocalCap_.sampleRate_ |= A2DP_AAC_SAMPLE_RATE_OCTET2_48000;
    }

    codecLocalCap_.bitsPerSample = A2DP_AAC_SINK_CAPS.bitsPerSample;

    if (A2DP_AAC_SINK_CAPS.channelMode & A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE) {
        codecLocalCap_.channelMode_ |= A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE;
    }
    if (A2DP_AAC_SINK_CAPS.channelMode & A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE) {
        codecLocalCap_.channelMode_ |= A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE;
    }
}

void A2dpCodecConfigAacBase::SetSampleRateUser(uint32_t sampFreq, A2dpAACCapability &resultCap)
{
    switch (userCodecConfig_.sampleRate_) {
        case A2DP_AAC_SAMPLE_RATE_OCTET1_44100:
            if (sampFreq & A2DP_AAC_SAMPLE_RATE_OCTET1_44100) {
                resultCap.sampleFreq = A2DP_AAC_SAMPLE_RATE_OCTET1_44100;
                codecCap_.sampleRate_ = userCodecConfig_.sampleRate_;
                codecConfig_.sampleRate_ = userCodecConfig_.sampleRate_;
            }
            break;
        case A2DP_AAC_SAMPLE_RATE_OCTET2_48000:
            if (sampFreq & A2DP_AAC_SAMPLE_RATE_OCTET2_48000) {
                resultCap.sampleFreq = A2DP_AAC_SAMPLE_RATE_OCTET2_48000;
                codecCap_.sampleRate_ = userCodecConfig_.sampleRate_;
                codecConfig_.sampleRate_ = userCodecConfig_.sampleRate_;
            }
            break;
        case A2DP_AAC_SAMPLE_RATE_OCTET1_32000:
        case A2DP_AAC_SAMPLE_RATE_OCTET1_16000:
        case A2DP_SAMPLE_RATE_NONE:
            resultCap.sampleFreq = A2DP_SAMPLE_RATE_NONE;
            codecCap_.sampleRate_ = A2DP_SAMPLE_RATE_NONE;
            codecConfig_.sampleRate_ = A2DP_SAMPLE_RATE_NONE;
            break;
        default:
            break;
    }
}

void A2dpCodecConfigAacBase::SetSampleRateSelectableCapa(uint32_t sampFreq)
{
    if (sampFreq & A2DP_AAC_SAMPLE_RATE_OCTET1_44100) {
        codecSelectableCap_.sampleRate_ |= A2DP_AAC_SAMPLE_RATE_OCTET1_44100;
    }
    if (sampFreq & A2DP_AAC_SAMPLE_RATE_OCTET2_48000) {
        codecSelectableCap_.sampleRate_ |= A2DP_AAC_SAMPLE_RATE_OCTET2_48000;
    }
}

void A2dpCodecConfigAacBase::SetSampleRateCommonCapa(uint32_t sampFreq)
{
    if (sampFreq & A2DP_AAC_SAMPLE_RATE_OCTET1_44100) {
        codecCap_.sampleRate_ = A2DP_AAC_SAMPLE_RATE_OCTET1_44100;
    }
    if (sampFreq & A2DP_AAC_SAMPLE_RATE_OCTET2_48000) {
        codecCap_.sampleRate_ = A2DP_AAC_SAMPLE_RATE_OCTET2_48000;
    }
}

bool A2dpCodecConfigAacBase::SetSampleRateAudioConfig(uint32_t sampFreq, A2dpAACCapability &resultCap)
{
    switch (audioCodecConfig_.sampleRate_) {
        case A2DP_AAC_SAMPLE_RATE_OCTET1_44100:
            if (sampFreq & A2DP_AAC_SAMPLE_RATE_OCTET1_44100) {
                resultCap.sampleFreq = A2DP_AAC_SAMPLE_RATE_OCTET1_44100;
                codecConfig_.sampleRate_ = A2DP_AAC_SAMPLE_RATE_OCTET1_44100;
                return true;
            }
            break;
        case A2DP_AAC_SAMPLE_RATE_OCTET2_48000:
            if (sampFreq & A2DP_AAC_SAMPLE_RATE_OCTET2_48000) {
                resultCap.sampleFreq = A2DP_AAC_SAMPLE_RATE_OCTET2_48000;
                codecConfig_.sampleRate_ = A2DP_AAC_SAMPLE_RATE_OCTET2_48000;
                return true;
            }
            break;
        default:
            LOG_ERROR("[ConfigAacBase] %{public}s [not 44100 or 48000]", __func__);
            break;
    }
    return false;
}

bool A2dpCodecConfigAacBase::SetSampleRateBestMatch(uint32_t sampFreq, A2dpAACCapability &resultCap)
{
    if (sampFreq & A2DP_AAC_SAMPLE_RATE_OCTET1_44100) {
        resultCap.sampleFreq = A2DP_AAC_SAMPLE_RATE_OCTET1_44100;
        codecConfig_.sampleRate_ = A2DP_AAC_SAMPLE_RATE_OCTET1_44100;
        return true;
    }

    if (sampFreq & A2DP_AAC_SAMPLE_RATE_OCTET2_48000) {
        resultCap.sampleFreq = A2DP_AAC_SAMPLE_RATE_OCTET2_48000;
        codecConfig_.sampleRate_ = A2DP_AAC_SAMPLE_RATE_OCTET2_48000;
        return true;
    }
    return false;
}

bool A2dpCodecConfigAacBase::SetSampleRateDefaultConfig(uint32_t sampFreq, A2dpAACCapability &resultCap)
{
    if (sampFreq & A2DP_AAC_SAMPLE_RATE_OCTET1_44100) {
        resultCap.sampleFreq = A2DP_AAC_SAMPLE_RATE_OCTET1_44100;
        codecConfig_.sampleRate_ = A2DP_AAC_SAMPLE_RATE_OCTET1_44100;
        return true;
    }
    if (sampFreq & A2DP_AAC_SAMPLE_RATE_OCTET2_48000) {
        resultCap.sampleFreq = A2DP_AAC_SAMPLE_RATE_OCTET2_48000;
        codecConfig_.sampleRate_ = A2DP_AAC_SAMPLE_RATE_OCTET2_48000;
        return true;
    }
    return false;
}

bool A2dpCodecConfigAacBase::SetSampleRate(
    const A2dpAACCapability &localCapability, A2dpAACCapability peerCapability, A2dpAACCapability &resultCap)
{
    codecConfig_.sampleRate_ = A2DP_SAMPLE_RATE_NONE;
    uint32_t sampFreq = localCapability.sampleFreq & peerCapability.sampleFreq;

    LOG_INFO("[ConfigAacBase] %{public}s [sampFreq:0x%x] [uFreq:0x%x] [default:0x%x] [aFreq:0x%x] [peer:0x%x]",
        __func__,
        sampFreq,
        userCodecConfig_.sampleRate_,
        A2DP_AAC_DEFAULT_CONFIG.sampleFreq,
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

        if (SetSampleRateDefaultConfig(A2DP_AAC_DEFAULT_CONFIG.sampleFreq & peerCapability.sampleFreq, resultCap)) {
            break;
        }

        if (SetSampleRateBestMatch(sampFreq, resultCap)) {
            break;
        }
    } while (false);

    LOG_INFO("[ConfigAacBase] %{public}s [sampFreq:0x%x]", __func__, codecConfig_.sampleRate_);
    if (codecConfig_.sampleRate_ == A2DP_SAMPLE_RATE_NONE) {
        LOG_ERROR("%{public}s: sampleRate_ had no value need reset config", __func__);
        return ResetCodecConfig();
    }
    return true;
}

void A2dpCodecConfigAacBase::SetBitsPerSampleUser(const A2dpAACCapability &localCapability)
{
    switch (userCodecConfig_.bitsPerSample) {
        case A2DP_SAMPLE_BITS_16:
            if (localCapability.bitsPerSample & A2DP_SAMPLE_BITS_16) {
                codecCap_.bitsPerSample = userCodecConfig_.bitsPerSample;
                codecConfig_.bitsPerSample = userCodecConfig_.bitsPerSample;
            }
            break;
        case A2DP_SAMPLE_BITS_24:
            if (localCapability.bitsPerSample & A2DP_SAMPLE_BITS_24) {
                codecCap_.bitsPerSample = userCodecConfig_.bitsPerSample;
                codecConfig_.bitsPerSample = userCodecConfig_.bitsPerSample;
            }
            break;
        case A2DP_SAMPLE_BITS_32:
            if (localCapability.bitsPerSample & A2DP_SAMPLE_BITS_32) {
                codecCap_.bitsPerSample = userCodecConfig_.bitsPerSample;
                codecConfig_.bitsPerSample = userCodecConfig_.bitsPerSample;
            }
            break;
        case A2DP_SAMPLE_BITS_NONE:
            codecCap_.bitsPerSample = A2DP_SAMPLE_BITS_NONE;
            codecConfig_.bitsPerSample = A2DP_SAMPLE_BITS_NONE;
            break;
        default:
            break;
    }
}

void A2dpCodecConfigAacBase::SetBitsPerSampleSelectableCapa(const A2dpAACCapability &localCapability)
{
    codecSelectableCap_.bitsPerSample = localCapability.bitsPerSample;
}

void A2dpCodecConfigAacBase::SetBitsPerSampleCommonCapa(const A2dpAACCapability &localCapability)
{
    codecCap_.bitsPerSample = localCapability.bitsPerSample;
}

bool A2dpCodecConfigAacBase::SetBitsPerSampleAudioConfig(
    const A2dpAACCapability &localCapability, A2dpAACCapability &resultCap)
{
    switch (audioCodecConfig_.bitsPerSample) {
        case A2DP_SAMPLE_BITS_16:
            if (localCapability.bitsPerSample & A2DP_SAMPLE_BITS_16) {
                codecConfig_.bitsPerSample = A2DP_SAMPLE_BITS_16;
                resultCap.bitsPerSample = A2DP_SAMPLE_BITS_16;
                return true;
            }
            break;
        case A2DP_SAMPLE_BITS_24:
            if (localCapability.bitsPerSample & A2DP_SAMPLE_BITS_24) {
                codecConfig_.bitsPerSample = A2DP_SAMPLE_BITS_24;
                resultCap.bitsPerSample = A2DP_SAMPLE_BITS_24;
                return true;
            }
            break;
        case A2DP_SAMPLE_BITS_32:
            if (localCapability.bitsPerSample & A2DP_SAMPLE_BITS_32) {
                codecConfig_.bitsPerSample = A2DP_SAMPLE_BITS_32;
                resultCap.bitsPerSample = A2DP_SAMPLE_BITS_32;
                return true;
            }
            break;
        default:
            LOG_INFO("[ConfigAacBase] %{public}s [none]", __func__);
            break;
    }
    LOG_INFO("[ConfigAacBase] %{public}s [no match audio config]", __func__);
    return false;
}

bool A2dpCodecConfigAacBase::SetBitsPerSampleBestMatch(
    const A2dpAACCapability &localCapability, A2dpAACCapability &resultCap)
{
    if (localCapability.bitsPerSample & A2DP_SAMPLE_BITS_16) {
        codecConfig_.bitsPerSample = A2DP_SAMPLE_BITS_16;
        resultCap.bitsPerSample = A2DP_SAMPLE_BITS_16;
        return true;
    }

    if (localCapability.bitsPerSample & A2DP_SAMPLE_BITS_24) {
        codecConfig_.bitsPerSample = A2DP_SAMPLE_BITS_24;
        resultCap.bitsPerSample = A2DP_SAMPLE_BITS_24;
        return true;
    }

    if (localCapability.bitsPerSample & A2DP_SAMPLE_BITS_32) {
        codecConfig_.bitsPerSample = A2DP_SAMPLE_BITS_32;
        resultCap.bitsPerSample = A2DP_SAMPLE_BITS_32;
        return true;
    }
    LOG_INFO("[ConfigAacBase] %{public}s [no match audio config]", __func__);
    return false;
}

bool A2dpCodecConfigAacBase::SetBitsPerSampleDefaultConfig(uint8_t bitPerSample, A2dpAACCapability &resultCap)
{
    if (bitPerSample & A2DP_SAMPLE_BITS_16) {
        codecConfig_.bitsPerSample = A2DP_SAMPLE_BITS_16;
        resultCap.bitsPerSample = A2DP_SAMPLE_BITS_16;
        return true;
    }

    if (bitPerSample & A2DP_SAMPLE_BITS_24) {
        codecConfig_.bitsPerSample = A2DP_SAMPLE_BITS_24;
        resultCap.bitsPerSample = A2DP_SAMPLE_BITS_24;
        return true;
    }

    if (bitPerSample & A2DP_SAMPLE_BITS_32) {
        codecConfig_.bitsPerSample = A2DP_SAMPLE_BITS_32;
        resultCap.bitsPerSample = A2DP_SAMPLE_BITS_32;
        return true;
    }

    LOG_INFO("[ConfigAacBase] %{public}s[bitsPerSample:0x%x]", __func__, codecConfig_.bitsPerSample);
    return false;
}

bool A2dpCodecConfigAacBase::SetBitsPerSample(const A2dpAACCapability &localCapability, A2dpAACCapability &resultCap)
{
    LOG_INFO("[ConfigAacBase] %{public}s [uBitsPerSample:0x%x] [aBitsPerSample::0x%x][dBitsPerSample::0x%x]",
        __func__,
        userCodecConfig_.bitsPerSample,
        audioCodecConfig_.bitsPerSample,
        A2DP_AAC_DEFAULT_CONFIG.bitsPerSample);

    codecConfig_.bitsPerSample = A2DP_SAMPLE_BITS_NONE;

    SetBitsPerSampleUser(localCapability);

    do {
        SetBitsPerSampleSelectableCapa(localCapability);

        if (codecConfig_.bitsPerSample != A2DP_SAMPLE_BITS_NONE) {
            break;
        }

        SetBitsPerSampleCommonCapa(localCapability);

        if (SetBitsPerSampleAudioConfig(localCapability, resultCap)) {
            break;
        }

        if (SetBitsPerSampleDefaultConfig(A2DP_AAC_DEFAULT_CONFIG.bitsPerSample, resultCap)) {
            break;
        }

        if (SetBitsPerSampleBestMatch(localCapability, resultCap)) {
            break;
        }
    } while (false);

    LOG_INFO("[ConfigAacBase] %{public}s [bitsPerSample:0x%x]", __func__, codecConfig_.bitsPerSample);

    if (codecConfig_.bitsPerSample == A2DP_SAMPLE_BITS_NONE) {
        LOG_ERROR("%{public}s: bitsPerSample had no value", __func__);
        return ResetCodecConfig();
    }
    return true;
}

void A2dpCodecConfigAacBase::SetChannelModeUser(uint8_t chMode, A2dpAACCapability &resultCap)
{
    switch (userCodecConfig_.channelMode_) {
        case A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE:
            if (chMode & A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE) {
                resultCap.channelMode = A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE;
                codecCap_.channelMode_ = userCodecConfig_.channelMode_;
                codecConfig_.channelMode_ = userCodecConfig_.channelMode_;
            }
            break;
        case A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE:
            if (chMode & A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE) {
                resultCap.channelMode = A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE;
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

void A2dpCodecConfigAacBase::SetChannelModeSelectableCapa(uint8_t chMode)
{
    if (chMode & A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE) {
        codecSelectableCap_.channelMode_ |= A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE;
    }
    if (chMode & A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE) {
        codecSelectableCap_.channelMode_ |= A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE;
    }
}

void A2dpCodecConfigAacBase::SetChannelModeCommonCapa(uint8_t chMode)
{
    if (chMode & A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE) {
        codecCap_.channelMode_ |= A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE;
    }

    if (chMode & A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE) {
        codecCap_.channelMode_ |= A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE;
    }
}

bool A2dpCodecConfigAacBase::SetChannelModeAudioConfig(uint8_t chMode, A2dpAACCapability &resultCap)
{
    switch (audioCodecConfig_.channelMode_) {
        case A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE:
            if (chMode & A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE) {
                resultCap.channelMode = A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE;
                codecConfig_.channelMode_ = A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE;
                return true;
            }
            break;
        case A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE:
            if (chMode & A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE) {
                resultCap.channelMode = A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE;
                codecConfig_.channelMode_ = A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE;
                return true;
            }
            break;
        case A2DP_CHANNEL_MODE_NONE:
            break;
        default:
            break;
    }
    return false;
}

bool A2dpCodecConfigAacBase::SetChannelModeBestMatch(uint8_t chMode, A2dpAACCapability &resultCap)
{
    if (chMode & A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE) {
        resultCap.channelMode = A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE;
        codecConfig_.channelMode_ = A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE;
        return true;
    }

    if (chMode & A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE) {
        resultCap.channelMode = A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE;
        codecConfig_.channelMode_ = A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE;
        return true;
    }
    return false;
}

bool A2dpCodecConfigAacBase::SetChannelModeDefaultConfig(uint8_t chMode, A2dpAACCapability &resultCap)
{
    if (chMode & A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE) {
        resultCap.channelMode = A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE;
        codecConfig_.channelMode_ = A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE;
        return true;
    }

    if (chMode & A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE) {
        resultCap.channelMode = A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE;
        codecConfig_.channelMode_ = A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE;
        return true;
    }
    return false;
}

bool A2dpCodecConfigAacBase::SetChannelMode(
    const A2dpAACCapability &localCapability, A2dpAACCapability peerCapability, A2dpAACCapability &resultCap)
{
    uint8_t chMode = localCapability.channelMode & peerCapability.channelMode;
    codecConfig_.channelMode_ = A2DP_CHANNEL_MODE_NONE;

    LOG_INFO("[ConfigAacBase] %{public}s [chMode:0x%x] [uMode:0x%x] [default:0x%x] [aMode:0x%x] [peer:0x%x]",
        __func__,
        chMode,
        userCodecConfig_.channelMode_,
        A2DP_AAC_DEFAULT_CONFIG.channelMode,
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

        if (SetChannelModeDefaultConfig(A2DP_AAC_DEFAULT_CONFIG.channelMode & peerCapability.channelMode, resultCap)) {
            break;
        }

        if (SetChannelModeBestMatch(chMode, resultCap)) {
            break;
        }
    } while (false);

    LOG_INFO("[ConfigAacBase] %{public}s [channelMode_:0x%x]", __func__, codecConfig_.channelMode_);

    if (codecConfig_.channelMode_ == A2DP_CHANNEL_MODE_NONE) {
        LOG_ERROR("%{public}s: channelMode had no value", __func__);
        return ResetCodecConfig();
    }
    return true;
}

bool A2dpCodecConfigAacBase::SetCodecSpecific(void)
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

bool A2dpCodecConfigAacBase::ResetCodecConfig(void)
{
    LOG_ERROR("[ConfigAacBase] %{public}s", __func__);
    codecConfig_ = savedCodecConfig_;
    codecCap_ = savedCodecCapability_;
    userCodecConfig_ = savedCodecUserConfig_;
    (void)memcpy_s(otaCodecConfig_, A2DP_CODEC_SIZE, savedOtaCodecConfig_, A2DP_CODEC_SIZE);
    return false;
}

bool A2dpCodecConfigAacBase::SetCodecConfig(const uint8_t *peerCodeInfo, uint8_t *resultCodecInfo)
{
    LOG_INFO("[ConfigAacBase] %{public}s\n", __func__);
    const A2dpAACCapability *a2dpAacCaps = (isSource_) ? &A2DP_AAC_SOURCE_CAPS : &A2DP_AAC_SINK_CAPS;
    A2dpAACCapability cfgCap = {};
    A2dpAACCapability cfgCapResult = {};
    savedCodecConfig_ = codecConfig_;
    savedCodecCapability_ = codecCap_;
    savedCodecUserConfig_ = userCodecConfig_;
    (void)memcpy_s(savedOtaCodecConfig_, A2DP_CODEC_SIZE, otaCodecConfig_, A2DP_CODEC_SIZE);

    if (ParseInfoAac(&cfgCap, peerCodeInfo) != A2DP_SUCCESS) {
        LOG_ERROR("[ConfigAacBase] %{public}s: can't parse peer's capabilities\n", __func__);
        return ResetCodecConfig();
    }

    cfgCapResult.objectType = a2dpAacCaps->objectType;
    cfgCapResult.variableBitRateSupport = a2dpAacCaps->variableBitRateSupport & cfgCap.variableBitRateSupport;

    if (cfgCap.bitRate < A2DP_AAC_MIN_BITRATE) {
        // Bogus bit rate
        cfgCapResult.bitRate = a2dpAacCaps->bitRate;
    } else {
        cfgCapResult.bitRate = std::min(a2dpAacCaps->bitRate, cfgCap.bitRate);
    }

    if (SetSampleRate(*a2dpAacCaps, cfgCap, cfgCapResult) != true) {
        LOG_ERROR("%{public}s: SetSampleRate failed", __func__);
    }
    if (SetBitsPerSample(*a2dpAacCaps, cfgCapResult) != true) {
        LOG_ERROR("%{public}s: SetBitsPerSample failed", __func__);
    }
    if (SetChannelMode(*a2dpAacCaps, cfgCap, cfgCapResult) != true) {
        LOG_ERROR("%{public}s: SetChannelMode failed", __func__);
    }

    if (BuildAacInfo(&cfgCapResult, resultCodecInfo) != A2DP_SUCCESS) {
        LOG_INFO("[ConfigAacBase] %{public}s: BuildAacInfo for return config failed\n", __func__);
        return ResetCodecConfig();
    }
    SetCodecSpecific();
    if (BuildAacInfo(&cfgCapResult, otaCodecConfig_) != A2DP_SUCCESS) {
        LOG_INFO("[ConfigAacBase] %{public}s: Build Aac Info failed\n", __func__);
        return ResetCodecConfig();
    }
    return true;
}

void A2dpCodecConfigAacBase::SetSampleFrameCapa(
    const A2dpAACCapability &localCapability, A2dpAACCapability peerCapability)
{
    uint32_t sampFreq = localCapability.sampleFreq & peerCapability.sampleFreq;
    if (sampFreq & A2DP_AAC_SAMPLE_RATE_OCTET1_44100) {
        codecSelectableCap_.sampleRate_ |= A2DP_AAC_SAMPLE_RATE_OCTET1_44100;
    }
    if (sampFreq & A2DP_AAC_SAMPLE_RATE_OCTET2_48000) {
        codecSelectableCap_.sampleRate_ |= A2DP_AAC_SAMPLE_RATE_OCTET2_48000;
    }
}

void A2dpCodecConfigAacBase::SetBitsPerSampleCapa(const A2dpAACCapability &localCapability)
{
    codecSelectableCap_.bitsPerSample = localCapability.bitsPerSample;
}

void A2dpCodecConfigAacBase::SetChannelModeCapa(
    const A2dpAACCapability &localCapability, A2dpAACCapability peerCapability)
{
    uint32_t chMode = localCapability.channelMode & peerCapability.channelMode;
    if (chMode & A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE) {
        codecSelectableCap_.channelMode_ |= A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE;
    }
    if (chMode & A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE) {
        codecSelectableCap_.channelMode_ |= A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE;
    }
}

bool A2dpCodecConfigAacBase::ResetCodecCapability(void)
{
    LOG_ERROR("[ConfigAacBase] %{public}s", __func__);
    codecSelectableCap_ = savedCodecSelectableCapability;
    (void)memcpy_s(otaCodecPeerCapability_, A2DP_CODEC_SIZE, savedOtaCodecPeerCapability, A2DP_CODEC_SIZE);
    return false;
}

bool A2dpCodecConfigAacBase::SetPeerCodecCapabilities(const uint8_t *peerCapabilities)
{
    const A2dpAACCapability *a2dpAacCaps = (isSource_) ? &A2DP_AAC_SOURCE_CAPS : &A2DP_AAC_SINK_CAPS;

    A2dpAACCapability cfgCap = {};

    if (ParseInfoAac(&cfgCap, peerCapabilities) != A2DP_SUCCESS) {
        LOG_ERROR("[ConfigAacBase]%{public}s: can't parse peer's capabilities", __func__);
        return ResetCodecCapability();
    }
    // Compute the selectable capability - sample rate
    SetSampleFrameCapa(*a2dpAacCaps, cfgCap);
    // Compute the selectable capability - bits per sample
    SetBitsPerSampleCapa(*a2dpAacCaps);
    // Compute the selectable capability - channel mode
    SetChannelModeCapa(*a2dpAacCaps, cfgCap);

    if (BuildAacInfo(&cfgCap, otaCodecPeerCapability_) != A2DP_SUCCESS) {
        LOG_ERROR("[ConfigAacBase]%{public}s: BuildAacInfo Error", __func__);
        return ResetCodecCapability();
    }
    return true;
}

int GetBitrateAac(const uint8_t *codecInfo)
{
    A2dpAACCapability cfgCap = {};
    if (ParseInfoAac(&cfgCap, codecInfo) != A2DP_SUCCESS) {
        LOG_ERROR("%{public}s: cannot get bitRate information", __func__);
        return -1;
    }
    LOG_INFO("[ConfigAacBase] %{public}s [bitRate:0x%x]", __func__, cfgCap.bitRate);
    return cfgCap.bitRate;
}

int GetVariableBitRateSupportAac(const uint8_t *codecInfo)
{
    A2dpAACCapability cfgCap = {};

    // Check whether the codec info contains valid data
    if (ParseInfoAac(&cfgCap, codecInfo) != A2DP_SUCCESS) {
        LOG_ERROR("%{public}s: cannot decode codec information", __func__);
        return -1;
    }
    LOG_INFO("%{public}s [variableBitRateSupport:0x%x]", __func__, cfgCap.variableBitRateSupport);
    switch (cfgCap.variableBitRateSupport) {
        case A2DP_AAC_VARIABLE_BIT_RATE_DISABLED:
        case A2DP_AAC_VARIABLE_BIT_RATE_ENABLED:
            return cfgCap.variableBitRateSupport;
        default:
            break;
    }
    return -1;
}

int GetSampleRateAac(const uint8_t *codecInfo)
{
    A2dpAACCapability cfgCap = {};
    if (ParseInfoAac(&cfgCap, codecInfo) != A2DP_SUCCESS) {
        LOG_ERROR("%{public}s: cannot get SampleRate Aac information", __func__);
        return -1;
    }
    LOG_INFO("[A2dpCodecConfigAacBase] %{public}s [sampleFreq:0x%x]", __func__, cfgCap.sampleFreq);
    switch (cfgCap.sampleFreq) {
        case A2DP_AAC_SAMPLE_RATE_OCTET1_8000:
            return SAMPLE_RATE8000;
        case A2DP_AAC_SAMPLE_RATE_OCTET1_11025:
            return SAMPLE_RATE11025;
        case A2DP_AAC_SAMPLE_RATE_OCTET1_12000:
            return SAMPLE_RATE12000;
        case A2DP_AAC_SAMPLE_RATE_OCTET1_16000:
            return SAMPLE_RATE16000;
        case A2DP_AAC_SAMPLE_RATE_OCTET1_22050:
            return SAMPLE_RATE22050;
        case A2DP_AAC_SAMPLE_RATE_OCTET1_24000:
            return SAMPLE_RATE24000;
        case A2DP_AAC_SAMPLE_RATE_OCTET1_32000:
            return SAMPLE_RATE32000;
        case A2DP_AAC_SAMPLE_RATE_OCTET1_44100:
            return SAMPLE_RATE44100;
        case A2DP_AAC_SAMPLE_RATE_OCTET2_48000:
            return SAMPLE_RATE48000;
        case A2DP_AAC_SAMPLE_RATE_OCTET2_64000:
            return SAMPLE_RATE64000;
        case A2DP_AAC_SAMPLE_RATE_OCTET2_88200:
            return SAMPLE_RATE88200;
        case A2DP_AAC_SAMPLE_RATE_OCTET2_96000:
            return SAMPLE_RATE96000;
        default:
            break;
    }

    return -1;
}
int ComputeMaxBitRateAac(const uint8_t *codecInfo, uint16_t mtu)
{
    A2dpAACCapability cfgCap = {};

    // Check whether the codec info contains valid data
    if (ParseInfoAac(&cfgCap, codecInfo) != A2DP_SUCCESS) {
        LOG_ERROR("%{public}s: cannot get SampleRate Aac information", __func__);
        return -1;
    }

    int samplingFreq = GetSampleRateAac(codecInfo);
    if (samplingFreq == -1) {
        return -1;
    }

    int samplesPerFrame = 0;
    switch (cfgCap.objectType) {
        case A2DP_AAC_MPEG2_OBJECT_TYPE:
        case A2DP_AAC_MPEG4_LC_OBJECT_TYPE:
            samplesPerFrame = PCM_CHANNEL_SAMPLES_PER_SAMPLE;
            break;
        case A2DP_AAC_MPEG4_LTP_OBJECT_TYPE:
        case A2DP_AAC_MPEG4_SCALABLE_OBJECT_TYPE:
            break;
        default:
            break;
    }
    if (samplesPerFrame == 0) {
        return -1;
    }

    // See Section 3.2.1 Estimating Average Frame Size from
    // the aacEncoder.pdf document included with the AAC source code.
    return (VALUE_OF_EIGHT * mtu * samplingFreq) / samplesPerFrame;
}

int GetTrackChannelModeAac(const uint8_t *codecInfo)
{
    A2dpAACCapability cfgCap = {};
    if (ParseInfoAac(&cfgCap, codecInfo) != A2DP_SUCCESS) {
        LOG_ERROR("%{public}s: cannot get Track ChannelMode information", __func__);
        return -1;
    }
    LOG_INFO("[ConfigAacBase] %{public}s [channelMode:0x%x]", __func__, cfgCap.channelMode);
    return cfgCap.channelMode;
}

int GetTrackChannelCountAac(const uint8_t *codecInfo)
{
    A2dpAACCapability cfgCap = {};
    if (ParseInfoAac(&cfgCap, codecInfo) != A2DP_SUCCESS) {
        LOG_ERROR("%{public}s: cannot get TrackChannel information", __func__);
        return -1;
    }
    LOG_INFO("[ConfigAacBase] %{public}s [channelMode:0x%x]", __func__, cfgCap.channelMode);
    switch (cfgCap.channelMode) {
        case A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE:
            return CHANNEL_MODEL1;
        case A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE:
            return CHANNEL_MODEL2;
        default:
            break;
    }
    return -1;
}

int GetObjectTypeCodeAac(const uint8_t *codecInfo)
{
    A2dpAACCapability cfgCap = {};

    // Check whether the codec info contains valid data
    if (A2DP_SUCCESS != ParseInfoAac(&cfgCap, codecInfo)) {
        LOG_ERROR("%{public}s: cannot decode codec information", __func__);
        return -1;
    }
    LOG_INFO("%{public}s [objectType:0x%x]", __func__, cfgCap.objectType);
    return cfgCap.objectType;
}

uint8_t BuildAacInfo(const A2dpAACCapability *aacCap, uint8_t *codecInfo)
{
    LOG_INFO("[ConfigAacBase] %{public}s\n", __func__);
    /// AAC codec Info
    *codecInfo++ = A2DP_CODEC_AAC_INFO_LEN;  /// aac information length
    *codecInfo++ = (A2DP_MEDIA_TYPE_AUDIO << BIT_MOVE_LEFT4);
    *codecInfo++ = A2DP_CODEC_TYPE_AAC;  /// AAC media codec type
    *codecInfo++ = aacCap->objectType;
    *codecInfo++ = aacCap->sampleFreq;
    *codecInfo++ = ((aacCap->sampleFreq >> BIT_MOVE_RIGHT8) & 0xF0) | (aacCap->channelMode & 0x0C);
    *codecInfo++ = (aacCap->variableBitRateSupport & 0x80) | ((aacCap->bitRate >> BIT_MOVE_RIGHT16) & 0x7F);
    *codecInfo++ = ((aacCap->bitRate >> BIT_MOVE_RIGHT8) & 0xFF);
    *codecInfo++ = (aacCap->bitRate & 0xFF);
    return A2DP_SUCCESS;
}

uint8_t ParseInfoAac(A2dpAACCapability *aacCap, const uint8_t *codecInfo)
{
    LOG_INFO("[ConfigAacBase] %{public}s\n", __func__);
    uint8_t losc;
    uint8_t mediaType;
    uint8_t codecType;
    if (codecInfo == nullptr) {
        return A2DP_CODEC_ERROR_INVALID_PARAMS;
    }
    // Check the codec capability length931
    losc = *codecInfo++;
    if (losc != A2DP_CODEC_AAC_INFO_LEN) {
        return A2DP_CODEC_ERROR_WRONG_CODEC;
    }

    mediaType = (*codecInfo++) >> BIT_MOVE_RIGHT4;
    codecType = *codecInfo++;
    /* Check the Media Type and Media Codec Type */
    if (mediaType != A2DP_MEDIA_TYPE_AUDIO || codecType != A2DP_CODEC_TYPE_AAC) {
        return A2DP_CODEC_ERROR_WRONG_CODEC;
    }
    aacCap->objectType = *codecInfo++ & 0xF0;
    aacCap->sampleFreq = *codecInfo++ & 0xFF;
    aacCap->sampleFreq |= (*codecInfo & 0xF0) << BIT_MOVE_LEFT8;
    aacCap->channelMode = *codecInfo & 0x0C;
    codecInfo++;
    aacCap->variableBitRateSupport = *codecInfo & 0x80;
    aacCap->bitRate = (*codecInfo++) << BIT_MOVE_LEFT16;
    aacCap->bitRate |= (*codecInfo++) << BIT_MOVE_LEFT8;
    aacCap->bitRate |= *codecInfo++;

    return A2DP_SUCCESS;
}

bool IsAacConfigChanged(const uint8_t *lastCodecInfo, const uint8_t *newCodecInfo)
{
    A2dpAACCapability lastInfo = {};
    A2dpAACCapability newInfo = {};

    if (ParseInfoAac(&lastInfo, lastCodecInfo) != A2DP_SUCCESS) {
        LOG_ERROR("%{public}s: cannot parse lastCodecInfo", __func__);
        return false;
    }

    if (ParseInfoAac(&newInfo, newCodecInfo) != A2DP_SUCCESS) {
        LOG_ERROR("%{public}s: cannot parse newCodecInfo", __func__);
        return false;
    }

    return (lastInfo.sampleFreq == newInfo.sampleFreq) && (lastInfo.channelMode == newInfo.channelMode) &&
           (lastInfo.bitRate == newInfo.bitRate);
}
}  // namespace bluetooth
}  // namespace OHOS