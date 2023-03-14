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

#ifndef HISTREAMER_BIT_READER_H
#define HISTREAMER_BIT_READER_H

#include <algorithm>
#include <cstdint> // NOLINT

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Ffmpeg {
class BitReader {
public:
    BitReader();

    BitReader(const uint8_t* buffer, size_t bufferSize);

    BitReader(const uint8_t* begin, const uint8_t* end);

    template <typename T>
    BitReader(const T* buffer, size_t bufferSize) : BitReader(buffer, buffer + bufferSize)
    {
    }

    template <typename T>
    BitReader(const T* begin, const T* end)
        : BitReader(reinterpret_cast<const uint8_t*>(begin), reinterpret_cast<const uint8_t*>(end)) // NOLINT: cast
    {
    }

    ~BitReader();

    template <typename T, typename std::enable_if<std::is_unsigned<T>::value, bool>::type = true>
    bool ReadBits(uint8_t bits, T& val)
    {
        if (GetAvailableBits() < bits) {
            return false;
        }
        val = 0;
        uint8_t remainBits = bits;
        for (uint8_t toRead; remainBits; remainBits -= toRead) {
            if (availBits_ == 0) {
                ++cur_;
                availBits_ = 8; // 8
            }
            toRead = std::min(remainBits, availBits_);
            uint8_t shift = availBits_ - toRead;
            uint64_t mask = 0xFF >> (0x08 - toRead);
            val = static_cast<T>((val << toRead) | static_cast<T>(((*cur_) >> shift) & mask));
            availBits_ -= toRead;
        }
        return true;
    }

    size_t GetAvailableBits() const;

    const uint8_t* GetCurrentPtr() const;

    void SkipBits(size_t bits);

    bool SeekTo(size_t bitPos);

    template <typename T>
    bool PeekBits(uint8_t bits, T& val)
    {
        auto tmp = *this;
        return tmp.ReadBits<T>(bits, val);
    }

    template <typename T>
    void Reset(const T* buffer, size_t bufferSize)
    {
        Reset(buffer, buffer + bufferSize);
    }

    template <typename T>
    void Reset(const T* begin, const T* end)
    {
        Reset(reinterpret_cast<const uint8_t*>(begin), reinterpret_cast<const uint8_t*>(end)); // NOLINT: cast
    }

private:
    void Reset(const uint8_t* begin, const uint8_t* end);

    const uint8_t* begin_;
    const uint8_t* cur_;
    const uint8_t* end_;
    uint8_t availBits_{8};
};
} // namespace Ffmpeg
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_BIT_READER_H
