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

#include "../include/a2dp_encoder_sbc.h"
#include <cmath>
#include <cstring>
#include <mutex>
#include "../../include/a2dp_sbc_param_ctrl.h"
#include "log.h"
#include "packet.h"
#include "securec.h"
#include "../../../a2dp_service.h"

namespace OHOS {
namespace bluetooth {
const int BIT_SBC_NUMBER_PER_SAMPLE = 8;
const int MS_TO_US = 1000;
const int NUMBER32 = 32;
const int BIT8 = 8;
const int SUBBAND8 = 8;
const int SUBBAND4 = 4;
const int CHANNEL_ONE = 1;
const int CHANNEL_TWO = 2;
const int PROTECT_ONE = 1;
const int PROTECT_TWO = 2;
const int PROTECT_THREE = 3;
const int BIT_NUMBER5 = 5;
const int PCM_REMAIN_BUFFER_SIZE = 1024;
const int BIT_POOL_SIXTEEN = 16;
const int FRAGMENT_SIZE_TWO = 2;
const int FRAGMENT_SIZE_THREE = 3;
const int VALUE_TWO = 2;

sbc::CodecParam g_sbcEncode = {};


std::recursive_mutex g_sbcMutex {};
A2dpSbcEncoder::A2dpSbcEncoder(
    const A2dpEncoderInitPeerParams *peerParams, A2dpCodecConfig *config) : A2dpEncoder(config)
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    a2dpSbcEncoderCb_.isPeerEdr = peerParams->isPeerEdr;
    a2dpSbcEncoderCb_.peerSupports3mbps = peerParams->peerSupports3mbps;
    a2dpSbcEncoderCb_.peerMtu = peerParams->peermtu;
    a2dpSbcEncoderCb_.timestamp = 0;
    a2dpSbcEncoderCb_.sendDataSize = 0;
    codecLib_ = std::make_unique<A2dpSBCDynamicLibCtrl>(true);
    codecSbcEncoderLib_ = codecLib_->LoadCodecSbcLib();
    if (codecSbcEncoderLib_ == nullptr) {
        LOG_ERROR("[SbcDecoder] %{public}s load sbc lib failed\n", __func__);
    }
    updateParam();
}

A2dpSbcEncoder::~A2dpSbcEncoder()
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    if (codecSbcEncoderLib_ != nullptr) {
        codecSbcEncoderLib_->sbcEncoder.destroySbcEncode(sbcEncoder_);
        codecLib_->UnloadCodecSbcLib(codecSbcEncoderLib_);
        codecSbcEncoderLib_ = nullptr;
    }
}

void A2dpSbcEncoder::ResetFeedingState(void)
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_sbcMutex);
    (void)memset_s(&a2dpSbcEncoderCb_.feedingState, sizeof(A2dpSbcFeedingState), 0, sizeof(A2dpSbcFeedingState));
    a2dpSbcEncoderCb_.feedingState.bytesPerTick =
        a2dpSbcEncoderCb_.feedingParams.sampleRate * a2dpSbcEncoderCb_.feedingParams.bitsPerSample /
        BIT_SBC_NUMBER_PER_SAMPLE * a2dpSbcEncoderCb_.feedingParams.channelCount * A2DP_SBC_ENCODER_INTERVAL_MS /
        MS_TO_US;
    LOG_INFO("[SbcEncoder] %{public}s, sampleRate %{public}u, bitsPerSample %{public}u channelCount %{public}u\n",
        __func__, a2dpSbcEncoderCb_.feedingParams.sampleRate, a2dpSbcEncoderCb_.feedingParams.bitsPerSample,
        a2dpSbcEncoderCb_.feedingParams.channelCount);

    a2dpSbcEncoderCb_.timestamp = 0;
    a2dpSbcEncoderCb_.sendDataSize = 0;
}

void A2dpSbcEncoder::GetRenderPosition(uint16_t &sendDataSize, uint32_t &timeStamp)
{
    std::lock_guard<std::recursive_mutex> lock(g_sbcMutex);
    sendDataSize = a2dpSbcEncoderCb_.sendDataSize;
    timeStamp = a2dpSbcEncoderCb_.timestamp;
    LOG_INFO("[A2dpSbcEncoder] %{public}s sendDataSize = %{public}hu, timeStamp = %{public}u\n",
        __func__, sendDataSize, timeStamp);
}

