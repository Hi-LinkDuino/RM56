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

#ifndef SBC_CODEC_H
#define SBC_CODEC_H

#include <cstddef>
#include <cstdint>
#include <sys/types.h>

#include "sbc_constant.h"

namespace sbc {
class IEncoderBase {
public:
    virtual ~IEncoderBase() = default;
    virtual ssize_t SBCEncode(const CodecParam& codecParam, const uint8_t* in, size_t iLength, uint8_t* out,
                   size_t oLength, size_t* written) = 0;
};

class IDecoderBase {
public:
    virtual ~IDecoderBase() = default;
    virtual ssize_t SBCDecode(const CodecParam& codecParam, const uint8_t* in, size_t iLength,
        uint8_t* out, size_t oLength, size_t* written) = 0;
};

typedef IEncoderBase* (*createSbcEncoder)(void);
typedef void (*destroySbcEncoder)(IEncoderBase* p);

typedef IDecoderBase* (*createSbcDecoder)(void);
typedef void (*destroySbcDecoder)(IDecoderBase* p);
}  // namespace sbc
#endif  // SBC_CODEC_H
