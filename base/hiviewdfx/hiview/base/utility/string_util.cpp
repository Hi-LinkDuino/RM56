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
#include "string_util.h"
#include <climits>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace OHOS {
namespace HiviewDFX {
namespace StringUtil {
using namespace std;
const char INDICATE_VALUE_CHAR = ':';
const char KEY_VALUE_END_CHAR = ';';
constexpr int SKIP_NEXT_INDEX_LENGTH = 2;
const std::string EMPTY_STRING = "";
std::string ConvertVectorToStr(const std::vector<std::string> &listStr, const std::string &split)
{
    std::string str("");
    for (auto &item : listStr) {
        if (str == "") {
            str = item;
        } else {
            str += split + item;
        }
    }
    return str;
}

string ReplaceStr(const string& str, const string& src, const string& dst)
{
    if (src.empty()) {
        return str;
    }

    string::size_type pos = 0;
    string strTmp = str;
    while ((pos = strTmp.find(src, pos)) != string::npos) {
        strTmp.replace(pos, src.length(), dst);
        pos += dst.length();
    }

    return strTmp;
}

string TrimStr(const string& str, const char cTrim)
{
    string strTmp = str;
    strTmp.erase(0, strTmp.find_first_not_of(cTrim));
    strTmp.erase(strTmp.find_last_not_of(cTrim) + sizeof(char));
    return strTmp;
}

void SplitStr(const string& str, const string& sep, vector<string>& strs,
              bool canEmpty, bool needTrim)
{
    strs.clear();
    string strTmp = needTrim ? TrimStr(str) : str;
    string strPart;
    while (true) {
        string::size_type pos = strTmp.find(sep);
        if (string::npos == pos || sep.empty()) {
            strPart = needTrim ? TrimStr(strTmp) : strTmp;
            if (!strPart.empty() || canEmpty) {
                strs.push_back(strPart);
            }
            break;
        } else {
            strPart = needTrim ? TrimStr(strTmp.substr(0, pos)) : strTmp.substr(0, pos);
            if (!strPart.empty() || canEmpty) {
                strs.push_back(strPart);
            }
            strTmp = strTmp.substr(sep.size() + pos, strTmp.size() - sep.size() - pos);
        }
    }
}

bool StrToInt(const string& str, int& value)
{
    if (str.empty() || (!isdigit(str.front()) && (str.front() != '-'))) {
        return false;
    }

    char* end = nullptr;
    const int base = 10;
    errno = 0;
    auto addr = str.c_str();
    auto result = strtol(addr, &end, base);
    if (end == addr || end[0] != '\0' || errno == ERANGE) {
        return false;
    }

    value = static_cast<int>(result);
    return true;
}

int StrToInt(const string& str)
{
    int id = -1;
    StrToInt(str, id);
    return id;
}

string DexToHexString(int value, bool upper)
{
    stringstream ioss;
    string hexString;
    if (upper) {
        ioss << setiosflags(ios::uppercase) << hex << value;
    } else {
        ioss << hex << value;
    }

    ioss >> hexString;
    return hexString;
}

KeyValuePair GetKeyValueByString(int &start, const std::string &inputString)
{
    std::string key;
    std::string value;
    auto length = inputString.size();
    while (start < length) {
        if (inputString[start] == INDICATE_VALUE_CHAR) {
            start++;
            break;
        }
        key.append(1, inputString[start]);
        start++;
    }

    while (start < length) {
        if (inputString[start] == KEY_VALUE_END_CHAR) {
            // replace ;; to ; in value;
            // one ';' means the end of a "key:value;"
            if (start + 1 < length && inputString[start + 1] == KEY_VALUE_END_CHAR) {
                value.append(1, inputString[start]);
                start = start + SKIP_NEXT_INDEX_LENGTH;
                continue;
            } else {
                start++;
                break;
            }
        }
        if (inputString[start] == INDICATE_VALUE_CHAR && start + 1 < length &&
            inputString[start + 1] == INDICATE_VALUE_CHAR) {
            // replace :: to :
            value.append(1, inputString[start]);
            start = start + SKIP_NEXT_INDEX_LENGTH;
            continue;
        }
        value.append(1, inputString[start]);
        start++;
    }
    return make_pair(key, make_pair(value, 0));
}

bool IsValidFloatNum(const std::string &value)
{
    int len = value.size();
    int pointNum = 0;

    for (int i = 0; i < len; i++) {
        if (isdigit(value[i])) {
            continue;
        }
        if (value[i] == '.') {
            pointNum++;
        } else {
            // the string contains not valid num;
            return false;
        }
    }
    // the string contains more than one character '.'
    if (pointNum > 1) {
        return false;
    }
    // the string format is .111/111.
    return isdigit(value[0]) && isdigit(value[len - 1]);
}

std::list<std::string> SplitStr(const std::string& str, char delimiter)
{
    std::list<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

string GetLeftSubstr(const string& input, const string& split)
{
    size_t pos = input.find(split, 0);
    if (pos == string::npos) {
        return input;
    }
    return input.substr(0, pos);
}

string GetRightSubstr(const string& input, const string& split)
{
    size_t pos = input.find(split, 0);
    if (pos == string::npos) {
        return "none";
    }
    return input.substr(pos + split.size(), input.size() - pos);
}

string GetRleftSubstr(const string& input, const string& split)
{
    size_t pos = input.rfind(split, string::npos);
    if (pos == string::npos) {
        return input;
    }
    return input.substr(0, pos);
}

string GetRrightSubstr(const string& input, const string& split)
{
    size_t pos = input.rfind(split, string::npos);
    if (pos == string::npos) {
        return "none";
    }
    return input.substr(pos + 1, input.size() - pos);
}

string EraseString(const string& input, const string& toErase)
{
    size_t pos = 0;
    string out = input;
    while ((pos = out.find(toErase, pos)) != string::npos) {
        out.erase(pos, toErase.size());
        pos = (pos == 0) ? (0) : (pos - 1);
    }
    return out;
}

string GetMidSubstr(const string& input, const string& begin, const string& end)
{
    string midstr = "";
    size_t beginPos = input.find(begin, 0);
    if (beginPos == string::npos) {
        return midstr;
    }
    beginPos = beginPos + begin.size();
    size_t endPos = input.find(end, beginPos);
    if (endPos == string::npos) {
        return midstr;
    }
    return input.substr(beginPos, endPos - beginPos);
}

string VectorToString(const vector<string>& src, bool reverse, const string& tag)
{
    string str;
    for (auto elment : src) {
        if (elment.empty()) {
            continue;
        }
        if (reverse) {
            str = elment + tag + str;
        } else {
            str = str + elment + tag;
        }
    }
    return str;
}

uint64_t StringToUl(const string& flag, int base)
{
    uint64_t ret = strtoul(flag.c_str(), NULL, base);
    if (ret == ULONG_MAX) {
        return 0;
    }
    return ret;
}

double StringToDouble(const string& input)
{
    if (input.empty()) {
        return 0;
    }
    char *e = nullptr;
    errno = 0;
    double temp = std::strtod(input.c_str(), &e);

    if (errno != 0) { // error, overflow or underflow
        return 0;
    }
    return temp;
}

std::string FindMatchSubString(const std::string& target, const std::string& begin, int offset,
    const std::string& end)
{
    auto matchPos = target.find_first_of(begin);
    if (matchPos == std::string::npos) {
        return "";
    }
    auto beginPos = matchPos + offset;
    if (beginPos > target.length()) {
        return "";
    }
    auto endPos = target.find_first_of(end, beginPos);
    if (endPos == std::string::npos) {
        return target.substr(beginPos);
    }
    return target.substr(beginPos, endPos);
}

std::string EscapeJsonStringValue(const std::string &value)
{
    std::string escapeValue;
    for (auto it = value.begin(); it != value.end(); it++) {
        switch (*it) {
            case '\\':
                escapeValue.push_back('\\');
                escapeValue.push_back('\\');
                break;
            case '\"':
                escapeValue.push_back('\\');
                escapeValue.push_back('"');
                break;
            case '\b':
                escapeValue.push_back('\\');
                escapeValue.push_back('b');
                break;
            case '\f':
                escapeValue.push_back('\\');
                escapeValue.push_back('f');
                break;
            case '\n':
                escapeValue.push_back('\\');
                escapeValue.push_back('n');
                break;
            case '\r':
                escapeValue.push_back('\\');
                escapeValue.push_back('r');
                break;
            case '\t':
                escapeValue.push_back('\\');
                escapeValue.push_back('t');
                break;
            default:
                escapeValue.push_back(*it);
                break;
        }
    }
    return escapeValue;
}
} // namespace StringUtil
} // namespace HiviewDFX
} // namespace OHOS