void A2dpSbcEncoder::SendFrames(uint64_t timeStampUs)
{
    LOG_INFO("[A2dpSbcEncoder] %{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_sbcMutex);
    A2dpSbcEncodeFrames();
}

uint16_t A2dpSbcEncoder::A2dpSbcGetSampleRate(const uint8_t *codecInfo)
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    switch (GetSampleRateSbc(codecInfo)) {
        case A2DP_SBC_SAMPLE_RATE_16000:
            return SBC_SAMPLE_RATE_16000;
        case A2DP_SBC_SAMPLE_RATE_32000:
            return SBC_SAMPLE_RATE_32000;
        case A2DP_SBC_SAMPLE_RATE_44100:
            return SBC_SAMPLE_RATE_44100;
        case A2DP_SBC_SAMPLE_RATE_48000:
            return SBC_SAMPLE_RATE_48000;
        default:
            LOG_INFO("Use default sampleRate: 48000");
            return SBC_SAMPLE_RATE_48000;
    }
}

uint16_t A2dpSbcEncoder::A2dpSbcGetBitsPerSample() const
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);

    switch (config_->GetAudioBitsPerSample()) {
        case A2DP_SAMPLE_BITS_16:
            return SBC_SAMPLE_BITS_16;
        case A2DP_SAMPLE_BITS_24:
            return SBC_SAMPLE_BITS_24;
        case A2DP_SAMPLE_BITS_32:
            return SBC_SAMPLE_BITS_32;
        default:
            LOG_INFO("Use default bitsPerSample: 16");
            return SBC_SAMPLE_BITS_16;
    }
}

uint8_t A2dpSbcEncoder::A2dpSbcGetTrackChannelCount(const uint8_t *codecInfo)
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    switch (GetTrackChannelCountSbc(codecInfo)) {
        case A2DP_SBC_CHANNEL_MODE_MONO:
            return SBC_SINGLE_CHANNEL_COUNT;
        case A2DP_SBC_CHANNEL_MODE_DUAL:
        case A2DP_SBC_CHANNEL_MODE_STEREO:
        case A2DP_SBC_CHANNEL_MODE_JOINT_STEREO:
            return SBC_DOUBLE_CHANNEL_COUNT;
        default:
            LOG_INFO("Failed extract a2dpSbcEncoderCb_.feedingParams.channelCount from config");
            LOG_INFO("Use default ChannelCount: 1");
            return SBC_SINGLE_CHANNEL_COUNT;
    }
}

int16_t A2dpSbcEncoder::A2dpSbcGetTrackChannelMode(const uint8_t *codecInfo)
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    switch (GetTrackChannelModeSbc(codecInfo)) {
        case A2DP_SBC_CHANNEL_MODE_MONO:
            return SBC_MONO;
        case A2DP_SBC_CHANNEL_MODE_DUAL:
            return SBC_DUAL;
        case A2DP_SBC_CHANNEL_MODE_STEREO:
            return SBC_STEREO;
        case A2DP_SBC_CHANNEL_MODE_JOINT_STEREO:
            return SBC_JOINT_STEREO;
        default:
            LOG_INFO("Use default channelMode: JOINT_STEREO");
            return SBC_JOINT_STEREO;
    }
}

int16_t A2dpSbcEncoder::A2dpSbcGetNumberOfSubbands(const uint8_t *codecInfo)
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    switch (GetNumberOfSubbandsSbc(codecInfo)) {
        case A2DP_SBC_SUBBAND_4:
            return SBC_SUBBAND_4;
        case A2DP_SBC_SUBBAND_8:
            return SBC_SUBBAND_8;
        default:
            LOG_INFO("Use default numOfSubBands: 8");
            return SBC_SUBBAND_8;
    }
}

