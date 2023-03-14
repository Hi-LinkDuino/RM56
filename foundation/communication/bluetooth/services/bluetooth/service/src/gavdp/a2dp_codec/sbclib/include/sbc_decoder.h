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

#ifndef SBC_DECODER_H
#define SBC_DECODER_H

#include <cstddef>
#include <cstdint>
#include <sys/types.h>
#include "sbc_codec.h"
#include "sbc_constant.h"
#include "sbc_frame.h"

namespace sbc {
class Decoder: public IDecoderBase {
public:
    explicit Decoder();
    ~Decoder() override = default;
    ssize_t SBCDecode(const CodecParam& codecParam, const uint8_t* in, size_t iLength,
        uint8_t* out, size_t oLength, size_t* written) override;
private:
    void Init(const Frame& frame);
    int Synthesize(const Frame& frame);
    void Synthesize4(const Frame &frame, int ch, int blk);
    void Synthesize8(const Frame &frame, int ch, int blk);

    int32_t v_[2][170] {};
    int offset_[2][16] {};
    int16_t pcmSamples_[2][16 * 8] {};
    bool initialized_ {};
    Frame frame_ {};
};
} // namespace sbc
#endif // SBC_DECODER_H
