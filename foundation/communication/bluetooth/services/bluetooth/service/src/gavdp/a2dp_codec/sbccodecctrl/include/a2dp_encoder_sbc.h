/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef A2DP_ENCODER_SBC_H
#define A2DP_ENCODER_SBC_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include "a2dp_sbc_dynamic_lib_ctrl.h"
#include "../../include/a2dp_codec_config.h"
#include "../../include/a2dp_codec_wrapper.h"
#include "packet.h"

namespace OHOS {
namespace bluetooth {
/* A2DP sbc max mtu size.(byte) */
constexpr int A2DP_SBC_MAX_PACKET_SIZE = 6144;
constexpr int MAX_2MBPS_SBC_PAYLOAD_MTU = 655;
constexpr int MAX_3MBPS_SBC_PAYLOAD_MTU = 997;
constexpr int MAX_PCM_FRAME_NUM_PER_TICK = 14;
/* AVDP media headsize. */
constexpr int A2DP_SBC_PACKET_HEAD_SIZE = 23;
constexpr int A2DP_SBC_MEDIA_PAYLOAD_HEAD_SIZE = 1;
constexpr int A2DP_SBC_ENCODER_INTERVAL_MS = 20;
constexpr int A2DP_SBC_NON_EDR_MAX_BITRATE = 229;
constexpr int A2DP_SBC_DEFAULT_BITRATE = 328;
constexpr int A2DP_SBC_HQ_DUAL_BP_53_FRAME_SIZE = 224;
constexpr int A2DP_SBC_FRAGMENT_HEADER = 1;
/* sample rate for sbc encode param */
constexpr int SBC_SAMPLE_RATE_16000 = 16000;
constexpr int SBC_SAMPLE_RATE_32000 = 32000;
constexpr int SBC_SAMPLE_RATE_44100 = 44100;
constexpr int SBC_SAMPLE_RATE_48000 = 48000;
/* sample freq for sbc encode param */
constexpr int SBC_SAMPLE_BITS_16 = 16;
constexpr int SBC_SAMPLE_BITS_24 = 24;
constexpr int SBC_SAMPLE_BITS_32 = 32;
/* channel Mode for sbc encode param */
constexpr int SBC_MONO = 0;
constexpr int SBC_DUAL = 1;
constexpr int SBC_STEREO = 2;
constexpr int SBC_JOINT_STEREO = 3;
/* channel count for sbc encode param */
constexpr int SBC_SINGLE_CHANNEL_COUNT = 1;
constexpr int SBC_DOUBLE_CHANNEL_COUNT = 2;
/* sub-band for sbc encode param */
constexpr int SBC_SUBBAND_4 = 4;
constexpr int SBC_SUBBAND_8 = 8;
/* block-length for sbc encode param */
constexpr int SBC_BLOCKS_4 = 4;
constexpr int SBC_BLOCKS_8 = 8;
constexpr int SBC_BLOCKS_12 = 12;
constexpr int SBC_BLOCKS_16 = 16;
/* allocation-method for sbc encode param */
constexpr int SBC_LOUDNESS = 0;
constexpr int SBC_SNR = 1;
constexpr int FRAME_TWO = 2;
constexpr int FRAME_THREE = 3;

struct A2dpSBCFeedingParams {
    uint32_t sampleRate;     // 16000, 32000, 44100 or 48000.
    uint16_t bitsPerSample;  // 8, 16, 24 or32.
    uint8_t channelCount;    // mono: 1, else 2.
};

struct A2dpSbcFeedingState {
    uint32_t aaFrameCounter;
    int32_t aaFeedCounter;
    int32_t aaFeedResidue;
    uint32_t counter;
    uint32_t bytesPerTick;          // Pcm bytes read during each media task tick.
    uint64_t lastFrameTimestampNs;  // Last tick timestamp, values in ns.
};

struct SBCEncoderParams {
    uint32_t samplingFreq;     // 16000, 32000, 44100 or 48000.
    int16_t channelMode;       // mono:0, dual:1, stereo:2, joint stereo:3.
    int16_t subBands;     // 4 or 8.
    int16_t channels;     // 1 or 2.
    int16_t numOfBlocks;       // 4, 8, 12 or 16.
    int16_t allocationMethod;  // loudness:0, snr:1.
    int16_t bitPool;           // bitPool
    uint16_t bitRate;          // exp: 328.
};

struct A2dpSbcEncoderCb {
    uint16_t mtuSize;
    bool isPeerEdr;          // Whether peer device supports EDR.
    bool peerSupports3mbps;  // Whether peer device supports 3mbps EDR.
    uint16_t peerMtu;        // A2DP peer mtu.
    uint32_t timestamp;      // A2DP frames timestamp.
    SBCEncoderParams sbcEncoderParams;
    A2dpSBCFeedingParams feedingParams;
    A2dpSbcFeedingState feedingState;
    uint8_t pcmBuffer[A2DP_SBC_MAX_PACKET_SIZE * FRAME_THREE];
    uint8_t pcmRemain[A2DP_SBC_MAX_PACKET_SIZE];
    uint16_t offsetPCM;
    uint16_t sendDataSize;
};

class A2dpSbcEncoder : public A2dpEncoder {
public:
    A2dpSbcEncoder(const A2dpEncoderInitPeerParams *peerParams, A2dpCodecConfig *config);
    ~A2dpSbcEncoder() override;
    void ResetFeedingState(void) override;
    void SendFrames(uint64_t timeStampUs) override;
    void UpdateEncoderParam() override;
    void GetRenderPosition(uint16_t &sendDataSize, uint32_t &timeStamp) override;

private:
    sbc::IEncoderBase* sbcEncoder_ = nullptr;
    std::unique_ptr<A2dpSBCDynamicLibCtrl> codecLib_ = nullptr;
    CODECSbcLib *codecSbcEncoderLib_ = nullptr;
    void updateParam(void);
    bool A2dpSbcReadFeeding(uint32_t *bytesRead);
    void A2dpSbcCalculateEncBitPool(uint16_t samplingFreq, uint16_t minBitPool, uint16_t maxBitPool);
    void A2dpSbcEncodeFrames(void);
    void CalculateSbcPCMRemain(uint16_t codecSize, uint32_t bytesNum, uint8_t *numOfFrame);
    void UpdateMtuSize(void);
    static uint16_t A2dpSbcGetSampleRate(const uint8_t *codecInfo);
    uint16_t A2dpSbcGetBitsPerSample() const;
    static uint8_t A2dpSbcGetTrackChannelCount(const uint8_t *codecInfo);
    static int16_t A2dpSbcGetTrackChannelMode(const uint8_t *codecInfo);
    static int16_t A2dpSbcGetNumberOfSubbands(const uint8_t *codecInfo);
    static int16_t A2dpSbcGetNumberOfBlocks(const uint8_t *codecInfo);
    static int16_t A2dpSbcGetAllocationMethodCode(const uint8_t *codecInfo);
    void A2dpSbcUpdateEncodeFeedingParams(const uint8_t *codecInfo);
    void A2dpSbcUpdateEncoderParams(SBCEncoderParams *encParams, const uint8_t *codecInfo) const;
    void SetSBCParam(void);
    void ConvertFreqParamToSBCParam(void);
    void ConvertModeParamToSBCParam(void);
    void ConvertSubbandsParamToSBCParam(void);
    void ConvertBlockParamToSBCParam(void);
    void ConvertAllocationParamToSBCParam(void);
    void ConvertBitpoolParamToSBCParam(void);
    void EnqueuePacket(Packet *pkt,
        size_t frames, const uint32_t bytes, uint32_t timeStamp, const uint16_t frameSize) const;
    void EnqueuePacketFragment(Packet *pkt,
        size_t frames, const uint32_t bytes, uint32_t timeStamp, const uint16_t frameSize) const;
    A2dpSbcEncoderCb a2dpSbcEncoderCb_ {};
    BT_DISALLOW_COPY_AND_ASSIGN(A2dpSbcEncoder);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // A2DP_ENCODER_SBC_H