int16_t A2dpSbcEncoder::A2dpSbcGetNumberOfBlocks(const uint8_t *codecInfo)
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    switch (GetNumberOfBlocksSbc(codecInfo)) {
        case A2DP_SBC_BLOCKS_4:
            return SBC_BLOCKS_4;
        case A2DP_SBC_BLOCKS_8:
            return SBC_BLOCKS_8;
        case A2DP_SBC_BLOCKS_12:
            return SBC_BLOCKS_12;
        case A2DP_SBC_BLOCKS_16:
            return SBC_BLOCKS_16;
        default:
            LOG_INFO("Use default numOfBlocks: 16");
            return SBC_BLOCKS_16;
    }
}

int16_t A2dpSbcEncoder::A2dpSbcGetAllocationMethodCode(const uint8_t *codecInfo)
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    switch (GetAllocationMethodCodeSbc(codecInfo)) {
        case A2DP_SBC_ALLOC_MODE_S:
            return SBC_SNR;
        case A2DP_SBC_ALLOC_MODE_L:
            return SBC_LOUDNESS;
        default:
            LOG_INFO("Use default allocationMethod: LOUDNESS");
            return SBC_LOUDNESS;
    }
}

void A2dpSbcEncoder::A2dpSbcUpdateEncodeFeedingParams(const uint8_t *codecInfo)
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    a2dpSbcEncoderCb_.feedingParams.sampleRate = A2dpSbcGetSampleRate(codecInfo);
    a2dpSbcEncoderCb_.feedingParams.bitsPerSample = A2dpSbcGetBitsPerSample();
    a2dpSbcEncoderCb_.feedingParams.channelCount = A2dpSbcGetTrackChannelCount(codecInfo);

    LOG_INFO("[SbcEncoder] %{public}s [sampleRate:%u] [bitsPerSample:%u] [channelMode:%u]",
        __func__,
        a2dpSbcEncoderCb_.feedingParams.sampleRate,
        a2dpSbcEncoderCb_.feedingParams.bitsPerSample,
        a2dpSbcEncoderCb_.feedingParams.channelCount);
}

void A2dpSbcEncoder::A2dpSbcUpdateEncoderParams(SBCEncoderParams *encParams, const uint8_t *codecInfo) const
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    encParams->channelMode = A2dpSbcGetTrackChannelMode(codecInfo);
    encParams->subBands = A2dpSbcGetNumberOfSubbands(codecInfo);
    encParams->numOfBlocks = A2dpSbcGetNumberOfBlocks(codecInfo);
    encParams->allocationMethod = A2dpSbcGetAllocationMethodCode(codecInfo);
    encParams->samplingFreq = A2dpSbcGetSampleRate(codecInfo);
    encParams->channels = A2dpSbcGetTrackChannelCount(codecInfo);

    LOG_INFO("[SbcEncoder] %{public}s [cMode:%u] [sub:%u] [blk:%u] [allc::%u] [freq:%u] [ch:%u]",
        __func__,
        encParams->channelMode,
        encParams->subBands,
        encParams->numOfBlocks,
        encParams->allocationMethod,
        encParams->samplingFreq,
        encParams->channels);
}

void A2dpSbcEncoder::SetSBCParam()
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    ConvertFreqParamToSBCParam();
    ConvertModeParamToSBCParam();
    ConvertSubbandsParamToSBCParam();
    ConvertBlockParamToSBCParam();
    ConvertAllocationParamToSBCParam();
    ConvertBitpoolParamToSBCParam();
    g_sbcEncode.endian = sbc::SBC_ENDIANESS_LE;
    sbcEncoder_ = codecSbcEncoderLib_->sbcEncoder.createSbcEncode();
    LOG_INFO("[SbcEncoder] %{public}s[freq:%u][mode:%u][sub:%u][block:%u][alc:%u][bitpool:%u]\n",
        __func__,
        g_sbcEncode.frequency,
        g_sbcEncode.channelMode,
        g_sbcEncode.subbands,
        g_sbcEncode.blocks,
        g_sbcEncode.allocation,
        g_sbcEncode.bitpool);
}

void A2dpSbcEncoder::updateParam(void)
{
    UpdateEncoderParam();
}

