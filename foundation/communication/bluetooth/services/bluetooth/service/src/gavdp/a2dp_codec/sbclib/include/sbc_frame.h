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

#ifndef SBC_FRAME_H
#define SBC_FRAME_H

#include <sys/types.h>
#include <cstddef>
#include <cstdint>

namespace sbc {

struct sbcInnerInfo {
    int channel;
    int subband;
};

class Frame {
public:
    uint8_t frequency_ {};
    uint8_t blockMode_ {};
    uint8_t blocks_ {};
    uint8_t channelMode_ {};
    uint8_t channels_ {};
    uint8_t allocation_ {};
    uint8_t subbandMode_ {};
    uint8_t subbands_ {};
    uint8_t bitpool_ {};
    uint8_t joint_ {};
    uint32_t scaleFactor_[2][8] {};
    int32_t audioSamples_[16][2][8] {};
    int32_t samples_[16][2][8] {};
    uint16_t codeSize_ {};
    uint16_t length_ {};

    Frame();
    bool IsValid() const;
    int Unpack(const uint8_t* bufStream, size_t size);
    ssize_t Pack(uint8_t* bufStream, const Frame& frame, int joint);

private:
    ssize_t PackFrameInternal(const Frame& frame, uint8_t* bufStream, int subbands, int channels, int joint);
    int UnpackFrameStream(Frame& frame, const uint8_t* bufStream, size_t len);
    void SbcCalculateBits(const Frame& frame, int (*bits)[8]);
    void SbcCaculateLevelsAndSampleDelta(const Frame& frame, int (*bits)[8],
                                         uint32_t (*levels)[8], uint32_t (*sampleDelta)[8]);
    void SbcCalculateBitsNormal(const Frame& frame, int (*bits)[8]);
    void SbcCalculateBitsStereo(const Frame& frame, int (*bits)[8]);
    int SbcCalculateBitsMax(const Frame& frame, const int channel, const int subbands, int (*bits)[8]);
    int SbcCalculateBitsMaxStereo(const Frame& frame, const int subbands, int (*bits)[8]);
    int SbcCalculateBitsMaxStereoSNR(const Frame& frame, const int subbands, int (*bits)[8]);
    int SbcCalculateBitsMaxStereoLoudness(const Frame& frame, const int subbands, int (*bits)[8]);
    void SbcCalculateBitsSliceStereo(const uint8_t bitPool, const int subbands, int (*bits)[8], int *bitCount,
                                     int *bitSlice) const;
    int SbcCalculateBitsSliceCountStereo(const int subbands, int (*bits)[8], int *bitSlice);
    static void SbcCalculateBitsInnerStereo(const int subbands, int bitSlice, int (*bitNeed)[8], int (*bits)[8]);
    int SbcCalculateBitsSliceNormal(const uint8_t bitPool, const int subbands, int (*bits)[8], int channel,
                                    int *bitSlice) const;
    static int SbcCalculateLoudness(const Frame& frame, int channel, int subband);
    static uint8_t SbcCrc8(const uint8_t* data, size_t len);
    uint AdjustSbcConsumeValue(uint consumed);
    int32_t GetSbcAudioSample(int audioSample, uint32_t scale, uint32_t level);
    uint8_t CalculateSbcCrc(Frame &frame, const uint8_t *bufStream, sbcInnerInfo sbcInnerInfo, uint &consumed, uint &crcpos);
};
} // namespace sbc
#endif // SBC_FRAME_H
