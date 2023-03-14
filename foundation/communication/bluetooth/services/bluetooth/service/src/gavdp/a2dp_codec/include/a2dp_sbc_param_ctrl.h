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

#ifndef A2DP_SBC_PARAM_CTRL_H
#define A2DP_SBC_PARAM_CTRL_H

#include <cstdint>

#include "a2dp_codec_config.h"
#include "a2dp_codec_constant.h"
#include "a2dp_codec_wrapper.h"

namespace OHOS {
namespace bluetooth {
class A2dpCodecConfigSbcBase : public A2dpCodecConfig {
public:
    bool SetCodecConfig(const uint8_t *peerCodeInfo, uint8_t *resultCodecInfo) override;
    bool SetPeerCodecCapabilities(const uint8_t *peerCapabilities) override;

protected:
    A2dpCodecConfigSbcBase(A2dpCodecIndex codecIndex, A2dpCodecPriority codecPriority, bool isSource)
        : A2dpCodecConfig(codecIndex, codecPriority), isSource_(isSource)
    {}
    ~A2dpCodecConfigSbcBase() override = default;
    void SetSampleRateUser(uint32_t sampFreq, A2dpSBCCapability &resultCap);
    void SetSampleRateSelectableCapa(uint32_t sampFreq);
    void SetSampleRateCommonCapa(uint32_t sampFreq);
    bool SetSampleRateAudioConfig(uint32_t sampFreq, A2dpSBCCapability &resultCap);
    bool SetSampleRateBestMatch(uint32_t sampFreq, A2dpSBCCapability &resultCap);
    bool SetSampleRateDefaultConfig(uint32_t sampFreq, A2dpSBCCapability &resultCap);
    bool SetSampleRate(
        const A2dpSBCCapability &localCapability, A2dpSBCCapability peerCapability, A2dpSBCCapability &resultCap);
    void SetBitsPerSampleUser();
    void SetBitsPerSampleSelectableCapa(const A2dpSBCCapability &localCapability);
    void SetBitsPerSampleCommonCapa();
    bool SetBitsPerSampleAudioConfig(A2dpSBCCapability &resultCap);
    bool SetBitsPerSampleBestMatch(A2dpSBCCapability &resultCap);
    bool SetBitsPerSampleDefaultConfig(A2dpSBCCapability &resultCap);
    bool SetBitsPerSample(const A2dpSBCCapability &localCapability, A2dpSBCCapability &resultCap);
    void SetChannelModeUser(uint8_t chMode, A2dpSBCCapability &resultCap);
    void SetChannelModeSelectableCapa(uint8_t chMode);
    void SetChannelModeCommonCapa(uint8_t chMode);
    bool SetChannelModeAudioConfig(uint8_t chMode, A2dpSBCCapability &resultCap);
    bool SetChannelModeBestMatch(uint8_t chMode, A2dpSBCCapability &resultCap);
    bool SetChannelModeDefaultConfig(uint8_t chMode, A2dpSBCCapability &resultCap);
    bool SetChannelMode(
        const A2dpSBCCapability &localCapability, A2dpSBCCapability peerCapability, A2dpSBCCapability &resultCap);
    bool SetBlockLength(
        const A2dpSBCCapability &localCapability, A2dpSBCCapability peerCapability, A2dpSBCCapability &resultCap);
    bool SetSubBands(
        const A2dpSBCCapability &localCapability, A2dpSBCCapability peerCapability, A2dpSBCCapability &resultCap);
    bool SetAllocationMethod(
        const A2dpSBCCapability &localCapability, A2dpSBCCapability peerCapability, A2dpSBCCapability &resultCap);
    bool SetBitPool(
        const A2dpSBCCapability &localCapability, A2dpSBCCapability peerCapability, A2dpSBCCapability &resultCap);
    bool SetCodecSpecific(void);
    bool ResetCodecConfig(void);

    void SetSampleFrameCapa(const A2dpSBCCapability &localCapability, A2dpSBCCapability peerCapability);
    void SetBitsPerSampleCapa(const A2dpSBCCapability &localCapability);
    void SetChannelModeCapa(const A2dpSBCCapability &localCapability, A2dpSBCCapability peerCapability);
    bool ResetCodecCapability(void);
    bool isSource_;  // True if local is Source
};

class A2dpCodecConfigSbcSource : public A2dpCodecConfigSbcBase {
public:
    explicit A2dpCodecConfigSbcSource(A2dpCodecPriority codecPriority);
    ~A2dpCodecConfigSbcSource() override = default;
    bool Initialize();
    uint64_t EncoderIntervalMs() const;
    int GetEffectiveMtu() const;
};

class A2dpCodecConfigSbcSink : public A2dpCodecConfigSbcBase {
public:
    explicit A2dpCodecConfigSbcSink(A2dpCodecPriority codecPriority);
    ~A2dpCodecConfigSbcSink() override = default;

    bool Init();
    uint64_t EncoderIntervalMs() const;
    int GetEffectiveMtu() const;
};

uint8_t ParseInfoSbc(A2dpSBCCapability *sbcCap, const uint8_t *codecInfo);
bool IsSbcConfigChanged(const uint8_t *lastCodecInfo, const uint8_t *newCodecInfo);
uint8_t BuildSbcInfo(A2dpSBCCapability *sbcCap, uint8_t *codecInfo);
int GetMinBitpoolSbc(const uint8_t *codecInfo);
int GetMaxBitpoolSbc(const uint8_t *codecInfo);
int GetSampleRateSbc(const uint8_t *codecInfo);
int GetTrackChannelModeSbc(const uint8_t *codecInfo);
int GetTrackChannelCountSbc(const uint8_t *codecInfo);
int GetNumberOfSubbandsSbc(const uint8_t *codecInfo);
int GetNumberOfBlocksSbc(const uint8_t *codecInfo);
int GetAllocationMethodCodeSbc(const uint8_t *codecInfo);
}  // namespace bluetooth
}  // namespace OHOS
#endif  // !A2DP_SBC_PARAM_CTRL_H