void A2dpSbcEncoder::UpdateEncoderParam(void)
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    SBCEncoderParams *encParams = &a2dpSbcEncoderCb_.sbcEncoderParams;
    uint16_t samplingFreq = 48000;
    uint8_t codecInfo[A2DP_CODEC_SIZE];
    uint8_t *codecConfigInfo = &codecInfo[0];
    if (!config_->CopyOutOtaCodecConfig(codecConfigInfo)) {
        LOG_ERROR("Failed to read A2dpCodecConfig!");
        return;
    }
    const uint8_t *codecCfgInfo = codecConfigInfo;
    int minBitPool = GetMinBitpoolSbc(codecCfgInfo);
    int maxBitPool = GetMaxBitpoolSbc(codecCfgInfo);

    A2dpSbcUpdateEncodeFeedingParams(codecCfgInfo);

    ResetFeedingState();

    A2dpSbcUpdateEncoderParams(encParams, codecCfgInfo);

    const uint16_t mtu = A2DP_SBC_MAX_PACKET_SIZE - A2DP_SBC_PACKET_HEAD_SIZE - A2DP_SBC_MEDIA_PAYLOAD_HEAD_SIZE;
    if (mtu < a2dpSbcEncoderCb_.peerMtu) {
        a2dpSbcEncoderCb_.mtuSize = mtu;
    } else {
        a2dpSbcEncoderCb_.mtuSize = a2dpSbcEncoderCb_.peerMtu;
    }

    samplingFreq = encParams->samplingFreq;
    encParams->bitRate = A2DP_SBC_DEFAULT_BITRATE;

    if (!a2dpSbcEncoderCb_.isPeerEdr) {
        encParams->bitRate = A2DP_SBC_NON_EDR_MAX_BITRATE;
    }

    A2dpSbcCalculateEncBitPool(samplingFreq, minBitPool, maxBitPool);
    SetSBCParam();
    UpdateMtuSize();
}

bool A2dpSbcEncoder::A2dpSbcReadFeeding(uint32_t *bytesRead)
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);

    uint32_t expectedReadPcmData = a2dpSbcEncoderCb_.feedingState.bytesPerTick;

    if (expectedReadPcmData + a2dpSbcEncoderCb_.offsetPCM > FRAME_THREE * A2DP_SBC_MAX_PACKET_SIZE) {
        LOG_ERROR("[Feeding] buffer no space");
        return false;
    }
    A2dpProfile *profile = GetProfileInstance(A2DP_ROLE_SOURCE);
    uint32_t actualReadPcmData = profile->GetPcmData(&a2dpSbcEncoderCb_.pcmBuffer[a2dpSbcEncoderCb_.offsetPCM],
        expectedReadPcmData);
    LOG_INFO("[ReadA2dpSharedBuffer][expectedReadPcmData:%u][actualReadPcmData:%u]",
        expectedReadPcmData, actualReadPcmData);

    *bytesRead = actualReadPcmData;

    if (actualReadPcmData) {
        LOG_INFO("[Feeding][offsetPCM:%u][readBytes:%u]", a2dpSbcEncoderCb_.offsetPCM, actualReadPcmData);
        return true;
    } else {
        LOG_ERROR("[Feeding][read no data][readBytes:%u]", actualReadPcmData);
        return false;
    }
}

void A2dpSbcEncoder::ConvertFreqParamToSBCParam(void)
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    SBCEncoderParams *encParams = &a2dpSbcEncoderCb_.sbcEncoderParams;
    switch (encParams->samplingFreq) {
        case SBC_SAMPLE_RATE_16000:
            g_sbcEncode.frequency = sbc::SBC_FREQ_16000;
            break;
        case SBC_SAMPLE_RATE_32000:
            g_sbcEncode.frequency = sbc::SBC_FREQ_32000;
            break;
        case SBC_SAMPLE_RATE_44100:
            g_sbcEncode.frequency = sbc::SBC_FREQ_44100;
            break;
        case SBC_SAMPLE_RATE_48000:
            g_sbcEncode.frequency = sbc::SBC_FREQ_48000;
            break;
        default:
            g_sbcEncode.frequency = sbc::SBC_FREQ_44100;
            break;
    }
}

