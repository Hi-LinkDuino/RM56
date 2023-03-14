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
#include "../include/sbc_frame.h"
#include <memory>
#include "../include/sbc_constant.h"
#include "../include/sbc_tables.h"
#include "sys/types.h"

namespace  sbc {
const int SBC_ENCODE_FIXED_EXTRA_BITS = 2;
const int SCALE_OUT_BITS = 15;
const int VALUE_ZERO = 0;
const int VALUE3 = 3;
const int VALUE8 = 8;
const int VALUE11 = 11;
const int VALUE32 = 32;
const int SUBBAND_FOUR = 4;
const int SUBBAND_EIGHT = 8;
const int CHANNEL_ONE = 1;
const int CHANNEL_TWO = 2;
const int BLOCK_FOUR = 4;
const int BLOCK_EIGHT = 8;
const int BLOCK_TWELVE = 12;
const int BLOCK_SIXTEEN = 16;
const int MOVE_RIGHT_BIT7 = 7;
const int VALUE_OF_SIXTEEN = 16;
const int VALUE_OF_TWO = 2;
const int VALUE_OF_FOUR = 4;
const int VALUE_OF_SEVEN = 7;
const int VALUE_NEGATIVE5 = -5;
const int MOVE_BIT1 = 1;
const int MOVE_BIT2 = 2;
const int MOVE_BIT3 = 3;
const int MOVE_BIT4 = 4;
const int MOVE_BIT5 = 5;
const int MOVE_BIT6 = 6;
const int MOVE_BIT32 = 32;
const int MIN__HEADER_SIZE = 4;
const int INDEX_VALUE3 = 3;

const uint8_t SBC_CRC_TABLE [256] = {
    0x00, 0x1D, 0x3A, 0x27, 0x74, 0x69, 0x4E, 0x53,
    0xE8, 0xF5, 0xD2, 0xCF, 0x9C, 0x81, 0xA6, 0xBB,
    0xCD, 0xD0, 0xF7, 0xEA, 0xB9, 0xA4, 0x83, 0x9E,
    0x25, 0x38, 0x1F, 0x02, 0x51, 0x4C, 0x6B, 0x76,
    0x87, 0x9A, 0xBD, 0xA0, 0xF3, 0xEE, 0xC9, 0xD4,
    0x6F, 0x72, 0x55, 0x48, 0x1B, 0x06, 0x21, 0x3C,
    0x4A, 0x57, 0x70, 0x6D, 0x3E, 0x23, 0x04, 0x19,
    0xA2, 0xBF, 0x98, 0x85, 0xD6, 0xCB, 0xEC, 0xF1,
    0x13, 0x0E, 0x29, 0x34, 0x67, 0x7A, 0x5D, 0x40,
    0xFB, 0xE6, 0xC1, 0xDC, 0x8F, 0x92, 0xB5, 0xA8,
    0xDE, 0xC3, 0xE4, 0xF9, 0xAA, 0xB7, 0x90, 0x8D,
    0x36, 0x2B, 0x0C, 0x11, 0x42, 0x5F, 0x78, 0x65,
    0x94, 0x89, 0xAE, 0xB3, 0xE0, 0xFD, 0xDA, 0xC7,
    0x7C, 0x61, 0x46, 0x5B, 0x08, 0x15, 0x32, 0x2F,
    0x59, 0x44, 0x63, 0x7E, 0x2D, 0x30, 0x17, 0x0A,
    0xB1, 0xAC, 0x8B, 0x96, 0xC5, 0xD8, 0xFF, 0xE2,
    0x26, 0x3B, 0x1C, 0x01, 0x52, 0x4F, 0x68, 0x75,
    0xCE, 0xD3, 0xF4, 0xE9, 0xBA, 0xA7, 0x80, 0x9D,
    0xEB, 0xF6, 0xD1, 0xCC, 0x9F, 0x82, 0xA5, 0xB8,
    0x03, 0x1E, 0x39, 0x24, 0x77, 0x6A, 0x4D, 0x50,
    0xA1, 0xBC, 0x9B, 0x86, 0xD5, 0xC8, 0xEF, 0xF2,
    0x49, 0x54, 0x73, 0x6E, 0x3D, 0x20, 0x07, 0x1A,
    0x6C, 0x71, 0x56, 0x4B, 0x18, 0x05, 0x22, 0x3F,
    0x84, 0x99, 0xBE, 0xA3, 0xF0, 0xED, 0xCA, 0xD7,
    0x35, 0x28, 0x0F, 0x12, 0x41, 0x5C, 0x7B, 0x66,
    0xDD, 0xC0, 0xE7, 0xFA, 0xA9, 0xB4, 0x93, 0x8E,
    0xF8, 0xE5, 0xC2, 0xDF, 0x8C, 0x91, 0xB6, 0xAB,
    0x10, 0x0D, 0x2A, 0x37, 0x64, 0x79, 0x5E, 0x43,
    0xB2, 0xAF, 0x88, 0x95, 0xC6, 0xDB, 0xFC, 0xE1,
    0x5A, 0x47, 0x60, 0x7D, 0x2E, 0x33, 0x14, 0x09,
    0x7F, 0x62, 0x45, 0x58, 0x0B, 0x16, 0x31, 0x2C,
    0x97, 0x8A, 0xAD, 0xB0, 0xE3, 0xFE, 0xD9, 0xC4
};

uint8_t Frame::SbcCrc8(const uint8_t* data, size_t len)
{
    uint8_t crc = 0x0f;
    int index = VALUE_ZERO;
    for (index = 0; index < static_cast<int>(len / VALUE8); index++) {
        crc = SBC_CRC_TABLE [crc ^ data[index]];
    }

    uint8_t octet = static_cast<int>(len % VALUE8) ? data[index] : 0;
    for (int i = 0; i < static_cast<int>(len % VALUE8); i++) {
        char bit = ((octet ^ crc) & 0x80) >> MOVE_RIGHT_BIT7;
        crc = ((crc & 0x7f) << 1) ^ (bit ? 0x1d : 0);
        octet = octet << 1;
    }
    return crc;
}

int Frame::SbcCalculateLoudness(const Frame& frame, int channel, int subband)
{
    int loudness = VALUE_ZERO;

    if (frame.subbands_ == SUBBAND_FOUR) {
        loudness = frame.scaleFactor_[channel][subband] - SBC_OFFSET_4[frame.frequency_][subband];
    } else {
        loudness = frame.scaleFactor_[channel][subband] - SBC_OFFSET_8[frame.frequency_][subband];
    }

    return loudness;
}


void Frame::SbcCalculateBitsNormal(const Frame& frame, int (*bits)[VALUE8])
{
    int subbands = (frame.subbands_ == SUBBAND_FOUR) ? SUBBAND_FOUR : SUBBAND_EIGHT;
    int bitNeed[CHANNEL_TWO][VALUE8] = {};
    for (int channel = 0; channel < frame.channels_; channel++) {
        int bitSlice = SbcCalculateBitsMax(frame, channel, subbands, bitNeed);
        int bitCount = SbcCalculateBitsSliceNormal(frame.bitpool_, subbands, bitNeed, channel, &bitSlice);

        for (int subband = 0; subband < subbands; subband++) {
            if (bitNeed[channel][subband] < bitSlice + VALUE_OF_TWO) {
                bits[channel][subband] = 0;
            } else {
                bits[channel][subband] = bitNeed[channel][subband] - bitSlice;
                if (bits[channel][subband] > VALUE_OF_SIXTEEN) {
                    bits[channel][subband] = VALUE_OF_SIXTEEN;
                }
            }
        }

        for (int subband = 0; bitCount < frame.bitpool_ && subband < subbands; subband++) {
            if ((bits[channel][subband] >= VALUE_OF_TWO) && (bits[channel][subband] < VALUE_OF_SIXTEEN)) {
                bits[channel][subband]++;
                bitCount++;
            } else if ((bitNeed[channel][subband] == bitSlice + 1) && (frame.bitpool_ > bitCount + 1)) {
                bits[channel][subband] = VALUE_OF_TWO;
                bitCount += VALUE_OF_TWO;
            }
        }

        for (int subband = 0; bitCount < frame.bitpool_ && subband < subbands; subband++) {
            if (bits[channel][subband] < VALUE_OF_SIXTEEN) {
                bits[channel][subband]++;
                bitCount++;
            }
        }
    }
}

int Frame::SbcCalculateBitsSliceNormal(const uint8_t bitPool, const int subbands, int (*bits)[8],
                                       int channel, int *bitSlice) const
{
    int sliceCount = VALUE_ZERO;
    int bitCount = VALUE_ZERO;

    do {
        (*bitSlice)--;
        bitCount += sliceCount;
        sliceCount = VALUE_ZERO;

        for (int subband = 0; subband < subbands; subband++) {
            if ((bits[channel][subband] > (*bitSlice + 1)) && (bits[channel][subband] <
                (*bitSlice + VALUE_OF_SIXTEEN))) {
                    sliceCount++;
            } else if (bits[channel][subband] == (*bitSlice + 1)){
                sliceCount += VALUE_OF_TWO;
            }
        }
    } while ((bitCount + sliceCount) < bitPool);

    if ((bitCount + sliceCount) == bitPool) {
        bitCount += sliceCount;
        (*bitSlice)--;
    }

    return bitCount;
}

int Frame::SbcCalculateBitsMax(const Frame& frame, const int channel, int subbands,
    int (*bits)[VALUE8])
{
    int loudness = VALUE_ZERO;
    int maxBit = VALUE_ZERO;

    if (frame.allocation_ == SBC_ALLOCATION_SNR) {
        for (int subband = 0; subband < subbands; subband++) {
            bits[channel][subband] = frame.scaleFactor_[channel][subband];
            if (bits[channel][subband] > maxBit) {
                maxBit = bits[channel][subband];
            }
        }
    } else {
        for (int subband = 0; subband < subbands; subband++) {
            if (frame.scaleFactor_[channel][subband] == 0) {
                bits[channel][subband] = VALUE_NEGATIVE5;
            } else {
                loudness = SbcCalculateLoudness(frame, channel, subband);
                bits[channel][subband] = (loudness > 0) ? (loudness / VALUE_OF_TWO) : loudness;
            }
            if (bits[channel][subband] > maxBit) {
                maxBit = bits[channel][subband];
            }
        }
    }

    return (maxBit + 1);
}

int Frame::SbcCalculateBitsMaxStereo(const Frame& frame, const int subbands, int (*bits)[VALUE8])
{
    int maxBit = VALUE_ZERO;

    if (frame.allocation_ == SBC_ALLOCATION_SNR) {
        for (int channel  = 0; channel < CHANNEL_TWO; channel++) {
            for (int subband = 0; subband < subbands; subband++) {
                bits[channel][subband] = frame.scaleFactor_[channel][subband];
                if (bits[channel][subband] > maxBit) {
                    maxBit = bits[channel][subband];
                }
            }
        }
    } else {
        for (int channel = 0; channel < CHANNEL_TWO; channel++) {
            for (int subband = 0; subband < subbands; subband++) {
                if (frame.scaleFactor_[channel][subband] == 0) {
                    bits[channel][subband] = VALUE_NEGATIVE5;
                } else {
                    int loudness = SbcCalculateLoudness(frame, channel, subband);
                    bits[channel][subband] = (loudness > 0) ? (loudness / VALUE_OF_TWO) : loudness;
                }
                if (bits[channel][subband] > maxBit) {
                    maxBit = bits[channel][subband];
                }
            }
        }
    }
    return (maxBit + 1);
}

void Frame::SbcCalculateBitsSliceStereo(const uint8_t bitPool, const int subbands,
                                        int (*bits)[VALUE8], int *bitCount,
                                        int *bitSlice) const
{
    int sliceCount = VALUE_ZERO;

    do {
        (*bitSlice)--;
        (*bitCount) += sliceCount;
        sliceCount = VALUE_ZERO;

        for (int channel = 0; channel < CHANNEL_TWO; channel++) {
            for (int subband = 0; subband < subbands; subband++) {
                if ((bits[channel][subband] > ((*bitSlice) + 1)) &&
                    (bits[channel][subband] < ((*bitSlice) + VALUE_OF_SIXTEEN))) {
                        sliceCount++;
                } else if (bits[channel][subband] == ((*bitSlice) + 1)) {
                    sliceCount += VALUE_OF_TWO;
                }
            }
        }
    } while ((*bitCount) + sliceCount < bitPool);

    if ((*bitCount) + sliceCount == bitPool) {
        (*bitCount) += sliceCount;
        (*bitSlice)--;
    }
}

void Frame::SbcCalculateBitsInnerStereo(const int subbands, int bitSlice,
                                        int (*bitNeed)[VALUE8],
                                        int (*bits)[VALUE8])
{
    for (int channel = 0; channel < CHANNEL_TWO; channel++) {
        for (int subband = 0; subband < subbands; subband++) {
            if (bitNeed[channel][subband] < bitSlice + VALUE_OF_TWO) {
                bits[channel][subband] = 0;
            } else {
                bits[channel][subband] = bitNeed[channel][subband] - bitSlice;
                if (bits[channel][subband] > VALUE_OF_SIXTEEN) {
                    bits[channel][subband] = VALUE_OF_SIXTEEN;
                }
            }
        }
    }
}

void Frame::SbcCalculateBitsStereo(const Frame& frame, int (*bits)[VALUE8])
{
    int subbands = (frame.subbands_ == SUBBAND_FOUR) ? SUBBAND_FOUR : SUBBAND_EIGHT;
    int bitNeed[CHANNEL_TWO][VALUE8] = {};
    int bitCount = VALUE_ZERO;

    int bitSlice = SbcCalculateBitsMaxStereo(frame, subbands, bitNeed);
    SbcCalculateBitsSliceStereo(frame.bitpool_, subbands, bitNeed, &bitCount, &bitSlice);
    SbcCalculateBitsInnerStereo(subbands, bitSlice, bitNeed, bits);

    int channel = VALUE_ZERO;
    int subband = VALUE_ZERO;

    while (bitCount < frame.bitpool_) {
        if ((bits[channel][subband] >= VALUE_OF_TWO) && (bits[channel][subband] < VALUE_OF_SIXTEEN)) {
            bits[channel][subband]++;
            bitCount++;
        } else if ((bitNeed[channel][subband] == bitSlice + 1) && (frame.bitpool_ > bitCount + 1)) {
            bits[channel][subband] = VALUE_OF_TWO;
            bitCount += VALUE_OF_TWO;
        }
        if (channel == 1) {
            channel = 0;
            subband++;
            if (subband >= subbands) {
                break;
            }
        } else {
            channel = 1;
        }
    }

    channel = VALUE_ZERO;
    subband = VALUE_ZERO;

    while (bitCount < frame.bitpool_) {
        if (bits[channel][subband] < VALUE_OF_SIXTEEN) {
            bits[channel][subband]++;
            bitCount++;
        }
        if (channel == 1) {
            channel = VALUE_ZERO;
            subband++;
            if (subband >= subbands) {
                break;
            }
        } else {
            channel = 1;
        }
    }
}

void Frame::SbcCalculateBits(const Frame& frame, int (*bits)[VALUE8])
{
    switch (frame.channelMode_) {
        case SBC_CHANNEL_MODE_MONO:
        case SBC_CHANNEL_MODE_DUAL_CHANNEL:
            SbcCalculateBitsNormal(frame, bits);
            break;
        case SBC_CHANNEL_MODE_STEREO:
        case SBC_CHANNEL_MODE_JOINT_STEREO:
            SbcCalculateBitsStereo(frame, bits);
            break;
        default:
            SbcCalculateBitsNormal(frame, bits);
            break;
    }
}

int Frame::UnpackFrameStream(Frame& frame, const uint8_t* bufStream, size_t len)
{
    uint8_t crc[VALUE11] = {0};
    crc[0] = bufStream[1];
    crc[1] = bufStream[VALUE_OF_TWO];
    uint crcpos = VALUE_OF_SIXTEEN;
    uint consumed = VALUE32;

    if (frame.channelMode_ == SBC_CHANNEL_MODE_JOINT_STEREO) {
        if ((consumed + frame.subbands_) > static_cast<uint>(len * VALUE8)) {
            return SBC_ERROR_NO_ENOUGH_BITSTREAM;
        }
        frame.joint_ = 0x00;

        for (int subband = 0; subband < frame.subbands_ - 1; subband++) {
            frame.joint_ |= ((bufStream[VALUE_OF_FOUR] >> (VALUE_OF_SEVEN - subband)) & 0x01) << subband;
        }
        if (frame.subbands_ == SUBBAND_FOUR) {
            crc[crcpos / VALUE8] = bufStream[VALUE_OF_FOUR] & 0xf0;
        } else {
            crc[crcpos / VALUE8] = bufStream[VALUE_OF_FOUR];
        }

        consumed += frame.subbands_;
        crcpos += frame.subbands_;
    }

    if (static_cast<uint>(len * VALUE8) < (consumed + (VALUE_OF_FOUR * frame.subbands_ * frame.channels_))) {
        return SBC_ERROR_NO_ENOUGH_BITSTREAM;
    }

    for (int channel = 0; channel < frame.channels_; channel++) {
        for (int subband = 0; subband < frame.subbands_; subband++) {
            frame.scaleFactor_[channel][subband] =
                (bufStream[consumed >> MOVE_BIT3] >> (VALUE_OF_FOUR - (consumed & VALUE_OF_SEVEN))) & 0x0F;
            crc[crcpos >> MOVE_BIT3] |=
                frame.scaleFactor_[channel][subband] << (VALUE_OF_FOUR - (crcpos & VALUE_OF_SEVEN));

            consumed += VALUE_OF_FOUR;
            crcpos += VALUE_OF_FOUR;
        }
    }

    if (bufStream[INDEX_VALUE3] != SbcCrc8(crc, crcpos)) {
        return SBC_ERROR_INVALID_CRC;
    }
    int bits[VALUE_OF_TWO][VALUE8];
    uint32_t levels[VALUE_OF_TWO][VALUE8];
    SbcCalculateBits(frame, bits);

    for (uint8_t channel = 0; channel < frame.channels_; channel++) {
        for (uint8_t subband = 0; subband < frame.subbands_; subband++) {
            levels[channel][subband] = (1 << bits[channel][subband]) - 1;
        }
    }

    for (uint8_t blk = 0; blk < frame.blocks_; blk++) {
        for (uint8_t channel = 0; channel < frame.channels_; channel++) {
            for (uint8_t subband = 0; subband < frame.subbands_; subband++) {
                if (levels[channel][subband] == 0) {
                    frame.samples_[blk][channel][subband] = 0;
                    continue;
                }

                uint32_t shift = frame.scaleFactor_[channel][subband] + 1 + SBC_ENCODE_FIXED_EXTRA_BITS;

                int audioSample = 0;
                for (int bit = 0; bit < bits[channel][subband]; bit++) {
                    if (consumed > static_cast<uint>(len * VALUE8)) {
                        return -1;
                    }
                    if ((bufStream[consumed >> MOVE_BIT3] >> (VALUE_OF_SEVEN - (consumed & VALUE_OF_SEVEN))) & 0x01) {
                        audioSample |= 1 << (bits[channel][subband] - bit - 1);
                    }
                    consumed++;
                }

                frame.samples_[blk][channel][subband] = static_cast<int32_t>
                    ((((static_cast<uint64_t> (audioSample) << 1) | 1) << shift) /
                        levels[channel][subband]) - (1 << shift);
            }
        }
    }

    if (frame.channelMode_ == SBC_CHANNEL_MODE_JOINT_STEREO) {
        for (uint8_t blk = 0; blk < frame.blocks_; blk++) {
            for (uint8_t subband = 0; subband < frame.subbands_; subband++) {
                if (frame.joint_ & (0x01 << subband)) {
                    uint32_t val = frame.samples_[blk][0][subband] + frame.samples_[blk][1][subband];
                    frame.samples_[blk][1][subband] =
                        frame.samples_[blk][0][subband] - frame.samples_[blk][1][subband];
                    frame.samples_[blk][0][subband] = val;
                }
            }
        }
    }

    if ((consumed & VALUE_OF_SEVEN) != 0) {
        consumed += VALUE8 - (consumed & VALUE_OF_SEVEN);
    }
    return consumed >> MOVE_BIT3;
}

Frame::Frame() {}

int Frame::Unpack(const uint8_t* bufStream, size_t size)
{
    if (size < MIN__HEADER_SIZE || bufStream[0] != SBC_SYNCWORD) {
        return SBC_ERROR_INVALID_ARG;
    }
    auto frame = std::make_unique<Frame>();
    frequency_ = (bufStream[1] >> MOVE_BIT6) & VALUE3;
    blockMode_ = (bufStream[1] >> MOVE_BIT4) & VALUE3;
    switch (blockMode_) {
        case SBC_BLOCK4:
            blocks_ = BLOCK_FOUR;
            break;
        case SBC_BLOCK8:
            blocks_ = BLOCK_EIGHT;
            break;
        case SBC_BLOCK12:
            blocks_ = BLOCK_TWELVE;
            break;
        case SBC_BLOCK16:
            blocks_ = BLOCK_SIXTEEN;
            break;
        default:
            blocks_ = BLOCK_EIGHT;
            break;
    }
    channelMode_ = (bufStream[1] >> MOVE_BIT2) & VALUE3;
    switch (channelMode_) {
        case SBC_CHANNEL_MODE_MONO:
            channels_ = CHANNEL_ONE;
            break;
        case SBC_CHANNEL_MODE_DUAL_CHANNEL:
        case SBC_CHANNEL_MODE_STEREO:
        case SBC_CHANNEL_MODE_JOINT_STEREO:
            channels_ = CHANNEL_TWO;
            break;
        default:
            channels_ = CHANNEL_ONE;
            break;
    }
    allocation_ = (bufStream[1] >> 1) & 0x01;
    subbandMode_ = (bufStream[1] & 0x01);
    subbands_ = subbandMode_ ? SUBBAND_EIGHT : SUBBAND_FOUR;
    bitpool_ = bufStream[VALUE_OF_TWO];

    if (!IsValid()) {
        return SBC_ERROR_INVALID_FRAME;
    }
    return UnpackFrameStream(*this, bufStream, size);
}

bool Frame::IsValid() const
{
    if (((channelMode_ == SBC_CHANNEL_MODE_MONO) || (channelMode_ == SBC_CHANNEL_MODE_DUAL_CHANNEL)) &&
        (bitpool_ > subbands_ * VALUE_OF_SIXTEEN)) {
            return false;
        }
    if (((channelMode_ == SBC_CHANNEL_MODE_STEREO) || (channelMode_ == SBC_CHANNEL_MODE_JOINT_STEREO)) &&
        (bitpool_ > subbands_ * MOVE_BIT32)) {
            return false;
        }

    return true;
}

#define PUT_BITS(data, cache, count, v, n)                \
        do {                                                                \
                (cache) = (v) | ((cache) << (n));                        \
                (count) += (n);                                        \
                if ((count) >= VALUE_OF_SIXTEEN) {                                        \
                        (count) -= VALUE8;                                \
                        *(data)++ = static_cast<uint8_t>                                \
                                ((cache) >> (count));                \
                        (count) -= VALUE8;                                \
                        *(data)++ = static_cast<uint8_t>                                \
                                ((cache) >> (count));                \
                }                                                        \
        } while (0)

#define FLUSH_BITS(data, cache, count)                        \
        do {                                                                \
                while ((count) >= VALUE8) {                                \
                        (count) -= VALUE8;                                \
                        *(data)++ = static_cast<uint8_t>                                \
                                ((cache) >> (count));                \
                }                                                        \
                if ((count) > 0)        {                                \
                        *(data)++ = static_cast<uint8_t>                                \
                                ((cache) << (VALUE8 - (count)));        \
                }                           \
        } while (0)

void Frame::SbcCaculateLevelsAndSampleDelta(const Frame& frame, int (*bits)[VALUE8],
                                            uint32_t (*levels)[VALUE8], uint32_t (*sampleDelta)[VALUE8])
{
    for (int channel = 0; channel < frame.channels_; channel++) {
        for (int subband = 0; subband < frame.subbands_; subband++) {
            levels[channel][subband] = ((1 << bits[channel][subband]) - 1)
                << (MOVE_BIT32 - (frame.scaleFactor_[channel][subband] + SCALE_OUT_BITS + MOVE_BIT2));
            sampleDelta[channel][subband] = static_cast<uint32_t> (1
                << (frame.scaleFactor_[channel][subband] + SCALE_OUT_BITS + 1));
        }
    }
}

ssize_t Frame::PackFrameInternal(const Frame& frame, uint8_t* bufStream,
                    int subbands, int channels, int joint)
{
    uint8_t *data = bufStream + VALUE_OF_FOUR;
    uint32_t bitsCache = VALUE_ZERO;
    uint32_t bitsCount = VALUE_ZERO;
    uint8_t crc[VALUE11] = {};
    int crcpos = VALUE_ZERO;
    uint32_t audioSample = VALUE_ZERO;
    int bits[VALUE_OF_TWO][VALUE8] = {};
    uint32_t levels[VALUE_OF_TWO][VALUE8] = {};
    uint32_t sampleDelta[VALUE_OF_TWO][VALUE8] = {};
    crc[0] = bufStream[1];
    crc[1] = bufStream[VALUE_OF_TWO];
    crcpos = VALUE_OF_SIXTEEN;
    if (frame.channelMode_ == SBC_CHANNEL_MODE_JOINT_STEREO) {
        PUT_BITS(data, bitsCache, bitsCount, joint, subbands);
        crc[crcpos >> MOVE_BIT3] = joint;
        crcpos += subbands;
    }
    for (int channel = 0; channel < channels; channel++) {
        for (int subband = 0; subband < subbands; subband++) {
            PUT_BITS(data, bitsCache, bitsCount, frame.scaleFactor_[channel][subband] & 0x0F, MOVE_BIT4);
            crc[crcpos >> MOVE_BIT3] <<= MOVE_BIT4;
            crc[crcpos >> MOVE_BIT3] |= frame.scaleFactor_[channel][subband] & 0x0F;
            crcpos += MOVE_BIT4;
        }
    }
    if (crcpos % VALUE8) {
        crc[crcpos >> MOVE_BIT3] <<= VALUE8 - (crcpos % VALUE8);
    }
    bufStream[INDEX_VALUE3] = SbcCrc8(crc, crcpos);
    SbcCalculateBits(frame, bits);
    SbcCaculateLevelsAndSampleDelta(frame, bits, levels, sampleDelta);
    for (int blk = 0; blk < frame.blocks_; blk++) {
        for (int channel = 0; channel < channels; channel++) {
            for (int subband = 0; subband < subbands; subband++) {
                if (bits[channel][subband] == 0) {
                    continue;
                }
                audioSample = (static_cast<uint64_t>(levels[channel][subband]) *
                    (sampleDelta[channel][subband] + frame.audioSamples_[blk][channel][subband])) >> MOVE_BIT32;
                PUT_BITS(data, bitsCache, bitsCount, audioSample, bits[channel][subband]);
            }
        }
    }
    FLUSH_BITS(data, bitsCache, bitsCount);
    return data - bufStream;
}


ssize_t Frame::Pack(uint8_t* bufStream, const Frame& frame, int joint)
{
    bufStream[0] = SBC_SYNCWORD;
    bufStream[1] = (frame.frequency_ & VALUE3) << MOVE_BIT6;
    bufStream[1] |= (frame.blockMode_ & VALUE3) << MOVE_BIT4;
    bufStream[1] |= (frame.channelMode_ & VALUE3) << MOVE_BIT2;
    bufStream[1] |= (frame.allocation_ & 0x01) << MOVE_BIT1;
    bufStream[VALUE_OF_TWO] = frame.bitpool_;

    int subbands = SUBBAND_FOUR;
    int channels = CHANNEL_ONE;

    if (subbands_ != SUBBAND_FOUR) {
        subbands = SUBBAND_EIGHT;
    }

    if (((frame.channelMode_ == SBC_CHANNEL_MODE_MONO) || (frame.channelMode_ == SBC_CHANNEL_MODE_DUAL_CHANNEL))
        && (bitpool_ > (subbands << MOVE_BIT4))) {
        return -1;
    }

    if (((frame.channelMode_ == SBC_CHANNEL_MODE_STEREO) || (frame.channelMode_ == SBC_CHANNEL_MODE_JOINT_STEREO))
        && (bitpool_ > (subbands << MOVE_BIT5))) {
        return -1;
    }

    if (subbands_ == SUBBAND_FOUR) {
        channels = (frame.channels_ == CHANNEL_ONE) ? CHANNEL_ONE : CHANNEL_TWO;
    } else {
        bufStream[1] |= 0x01;
        channels = (frame.channels_ == CHANNEL_ONE) ? CHANNEL_ONE : CHANNEL_TWO;
    }
    return PackFrameInternal(frame, bufStream, subbands, channels, joint);
}
} // namespace sbc
