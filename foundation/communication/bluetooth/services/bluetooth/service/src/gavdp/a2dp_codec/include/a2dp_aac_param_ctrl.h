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

#ifndef A2DP_AAC_PARAM_CTRL_H
#define A2DP_AAC_PARAM_CTRL_H

#include <cstdint>

#include "a2dp_aac_param_ctrl.h"
#include "a2dp_codec_config.h"
#include "a2dp_codec_constant.h"
#include "a2dp_codec_wrapper.h"

namespace OHOS {
namespace bluetooth {
class A2dpCodecConfigAacBase : public A2dpCodecConfig {
public:
    bool SetCodecConfig(const uint8_t *peerCodeInfo, uint8_t *resultCodecInfo) override;
    bool SetPeerCodecCapabilities(const uint8_t *peerCapabilities) override;

protected:
    A2dpCodecConfigAacBase(A2dpCodecIndex codecIndex, A2dpCodecPriority codecPriority, bool isSource)
        : A2dpCodecConfig(codecIndex, codecPriority), isSource_(isSource)
    {}
    ~A2dpCodecConfigAacBase() override = default;
    void SetSampleRateUser(uint32_t sampFreq, A2dpAACCapability &resultCap);
    void SetSampleRateSelectableCapa(uint32_t sampFreq);
    void SetSampleRateCommonCapa(uint32_t sampFreq);
    bool SetSampleRateAudioConfig(uint32_t sampFreq, A2dpAACCapability &resultCap);
    bool SetSampleRateDefaultConfig(uint32_t sampFreq, A2dpAACCapability &resultCap);
    bool SetSampleRateBestMatch(uint32_t sampFreq, A2dpAACCapability &resultCap);
    bool SetSampleRate(
        const A2dpAACCapability &localCapability, A2dpAACCapability peerCapability, A2dpAACCapability &resultCap);
    void SetBitsPerSampleUser(const A2dpAACCapability &localCapability);
    void SetBitsPerSampleSelectableCapa(const A2dpAACCapability &localCapability);
    void SetBitsPerSampleCommonCapa(const A2dpAACCapability &localCapability);
    bool SetBitsPerSampleAudioConfig(const A2dpAACCapability &localCapability, A2dpAACCapability &resultCap);
    bool SetBitsPerSampleBestMatch(const A2dpAACCapability &localCapability, A2dpAACCapability &resultCap);
    bool SetBitsPerSampleDefaultConfig(uint8_t bitPerSample, A2dpAACCapability &resultCap);
    bool SetBitsPerSample(const A2dpAACCapability &localCapability, A2dpAACCapability &resultCap);
    void SetChannelModeUser(uint8_t chMode, A2dpAACCapability &resultCap);
    void SetChannelModeSelectableCapa(uint8_t chMode);
    void SetChannelModeCommonCapa(uint8_t chMode);
    bool SetChannelModeAudioConfig(uint8_t chMode, A2dpAACCapability &resultCap);
    bool SetChannelModeBestMatch(uint8_t chMode, A2dpAACCapability &resultCap);
    bool SetChannelModeDefaultConfig(uint8_t chMode, A2dpAACCapability &resultCap);
    bool SetChannelMode(
        const A2dpAACCapability &localCapability, A2dpAACCapability peerCapability, A2dpAACCapability &resultCap);
    bool SetBlockLength(
        const A2dpAACCapability &localCapability, A2dpAACCapability peerCapability, A2dpAACCapability &resultCap);
    bool SetSubBands(
        const A2dpAACCapability &localCapability, A2dpAACCapability peerCapability, A2dpAACCapability &resultCap);
    bool SetAllocationMethod(
        const A2dpAACCapability &localCapability, A2dpAACCapability peerCapability, A2dpAACCapability &resultCap);
    bool SetBitPool(
        const A2dpAACCapability &localCapability, A2dpAACCapability peerCapability, A2dpAACCapability &resultCap);
    bool SetCodecSpecific(void);
    bool ResetCodecConfig(void);

    void SetSampleFrameCapa(const A2dpAACCapability &localCapability, A2dpAACCapability peerCapability);
    void SetBitsPerSampleCapa(const A2dpAACCapability &localCapability);
    void SetChannelModeCapa(const A2dpAACCapability &localCapability, A2dpAACCapability peerCapability);
    bool ResetCodecCapability(void);
    bool isSource_;  // True if local is Source
};

class A2dpCodecConfigAacSource : public A2dpCodecConfigAacBase {
public:
    explicit A2dpCodecConfigAacSource(A2dpCodecPriority codecPriority);
    ~A2dpCodecConfigAacSource() override = default;
    bool Initialize();
    uint64_t EncoderIntervalMs() const;
    int GetEffectiveMtu() const;
};

class A2dpCodecConfigAacSink : public A2dpCodecConfigAacBase {
public:
    explicit A2dpCodecConfigAacSink(A2dpCodecPriority codecPriority);
    ~A2dpCodecConfigAacSink() override = default;

    bool Init();
    uint64_t EncoderIntervalMs() const;
    int GetEffectiveMtu() const;
};

uint8_t ParseInfoAac(A2dpAACCapability *aacCap, const uint8_t *codecInfo);
bool IsAacConfigChanged(const uint8_t *lastCodecInfo, const uint8_t *newCodecInfo);
uint8_t BuildAacInfo(const A2dpAACCapability *aacCap, uint8_t *codecInfo);
int GetVariableBitRateSupportAac(const uint8_t *codecInfo);
int GetBitrateAac(const uint8_t *codecInfo);
int GetSampleRateAac(const uint8_t *codecInfo);
int ComputeMaxBitRateAac(const uint8_t *codecInfo, uint16_t mtu);
int GetTrackChannelModeAac(const uint8_t *codecInfo);
int GetTrackChannelCountAac(const uint8_t *codecInfo);
int GetObjectTypeCodeAac(const uint8_t *codecInfo);
}  // namespace bluetooth
}  // namespace OHOS
#endif  // !A2DP_AAC_PARAM_CTRL_H