void A2dpSbcEncoder::ConvertModeParamToSBCParam(void)
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    SBCEncoderParams *encParams = &a2dpSbcEncoderCb_.sbcEncoderParams;
    switch (encParams->channelMode) {
        case SBC_MONO:
            g_sbcEncode.channelMode = sbc::SBC_CHANNEL_MODE_MONO;
            break;
        case SBC_DUAL:
            g_sbcEncode.channelMode = sbc::SBC_CHANNEL_MODE_DUAL_CHANNEL;
            break;
        case SBC_STEREO:
            g_sbcEncode.channelMode = sbc::SBC_CHANNEL_MODE_STEREO;
            break;
        case SBC_JOINT_STEREO:
            g_sbcEncode.channelMode = sbc::SBC_CHANNEL_MODE_JOINT_STEREO;
            break;
        default:
            g_sbcEncode.channelMode = sbc::SBC_CHANNEL_MODE_STEREO;
            break;
    }
}

void A2dpSbcEncoder::ConvertSubbandsParamToSBCParam(void)
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    SBCEncoderParams *encParams = &a2dpSbcEncoderCb_.sbcEncoderParams;
    switch (encParams->subBands) {
        case SBC_SUBBAND_4:
            g_sbcEncode.subbands = sbc::SBC_SUBBAND4;
            break;
        case SBC_SUBBAND_8:
            g_sbcEncode.subbands = sbc::SBC_SUBBAND8;
            break;
        default:
            g_sbcEncode.subbands = sbc::SBC_SUBBAND8;
            break;
    }
}

void A2dpSbcEncoder::ConvertBlockParamToSBCParam(void)
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    SBCEncoderParams *encParams = &a2dpSbcEncoderCb_.sbcEncoderParams;
    switch (encParams->numOfBlocks) {
        case SBC_BLOCKS_4:
            g_sbcEncode.blocks = sbc::SBC_BLOCK4;
            break;
        case SBC_BLOCKS_8:
            g_sbcEncode.blocks = sbc::SBC_BLOCK8;
            break;
        case SBC_BLOCKS_12:
            g_sbcEncode.blocks = sbc::SBC_BLOCK12;
            break;
        case SBC_BLOCKS_16:
            g_sbcEncode.blocks = sbc::SBC_BLOCK16;
            break;
        default:
            g_sbcEncode.blocks = sbc::SBC_BLOCK16;
            break;
    }
}

void A2dpSbcEncoder::ConvertAllocationParamToSBCParam(void)
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    SBCEncoderParams *encParams = &a2dpSbcEncoderCb_.sbcEncoderParams;
    switch (encParams->allocationMethod) {
        case SBC_LOUDNESS:
            g_sbcEncode.allocation = sbc::SBC_ALLOCATION_LOUDNESS;
            break;
        case SBC_SNR:
            g_sbcEncode.allocation = sbc::SBC_ALLOCATION_SNR;
            break;
        default:
            g_sbcEncode.allocation = sbc::SBC_ALLOCATION_LOUDNESS;
            break;
    }
}

void A2dpSbcEncoder::ConvertBitpoolParamToSBCParam(void)
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    SBCEncoderParams *encParams = &a2dpSbcEncoderCb_.sbcEncoderParams;
    g_sbcEncode.bitpool = encParams->bitPool;
}

void A2dpSbcEncoder::CalculateSbcPCMRemain(uint16_t codecSize, uint32_t bytesNum, uint8_t *numOfFrame)
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    if (codecSize != 0) {
        LOG_INFO("[offset:%u][bytes:%u][codecSize:%u]", a2dpSbcEncoderCb_.offsetPCM, bytesNum, codecSize);
        *numOfFrame = (a2dpSbcEncoderCb_.offsetPCM + bytesNum) / codecSize;
        a2dpSbcEncoderCb_.offsetPCM = (a2dpSbcEncoderCb_.offsetPCM + bytesNum) % codecSize;
        uint16_t encodePCMSize = *numOfFrame * codecSize;
        LOG_INFO("[numOfFrame:%u][offset:%u][bytes:%u]", *numOfFrame, a2dpSbcEncoderCb_.offsetPCM, bytesNum);
        if (a2dpSbcEncoderCb_.offsetPCM > PCM_REMAIN_BUFFER_SIZE) {
            a2dpSbcEncoderCb_.offsetPCM = PCM_REMAIN_BUFFER_SIZE;
        }
        (void)memcpy_s(a2dpSbcEncoderCb_.pcmRemain, A2DP_SBC_MAX_PACKET_SIZE,
            &a2dpSbcEncoderCb_.pcmBuffer[encodePCMSize], a2dpSbcEncoderCb_.offsetPCM);
    }
}

