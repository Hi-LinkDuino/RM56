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

#include "common/hap_byte_buffer.h"

#include "securec.h"

#include "common/hap_verify_log.h"

namespace OHOS {
namespace Security {
namespace Verify {
const int HapByteBuffer::MAX_PRINT_LENGTH = 200;
const int HapByteBuffer::HEX_PRINT_LENGTH = 3;

HapByteBuffer::HapByteBuffer() : buffer(nullptr), position(0), limit(0), capacity(0)
{
}

HapByteBuffer::HapByteBuffer(int bufferCapacity) : buffer(nullptr), position(0), limit(0), capacity(0)
{
    Init(bufferCapacity);
}

HapByteBuffer::HapByteBuffer(const HapByteBuffer& other) : buffer(nullptr), position(0), limit(0), capacity(0)
{
    Init(other.GetCapacity());
    if (buffer != nullptr && capacity > 0) {
        if (memcpy_s(buffer.get(), capacity, other.GetBufferPtr(), other.GetCapacity()) != EOK) {
            HAPVERIFY_LOG_ERROR(LABEL, "memcpy_s failed");
            return;
        }
        position = other.GetPosition();
        limit = other.GetLimit();
    }
}

HapByteBuffer::~HapByteBuffer()
{
    buffer.reset(nullptr);
}

void HapByteBuffer::Init(int bufferCapacity)
{
    if (bufferCapacity > 0) {
        buffer = std::make_unique<char[]>(bufferCapacity);
        if (buffer != nullptr) {
            limit = bufferCapacity;
            capacity = bufferCapacity;
        }
    } else {
        HAPVERIFY_LOG_INFO(LABEL, "bufferCapacity %{public}d is too small", bufferCapacity);
    }
}

HapByteBuffer& HapByteBuffer::operator=(const HapByteBuffer& other)
{
    if (&other == this) {
        return *this;
    }

    buffer.reset(nullptr);
    Init(other.GetCapacity());
    if (buffer != nullptr && other.GetBufferPtr() != nullptr && capacity > 0) {
        if (memcpy_s(buffer.get(), capacity, other.GetBufferPtr(), other.GetCapacity()) != EOK) {
            HAPVERIFY_LOG_ERROR(LABEL, "memcpy_s failed");
            return *this;
        }
        position = other.GetPosition();
        limit = other.GetLimit();
    }
    return *this;
}

bool HapByteBuffer::CheckInputForGettingData(int index, int dataLen)
{
    if (buffer == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "buffer is nullptr");
        return false;
    }
    if (index < 0) {
        HAPVERIFY_LOG_ERROR(LABEL, "invalid index %{public}d", index);
        return false;
    }
    long long getDataLast = static_cast<long long>(position) + static_cast<long long>(index) +
        static_cast<long long>(dataLen);
    if (getDataLast > static_cast<long long>(limit)) {
        HAPVERIFY_LOG_ERROR(LABEL, "position %{public}d, index  %{public}d, limit %{public}d",
            position, index, limit);
        return false;
    }
    return true;
}

bool HapByteBuffer::GetInt64(long long& value)
{
    if (!GetInt64(0, value)) {
        HAPVERIFY_LOG_ERROR(LABEL, "GetInt64 failed");
        return false;
    }
    position += sizeof(long long);
    return true;
}

bool HapByteBuffer::GetInt64(int index, long long& value)
{
    if (!CheckInputForGettingData(index, sizeof(long long))) {
        HAPVERIFY_LOG_ERROR(LABEL, "Failed to get Int64");
        return false;
    }

    if (memcpy_s(&value, sizeof(value), (buffer.get() + position + index), sizeof(long long)) != EOK) {
        HAPVERIFY_LOG_ERROR(LABEL, "memcpy_s failed");
        return false;
    }
    return true;
}

int HapByteBuffer::GetCapacity() const
{
    return capacity;
}

const char* HapByteBuffer::GetBufferPtr() const
{
    return buffer.get();
}

bool HapByteBuffer::GetInt32(int& value)
{
    if (!GetInt32(0, value)) {
        HAPVERIFY_LOG_ERROR(LABEL, "GetInt32 failed");
        return false;
    }
    position += sizeof(int);
    return true;
}

bool HapByteBuffer::GetInt32(int index, int& value)
{
    if (!CheckInputForGettingData(index, sizeof(int))) {
        HAPVERIFY_LOG_ERROR(LABEL, "Failed to get Int32");
        return false;
    }

    if (memcpy_s(&value, sizeof(value), (buffer.get() + position + index), sizeof(int)) != EOK) {
        HAPVERIFY_LOG_ERROR(LABEL, "memcpy_s failed");
        return false;
    }
    return true;
}

bool HapByteBuffer::GetUInt32(int index, unsigned int& value)
{
    if (!CheckInputForGettingData(index, sizeof(unsigned int))) {
        HAPVERIFY_LOG_ERROR(LABEL, "Failed to get UInt32");
        return false;
    }

    if (memcpy_s(&value, sizeof(value), (buffer.get() + position + index), sizeof(unsigned int)) != EOK) {
        HAPVERIFY_LOG_ERROR(LABEL, "memcpy_s failed");
        return false;
    }
    return true;
}

bool HapByteBuffer::GetUInt16(int index, unsigned short& value)
{
    if (!CheckInputForGettingData(index, sizeof(unsigned short))) {
        HAPVERIFY_LOG_ERROR(LABEL, "Failed to get UInt16");
        return false;
    }

    if (memcpy_s(&value, sizeof(value), (buffer.get() + position + index), sizeof(unsigned short)) != EOK) {
        HAPVERIFY_LOG_ERROR(LABEL, "memcpy_s failed");
        return false;
    }
    return true;
}

void HapByteBuffer::PutInt32(int offset, int value)
{
    if (buffer != nullptr && offset >= 0 && limit - offset >= static_cast<int>(sizeof(value))) {
        if (memcpy_s((buffer.get() + offset), (limit - offset), &value, sizeof(value)) != EOK) {
            HAPVERIFY_LOG_ERROR(LABEL, "memcpy_s failed");
        }
    }
}

void HapByteBuffer::PutByte(int offset, char value)
{
    if (buffer != nullptr && offset >= 0 && limit - offset >= static_cast<int>(sizeof(value))) {
        if (memcpy_s((buffer.get() + offset), (limit - offset), (&value), sizeof(value)) != EOK) {
            HAPVERIFY_LOG_ERROR(LABEL, "memcpy_s failed");
        }
    }
}

void HapByteBuffer::PutData(int offset, const char data[], int len)
{
    if (buffer != nullptr && data != nullptr && offset >= 0 && len > 0 && (limit - offset) >= len) {
        if (memcpy_s((buffer.get() + offset), (limit - offset), data, len) != EOK) {
            HAPVERIFY_LOG_ERROR(LABEL, "memcpy_s failed");
        }
    }
}

void HapByteBuffer::SetPosition(int pos)
{
    if (pos >= 0 && pos <= limit) {
        position = pos;
    }
}

void HapByteBuffer::Slice()
{
    if (position >= capacity || limit > capacity || position >= limit || buffer == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "position %{public}d capacity %{public}d limit %{public}d error",
            position, capacity, limit);
        return;
    }
    int newCapacity = limit - position;
    std::unique_ptr<char[]> newBuffer = std::make_unique<char[]>(newCapacity);
    if (newBuffer == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "make_unique failed");
        return;
    }
    if (memcpy_s(newBuffer.get(), newCapacity, (buffer.get() + position), (limit - position)) != EOK) {
        HAPVERIFY_LOG_ERROR(LABEL, "memcpy_s failed");
        return;
    }
    buffer.reset(newBuffer.release());
    position = 0;
    capacity = newCapacity;
    limit = capacity;
}

