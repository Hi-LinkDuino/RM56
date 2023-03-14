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

#ifndef A2DP_CODEC_ERRORS_H
#define A2DP_CODEC_ERRORS_H

#include <stdio.h>

enum A2dpCodecError {
    // Failed
    A2DP_CODEC_ERROR_FAIL = 0x0A,
    A2DP_CODEC_ERROR_BUSY = 0x0B,

    // Bad parameters
    A2DP_CODEC_ERROR_INVALID_PARAMS = 0x0C,

    // Wrong codec info
    A2DP_CODEC_ERROR_WRONG_CODEC = 0x0D,

    // Media Codec Type is not valid
    A2DP_CODEC_ERROR_BAD_CODEC_TYPE = 0xC1,

    // Media Codec Type is not supported
    A2DP_CODEC_ERROR_NOSUPPORTED_CODEC_TYPE = 0xC2,

    // Sampling Frequency is not valid or multiple values have been selected
    A2DP_CODEC_ERROR_BAD_SAMP_FREQ = 0xC3,

    // Sampling Frequency is not supported
    A2DP_CODEC_ERROR_NOSUPPORTED_SAMP_FREQ = 0xC4,

    // Channel Mode is not valid or multiple values * have been selected
    A2DP_CODEC_ERROR_BAD_CHANNEL_MODE = 0xC5,

    // Channel Mode is not supported
    A2DP_CODEC_ERROR_NOSUPPORTED_CHANNEL_MODE = 0xC6,

    // None or multiple values have been selected for Number of Subbands
    A2DP_CODEC_ERROR_BAD_SUBBANDS = 0xC7,

    /* Number of Subbands is not supported */
    A2DP_CODEC_ERROR_NOSUPPORTED_SUBBANDS = 0xC8,

    /* Allocation Method is not supported */
    A2DP_CODEC_ERROR_NOSUPPORTED_ALLOC_METHOD = 0xCA,

    /* Minimum Bitpool Value is not valid */
    A2DP_CODEC_ERROR_BAD_MIN_BITPOOL = 0xCB,

    /* Minimum Bitpool Value is not supported */
    A2DP_CODEC_ERROR_NOSUPPORTED_MIN_BITPOOL = 0xCC,

    /* Maximum Bitpool Value is not valid */
    A2DP_CODEC_ERROR_BAD_MAX_BITPOOL = 0xCD,

    /* Maximum Bitpool Value is not supported */
    A2DP_CODEC_ERROR_NOSUPPORTED_MAX_BITPOOL = 0xCE,

    // None or multiple values have been selected for Bit Rate
    A2DP_CODEC_ERROR_BAD_BIT_RATE = 0xD4,

    // Bit Rate is not supported
    A2DP_CODEC_ERROR_NOSUPPORTED_BIT_RATE = 0xD5,

    // Object Type is not supported
    A2DP_CODEC_ERROR_NOSUPPORTED_OBJ_TYPE = 0xD6,

    // None or multiple values have been selected for Channels
    A2DP_CODEC_ERROR_BAD_CHANNEL = 0xD8,

    // Channels is not supported
    A2DP_CODEC_ERROR_NOSUPPORTED_CHANNEL = 0xD9,

    // None or multiple values have been selected for Block Length
    A2DP_CODEC_ERROR_BAD_BLOCK_LEN = 0xDD,

    // The requested CP Type is not supported.
    A2DP_CODEC_ERROR_BAD_CP_TYPE = 0xE0,
};

#endif  // A2DP_CODEC_ERRORS_H