void A2dpSbcEncoder::A2dpSbcEncodeFrames(void)
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    size_t encoded = 0;
    SBCEncoderParams *encParams = &a2dpSbcEncoderCb_.sbcEncoderParams;
    uint16_t blocksXsubbands = encParams->subBands * encParams->numOfBlocks;
    uint16_t channelMode = (g_sbcEncode.channelMode == sbc::SBC_CHANNEL_MODE_MONO) ? CHANNEL_ONE : CHANNEL_TWO;
    uint16_t subbands = g_sbcEncode.subbands ? SUBBAND8 : SUBBAND4;
    const uint16_t blocks = SUBBAND4 + (g_sbcEncode.blocks * SUBBAND4);
    uint16_t codecSize = subbands * blocks * channelMode * VALUE_TWO;
    Packet *pkt = PacketMalloc(A2DP_SBC_FRAGMENT_HEADER, 0, 0);
    uint32_t bytesNum = 0;
    uint8_t numOfFrame = 0;
    if (A2dpSbcReadFeeding(&bytesNum)) {
        CalculateSbcPCMRemain(codecSize, bytesNum, &numOfFrame);
        uint8_t frameIter = 0;
        uint16_t pcmOffset = 0;
        while (numOfFrame) {
            uint8_t outputBuf[A2DP_SBC_HQ_DUAL_BP_53_FRAME_SIZE] = {};
            int16_t outputLen = sbcEncoder_->SBCEncode(g_sbcEncode, &a2dpSbcEncoderCb_.pcmBuffer[pcmOffset],
                blocksXsubbands * channelMode, outputBuf, sizeof(outputBuf), &encoded);
            LOG_INFO("[SbcEncoder] %{public}s encoded %{public}zu, pcmOffset%{public}u\n",
                __func__, encoded, pcmOffset);
            if (outputLen < 0) {
                LOG_ERROR("err occur.");
            }
            pcmOffset += blocksXsubbands * channelMode * CHANNEL_TWO;
            Buffer *encBuf = BufferMalloc(encoded);
            if (memcpy_s(BufferPtr(encBuf), encoded, outputBuf, encoded) != EOK) {
                BufferFree(encBuf);
                PacketFree(pkt);
                return;
            }
            PacketPayloadAddLast(pkt, encBuf);
            BufferFree(encBuf);
            numOfFrame--;
            frameIter++;
        }
        uint16_t encodePacketSize = PacketSize(pkt);
        (void)memset_s(a2dpSbcEncoderCb_.pcmBuffer, A2DP_SBC_MAX_PACKET_SIZE * FRAME_THREE,
            0, sizeof(a2dpSbcEncoderCb_.pcmBuffer));
        if (memcpy_s(a2dpSbcEncoderCb_.pcmBuffer, A2DP_SBC_MAX_PACKET_SIZE * FRAME_THREE,
            a2dpSbcEncoderCb_.pcmRemain, a2dpSbcEncoderCb_.offsetPCM) != EOK) {
                PacketFree(pkt);
                return;
            }
        (void)memset_s(a2dpSbcEncoderCb_.pcmRemain, A2DP_SBC_MAX_PACKET_SIZE, 0, sizeof(a2dpSbcEncoderCb_.pcmRemain));
        if (encodePacketSize > 0) {
            uint32_t pktTimeStamp = a2dpSbcEncoderCb_.timestamp;
            a2dpSbcEncoderCb_.timestamp += frameIter * blocksXsubbands;
            a2dpSbcEncoderCb_.sendDataSize += codecSize * frameIter;
            EnqueuePacket(pkt, frameIter, encodePacketSize, pktTimeStamp, (uint16_t)encoded);  // Enqueue Packet.
            LOG_INFO("[SbcEncoder] %{public}s timestamp %{public}u, sendDataSize%{public}u\n",
                __func__, a2dpSbcEncoderCb_.timestamp, a2dpSbcEncoderCb_.sendDataSize);
        }
    }
    PacketFree(pkt);
}

