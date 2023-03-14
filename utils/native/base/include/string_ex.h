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
#ifndef STRING_EX_H
#define STRING_EX_H

#include <string>
#include <vector>

namespace OHOS {

/**
 * The UpperStr function convert all letters of str to uppercase.
 */
std::string UpperStr(const std::string& str);

/**
 * The LowerStr function convert all letters of str to lowercase.
 */
std::string LowerStr(const std::string& str);

/**
 * The ReplaceStr function will replace src with dst int base.
 */
std::string ReplaceStr(const std::string& str, const std::string& src, const std::string& dst);

/**
 * The TrimStr function will trim str by cTrim front and end.
 */
std::string TrimStr(const std::string& str, const char cTrim = ' ');

/**
 * The DexToHexString function convert decimal to hexadecimal string.
 */
std::string DexToHexString(int value, bool upper = true);

/**
 * The SplitStr function will split str by strSep.
 */
void SplitStr(const std::string& str, const std::string& sep, std::vector<std::string>& strs,
              bool canEmpty = false, bool needTrim = true);

/**
 * The ToString function convert int and double and so on to str.
 */
template<class T>
inline std::string ToString(T iValue)
{
    return std::to_string(iValue);
}

/**
 * The StrToInt function convert str to int.
 */
bool StrToInt(const std::string& str, int& value);

/**
 * The IsNumericStr function judge all characters of the string are numbers,
 * return true if all are numbers, else false.
 */
bool IsNumericStr(const std::string& str);

/**
 * The IsAlphaStr function judge all characters of the string are alphabet,
 * return true if all are alphabet, else false.
 */
bool IsAlphaStr(const std::string& str);

/**
 * The IsUpperStr function judge all characters of the string are uppercase,
 * return true if all are uppercase, else false.
 */
bool IsUpperStr(const std::string& str);

/**
 * The IsLowerStr function judge all characters of the string are lowercase,
 * return true if all are lowercase, else false.
 */
bool IsLowerStr(const std::string& str);

/**
 * The IsSubStr function judge the sub in str,
 * return true if sub in str, else false.
 */
bool IsSubStr(const std::string& str, const std::string& sub);

/**
 * The GetFirstSubStrBetween function get the first sub_str between left and right
 * return the rightstr pos, if failed return string::npos.
 */
std::string::size_type GetFirstSubStrBetween(const std::string& str, const std::string& left,
                                             const std::string& right, std::string& sub);

/**
 * The GetSubStrBetween function get the sub strings between left and right.
 */
void GetSubStrBetween(const std::string& str, const std::string& left,
                      const std::string& right, std::vector<std::string>& sub);

/**
 * The IsSameTextStr function judge the first's letter is same with second,
 * return true if same, else false.
 */
bool IsSameTextStr(const std::string& first, const std::string& second);

bool IsAsciiString(const std::string& str);

/**
 * The str16ToStr8 function convert string16 to string8.
 * If convert failed, return an empty string.
 */
std::string Str16ToStr8(const std::u16string& str16);

/**
 * The Str8ToStr16 function convert string8 to string16.
 * If convert failed, return an empty u16string.
 */
std::u16string Str8ToStr16(const std::string& str);
} // namespace OHOS

#endif // STRING_EX_H
