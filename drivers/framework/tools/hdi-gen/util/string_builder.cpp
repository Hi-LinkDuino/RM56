/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "util/string_builder.h"
#include "securec.h"
#include "util/logger.h"

namespace OHOS {
namespace HDI {
const char *StringBuilder::TAG = "StringBuilder";
constexpr int LINE_MAX_SIZE = 1024;

StringBuilder::~StringBuilder()
{
    if (buffer_ != nullptr) {
        free(buffer_);
    }
}

StringBuilder &StringBuilder::Append(char c)
{
    if (position_ + 1 >= capacity_) {
        if (!Grow(1)) {
            return *this;
        }
    }

    buffer_[position_] = c;
    position_ += 1;
    return *this;
}

StringBuilder &StringBuilder::Append(const char *string)
{
    if (string == nullptr || string[0] == '\0') {
        return *this;
    }

    size_t len = strlen(string);
    if (position_ + len >= capacity_) {
        if (!Grow(len)) {
            return *this;
        }
    }

    (void)memcpy_s(buffer_ + position_, capacity_ - position_, string, len);
    position_ += len;
    return *this;
}

StringBuilder &StringBuilder::Append(const String &string)
{
    if (string.IsEmpty()) {
        return *this;
    }

    size_t len = string.GetLength();
    if (position_ + len >= capacity_) {
        if (!Grow(len)) {
            return *this;
        }
    }

    (void)memcpy_s(buffer_ + position_, capacity_ - position_, string.string(), len);
    position_ += len;
    return *this;
}

StringBuilder &StringBuilder::AppendFormat(const char *format, ...)
{
    va_list args, argsCopy;

    va_start(args, format);
    va_copy(argsCopy, args);

    char buf[LINE_MAX_SIZE] = {0};
    int len = vsnprintf_s(buf, LINE_MAX_SIZE, LINE_MAX_SIZE - 1, format, args);
    if (len <= 0) {
        va_end(args);
        va_end(argsCopy);
        return *this;
    }

    if (position_ + len >= capacity_) {
        if (!Grow(len)) {
            va_end(args);
            va_end(argsCopy);
            return *this;
        }
    }

    if (vsnprintf_s(buffer_ + position_, len + 1, len, format, argsCopy) < 0) {
        va_end(args);
        va_end(argsCopy);
        return *this;
    }
    position_ += len;

    va_end(args);
    va_end(argsCopy);

    return *this;
}

bool StringBuilder::Grow(size_t size)
{
    if (capacity_ > String::MAX_SIZE) {
        Logger::E(TAG, "The StringBuilder is full.");
        return false;
    }
    // 256->the default capacity.
    size_t newSize = (capacity_ == 0) ? 256 : (capacity_ * 2);
    if (newSize < capacity_ + size) {
        newSize = capacity_ + size;
    }
    if (newSize > String::MAX_SIZE) {
        newSize = String::MAX_SIZE;
    }
    if (newSize <= capacity_) {
        return false;
    }

    char *newBuffer = reinterpret_cast<char *>(calloc(newSize, 1));
    if (newBuffer == nullptr) {
        Logger::E(TAG, "Fail to malloc %lu bytes memory.", newSize);
        return false;
    }

    if (buffer_ != nullptr) {
        (void)memcpy_s(newBuffer, newSize, buffer_, capacity_);
        free(buffer_);
    }
    buffer_ = newBuffer;
    capacity_ = newSize;
    return true;
}

String StringBuilder::ToString() const
{
    return String(buffer_, position_);
}
} // namespace HDI
} // namespace OHOS