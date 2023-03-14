/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_STRING_H
#define OHOS_HDI_STRING_H

#include <vector>

namespace OHOS {
namespace HDI {
class String {
public:
    inline String() {}

    String(const char *string);

    String(const char *string, size_t length);

    String(const String &other);

    String(String &&other);

    ~String();

    inline const char *string() const
    {
        return string_;
    }

    inline operator const char *() const
    {
        return string_;
    }

    inline bool IsNull() const
    {
        return string_ == nullptr;
    }

    inline bool IsEmpty() const
    {
        return string_ == nullptr || string_[0] == '\0';
    }

    int GetLength() const;

    char operator[](int index) const;

    bool Equals(const char *string) const;

    bool Equals(const String &other) const;

    int Compare(const String &other) const;

    int GetHashCode() const;

    int IndexOf(char c, int fromIndex = 0) const;

    int IndexOf(const char *string, int fromIndex = 0) const;

    int IndexOf(const String &other, int fromIndex = 0) const;

    int LastIndexOf(char c, int fromIndex = 0) const;

    int LastIndexOf(const char *string, int fromIndex = 0) const;

    int LastIndexOf(const String &other, int fromIndex = 0) const;

    bool StartsWith(const char *string) const;

    bool StartsWith(const String &other) const;

    bool EndsWith(char c) const;

    bool EndsWith(const char *string) const;

    bool EndsWith(const String &other) const;

    String ToLowerCase() const;

    String ToUpperCase() const;

    String Substring(int begin) const;

    String Substring(int begin, int end) const;

    String Replace(char oldChar, char newChar) const;

    String Replace(const char *target, const char *replacement) const;

    String Replace(const String &target, const String &replacement) const;

    String &Replace(int position, int len, const String &other);

    std::vector<String> Split(const String &separator) const;

    String &insert(int index, const String &other);

    String &operator=(const char *string);

    String &operator=(const String &other);

    String &operator=(String &&other);

    String &operator+=(const char *string);

    String &operator+=(const String &other);

    static String Format(const char *format, ...);

    static const char *TAG;
    static constexpr int MAX_SIZE = 262144; // 2^18

private:
    explicit String(int size);

    int LastIndexOfInternal(const char *string, int fromIndex) const;

    char *string_ = nullptr;
};

inline String operator+(const String &string1, const String &string2)
{
    String newStr;
    newStr += string1;
    newStr += string2;
    return newStr;
}

struct StringHashFunc {
    int operator()(const String &key) const
    {
        return key.GetHashCode();
    }
};

struct StringEqualFunc {
    bool operator()(const String &lhs, const String &rhs) const
    {
        return lhs.Equals(rhs);
    }
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_STRING_H