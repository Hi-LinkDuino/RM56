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

#ifndef SBC_CONSTANT_H
#define SBC_CONSTANT_H

#include <cstddef>
#include <cstdint>

namespace sbc {
#define SBC_MAX_NUM_OF_BLOCKS 16
#define SBC_MAX_NUM_OF_CHANNELS 2
#define SBC_MAX_NUM_OF_SUBBANDS 8

#define SBC_MAX_PCM_BUFFER_SIZE \
    (SBC_MAX_NUM_OF_BLOCKS * SBC_MAX_NUM_OF_SUBBANDS * SBC_MAX_NUM_OF_CHANNELS)

// Codec param
typedef struct {
    uint8_t frequency;
    uint8_t blocks;
    uint8_t subbands;
    uint8_t channelMode;
    uint8_t allocation;
    uint8_t bitpool;
    uint8_t endian;
} CodecParam;

// Errors
enum SbcErrors {
    SBC_ERROR_INVALID_ARG = -1,
    SBC_ERROR_NO_ENOUGH_BITSTREAM = -2,
    SBC_ERROR_INVALID_FRAME = -3,
    SBC_ERROR_INVALID_CRC = -4,
    SBC_ERROR_BITPOOL_OUT = -5,
};

// SyncWord declaration
enum SbcSyncWord {
    SBC_SYNCWORD = 0x9C,
    MSBC_SYNCWORD = 0xAD,
};

// Sampling frequency
enum SbcSamplingFrequency {
    SBC_FREQ_16000 = 0x00,
    SBC_FREQ_32000 = 0x01,
    SBC_FREQ_44100 = 0x02,
    SBC_FREQ_48000 = 0x03,
};

// Blocks
enum SbcBlockMode {
    SBC_BLOCK4 = 0x00,
    SBC_BLOCK8 = 0x01,
    SBC_BLOCK12 = 0x02,
    SBC_BLOCK16 = 0x03,
};

// Channel Mode
enum SbcChannelMode {
    SBC_CHANNEL_MODE_MONO = 0x00,
    SBC_CHANNEL_MODE_DUAL_CHANNEL = 0x01,
    SBC_CHANNEL_MODE_STEREO = 0x02,
    SBC_CHANNEL_MODE_JOINT_STEREO = 0x3,
};

// Subbands declaration
enum SbcSubband {
    SBC_SUBBAND4 = 0x00,
    SBC_SUBBAND8 = 0x01,
};

// Sbc data endianness
enum SbcEndianess {
    SBC_ENDIANESS_LE = 0x00,
    SBC_ENDIANESS_BE = 0x01,
};

// Sbc allocation method
enum SbcAllocation {
    SBC_ALLOCATION_LOUDNESS = 0x00,
    SBC_ALLOCATION_SNR = 0x01,
};
} // namespace sbc
#endif //  SBC_CONSTANT_H