int HapByteBuffer::GetPosition() const
{
    return position;
}

int HapByteBuffer::GetLimit() const
{
    return limit;
}

void HapByteBuffer::SetLimit(int lim)
{
    if (lim <= capacity && lim >= position) {
        limit = lim;
    }
}

int HapByteBuffer::Remaining() const
{
    return limit - position;
}

bool HapByteBuffer::HasRemaining() const
{
    return position < limit;
}

bool HapByteBuffer::CopyPartialBuffer(const HapByteBuffer& other, int len)
{
    int readableDataLen = other.Remaining();
    if (readableDataLen < len) {
        HAPVERIFY_LOG_ERROR(LABEL, "readableDataLen %{public}d less than len %{public}d", readableDataLen, len);
        return false;
    }

    buffer.reset(nullptr);
    buffer = std::make_unique<char[]>(len);
    if (buffer != nullptr && other.GetBufferPtr() != nullptr) {
        capacity = len;
        limit = capacity;
        if (memcpy_s(buffer.get(), capacity, (other.GetBufferPtr() + other.GetPosition()), len) != EOK) {
            HAPVERIFY_LOG_ERROR(LABEL, "memcpy_s failed");
            return false;
        }
    }
    position = 0;
    return true;
}

void HapByteBuffer::Clear()
{
    position = 0;
    limit = capacity;
}

bool HapByteBuffer::IsEqual(const HapByteBuffer& other)
{
    if (&other == this) {
        return true;
    }
    if (capacity != other.GetCapacity() || other.GetBufferPtr() == nullptr || buffer == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "invalid input");
        return false;
    }
    const char* otherBuffer = other.GetBufferPtr();
    for (int i = 0; i < capacity; i++) {
        if (buffer[i] != otherBuffer[i]) {
            HAPVERIFY_LOG_ERROR(LABEL, "diff value[%{public}d]: %{public}x %{public}x",
                i, buffer[i], otherBuffer[i]);
            return false;
        }
    }
    return true;
}

bool HapByteBuffer::IsEqual(const std::string& other)
{
    if (capacity != static_cast<int>(other.size()) || buffer == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "invalid input");
        return false;
    }
    for (int i = 0; i < capacity; i++) {
        if (buffer[i] != other[i]) {
            HAPVERIFY_LOG_ERROR(LABEL, "diff value[%{public}d]: %{public}x %{public}x",
                i, buffer[i], other[i]);
            return false;
        }
    }
    return true;
}

void HapByteBuffer::SetCapacity(int cap)
{
    if (buffer != nullptr) {
        buffer.reset(nullptr);
        position = 0;
        limit = 0;
        capacity = 0;
    }
    Init(cap);
}
} // namespace Verify
} // namespace Security
} // namespace OHOS
