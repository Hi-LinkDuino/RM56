/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#include "bit_reader.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Ffmpeg {
BitReader::BitReader() : begin_(nullptr), cur_(nullptr), end_(nullptr)
{
}

BitReader::BitReader(const uint8_t* buffer, size_t bufferSize) : begin_(buffer), cur_(buffer), end_(buffer + bufferSize)
{
}

BitReader::BitReader(const uint8_t* begin, const uint8_t* end) : begin_(begin), cur_(begin), end_(end)
{
}

BitReader::~BitReader()
{
    begin_ = nullptr;
    cur_ = nullptr;
    end_ = nullptr;
    availBits_ = 8; // 8
}

size_t BitReader::GetAvailableBits() const
{
    return (cur_ != end_) ? static_cast<std::size_t>(((end_ - cur_ - 1) * 8) + availBits_) : 0; // 8
}

const uint8_t* BitReader::GetCurrentPtr() const
{
    return cur_;
}

void BitReader::SkipBits(size_t bits)
{
    if (bits <= availBits_) {
        availBits_ -= static_cast<uint8_t>(bits);
        return;
    }
    auto skipBits = bits;
    cur_ += (1 + (skipBits -= availBits_) / 8); // 8
    if (cur_ >= end_) {
        cur_ = end_;
        availBits_ = 0;
    } else {
        availBits_ = 8 - skipBits % 8; // 8
    }
}

bool BitReader::SeekTo(size_t bitPos)
{
    size_t bytePos = bitPos / 8; // 8
    if (begin_ + bytePos >= end_) {
        return false;
    }
    cur_ = begin_ + bytePos;
    uint8_t skipBits = bitPos % 8; // 8
    availBits_ = 8 - skipBits;     // 8
    SkipBits(skipBits);
    return true;
}

void BitReader::Reset(const uint8_t* begin, const uint8_t* end)
{
    begin_ = begin;
    cur_ = begin;
    end_ = end;
    availBits_ = 8; // 8
}
} // namespace Ffmpeg
} // namespace Plugin
} // namespace Media
} // namespace OHOS
