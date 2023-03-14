/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "raw_stream_data.h"

#include <utility>

#include "securec.h"
#include "stream_common.h"

namespace Communication {
namespace SoftBus {
std::unique_ptr<IStream> IStream::MakeRawStream(StreamData &data, const StreamFrameInfo &info)
{
    static_cast<void>(info);
    auto raw = std::make_unique<RawStreamData>();
    raw->InitStreamData(std::move(data.buffer), data.bufLen, std::move(data.extBuffer), data.extLen);

    return raw;
}

std::unique_ptr<IStream> IStream::MakeRawStream(const char *buf, ssize_t bufLen, const StreamFrameInfo &info, int scene)
{
    if (scene != COMPATIBLE_SCENE && scene != SOFTBUS_SCENE) {
        return nullptr;
    }

    if (bufLen <= 0 || bufLen > MAX_STREAM_LEN) {
        return nullptr;
    }

    static_cast<void>(info);
    auto raw = std::make_unique<RawStreamData>();
    if (scene == COMPATIBLE_SCENE) {
        auto buffer = std::make_unique<char[]>(bufLen);
        auto ret = memcpy_s(buffer.get(), bufLen, buf, bufLen);
        if (ret != 0) {
            return nullptr;
        }
        raw->InitStreamData(std::move(buffer), bufLen, nullptr, 0);

        return raw;
    }

    auto buffer = std::make_unique<char[]>(bufLen + RawStreamData::FRAME_HEADER_LEN);
    auto ret = memcpy_s(buffer.get() + RawStreamData::FRAME_HEADER_LEN, bufLen, buf, bufLen);
    if (ret != 0) {
        return nullptr;
    }
    RawStreamData::InsertBufferLength(bufLen, RawStreamData::FRAME_HEADER_LEN,
        reinterpret_cast<uint8_t *>(buffer.get()));

    raw->InitStreamData(std::move(buffer), bufLen + RawStreamData::FRAME_HEADER_LEN, nullptr, 0);

    return raw;
}

int RawStreamData::InitStreamData(std::unique_ptr<char[]> buffer, ssize_t bufLen,
    std::unique_ptr<char[]> extBuffer, ssize_t extLen)
{
    streamData_ = std::move(buffer);
    streamLen_ = bufLen;
    static_cast<void>(extBuffer);
    static_cast<void>(extLen);
    return 0;
}

std::unique_ptr<char[]> RawStreamData::GetBuffer()
{
    return std::unique_ptr<char[]>(streamData_.release());
}

ssize_t RawStreamData::GetBufferLen() const
{
    return streamLen_;
};

void RawStreamData::InsertBufferLength(int num, int length, uint8_t *output)
{
    for (int i = 0; i < length; i++) {
        output[length - 1 - i] = static_cast<unsigned int>(
            ((static_cast<unsigned int>(num) >> static_cast<unsigned int>(BYTE_TO_BIT * i))) & INT_TO_BYTE);
    }
}
} // namespace SoftBus
} // namespace Communication
