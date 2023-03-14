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

#ifndef SBC_ENCODER_H
#define SBC_ENCODER_H

#include <cstddef>
#include <cstdint>
#include <sys/types.h>
#include "sbc_codec.h"
#include "sbc_constant.h"
#include "sbc_frame.h"

namespace sbc {
class Encoder : public IEncoderBase {
public:
    static constexpr int BUFFER_SIZE = 328;
    explicit Encoder();
    virtual ~Encoder();
    ssize_t SBCEncode(const CodecParam& codecParam, const uint8_t* in, size_t iLength, uint8_t* out,
                   size_t oLength, size_t* written) override;

private:
    void Init(const Frame& frame);
    static size_t CalculateFrameLength(const CodecParam& codecParam);
    static size_t CalculateCodecSize(const CodecParam& codecParam);
    void UpdateCodecFormat(const CodecParam& codecParam);
    void Analyze4SubbandsInternal(int16_t *x, int32_t *outData, int increseValue);
    void Analyze8SubbandsInternal(int16_t *x, int32_t *outData, int increseValue);
    static void AnalyzeFourForPolyphaseFilter(int32_t *temp, const int16_t *inData, const int16_t *consts);
    static void AnalyzeFourForScaling(int32_t *temp1, int16_t *temp2);
    static void AnalyzeFourForCosTransform(int32_t *temp1, int16_t *temp2, const int16_t *consts);
    void AnalyzeFourFunction(const int16_t *inData, int32_t *outData, const int16_t *consts) const;
    static void AnalyzeEightForPolyphaseFilter(int32_t *temp, const int16_t *inData, const int16_t *consts);
    static void AnalyzeEightForScaling(int32_t *temp1, int16_t *temp2);
    static void AnalyzeEightForCosTransform(int32_t *temp1, int16_t *temp2, const int16_t *consts);
    void AnalyzeEightFunction(const int16_t *inData, int32_t *outData, const int16_t *consts) const;
    int Analyze4Subbands(int position, int16_t x[2][BUFFER_SIZE], Frame& frame, int increment);
    int Analyze8Subbands(int position, int16_t x[2][BUFFER_SIZE], Frame& frame, int increment);
    void Get8SubbandSamplingPointInternal(const uint8_t* pcm, int16_t(*x)[BUFFER_SIZE],
                                          int *samples, int channels, int bigEndian);
    void Get8SubbandSamplingPoint16(const uint8_t* pcm, int16_t(*x)[BUFFER_SIZE],
                                    int *samples, int channels, int bigEndian);
    void Get8SubbandSamplingPoint8(const uint8_t* pcm, int16_t(*x)[BUFFER_SIZE],
                                   int *samples, int channels, int bigEndian);
    int Get8SubbandSamplingPoint(const uint8_t* pcm, int16_t(*x)[BUFFER_SIZE],
                                 int *samples, int channels, int bigEndian);
    int Get4SubbandSamplingPoint(const uint8_t* pcm, int16_t x[2][BUFFER_SIZE],
                                 int samples, int channels, int bigEndian);
    void CalculateScalefactors(int32_t samples[16][2][8], uint32_t scaleFactor[2][8],
                               int blocks, int channels, int subbands) const;
    void CalculateScalefactorsJointInternal(uint32_t &x, uint32_t &y, int32_t &tmp0, int32_t &tmp1);
    void CalculateScalefactorsJointForTheRestSubband(uint32_t &x, uint32_t &y, int32_t &tmp0,
                                                     int32_t &tmp1, int &joint);
    int CalculateScalefactorsJoint(void);

    bool initialized_ {};
    Frame frame_ {};
    int position_ {};
    uint8_t increment_ {};
    int16_t x_[2][BUFFER_SIZE] {};
};
} // namespace sbc
#endif // SBC_ENCODER_H