void A2dpSbcEncoder::EnqueuePacket(
    Packet *pkt, size_t frames, const uint32_t bytes, uint32_t timeStamp, const uint16_t frameSize) const
{
    LOG_INFO("[EnqueuePacket][frameSize:%hu][FrameNum:%zu], mtu[%hu], totalSize[%u]",
        frameSize, frames, a2dpSbcEncoderCb_.mtuSize, PacketSize(pkt));
    if (PacketSize(pkt) < static_cast<uint32_t>(a2dpSbcEncoderCb_.mtuSize)) {
        Buffer *header = PacketHead(pkt);
        uint8_t *p = static_cast<uint8_t*>(BufferPtr(header));
        *p = frames;
        A2dpProfile *profile = GetProfileInstance(A2DP_ROLE_SOURCE);
        profile->EnqueuePacket(pkt, frames, bytes, timeStamp);  // Enqueue Packet.
    } else {
        EnqueuePacketFragment(pkt, frames, bytes, timeStamp, frameSize);
    }
}

void A2dpSbcEncoder::EnqueuePacketFragment(
    Packet *pkt, size_t frames, const uint32_t bytes, uint32_t timeStamp, const uint16_t frameSize) const
{
    LOG_INFO("[SbcEncoder] %{public}s\n", __func__);
    uint8_t count = 1;
    uint32_t pktLen = 0;
    uint8_t frameNum = 0;
    uint8_t sentFrameNum = 0;
    uint16_t blocksXsubbands
        = a2dpSbcEncoderCb_.sbcEncoderParams.subBands * a2dpSbcEncoderCb_.sbcEncoderParams.numOfBlocks;
    bool frameFragmented = false;
    if ((static_cast<uint16_t>(frames) / FRAGMENT_SIZE_TWO  + 1) * frameSize  < (a2dpSbcEncoderCb_.mtuSize)) {
        count = FRAGMENT_SIZE_TWO;
        pktLen = (frames / count) * frameSize;
    } else {
        count = FRAGMENT_SIZE_THREE;
        pktLen = (frames / count) * frameSize;
        if (pktLen > static_cast<uint32_t>(a2dpSbcEncoderCb_.mtuSize) ||
            (frames - frames / count * FRAGMENT_SIZE_TWO) * frameSize  >
            static_cast<uint32_t>(a2dpSbcEncoderCb_.mtuSize)) {
            frameFragmented = true;
            LOG_ERROR("mtu size isn't enough.[frameSize:%u] mtu[%u]", pktLen, a2dpSbcEncoderCb_.mtuSize);
        }
    }
    frameNum = frames / count;
    if (!frameFragmented) {
        size_t remainFrames = frames;
        uint16_t offset = 0;
        do {
            Packet *mediaPacket = PacketMalloc(A2DP_SBC_FRAGMENT_HEADER, 0, 0);
            if (count == 1) {
                frameNum = remainFrames;
                pktLen = frameNum * frameSize;
            }
            count--;
            LOG_INFO("[EnqueuePacket] [pktLen:%u] [sFrameNum:%u] [remain:%u]", pktLen, frameNum, PacketSize(pkt));
            Buffer *header = PacketHead(mediaPacket);
            uint8_t *p = static_cast<uint8_t*>(BufferPtr(header));
            *p = frameNum;
            uint8_t bufferFra[1024];
            size_t encoded = pktLen;
            PacketPayloadRead(pkt, bufferFra, offset, pktLen);
            offset += pktLen;
            Buffer *encBuf = BufferMalloc(encoded);
            if (memcpy_s(BufferPtr(encBuf), encoded, bufferFra, encoded) != EOK) {
                LOG_ERROR("[EnqueuePacket] memcpy_s fail");
                return;
            }
            PacketPayloadAddLast(mediaPacket, encBuf);
            BufferFree(encBuf);
            remainFrames -= frameNum;

            uint16_t encodePacketSize = PacketSize(mediaPacket);
            LOG_ERROR("[EnqueuePacket] encodePacketSize is ");
            A2dpProfile *profile = GetProfileInstance(A2DP_ROLE_SOURCE);
            profile->EnqueuePacket(mediaPacket, frameNum, encodePacketSize, timeStamp + sentFrameNum * blocksXsubbands);
            // Enqueue Packet.
            sentFrameNum += frameNum;
            PacketFree(mediaPacket);
        } while (count > 0);
    }
}

