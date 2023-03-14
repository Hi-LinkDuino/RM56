/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "util/string.h"

#include <atomic>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <new>

#include "securec.h"
#include "util/logger.h"
#include "util/string_builder.h"

namespace OHOS {
namespace HDI {
constexpr int LINE_MAX_SIZE = 1024;

using SharedData = struct SharedData {
    SharedData(int refCount, int size) : refCount_(refCount), size_(size) {}

    static SharedData *Allocate(int size);

    static void AddRef(const void *handle);

    static void Release(const void *handle);

    inline static char *ToString(SharedData *header)
    {
        return reinterpret_cast<char *>(header + 1);
    }

    inline static SharedData *GetHeader(const void *handle)
    {
        return reinterpret_cast<SharedData *>(const_cast<void *>(handle)) - 1;
    }

    std::atomic<int> refCount_;
    int size_;
};

SharedData *SharedData::Allocate(int size)
{
    if (size < 0) {
        Logger::E(String::TAG, "Size %d is illegal.", size);
        return nullptr;
    }
    if (size > String::MAX_SIZE) {
        Logger::E(String::TAG, "The string is too large to alloc.");
        return nullptr;
    }

    auto handle = reinterpret_cast<SharedData *>(malloc(sizeof(SharedData) + size + 1));
    if (handle == nullptr) {
        Logger::E(String::TAG, "Fail to malloc %lu bytes memory", size);
        return handle;
    }

    new (handle) SharedData(1, size);
    return handle;
}

void SharedData::AddRef(const void *handle)
{
    if (handle == nullptr) {
        return;
    }

    SharedData *data = GetHeader(handle);
    int before = data->refCount_.fetch_add(1);
    if (before + 1 <= 1) {
        Logger::E(String::TAG, "The refCount %d of data is error in AddRef.", before);
    };
}

void SharedData::Release(const void *handle)
{
    if (handle == nullptr) {
        return;
    }

    SharedData *data = GetHeader(handle);
    int before = data->refCount_.fetch_sub(1);
    if (before - 1 == 0) {
        free(data);
    } else if (before - 1 < 0) {
        Logger::E(String::TAG, "The refCount %d of data is error in Release.", before - 1);
    };
}

const char *String::TAG = "String";

String::String(const char *string)
{
    if (string != nullptr) {
        string_ = SharedData::ToString(SharedData::Allocate(strlen(string)));
        if (string_ == nullptr) {
            return;
        }

        if (strcpy_s(string_, strlen(string) + 1, string) != EOK) {
            Logger::E(String::TAG, "The Construct of \"%s\" is failed.", string);
            SharedData::Release(string_);
            string_ = nullptr;
        }
    }
}

String::String(const char *string, size_t length)
{
    if (string != nullptr) {
        string_ = SharedData::ToString(SharedData::Allocate(length));
        if (string_ != nullptr) {
            (void)memcpy_s(string_, length + 1, string, length);
            string_[length] = '\0';
        }
    }
}

String::String(const String &other)
{
    string_ = other.string_;
    SharedData::AddRef(string_);
}

String::String(String &&other)
{
    string_ = other.string_;
    other.string_ = nullptr;
}

String::String(int size)
{
    string_ = SharedData::ToString(SharedData::Allocate(size));
    if (string_ != nullptr) {
        (void)memset_s(string_, size + 1, 0, size + 1);
    }
}

String::~String()
{
    SharedData::Release(string_);
}

int String::GetLength() const
{
    if (string_ == nullptr) {
        return 0;
    }

    return SharedData::GetHeader(string_)->size_;
}

char String::operator[](int index) const
{
    if (index < 0 || index >= GetLength()) {
        return '\0';
    }
    return string_[index];
}

bool String::Equals(const char *string) const
{
    if (string_ == nullptr && string == nullptr) {
        return true;
    }

    if (string != nullptr && string_ != nullptr) {
        if ((size_t)GetLength() != strlen(string)) {
            return false;
        }
        return strcmp(string, string_) == 0;
    }

    return false;
}

bool String::Equals(const String &other) const
{
    if (string_ == nullptr && other.string_ == nullptr) {
        return true;
    }

    if (string_ != nullptr && other.string_ != nullptr) {
        if (GetLength() != other.GetLength()) {
            return false;
        }
        return strcmp(string_, other.string_) == 0;
    }
    return false;
}

int String::Compare(const String &other) const
{
    if (string_ == nullptr) {
        if (other.string_ == nullptr) {
            return 0;
        } else {
            return -1;
        }
    }

    if (other.string_ == nullptr) {
        return 1;
    }

    return strcmp(string_, other.string_);
}

int String::GetHashCode() const
{
    // BKDR Hash Function
    unsigned int seed = 31; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    const char *string = string_;
    if (string != nullptr) {
        for (; *string; ++string) {
            hash = hash * seed + (*string);
        }
    }
    return (hash & 0x7FFFFFFF);
}

int String::IndexOf(char c, int fromIndex) const
{
    if (IsEmpty() || c == '\0') {
        return -1;
    }

    if (fromIndex < 0) {
        fromIndex = 0;
    } else if (fromIndex >= GetLength()) {
        return -1;
    }

    char *p = string_ + fromIndex;
    char *end = string_ + GetLength();
    while (p != end) {
        if (*p == c) {
            return p - string_;
        }
        p++;
    }
    return -1;
}

int String::IndexOf(const char *string, int fromIndex) const
{
    if (IsEmpty() || string == nullptr || string[0] == '\0') {
        return -1;
    }

    if (fromIndex < 0) {
        fromIndex = 0;
    } else if (fromIndex >= GetLength()) {
        return -1;
    }

    char *c = strstr(string_ + fromIndex, string);
    return (c != nullptr) ? (c - string_) : -1;
}

int String::IndexOf(const String &other, int fromIndex) const
{
    if (IsEmpty() || other.IsEmpty()) {
        return -1;
    }

    if (fromIndex < 0) {
        fromIndex = 0;
    } else if (fromIndex >= GetLength()) {
        return -1;
    }

    char *c = strstr(string_ + fromIndex, other.string_);
    return (c != nullptr) ? (c - string_) : -1;
}

int String::LastIndexOf(char c, int fromIndex) const
{
    if (IsEmpty() || c == '\0') {
        return -1;
    }

    if (fromIndex < 0) {
        return -1;
    } else if (fromIndex == 0 || fromIndex >= GetLength()) {
        fromIndex = GetLength() - 1;
    }
    char *p = string_ + fromIndex;
    while (p != string_) {
        if (*p == c) {
            return p - string_;
        }
        p--;
    }
    return -1;
}

int String::LastIndexOf(const char *string, int fromIndex) const
{
    if (IsEmpty() || string == nullptr || string[0] == '\0') {
        return -1;
    }

    if (fromIndex < 0) {
        return -1;
    } else if (fromIndex == 0 || fromIndex >= GetLength()) {
        fromIndex = GetLength() - 1;
    }

    return LastIndexOfInternal(string, fromIndex);
}

int String::LastIndexOf(const String &other, int fromIndex) const
{
    if (IsEmpty() || other.IsEmpty()) {
        return -1;
    }

    if (fromIndex < 0) {
        return -1;
    } else if (fromIndex == 0 || fromIndex >= GetLength()) {
        fromIndex = GetLength() - 1;
    }

    return LastIndexOfInternal(other.string(), fromIndex);
}

int String::LastIndexOfInternal(const char *string, int fromIndex) const
{
    int sourceLen = GetLength();
    int stringLen = strlen(string);
    int rightIndex = sourceLen - stringLen;
    if (fromIndex > rightIndex) {
        fromIndex = rightIndex;
    }

    int stringLastIndex = stringLen - 1;
    char stringLastChar = string[stringLastIndex];
    int leftIndex = stringLen - 1;
    int index = leftIndex + fromIndex;

    while (true) {
        while (index >= leftIndex && string_[index] != stringLastChar) {
            index--;
        }
        if (index < leftIndex) {
            return -1;
        }

        int compareIndex = index - 1;
        int start = compareIndex - (stringLen - 1);
        int strIndex = stringLastIndex - 1;

        while (compareIndex > start) {
            if (string_[compareIndex--] != string[strIndex--]) {
                index--;
                break;
            }
        }

        if (compareIndex == start) {
            return start + 1;
        }
    }
}

bool String::StartsWith(const char *string) const
{
    if (string == nullptr || string_ == nullptr) {
        return false;
    }

    if (string[0] == '\0' && string_[0] == '\0') {
        return true;
    }

    size_t count = strlen(string);
    if (count > (size_t)GetLength()) {
        return false;
    }

    return memcmp(string_, string, count) == 0;
}

bool String::StartsWith(const String &other) const
{
    if (other.string_ == nullptr || string_ == nullptr) {
        return false;
    }

    if (other.string_[0] == '\0' && string_[0] == '\0') {
        return true;
    }

    size_t count = other.GetLength();
    if (count > (size_t)GetLength()) {
        return false;
    }

    return memcmp(string_, other.string_, count) == 0;
}

bool String::EndsWith(char c) const
{
    if (string_ == nullptr) {
        return false;
    }

    size_t len = GetLength();
    if (len < 1) {
        return false;
    }

    return string_[len - 1] == c;
}

bool String::EndsWith(const char *string) const
{
    if (string == nullptr || string_ == nullptr) {
        return false;
    }

    if (string[0] == '\0') {
        return true;
    }

    size_t count = strlen(string);
    size_t len = GetLength();
    if (count > len) {
        return false;
    }

    return memcmp(string_ + len - count, string, count) == 0;
}

bool String::EndsWith(const String &other) const
{
    if (other.string_ == nullptr || string_ == nullptr) {
        return false;
    }

    if (other.string_[0] == '\0') {
        return true;
    }

    size_t count = other.GetLength();
    size_t len = GetLength();
    if (count > len) {
        return false;
    }

    return memcmp(string_ + len - count, other.string_, count) == 0;
}

String String::ToLowerCase() const
{
    if (IsEmpty()) {
        return *this;
    }

    size_t size = GetLength();
    for (size_t i = 0; i < size; i++) {
        if (isupper(string_[i])) {
            String newStr(string_);
            for (size_t j = i; j < size; j++) {
                newStr.string_[j] = tolower(newStr.string_[j]);
            }
            return newStr;
        }
    }
    return *this;
}

String String::ToUpperCase() const
{
    if (IsEmpty()) {
        return *this;
    }

    size_t size = GetLength();
    for (size_t i = 0; i < size; i++) {
        if (islower(string_[i])) {
            String newStr(string_);
            for (size_t j = i; j < size; j++) {
                newStr.string_[j] = toupper(newStr.string_[j]);
            }
            return newStr;
        }
    }
    return *this;
}

String String::Substring(int begin) const
{
    if (begin < 0 || begin >= GetLength()) {
        return String();
    }

    return String(string_ + begin);
}

String String::Substring(int begin, int end) const
{
    if (begin < 0 || end > GetLength() || begin > end) {
        return String();
    }

    return String(string_ + begin, end - begin);
}

String String::Replace(char oldChar, char newChar) const
{
    if (oldChar == newChar) {
        return *this;
    }

    size_t size = GetLength();
    for (size_t i = 0; i < size; i++) {
        if (string_[i] != oldChar) {
            continue;
        }

        String newStr(string_);
        for (size_t j = i; j < size; j++) {
            if (newStr.string_[j] == oldChar) {
                newStr.string_[j] = newChar;
            }
        }
        return newStr;
    }
    return *this;
}

String String::Replace(const char *target, const char *replacement) const
{
    if (target == nullptr || target[0] == '\0' || replacement == nullptr) {
        return *this;
    }

    int index = IndexOf(target);
    if (index == -1) {
        return *this;
    }

    StringBuilder sb;
    int begin = 0;
    int step = strlen(target);
    while (index != -1) {
        sb.Append(Substring(begin, index));
        sb.Append(replacement);
        begin = index + step;
        index = IndexOf(target, begin);
    }
    sb.Append(Substring(begin));
    return sb.ToString();
}

String String::Replace(const String &target, const String &replacement) const
{
    if (target.IsEmpty() || replacement.IsNull()) {
        return *this;
    }

    int index = IndexOf(target);
    if (index == -1) {
        return *this;
    }

    StringBuilder sb;
    int begin = 0;
    int step = target.GetLength();
    while (index != -1) {
        sb.Append(Substring(begin, index));
        sb.Append(replacement);
        begin = index + step;
        index = IndexOf(target, begin);
    }
    sb.Append(Substring(begin));
    return sb.ToString();
}

String &String::Replace(int position, int len, const String &other)
{
    if (position < 0 || position > GetLength()) {
        return *this;
    }

    if (len < 0) {
        return *this;
    }

    if (other.IsEmpty()) {
        return *this;
    }

    String lStr = Substring(0, position);
    String rStr = Substring(position + len);
    String newStr;
    newStr += lStr + other + rStr;

    SharedData::Release(string_);
    SharedData::AddRef(newStr.string_);
    string_ = newStr.string_;
    return *this;
}

std::vector<String> String::Split(const String &separator) const
{
    std::vector<String> result;
    if (IsEmpty()) {
        return result;
    }

    int startIndex = 0;
    int endIndex = IndexOf(separator, startIndex);
    while (endIndex != -1) {
        result.push_back(Substring(startIndex, endIndex));
        startIndex = endIndex + 1;
        endIndex = IndexOf(separator, startIndex);
    }

    if (startIndex < GetLength()) {
        result.push_back(Substring(startIndex));
    }

    return result;
}

String &String::insert(int index, const String &other)
{
    if (index < 0 || index > GetLength()) {
        return *this;
    }

    if (other.IsEmpty()) {
        return *this;
    }

    String newStr;

    String lStr = Substring(0, index);
    String rStr = Substring(index);

    newStr += lStr + other + rStr;

    SharedData::Release(string_);
    SharedData::AddRef(newStr.string_);
    string_ = newStr.string_;
    return *this;
}

String &String::operator=(const char *string)
{
    SharedData::Release(string_);

    if (string == nullptr) {
        string_ = nullptr;
        return *this;
    }

    string_ = SharedData::ToString(SharedData::Allocate(strlen(string)));
    if (string_ == nullptr) {
        return *this;
    }

    if (strcpy_s(string_, strlen(string) + 1, string) != EOK) {
        Logger::E(String::TAG, "The operator= of \"%s\" is failed.", string);
        SharedData::Release(string_);
        string_ = nullptr;
    }

    return *this;
}

String &String::operator=(const String &other)
{
    if (string_ == other.string_) {
        return *this;
    }

    SharedData::Release(string_);
    SharedData::AddRef(other.string_);
    string_ = other.string_;
    return *this;
}

String &String::operator=(String &&other)
{
    SharedData::Release(string_);
    string_ = other.string_;
    other.string_ = nullptr;
    return *this;
}

String &String::operator+=(const char *other)
{
    if (other == nullptr || other[0] == '\0') {
        return *this;
    }

    int thisSize = GetLength();
    int newSize = thisSize + strlen(other);
    String newString(newSize);
    if (newString.string_ == nullptr) {
        Logger::E(String::TAG, "The operator+= of \"%s\" is failed.", string_);
        goto finished;
    }

    if (string_ != nullptr && thisSize > 0) {
        if (memcpy_s(newString.string_, newSize + 1, string_, thisSize) != EOK) {
            Logger::E(String::TAG, "The operator+= of \"%s\" is failed. 2", string_);
            goto finished;
        }
    }

    if (strcpy_s(newString.string_ + thisSize, newSize + 1 - thisSize, other) != EOK) {
        Logger::E(String::TAG, "The operator+= of \"%s\" is failed.", string_);
        goto finished;
    }

finished:
    SharedData::Release(string_);
    SharedData::AddRef(newString.string_);
    string_ = newString.string_;
    return *this;
}

String &String::operator+=(const String &other)
{
    if (other.IsEmpty()) {
        return *this;
    }

    int thisSize = GetLength();
    int newSize = thisSize + other.GetLength();
    String newString(newSize);
    if (newString.string_ == nullptr) {
        Logger::E(String::TAG, "The operator+= of \"%s\" is failed. 1", string_);
        goto finished;
    }

    if (string_ != nullptr && thisSize > 0) {
        if (memcpy_s(newString.string_, newSize + 1, string_, thisSize) != EOK) {
            Logger::E(String::TAG, "The operator+= of \"%s\" is failed. 2", string_);
            goto finished;
        }
    }

    if (strcpy_s(newString.string_ + thisSize, newSize + 1 - thisSize, other.string_) != EOK) {
        Logger::E(String::TAG, "The operator+= of \"%s\" is failed. 3", string_);
        goto finished;
    }

finished:
    SharedData::Release(string_);
    SharedData::AddRef(newString.string_);
    string_ = newString.string_;
    return *this;
}

String String::Format(const char *format, ...)
{
    va_list args, argsCopy;

    va_start(args, format);
    va_copy(argsCopy, args);

    char buf[LINE_MAX_SIZE] = {0};
    int len = vsnprintf_s(buf, LINE_MAX_SIZE, LINE_MAX_SIZE - 1, format, args);
    String string;
    if (len <= 0) {
        va_end(args);
        va_end(argsCopy);
        return string;
    }

    string = String(len);
    if (string.string_ == nullptr) {
        va_end(args);
        va_end(argsCopy);
        return string;
    }

    if (vsnprintf_s(string.string_, len + 1, len, format, argsCopy) < 0) {
        va_end(args);
        va_end(argsCopy);
        return string;
    }

    va_end(args);
    va_end(argsCopy);
    return string;
}
} // namespace HDI
} // namespace OHOS