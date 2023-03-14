/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CODEC_BYTE_BUFFER_OPERATOR_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CODEC_BYTE_BUFFER_OPERATOR_H

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "base/log/log.h"
#include "base/utils/noncopyable.h"

namespace OHOS::Ace::Framework {

class ByteBufferReader final {
public:
    explicit ByteBufferReader(const std::vector<uint8_t>& buffer) : buffer_(buffer) {}
    ~ByteBufferReader() = default;

    bool ReadData(uint8_t& value) const
    {
        return ReadValue(value);
    }
    bool ReadData(int32_t& value) const
    {
        return ReadValue(value);
    }
    bool ReadData(int64_t& value) const
    {
        return ReadValue(value);
    }
    bool ReadData(double& value) const
    {
        return ReadValue(value);
    }
    bool ReadData(std::string& value) const
    {
        return ReadArray(value);
    }

    bool ReadData(std::vector<int8_t>& dst) const
    {
        return ReadArray(dst);
    }
    bool ReadData(std::vector<int16_t>& dst) const
    {
        return ReadArray(dst);
    }
    bool ReadData(std::vector<int32_t>& dst) const
    {
        return ReadArray(dst);
    }

    bool ReadData(std::map<std::string, std::string>& dst) const;
    bool ReadData(std::set<std::string>& dst) const;

private:
    template<class T>
    bool ReadValue(T& value) const
    {
        if (readPos_ + sizeof(T) > buffer_.size()) {
            LOGW("Exceed buffer size, readPos = %{public}u, buffer size = %{public}zu", readPos_, buffer_.size());
            return false;
        }
        value = *reinterpret_cast<const T*>(buffer_.data() + readPos_);
        readPos_ += sizeof(T);
        return true;
    }

    template<class T>
    bool ReadArray(T& dst) const
    {
        int32_t length = -1;
        if (!ReadData(length) || length < 0 ||
            readPos_ + static_cast<uint32_t>(sizeof(typename T::value_type) * length) > buffer_.size()) {
            LOGW("Could not read array length or array length is invalid");
            return false;
        }
        auto data = reinterpret_cast<const typename T::value_type*>(buffer_.data() + readPos_);
        dst.assign(data, data + length);
        readPos_ += static_cast<uint32_t>(sizeof(typename T::value_type) * length);
        return true;
    }

    const std::vector<uint8_t>& buffer_;
    mutable uint32_t readPos_ = 0;

    ACE_DISALLOW_COPY_AND_MOVE(ByteBufferReader);
};

class ByteBufferWriter final {
public:
    ByteBufferWriter(std::vector<uint8_t>& buffer) : buffer_(buffer) {}
    ~ByteBufferWriter() = default;

    void WriteData(uint8_t value)
    {
        WriteValue(value);
    }
    void WriteData(int32_t value)
    {
        WriteValue(value);
    }
    void WriteData(int64_t value)
    {
        WriteValue(value);
    }
    void WriteData(double value)
    {
        WriteValue(value);
    }
    void WriteData(const std::string& src)
    {
        WriteArray(src);
    }

    void WriteData(const std::vector<int8_t>& src)
    {
        WriteArray(src);
    }
    void WriteData(const std::vector<int16_t>& src)
    {
        WriteArray(src);
    }
    void WriteData(const std::vector<int32_t>& src)
    {
        WriteArray(src);
    }

    void WriteData(const std::map<std::string, std::string>& mapValue);
    void WriteData(const std::set<std::string>& setValue);

private:
    template<class T>
    void WriteValue(T value)
    {
        auto data = reinterpret_cast<uint8_t*>(&value);
        buffer_.insert(buffer_.end(), data, data + sizeof(T));
    }

    template<class T>
    void WriteArray(const T& array)
    {
        WriteData(static_cast<int32_t>(array.size()));
        auto data = reinterpret_cast<const uint8_t*>(array.data());
        buffer_.insert(buffer_.end(), data, data + sizeof(typename T::value_type) * array.size());
    }

    std::vector<uint8_t>& buffer_;

    ACE_DISALLOW_COPY_AND_MOVE(ByteBufferWriter);
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CODEC_BYTE_BUFFER_OPERATOR_H
