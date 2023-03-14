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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_STRING_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_STRING_UTILS_H

#include <cmath>
#include <codecvt>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

#include "base/geometry/dimension.h"
#include "base/geometry/calc_dimension.h"
#include "base/utils/linear_map.h"
#include "base/utils/utils.h"

namespace OHOS::Ace::StringUtils {

ACE_EXPORT extern const char DEFAULT_STRING[];
ACE_EXPORT extern const std::wstring DEFAULT_WSTRING;
ACE_EXPORT extern const std::u16string DEFAULT_USTRING;
constexpr int32_t TEXT_CASE_LOWERCASR = 1;
constexpr int32_t TEXT_CASE_UPPERCASR = 2;

inline std::u16string Str8ToStr16(const std::string& str)
{
    if (str == DEFAULT_STRING) {
        return DEFAULT_USTRING;
    }
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert(DEFAULT_STRING, DEFAULT_USTRING);
    std::u16string result = convert.from_bytes(str);
    return result == DEFAULT_USTRING ? u"" : result;
}

inline std::string Str16ToStr8(const std::u16string& str)
{
    if (str == DEFAULT_USTRING) {
        return DEFAULT_STRING;
    }
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert(DEFAULT_STRING);
    std::string result = convert.to_bytes(str);
    return result == DEFAULT_STRING ? "" : result;
}

inline std::wstring ToWstring(const std::string& str)
{
    if (str == DEFAULT_STRING) {
        return DEFAULT_WSTRING;
    }
#ifdef WINDOWS_PLATFORM
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert(DEFAULT_STRING, DEFAULT_WSTRING);
#else
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert(DEFAULT_STRING, DEFAULT_WSTRING);
#endif
    std::wstring result = convert.from_bytes(str);
    return result == DEFAULT_WSTRING ? L"" : result;
}

inline std::string ToString(const std::wstring& str)
{
    if (str == DEFAULT_WSTRING) {
        return DEFAULT_STRING;
    }
#ifdef WINDOWS_PLATFORM
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert(DEFAULT_STRING);
#else
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert(DEFAULT_STRING);
#endif
    std::string result = convert.to_bytes(str);
    return result == DEFAULT_STRING ? "" : result;
}

inline bool NotInUtf16Bmp(char16_t c)
{
    return (c & 0xF800) == 0xD800;
}

inline bool NotInBmp(wchar_t ch)
{
    return ch >= 0xD800 && ch <= 0xDBFF;
}

inline bool IsNumber(const std::string& value)
{
    if (value.empty()) {
        return false;
    }
    return std::all_of(value.begin(), value.end(), [](char i) { return isdigit(i); });
}

inline void ReplaceSpace(std::string& data)
{
    bool isFirstSpace = true;
    auto iter = data.begin();
    while (iter != data.end()) {
        if (*iter == ' ') {
            if (isFirstSpace) {
                iter++;
                isFirstSpace = false;
            } else {
                iter = data.erase(iter);
            }
        } else if (*iter == '\t') {
            *iter = ' ';
        } else {
            isFirstSpace = true;
            iter++;
        }
    }
}

inline void ReplaceTabAndNewLine(std::string& data)
{
    for (auto& i : data) {
        if (i == '\r' || i == '\n') {
            i = ' ';
        }
    }
    ReplaceSpace(data);
}

inline int32_t StringToInt(const std::string& value)
{
    errno = 0;
    char* pEnd = nullptr;
    int64_t result = std::strtol(value.c_str(), &pEnd, 10);
    if (pEnd == value.c_str() || (result < INT_MIN || result > INT_MAX) || errno == ERANGE) {
        return 0;
    } else {
        return result;
    }
}

inline int64_t StringToLongInt(const std::string& value)
{
    errno = 0;
    char* pEnd = nullptr;
    int64_t result = std::strtoll(value.c_str(), &pEnd, 10);
    if (pEnd == value.c_str() || errno == ERANGE) {
        return 0;
    } else {
        return result;
    }
}

inline uint32_t StringToUint(const std::string& value, uint32_t defaultErr = 0)
{
    errno = 0;
    char* pEnd = nullptr;
    uint64_t result = std::strtoul(value.c_str(), &pEnd, 10);
    if (pEnd == value.c_str() || result > UINT32_MAX || errno == ERANGE) {
        return defaultErr;
    } else {
        return result;
    }
}

inline double StringToDouble(const std::string& value)
{
    char* pEnd = nullptr;
    double result = std::strtod(value.c_str(), &pEnd);
    if (pEnd == value.c_str() || errno == ERANGE) {
        return 0.0;
    } else {
        return result;
    }
}

inline float StringToFloat(const std::string& value)
{
    char* pEnd = nullptr;
    float result = std::strtof(value.c_str(), &pEnd);
    if (pEnd == value.c_str() || errno == ERANGE) {
        return 0.0f;
    } else {
        return result;
    }
}

inline Dimension StringToDimensionWithUnit(const std::string& value, DimensionUnit defaultUnit = DimensionUnit::PX)
{
    errno = 0;
    if (std::strcmp(value.c_str(), "auto") == 0) {
        return Dimension(0, DimensionUnit::AUTO);
    }
    char* pEnd = nullptr;
    double result = std::strtod(value.c_str(), &pEnd);
    if (pEnd == value.c_str() || errno == ERANGE) {
        return Dimension(0.0, defaultUnit);
    } else if (pEnd != nullptr) {
        if (std::strcmp(pEnd, "%") == 0) {
            // Parse percent, transfer from [0, 100] to [0, 1]
            return Dimension(result / 100.0, DimensionUnit::PERCENT);
        } else if (std::strcmp(pEnd, "px") == 0) {
            return Dimension(result, DimensionUnit::PX);
        } else if (std::strcmp(pEnd, "vp") == 0) {
            return Dimension(result, DimensionUnit::VP);
        } else if (std::strcmp(pEnd, "fp") == 0) {
            return Dimension(result, DimensionUnit::FP);
        } else if ((pEnd) && (std::strcmp(pEnd, "lpx") == 0)) {
            return Dimension(result, DimensionUnit::LPX);
        }
    }
    return Dimension(result, defaultUnit);
}

inline CalcDimension StringToCalcDimension(const std::string& value, bool useVp = false)
{
    if (value.find("calc") != std::string::npos) {
        return CalcDimension(value, DimensionUnit::CALC);
    } else {
        return StringToDimensionWithUnit(value, useVp ? DimensionUnit::VP : DimensionUnit::PX);
    }
}


inline Dimension StringToDimension(const std::string& value, bool useVp = false)
{
    return StringToDimensionWithUnit(value, useVp ? DimensionUnit::VP : DimensionUnit::PX);
}

inline double StringToDegree(const std::string& value)
{
    // https://developer.mozilla.org/zh-CN/docs/Web/CSS/angle
    constexpr static double DEGREES = 360.0;
    constexpr static double GRADIANS = 400.0;
    constexpr static double RADIUS = 2 * M_PI;

    errno = 0;
    char* pEnd = nullptr;
    double result = std::strtod(value.c_str(), &pEnd);
    if (pEnd == value.c_str() || errno == ERANGE) {
        return 0.0;
    } else if (pEnd) {
        if ((std::strcmp(pEnd, "deg")) == 0) {
            return result;
        } else if (std::strcmp(pEnd, "grad") == 0) {
            return result / GRADIANS * DEGREES;
        } else if (std::strcmp(pEnd, "rad") == 0) {
            return result / RADIUS * DEGREES;
        } else if (std::strcmp(pEnd, "turn") == 0) {
            return result * DEGREES;
        }
    }
    return StringToDouble(value);
}

template<class T>
inline void StringSpliter(const std::string& source, char delimiter, T (*func)(const std::string&), std::vector<T>& out)
{
    out.erase(out.begin(), out.end());

    if (source.empty()) {
        return;
    }

    std::size_t startIndex = 0;
    for (std::size_t index = 0; index < source.size(); index++) {
        if (source[index] != delimiter) {
            continue;
        }

        if (index > startIndex) {
            out.emplace_back(func(source.substr(startIndex, index - startIndex)));
        }
        startIndex = index + 1;
    }

    if (startIndex < source.size()) {
        out.emplace_back(func(source.substr(startIndex)));
    }
}

inline void StringSpliter(const std::string& source, char delimiter, std::vector<std::string>& out)
{
    using Func = std::string (*)(const std::string&);
    Func func = [](const std::string& value) { return value; };
    StringSpliter(source, delimiter, func, out);
}

inline void StringSpliter(const std::string& source, char delimiter, std::vector<double>& out)
{
    using Func = double (*)(const std::string&);
    Func func = [](const std::string& value) { return StringToDouble(value); };
    StringSpliter(source, delimiter, func, out);
}

inline void StringSpliter(const std::string& source, char delimiter, std::vector<float>& out)
{
    using Func = float (*)(const std::string&);
    Func func = [](const std::string& value) { return StringToFloat(value); };
    StringSpliter(source, delimiter, func, out);
}

inline void StringSpliter(const std::string& source, char delimiter, std::vector<int>& out)
{
    using Func = int32_t (*)(const std::string&);
    Func func = [](const std::string& value) { return StringToInt(value); };
    StringSpliter(source, delimiter, func, out);
}

inline void StringSpliter(const std::string& source, char delimiter, std::vector<Dimension>& out)
{
    using Func = Dimension (*)(const std::string&);
    Func func = [](const std::string& value) { return StringToDimension(value); };
    StringSpliter(source, delimiter, func, out);
}

inline std::string DoubleToString(double value, int32_t precision = 2)
{
    std::ostringstream result;
    result.precision(precision);
    result << std::fixed << value;
    return result.str();
}

inline void DeleteAllMark(std::string& str, const char mark)
{
    str.erase(std::remove(str.begin(), str.end(), mark), str.end());
}

inline std::string TrimStr(const std::string& str, char cTrim = ' ')
{
    auto firstPos = str.find_first_not_of(cTrim);
    if (firstPos == std::string::npos) {
        return str;
    }
    auto endPos = str.find_last_not_of(cTrim);
    return str.substr(firstPos, endPos - firstPos + 1);
}

inline void TrimStrLeadingAndTrailing(std::string& str, char cTrim = ' ')
{
    auto firstIndexNotOfSpace = str.find_first_not_of(" ");
    if (firstIndexNotOfSpace == std::string::npos) {
        str = "";
        return;
    }
    str.erase(0, firstIndexNotOfSpace);
    auto lastIndexNotOfSpace = str.find_last_not_of(" ");
    if (lastIndexNotOfSpace == std::string::npos) {
        str = "";
        return;
    }
    str.erase(lastIndexNotOfSpace + 1);
}

inline void SplitStr(
    const std::string& str, const std::string& sep, std::vector<std::string>& out, bool needTrim = true)
{
    out.erase(out.begin(), out.end());

    if (str.empty() || sep.empty()) {
        return;
    }

    std::string strPart;
    std::string::size_type startPos = 0;
    std::string::size_type pos = str.find_first_of(sep, startPos);
    while (pos != std::string::npos) {
        if (pos > startPos) {
            strPart = needTrim ? TrimStr(str.substr(startPos, pos - startPos)) : str.substr(startPos, pos - startPos);
            out.emplace_back(std::move(strPart));
        }
        startPos = pos + sep.size();
        pos = str.find_first_of(sep, startPos);
    }

    if (startPos < str.size()) {
        strPart = needTrim ? TrimStr(str.substr(startPos)) : str.substr(startPos);
        out.emplace_back(std::move(strPart));
    }
}

inline void SplitStr(const std::string& str, const std::string& sep, std::vector<Dimension>& out, bool needTrim = true)
{
    out.erase(out.begin(), out.end());
    if (str.empty() || sep.empty()) {
        return;
    }
    std::string strPart;
    std::string::size_type startPos = 0;
    std::string::size_type pos = str.find_first_of(sep, startPos);
    while (pos != std::string::npos) {
        if (pos > startPos) {
            strPart = needTrim ? TrimStr(str.substr(startPos, pos - startPos)) : str.substr(startPos, pos - startPos);
            if (!strPart.empty()) {
                out.emplace_back(StringToDimension(std::move(strPart)));
            }
        }
        startPos = pos + sep.size();
        pos = str.find_first_of(sep, startPos);
    }
    if (startPos < str.size()) {
        strPart = needTrim ? TrimStr(str.substr(startPos)) : str.substr(startPos);
        if (!strPart.empty()) {
            out.emplace_back(StringToDimension(std::move(strPart)));
        }
    }
}

const std::string ACE_EXPORT FormatString(const char* fmt, ...);

inline bool StartWith(const std::string& dst, const std::string& prefix)
{
    return dst.compare(0, prefix.size(), prefix) == 0;
}

inline bool EndWith(const std::string& dst, const std::string& suffix)
{
    return (dst.size() >= suffix.size()) && dst.compare(dst.size() - suffix.size(), suffix.size(), suffix) == 0;
}

inline void TransfromStrCase(std::string& str, int32_t textCase)
{
    if (str.empty()) {
        return;
    }

    switch (textCase) {
        case TEXT_CASE_LOWERCASR:
            transform(str.begin(), str.end(), str.begin(), ::tolower);
            break;
        case TEXT_CASE_UPPERCASR:
            transform(str.begin(), str.end(), str.begin(), ::toupper);
            break;
        default:
            break;
    }
}

} // namespace OHOS::Ace::StringUtils

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_STRING_UTILS_H