void A2dpSbcEncoder::A2dpSbcCalculateEncBitPool(uint16_t samplingFreq, uint16_t minBitPool, uint16_t maxBitPool)
{
    SBCEncoderParams *encParams = &a2dpSbcEncoderCb_.sbcEncoderParams;
    uint8_t protect = 0;
    int16_t bitPool = 0;
    int16_t bitRate = 0;
    if ((encParams == nullptr) || (samplingFreq == 0) || (encParams->subBands == 0) || (encParams->channels == 0)) {
        return;
    }
    do {  // Dynamic bitpool support.
        if ((encParams->channelMode == SBC_JOINT_STEREO) || (encParams->channelMode == SBC_STEREO)) {
            bitPool = static_cast<int16_t>((encParams->bitRate * encParams->subBands * MS_TO_US / samplingFreq) -
                                           ((NUMBER32 + (SUBBAND4 * encParams->subBands * encParams->channels) +
                                           ((encParams->channelMode - CHANNEL_TWO) * encParams->subBands)) /
                                           encParams->numOfBlocks));
            int16_t frameLen =
                SUBBAND4 + (SUBBAND4 * encParams->subBands * encParams->channels) / BIT8 +
                (((encParams->channelMode - CHANNEL_TWO) * encParams->subBands) + (encParams->numOfBlocks * bitPool)) /
                BIT8;
            bitRate = (BIT8 * frameLen * samplingFreq) / (encParams->subBands * encParams->numOfBlocks * MS_TO_US);
            if (bitRate > encParams->bitRate) {
                bitPool--;
            }
            encParams->bitPool =
                (bitPool > (NUMBER32 * encParams->subBands)) ? (NUMBER32 * encParams->subBands) : bitPool;
        } else {  // Mono or Dual Channel.
            bitPool = static_cast<int16_t>(
                ((encParams->subBands * encParams->bitRate * MS_TO_US) / (samplingFreq * encParams->channels)) -
                (((NUMBER32 / encParams->channels) + (SUBBAND4 * encParams->subBands)) / encParams->numOfBlocks));
            encParams->bitPool = (bitPool > (BIT_POOL_SIXTEEN * encParams->subBands)) ?
                (BIT_POOL_SIXTEEN * encParams->subBands) : bitPool;
        }
        if (bitPool > maxBitPool) {
            encParams->bitRate -= BIT_NUMBER5;
            protect |= PROTECT_ONE;
        } else if (bitPool < minBitPool) {
            uint16_t preBitRate = encParams->bitRate;
            encParams->bitRate += BIT_NUMBER5;
            protect |= PROTECT_TWO;
            if (encParams->bitRate < preBitRate) {
                protect |= PROTECT_THREE;
            }
        } else {
            break;
        }
        if (protect == PROTECT_THREE) {
            break;
        }
    } while (true);
    encParams->bitPool = bitPool;
}

void A2dpSbcEncoder::UpdateMtuSize(void)
{
    LOG_INFO("[SBCEncoder] %{public}s", __func__);
    uint16_t mtu = a2dpSbcEncoderCb_.mtuSize;

    if (a2dpSbcEncoderCb_.isPeerEdr) {
        if (!a2dpSbcEncoderCb_.peerSupports3mbps) {
            if (mtu > MAX_2MBPS_SBC_PAYLOAD_MTU) {  // MTU is only payload.
                mtu = MAX_2MBPS_SBC_PAYLOAD_MTU;
                a2dpSbcEncoderCb_.mtuSize = mtu;
            }
        } else {                                    // EDR 3mbps.
            if (mtu > MAX_3MBPS_SBC_PAYLOAD_MTU) {  // MTU is only payload.
                mtu = MAX_3MBPS_SBC_PAYLOAD_MTU;
                a2dpSbcEncoderCb_.mtuSize = mtu;
            }
        }
    }
    LOG_INFO("[SBCEncoder] %{public}s [mtuSize:%u]", __func__, a2dpSbcEncoderCb_.mtuSize);
}
}  // namespace bluetooth
}  // namespace OHOS