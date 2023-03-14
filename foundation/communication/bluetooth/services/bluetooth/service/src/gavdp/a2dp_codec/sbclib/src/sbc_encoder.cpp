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
#include "../include/sbc_encoder.h"
#include <memory>
#include "../include/sbc_constant.h"
#include "../include/sbc_math.h"
#include "../include/sbc_tables.h"
#include "foundation/communication/bluetooth/services/bluetooth/service/src/gavdp/a2dp_codec/sbclib/include/sbc_frame.h"
#include "new"
#include "securec.h"
#include "sys/types.h"

namespace sbc {
const int SUBBAND_4 = 4;
const int SUBBAND_8 = 8;

const int CHANNEL_1 = 1;
const int CHANNEL_2 = 2;

const int NOT_JOINT_STEREO = 0;
const int JOINT_STEREO = 1;

const int INCREMENT_VALUE = 4;
const int SCALE_OUT_BITS = 15;

const int VALUE_0 = 0;
const int VALUE_1 = 1;
const int VALUE_2 = 2;
const int VALUE_3 = 3;
const int VALUE_4 = 4;
const int VALUE_5 = 5;
const int VALUE_6 = 6;
const uint VALUE_7 = 7;
const int VALUE_8 = 8;
const int VALUE_9 = 9;
const int VALUE_10 = 10;
const int VALUE_11 = 11;
const int VALUE_12 = 12;
const int VALUE_13 = 13;
const int VALUE_14 = 14;
const int VALUE_15 = 15;
const int VALUE_16 = 16;
const int VALUE_24 = 24;
const int VALUE_31 = 31;
const int VALUE_32 = 32;
const int VALUE_36 = 36;
const int VALUE_40 = 40;
const int VALUE_72 = 72;
const int CHANNEL_NUM = 2;
const int BLOCK_NUM = 16;
const int SUBBAND_NUM = 8;


const int COEFFICIENT_40 = 40;
const int COEFFICIENT_80 = 80;

Encoder::Encoder()
{
    initialized_ = false;
}

Encoder::~Encoder()
{}

extern "C" Encoder* CreateEncode()
{
    std::unique_ptr<Encoder> encoder = std::make_unique<Encoder>();
    return encoder.release();
}

extern "C" void DestroyEncode(Encoder *p)
{
    delete p;
}

void Encoder::Init(const Frame &frame)
{
    (void)memset_s(x_, sizeof(x_), VALUE_0, sizeof(x_));
    increment_ = INCREMENT_VALUE;
    position_ = (BUFFER_SIZE - frame.subbands_ * VALUE_9) & ~VALUE_7;
}

size_t Encoder::CalculateFrameLength(const CodecParam &codecParam)
{
    uint8_t subbands = codecParam.subbands ? SUBBAND_8: SUBBAND_4;
    uint8_t blocks = (codecParam.blocks * VALUE_4) + VALUE_4;
    uint8_t joint = (codecParam.channelMode == SBC_CHANNEL_MODE_STEREO) ? JOINT_STEREO : NOT_JOINT_STEREO;
    uint8_t channels = (codecParam.channelMode == SBC_CHANNEL_MODE_MONO) ? CHANNEL_1 : CHANNEL_2;
    uint8_t bitpool = codecParam.bitpool;
    size_t size =  VALUE_4 + (VALUE_4 * subbands * channels) / VALUE_8;

    if (channels == CHANNEL_1 || codecParam.channelMode == SBC_CHANNEL_MODE_DUAL_CHANNEL) {
        size += ((blocks * channels * bitpool) + VALUE_7) / VALUE_8;
    } else {
        size += (((joint ? subbands : VALUE_0) + blocks * bitpool) + VALUE_7) / VALUE_8;
    }
    return size;
}

size_t Encoder::CalculateCodecSize(const CodecParam &codecParam)
{
    uint16_t subbands = codecParam.subbands ? SUBBAND_8 : SUBBAND_4;
    uint16_t channels = (codecParam.channelMode == SBC_CHANNEL_MODE_MONO) ? CHANNEL_1 : CHANNEL_2;
    uint16_t blocks = (codecParam.blocks * VALUE_4) + VALUE_4;

    return subbands * blocks * channels * VALUE_2;
}

void Encoder::UpdateCodecFormat(const CodecParam &codecParam)
{
    if (!initialized_) {
        frame_.frequency_ = codecParam.frequency;
        frame_.channelMode_ = codecParam.channelMode;
        frame_.channels_ = ((codecParam.channelMode == SBC_CHANNEL_MODE_MONO) ? CHANNEL_1 : CHANNEL_2);
        frame_.allocation_ = codecParam.allocation;
        frame_.subbandMode_ = codecParam.subbands;
        frame_.subbands_ = codecParam.subbands ? SUBBAND_8 : SUBBAND_4;
        frame_.blockMode_ = codecParam.blocks;
        frame_.blocks_ = (codecParam.blocks * VALUE_4) + VALUE_4;
        frame_.bitpool_ = codecParam.bitpool;
        frame_.codeSize_ = CalculateCodecSize(codecParam);
        frame_.length_ = CalculateFrameLength(codecParam);
        Init(frame_);
        initialized_ = true;
    } else {
        frame_.length_ = CalculateFrameLength(codecParam);
        frame_.bitpool_ = codecParam.bitpool;
    }
}

ssize_t Encoder::SBCEncode(const CodecParam &codecParam, const uint8_t* in, size_t iLength,
                           uint8_t* out, size_t oLength, size_t* written)
{
    int samples = VALUE_0;
    ssize_t frameLen = VALUE_0;

    if (written) {
        *written = VALUE_0;
    }

    UpdateCodecFormat(codecParam);
    int pcmSamples = frame_.subbands_ * frame_.blocks_;

    if (!out || oLength < frame_.length_) {
        return SBC_ERROR_INVALID_FRAME;
    }

    if (frame_.subbands_ == SUBBAND_8) {
        position_ = Get8SubbandSamplingPoint(in, x_,
                                             &pcmSamples,
                                             frame_.channels_,
                                             codecParam.endian);
        samples = Analyze8Subbands(position_, x_, frame_, increment_);
    } else {
        position_ = Get4SubbandSamplingPoint(in, x_,
                                             pcmSamples,
                                             frame_.channels_,
                                             codecParam.endian);
        samples = Analyze4Subbands(position_, x_, frame_, increment_);
    }

    if (frame_.channelMode_ == SBC_CHANNEL_MODE_JOINT_STEREO) {
        int j = CalculateScalefactorsJoint();
        frameLen = frame_.Pack(out, frame_, j);
    } else {
        CalculateScalefactors(frame_.audioSamples_, frame_.scaleFactor_, frame_.blocks_,
                              frame_.channels_, frame_.subbands_);
        frameLen = frame_.Pack(out, frame_, VALUE_0);
    }

    if (written) {
        *written = frameLen;
    }

    return samples * frame_.channels_ * VALUE_2;
}

void Encoder::AnalyzeFourForPolyphaseFilter(int32_t *temp, const int16_t *inData, const int16_t *consts)
{
    for (int hop = VALUE_0; hop < COEFFICIENT_40; hop += VALUE_8) {
        temp[VALUE_0] += static_cast<int32_t>(inData[hop] * consts[hop]);
        temp[VALUE_0] += static_cast<int32_t>(inData[hop + VALUE_1] * consts[hop + VALUE_1]);
        temp[VALUE_1] += static_cast<int32_t>(inData[hop + VALUE_2] * consts[hop + VALUE_2]);
        temp[VALUE_1] += static_cast<int32_t>(inData[hop + VALUE_3] * consts[hop + VALUE_3]);
        temp[VALUE_2] += static_cast<int32_t>(inData[hop + VALUE_4] * consts[hop + VALUE_4]);
        temp[VALUE_2] += static_cast<int32_t>(inData[hop + VALUE_5] * consts[hop + VALUE_5]);
        temp[VALUE_3] += static_cast<int32_t>(inData[hop + VALUE_6] * consts[hop + VALUE_6]);
        temp[VALUE_3] += static_cast<int32_t>(inData[hop + VALUE_7] * consts[hop + VALUE_7]);
    }
}

void Encoder::AnalyzeFourForScaling(int32_t *temp1, int16_t *temp2)
{
    temp2[VALUE_0] = temp1[VALUE_0] >> PROTO_BAND4_SCALE;
    temp2[VALUE_1] = temp1[VALUE_1] >> PROTO_BAND4_SCALE;
    temp2[VALUE_2] = temp1[VALUE_2] >> PROTO_BAND4_SCALE;
    temp2[VALUE_3] = temp1[VALUE_3] >> PROTO_BAND4_SCALE;
}

void Encoder::AnalyzeFourForCosTransform(int32_t *temp1, int16_t *temp2, const int16_t *consts)
{
    temp1[VALUE_0]  = static_cast<int32_t>(temp2[VALUE_0] * consts[COEFFICIENT_40 + VALUE_0]);
    temp1[VALUE_0] += static_cast<int32_t>(temp2[VALUE_1] * consts[COEFFICIENT_40 + VALUE_1]);
    temp1[VALUE_1]  = static_cast<int32_t>(temp2[VALUE_0] * consts[COEFFICIENT_40 + VALUE_2]);
    temp1[VALUE_1] += static_cast<int32_t>(temp2[VALUE_1] * consts[COEFFICIENT_40 + VALUE_3]);
    temp1[VALUE_2]  = static_cast<int32_t>(temp2[VALUE_0] * consts[COEFFICIENT_40 + VALUE_4]);
    temp1[VALUE_2] += static_cast<int32_t>(temp2[VALUE_1] * consts[COEFFICIENT_40 + VALUE_5]);
    temp1[VALUE_3]  = static_cast<int32_t>(temp2[VALUE_0] * consts[COEFFICIENT_40 + VALUE_6]);
    temp1[VALUE_3] += static_cast<int32_t>(temp2[VALUE_1] * consts[COEFFICIENT_40 + VALUE_7]);

    temp1[VALUE_0] += static_cast<int32_t>(temp2[VALUE_2] * consts[COEFFICIENT_40 + VALUE_8]);
    temp1[VALUE_0] += static_cast<int32_t>(temp2[VALUE_3] * consts[COEFFICIENT_40 + VALUE_9]);
    temp1[VALUE_1] += static_cast<int32_t>(temp2[VALUE_2] * consts[COEFFICIENT_40 + VALUE_10]);
    temp1[VALUE_1] += static_cast<int32_t>(temp2[VALUE_3] * consts[COEFFICIENT_40 + VALUE_11]);
    temp1[VALUE_2] += static_cast<int32_t>(temp2[VALUE_2] * consts[COEFFICIENT_40 + VALUE_12]);
    temp1[VALUE_2] += static_cast<int32_t>(temp2[VALUE_3] * consts[COEFFICIENT_40 + VALUE_13]);
    temp1[VALUE_3] += static_cast<int32_t>(temp2[VALUE_2] * consts[COEFFICIENT_40 + VALUE_14]);
    temp1[VALUE_3] += static_cast<int32_t>(temp2[VALUE_3] * consts[COEFFICIENT_40 + VALUE_15]);
}


void Encoder::AnalyzeFourFunction(const int16_t *inData, int32_t *outData, const int16_t *consts) const
{
    int32_t t1[VALUE_4] = {};
    int16_t t2[VALUE_4] = {};

    t1[VALUE_0] = t1[VALUE_1] = t1[VALUE_2] = t1[VALUE_3] =
        static_cast<int32_t>(VALUE_1 << (PROTO_BAND4_SCALE - VALUE_1));

    AnalyzeFourForPolyphaseFilter(t1, inData, consts);
    AnalyzeFourForScaling(t1, t2);
    AnalyzeFourForCosTransform(t1, t2, consts);

    outData[VALUE_0] = t1[VALUE_0] >> (COS_TABLE_BAND4_SCALE - SCALE_OUT_BITS);
    outData[VALUE_1] = t1[VALUE_1] >> (COS_TABLE_BAND4_SCALE - SCALE_OUT_BITS);
    outData[VALUE_2] = t1[VALUE_2] >> (COS_TABLE_BAND4_SCALE - SCALE_OUT_BITS);
    outData[VALUE_3] = t1[VALUE_3] >> (COS_TABLE_BAND4_SCALE - SCALE_OUT_BITS);
}

void Encoder::AnalyzeEightForPolyphaseFilter(int32_t *temp, const int16_t *inData, const int16_t *consts)
{
    for (int hop = VALUE_0; hop < COEFFICIENT_80; hop += VALUE_16) {
        temp[VALUE_0] += static_cast<int32_t>(inData[hop] * consts[hop]);
        temp[VALUE_0] += static_cast<int32_t>(inData[hop + VALUE_1] * consts[hop + VALUE_1]);
        temp[VALUE_1] += static_cast<int32_t>(inData[hop + VALUE_2] * consts[hop + VALUE_2]);
        temp[VALUE_1] += static_cast<int32_t>(inData[hop + VALUE_3] * consts[hop + VALUE_3]);
        temp[VALUE_2] += static_cast<int32_t>(inData[hop + VALUE_4] * consts[hop + VALUE_4]);
        temp[VALUE_2] += static_cast<int32_t>(inData[hop + VALUE_5] * consts[hop + VALUE_5]);
        temp[VALUE_3] += static_cast<int32_t>(inData[hop + VALUE_6] * consts[hop + VALUE_6]);
        temp[VALUE_3] += static_cast<int32_t>(inData[hop + VALUE_7] * consts[hop + VALUE_7]);
        temp[VALUE_4] += static_cast<int32_t>(inData[hop + VALUE_8] * consts[hop + VALUE_8]);
        temp[VALUE_4] += static_cast<int32_t>(inData[hop + VALUE_9] * consts[hop + VALUE_9]);
        temp[VALUE_5] += static_cast<int32_t>(inData[hop + VALUE_10] * consts[hop + VALUE_10]);
        temp[VALUE_5] += static_cast<int32_t>(inData[hop + VALUE_11] * consts[hop + VALUE_11]);
        temp[VALUE_6] += static_cast<int32_t>(inData[hop + VALUE_12] * consts[hop + VALUE_12]);
        temp[VALUE_6] += static_cast<int32_t>(inData[hop + VALUE_13] * consts[hop + VALUE_13]);
        temp[VALUE_7] += static_cast<int32_t>(inData[hop + VALUE_14] * consts[hop + VALUE_14]);
        temp[VALUE_7] += static_cast<int32_t>(inData[hop + VALUE_15] * consts[hop + VALUE_15]);
    }
}

void Encoder::AnalyzeEightForScaling(int32_t *temp1, int16_t *temp2)
{
    temp2[VALUE_0] = temp1[VALUE_0] >> PROTO_BAND8_SCALE;
    temp2[VALUE_1] = temp1[VALUE_1] >> PROTO_BAND8_SCALE;
    temp2[VALUE_2] = temp1[VALUE_2] >> PROTO_BAND8_SCALE;
    temp2[VALUE_3] = temp1[VALUE_3] >> PROTO_BAND8_SCALE;
    temp2[VALUE_4] = temp1[VALUE_4] >> PROTO_BAND8_SCALE;
    temp2[VALUE_5] = temp1[VALUE_5] >> PROTO_BAND8_SCALE;
    temp2[VALUE_6] = temp1[VALUE_6] >> PROTO_BAND8_SCALE;
    temp2[VALUE_7] = temp1[VALUE_7] >> PROTO_BAND8_SCALE;
}

void Encoder::AnalyzeEightForCosTransform(int32_t *temp1, int16_t *temp2, const int16_t *consts)
{
    temp1[VALUE_0] = temp1[VALUE_1] = temp1[VALUE_2] = temp1[VALUE_3] = temp1[VALUE_4] =
        temp1[VALUE_5] = temp1[VALUE_6] = temp1[VALUE_7] = VALUE_0;
    for (int i = VALUE_0; i < VALUE_4; i++) {
        temp1[VALUE_0] += static_cast<int32_t>(temp2[i * VALUE_2 + VALUE_0] *
                                               consts[COEFFICIENT_80 + i * VALUE_16 + VALUE_0]);
        temp1[VALUE_0] += static_cast<int32_t>(temp2[i * VALUE_2 + VALUE_1] *
                                               consts[COEFFICIENT_80 + i * VALUE_16 + VALUE_1]);
        temp1[VALUE_1] += static_cast<int32_t>(temp2[i * VALUE_2 + VALUE_0] *
                                               consts[COEFFICIENT_80 + i * VALUE_16 + VALUE_2]);
        temp1[VALUE_1] += static_cast<int32_t>(temp2[i * VALUE_2 + VALUE_1] *
                                               consts[COEFFICIENT_80 + i * VALUE_16 + VALUE_3]);
        temp1[VALUE_2] += static_cast<int32_t>(temp2[i * VALUE_2 + VALUE_0] *
                                               consts[COEFFICIENT_80 + i * VALUE_16 + VALUE_4]);
        temp1[VALUE_2] += static_cast<int32_t>(temp2[i * VALUE_2 + VALUE_1] *
                                               consts[COEFFICIENT_80 + i * VALUE_16 + VALUE_5]);
        temp1[VALUE_3] += static_cast<int32_t>(temp2[i * VALUE_2 + VALUE_0] *
                                               consts[COEFFICIENT_80 + i * VALUE_16 + VALUE_6]);
        temp1[VALUE_3] += static_cast<int32_t>(temp2[i * VALUE_2 + VALUE_1] *
                                               consts[COEFFICIENT_80 + i * VALUE_16 + VALUE_7]);
        temp1[VALUE_4] += static_cast<int32_t>(temp2[i * VALUE_2 + VALUE_0] *
                                               consts[COEFFICIENT_80 + i * VALUE_16 + VALUE_8]);
        temp1[VALUE_4] += static_cast<int32_t>(temp2[i * VALUE_2 + VALUE_1] *
                                               consts[COEFFICIENT_80 + i * VALUE_16 + VALUE_9]);
        temp1[VALUE_5] += static_cast<int32_t>(temp2[i * VALUE_2 + VALUE_0] *
                                               consts[COEFFICIENT_80 + i * VALUE_16 + VALUE_10]);
        temp1[VALUE_5] += static_cast<int32_t>(temp2[i * VALUE_2 + VALUE_1] *
                                               consts[COEFFICIENT_80 + i * VALUE_16 + VALUE_11]);
        temp1[VALUE_6] += static_cast<int32_t>(temp2[i * VALUE_2 + VALUE_0] *
                                               consts[COEFFICIENT_80 + i * VALUE_16 + VALUE_12]);
        temp1[VALUE_6] += static_cast<int32_t>(temp2[i * VALUE_2 + VALUE_1] *
                                               consts[COEFFICIENT_80 + i * VALUE_16 + VALUE_13]);
        temp1[VALUE_7] += static_cast<int32_t>(temp2[i * VALUE_2 + VALUE_0] *
                                               consts[COEFFICIENT_80 + i * VALUE_16 + VALUE_14]);
        temp1[VALUE_7] += static_cast<int32_t>(temp2[i * VALUE_2 + VALUE_1] *
                                               consts[COEFFICIENT_80 + i * VALUE_16 + VALUE_15]);
    }
}

void Encoder::AnalyzeEightFunction(const int16_t *inData, int32_t *outData, const int16_t *consts) const
{
    int32_t t1[VALUE_8] = {};
    int16_t t2[VALUE_8] = {};

    t1[VALUE_0] = t1[VALUE_1] = t1[VALUE_2] = t1[VALUE_3] = t1[VALUE_4] =
        t1[VALUE_5] = t1[VALUE_6] = t1[VALUE_7] = static_cast<int32_t>(VALUE_1 << (PROTO_BAND8_SCALE - VALUE_1));
    AnalyzeEightForPolyphaseFilter(t1, inData, consts);
    AnalyzeEightForScaling(t1, t2);
    AnalyzeEightForCosTransform(t1, t2, consts);
    for (int i = VALUE_0; i < VALUE_8; i++) {
        outData[i] = t1[i] >> (COS_TABLE_BAND8_SCALE - SCALE_OUT_BITS);
    }
}

void Encoder::Analyze4SubbandsInternal(int16_t *x, int32_t *outData, int increseValue)
{
    /* Analyze blocks */
    AnalyzeFourFunction(x + VALUE_12, outData, ANALYSIS_CONSTS_BAND4_ODD_MODE);
    outData += increseValue;
    AnalyzeFourFunction(x + VALUE_8, outData, ANALYSIS_CONSTS_BAND4_EVEN_MODE);
    outData += increseValue;
    AnalyzeFourFunction(x + VALUE_4, outData, ANALYSIS_CONSTS_BAND4_ODD_MODE);
    outData += increseValue;
    AnalyzeFourFunction(x + VALUE_0, outData, ANALYSIS_CONSTS_BAND4_EVEN_MODE);
}

void Encoder::Analyze8SubbandsInternal(int16_t *x, int32_t *outData, int increseValue)
{
    /* Analyze blocks */
    AnalyzeEightFunction(x + VALUE_24, outData, ANALYSIS_CONSTS_BAND8_ODD_MODE);
    outData += increseValue;
    AnalyzeEightFunction(x + VALUE_16, outData, ANALYSIS_CONSTS_BAND8_EVEN_MODE);
    outData += increseValue;
    AnalyzeEightFunction(x + VALUE_8, outData, ANALYSIS_CONSTS_BAND8_ODD_MODE);
    outData += increseValue;
    AnalyzeEightFunction(x + VALUE_0, outData, ANALYSIS_CONSTS_BAND8_EVEN_MODE);
}

int Encoder::Analyze4Subbands(int position, int16_t x[CHANNEL_NUM][BUFFER_SIZE],
                              Frame& frame, int increment)
{
    int16_t *fourBandBuffer = nullptr;
    for (int ch = VALUE_0; ch < frame.channels_; ch++) {
        fourBandBuffer = &x[ch][position - SUBBAND_4 * increment + frame.blocks_ * SUBBAND_4];
         for (int blk = VALUE_0; blk < frame.blocks_; blk += increment) {
            Analyze4SubbandsInternal(fourBandBuffer, frame.audioSamples_[blk][ch],
                                     frame.audioSamples_[blk + VALUE_1][ch] - frame.audioSamples_[blk][ch]);
            fourBandBuffer -= SUBBAND_4 * increment;
        }
    }
    return frame.blocks_ * SUBBAND_4;
}

int Encoder::Analyze8Subbands(int position, int16_t x[CHANNEL_NUM][BUFFER_SIZE],
                              Frame& frame, int increment)
{
    int16_t *eightBandBuff = nullptr;
    for (int ch = VALUE_0; ch < frame.channels_; ch++) {
        eightBandBuff = &x[ch][position - SUBBAND_8 * increment + frame.blocks_ * SUBBAND_8];
         for (int blk = VALUE_0; blk < frame.blocks_; blk += increment) {
            Analyze8SubbandsInternal(eightBandBuff, frame.audioSamples_[blk][ch],
                                     frame.audioSamples_[blk + VALUE_1][ch] - frame.audioSamples_[blk][ch]);
            eightBandBuff -= SUBBAND_8 * increment;
        }
    }
    return frame.blocks_ * SUBBAND_8;
}

static int16_t UnalignedBigEndian(const uint8_t *ptr)
{
    return static_cast<int16_t>((ptr[VALUE_0] << VALUE_8) | ptr[VALUE_1]);
}

static int16_t UnalignedLittleEndian(const uint8_t *ptr)
{
    return static_cast<int16_t>(ptr[VALUE_0] | (ptr[VALUE_1] << VALUE_8));
}

int Encoder::Get4SubbandSamplingPoint(const uint8_t *pcm, int16_t x[CHANNEL_NUM][BUFFER_SIZE],
                                      int samples, int channels, int bigEndian)
{
    if (position_ < samples) {
        if (channels > VALUE_0) {
            (void)memcpy_s(&x[VALUE_0][BUFFER_SIZE - VALUE_40], VALUE_36 * sizeof(int16_t), &x[VALUE_0][position_],
                           VALUE_36 * sizeof(int16_t));
        }
        if (channels > VALUE_1) {
            (void)memcpy_s(&x[VALUE_1][BUFFER_SIZE - VALUE_40], VALUE_36 * sizeof(int16_t), &x[VALUE_1][position_],
                           VALUE_36 * sizeof(int16_t));
        }
        position_ = BUFFER_SIZE - VALUE_40;
    }
#define PCM(i) (bigEndian ? UnalignedBigEndian(pcm + (i) * VALUE_2) : UnalignedLittleEndian(pcm + (i) * VALUE_2))
    while ((samples -= VALUE_8) >= VALUE_0) {
        position_ -= VALUE_8;
        if (channels > VALUE_0) {
            int16_t *fourBandBuffer = &x[VALUE_0][position_];
            fourBandBuffer[VALUE_0]  = PCM(VALUE_0 + VALUE_7 * channels);
            fourBandBuffer[VALUE_1]  = PCM(VALUE_0 + VALUE_3 * channels);
            fourBandBuffer[VALUE_2]  = PCM(VALUE_0 + VALUE_6 * channels);
            fourBandBuffer[VALUE_3]  = PCM(VALUE_0 + VALUE_4 * channels);
            fourBandBuffer[VALUE_4]  = PCM(VALUE_0 + VALUE_0 * channels);
            fourBandBuffer[VALUE_5]  = PCM(VALUE_0 + VALUE_2 * channels);
            fourBandBuffer[VALUE_6]  = PCM(VALUE_0 + VALUE_1 * channels);
            fourBandBuffer[VALUE_7]  = PCM(VALUE_0 + VALUE_5 * channels);
        }
        if (channels > VALUE_1) {
            int16_t *fourBandBuffer = &x[VALUE_1][position_];
            fourBandBuffer[VALUE_0]  = PCM(VALUE_1 + VALUE_7 * channels);
            fourBandBuffer[VALUE_1]  = PCM(VALUE_1 + VALUE_3 * channels);
            fourBandBuffer[VALUE_2]  = PCM(VALUE_1 + VALUE_6 * channels);
            fourBandBuffer[VALUE_3]  = PCM(VALUE_1 + VALUE_4 * channels);
            fourBandBuffer[VALUE_4]  = PCM(VALUE_1 + VALUE_0 * channels);
            fourBandBuffer[VALUE_5]  = PCM(VALUE_1 + VALUE_2 * channels);
            fourBandBuffer[VALUE_6]  = PCM(VALUE_1 + VALUE_1 * channels);
            fourBandBuffer[VALUE_7]  = PCM(VALUE_1 + VALUE_5 * channels);
        }
        pcm += VALUE_16 * channels;
    }
    return position_;
}

void Encoder::Get8SubbandSamplingPointInternal(const uint8_t *pcm, int16_t (*x)[BUFFER_SIZE],
                                              int *samples, int channels, int bigEndian)
{
#define PCM(i) (bigEndian ? UnalignedBigEndian(pcm + (i) * VALUE_2) : UnalignedLittleEndian(pcm + (i) * VALUE_2))

    if (position_ % VALUE_16 == VALUE_8) {
        position_ -= VALUE_8;
        *samples -= VALUE_8;
        if (channels > VALUE_0) {
            int16_t *eightBandBuffer = &x[VALUE_0][position_];
            eightBandBuffer[VALUE_0]  = PCM(VALUE_0 + (VALUE_15 - VALUE_8) * channels);
            eightBandBuffer[VALUE_2]  = PCM(VALUE_0 + (VALUE_14 - VALUE_8) * channels);
            eightBandBuffer[VALUE_3]  = PCM(VALUE_0 + (VALUE_8 - VALUE_8) * channels);
            eightBandBuffer[VALUE_4]  = PCM(VALUE_0 + (VALUE_13 - VALUE_8) * channels);
            eightBandBuffer[VALUE_5]  = PCM(VALUE_0 + (VALUE_9 - VALUE_8) * channels);
            eightBandBuffer[VALUE_6]  = PCM(VALUE_0 + (VALUE_12 - VALUE_8) * channels);
            eightBandBuffer[VALUE_7]  = PCM(VALUE_0 + (VALUE_10 - VALUE_8) * channels);
            eightBandBuffer[VALUE_8]  = PCM(VALUE_0 + (VALUE_11 - VALUE_8) * channels);
        }
        if (channels > VALUE_1) {
            int16_t *eightBandBuffer = &x[VALUE_1][position_];
            eightBandBuffer[VALUE_0]  = PCM(VALUE_1 + (VALUE_15 - VALUE_8) * channels);
            eightBandBuffer[VALUE_2]  = PCM(VALUE_1 + (VALUE_14 - VALUE_8) * channels);
            eightBandBuffer[VALUE_3]  = PCM(VALUE_1 + (VALUE_8 - VALUE_8) * channels);
            eightBandBuffer[VALUE_4]  = PCM(VALUE_1 + (VALUE_13 - VALUE_8) * channels);
            eightBandBuffer[VALUE_5]  = PCM(VALUE_1 + (VALUE_9 - VALUE_8) * channels);
            eightBandBuffer[VALUE_6]  = PCM(VALUE_1 + (VALUE_12 - VALUE_8) * channels);
            eightBandBuffer[VALUE_7]  = PCM(VALUE_1 + (VALUE_10 - VALUE_8) * channels);
            eightBandBuffer[VALUE_8]  = PCM(VALUE_1 + (VALUE_11 - VALUE_8) * channels);
        }

        pcm += VALUE_16 * channels;
    }
#undef PCM
}

void Encoder::Get8SubbandSamplingPoint16(const uint8_t *pcm, int16_t (*x)[BUFFER_SIZE],
                                         int *samples, int channels, int bigEndian)
{
#define PCM(i) (bigEndian ? UnalignedBigEndian(pcm + (i) * VALUE_2) : UnalignedLittleEndian(pcm + (i) * VALUE_2))

    while (*samples >= VALUE_16) {
        position_ -= VALUE_16;
        if (channels > VALUE_0) {
            int16_t *eightBandBuffer = &x[VALUE_0][position_];
            eightBandBuffer[VALUE_0]  = PCM(VALUE_0 + VALUE_15 * channels);
            eightBandBuffer[VALUE_1]  = PCM(VALUE_0 + VALUE_7 * channels);
            eightBandBuffer[VALUE_2]  = PCM(VALUE_0 + VALUE_14 * channels);
            eightBandBuffer[VALUE_3]  = PCM(VALUE_0 + VALUE_8 * channels);
            eightBandBuffer[VALUE_4]  = PCM(VALUE_0 + VALUE_13 * channels);
            eightBandBuffer[VALUE_5]  = PCM(VALUE_0 + VALUE_9 * channels);
            eightBandBuffer[VALUE_6]  = PCM(VALUE_0 + VALUE_12 * channels);
            eightBandBuffer[VALUE_7]  = PCM(VALUE_0 + VALUE_10 * channels);
            eightBandBuffer[VALUE_8]  = PCM(VALUE_0 + VALUE_11 * channels);
            eightBandBuffer[VALUE_9]  = PCM(VALUE_0 + VALUE_3 * channels);
            eightBandBuffer[VALUE_10] = PCM(VALUE_0 + VALUE_6 * channels);
            eightBandBuffer[VALUE_11] = PCM(VALUE_0 + VALUE_0 * channels);
            eightBandBuffer[VALUE_12] = PCM(VALUE_0 + VALUE_5 * channels);
            eightBandBuffer[VALUE_13] = PCM(VALUE_0 + VALUE_1 * channels);
            eightBandBuffer[VALUE_14] = PCM(VALUE_0 + VALUE_4 * channels);
            eightBandBuffer[VALUE_15] = PCM(VALUE_0 + VALUE_2 * channels);
        }
        if (channels > VALUE_1) {
            int16_t *eightBandBuffer = &x[VALUE_1][position_];
            eightBandBuffer[VALUE_0]  = PCM(VALUE_1 + VALUE_15 * channels);
            eightBandBuffer[VALUE_1]  = PCM(VALUE_1 + VALUE_7 * channels);
            eightBandBuffer[VALUE_2]  = PCM(VALUE_1 + VALUE_14 * channels);
            eightBandBuffer[VALUE_3]  = PCM(VALUE_1 + VALUE_8 * channels);
            eightBandBuffer[VALUE_4]  = PCM(VALUE_1 + VALUE_13 * channels);
            eightBandBuffer[VALUE_5]  = PCM(VALUE_1 + VALUE_9 * channels);
            eightBandBuffer[VALUE_6]  = PCM(VALUE_1 + VALUE_12 * channels);
            eightBandBuffer[VALUE_7]  = PCM(VALUE_1 + VALUE_10 * channels);
            eightBandBuffer[VALUE_8]  = PCM(VALUE_1 + VALUE_11 * channels);
            eightBandBuffer[VALUE_9]  = PCM(VALUE_1 + VALUE_3 * channels);
            eightBandBuffer[VALUE_10] = PCM(VALUE_1 + VALUE_6 * channels);
            eightBandBuffer[VALUE_11] = PCM(VALUE_1 + VALUE_0 * channels);
            eightBandBuffer[VALUE_12] = PCM(VALUE_1 + VALUE_5 * channels);
            eightBandBuffer[VALUE_13] = PCM(VALUE_1 + VALUE_1 * channels);
            eightBandBuffer[VALUE_14] = PCM(VALUE_1 + VALUE_4 * channels);
            eightBandBuffer[VALUE_15] = PCM(VALUE_1 + VALUE_2 * channels);
        }
        pcm += VALUE_32 * channels;
        *samples -= VALUE_16;
    }
#undef PCM
}

void Encoder::Get8SubbandSamplingPoint8(const uint8_t *pcm, int16_t (*x)[BUFFER_SIZE],
                                       int *samples, int channels, int bigEndian)
{
#define PCM(i) (bigEndian ? UnalignedBigEndian(pcm + (i) * VALUE_2) : UnalignedLittleEndian(pcm + (i) * VALUE_2))

    if (*samples == VALUE_8) {
        position_ -= VALUE_8;
        if (channels > VALUE_0) {
            int16_t *eightBandBuffer = &x[VALUE_0][position_];
            eightBandBuffer[-VALUE_7] = PCM(VALUE_0 + VALUE_7 * channels);
            eightBandBuffer[VALUE_1]  = PCM(VALUE_0 + VALUE_3 * channels);
            eightBandBuffer[VALUE_2]  = PCM(VALUE_0 + VALUE_6 * channels);
            eightBandBuffer[VALUE_3]  = PCM(VALUE_0 + VALUE_0 * channels);
            eightBandBuffer[VALUE_4]  = PCM(VALUE_0 + VALUE_5 * channels);
            eightBandBuffer[VALUE_5]  = PCM(VALUE_0 + VALUE_1 * channels);
            eightBandBuffer[VALUE_6]  = PCM(VALUE_0 + VALUE_4 * channels);
            eightBandBuffer[VALUE_7]  = PCM(VALUE_0 + VALUE_2 * channels);
        }
        if (channels > VALUE_1) {
            int16_t *eightBandBuffer = &x[VALUE_1][position_];
            eightBandBuffer[-VALUE_7] = PCM(VALUE_1 + VALUE_7 * channels);
            eightBandBuffer[VALUE_1]  = PCM(VALUE_1 + VALUE_3 * channels);
            eightBandBuffer[VALUE_2]  = PCM(VALUE_1 + VALUE_6 * channels);
            eightBandBuffer[VALUE_3]  = PCM(VALUE_1 + VALUE_0 * channels);
            eightBandBuffer[VALUE_4]  = PCM(VALUE_1 + VALUE_5 * channels);
            eightBandBuffer[VALUE_5]  = PCM(VALUE_1 + VALUE_1 * channels);
            eightBandBuffer[VALUE_6]  = PCM(VALUE_1 + VALUE_4 * channels);
            eightBandBuffer[VALUE_7]  = PCM(VALUE_1 + VALUE_2 * channels);
        }
    }
#undef PCM
}

int Encoder::Get8SubbandSamplingPoint(const uint8_t *pcm, int16_t (*x)[BUFFER_SIZE],
                                      int *samples, int channels, int bigEndian)
{
    if (position_ < *samples) {
        if (channels > VALUE_0) {
            (void)memcpy_s(&x[VALUE_0][BUFFER_SIZE - VALUE_72], VALUE_72 * sizeof(int16_t), &x[VALUE_0][position_],
                           VALUE_72 * sizeof(int16_t));
        }
        if (channels > VALUE_1) {
            (void)memcpy_s(&x[VALUE_1][BUFFER_SIZE - VALUE_72], VALUE_72 * sizeof(int16_t), &x[VALUE_1][position_],
                           VALUE_72 * sizeof(int16_t));
        }
        position_ = BUFFER_SIZE - VALUE_72;
    }

    Get8SubbandSamplingPointInternal(pcm, x, samples, channels, bigEndian);

    Get8SubbandSamplingPoint16(pcm, x, samples, channels, bigEndian);

    Get8SubbandSamplingPoint8(pcm, x, samples, channels, bigEndian);

    return position_;
}

static int SbcClz(uint32_t x)
{
#ifdef __GNUC__
    return __builtin_clz(x);
#else
    int cnt = VALUE_0;
    while (x) {
        cnt++;
        x >>= VALUE_1;
    }
    return VALUE_32 - cnt;
#endif
}

void Encoder::CalculateScalefactors(int32_t samples[BLOCK_NUM][CHANNEL_NUM][SUBBAND_NUM],
                                    uint32_t scaleFactor[CHANNEL_NUM][SUBBAND_NUM],
                                    int blocks, int channels, int subbands) const
{
    for (int ch = VALUE_0; ch < channels; ch++) {
        for (int sb = VALUE_0; sb < subbands; sb++) {
            uint32_t x = VALUE_1 << SCALE_OUT_BITS;
            for (int blk = VALUE_0; blk < blocks; blk++) {
                int32_t tmp = FABS(samples[blk][ch][sb]);
                if (tmp != VALUE_0) {
                    x |= tmp - VALUE_1;
                }
            }
            scaleFactor[ch][sb] = (VALUE_31 - SCALE_OUT_BITS) - SbcClz(x);
        }
    }
}

void Encoder::CalculateScalefactorsJointInternal(uint32_t &x, uint32_t &y, int32_t &tmp0, int32_t &tmp1)
{
    int sb = frame_.subbands_ - VALUE_1;
    for (int blk = VALUE_0; blk < frame_.blocks_; blk++) {
        tmp0 = FABS(frame_.audioSamples_[blk][VALUE_0][sb]);
        tmp1 = FABS(frame_.audioSamples_[blk][VALUE_1][sb]);
        if (tmp0 != VALUE_0) {
            x |= tmp0 - VALUE_1;
        }
        if (tmp1 != VALUE_0) {
            y |= tmp1 - VALUE_1;
        }
    }
    frame_.scaleFactor_[VALUE_0][sb] = (VALUE_31 - SCALE_OUT_BITS) - SbcClz(x);
    frame_.scaleFactor_[VALUE_1][sb] = (VALUE_31 - SCALE_OUT_BITS) - SbcClz(y);
}

void Encoder::CalculateScalefactorsJointForTheRestSubband(uint32_t &x, uint32_t &y,
                                                          int32_t &tmp0, int32_t &tmp1,
                                                          int &joint)
{
    int sb = frame_.subbands_ - VALUE_1;
    while (--sb >= VALUE_0) {
        int32_t sbSampleJoint[VALUE_16][VALUE_2];
        x = VALUE_1 << SCALE_OUT_BITS;
        y = VALUE_1 << SCALE_OUT_BITS;
        for (int blk = VALUE_0; blk < frame_.blocks_; blk++) {
            tmp0 = frame_.audioSamples_[blk][VALUE_0][sb];
            tmp1 = frame_.audioSamples_[blk][VALUE_1][sb];
            sbSampleJoint[blk][VALUE_0] = ASR(tmp0, VALUE_1) + ASR(tmp1, VALUE_1);
            sbSampleJoint[blk][VALUE_1] = ASR(tmp0, VALUE_1) - ASR(tmp1, VALUE_1);
            tmp0 = FABS(tmp0);
            tmp1 = FABS(tmp1);
            if (tmp0 != VALUE_0) {
                x |= tmp0 - VALUE_1;
            }
            if (tmp1 != VALUE_0) {
                y |= tmp1 - VALUE_1;
            }
        }
        frame_.scaleFactor_[VALUE_0][sb] = (VALUE_31 - SCALE_OUT_BITS) - SbcClz(x);
        frame_.scaleFactor_[VALUE_1][sb] = (VALUE_31 - SCALE_OUT_BITS) - SbcClz(y);
        x = VALUE_1 << SCALE_OUT_BITS;
        y = VALUE_1 << SCALE_OUT_BITS;
        for (int blk = VALUE_0; blk < frame_.blocks_; blk++) {
            tmp0 = FABS(sbSampleJoint[blk][VALUE_0]);
            tmp1 = FABS(sbSampleJoint[blk][VALUE_1]);
            if (tmp0 != VALUE_0) {
                x |= tmp0 - VALUE_1;
            }
            if (tmp1 != VALUE_0) {
                y |= tmp1 - VALUE_1;
            }
        }
        x = (VALUE_31 - SCALE_OUT_BITS) - SbcClz(x);
        y = (VALUE_31 - SCALE_OUT_BITS) - SbcClz(y);

        if ((frame_.scaleFactor_[VALUE_0][sb] + frame_.scaleFactor_[VALUE_1][sb]) > x + y) {
            joint |= VALUE_1 << (frame_.subbands_ - VALUE_1 - sb);
            frame_.scaleFactor_[VALUE_0][sb] = x;
            frame_.scaleFactor_[VALUE_1][sb] = y;
            for (int blk = VALUE_0; blk < frame_.blocks_; blk++) {
                frame_.audioSamples_[blk][VALUE_0][sb] = sbSampleJoint[blk][VALUE_0];
                frame_.audioSamples_[blk][VALUE_1][sb] = sbSampleJoint[blk][VALUE_1];
            }
        }
    }
}


int Encoder::CalculateScalefactorsJoint(void)
{
    int joint = VALUE_0;
    int32_t tmp0 = 0;
    int32_t tmp1 = 0;
    uint32_t x = 0;
    uint32_t y = 0;

    x = VALUE_1 << SCALE_OUT_BITS;
    y = VALUE_1 << SCALE_OUT_BITS;
    CalculateScalefactorsJointInternal(x, y, tmp0, tmp1);
    CalculateScalefactorsJointForTheRestSubband(x, y, tmp0, tmp1, joint);
    return joint;
}
} // namespace sbc
